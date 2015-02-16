#ifndef STATEHISTORY_H
#define STATEHISTORY_H

#include <QList>
#include <QMutex>

#include "Enums.h"

class StateHistory
{
public:
    StateHistory();

    void add( const StateEnum::StateValue &value );

    StateEnum::StateValue current();
    StateEnum::StateValue previous();

private:
    QList<StateEnum::StateValue> _historyList;
    QMutex _mutex;
};

#endif // STATEHISTORY_H
