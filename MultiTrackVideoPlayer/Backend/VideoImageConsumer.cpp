#include "Backend/VideoImageConsumer.h"
#include "Backend/Timing.h"

#include <QApplication>
#include <QStringList>

VideoImageConsumer::VideoImageConsumer( SharedThreadData *shared,
                                        QWaitCondition *sleepCond,
                                        QMutex *sleepMutex )
    : _sleepTime(-1), PADDING_MSEC( 10 ) {

    _debugDisplay = qApp->arguments().contains( "--display-debug" );
    _debugOutOfSync = qApp->arguments().contains( "--sync-debug" );
    _debugSleep = qApp->arguments().contains( "--sleep-debug" );

    _sleepCond = sleepCond;
    _sleepMutex = sleepMutex;

    _terminate = false;

    _shared = shared;
    _timing = _shared->timing;
    _state = _shared->state;
    connect( _shared->queue(), SIGNAL(nonEmpty()), this, SLOT(onNotEmpty()) );
}

VideoImageConsumer::~VideoImageConsumer() {
    quit();
    wait();
}

inline void VideoImageConsumer::hurryUp( bool display ) {
    _shared->queueMutex()->lock();
    while ( _shared->queue()->size() > 0 ) {
        if ( _shared->queue()->peek()->getPlaybackTimestamp() >= _timing->internalPlayTs() )
            break;

        if ( display ) {
            VideoImagePtr vi = _shared->queue()->pop();
            sendDisplayImage( vi );

            _shared->stats.outOfSync++;
            if ( _debugOutOfSync ) {
                qDebug( "(-) out of sync; its: %lld %s",
                        _timing->internalPlayTs(), qPrintable( vi->toString() ) );
            }
        } else {
            // Aus der Queue nehmen
            _shared->queue()->pop();
        }

        _timing->update();
        if ( _shared->queue()->isEmpty() )
            break;
    }
    _shared->queueMutex()->unlock();
}

void VideoImageConsumer::onNotEmpty() {
    _shared->wakeAllNotEmptyCond();
}

inline void VideoImageConsumer::sendDisplayImage( VideoImagePtr vi ) {
    emit displayImage( vi );

    _shared->stats.incDisplayedFrames();

    if ( _debugDisplay ) {
        qDebug( "displaying; its: %lld %s",
                _timing->internalPlayTs(), qPrintable( vi->toString() ) );
    }
}

inline bool VideoImageConsumer::isFinished() {
    qDebug() << "--- isFinished() ---";
    qDebug() << "**Max pts:" << _shared->maxPlaybackTimestamp();
    qDebug() << "Max ts:" << _shared->maxTimestamp();
    qDebug() << "pts" << _timing->internalPlayTs();
    qDebug() << "jts" << _timing->jumpTimestamp();
    qDebug() << "uits" << _timing->uiTimestamp();
    qDebug() << "--------------------------";

    return (_shared->multiplier() >= 0 && _timing->internalPlayTs() >= _shared->maxPlaybackTimestamp())
            || (_shared->multiplier() < 0 && _timing->internalPlayTs() >= 0 );
}

inline void VideoImageConsumer::handleEmptyQueue() {
    if ( isFinished() ) {
        // für den Fall das die UI nicht gestoppt hat
        if ( _state->getState() != StateEnum::Stopped ) {
            qDebug() << "[[[STOP1]]]";
            if ( !_shared->repeat() )
                _terminate = true;
            _state->changeToStopped();
        }
    } else if ( _state->getState() != StateEnum::Stopped ) {
        _state->changeToBuffering();
        _shared->stats.incBufferedCount();
        _shared->waitOnNotEmptyCond();
    }
}

void VideoImageConsumer::changeStateToBuffering() {
    _state->changeToBuffering();
    _shared->stats.incBufferedCount();
}

// Darauf warten, dass alle Threads mindestens ein GOP dekodiert haben
void VideoImageConsumer::waitForAtLeastOneGopFromEveryThread() {
    qDebug() << "@@@ Before semaphore acquire, sema available:" << _shared->bufferSemaphore.available();
    _shared->bufferSemaphore.acquire( _shared->numberOfTracks() );
    qDebug() << "@@@ Semaphore acquired, continuing ...";
}

void VideoImageConsumer::waitOnSleepCond( quint64 ms ) {
    _sleepMutex->lock();
    _sleepCond->wait( _sleepMutex, ms );
    _sleepMutex->unlock();
}

inline void VideoImageConsumer::goodNightConsumer() {
    if ( _debugSleep )
        qDebug() << "sleeping" << _sleepTime << "ms"
                 << "its:" << _timing->internalPlayTs();

    if ( _sleepTime > 1000 ) {
        qDebug() << "Sleep time over 1000ms:";
        qDebug() << "\t its = " << _timing->internalPlayTs();

        if ( _shared->queue()->size() == 0 ) {
            qDebug() << "\t nothing in queue";
        } else {
            VideoImagePtr vi = _shared->peekQueue();
            qDebug( "\t first item in queue: %s", qPrintable( vi->toString() ) );
        }
    }

    waitOnSleepCond( _sleepTime );
}

inline void VideoImageConsumer::displayFrames() {
    _timing->update();
    // alles anzeigen, was mit dem internen Zeitstempel uebereinstimmt
    while ( !_queueWasEmpty && _shared->peekQueue()->getPlaybackTimestamp() == _shared->timing->internalPlayTs() ) {
        VideoImagePtr vi = _shared->popQueue();
        sendDisplayImage( vi );

        if ( _shared->isQueueEmptyWithLock() ) {
            handleEmptyQueue();
            _queueWasEmpty = true;
        }
    }
}

inline void VideoImageConsumer::displayOutOfSyncFrames() {
    _timing->update();
    if ( !_queueWasEmpty ) {
        // alles anzeigen, was zu spät dran ist
        if ( _shared->peekQueue()->getPlaybackTimestamp() < _timing->internalPlayTs() )
            hurryUp();

        _timing->update();
        if ( !_shared->isQueueEmptyWithLock() )
            _sleepTime = _shared->peekQueue()->getPlaybackTimestamp() - _timing->internalPlayTs() - PADDING_MSEC;
    }
}

void VideoImageConsumer::preBufferGop( bool startup ) {
    changeStateToBuffering();
    waitForAtLeastOneGopFromEveryThread();

    if ( startup )
        _timing->start();

    _timing->update();

    // Vor dem Start alles Löschen, was hinter dem internen Zeitstempel ist.
    // Die Decoder springen nämlich ein Bild vor den angegebenen Zeitstempel,
    // damit bei einem Sprung die richtigen Bilder angezeigt werden. Dieses
    // Bild wurde in der UI auch schon angezeigt. Also brauchen wir es nicht
    // nochmal anzeigen und dadurch ein "out of sync" auslösen.
    hurryUp( false );
}

void VideoImageConsumer::run() {
    _terminate = false;
    _queueWasEmpty = false;
    preBufferGop( true );

    while ( _state->getState() != StateEnum::Stopped ) {
        //_timing->printDebugOutput();
        if ( _state->getState() == StateEnum::Paused ) {
            if ( _state->getState() == StateEnum::Paused ) {
                waitOnSleepCond();

                // Abspielen ist zwischendrinnen gestoppt worden
                if ( _state->getState() == StateEnum::Stopped ) {
                    _terminate = true;
                    break;
                }
            }

            if ( !_terminate )
                preBufferGop( false );
        }

        if ( _terminate ) break;

        _state->changeToPlaying();
        _sleepTime = -1;

        if ( _shared->isQueueEmptyWithLock() ) {
            handleEmptyQueue();
            _queueWasEmpty = true;
        } else {
            displayFrames();
            displayOutOfSyncFrames();
        }

        if ( _sleepTime > 0 )
            goodNightConsumer();

        _queueWasEmpty = false;
    }
}
