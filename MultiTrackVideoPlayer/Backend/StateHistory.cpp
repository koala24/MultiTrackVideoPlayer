#include "StateHistory.h"

StateHistory::StateHistory() : _historyList(), _mutex() {
}

void StateHistory::add( const StateEnum::StateValue &value ) {
    _mutex.lock();
    _historyList.prepend( value );

    if ( _historyList.size() > 5 )
        _historyList.removeLast();
    _mutex.unlock();
}

StateEnum::StateValue StateHistory::current() {
    _mutex.lock();
    StateEnum::StateValue val = StateEnum::Stopped;
    if ( _historyList.size() != 0 )
        val = _historyList.first();
    _mutex.unlock();

    return val;
}

StateEnum::StateValue StateHistory::previous() {
    _mutex.lock();
    StateEnum::StateValue val = StateEnum::Stopped;
    if ( _historyList.size() >= 2 )
        val = _historyList.at( 1 );
    _mutex.unlock();

    return val;
}
