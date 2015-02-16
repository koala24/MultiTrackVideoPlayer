#include "parser.h"

Parser::Parser()
{
}

void Parser::writeJPEGXMLFile(QString filename, int fps, QSize size)
{
    QFileInfo info(filename);
    QDir dir(info.absolutePath());
    QString dirName = dir.dirName();
    VideoTrack *vt = new VideoTrack(dirName, dirName, "JPEG");
    qreal msec = 1000.0 / (qreal)fps;
    uint timestamp = 0;
    uint i = 1;

    QStringList ls = dir.entryList();
    ls.removeOne(".");
    ls.removeOne("..");

    for(QStringList::const_iterator it = ls.constBegin(); it != ls.constEnd(); ++it, ++i)
    {
        IVideoGop *gop = new VideoGop(0, timestamp);
        IVideoPicture *picture = new VideoPicture(timestamp, 0);
        gop->addVideoPicture(picture);
        timestamp = qRound(msec * i);
        vt->addVideoGop(gop);
    }

    QFileInfo fileInfo(ls[0]);
    QString baseName = fileInfo.baseName();
    baseName.resize(baseName.size() - 4);
    vt->setFileName(baseName);
    vt->setVideoSize(size);
    vt->writeXMLFile(dir.absolutePath() + "/" + baseName);
    delete vt;
}

void Parser::writeH264XMLFile(QString filename, int fps, QSize size)
{
    QFile *file = new QFile(filename);
    if (file->open(QIODevice::ReadOnly))
    {
        QFileInfo fileInfo(filename);

        QChar type;
        qreal msec = 1000.0 / (qreal)fps;
        uint timestamp = 0;
        uint i = 0;

        QVector<OffsetType> offsettype = H264Parser::parseAnnexB(file);

        IVideoTrack *vt = new VideoTrack(fileInfo.fileName(), fileInfo.fileName(), type);
        IVideoGop *gop;
        IVideoPicture *picture;

        for (QVector<OffsetType>::const_iterator it = offsettype.constBegin();
             it != offsettype.constEnd();++it, ++i)
        {

            if (it->_type != 1)
            {
                gop = new VideoGop();
                picture = new VideoPicture(timestamp, it->_offset - 4);
                gop->addVideoPicture(picture);
                while (it->_type != 1)
                {
                    it++;
                }
                i++;
            }
            timestamp = qRound(msec * i);
            if (it->_type == 1)
            { //non-IDR
                picture = new VideoPicture(timestamp, it->_offset - 4);
                gop->addVideoPicture(picture);
            }
            if ((it + 1)->_type != 1)
            {
                vt->addVideoGop(gop);
            }
            if (it == offsettype.constEnd())
            {
                vt->addVideoGop(gop);
            }
        }
        vt->setVideoSize(size);
        vt->setFileType(QString("H264"));
        vt->setTrackName(fileInfo.fileName());
        vt->writeXMLFile(fileInfo.absolutePath() + "/" + fileInfo.baseName());
        delete vt;
        file->close();
    }
}

void Parser::createXMLFile(QString filename, int fps, QSize size)
{
    if (filename.endsWith(".jpg"))
    {
        writeJPEGXMLFile(filename, fps, size);
    }
    if (filename.endsWith(".264"))
    {
        writeH264XMLFile(filename, fps, size);
    }
}
