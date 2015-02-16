#include "H264Decoder.h"
#include "Exception.h"

#include <pthread.h>
#include <sys/stat.h>
#include <cstring>

#include <QImage>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

// http://ffmpeg.org/pipermail/libav-user/2011-November/000896.html
static int lockmgr( void **mutex, enum AVLockOp op ) {
    pthread_mutex_t** pmutex = (pthread_mutex_t**) mutex;
    switch (op) {
        case AV_LOCK_CREATE:
            *pmutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
            pthread_mutex_init(*pmutex, NULL);
            break;
        case AV_LOCK_OBTAIN:
            pthread_mutex_lock(*pmutex);
            break;
        case AV_LOCK_RELEASE:
            pthread_mutex_unlock(*pmutex);
            break;
        case AV_LOCK_DESTROY:
            pthread_mutex_destroy(*pmutex);
            free(*pmutex);
            break;
    }
    return 0;
}

H264Decoder::H264Decoder(const QString &workingDirectory,
                          VideoTrack *videoTrack,
                          QObject *parent ) :
    QObject(parent)
{
    avcodec_register_all();

    _videoTrack = videoTrack;
    _videoSize = _videoTrack->getVideoSize();
    _pictureList = _videoTrack->getVideoPictureList();

    _openSuccessfulCalled = false;
    _filename = workingDirectory + QDir::separator() + _videoTrack->getFileName();

    _fileHandle = 0;
    _filesize = 0;
    _context = 0;
    _frame = 0;
    _outFrame = 0;
    _outBuffer = 0;
    _swsContext = 0;

    _inbuf = (uint8_t *) malloc( 1024 );
    _maxGops = _videoTrack->countGops();

    IVideoGop *lastGop = _videoTrack->getVideoGops()[_maxGops - 1];
    _maxTimestamp = lastGop->getVideoPictures()[lastGop->countPictures() - 1]->getTimestamp();

    _currentGopIndex = 0;
    _currentGop = 0;
    _currentVideoPicture = 0;
    _jumpedTimestamp = 0;
    _frameLength = _pictureList.size() >= 2 ? _pictureList[1]->getTimestamp() : 0;

    _debugDecoder = qApp->arguments().contains( "--decoder-debug" );
}

H264Decoder::~H264Decoder() {
    if ( _context ) {
        avcodec_close( _context );
        av_free( _context );
    }

    if ( _frame )
        avcodec_free_frame( &_frame );

    if ( _outFrame )
        av_frame_free( &_outFrame );

    if ( _inbuf )
        free( _inbuf );

    if ( _outBuffer )
        delete[] _outBuffer;

    if (_swsContext)
        sws_freeContext(_swsContext);

    av_lockmgr_register( NULL );
}

void H264Decoder::allocOutputFrame() {
    _outFrame = avcodec_alloc_frame();
    if( _outFrame == NULL )
        throw new Exception( "AVFrame couldn't created." );

    int numBytes = avpicture_get_size( PIX_FMT_RGB32,
                                   _videoSize.width(),
                                   _videoSize.height() );
    _outBuffer = new uint8_t[numBytes];

    avpicture_fill( (AVPicture *)_outFrame, _outBuffer, PIX_FMT_RGB32,
                    _videoSize.width(), _videoSize.height() );
}

void H264Decoder::throwException( const QString errorMessage ) {
    throw Exception( QString( "Error in '%1': %2" ).
                     arg( _videoTrack->getTrackName() ).
                     arg( errorMessage ) );
}

quint64 H264Decoder::statFilesize() {
    struct stat st;
    stat( _filename.toStdString().c_str(), &st );
    return st.st_size;
}

void H264Decoder::open() {
    if ( _openSuccessfulCalled )
        return;

    av_lockmgr_register( &lockmgr );

    _codec = avcodec_find_decoder( AV_CODEC_ID_H264 );
    if ( !_codec )
        throwException( "Codec not found" );

    _context = avcodec_alloc_context3( _codec );
    if ( !_context )
        throwException( "Could not allocate video codec context" );

    if ( _videoSize.width() <= 0 || _videoSize.height() <= 0 ) {
        throwException( QString( "Invalid Video Size: %1x%2" ).
                     arg( _videoSize.width() ).arg( _videoSize.height() ) );
    }

    _context->pix_fmt = PIX_FMT_YUV420P16;
    _context->width = _videoSize.width();
    _context->height = _videoSize.height();

    if ( avcodec_open2( _context, _codec, NULL ) < 0 )
        throwException( "Could not open codec" );

    _fileHandle = fopen( _filename.toStdString().c_str(), "rb" );
    if ( !_fileHandle )
        throwException( "Could not open " + _filename );

    _filesize = statFilesize();

    _frame = avcodec_alloc_frame();
    if ( !_frame )
        throwException( "Could not allocate video frame" );

    allocOutputFrame();

    qDebug( "Opened '%s' (%dx%d, %llums, %d frames) successful.",
            qPrintable( _videoTrack->getFileName() ),
            _videoSize.width(), _videoSize.height(),
            _maxTimestamp, _pictureList.size() );

    _openSuccessfulCalled = true;
}

bool H264Decoder::hasMore( qreal multiplier ) {
    if ( multiplier < 0 )
        return _currentGopIndex >= 0;
    else
        return _currentGopIndex < (qint64) _maxGops;
}

void H264Decoder::jumpToTimestamp(quint64 timestamp) {
    for ( quint64 i = 0; i < _maxGops; i++ ) {
        for ( int p = 0; p < _videoTrack->getVideoGops()[i]->countPictures(); p++ ) {
            if ( _videoTrack->getVideoGops()[i]->getVideoPictures()[p]->getTimestamp() >= timestamp ) {
                _currentGopIndex = i;
                _jumpedTimestamp = timestamp;

                if ( _debugDecoder )
                    qDebug() << "+++ jumped to" << _currentGopIndex << "cause of" << timestamp;

                return;
            }
        }
    }

    // nichts gefunden, also den letzten GOP nutzen
    _currentGopIndex = _maxGops - 1;
    _jumpedTimestamp = timestamp;
}

VideoImagePtr H264Decoder::decodeFrame( quint64 bytesToRead, qreal multiplier ) {
    if ( !_openSuccessfulCalled )
        open();

    AVPacket packet;
    int len, gotFrame;

    size_t ibSize = sizeof( uint8_t ) * bytesToRead + FF_INPUT_BUFFER_PADDING_SIZE;
    _inbuf = (uint8_t *) realloc( _inbuf, ibSize );
    if ( _inbuf == NULL )
        throwException( QString( "Can't reallocate memory with size %1" ).
                     arg( bytesToRead ) );

    std::memset( _inbuf, 0, ibSize );
    av_init_packet( &packet );

    packet.size = fread( _inbuf, 1, bytesToRead, _fileHandle );
    if ( packet.size > 0 ) {
        packet.data = _inbuf;

        len = avcodec_decode_video2( _context, _frame, &gotFrame, &packet );
        if ( len < 0 )
            throwException( QString( "Error while decoding frame." ) );

        if ( gotFrame )
            return convertDecodedFrameToRgbImage( multiplier );
    }

    // invalid VideoImage Objekt - es konnte kein Frame dekodiert werden
    return VideoImagePtr( new VideoImage( _videoTrack ) );
}

void H264Decoder::close() {
    if ( _fileHandle )
        fclose( _fileHandle );
    _openSuccessfulCalled = false;
}

VideoTrack *H264Decoder::getVideoTrack() {
    return _videoTrack;
}

VideoImagePtr H264Decoder::convertDecodedFrameToRgbImage( qreal multiplier ) {
    if ( _swsContext == NULL ) {
        _swsContext = sws_getContext( _videoSize.width(),
                                      _videoSize.height(),
                                      _context->pix_fmt,
                                      _videoSize.width(),
                                      _videoSize.height(),
                                      PIX_FMT_RGB32,
                                      SWS_BICUBIC,
                                      NULL,
                                      NULL,
                                      NULL );

        if ( _swsContext == NULL )
            throwException( "Error creating SWScale Context" );
    }

    sws_scale( _swsContext, _frame->data, _frame->linesize,
               0, _videoSize.height(), _outFrame->data, _outFrame->linesize );

    QImage image( _videoSize.width(), _videoSize.height(), QImage::Format_RGB32 );
    for( int y = 0; y < _videoSize.height(); y++ )
        memcpy( image.scanLine( y ),
                _outFrame->data[0] + y * _outFrame->linesize[0],
                _videoSize.width() * 4 );

    qreal fTimestamp = calcPlaybackTimestamp( _currentVideoPicture->getTimestamp(), multiplier );
    return VideoImagePtr( new VideoImage( image,
                                          fTimestamp,
                                          _currentVideoPicture,
                                          _videoTrack ) );
}

quint64 H264Decoder::readOffsetOfFirstPictureInNextGop( quint64 currGopIndex ) {
    if ( currGopIndex + 1 == _maxGops )
        return _filesize;

    VideoGop *gop = readGop( currGopIndex + 1 );
    return gop->getVideoPictures()[0]->getOffset();
}

VideoGop* H264Decoder::readGop( quint64 index ) {
    if ( index >= _maxGops )
        return 0;
    else
        return dynamic_cast<VideoGop *>( _videoTrack->getVideoGops()[index] );
}

ListOfVideoImagesPtr H264Decoder::decode( qreal multiplier ) {
    ListOfVideoImagesPtr gopList = ListOfVideoImagesPtr( new ListOfVideoImages() );
    if ( (quint64)_currentGopIndex >= _maxGops )
        return gopList;

    quint64 nextOffset = 0;
    _currentGop = readGop( _currentGopIndex );

    if ( _debugDecoder )
        qDebug().nospace() << "CurrentGopIndex = " << _currentGopIndex << " @" << _videoTrack->getTrackName();

    for ( int i = 0; i < _currentGop->countPictures(); i++ ) {
        _currentVideoPicture = dynamic_cast<VideoPicture *>( _currentGop->getVideoPictures()[i] );

        if ( i == 0 )
            fseek( _fileHandle, _currentVideoPicture->getOffset(), SEEK_SET );

        // wenn das Bild nicht das letzte Bild in dem GOP ist
        if ( i + 1 < _currentGop->countPictures() ) {
            VideoPicture *next = dynamic_cast<VideoPicture *>( _currentGop->getVideoPictures()[i + 1] );
            nextOffset = next->getOffset();
        } else if ( i + 1 == _currentGop->countPictures() ) { // letzte Bild in GOP
            nextOffset = readOffsetOfFirstPictureInNextGop( _currentGopIndex );
        }

        quint64 size = nextOffset - _currentVideoPicture->getOffset();
        VideoImagePtr vi = decodeFrame( size, multiplier );
        //qDebug() << "rts:" << _currentVideoPicture->getTimestamp()
        //         << "j+l:" << _jumpedTimestamp + _frameLength;

        if ( ((qint64)_currentVideoPicture->getTimestamp() > _jumpedTimestamp - _frameLength
              && multiplier >= 0 )
             || ((qint64)_currentVideoPicture->getTimestamp() < _jumpedTimestamp + _frameLength
              && multiplier < 0 )
             || ((qint64)_currentVideoPicture->getTimestamp() == _jumpedTimestamp)
             || _jumpedTimestamp < 0 ) {
            // nu darfste das Bild hinzufuegen
            //qDebug() << "Including:" << vi->getPlaybackTimestamp();
            gopList->append( vi );
        } else {
            //qDebug() << "Not including:" << vi->getPlaybackTimestamp();
        }

        nextOffset = _currentVideoPicture->getOffset();
    }

    _jumpedTimestamp = -1;
    if ( multiplier >= 0 )
        _currentGopIndex++;
    else
        _currentGopIndex--;

    return gopList;
}
