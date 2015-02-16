#ifndef DECODERTHREAD_H
#define DECODERTHREAD_H

#include <QThread>

#include "Decoder/IDecoder.h"
#include "VideoData/videoimage.h"
#include "gui/VideoWindow.h"

#include "Backend/SharedThreadData.h"
#include "Backend/ThreadController.h"

class SharedThreadData;
class ThreadInfo;

class DecoderThread : public QThread
{
    Q_OBJECT

    void run();

public:
    DecoderThread(ThreadInfo *info, SharedThreadData *data);
    ~DecoderThread();

    int getThreadId() const;


signals:
    void gopDecoded( ListOfVideoImagesPtr gop );
    void decodingFinished( DecoderThread *sender );

private:
    SharedThreadData *_shared;
    int _maxDecodedTimestamp;
    int _lastMaxDecodedTimestamp;
    int _decodedGops;

    const int EARLY_WAKE_MS;

    QMutex _mutex;

    ThreadInfo *_info;

    void decode();
    void waitOrNext();
    inline bool shouldWaitForNextSegment();
    void openDecoder();
};

#endif // DECODERTHREAD_H
