#ifndef SHAREDTHREADDATA_H
#define SHAREDTHREADDATA_H

#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QTime>
#include <QElapsedTimer>
#include <QSemaphore>

#include "VideoImageConsumer.h"
#include "DecoderThread.h"
#include "ThreadController.h"
#include "Backend/PriorityQueue.h"
#include "Backend/State.h"
#include "Backend/Timing.h"

class State;
class DecoderThread;
class VideoImageConsumer;
class ThreadController;
class Timing;

struct Statistics {
    int outOfSync;
    quint64 displayedFrames;
    quint64 buffered;
    QList<qint64> timeList;

    QMutex *mutex;

    Statistics() {
        reset();
        mutex = new QMutex();
    }

    ~Statistics() {
        delete mutex;
    }

    inline void reset() {
        outOfSync = 0;
        displayedFrames = 0;
        buffered = 0;
        timeList.clear();
    }

    inline void addTimeMeasurement( int time ) {
        timeList.append( time );
    }

    inline void incDisplayedFrames() {
        mutex->lock();
        ++displayedFrames;
        mutex->unlock();
    }

    inline void incBufferedCount() {
        mutex->lock();
        ++buffered;
        mutex->unlock();
    }

    void printStats() {
        qDebug();
        qDebug() << "Stats";
        qDebug() << "-----";
        qDebug() << "sync failed:" << outOfSync;
        qDebug() << "buffered;" << buffered;
        qDebug() << "displayed frames:" << displayedFrames;

        quint64 sum = 0;
        foreach ( qint64 i, timeList )
            sum += i;
        qDebug( "avg push time: %0.4fms", (((qreal)sum / timeList.size()) / 1000000) );
        qDebug();
    }
};

class SharedThreadData
{
    friend class ThreadController;
    friend class PlaybackController;

public:
    SharedThreadData();
    ~SharedThreadData();

    /* public fields */
    quint64 segmentSize;
    QSemaphore bufferSemaphore;
    QMutex *openMutex;

    Timing *timing;
    Statistics stats;
    State *state;
    ThreadController *threadController;

    /* Methods */
    PriorityQueue* queue();
    QMutex *queueMutex();

    /* Thread-safe Methods */
    bool isQueueEmptyWithLock();
    void pushQueue( ListOfVideoImagesPtr gop );
    VideoImagePtr peekQueue();
    VideoImagePtr popQueue();
    void clearQueue();

    void waitOnNotEmptyCond();
    void wakeAllNotEmptyCond();

    qreal multiplier();
    void setMultiplier( qreal multiplier );
    void setPlaybackSpeed( qreal speed );

    qint64 maxTimestamp() const;
    qint64 maxPlaybackTimestamp() const;
    void setMaxTimestamp(const qint64 &maxTimestamp);

    bool wasMultiplierChangedAfterLastRequest();

    void resetBufferSemaphore();

    int numberOfTracks();
    bool shouldThreadDecodeOnlyOneGop( DecoderThread *thread ) const;
    bool canThreadPutResultsIntoQueue( DecoderThread *thread ) const;

    bool repeat() const;
    void setRepeat( bool repeat );

private:
    qreal _lastMultiplier;
    qreal _multiplier;

    qint64 _maxTimestamp;         // der echte
    qint64 _maxPlaybackTimestamp; // fuer das Abspielen in einer anderen Geschw.

    QMutex *_repeatMutex;
    QMutex *_queueMutex;
    QWaitCondition *_notEmptyCond;
    PriorityQueue *_queue;

    bool _decodeOnlyOneGop;
    bool _repeat;
};

#endif // SHAREDTHREADDATA_H
