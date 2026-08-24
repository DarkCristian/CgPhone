#pragma once

#include <QString>

class Mp3Encoder {
public:
    struct Result {
        bool success = false;
        QString error;
        QString encoderPath;
    };

    static Result encodeWav(const QString &wavPath, const QString &mp3Path);
};
