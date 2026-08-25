#include "RecordingEncoderLocator.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QList>

RecordingEncoderLocator::Result RecordingEncoderLocator::locateLame() {
    struct Candidate { QString path; QString source; };
    const QString windows = qEnvironmentVariable("WINDIR");
    const QString app = QCoreApplication::applicationDirPath();
    const QList<Candidate> candidates = {
        {QDir(app).filePath("codecs/libmp3lame.dll"), "bundled"},
        {QDir(app).filePath("codecs/x64/libmp3lame.dll"), "bundled-x64"},
        {QDir(windows).filePath("System32/libmp3lame.dll"), "system-x64"}
    };

    for (const auto &candidate : candidates) {
        if (!QFileInfo::exists(candidate.path)) continue;
        QLibrary library(candidate.path);
        if (!library.load()) continue;
        const bool nativeApi = library.resolve("lame_init") &&
                               library.resolve("lame_init_params") &&
                               library.resolve("lame_encode_buffer") &&
                               library.resolve("lame_encode_flush") &&
                               library.resolve("lame_close");
        library.unload();
        if (nativeApi) return {QDir::toNativeSeparators(candidate.path), candidate.source, true};
    }
    return {};
}
