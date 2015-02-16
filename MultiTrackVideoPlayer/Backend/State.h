#ifndef STATE_H
#define STATE_H

#include <QObject>

#include "Backend/StateHistory.h"
#include "Backend/SharedThreadData.h"
#include "Enums.h"

class SharedThreadData;

class State : public QObject {
    Q_OBJECT
public:
    explicit State( SharedThreadData *shared );
    ~State();

    void changeToPlaying();
    void changeToPaused();
    void changeToStopped();
    void changeToBuffering();

    StateEnum::StateValue getState();
    StateEnum::StateValue getPreviousState();

signals:
    void running();
    void paused();
    void stopped();
    void buffering();

private:
    StateHistory _history;
    SharedThreadData *_shared;
    QMutex _mutex;

    void printStateChanged();
};

#endif // STATE_H
