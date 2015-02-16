#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "VideoData/videotrackcollection.h"
#include "VideoData/videotrack.h"
#include "VideoData/videogop.h"
#include "VideoData/videoimage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    IVideoGop *gop = new VideoGop(999, 666);
    IVideoPicture *picture = new VideoPicture(5323, 2323);
    gop->addVideoPicture(picture);
    gop->addVideoPicture(new VideoPicture(12333, 6323));

    VideoTrack *track = new VideoTrack("MeinTrack", "Track", "I");
    QList<IVideoGop*> gops;
    gops.append(gop);
    track->setVideoGops(gops);
    track->writeXMLFile();

    VideoTrack *track2 = new VideoTrack();
    //track2->readXMLFile("ballroom_0_24frPs.264");
    //track2->readXMLFile("ballroom_5");
    track2->readXMLFile("Track");
    qDebug() << "Track loaded from XML File: \n" + track2->toString();

    VideoImage videoImage;
    videoImage.setVideoPicture(picture);
    qDebug() << videoImage.toString();

    IVideoPicture *pic = track->findPicByTimestamp(5323);
    qDebug() << "Found " + pic->toString();

    delete track;
    delete track2;

}

MainWindow::~MainWindow()
{
    delete ui;
}
