#include "Timing.h"

void Timing::reset() {
    _lastTs = _jumpTimestamp = _uiTimestamp = _internalPlayTimestamp = 0;
}

void Timing::start() {
    _sinceStart.start();
}

void Timing::resume() {
    _sinceStart.restart();
    update();
}

void Timing::pause() {
    _sinceStart.restart();
}

void Timing::update() {
    _lastTs = _internalPlayTimestamp;
    int since = _sinceStart.restart();
    _internalPlayTimestamp += since;
    _jumpTimestamp += since;

    if ( _lastTs != _internalPlayTimestamp ) {
        _uiTimestamp = _internalPlayTimestamp / _shared->multiplier();

        emit timestampChanged( _uiTimestamp );
    }
}

void Timing::printDebugOutput() {
    qDebug() << "its:" << _internalPlayTimestamp;
    qDebug() << "uts:" << _uiTimestamp;
    qDebug() << "jts:" << _jumpTimestamp;
}

qint64 Timing::jumpTimestamp() const {
    return qAbs( _internalPlayTimestamp / _shared->multiplier() );
}

qint64 Timing::convertIntoPlaybackTimestamp( qreal timestamp,
                                             bool inverseMultiplier,
                                             bool noDirectionRestriction,
                                             bool calcOnlyMaxTs) {
    qint64 ts = timestamp;
    if ( ( _shared->multiplier() < 0 && !calcOnlyMaxTs ) || noDirectionRestriction )
        ts = _shared->maxTimestamp() - ts;
    return ts * ( inverseMultiplier ? (1 / qAbs( _shared->multiplier() )) : _shared->multiplier() );
}
