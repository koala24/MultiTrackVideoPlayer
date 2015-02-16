#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include <QObject>
#include <QSize>
#include <QList>
#include <QDir>

#include "IDecoder.h"
#include "VideoData/videoimage.h"
#include "VideoData/videotrack.h"

class JpegDecoder : public QObject, public IDecoder
{
    Q_OBJECT
public:
    explicit JpegDecoder( const QString &workingDirectory,
                          VideoTrack *videoTrack,
                          QObject *parent = 0 );

    ~JpegDecoder();

    virtual void open();
    virtual void close();

    virtual bool hasMore( qreal multiplier );

    virtual void jumpToTimestamp(quint64 timestamp);

    virtual ListOfVideoImagesPtr decode( qreal multiplier );

    virtual VideoTrack* getVideoTrack();

    void setFileCounterLength(quint64);

private:
    QString _filename;

    VideoTrack *_videoTrack;
    QList<IVideoPicture *> _pictureList;
    QSize _videoSize;

    quint64 _maxFrames;
    quint64 _currentFrame;
    quint64 _fileCounterLength;
    IVideoPicture *_currentVideoPicture;


    qreal _multiplier;
    quint64 _maxTimestamp;
    qint64 _jumpedTimestamp;
    qint64 _frameLength;

    bool _openSuccessfulCalled;

    /* Private Methods */
    void throwException(const QString errorMessage);
    QString getRealFileName();

    VideoImagePtr decodeNext( qreal multiplier );

    void decrementCurrentFrame(quint64 minuent);
};

#endif // JPEGDECODER_H
