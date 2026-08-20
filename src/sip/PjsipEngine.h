#pragma once

#include "ISipEngine.h"
#include <memory>

namespace pj { class Endpoint; class Account; class Call; }

class PjsipEngine final : public ISipEngine {
    Q_OBJECT
public:
    explicit PjsipEngine(QObject *parent = nullptr);
    ~PjsipEngine() override;
    void configure(const SipAccountConfig &config) override;
    void registerAccount() override;
    void makeCall(const QString &destination) override;
    void answer() override;
    void hangup() override;
    void transfer(const QString &extension) override;
    void sendDtmf(const QString &digits) override;
    bool setLocalAudioMonitor(bool enabled) override;
    void setDnd(bool enabled) override { m_dnd = enabled; }
    void setAutoAnswer(bool enabled) override { m_autoAnswer = enabled; }

private:
    class AccountImpl;
    class CallImpl;
    friend class AccountImpl;
    friend class CallImpl;
    void initializeEndpoint();
    void incoming(int callId);
    void reportCallState(CallImpl *call);
    void reportEarlyMedia(CallImpl *call);
    void reportRegistration(bool active, int code, const std::string &reason);
    void reportError(const QString &message);
    void releaseDisconnectedCall(CallImpl *call);
    SipAccountConfig m_config;
    std::unique_ptr<pj::Endpoint> m_endpoint;
    std::unique_ptr<AccountImpl> m_account;
    std::unique_ptr<CallImpl> m_call;
    bool m_initialized = false, m_dnd = false, m_autoAnswer = false;
    bool m_localAudioMonitor = false;
};
