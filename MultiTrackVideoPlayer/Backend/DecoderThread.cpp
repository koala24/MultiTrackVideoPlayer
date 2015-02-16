#include "Backend/DecoderThread.h"
#include "Backend/ThreadController.h"
#include "Exception.h"
#include "Enums.h"

#include <QDebug>

DecoderThread::DecoderThread(ThreadInfo *info, SharedThreadData *data) :
    EARLY_WAKE_MS( 1000 ), _mutex() {

    _info = info;
    _shared = data;
    _lastMaxDecodedTimestamp = _maxDecodedTimestamp = 0;
}

DecoderThread::~DecoderThread() {
    this->quit();
    this->wait();
}

void DecoderThread::decode() {
    ListOfVideoImagesPtr listVideoImage;
    try {
        listVideoImage = _info->decoder()->decode( _shared->multiplier() );
        qDebug( "Got ListOfVideoImages with size %i from '%s'",
                listVideoImage->size(),
                qPrintable( _info->decoder()->getVideoTrack()->getFileName() ) );

        if ( _shared->canThreadPutResultsIntoQueue( this ) )
            _shared->pushQueue( listVideoImage );

        if ( listVideoImage->size() > 0 )
            _maxDecodedTimestamp = listVideoImage->back()->getPlaybackTimestamp();

        emit gopDecoded( listVideoImage );
    } catch ( Exception &e ) {
        qDebug() << e.message();
    }

    // wenn ein GOP dekodiert wurde wird ein Semaphore freigeben, jeder
    // Thread soll mindestens einen GOP dekodieren, bevor der Consumer
    // anfaengt
    if ( ++_decodedGops == 1 )
        _shared->bufferSemaphore.release();
}

void DecoderThread::openDecoder() {
    _shared->openMutex->lock();
    try {
        _info->decoder()->open();
    } catch ( Exception &e ) {
        qCritical() << "Opening video file failed. Cause:" << e.message();
        _shared->openMutex->unlock();

        return;
    }
    _shared->openMutex->unlock();
}

void DecoderThread::waitOrNext() {
    if ( shouldWaitForNextSegment() ) {
        while ( shouldWaitForNextSegment() ) {
            _mutex.lock();
            qint64 sleep = _maxDecodedTimestamp - _shared->timing->internalPlayTs() - 1000;
            _info->segmentWaitCondition()->wait( &_mutex, sleep );
            _mutex.unlock();

            if ( _shared->state->getState() == StateEnum::Paused )
                break;
        }
    }
}

void DecoderThread::run() {
    _decodedGops = 0;
    openDecoder();

    qDebug( "Start decoding (jts: %lld, its: %lld) ...",
            _shared->timing->jumpTimestamp(),
            _shared->timing->internalPlayTs() );
    _info->decoder()->jumpToTimestamp( _shared->timing->jumpTimestamp() );

    // --- Dekodierung ---
    if ( _info->isIntermediate() ) {
        decode();
    } else {
        while ( _info->decoder()->hasMore( _shared->multiplier() )
                && (_shared->state->getState() == StateEnum::Playing
                    || _shared->state->getState() == StateEnum::Buffering ) ) {

            decode();
            waitOrNext();
        }
    }

    // fuer den Fall, das kein GOP dekodiert wurde. So wartet der Semaphore
    // nicht unendlich
    if ( _decodedGops == 0 )
        _shared->bufferSemaphore.release();

    emit decodingFinished( this );
}

inline bool DecoderThread::shouldWaitForNextSegment() {
    return (qint64)_shared->timing->internalPlayTs() < _maxDecodedTimestamp - EARLY_WAKE_MS
            && _maxDecodedTimestamp >= (qint64)(_shared->segmentSize - _lastMaxDecodedTimestamp)
            && _shared->state->getState() == StateEnum::Playing
            && !_info->isIntermediate();
}

int DecoderThread::getThreadId() const {
    return _info->threadId();
}
