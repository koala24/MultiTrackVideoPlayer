#include "videotrackcollection.h"
#include "videotrack.h"
#include "videogop.h"

#include <QString>
#include <QChar>
#include <QDir>
#include <QtCore>

#include <iostream>
#include <vector>
#include <iterator>
#include <cstring>
#include <algorithm>

#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

void write_xml_file(QString outfile, QDir dir, int fps, int offset, QString type, QSize size){
    VideoTrack *vt = new VideoTrack(outfile, outfile, type);
    qreal msec = 1000.0 / (qreal)fps;
    uint timestamp = 0;
    uint i = 1;

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QStringList nameFilter;
    nameFilter << "*.jpg" << "*.jpeg";
    QFileInfoList list = dir.entryInfoList(nameFilter, QDir::Files);
    dir.setNameFilters(nameFilter);

    QStringList ls = dir.entryList();
//    ls.removeOne(".");
//    ls.removeOne("..");
    
    QFileInfo fileInfo(ls[0]);
    QString baseName = fileInfo.baseName();
    // last 3 digits are for file counter (<name>001.jpg etc)
    baseName.resize(baseName.size() - 3);

    for(QStringList::const_iterator it = ls.constBegin(); it != ls.constEnd(); ++it, ++i){
      qDebug() << *it;
      IVideoGop *gop = new VideoGop();
      IVideoPicture *picture = new VideoPicture(timestamp, offset);
      gop->addVideoPicture(picture);
      timestamp = qRound(msec * i);
      vt->addVideoGop(gop);
    }
    qDebug() << baseName;
    vt->setFileName(baseName);
    vt->setVideoSize(size);
    vt->writeXMLFile();
}

int main(int argc, char* argv[]){
  if(argc != 3){
    cout << "<dir> <fps>";
    return -1;
  }
  QDir dir(argv[1]);
  if(!dir.exists()){
      cout << "Directory does not exist";
      return -1;
  }

  QDir absDir(dir.absolutePath());
  QString dirName = absDir.dirName();

  int fps = atoi(argv[2]);

  QSize size(640, 480);
  write_xml_file(dirName, absDir, fps, 0, QString("JPEG"), size);

  return 0;
}
