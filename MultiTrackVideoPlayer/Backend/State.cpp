#include "Backend/State.h"
#include "Backend/Timing.h"

#include <QThread>

State::State( SharedThreadData *shared ) :
    _history(),
    _mutex() {

    _shared = shared;
}

State::~State() {
}

void State::changeToPlaying() {
    if ( _history.current() == StateEnum::Playing )
        return;

    _mutex.lock();
        _history.add( StateEnum::Playing );
        printStateChanged();

        if ( _history.previous() == StateEnum::Paused ) {
            qDebug() << "Resuming at its:" << _shared->timing->internalPlayTs()
                     << "jts:" << _shared->timing->jumpTimestamp();
            _shared->wakeAllNotEmptyCond();
            _shared->threadController->wakeConsumerThread();
            _shared->timing->resume();
        }

        if ( _history.previous() != StateEnum::Buffering )
            _shared->threadController->startAllThreads();

        emit running();
    _mutex.unlock();
}

void State::changeToPaused() {
    _mutex.lock();
        _history.add( StateEnum::Paused );
        printStateChanged();
        qDebug() << "Paused at its:" << _shared->timing->internalPlayTs()
                 << "jts:" << _shared->timing->jumpTimestamp();

        _shared->threadController->wakeAllDecoderThreads();
        _shared->timing->pause();
        _shared->clearQueue();

        emit paused();
    _mutex.unlock();

    // Es muss außerhalb stehen, da sonst ein Deadlock auftritt
    _shared->threadController->stopAllDecoderThreads();
}

void State::changeToStopped() {
    _mutex.lock();
        _history.add( StateEnum::Stopped );
        printStateChanged();

        _shared->resetBufferSemaphore();

        _shared->threadController->wakeConsumerThread();
        _shared->wakeAllNotEmptyCond();
        _shared->clearQueue();
        _shared->timing->reset();

        _shared->stats.printStats();

        emit stopped();
    _mutex.unlock();
    _shared->threadController->stopAllThreads();
}

void State::changeToBuffering() {
    _mutex.lock();
        _history.add( StateEnum::Buffering );
        printStateChanged();
        _shared->timing->pause();

        emit buffering();
    _mutex.unlock();
}

StateEnum::StateValue State::getState() {
    return _history.current();
}

StateEnum::StateValue State::getPreviousState() {
    return _history.previous();
}

void State::printStateChanged() {
    qDebug() << "State changed from"
             << StateEnum::toString( _history.previous() )
             << "to"
             << StateEnum::toString( _history.current() );
}
