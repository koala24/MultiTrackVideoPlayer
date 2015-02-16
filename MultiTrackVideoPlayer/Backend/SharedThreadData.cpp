#include "Backend/SharedThreadData.h"
#include "Backend/Timing.h"

#include <QThread>

SharedThreadData::SharedThreadData() :
    bufferSemaphore(),
    stats(),
    _repeat( false ) {

    _repeatMutex = new QMutex();
    _queueMutex = new QMutex();
    _notEmptyCond = new QWaitCondition();
    _queue = new PriorityQueue();
    timing = new Timing( this );

    _lastMultiplier = _multiplier = 1.0;
    _maxTimestamp = 0;
    segmentSize = 5000;

    openMutex = new QMutex();
    state = new State( this );
    threadController = new ThreadController( this );
}

SharedThreadData::~SharedThreadData() {
    delete _repeatMutex;
    delete _queueMutex;
    delete _notEmptyCond;
    delete _queue;
    delete state;
    delete threadController;
    delete timing;
    delete openMutex;
}

PriorityQueue *SharedThreadData::queue() {
    return _queue;
}

QMutex *SharedThreadData::queueMutex() {
    return _queueMutex;
}

bool SharedThreadData::isQueueEmptyWithLock() {
    _queueMutex->lock();
    bool empty = _queue->isEmpty();
    _queueMutex->unlock();
    return empty;
}

void SharedThreadData::pushQueue( ListOfVideoImagesPtr gop ) {
    _queueMutex->lock();
        QElapsedTimer timer;
        timer.start();
        _queue->push( gop );
        stats.addTimeMeasurement( timer.nsecsElapsed() );
    _queueMutex->unlock();
}

VideoImagePtr SharedThreadData::peekQueue() {
    _queueMutex->lock();
    VideoImagePtr vi = _queue->peek();
    _queueMutex->unlock();
    return vi;
}

VideoImagePtr SharedThreadData::popQueue() {
    _queueMutex->lock();
    VideoImagePtr vi = _queue->pop();
    _queueMutex->unlock();
    return vi;
}

void SharedThreadData::clearQueue() {
    _queueMutex->lock();
    _queue->clear();
    _queueMutex->unlock();
}

void SharedThreadData::waitOnNotEmptyCond() {
    qDebug() << "waiting 'notEmptyCond' ...";
    timing->pause();
    _queueMutex->lock();
    _notEmptyCond->wait( _queueMutex );
    _queueMutex->unlock();
    timing->resume();
}

void SharedThreadData::wakeAllNotEmptyCond() {
    qDebug() << "Waking 'notEmptyCond'";
    _notEmptyCond->wakeAll();
}

qreal SharedThreadData::multiplier() {
    return _multiplier;
}

void SharedThreadData::setMultiplier( qreal multiplier ) {
    _lastMultiplier =  _multiplier;
    _multiplier = multiplier;
}

void SharedThreadData::setPlaybackSpeed( qreal speed ) {
    if ( _multiplier < 0 )
        _multiplier = speed * -1.0;
    else
        _multiplier = speed * 1.0;

    _maxPlaybackTimestamp = timing->convertIntoPlaybackTimestamp( _maxTimestamp,
                                                                  false,
                                                                  false,
                                                                  true );
}

/**
 * @brief Prueft, ob sich der Multiplier nach der letzten Abfrage dieser
 *        Methode geaendert hat.
 * @return
 */
bool SharedThreadData::wasMultiplierChangedAfterLastRequest() {
    qDebug() << _lastMultiplier << " --- " << _multiplier;
    bool result = _lastMultiplier != _multiplier;
    _lastMultiplier = _multiplier;
    return result;
}

void SharedThreadData::resetBufferSemaphore() {
    bufferSemaphore.acquire( bufferSemaphore.available() );
}

int SharedThreadData::numberOfTracks() {
    return threadController->numberOfThreads();
}

bool SharedThreadData::shouldThreadDecodeOnlyOneGop( DecoderThread *thread ) const {
    return threadController->isThreadIntermediate( thread );
}

bool SharedThreadData::canThreadPutResultsIntoQueue( DecoderThread *thread ) const {
    return threadController->canPutResultsIntoQueue( thread );
}

bool SharedThreadData::repeat() const {
    QMutexLocker locker( _repeatMutex );
    return _repeat;
}

void SharedThreadData::setRepeat( bool repeat ) {
    QMutexLocker locker( _repeatMutex );
    _repeat = repeat;
}


qint64 SharedThreadData::maxTimestamp() const {
    return _maxTimestamp;
}

void SharedThreadData::setMaxTimestamp( const qint64 &maxTimestamp ) {
    _maxTimestamp = maxTimestamp;
    _maxPlaybackTimestamp = timing->convertIntoPlaybackTimestamp( maxTimestamp,
                                                                  false,
                                                                  false,
                                                                  true );
}

qint64 SharedThreadData::maxPlaybackTimestamp() const {
    return _maxPlaybackTimestamp;
}
