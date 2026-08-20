#include "AppController.h"
#include "sip/MockSipEngine.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif
#ifdef CGPHONE_WITH_PJSIP
#include "sip/PjsipEngine.h"
#endif

AppController::AppController(QObject *parent)
    : QObject(parent), m_settings(this), m_history(this), m_adminMode(SettingsStore::isRunningElevated()) {
    m_configurationMode = QCoreApplication::arguments().contains("--configure") && m_adminMode;
#ifdef CGPHONE_WITH_PJSIP
    m_sip = std::make_unique<PjsipEngine>();
#else
    m_sip = std::make_unique<MockSipEngine>();
#endif
    connect(m_sip.get(), &ISipEngine::callStateChanged, this, &AppController::onCallState);
    connect(m_sip.get(), &ISipEngine::registrationChanged, this, [this](bool ok, const QString &text) {
        m_registered = ok; m_registrationText = text; emit registrationChanged();
    });
    connect(m_sip.get(), &ISipEngine::errorOccurred, this, &AppController::toast);
    connect(&m_durationTimer, &QTimer::timeout, this, &AppController::durationChanged);
    m_durationTimer.setInterval(1000);
    m_ringtone.setSource(QUrl("qrc:/qt/qml/CgPhone/assets/sounds/ringtone.wav")); m_ringtone.setLoopCount(QSoundEffect::Infinite);
    m_ringback.setSource(QUrl("qrc:/qt/qml/CgPhone/assets/sounds/ringing.wav")); m_ringback.setLoopCount(QSoundEffect::Infinite);
    m_hangupSound.setSource(QUrl("qrc:/qt/qml/CgPhone/assets/sounds/hangup.wav"));
    QSettings userSettings;
    m_dnd = userSettings.value("behavior/dnd", false).toBool();
    m_autoAnswer = userSettings.value("behavior/autoAnswer", false).toBool();
    m_sip->setDnd(m_dnd); m_sip->setAutoAnswer(m_autoAnswer);
    m_loadedAccount = m_settings.loadAccount();
    m_sip->configure(m_loadedAccount);
    if (!m_configurationMode && !m_loadedAccount.user.trimmed().isEmpty() && !m_loadedAccount.server.trimmed().isEmpty()) m_sip->registerAccount();
    if (!m_configurationMode) {
        connect(&m_configRefreshTimer, &QTimer::timeout, this, &AppController::refreshAccountIfChanged);
        m_configRefreshTimer.start(1500);
    }
}

void AppController::setDialedNumber(const QString &value) { if (value == m_dialedNumber) return; m_dialedNumber = value; emit dialedNumberChanged(); }
void AppController::appendDigit(const QString &digit) { if (m_inCall && m_wasConnected) m_sip->sendDtmf(digit); else if (!m_inCall) setDialedNumber(m_dialedNumber + digit); }
void AppController::backspace() { setDialedNumber(m_dialedNumber.chopped(1)); }
void AppController::call() { m_sip->makeCall(m_dialedNumber); }
void AppController::answer() { m_sip->answer(); }
void AppController::hangup() { m_sip->hangup(); }
void AppController::transfer(const QString &extension) { m_sip->transfer(extension); }
void AppController::setDnd(bool value) { if (m_dnd == value) return; m_dnd = value; m_sip->setDnd(value); QSettings().setValue("behavior/dnd", value); emit dndChanged(); }
void AppController::setAutoAnswer(bool value) { if (m_autoAnswer == value) return; m_autoAnswer = value; m_sip->setAutoAnswer(value); QSettings().setValue("behavior/autoAnswer", value); emit autoAnswerChanged(); }
void AppController::registerAccount() { m_sip->configure(m_settings.loadAccount()); m_sip->registerAccount(); }

void AppController::saveAccount(const QString &user, const QString &password, const QString &server, const QString &proxy, bool proxyEnabled, const QString &logoutCode, bool alwaysVisible, bool startWithOs) {
    if (!m_adminMode) { emit toast(tr("Se requieren privilegios de administrador")); return; }
    SipAccountConfig c{user,password,server,proxy,proxyEnabled,logoutCode,alwaysVisible,startWithOs};
    if (!m_settings.saveAccount(c)) { emit toast(tr("No se pudo aplicar el inicio con el SO o guardar la configuración")); return; }
    m_loadedAccount = c; m_sip->configure(c); emit accountChanged(); emit toast(tr("Configuración guardada"));
    if (m_configurationMode) QTimer::singleShot(120, qApp, &QCoreApplication::quit);
}

void AppController::requestAdminConfiguration() {
#ifdef Q_OS_WIN
    const auto executable = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    const auto result = reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"runas",
        reinterpret_cast<LPCWSTR>(executable.utf16()), L"--configure", nullptr, SW_SHOWNORMAL));
    if (result <= 32) emit toast(tr("No se pudo abrir la configuración como administrador"));
#else
    emit toast(tr("Ejecutá CgPhone con sudo/root para configurar esta versión"));
#endif
}

void AppController::discardConfiguration() {
    if (m_configurationMode) QCoreApplication::quit();
}

void AppController::logoutAgent() {
    if (m_inCall) { emit toast(tr("Finalizá la llamada antes de desloguearte")); return; }
    const auto account = m_settings.loadAccount();
    const auto code = account.logoutCode.trimmed().isEmpty() ? QStringLiteral("*02") : account.logoutCode;
    setDialedNumber(code);
    m_sip->makeCall(code);
}

void AppController::toggleLocalAudioMonitor() {
    if (m_inCall) { emit toast(tr("La prueba de audio no está disponible durante una llamada")); return; }
    const bool requested = !m_localAudioMonitor;
    if (!m_sip->setLocalAudioMonitor(requested)) return;
    m_localAudioMonitor = requested;
    emit localAudioMonitorChanged();
    emit toast(requested ? tr("Prueba local activa · hablá para comprobar el headset")
                         : tr("Prueba local finalizada"));
}

void AppController::refreshAccountIfChanged() {
    const auto current = m_settings.loadAccount();
    if (current.user == m_loadedAccount.user && current.password == m_loadedAccount.password &&
        current.server == m_loadedAccount.server && current.proxy == m_loadedAccount.proxy &&
        current.proxyEnabled == m_loadedAccount.proxyEnabled && current.logoutCode == m_loadedAccount.logoutCode &&
        current.alwaysVisible == m_loadedAccount.alwaysVisible && current.startWithOs == m_loadedAccount.startWithOs) return;
    m_loadedAccount = current;
    emit accountChanged();
    m_sip->configure(m_loadedAccount);
    if (!m_loadedAccount.user.trimmed().isEmpty() && !m_loadedAccount.server.trimmed().isEmpty()) m_sip->registerAccount();
}

QVariantMap AppController::account() const {
    const auto c = m_settings.loadAccount();
    return {{"user",c.user},{"password",c.password},{"server",c.server},{"proxy",c.proxy},{"proxyEnabled",c.proxyEnabled},{"logoutCode",c.logoutCode},{"alwaysVisible",c.alwaysVisible},{"startWithOs",c.startWithOs}};
}

QString AppController::duration() const {
    const qint64 seconds = m_elapsed.isValid() ? m_elapsed.elapsed() / 1000 : 0;
    return QString("%1:%2").arg(seconds / 60, 2, 10, QLatin1Char('0')).arg(seconds % 60, 2, 10, QLatin1Char('0'));
}

void AppController::onCallState(ISipEngine::CallState state, const QString &peer) {
    if (state != ISipEngine::CallState::Idle && m_localAudioMonitor) {
        m_sip->setLocalAudioMonitor(false);
        m_localAudioMonitor = false;
        emit localAudioMonitorChanged();
    }
    m_peer = peer;
    switch (state) {
    case ISipEngine::CallState::Calling: m_callStatus = tr("Llamando…"); m_inCall = true; m_incoming = false; m_callDirection="saliente"; m_wasConnected=false; m_elapsed.invalidate(); if (!m_ringback.isPlaying()) m_ringback.play(); break;
    case ISipEngine::CallState::Incoming: m_callStatus = tr("Llamada entrante"); m_inCall = true; m_incoming = true; m_callDirection="entrante"; m_wasConnected=false; m_ringtone.play(); break;
    case ISipEngine::CallState::EarlyMedia: m_ringtone.stop(); m_ringback.stop(); m_callStatus = tr("Audio de la central…"); m_inCall = true; m_incoming = false; break;
    case ISipEngine::CallState::Connected: m_ringtone.stop(); m_ringback.stop(); m_callStatus = tr("Conectada"); m_inCall = true; m_incoming = false; m_wasConnected=true; m_elapsed.restart(); m_durationTimer.start(); break;
    case ISipEngine::CallState::Ended:
        m_history.addCall({peer, m_callDirection, QDateTime::currentDateTime(), int(m_elapsed.isValid() ? m_elapsed.elapsed()/1000 : 0), m_callDirection=="entrante" && !m_wasConnected});
        m_ringtone.stop(); m_ringback.stop(); m_hangupSound.play(); m_callStatus = tr("Finalizada"); m_inCall = false; m_incoming=false; m_durationTimer.stop(); m_elapsed.invalidate(); break;
    case ISipEngine::CallState::Error: m_callStatus = tr("Error"); m_inCall = false; m_incoming=false; break;
    case ISipEngine::CallState::Idle: m_callStatus = tr("Listo"); m_inCall = false; m_incoming=false; break;
    }
    emit callChanged(); emit durationChanged();
}
