#include "PlaybackController.h"

#include "Decoder/H264Decoder.h"
#include "Decoder/JpegDecoder.h"


PlaybackController::PlaybackController() :
    _decoderCollection(),
    _shared(),
    _lastThreadId( -1 )
{
    _threadController = _shared.threadController;
    _timing = _shared.timing;
    _state = _shared.state;

    qRegisterMetaType<ListOfVideoImagesPtr>( "ListOfVideoImagesPtr" );
    connectSignals();

    connect( _timing, SIGNAL(timestampChanged(int)),
             this, SIGNAL(timestampChanged(int)) );
}

PlaybackController::~PlaybackController() {
    qDeleteAll( _decoderCollection );
    _decoderCollection.clear();
}

void PlaybackController::connectSignals() {
    connect( _threadController->getConsumerThread(), SIGNAL(displayImage(VideoImagePtr)),
             this, SLOT(dispatchVideoImage(VideoImagePtr)) );

    // forward
    connect( _state, SIGNAL(buffering()), this, SIGNAL(buffering()) );
    connect( _state, SIGNAL(running()), this, SIGNAL(playing()) );
    connect( _state, SIGNAL(stopped()), this, SIGNAL(stopped()) );
}

/**
 * @brief PlaybackController::addTrack
 * @param track Track der dem Controller hinzugefuegt werden soll
 * @return Erfolgreich oder nicht
 */
bool PlaybackController::addTrack( VideoTrack *track ) {
    DecoderThread *thread = _threadController->addDecoderThread( track );
    if ( thread == 0 ) {
        qCritical() << "Error creating DecoderThread";
        return false;
    }

    // Intermediate Signals
    connect( thread, SIGNAL(gopDecoded(ListOfVideoImagesPtr)),
             this, SLOT(gotFirstDecodedGop(ListOfVideoImagesPtr)) );

    _threadController->startIntermediateDecoding( thread );

    return true;
}

PlayDirection::Direction PlaybackController::getCurrentPlayDirection() {
    if ( _shared.multiplier() < 0 )
        return PlayDirection::Backward;
    else
        return PlayDirection::Forward;
}

void PlaybackController::setMaxTimestamp( quint64 timestamp ) {
    _shared.setMaxTimestamp( timestamp );
}

StateEnum::StateValue PlaybackController::getCurrentState() const {
    return _state->getState();
}

void PlaybackController::dispatchVideoImage( VideoImagePtr videoImage ) {
    if ( !videoImage->isValid() )
        qDebug() << "Got invalid frame from "
                 << videoImage->getParentTrack()->getFileName();

    VideoWindow *win = videoImage->getParentTrack()->getAssignedWindow();
    win->updateVideoImage( videoImage );
}

void PlaybackController::gotFirstDecodedGop( ListOfVideoImagesPtr imageList ) {
    if ( !_threadController->isThreadIntermediate( (DecoderThread *)sender() ) )
        return;

    if ( imageList->size() == 0 )
        return;

    VideoImagePtr vi = imageList->first();
    VideoWindow *assignedWindow = vi->getParentTrack()->getAssignedWindow();
    assignedWindow->updateVideoImage( vi );
}

void PlaybackController::resetBufferSemaphore() {
    // reset semaphore
    if ( _shared.bufferSemaphore.available() > 0 )
        _shared.resetBufferSemaphore();
}

void PlaybackController::play() {
    _threadController->setDecodeOnlyOneGop( false );

    resetBufferSemaphore();
    _state->changeToPlaying();

    _playDirectionAtLastPlay = getCurrentPlayDirection();
}

void PlaybackController::pause() {
    _state->changeToPaused();
}

void PlaybackController::stop() {
    if ( _state->getState() == StateEnum::Stopped )
        return;

    _state->changeToStopped();
}

void PlaybackController::changePlaybackSpeed( qreal speedMultiplier ) {
    bool pausedByMethod = false;
    if ( _state->getState() == StateEnum::Playing ) {
        pausedByMethod = true;
        pause();
    }

    _shared.clearQueue();
    _shared.setPlaybackSpeed( speedMultiplier );

    qint64 ts = _timing->uiTimestamp() * _shared.multiplier();
    _timing->setPlaybackTs( ts );

    if ( pausedByMethod )
        play();
}

void PlaybackController::changePlayDirection() {
    qint64 pts = 0;

    _shared.setMultiplier( _shared.multiplier() * -1.0 );
    pts = _timing->internalPlayTs() * -1.0;

    qDebug() << "changePlayDirection(): pts =" << pts << "jts =" << _timing->jumpTimestamp();
    _timing->setPlaybackTs( pts );
}

void PlaybackController::jumpToTimestamp( qint64 timestamp ) {
    qDebug() << "jumpToTimestamp(" << timestamp << ") entered; its:"
             << _timing->internalPlayTs();

    bool paused = false;
    if ( getCurrentState() == StateEnum::Playing ) {
        paused = true;
        pause();
    }

    _timing->setPlaybackTs( timestamp * _shared.multiplier() );

    if ( paused ) {
        play();
    } else {
        resetBufferSemaphore();
        _threadController->startIntermediateDecoding();
        _threadController->waitForFinishing();
    }
}

void PlaybackController::enableRepeat( bool enable ) {
    _shared.setRepeat( enable );
}
