#pragma once

#include <QObject>
#include <QTimer>

class SystemAudioController final : public QObject {
    Q_OBJECT
    Q_PROPERTY(int outputVolume READ outputVolume WRITE setOutputVolume NOTIFY outputVolumeChanged)
    Q_PROPERTY(int microphoneVolume READ microphoneVolume WRITE setMicrophoneVolume NOTIFY microphoneVolumeChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
public:
    explicit SystemAudioController(QObject *parent = nullptr);
    ~SystemAudioController() override;

    int outputVolume() const { return m_outputVolume; }
    int microphoneVolume() const { return m_microphoneVolume; }
    bool available() const { return m_available; }

    Q_INVOKABLE void setOutputVolume(int value);
    Q_INVOKABLE void setMicrophoneVolume(int value);
    Q_INVOKABLE void refresh();

signals:
    void outputVolumeChanged();
    void microphoneVolumeChanged();
    void availableChanged();

private:
    int queryVolume(bool capture, bool *ok) const;
    bool applyVolume(bool capture, int value) const;
    int m_outputVolume = 50;
    int m_microphoneVolume = 50;
    bool m_available = false;
    bool m_comInitialized = false;
    QTimer m_refreshTimer;
};

