#ifndef H264DECODER_H
#define H264DECODER_H

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include <QObject>
#include <QSize>
#include <QList>

#include "IDecoder.h"
#include "VideoData/videoimage.h"
#include "VideoData/videotrack.h"

extern "C" {
    #include <libavutil/opt.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/common.h>
    #include <libswscale/swscale.h>
}

class H264Decoder : public QObject, public IDecoder
{
    Q_OBJECT
public:
    explicit H264Decoder( const QString &workingDirectory,
                          VideoTrack *videoTrack,
                          QObject *parent = 0 );

    ~H264Decoder();

    virtual void open();
    virtual void close();

    virtual bool hasMore( qreal multiplier );

    virtual void jumpToTimestamp(quint64 timestamp);

    virtual ListOfVideoImagesPtr decode( qreal multiplier );

    virtual VideoTrack* getVideoTrack();

private:
    FILE *_fileHandle;
    uint8_t *_inbuf;
    uint8_t *_outBuffer;

    AVCodec *_codec;
    AVCodecContext *_context;
    AVFrame *_frame;
    AVFrame *_outFrame;
    struct SwsContext *_swsContext;

    QString _filename;
    quint64 _filesize;

    VideoTrack *_videoTrack;
    QList<IVideoPicture *> _pictureList;
    QSize _videoSize;

    qint64 _jumpedTimestamp;
    quint64 _maxTimestamp;
    int _frameLength;

    qint64 _currentGopIndex;
    quint64 _maxGops;
    VideoPicture *_currentVideoPicture;
    VideoGop *_currentGop;

    bool _openSuccessfulCalled;
    bool _debugDecoder;

    /* Private Methods */
    void allocOutputFrame();
    VideoImagePtr convertDecodedFrameToRgbImage(qreal multiplier);
    void throwException(const QString errorMessage);

    quint64 statFilesize();

    VideoGop* readGop( quint64 index );
    quint64 readOffsetOfFirstPictureInNextGop(quint64 currGopIndex);

    VideoImagePtr decodeFrame( quint64 bytesToRead , qreal multiplier );
};

#endif // H264DECODER_H
