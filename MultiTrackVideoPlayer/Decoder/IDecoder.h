#ifndef IDECODER_H
#define IDECODER_H

#include "VideoData/videoimage.h"

typedef QList<VideoImagePtr> ListOfVideoImages;
typedef QSharedPointer<ListOfVideoImages> ListOfVideoImagesPtr;

inline qreal calcPlaybackTimestamp( quint64 timestamp, qreal multiplier ) {
    return (qreal)timestamp * multiplier;
}

class IDecoder {
public:
    virtual ~IDecoder() {}

    virtual void open() = 0;
    virtual void close() = 0;

    virtual bool hasMore( qreal multiplier ) = 0;

    virtual void jumpToTimestamp( quint64 timestamp ) = 0;

    virtual ListOfVideoImagesPtr decode( qreal multiplier ) = 0;

    virtual VideoTrack* getVideoTrack() = 0;
};

#endif // IDECODER_H
