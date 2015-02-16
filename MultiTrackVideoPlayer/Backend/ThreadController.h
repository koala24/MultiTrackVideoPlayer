#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H

#include <QList>
#include <QWaitCondition>

#include "VideoImageConsumer.h"
#include "DecoderThread.h"

class VideoImageConsumer;
class DecoderThread;
class SharedThreadData;

class ThreadInfo {
    friend class ThreadController;
public:
    ThreadInfo( int id, IDecoder *decoder ) {
        _threadId = id;
        _isIntermediate = false;
        _isRunning = false;
        _canPutResultsIntoQueue = true;
        _segmentWaitCond = new QWaitCondition();
        _decoder = decoder;
    }

    ~ThreadInfo() {
        delete _segmentWaitCond;
        delete _decoder;
    }

    int threadId() const {
        return _threadId;
    }

    bool isIntermediate() const {
        return _isIntermediate;
    }

    bool canPutResultsIntoQueue() const {
        return _canPutResultsIntoQueue;
    }

    bool shouldRun() const {
        return _isRunning;
    }

    QWaitCondition* segmentWaitCondition() {
        return _segmentWaitCond;
    }

    IDecoder* decoder() {
        return _decoder;
    }

private:
    int _threadId;
    bool _isIntermediate;
    bool _isRunning;
    bool _canPutResultsIntoQueue;
    QWaitCondition *_segmentWaitCond;
    IDecoder *_decoder;
};

class ThreadController : public QObject
{
    Q_OBJECT

public:
    explicit ThreadController(SharedThreadData *shared);
    ~ThreadController();

    DecoderThread* addDecoderThread( VideoTrack *track );
    void setConsumerThread( VideoImageConsumer *consumer );

    bool startThread( DecoderThread *thread );
    void startAllThreads( qint64 decodeToTimestamp = -1 );

    void stopDecoderThread( DecoderThread *thread );
    void stopAllThreads();
    void stopAllDecoderThreads();

    int numberOfThreads();

    qint64 decodeToTimestamp();
    void setDecodeOnlyOneGop( bool onlyOne );

    bool canPutResultsIntoQueue( DecoderThread *thread );

    bool isThreadIntermediate( DecoderThread *thread );
    void setThreadAsIntermediate( DecoderThread *thread, bool putResultInQueue = false );

    VideoImageConsumer* getConsumerThread();

    void wakeConsumerThread();
    void wakeAllDecoderThreads();

    void startIntermediateDecoding();
    void startIntermediateDecoding( DecoderThread *thread );

    void waitForFinishing();

private:
    QHash<DecoderThread *, ThreadInfo *> _threads;     // DecoderThread ptr owned
    VideoImageConsumer *_consumerThread;               // owned

    QList<DecoderThread *> _runningThreads;

    SharedThreadData *_shared;

    qint64 _decodeToTimestamp;
    int _lastUsedThreadId;

    QWaitCondition _consumerSleepCond;
    QMutex _consumerSleepMutex;

    void wakeDecoderThread( DecoderThread *thread );

private slots:
    void onThreadHasDecodingFinished( DecoderThread *sender );
};

#endif // THREADCONTROLLER_H
