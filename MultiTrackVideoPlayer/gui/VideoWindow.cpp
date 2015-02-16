#include "VideoWindow.h"

#include <QVBoxLayout>

VideoWindow::VideoWindow(int id, QWidget *parent) :
    QWidget(parent)
{
    _id = id;
    this->setGeometry(0, 0, 50, 50);
    this->setMinimumSize(50, 50);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setSpacing( 0 );
    layout->setContentsMargins( 0, 0, 0, 0 );

    _imageLabel = new QLabel( this );
    _imageLabel->setScaledContents(true);
    _scaling = false;
    _imageLabel->setObjectName("_imageLabel");

    layout->addWidget( _imageLabel );
}

void VideoWindow::updateVideoImage(const VideoImagePtr &image) {
    setWindowTitle( QString( "%1 t: %2").
                    arg( image->getParentTrack()->getTrackName() ).
                    arg( image->isValid() ? QString::number( image->getVideoPicture()->getTimestamp() ) : "invalid" ) );

    if (!_scaling)
    {
        _imageLabel->setPixmap( QPixmap::fromImage( image->getImage() ) );
    }
    else
    {
        _imageLabel->setPixmap( QPixmap::fromImage( image->getImage().scaled(this->height(), this->width(), Qt::KeepAspectRatio) ) );
    }
}

void VideoWindow::setScalingEnabled(bool enabled)
{
    _scaling = enabled;
}
