#pragma once

#include "CallHistoryModel.h"
#include "SettingsStore.h"
#include "sip/ISipEngine.h"
#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QSoundEffect>
#include <memory>

class AppController final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString dialedNumber READ dialedNumber WRITE setDialedNumber NOTIFY dialedNumberChanged)
    Q_PROPERTY(QString peer READ peer NOTIFY callChanged)
    Q_PROPERTY(QString callStatus READ callStatus NOTIFY callChanged)
    Q_PROPERTY(QString duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(bool inCall READ inCall NOTIFY callChanged)
    Q_PROPERTY(bool incoming READ incoming NOTIFY callChanged)
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
    Q_INVOKABLE void answer();
    Q_INVOKABLE void hangup();
    Q_INVOKABLE void transfer(const QString &extension);
    Q_INVOKABLE void registerAccount();
    Q_INVOKABLE void saveAccount(const QString &user, const QString &password, const QString &server,
                                 const QString &proxy, bool proxyEnabled, const QString &logoutCode,
                                 bool alwaysVisible, bool startWithOs);
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
    void toast(const QString &message);

private:
    void onCallState(ISipEngine::CallState state, const QString &peer);
    void refreshAccountIfChanged();
    std::unique_ptr<ISipEngine> m_sip;
    SettingsStore m_settings;
    CallHistoryModel m_history;
    QString m_dialedNumber, m_peer, m_callStatus = tr("Listo");
    bool m_inCall = false, m_incoming = false, m_dnd = false, m_autoAnswer = false;
    bool m_wasConnected = false;
    QString m_callDirection = "saliente";
    bool m_registered = false, m_adminMode = false, m_configurationMode = false;
    bool m_localAudioMonitor = false;
    QString m_registrationText = tr("Sin registrar");
    QElapsedTimer m_elapsed;
    QTimer m_durationTimer;
    QTimer m_configRefreshTimer;
    QSoundEffect m_ringtone, m_ringback, m_hangupSound;
    SipAccountConfig m_loadedAccount;
};
