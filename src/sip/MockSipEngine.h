#pragma once

#include "ISipEngine.h"
#include <QTimer>

class MockSipEngine final : public ISipEngine {
    Q_OBJECT
public:
    explicit MockSipEngine(QObject *parent = nullptr);
    void configure(const SipAccountConfig &config) override;
    void registerAccount() override;
    void makeCall(const QString &destination) override;
    void answer() override;
    void hangup() override;
    void transfer(const QString &extension) override;
    void sendDtmf(const QString &digits) override;
    void setHold(bool) override {}
    bool startRecording(const QString &) override { return false; }
    void stopRecording() override {}
    bool setLocalAudioMonitor(bool enabled) override;
    void setDnd(bool enabled) override { m_dnd = enabled; }
    void setAutoAnswer(bool enabled) override { m_autoAnswer = enabled; }

private:
    SipAccountConfig m_config;
    QString m_peer;
    bool m_dnd = false;
    bool m_autoAnswer = false;
    bool m_localAudioMonitor = false;
};
