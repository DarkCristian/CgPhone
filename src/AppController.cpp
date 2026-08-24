#include "AppController.h"
#include "sip/MockSipEngine.h"
#include "Mp3Encoder.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QDateTime>
#include <QRegularExpression>
#include <QtConcurrent>
#include <QPointer>
#include <QFile>
#include <QFileInfo>
#include <cstdio>
#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>

namespace {
WNDPROC g_diagnosticConsoleProcedure = nullptr;

LRESULT CALLBACK diagnosticConsoleWindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_CLOSE) {
        ShowWindow(window, SW_HIDE);
        return 0;
    }
    return g_diagnosticConsoleProcedure
        ? CallWindowProcW(g_diagnosticConsoleProcedure, window, message, wParam, lParam)
        : DefWindowProcW(window, message, wParam, lParam);
}
}
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
    connect(m_sip.get(), &ISipEngine::holdStateChanged, this, &AppController::onHoldStateChanged);
    connect(m_sip.get(), &ISipEngine::registrationChanged, this, [this](bool ok, const QString &text) {
        m_registered = ok; m_registrationText = text; emit registrationChanged();
        QSettings runtime;
        runtime.setValue("runtime/registered", ok);
        runtime.setValue("runtime/registrationText", text);
    });
    connect(m_sip.get(), &ISipEngine::errorOccurred, this, &AppController::toast);
    connect(&m_durationTimer, &QTimer::timeout, this, &AppController::durationChanged);
    m_durationTimer.setInterval(1000);
    m_ringtone.setSource(QUrl("qrc:/qt/qml/CgPhone/assets/sounds/ringtone.wav")); m_ringtone.setLoopCount(QSoundEffect::Infinite);
    m_ringback.setSource(QUrl("qrc:/qt/qml/CgPhone/assets/sounds/ringing.wav")); m_ringback.setLoopCount(QSoundEffect::Infinite);
    m_hangupSound.setSource(QUrl("qrc:/qt/qml/CgPhone/assets/sounds/hangup.wav"));
    // El paquete fuente puede reemplazar este WAV por el sonido corporativo
    // sin cambiar el backend ni el mapeo de teclado/DTMF.
    m_keypadSound.setSource(QUrl("qrc:/qt/qml/CgPhone/assets/sounds/phone-keypad-button-dial.wav"));
    QSettings userSettings;
    m_dnd = userSettings.value("behavior/dnd", false).toBool();
    m_autoAnswer = userSettings.value("behavior/autoAnswer", false).toBool();
    m_muteWarningShown = userSettings.value("behavior/muteWarningShown", false).toBool();
    m_sip->setDnd(m_dnd); m_sip->setAutoAnswer(m_autoAnswer);
    m_loadedAccount = m_settings.loadAccount();
    if (m_configurationMode) {
        m_registered = userSettings.value("runtime/registered", false).toBool();
        m_registrationText = userSettings.value("runtime/registrationText", tr("Sin registrar")).toString();
    }
    m_sip->configure(m_loadedAccount);
    connect(&m_holdTimer, &QTimer::timeout, this, [this] {
        if (!m_held || !m_holdElapsed.isValid()) return;
        const int seconds = int(m_holdElapsed.elapsed() / 1000);
        if (seconds < m_nextHoldReminder) return;
        const int reminder = m_nextHoldReminder;
        m_nextHoldReminder += 30;
        const QString elapsed = QString("%1:%2").arg(reminder / 60).arg(reminder % 60, 2, 10, QLatin1Char('0'));
        emit toast(tr("Hace %1 tenés al cliente holdeado, retomá la llamada").arg(elapsed));
    });
    m_holdTimer.setInterval(1000);
    connect(&m_muteTimer, &QTimer::timeout, this, [this] {
        if (m_muted && !m_held)
            emit toast(tr("Recordá que estás en mute"));
    });
    m_muteTimer.setInterval(3000);
    if (!m_configurationMode && !m_loadedAccount.user.trimmed().isEmpty() && !m_loadedAccount.server.trimmed().isEmpty()) m_sip->registerAccount();
    if (!m_configurationMode) {
        connect(&m_configRefreshTimer, &QTimer::timeout, this, &AppController::refreshAccountIfChanged);
        m_configRefreshTimer.start(1500);
    }
}

void AppController::setDialedNumber(const QString &value) { if (value == m_dialedNumber) return; m_dialedNumber = value; emit dialedNumberChanged(); }
void AppController::appendDigit(const QString &digit) {
    if (digit.isEmpty()) return;
    m_keypadSound.stop(); m_keypadSound.play();
    if (m_inCall && m_wasConnected) m_sip->sendDtmf(digit);
    else if (!m_inCall) setDialedNumber(m_dialedNumber + digit);
}
void AppController::backspace() { setDialedNumber(m_dialedNumber.chopped(1)); }
void AppController::call() { m_sip->makeCall(m_dialedNumber); }
void AppController::answer() { m_sip->answer(); }
void AppController::hangup() { m_sip->hangup(); }
void AppController::transfer(const QString &extension) { m_sip->transfer(extension); }
void AppController::toggleHold() {
    if (!m_inCall || !m_wasConnected || m_holdRequested) return;
    m_holdRequested = true;
    m_sip->setHold(!m_held);
    m_callStatus = m_held ? tr("Retomando llamada…") : tr("Poniendo en espera…");
    emit callChanged();
}

void AppController::onHoldStateChanged(bool held) {
    if (!m_inCall || !m_wasConnected) return;
    m_holdRequested = false;
    if (m_held == held) { emit callChanged(); return; }
    m_held = held;
    if (held) {
        m_holdElapsed.restart();
        m_nextHoldReminder = 30;
        m_holdTimer.start();
        m_callStatus = tr("En espera");
        emit toast(tr("Llamada en espera"));
    } else {
        m_holdTimer.stop();
        m_holdElapsed.invalidate();
        m_callStatus = tr("Conectada");
        emit toast(tr("Llamada retomada"));
    }
    emit callChanged();
}

void AppController::setMuted(bool muted) {
    if (m_muted == muted) return;
    m_muted = muted;
    if (muted) {
        if (!m_muteWarningShown) {
            emit toast(tr("Mutear el micrófono puede ser una mala práctica, hacer que se pierdan llamados y, según la política del call center, ocasionar sanciones."));
            m_muteWarningShown = true;
            QSettings().setValue("behavior/muteWarningShown", true);
        } else {
            emit toast(tr("Micrófono muteado"));
        }
        m_muteTimer.start();
    } else {
        m_muteTimer.stop();
        emit toast(tr("Micrófono activado"));
    }
    emit mutedChanged();
}

void AppController::toggleRecording() {
    if (!m_loadedAccount.localRecordingEnabled || !m_inCall || !m_wasConnected) return;
    if (m_recording) { m_sip->stopRecording(); m_recording = false; finalizeRecording(); emit callChanged(); return; }
    QDir directory(m_loadedAccount.recordingPath);
    if (!directory.exists() && !directory.mkpath(".")) { emit toast(tr("No se pudo crear la ruta de grabaciones")); return; }
    const auto peerSafe = (m_peer.isEmpty() ? QStringLiteral("Asterisk") : m_peer).replace(QRegularExpression("[^A-Za-z0-9_-]"), "_");
    const QString file = directory.filePath(QString("%1_%2.wav").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"), peerSafe));
    m_recording = m_sip->startRecording(QDir::toNativeSeparators(file));
    m_recordingWavPath = m_recording ? QDir::toNativeSeparators(file) : QString();
    emit toast(m_recording ? tr("Grabación local iniciada") : tr("No se pudo iniciar la grabación"));
    emit callChanged();
}

void AppController::finalizeRecording() {
    const QString wavPath = m_recordingWavPath;
    m_recordingWavPath.clear();
    if (wavPath.isEmpty()) return;
    if (m_loadedAccount.recordingFormat.compare(QStringLiteral("mp3"), Qt::CaseInsensitive) != 0) {
        emit toast(tr("Grabación WAV finalizada"));
        return;
    }

    const QString mp3Path = QFileInfo(wavPath).absolutePath() + QDir::separator() +
                            QFileInfo(wavPath).completeBaseName() + QStringLiteral(".mp3");
    emit toast(tr("Convirtiendo grabación a MP3…"));
    QPointer<AppController> self(this);
    QtConcurrent::run([self,wavPath,mp3Path] {
        const auto result = Mp3Encoder::encodeWav(wavPath, mp3Path);
        if (result.success) QFile::remove(wavPath);
        if (!self) return;
        QMetaObject::invokeMethod(self, [self,result] {
            if (!self) return;
            emit self->toast(result.success
                ? self->tr("Grabación MP3 finalizada")
                : self->tr("%1").arg(result.error));
        }, Qt::QueuedConnection);
    });
}

void AppController::toggleDebugConsole() {
#ifdef Q_OS_WIN
    if (const HWND console = GetConsoleWindow()) {
        const bool visible = IsWindowVisible(console);
        ShowWindow(console, visible ? SW_HIDE : SW_SHOW);
        if (!visible) SetForegroundWindow(console);
        return;
    }
    if (!AllocConsole()) return;
    FILE *stream = nullptr;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    SetConsoleTitleW(L"CgPhone · Diagnóstico SIP");
    if (const HWND console = GetConsoleWindow()) {
        g_diagnosticConsoleProcedure = reinterpret_cast<WNDPROC>(
            SetWindowLongPtrW(console, GWLP_WNDPROC,
                              reinterpret_cast<LONG_PTR>(diagnosticConsoleWindowProcedure)));
        SetForegroundWindow(console);
    }
#else
    emit toast(tr("La consola de diagnóstico se controla desde la terminal en Linux"));
#endif
}
void AppController::setDnd(bool value) {
    if (m_dnd == value) return;
    m_dnd = value;
    m_sip->setDnd(value);
    QSettings().setValue("behavior/dnd", value);
    if (value && m_autoAnswer) {
        m_autoAnswer = false;
        m_sip->setAutoAnswer(false);
        QSettings().setValue("behavior/autoAnswer", false);
        emit autoAnswerChanged();
    }
    emit dndChanged();
    emit toast(value ? tr("DND activado · las llamadas entrantes se rechazarán")
                     : tr("DND desactivado · disponible para recibir llamadas"));
}
void AppController::setAutoAnswer(bool value) {
    if (m_autoAnswer == value) return;
    m_autoAnswer = value;
    m_sip->setAutoAnswer(value);
    QSettings().setValue("behavior/autoAnswer", value);
    if (value && m_dnd) {
        m_dnd = false;
        m_sip->setDnd(false);
        QSettings().setValue("behavior/dnd", false);
        emit dndChanged();
    }
    emit autoAnswerChanged();
    emit toast(value ? tr("Autorespuesta activada") : tr("Autorespuesta desactivada"));
}
void AppController::registerAccount() { m_sip->configure(m_settings.loadAccount()); m_sip->registerAccount(); }

void AppController::saveAccount(const QString &user, const QString &password, const QString &server, const QString &proxy, bool proxyEnabled, const QString &logoutCode, bool alwaysVisible, bool startWithOs, const QVariantList &enabledCodecs, bool localRecordingEnabled, const QString &recordingPath, const QString &recordingFormat) {
    if (!m_adminMode) { emit toast(tr("Se requieren privilegios de administrador")); return; }
    SipAccountConfig c; c.user=user; c.password=password; c.server=server; c.proxy=proxy; c.proxyEnabled=proxyEnabled;
    c.logoutCode=logoutCode; c.alwaysVisible=alwaysVisible; c.startWithOs=startWithOs;
    c.enabledCodecs.clear();
    for (const auto &codec : enabledCodecs) c.enabledCodecs.append(codec.toString());
    if (c.enabledCodecs.isEmpty()) { emit toast(tr("Activá al menos un codec de audio")); return; }
    c.localRecordingEnabled=localRecordingEnabled; c.recordingPath=recordingPath;
    c.recordingFormat = recordingFormat.compare(QStringLiteral("mp3"), Qt::CaseInsensitive) == 0 ? QStringLiteral("mp3") : QStringLiteral("wav");
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
        current.alwaysVisible == m_loadedAccount.alwaysVisible && current.startWithOs == m_loadedAccount.startWithOs &&
        current.enabledCodecs == m_loadedAccount.enabledCodecs && current.localRecordingEnabled == m_loadedAccount.localRecordingEnabled &&
        current.recordingPath == m_loadedAccount.recordingPath && current.recordingFormat == m_loadedAccount.recordingFormat) return;
    m_loadedAccount = current;
    emit accountChanged();
    m_sip->configure(m_loadedAccount);
    if (!m_loadedAccount.user.trimmed().isEmpty() && !m_loadedAccount.server.trimmed().isEmpty()) m_sip->registerAccount();
}

QVariantMap AppController::account() const {
    const auto c = m_settings.loadAccount();
    return {{"user",c.user},{"password",c.password},{"server",c.server},{"proxy",c.proxy},{"proxyEnabled",c.proxyEnabled},{"logoutCode",c.logoutCode},{"alwaysVisible",c.alwaysVisible},{"startWithOs",c.startWithOs},{"enabledCodecs",c.enabledCodecs},{"localRecordingEnabled",c.localRecordingEnabled},{"recordingPath",c.recordingPath},{"recordingFormat",c.recordingFormat}};
}

QString AppController::duration() const {
    const qint64 seconds = m_held && m_holdElapsed.isValid() ? m_holdElapsed.elapsed() / 1000
                                                             : (m_elapsed.isValid() ? m_elapsed.elapsed() / 1000 : 0);
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
    case ISipEngine::CallState::Incoming:
        m_callStatus = m_dnd ? tr("Rechazada por DND") : (m_autoAnswer ? tr("Atendiendo automáticamente…") : tr("Llamada entrante"));
        m_inCall = true; m_incoming = !m_dnd && !m_autoAnswer; m_callDirection="entrante"; m_wasConnected=false;
        if (!m_dnd && !m_autoAnswer) m_ringtone.play();
        break;
    case ISipEngine::CallState::EarlyMedia: m_ringtone.stop(); m_ringback.stop(); m_callStatus = tr("Audio de la central…"); m_inCall = true; m_incoming = false; break;
    case ISipEngine::CallState::Connected: m_ringtone.stop(); m_ringback.stop(); m_callStatus = tr("Conectada"); m_inCall = true; m_incoming = false; m_wasConnected=true; m_elapsed.restart(); m_durationTimer.start(); break;
    case ISipEngine::CallState::Ended:
        m_history.addCall({peer, m_callDirection, QDateTime::currentDateTime(), int(m_elapsed.isValid() ? m_elapsed.elapsed()/1000 : 0), m_callDirection=="entrante" && !m_wasConnected});
        m_ringtone.stop(); m_ringback.stop(); { const bool wasRecording=m_recording; m_sip->stopRecording(); m_recording=false; if (wasRecording) finalizeRecording(); } m_held=false; m_holdRequested=false; m_holdTimer.stop(); m_holdElapsed.invalidate(); m_hangupSound.play(); m_callStatus = tr("Disponible"); m_inCall = false; m_incoming=false; m_durationTimer.stop(); m_elapsed.invalidate(); setDialedNumber({}); m_peer.clear(); break;
    case ISipEngine::CallState::Error: m_callStatus = tr("Error"); m_inCall = false; m_incoming=false; break;
    case ISipEngine::CallState::Idle: m_callStatus = tr("Listo"); m_inCall = false; m_incoming=false; break;
    }
    emit callChanged(); emit durationChanged();
}
