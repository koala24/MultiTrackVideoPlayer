#ifndef TIMING_H
#define TIMING_H

#include <QObject>
#include <QTime>

#include "SharedThreadData.h"

class SharedThreadData;
class Timing : public QObject {
    Q_OBJECT

private:
    QTime _sinceStart;
    qint64 _lastTs;
    qint64 _internalPlayTimestamp;
    qint64 _jumpTimestamp;
    int _uiTimestamp;

    SharedThreadData *_shared;

public:
    Timing( SharedThreadData *shared ) {
        _shared = shared;
        reset();
    }

    void reset();
    void start();
    void resume();
    void pause();
    void update();

    void printDebugOutput();

    inline qint64 lastTs() const {
        return _lastTs;
    }

    inline qint64 internalPlayTs() const {
        return _internalPlayTimestamp;
    }

    qint64 jumpTimestamp() const;

    inline int uiTimestamp() const {
        return _uiTimestamp;
    }

    inline void setPlaybackTs( qint64 timestamp ) {
        qDebug() << "=== internal timestamp manipulated to" << timestamp
                 << "last:" << _lastTs;
        _lastTs = _internalPlayTimestamp;
        _internalPlayTimestamp = timestamp;
    }

    qint64 convertIntoPlaybackTimestamp( qreal timestamp,
                                         bool inverseMultiplier = false,
                                         bool noDirectionRestriction = false,
                                         bool calcOnlyMaxTs = false );

signals:
    void timestampChanged( int timestamp );
};

#endif // TIMING_H
