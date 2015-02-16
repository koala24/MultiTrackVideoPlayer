#include "JpegDecoder.h"
#include "Exception.h"

#include <pthread.h>

#include <QImage>
#include <QDebug>
#include <QDir>

JpegDecoder::JpegDecoder(const QString &workingDirectory,
                          VideoTrack *videoTrack,
                          QObject *parent ) :
    QObject(parent)
{
    _videoTrack = videoTrack;
    _videoSize = _videoTrack->getVideoSize();
    _pictureList = _videoTrack->getVideoPictureList();

    _openSuccessfulCalled = false;
    _currentFrame = -1;

    _filename = workingDirectory + QDir::separator() + _videoTrack->getFileName();
    _currentVideoPicture = 0;

    _maxFrames = _pictureList.size();
    setFileCounterLength(3);

    IVideoGop *lastGop = _videoTrack->getVideoGops()[_maxFrames - 1];
    _maxTimestamp = lastGop->getVideoPictures()[lastGop->countPictures() - 1]->getTimestamp();
    _jumpedTimestamp = -1;

    _frameLength = _pictureList.size() >= 2 ? _pictureList[1]->getTimestamp() : 0;
}

JpegDecoder::~JpegDecoder() {
    if ( _openSuccessfulCalled )
        close();
}

void JpegDecoder::throwException( const QString errorMessage ) {
    throw Exception( QString( "Error in '%1': %2" ).
                     arg( _videoTrack->getTrackName() ).
                     arg( errorMessage ) );
}

void JpegDecoder::open() {
    if ( _videoSize.width() <= 0 || _videoSize.height() <= 0 ) {
        throwException( QString( "Invalid Video Size: %1x%2" ).
                     arg( _videoSize.width() ).arg( _videoSize.height() ) );
    }

    _openSuccessfulCalled = true;
}

VideoTrack *JpegDecoder::getVideoTrack() {
    return _videoTrack;
}

bool JpegDecoder::hasMore( qreal multiplier ) {

    if (multiplier < 0)
        return _currentFrame > 0;
    else
        return _currentFrame + 1 < (quint64) _maxFrames;
}

void JpegDecoder::jumpToTimestamp(quint64 timestamp ) {
    for (int i = 0; i < (int)_maxFrames; i++) {
        if (timestamp < _pictureList[i]->getTimestamp()) {
            _currentFrame = i - 1;
            _jumpedTimestamp = timestamp;
            return;
        } else if ( timestamp == _pictureList[i]->getTimestamp() ) {
            _currentFrame = i;
            _jumpedTimestamp = timestamp;
            return;
        }
    }

    _currentFrame = _maxFrames - 1;
    _jumpedTimestamp = timestamp;
}

VideoImagePtr JpegDecoder::decodeNext( qreal multiplier ) {
    _currentVideoPicture = _pictureList[_currentFrame];

    QImage image;
    if( image.load( getRealFileName() ) ){
        qreal fTimestamp = calcPlaybackTimestamp( _currentVideoPicture->getTimestamp(), multiplier );
        return VideoImagePtr( new VideoImage( image, fTimestamp,
                                              _currentVideoPicture, _videoTrack ) );
    }
    // invalid VideoImage Objekt - es konnte kein Frame dekodiert werden
    return VideoImagePtr( new VideoImage( _videoTrack ) );
}

void JpegDecoder::decrementCurrentFrame( quint64 minuent )
{
    if ( _currentFrame == 0 )
        _currentFrame = 0;
    else
        _currentFrame = minuent - 1;
}

void JpegDecoder::close() {
    _openSuccessfulCalled = false;
}

QString JpegDecoder::getRealFileName(){
    QString appendix = QString("%1").arg( _currentFrame + 1, _fileCounterLength, 10, QChar('0') ).toUpper();
    QString rfn = _filename + appendix + ".jpg";
    return rfn;
}

void JpegDecoder::setFileCounterLength(quint64 length){
    _fileCounterLength = length;
}

ListOfVideoImagesPtr JpegDecoder::decode( qreal multiplier ) {
    ListOfVideoImagesPtr gopList = ListOfVideoImagesPtr( new ListOfVideoImages() );

    const int GOP_SIZE = 10;
    int count = _maxFrames - _currentFrame;
    if (count > GOP_SIZE)
        count = GOP_SIZE;

    for (int i = 0; i < count; i++) {
        if (hasMore(multiplier)) {
            VideoImagePtr vi = decodeNext( multiplier );
            if ( ((qint64)_pictureList[_currentFrame]->getTimestamp() > _jumpedTimestamp - _frameLength && multiplier >= 0 )
                 || ((qint64)_pictureList[_currentFrame]->getTimestamp() < _jumpedTimestamp + _frameLength && multiplier < 0 )
                 || ((qint64)_pictureList[_currentFrame]->getTimestamp() == _jumpedTimestamp)
                 || _jumpedTimestamp < 0 ) {
                gopList->append( vi );
            }

            if ( i + 1 < count && multiplier >= 0)
                _currentFrame++;
            else if (i - 1 < count && multiplier < 0)
                _currentFrame--;
        }
        else {
            if ( _currentFrame == _maxFrames - 1 || _currentFrame == 0) {
                    VideoImagePtr vi = decodeNext( multiplier );
                    gopList->append( vi );
            }
        }
    }


    _jumpedTimestamp = -1;

     return gopList;
}
