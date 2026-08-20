#include "SettingsStore.h"
#include <QSettings>
#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QTextStream>
#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#include <wincrypt.h>
#else
#include <unistd.h>
#endif

SettingsStore::SettingsStore(QObject *parent) : QObject(parent) {}

static QSettings systemSettings() {
    return QSettings(QSettings::IniFormat, QSettings::SystemScope, "CgPhone", "CgPhone");
}

static bool startWithOsEnabled() {
#ifdef Q_OS_WIN
    QSettings runKey(QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                     QSettings::NativeFormat);
    return runKey.contains(QStringLiteral("CgPhone"));
#else
    return QFile::exists(QStringLiteral("/etc/xdg/autostart/CgPhone.desktop"));
#endif
}

static bool applyStartWithOs(bool enabled) {
#ifdef Q_OS_WIN
    QSettings runKey(QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                     QSettings::NativeFormat);
    if (enabled) {
        const auto executable = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        runKey.setValue(QStringLiteral("CgPhone"), QStringLiteral("\"%1\"").arg(executable));
    } else {
        runKey.remove(QStringLiteral("CgPhone"));
    }
    runKey.sync();
    return runKey.status() == QSettings::NoError;
#else
    const QString directory = QStringLiteral("/etc/xdg/autostart");
    const QString desktopFile = directory + QStringLiteral("/CgPhone.desktop");
    if (!enabled) return !QFile::exists(desktopFile) || QFile::remove(desktopFile);
    if (!QDir().mkpath(directory)) return false;
    QSaveFile file(desktopFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QString executable = QCoreApplication::applicationFilePath();
    executable.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
    executable.replace(QLatin1Char('"'), QStringLiteral("\\\""));
    QTextStream out(&file);
    out << "[Desktop Entry]\nType=Application\nName=CgPhone\nExec=\"" << executable
        << "\"\nTerminal=false\nX-GNOME-Autostart-enabled=true\n";
    return file.commit();
#endif
}

#ifdef Q_OS_WIN
static QByteArray protectMachineSecret(const QString &secret) {
    const auto plain = secret.toUtf8();
    DATA_BLOB input{DWORD(plain.size()), reinterpret_cast<BYTE *>(const_cast<char *>(plain.constData()))};
    DATA_BLOB output{};
    if (!CryptProtectData(&input, L"CgPhone SIP", nullptr, nullptr, nullptr,
                          CRYPTPROTECT_LOCAL_MACHINE | CRYPTPROTECT_UI_FORBIDDEN, &output)) return {};
    const QByteArray encrypted(reinterpret_cast<const char *>(output.pbData), int(output.cbData));
    LocalFree(output.pbData);
    return encrypted.toBase64();
}

static QString unprotectMachineSecret(const QByteArray &encoded) {
    const auto encrypted = QByteArray::fromBase64(encoded);
    if (encrypted.isEmpty()) return {};
    DATA_BLOB input{DWORD(encrypted.size()), reinterpret_cast<BYTE *>(const_cast<char *>(encrypted.constData()))};
    DATA_BLOB output{};
    if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &output)) return {};
    const QString secret = QString::fromUtf8(reinterpret_cast<const char *>(output.pbData), int(output.cbData));
    LocalFree(output.pbData);
    return secret;
}
#endif

SipAccountConfig SettingsStore::loadAccount() const {
    auto s = systemSettings();
    SipAccountConfig c;
    c.user = s.value("sip/user").toString();
#ifdef Q_OS_WIN
    c.password = unprotectMachineSecret(s.value("sip/passwordProtected").toByteArray());
    if (c.password.isEmpty()) c.password = s.value("sip/password").toString();
#else
    c.password = s.value("sip/password").toString();
#endif
    c.server = s.value("sip/server").toString();
    c.proxy = s.value("sip/proxy").toString();
    c.proxyEnabled = s.value("sip/proxyEnabled", false).toBool();
    c.logoutCode = s.value("agent/logoutCode", "*02").toString();
    c.alwaysVisible = s.value("behavior/alwaysVisible", false).toBool();
    c.startWithOs = startWithOsEnabled();
    return c;
}

bool SettingsStore::saveAccount(const SipAccountConfig &c) {
    if (!applyStartWithOs(c.startWithOs)) return false;
    auto s = systemSettings();
    s.setValue("sip/user", c.user);
#ifdef Q_OS_WIN
    const auto protectedPassword = protectMachineSecret(c.password);
    if (!protectedPassword.isEmpty()) { s.setValue("sip/passwordProtected", protectedPassword); s.remove("sip/password"); }
#else
    s.setValue("sip/password", c.password);
#endif
    s.setValue("sip/server", c.server); s.setValue("sip/proxy", c.proxy);
    s.setValue("sip/proxyEnabled", c.proxyEnabled);
    s.setValue("agent/logoutCode", c.logoutCode.isEmpty() ? "*02" : c.logoutCode);
    s.setValue("behavior/alwaysVisible", c.alwaysVisible);
    s.setValue("behavior/startWithOs", c.startWithOs);
    s.sync();
    return s.status() == QSettings::NoError;
}

bool SettingsStore::isRunningElevated() {
#ifdef Q_OS_WIN
    return IsUserAnAdmin();
#else
    return geteuid() == 0;
#endif
}
