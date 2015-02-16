#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QImage>

#include "VideoData/videoimage.h"

class VideoWindow : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWindow(int id, QWidget *parent = 0);

    void updateVideoImage( const VideoImagePtr &image );
    void setScalingEnabled(bool enabled);

signals:
   void closeSignal(int id);

private:
    int _id;
    QLabel *_imageLabel;
    bool _scaling;

protected:
   void closeEvent(QCloseEvent *event)
   {
      emit closeSignal(_id);
      QWidget::closeEvent(event);
   }

};

#endif // VIDEOWINDOW_H
