#ifndef VIDEOIMAGECONSUMER_H
#define VIDEOIMAGECONSUMER_H

#include <QMutex>
#include <QThread>
#include <QTime>
#include <QWaitCondition>

#include "Backend/SharedThreadData.h"
#include "Backend/Timing.h"
#include "Backend/State.h"
#include "VideoData/videoimage.h"

class SharedThreadData;
class State;
class Timing;

class VideoImageConsumer : public QThread
{
    Q_OBJECT

    void run();

public:
    VideoImageConsumer(SharedThreadData *shared,
                       QWaitCondition *sleepCond,
                       QMutex *sleepMutex);
    ~VideoImageConsumer();

    inline void hurryUp( bool display = true );
    inline int process();

private:
    SharedThreadData *_shared;
    Timing *_timing;
    State *_state;

    int _sleepTime;
    bool _debugOutOfSync;
    bool _debugDisplay;
    bool _debugSleep;
    bool _terminate;
    bool _queueWasEmpty;

    const int PADDING_MSEC;

    QMutex *_sleepMutex;
    QWaitCondition *_sleepCond;

    void handleEmptyQueue();
    void sendDisplayImage( VideoImagePtr vi );
    void changeStateToBuffering();
    void waitForAtLeastOneGopFromEveryThread();
    void waitOnSleepCond( quint64 ms = ULONG_LONG_MAX );

    void preBufferGop( bool startup );

    inline bool isFinished();

    inline void goodNightConsumer();
    inline void displayFrames();
    inline void displayOutOfSyncFrames();

signals:
    void displayImage( VideoImagePtr image );

private slots:
    void onNotEmpty();

};

#endif // VIDEOIMAGECONSUMER_H
