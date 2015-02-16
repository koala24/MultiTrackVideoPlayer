#ifndef VIDEOIMAGE_H
#define VIDEOIMAGE_H

#include <QImage>
#include <QSharedPointer>
#include "VideoData/ivideopicture.h"
#include "VideoData/videotrack.h"

class VideoTrack;
class IVideoGop;

class VideoImage
{
private:
    QImage _image;
    qint64 _playbackTimestamp;
    IVideoPicture *_videoPicture;
    VideoTrack *_videoTrack;
    bool _valid;

    void createPlaceholderImage();

public:
    VideoImage();
    VideoImage(VideoTrack *videoTrack);
    VideoImage(QImage image, qint64 playbackTimestamp,
               IVideoPicture *videoPicture, VideoTrack *videoTrack);
    virtual ~VideoImage();

    bool isValid() const;

    qint64 getPlaybackTimestamp() const;
    qint64 getRealTimestamp() const;
    IVideoGop* getParentGop() const;
    VideoTrack* getParentTrack() const;
    QImage getImage() const;

    IVideoPicture* getVideoPicture() const;

    QString toString();

};

typedef QSharedPointer<VideoImage> VideoImagePtr;

#endif // VIDEOIMAGE_H
