#pragma once

#include <QString>

class RecordingEncoderLocator {
public:
    struct Result {
        QString encoderPath;
        QString source;
        bool compatible = false;
    };

    static Result locateLame();
};

