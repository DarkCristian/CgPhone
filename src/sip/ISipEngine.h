#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

struct SipAccountConfig {
    QString user;
    QString password;
    QString server;
    QString proxy;
    bool proxyEnabled = false;
    QString logoutCode = "*02";
    bool alwaysVisible = false;
    bool startWithOs = false;
    QStringList enabledCodecs = {"PCMA/8000/1", "PCMU/8000/1"};
    bool localRecordingEnabled = false;
    QString recordingPath;
    QString recordingFormat = "wav";
};

class ISipEngine : public QObject {
    Q_OBJECT
public:
    enum class CallState { Idle, Incoming, Calling, EarlyMedia, Connected, Ended, Error };
    Q_ENUM(CallState)

    explicit ISipEngine(QObject *parent = nullptr) : QObject(parent) {}
    ~ISipEngine() override = default;

    virtual void configure(const SipAccountConfig &config) = 0;
    virtual void registerAccount() = 0;
    virtual void makeCall(const QString &destination) = 0;
    virtual void answer() = 0;
    virtual void hangup() = 0;
    virtual void transfer(const QString &extension) = 0;
    virtual void sendDtmf(const QString &digits) = 0;
    virtual void setHold(bool enabled) = 0;
    virtual bool startRecording(const QString &path) = 0;
    virtual void stopRecording() = 0;
    virtual bool setLocalAudioMonitor(bool enabled) = 0;
    virtual void setDnd(bool enabled) = 0;
    virtual void setAutoAnswer(bool enabled) = 0;

signals:
    void registrationChanged(bool registered, const QString &message);
    void callStateChanged(ISipEngine::CallState state, const QString &peer);
    void holdStateChanged(bool held);
    void errorOccurred(const QString &message);
};
