#include "ThreadController.h"
#include "Decoder/DecoderFactory.h"

#include <QCoreApplication>

ThreadController::ThreadController(SharedThreadData *shared) :
    _threads(),
    _runningThreads(),
    _lastUsedThreadId( -1 ),
    _consumerSleepCond(),
    _consumerSleepMutex()
{
    _shared = shared;
    _shared->_decodeOnlyOneGop = false;
    _consumerThread = new VideoImageConsumer( _shared,
                                              &_consumerSleepCond,
                                              &_consumerSleepMutex );
}

ThreadController::~ThreadController() {
    foreach (DecoderThread *thread, _threads.keys())
        delete thread;
    qDeleteAll( _threads );
    delete _consumerThread;
}

DecoderThread* ThreadController::addDecoderThread( VideoTrack *track ) {
    IDecoder *decoder = DecoderFactory::buildDecoder( track );
    if ( decoder == 0 )
        return 0;

    ThreadInfo *info = new ThreadInfo( ++_lastUsedThreadId, decoder );
    DecoderThread *thread = new DecoderThread( info, _shared );
    _threads.insert( thread, info );

    connect( thread, SIGNAL(decodingFinished(DecoderThread*)),
             this, SLOT(onThreadHasDecodingFinished(DecoderThread*)) );
    return thread;
}

bool ThreadController::startThread( DecoderThread *thread ) {
    ThreadInfo *info = _threads.value( thread );
    if ( info->_isRunning ) {
        qDebug() << "Thread with id" << info->_threadId << "currently running, waking ...";
        info->segmentWaitCondition()->wakeAll();
    } else {
        _runningThreads.append( thread );
        thread->start();
        if ( thread->isRunning() ) {
            info->_isRunning = true;
            return true;
        }
    }
    return false;
}

void ThreadController::startAllThreads( qint64 decodeToTimestamp ) {
    _decodeToTimestamp = decodeToTimestamp;
    qDebug() << "Starting consumer thread ...";
    _consumerThread->start();

    qDebug() << "Starting all decoder threads ...";
    foreach ( DecoderThread *thread, _threads.keys() )
        startThread( thread );
}

void ThreadController::stopDecoderThread( DecoderThread *thread ) {
    ThreadInfo *info = _threads.value( thread );
    info->_isRunning = false;
    wakeDecoderThread( thread );
    thread->quit();
    thread->wait();
}

void ThreadController::stopAllThreads() {
    qDebug() << "Stopping all decoder threads ...";
    stopAllDecoderThreads();

    qDebug() << "Stopping consumer thread ...";
    _consumerThread->quit();
    // sollte der Aufruf vom Consumer Thread gekommen sein, so brauch er nicht
    // auf sich selbst zu warten
    if ( _consumerThread->currentThread() != QThread::currentThread() )
        _consumerThread->wait();
}

void ThreadController::stopAllDecoderThreads() {
    foreach ( DecoderThread *thread, _threads.keys() )
        stopDecoderThread( thread );
}

int ThreadController::numberOfThreads() {
    return _threads.size();
}

qint64 ThreadController::decodeToTimestamp() {
    return _decodeToTimestamp;
}

void ThreadController::setDecodeOnlyOneGop( bool onlyOne ) {
    _shared->_decodeOnlyOneGop = onlyOne;
}

bool ThreadController::canPutResultsIntoQueue( DecoderThread *thread ) {
    return _threads.value( thread )->_canPutResultsIntoQueue;
}

bool ThreadController::isThreadIntermediate( DecoderThread *thread ) {
    return _threads.value( thread )->_isIntermediate;
}

void ThreadController::setThreadAsIntermediate( DecoderThread *thread, bool putResultInQueue ) {
    ThreadInfo *info = _threads.value( thread );
    info->_isIntermediate = true;
    info->_canPutResultsIntoQueue = putResultInQueue;
}

VideoImageConsumer* ThreadController::getConsumerThread() {
    return _consumerThread;
}

void ThreadController::wakeConsumerThread() {
    _consumerSleepCond.wakeAll();
}

void ThreadController::wakeAllDecoderThreads() {
    foreach ( DecoderThread *thread, _threads.keys() )
        wakeDecoderThread( thread );
}

void ThreadController::startIntermediateDecoding() {
    foreach ( DecoderThread *thread, _threads.keys() )
        startIntermediateDecoding( thread );
}

void ThreadController::startIntermediateDecoding( DecoderThread *thread ) {
    stopDecoderThread( thread );

    setThreadAsIntermediate( thread );
    setDecodeOnlyOneGop( true );

    startThread( thread );
}

void ThreadController::waitForFinishing() {
    // Irgendwie nicht wirklich schön, nur leider kommt das decodingFinished
    // Signal manchmal ein paar Millisekunden zu spät an, wodurch die Zustands-
    // Infos nicht rechtzeitig aktualisiert werden und es dadurch zu einem
    // Endlosbuffern kommen kann
    while( _runningThreads.size() > 0 )
        QCoreApplication::processEvents( QEventLoop::AllEvents, 5 );
}

void ThreadController::wakeDecoderThread( DecoderThread *thread ) {
    ThreadInfo *info = _threads.value( thread );
    info->segmentWaitCondition()->wakeAll();
}

void ThreadController::onThreadHasDecodingFinished( DecoderThread *sender ) {
    ThreadInfo *info = _threads.value( sender );
    qDebug().nospace() << "Thread with id " << sender->getThreadId()
                       << " has finished."
                       << (info->isIntermediate() ? " (marked as inter)" : "");


    info->_isIntermediate = false;
    info->_isRunning = false;
    info->_canPutResultsIntoQueue = true;

    _runningThreads.removeOne( sender );
}
