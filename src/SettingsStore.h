#pragma once

#include "sip/ISipEngine.h"
#include <QObject>

class SettingsStore final : public QObject {
    Q_OBJECT
public:
    explicit SettingsStore(QObject *parent = nullptr);
    SipAccountConfig loadAccount() const;
    bool saveAccount(const SipAccountConfig &config);
    static bool isRunningElevated();
};
