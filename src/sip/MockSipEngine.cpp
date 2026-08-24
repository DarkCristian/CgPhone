#include "MockSipEngine.h"

MockSipEngine::MockSipEngine(QObject *parent) : ISipEngine(parent) {}

void MockSipEngine::configure(const SipAccountConfig &config) { m_config = config; }

void MockSipEngine::registerAccount() {
    const bool valid = !m_config.user.trimmed().isEmpty() && !m_config.server.trimmed().isEmpty();
    QTimer::singleShot(350, this, [this, valid] {
        emit registrationChanged(valid, valid ? tr("Registrado (modo demostración)")
                                               : tr("Completá usuario y servidor"));
    });
}

void MockSipEngine::makeCall(const QString &destination) {
    m_peer = destination.trimmed();
    if (m_peer.isEmpty()) {
        emit errorOccurred(tr("Ingresá un número o interno"));
        return;
    }
    emit callStateChanged(CallState::Calling, m_peer, m_peer);
    QTimer::singleShot(900, this, [this] { emit callStateChanged(CallState::Connected, m_peer, m_peer); });
}

void MockSipEngine::answer() { emit callStateChanged(CallState::Connected, m_peer, m_peer); }
void MockSipEngine::hangup() { emit callStateChanged(CallState::Ended, m_peer, m_peer); }

void MockSipEngine::transfer(const QString &extension) {
    if (extension.trimmed().isEmpty()) {
        emit errorOccurred(tr("Ingresá el interno a transferir"));
        return;
    }
    emit callStateChanged(CallState::Ended, m_peer, m_peer);
}
void MockSipEngine::sendDtmf(const QString &) {}
bool MockSipEngine::setLocalAudioMonitor(bool enabled) { m_localAudioMonitor = enabled; return true; }
