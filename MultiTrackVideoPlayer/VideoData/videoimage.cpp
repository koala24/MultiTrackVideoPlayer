#include "videoimage.h"
#include "VideoData/videogop.h"

VideoImage::VideoImage()
    : _image(), _playbackTimestamp(0), _videoPicture(), _videoTrack(), _valid( false )
{
    createPlaceholderImage();
}

/**
 *
 */
VideoImage::VideoImage(VideoTrack *videoTrack)
    : _image(), _playbackTimestamp(0), _videoPicture(), _videoTrack(videoTrack), _valid( false )
{
    createPlaceholderImage();
}

/**
 *
 */
VideoImage::VideoImage(QImage image, qint64 playbackTimestamp,
                       IVideoPicture *videoPicture, VideoTrack *videoTrack)
    : _image(image), _playbackTimestamp( playbackTimestamp ),
      _videoPicture(videoPicture), _videoTrack(videoTrack), _valid( true )
{
}
/**
 *
 */
VideoImage::~VideoImage()
{
}

void VideoImage::createPlaceholderImage() {
    QImage placeholder( 1, 1, QImage::Format_RGB32 );
    placeholder.setPixel( QPoint( 0, 0 ), qRgb( 0, 128, 0 ) );
    placeholder.scaled( QSize( _videoTrack->getVideoSize() ) );
    _image = placeholder;
}

bool VideoImage::isValid() const {
    return _valid;
}

qint64 VideoImage::getPlaybackTimestamp() const {
    return _playbackTimestamp;
}

qint64 VideoImage::getRealTimestamp() const {
    return _videoPicture->getTimestamp();
}

IVideoGop *VideoImage::getParentGop() const {
    if ( _videoPicture )
        return dynamic_cast<VideoPicture*>( _videoPicture )->getVideoGop();
    else
        return 0;
}

VideoTrack *VideoImage::getParentTrack() const {
    return _videoTrack;
}

QImage VideoImage::getImage() const
{
    return _image;
}
/**
 *
 */
IVideoPicture* VideoImage::getVideoPicture() const
{
    return _videoPicture;
}
/**
 *
 */
QString VideoImage::toString() {
    if ( isValid() ) {
        return QString( "pts: %1 rts: %2 @%3" ).
                arg( QString::number( getPlaybackTimestamp() ) ).
                arg( QString::number( getRealTimestamp()) ).
                arg( getParentTrack()->getFileName() );
    } else {
        return "Invalid VideoImage";
    }
}
