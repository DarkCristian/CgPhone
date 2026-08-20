#include "PjsipEngine.h"
#include <pjsua2.hpp>
#include <QMetaObject>

using namespace pj;

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
            if (getInfo().state == PJSIP_INV_STATE_EARLY)
                owner->reportEarlyMedia(this);
        } catch (const Error &e) { owner->reportError(QString::fromStdString(e.info())); }
    }
    PjsipEngine *owner;
};

class PjsipEngine::AccountImpl final : public Account {
public:
    explicit AccountImpl(PjsipEngine *owner) : owner(owner) {}
    void onIncomingCall(OnIncomingCallParam &p) override {
        const int callId = p.callId;
        QMetaObject::invokeMethod(owner, [owner=owner,callId]{ owner->incoming(callId); }, Qt::QueuedConnection);
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
    EpConfig ep; ep.uaConfig.userAgent = "CgPhone/0.2.10"; ep.uaConfig.threadCnt = 1; ep.logConfig.level = 4;
    m_endpoint->libInit(ep);
    TransportConfig udp; udp.port = 0;
    m_endpoint->transportCreate(PJSIP_TRANSPORT_UDP, udp);
    m_endpoint->libStart(); m_initialized = true;
}

void PjsipEngine::configure(const SipAccountConfig &config) { m_config = config; }

void PjsipEngine::registerAccount() {
    try {
        initializeEndpoint();
        m_call.reset(); m_account = std::make_unique<AccountImpl>(this);
        AccountConfig ac;
        const auto server = m_config.server.trimmed();
        ac.idUri = QString("sip:%1@%2").arg(m_config.user, server).toStdString();
        ac.regConfig.registrarUri = QString("sip:%1").arg(server).toStdString();
        ac.sipConfig.authCreds.emplace_back("digest", "*", m_config.user.toStdString(), 0, m_config.password.toStdString());
        if (m_config.proxyEnabled && !m_config.proxy.trimmed().isEmpty())
            ac.sipConfig.proxies.push_back(QString("sip:%1;lr").arg(m_config.proxy.trimmed()).toStdString());
        m_account->create(ac);
    } catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

void PjsipEngine::makeCall(const QString &destination) {
    if (!m_account) { reportError(tr("La cuenta SIP no está registrada")); return; }
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
    if (m_dnd) {
        CallOpParam op; op.statusCode = PJSIP_SC_BUSY_HERE; m_call->answer(op); return;
    }
    QString peer;
    try { peer = QString::fromStdString(m_call->getInfo().remoteUri); } catch (...) {}
    if (m_autoAnswer) {
        // Sin demora artificial ni ringtone local: la central controla el
        // anuncio de campaña/cola y el momento del bridge con el cliente.
        answer();
        return;
    }
    emit callStateChanged(CallState::Incoming, peer);
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
        const auto peer=QString::fromStdString(ci.remoteUri);
        QMetaObject::invokeMethod(this, [this,state,peer,call]{ emit callStateChanged(state,peer); if (state==CallState::Ended) releaseDisconnectedCall(call); }, Qt::QueuedConnection);
    } catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

void PjsipEngine::reportEarlyMedia(CallImpl *call) {
    try {
        const auto ci = call->getInfo();
        if (ci.state != PJSIP_INV_STATE_EARLY) return;
        const auto peer = QString::fromStdString(ci.remoteUri);
        QMetaObject::invokeMethod(this, [this,peer]{
            emit callStateChanged(CallState::EarlyMedia, peer);
        }, Qt::QueuedConnection);
    } catch (const Error &e) { reportError(QString::fromStdString(e.info())); }
}

void PjsipEngine::reportError(const QString &message) {
    QMetaObject::invokeMethod(this, [this,message]{ emit errorOccurred(message); }, Qt::QueuedConnection);
}

void PjsipEngine::releaseDisconnectedCall(CallImpl *call) {
    if (m_call.get() == call) m_call.reset();
}

void PjsipEngine::reportRegistration(bool active, int code, const std::string &reason) {
    const auto text=QString("SIP %1 · %2").arg(code).arg(QString::fromStdString(reason));
    QMetaObject::invokeMethod(this, [this,active,text]{ emit registrationChanged(active,text); }, Qt::QueuedConnection);
}
