#ifndef ENUMS_H
#define ENUMS_H

#include <QString>

struct StateEnum {
    enum StateValue { Playing, Paused, Stopped, Buffering };

    static QString toString( StateEnum::StateValue value ) {
        switch ( value ) {
            case StateEnum::Playing:
                return "PLAYING";
            case StateEnum::Stopped:
                return "STOPPED";
            case StateEnum::Paused:
                return "PAUSED";
            case StateEnum::Buffering:
                return "BUFFERING";
            default:
                return "unknown state";
        }
    }
};

struct PlayDirection {
    enum Direction { Backward = -1, Forward = 1 };
};

#endif // ENUMS_H
