#include "Mp3Encoder.h"
#include "RecordingEncoderLocator.h"
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <cstring>
#include <QLibrary>
#include <QVector>

namespace {
using LameHandle = void *;
using LameInit = LameHandle (*)();
using LameClose = int (*)(LameHandle);
using LameSetInt = int (*)(LameHandle, int);
using LameInitParams = int (*)(LameHandle);
using LameEncodeBuffer = int (*)(LameHandle, const short *, const short *, int, unsigned char *, int);
using LameEncodeInterleaved = int (*)(LameHandle, short *, int, unsigned char *, int);
using LameFlush = int (*)(LameHandle, unsigned char *, int);

struct WavData {
    int channels = 0;
    int sampleRate = 0;
    QVector<short> samples;
};

bool readWav(const QString &path, WavData &wav, QString &error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) { error = QStringLiteral("No se pudo abrir el WAV"); return false; }
    QDataStream in(&file); in.setByteOrder(QDataStream::LittleEndian);
    if (file.read(4) != "RIFF") { error = QStringLiteral("El archivo no es RIFF/WAV"); return false; }
    quint32 riffSize = 0; in >> riffSize;
    Q_UNUSED(riffSize);
    if (file.read(4) != "WAVE") { error = QStringLiteral("El archivo no es WAVE"); return false; }

    quint16 format = 0, channels = 0, bits = 0;
    quint32 sampleRate = 0;
    QByteArray pcm;
    while (!in.atEnd()) {
        const QByteArray id = file.read(4);
        if (id.size() != 4) break;
        quint32 size = 0; in >> size;
        if (id == "fmt ") {
            in >> format >> channels >> sampleRate;
            quint32 byteRate; quint16 blockAlign;
            in >> byteRate >> blockAlign >> bits;
            Q_UNUSED(byteRate); Q_UNUSED(blockAlign);
            if (size > 16) file.seek(file.pos() + size - 16);
        } else if (id == "data") {
            pcm = file.read(size);
        } else {
            file.seek(file.pos() + size);
        }
        if (size & 1U) file.seek(file.pos() + 1);
    }
    if (format != 1 || bits != 16 || (channels != 1 && channels != 2) || sampleRate == 0 || pcm.isEmpty()) {
        error = QStringLiteral("LAME requiere WAV PCM de 16 bits, mono o estéreo");
        return false;
    }
    wav.channels = channels; wav.sampleRate = int(sampleRate);
    wav.samples.resize(pcm.size() / int(sizeof(short)));
    memcpy(wav.samples.data(), pcm.constData(), size_t(pcm.size()));
    return true;
}

template<typename T> T symbol(QLibrary &library, const char *name) {
    return reinterpret_cast<T>(library.resolve(name));
}
}

Mp3Encoder::Result Mp3Encoder::encodeWav(const QString &wavPath, const QString &mp3Path) {
    Result result;
    const auto located = RecordingEncoderLocator::locateLame();
    if (!located.compatible) {
        result.error = QStringLiteral("No se encontró libmp3lame.dll x64; se conservó el WAV");
        return result;
    }
    result.encoderPath = located.encoderPath;
    QLibrary library(located.encoderPath);
    if (!library.load()) { result.error = library.errorString(); return result; }

    const auto lameInit = symbol<LameInit>(library, "lame_init");
    const auto lameClose = symbol<LameClose>(library, "lame_close");
    const auto setRate = symbol<LameSetInt>(library, "lame_set_in_samplerate");
    const auto setChannels = symbol<LameSetInt>(library, "lame_set_num_channels");
    const auto setBitrate = symbol<LameSetInt>(library, "lame_set_brate");
    const auto setMode = symbol<LameSetInt>(library, "lame_set_mode");
    const auto initParams = symbol<LameInitParams>(library, "lame_init_params");
    const auto encodeMono = symbol<LameEncodeBuffer>(library, "lame_encode_buffer");
    const auto encodeStereo = symbol<LameEncodeInterleaved>(library, "lame_encode_buffer_interleaved");
    const auto flush = symbol<LameFlush>(library, "lame_encode_flush");
    if (!lameInit || !lameClose || !setRate || !setChannels || !setBitrate || !setMode ||
        !initParams || !encodeMono || !encodeStereo || !flush) {
        result.error = QStringLiteral("La DLL no expone la API nativa completa de LAME");
        return result;
    }

    WavData wav;
    if (!readWav(wavPath, wav, result.error)) return result;
    QFile output(mp3Path);
    if (!output.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        result.error = QStringLiteral("No se pudo crear el MP3");
        return result;
    }

    LameHandle lame = lameInit();
    if (!lame) { result.error = QStringLiteral("LAME no pudo inicializarse"); return result; }
    const auto cleanup = [&] { lameClose(lame); };
    if (setRate(lame, wav.sampleRate) < 0 || setChannels(lame, wav.channels) < 0 ||
        setBitrate(lame, 32) < 0 || setMode(lame, wav.channels == 1 ? 3 : 0) < 0 ||
        initParams(lame) < 0) {
        cleanup(); output.remove(); result.error = QStringLiteral("Parámetros MP3 rechazados por LAME"); return result;
    }

    constexpr int framesPerChunk = 8192;
    QByteArray encoded(65536, Qt::Uninitialized);
    const int totalFrames = wav.samples.size() / wav.channels;
    int offset = 0;
    QVector<short> left(framesPerChunk), right(framesPerChunk);
    while (offset < totalFrames) {
        const int frames = qMin(framesPerChunk, totalFrames - offset);
        int bytes = 0;
        if (wav.channels == 1) {
            const short *samples = wav.samples.constData() + offset;
            bytes = encodeMono(lame, samples, samples, frames,
                               reinterpret_cast<unsigned char *>(encoded.data()), encoded.size());
        } else {
            short *interleaved = wav.samples.data() + offset * 2;
            bytes = encodeStereo(lame, interleaved, frames,
                                 reinterpret_cast<unsigned char *>(encoded.data()), encoded.size());
        }
        if (bytes < 0 || output.write(encoded.constData(), bytes) != bytes) {
            cleanup(); output.close(); output.remove();
            result.error = QStringLiteral("Falló la codificación MP3");
            return result;
        }
        offset += frames;
    }
    const int finalBytes = flush(lame, reinterpret_cast<unsigned char *>(encoded.data()), encoded.size());
    cleanup();
    if (finalBytes < 0 || output.write(encoded.constData(), finalBytes) != finalBytes) {
        output.close(); output.remove(); result.error = QStringLiteral("Falló el cierre del MP3"); return result;
    }
    output.close();
    if (QFileInfo(mp3Path).size() <= 0) { QFile::remove(mp3Path); result.error = QStringLiteral("LAME produjo un archivo vacío"); return result; }
    result.success = true;
    return result;
}
