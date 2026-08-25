#include "PjsipEngine.h"
#include <pjsua2.hpp>
#include <QMetaObject>
#include <QRegularExpression>
#include <QFile>
#include <QDir>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>

using namespace pj;

namespace {
QString diagnosticLogPath() {
    const QString directory = QDir(QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation)).filePath(QStringLiteral("logs"));
    QDir().mkpath(directory);
    return QDir(directory).filePath(QStringLiteral("cgphone-sip.log"));
}
}

static QString sipDialTarget(const std::string &remoteUri) {
    const QString raw = QString::fromStdString(remoteUri).trimmed();
    const auto match = QRegularExpression(QStringLiteral("sip:([^@;>]+)"),
                                           QRegularExpression::CaseInsensitiveOption).match(raw);
    return match.hasMatch() ? match.captured(1).trimmed() : QString();
}

static QString friendlySipPeer(const std::string &remoteUri) {
    const QString raw = QString::fromStdString(remoteUri).trimmed();
    const auto displayMatch = QRegularExpression(QStringLiteral("^\\s*\\\"([^\\\"]+)\\\"")).match(raw);
    if (displayMatch.hasMatch()) {
        const QString display = displayMatch.captured(1).trimmed();
        if (!display.isEmpty() && display.compare(QStringLiteral("Unknown"), Qt::CaseInsensitive) != 0)
            return display;
    }
    const auto userMatch = QRegularExpression(QStringLiteral("sip:([^@;>]+)"), QRegularExpression::CaseInsensitiveOption).match(raw);
    if (userMatch.hasMatch() && !userMatch.captured(1).trimmed().isEmpty())
        return userMatch.captured(1).trimmed();
    return QStringLiteral("Asterisk");
}

class PjsipEngine::CallImpl final : public Call {
public:
    CallImpl(Account &account, PjsipEngine *owner, int id = PJSUA_INVALID_ID) : Call(account, id), owner(owner) {}
    void connectActiveAudio() {
        try {
            const auto ci = getInfo();
            for (unsigned i = 0; i < ci.media.size(); ++i) {
                if (ci.media[i].type == PJMEDIA_TYPE_AUDIO && ci.media[i].status == PJSUA_CALL_MEDIA_ACTIVE) {
                    auto audio = getAudioMedia(i);
                    auto &adm = Endpoint::instance().audDevManager();
                    // La central tiene prioridad: conectar cualquier RTP remoto
                    // activo al headset, incluso durante 183/early media.
                    audio.startTransmit(adm.getPlaybackDevMedia());
                    // No abrir el micrófono del agente antes del 200 OK.
                    if (ci.state == PJSIP_INV_STATE_CONFIRMED)
                        adm.getCaptureDevMedia().startTransmit(audio);
                }
            }
        } catch (const Error &e) { owner->reportError(QString::fromStdString(e.info())); }
    }
    void onCallState(OnCallStateParam &) override {
        try {
            if (getInfo().state == PJSIP_INV_STATE_CONFIRMED)
                connectActiveAudio();
        } catch (const Error &e) { owner->reportError(QString::fromStdString(e.info())); }
        owner->reportCallState(this);
    }
    void onCallMediaState(OnCallMediaStateParam &) override {
        connectActiveAudio();
        try {
            const auto info = getInfo();
            bool locallyHeld = false;
            for (const auto &media : info.media)
                if (media.type == PJMEDIA_TYPE_AUDIO && media.status == PJSUA_CALL_MEDIA_LOCAL_HOLD)
                    locallyHeld = true;
            QMetaObject::invokeMethod(owner, [owner=owner,locallyHeld]{
                emit owner->holdStateChanged(locallyHeld);
            }, Qt::QueuedConnection);
            if (info.state == PJSIP_INV_STATE_EARLY)
                owner->reportEarlyMedia(this);
        } catch (const Error &e) { owner->reportError(QString::fromStdString(e.info())); }
    }
    PjsipEngine *owner;
};

class PjsipEngine::AccountImpl final : public Account {
public:
    explicit AccountImpl(PjsipEngine *owner) : owner(owner) {}
    void onIncomingCall(OnIncomingCallParam &p) override {
        // El callId sólo es seguro mientras PJSIP conserva la sesión. Crear el
        // wrapper inmediatamente evita perder INVITEs al demorarlos en Qt.
        owner->incoming(p.callId);
    }
    void onRegState(OnRegStateParam &) override {
        try { const auto ai=getInfo(); owner->reportRegistration(ai.regIsActive, ai.regStatus, ai.regStatusText); }
        catch (const Error &e) { owner->reportError(QString::fromStdString(e.info())); }
    }
    PjsipEngine *owner;
};

PjsipEngine::PjsipEngine(QObject *parent) : ISipEngine(parent), m_endpoint(std::make_unique<Endpoint>()) {}
PjsipEngine::~PjsipEngine() {
    if (m_localAudioMonitor) setLocalAudioMonitor(false);
    m_call.reset(); m_account.reset();
    if (m_initialized) { try { m_endpoint->libDestroy(); } catch (...) {} }
}

void PjsipEngine::initializeEndpoint() {
    if (m_initialized) return;
    m_endpoint->libCreate();
    EpConfig ep;
    ep.uaConfig.userAgent = "CgPhone/0.3.0";
    ep.uaConfig.threadCnt = 1;
    ep.logConfig.level = 4;
    ep.logConfig.consoleLevel = 0;
    // PJSIP escribe directamente al archivo que consume DiagnosticWindow.
    // Evita depender de stdout/stderr o de callbacks entre hilos.
    ep.logConfig.filename = diagnosticLogPath().toStdString();
    ep.logConfig.consoleLevel = 0;
    m_endpoint->libInit(ep);
    TransportConfig udp; udp.port = 0;
    m_endpoint->transportCreate(PJSIP_TRANSPORT_UDP, udp);
    m_endpoint->libStart(); m_initialized = true;
}

void PjsipEngine::configure(const SipAccountConfig &config) { m_config = config; }

void PjsipEngine::registerAccount() {
    try {
        initializeEndpoint();
        const auto codecs = m_endpoint->codecEnum2();
        for (const auto &codec : codecs) m_endpoint->codecSetPriority(codec.codecId, 0);
        unsigned priority = 255;
        for (const auto &wanted : m_config.enabledCodecs) {
            for (const auto &codec : codecs) {
                if (QString::fromStdString(codec.codecId).compare(wanted, Qt::CaseInsensitive) == 0) {
                    m_endpoint->codecSetPriority(codec.codecId, priority--);
                    break;
                }
            }
        }
        m_call.reset(); m_account = std::make_unique<AccountImpl>(this);
        AccountConfig ac;
        const auto server = m_config.server.trimmed();
        ac.idUri = QString("sip:%1@%2").arg(m_config.user, server).toStdString();
        ac.regConfig.registrarUri = QString("sip:%1").arg(server).toStdString();
        ac.regConfig.retryIntervalSec = 30;
        ac.regConfig.firstRetryIntervalSec = 5;
        ac.natConfig.contactRewriteUse = 1;
        ac.natConfig.viaRewriteUse = 1;
        ac.natConfig.sdpNatRewriteUse = 1;
        ac.sipConfig.authCreds.emplace_back("digest", "*", m_config.user.toStdString(), 0, m_config.password.toStdString());
        if (m_config.proxyEnabled && !m_config.proxy.trimmed().isEmpty())
            ac.sipConfig.proxies.push_back(QString("sip:%1;lr").arg(m_config.proxy.trimmed()).toStdString());
        m_account->create(ac);
    } catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

void PjsipEngine::makeCall(const QString &destination) {
    if (!m_account) { reportError(tr("La cuenta SIP no está registrada")); return; }
    if (destination.trimmed().isEmpty()) { reportError(tr("Ingresá un número")); return; }
    if (m_call) { reportError(tr("Todavía hay una llamada en curso")); return; }
    try {
        m_call = std::make_unique<CallImpl>(*m_account, this);
        const QString uri = destination.startsWith("sip:") ? destination : QString("sip:%1@%2").arg(destination, m_config.server);
        CallOpParam op(true); m_call->makeCall(uri.toStdString(), op);
    } catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

void PjsipEngine::incoming(int callId) {
    if (m_call) {
        try { CallImpl busy(*m_account, this, callId); CallOpParam op; op.statusCode = PJSIP_SC_BUSY_HERE; busy.answer(op); }
        catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
        return;
    }
    m_call = std::make_unique<CallImpl>(*m_account, this, callId);
    QString peer;
    try { peer = friendlySipPeer(m_call->getInfo().remoteUri); } catch (...) {}
    QString target;
    try { target = sipDialTarget(m_call->getInfo().remoteUri); } catch (...) {}
    emit callStateChanged(CallState::Incoming, peer, target);
    if (m_dnd) {
        CallOpParam op; op.statusCode = PJSIP_SC_BUSY_HERE; m_call->answer(op); return;
    }
    if (m_autoAnswer) {
        // Sin demora artificial ni ringtone local: la central controla el
        // anuncio de campaña/cola y el momento del bridge con el cliente.
        answer();
        return;
    }
}

void PjsipEngine::answer() { if (!m_call) return; try { CallOpParam op; op.statusCode=PJSIP_SC_OK; m_call->answer(op); } catch (const Error &e) { reportError(QString::fromStdString(e.info())); } }
void PjsipEngine::hangup() { if (!m_call) return; try { CallOpParam op; op.statusCode=PJSIP_SC_DECLINE; m_call->hangup(op); } catch (const Error &e) { reportError(QString::fromStdString(e.info())); } }
void PjsipEngine::transfer(const QString &extension) {
    if (!m_call || extension.trimmed().isEmpty()) { reportError(tr("Ingresá el interno a transferir")); return; }
    try { CallOpParam op; const auto uri=QString("sip:%1@%2").arg(extension,m_config.server); m_call->xfer(uri.toStdString(), op); }
    catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

void PjsipEngine::sendDtmf(const QString &digits) {
    if (!m_call || digits.isEmpty()) return;
    try { m_call->dialDtmf(digits.toStdString()); }
    catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

void PjsipEngine::setHold(bool enabled) {
    if (!m_call) return;
    try {
        CallOpParam op(true);
        if (enabled) m_call->setHold(op);
        else { op.opt.flag = PJSUA_CALL_UNHOLD; m_call->reinvite(op); }
    } catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

bool PjsipEngine::startRecording(const QString &path) {
    if (!m_call || path.isEmpty() || m_recorder) return false;
    try {
        m_recorder = std::make_unique<AudioMediaRecorder>();
        m_recorder->createRecorder(path.toStdString());
        const auto ci = m_call->getInfo();
        for (unsigned i = 0; i < ci.media.size(); ++i) {
            if (ci.media[i].type == PJMEDIA_TYPE_AUDIO && ci.media[i].status == PJSUA_CALL_MEDIA_ACTIVE) {
                m_call->getAudioMedia(i).startTransmit(*m_recorder);
                Endpoint::instance().audDevManager().getCaptureDevMedia().startTransmit(*m_recorder);
                return true;
            }
        }
    } catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
    m_recorder.reset();
    return false;
}

void PjsipEngine::stopRecording() {
    if (!m_recorder) return;
    try {
        if (m_call) {
            const auto ci = m_call->getInfo();
            for (unsigned i = 0; i < ci.media.size(); ++i)
                if (ci.media[i].type == PJMEDIA_TYPE_AUDIO)
                    m_call->getAudioMedia(i).stopTransmit(*m_recorder);
            Endpoint::instance().audDevManager().getCaptureDevMedia().stopTransmit(*m_recorder);
        }
    } catch (...) {}
    m_recorder.reset();
}

bool PjsipEngine::setLocalAudioMonitor(bool enabled) {
    if (enabled == m_localAudioMonitor) return true;
    try {
        initializeEndpoint();
        auto &adm = Endpoint::instance().audDevManager();
        auto &capture = adm.getCaptureDevMedia();
        auto &playback = adm.getPlaybackDevMedia();
        if (enabled) capture.startTransmit(playback);
        else capture.stopTransmit(playback);
        m_localAudioMonitor = enabled;
        return true;
    } catch (const Error &e) {
        reportError(QString::fromStdString(e.info()));
        return false;
    }
}

void PjsipEngine::reportCallState(CallImpl *call) {
    try {
        const auto ci=call->getInfo();
        CallState state=CallState::Calling;
        bool hasActiveAudio=false;
        for (const auto &media : ci.media) {
            if (media.type==PJMEDIA_TYPE_AUDIO && media.status==PJSUA_CALL_MEDIA_ACTIVE) {
                hasActiveAudio=true;
                break;
            }
        }
        if (ci.state==PJSIP_INV_STATE_INCOMING || (ci.state==PJSIP_INV_STATE_EARLY && ci.role==PJSIP_ROLE_UAS)) state=CallState::Incoming;
        else if (ci.state==PJSIP_INV_STATE_EARLY && ci.role==PJSIP_ROLE_UAC && hasActiveAudio) state=CallState::EarlyMedia;
        else if (ci.state==PJSIP_INV_STATE_CONFIRMED) state=CallState::Connected;
        else if (ci.state==PJSIP_INV_STATE_DISCONNECTED) state=CallState::Ended;
        const auto peer=friendlySipPeer(ci.remoteUri);
        const auto target=sipDialTarget(ci.remoteUri);
        QMetaObject::invokeMethod(this, [this,state,peer,target,call]{ emit callStateChanged(state,peer,target); if (state==CallState::Ended) releaseDisconnectedCall(call); }, Qt::QueuedConnection);
    } catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

void PjsipEngine::reportEarlyMedia(CallImpl *call) {
    try {
        const auto ci = call->getInfo();
        if (ci.state != PJSIP_INV_STATE_EARLY) return;
        const auto peer = friendlySipPeer(ci.remoteUri);
        const auto target = sipDialTarget(ci.remoteUri);
        QMetaObject::invokeMethod(this, [this,peer,target]{
            emit callStateChanged(CallState::EarlyMedia, peer, target);
        }, Qt::QueuedConnection);
    } catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

void PjsipEngine::reportError(const QString &message) {
    QMetaObject::invokeMethod(this, [this,message]{ emit errorOccurred(message); }, Qt::QueuedConnection);
}

void PjsipEngine::releaseDisconnectedCall(CallImpl *call) {
    if (m_call.get() == call) { stopRecording(); m_call.reset(); }
}

void PjsipEngine::reportRegistration(bool active, int code, const std::string &reason) {
    const auto text=QString("SIP %1 · %2").arg(code).arg(QString::fromStdString(reason));
    QMetaObject::invokeMethod(this, [this,active,text]{ emit registrationChanged(active,text); }, Qt::QueuedConnection);
}
