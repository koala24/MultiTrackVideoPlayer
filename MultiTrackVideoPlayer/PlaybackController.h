#ifndef DECODERTHREADCONTROLLER_H
#define DECODERTHREADCONTROLLER_H

#include <QObject>
#include <QHash>
#include <QFileInfo>

#include "Decoder/IDecoder.h"
#include "gui/VideoWindow.h"
#include "VideoData/videotrack.h"

#include "Backend/DecoderThread.h"
#include "Backend/VideoImageConsumer.h"
#include "Backend/SharedThreadData.h"

#include "Enums.h"

class PlaybackController : public QObject
{
    Q_OBJECT
public:
    PlaybackController();
    ~PlaybackController();

    bool addTrack(VideoTrack *track);
    PlayDirection::Direction getCurrentPlayDirection();
    void setMaxTimestamp( quint64 timestamp );

    StateEnum::StateValue getCurrentState() const;

signals:
    void timestampChanged( int timestamp );

    void playing();
    void pausing();
    void stopped();
    void buffering();

public slots:
    void play();
    void pause();
    void stop();

    /**
     * @brief Ändert die Abspielgeschwindigkeit
     * @param speedMultiplier 1 = normale Geschwindigkeit
     *                        2 = doppelt so schnell
     *                        0.5 = halb so schnell usw...
     */
    void changePlaybackSpeed( qreal speedMultiplier );
    void changePlayDirection();
    void jumpToTimestamp(qint64 timestamp );

    void enableRepeat(bool enable);

private slots:
    void dispatchVideoImage( VideoImagePtr videoImage );
    void gotFirstDecodedGop( ListOfVideoImagesPtr imageList );

private:
    QList<IDecoder *> _decoderCollection;

    SharedThreadData _shared;
    ThreadController *_threadController;
    Timing *_timing;
    State *_state;

    int _lastThreadId;

    PlayDirection::Direction _playDirectionAtLastPlay;

    void connectSignals();

    void handleDecoderThreadAsNormalThread( DecoderThread *thread );
    void resetBufferSemaphore();
};

#endif // DECODERTHREADCONTROLLER_H
