#pragma once

#include "CallHistoryModel.h"
#include "SettingsStore.h"
#include "sip/ISipEngine.h"
#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QSoundEffect>
#include <QVariantList>
#include <memory>

class AppController final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString dialedNumber READ dialedNumber WRITE setDialedNumber NOTIFY dialedNumberChanged)
    Q_PROPERTY(QString peer READ peer NOTIFY callChanged)
    Q_PROPERTY(QString callStatus READ callStatus NOTIFY callChanged)
    Q_PROPERTY(QString duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(bool inCall READ inCall NOTIFY callChanged)
    Q_PROPERTY(bool incoming READ incoming NOTIFY callChanged)
    Q_PROPERTY(bool held READ held NOTIFY callChanged)
    Q_PROPERTY(bool recording READ recording NOTIFY callChanged)
    Q_PROPERTY(bool muted READ muted NOTIFY mutedChanged)
    Q_PROPERTY(bool localRecordingEnabled READ localRecordingEnabled NOTIFY accountChanged)
    Q_PROPERTY(bool dnd READ dnd WRITE setDnd NOTIFY dndChanged)
    Q_PROPERTY(bool autoAnswer READ autoAnswer WRITE setAutoAnswer NOTIFY autoAnswerChanged)
    Q_PROPERTY(bool registered READ registered NOTIFY registrationChanged)
    Q_PROPERTY(QString registrationText READ registrationText NOTIFY registrationChanged)
    Q_PROPERTY(bool adminMode READ adminMode CONSTANT)
    Q_PROPERTY(bool configurationMode READ configurationMode CONSTANT)
    Q_PROPERTY(QString sipUser READ sipUser NOTIFY accountChanged)
    Q_PROPERTY(bool alwaysVisible READ alwaysVisible NOTIFY accountChanged)
    Q_PROPERTY(bool startWithOs READ startWithOs NOTIFY accountChanged)
    Q_PROPERTY(bool localAudioMonitor READ localAudioMonitor NOTIFY localAudioMonitorChanged)
    Q_PROPERTY(CallHistoryModel* history READ history CONSTANT)
public:
    explicit AppController(QObject *parent = nullptr);
    QString dialedNumber() const { return m_dialedNumber; }
    void setDialedNumber(const QString &value);
    QString peer() const { return m_peer; }
    QString callStatus() const { return m_callStatus; }
    QString duration() const;
    bool inCall() const { return m_inCall; }
    bool incoming() const { return m_incoming; }
    bool held() const { return m_held; }
    bool recording() const { return m_recording; }
    bool muted() const { return m_muted; }
    bool localRecordingEnabled() const { return m_loadedAccount.localRecordingEnabled; }
    bool dnd() const { return m_dnd; }
    bool autoAnswer() const { return m_autoAnswer; }
    bool registered() const { return m_registered; }
    QString registrationText() const { return m_registrationText; }
    bool adminMode() const { return m_adminMode; }
    bool configurationMode() const { return m_configurationMode; }
    QString sipUser() const { return m_loadedAccount.user; }
    bool alwaysVisible() const { return m_loadedAccount.alwaysVisible; }
    bool startWithOs() const { return m_loadedAccount.startWithOs; }
    bool localAudioMonitor() const { return m_localAudioMonitor; }
    CallHistoryModel *history() { return &m_history; }

    Q_INVOKABLE void appendDigit(const QString &digit);
    Q_INVOKABLE void backspace();
    Q_INVOKABLE void call();
    Q_INVOKABLE void redial(const QString &destination);
    Q_INVOKABLE void answer();
    Q_INVOKABLE void hangup();
    Q_INVOKABLE void transfer(const QString &extension);
    Q_INVOKABLE void toggleHold();
    Q_INVOKABLE void toggleRecording();
    Q_INVOKABLE void setMuted(bool muted);
    Q_INVOKABLE void toggleDebugConsole();
    Q_INVOKABLE void registerAccount();
    Q_INVOKABLE void saveAccount(const QString &user, const QString &password, const QString &server,
                                 const QString &proxy, bool proxyEnabled, const QString &logoutCode,
                                 bool alwaysVisible, bool startWithOs, const QVariantList &enabledCodecs,
                                 bool localRecordingEnabled, const QString &recordingPath, const QString &recordingFormat);
    Q_INVOKABLE QVariantMap account() const;
    Q_INVOKABLE void requestAdminConfiguration();
    Q_INVOKABLE void discardConfiguration();
    Q_INVOKABLE void logoutAgent();
    Q_INVOKABLE void toggleLocalAudioMonitor();
    void setDnd(bool value);
    void setAutoAnswer(bool value);

signals:
    void dialedNumberChanged(); void callChanged(); void durationChanged();
    void dndChanged(); void autoAnswerChanged(); void registrationChanged();
    void accountChanged();
    void localAudioMonitorChanged();
    void mutedChanged();
    void toast(const QString &message);
    void debugConsoleToggleRequested();

private:
    void onCallState(ISipEngine::CallState state, const QString &peer, const QString &dialTarget);
    void onHoldStateChanged(bool held);
    void refreshAccountIfChanged();
    void finalizeRecording();
    std::unique_ptr<ISipEngine> m_sip;
    SettingsStore m_settings;
    CallHistoryModel m_history;
    QString m_dialedNumber, m_peer, m_callStatus = tr("Listo");
    QString m_recordingWavPath;
    QString m_peerDialTarget;
    bool m_inCall = false, m_incoming = false, m_held = false, m_holdRequested = false, m_recording = false, m_muted = false, m_dnd = false, m_autoAnswer = false;
    bool m_wasConnected = false;
    QString m_callDirection = "saliente";
    bool m_registered = false, m_adminMode = false, m_configurationMode = false;
    bool m_localAudioMonitor = false;
    bool m_muteWarningShown = false;
    QString m_registrationText = tr("Sin registrar");
    QElapsedTimer m_elapsed;
    QTimer m_durationTimer;
    QTimer m_configRefreshTimer;
    QTimer m_holdTimer;
    QElapsedTimer m_holdElapsed;
    int m_nextHoldReminder = 30;
    QTimer m_muteTimer;
    QSoundEffect m_ringtone, m_ringback, m_hangupSound, m_keypadSound;
    SipAccountConfig m_loadedAccount;
};
