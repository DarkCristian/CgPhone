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
        {QDir(windows).filePath("System32/lame_enc.dll"), "system"},
        {QDir(windows).filePath("SysWOW64/lame_enc.dll"), "system-x86"},
        {QDir(app).filePath("codecs/lame_enc.dll"), "bundled"},
        {QDir(app).filePath("codecs/x64/lame_enc.dll"), "bundled-x64"},
        {QDir(app).filePath("codecs/x86/lame_enc.dll"), "bundled-x86"}
    };

    for (const auto &candidate : candidates) {
        if (!QFileInfo::exists(candidate.path)) continue;
        QLibrary library(candidate.path);
        if (!library.load()) continue; // Also rejects an incompatible architecture.
        library.unload();
        return {candidate.path, candidate.source, true};
    }

    return {};
}
 
