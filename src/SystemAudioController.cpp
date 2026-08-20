#include "SystemAudioController.h"

#include <QProcess>
#include <QRegularExpression>
#include <QtGlobal>

#ifdef Q_OS_WIN
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <objbase.h>

static IAudioEndpointVolume *openEndpoint(bool capture) {
    IMMDeviceEnumerator *enumerator = nullptr;
    IMMDevice *device = nullptr;
    IAudioEndpointVolume *volume = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&enumerator)))) return nullptr;
    HRESULT endpointResult = enumerator->GetDefaultAudioEndpoint(capture ? eCapture : eRender, eCommunications, &device);
    if (FAILED(endpointResult)) endpointResult = enumerator->GetDefaultAudioEndpoint(capture ? eCapture : eRender, eConsole, &device);
    if (SUCCEEDED(endpointResult))
        device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, reinterpret_cast<void **>(&volume));
    if (device) device->Release();
    enumerator->Release();
    return volume;
}
#endif

SystemAudioController::SystemAudioController(QObject *parent) : QObject(parent) {
#ifdef Q_OS_WIN
    const HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    m_comInitialized = result == S_OK || result == S_FALSE;
#endif
    connect(&m_refreshTimer, &QTimer::timeout, this, &SystemAudioController::refresh);
    m_refreshTimer.start(2000);
    refresh();
}

SystemAudioController::~SystemAudioController() {
#ifdef Q_OS_WIN
    if (m_comInitialized) CoUninitialize();
#endif
}

int SystemAudioController::queryVolume(bool capture, bool *ok) const {
    *ok = false;
#ifdef Q_OS_WIN
    if (auto *endpoint = openEndpoint(capture)) {
        float scalar = 0.0f;
        const HRESULT result = endpoint->GetMasterVolumeLevelScalar(&scalar);
        endpoint->Release();
        if (SUCCEEDED(result)) { *ok = true; return qBound(0, qRound(scalar * 100.0f), 100); }
    }
#elif defined(Q_OS_LINUX)
    QProcess process;
    const QString target = capture ? QStringLiteral("@DEFAULT_SOURCE@") : QStringLiteral("@DEFAULT_SINK@");
    process.start(QStringLiteral("pactl"), {capture ? QStringLiteral("get-source-volume") : QStringLiteral("get-sink-volume"), target});
    if (process.waitForFinished(1200)) {
        const auto match = QRegularExpression(QStringLiteral("(\\d+)%")).match(QString::fromUtf8(process.readAllStandardOutput()));
        if (match.hasMatch()) { *ok = true; return qBound(0, match.captured(1).toInt(), 100); }
    }
#else
    Q_UNUSED(capture)
#endif
    return 0;
}

bool SystemAudioController::applyVolume(bool capture, int value) const {
    value = qBound(0, value, 100);
#ifdef Q_OS_WIN
    if (auto *endpoint = openEndpoint(capture)) {
        const HRESULT result = endpoint->SetMasterVolumeLevelScalar(float(value) / 100.0f, nullptr);
        endpoint->Release();
        return SUCCEEDED(result);
    }
#elif defined(Q_OS_LINUX)
    const QString target = capture ? QStringLiteral("@DEFAULT_SOURCE@") : QStringLiteral("@DEFAULT_SINK@");
    return QProcess::execute(QStringLiteral("pactl"), {capture ? QStringLiteral("set-source-volume") : QStringLiteral("set-sink-volume"), target, QString::number(value) + '%'}) == 0;
#else
    Q_UNUSED(capture)
#endif
    return false;
}

void SystemAudioController::setOutputVolume(int value) {
    value = qBound(0, value, 100);
    if (!applyVolume(false, value)) return;
    if (m_outputVolume != value) { m_outputVolume = value; emit outputVolumeChanged(); }
}

void SystemAudioController::setMicrophoneVolume(int value) {
    value = qBound(0, value, 100);
    if (!applyVolume(true, value)) return;
    if (m_microphoneVolume != value) { m_microphoneVolume = value; emit microphoneVolumeChanged(); }
}

void SystemAudioController::refresh() {
    bool outputOk = false, microphoneOk = false;
    const int output = queryVolume(false, &outputOk);
    const int microphone = queryVolume(true, &microphoneOk);
    const bool nowAvailable = outputOk && microphoneOk;
    if (outputOk && output != m_outputVolume) { m_outputVolume = output; emit outputVolumeChanged(); }
    if (microphoneOk && microphone != m_microphoneVolume) { m_microphoneVolume = microphone; emit microphoneVolumeChanged(); }
    if (nowAvailable != m_available) { m_available = nowAvailable; emit availableChanged(); }
}
