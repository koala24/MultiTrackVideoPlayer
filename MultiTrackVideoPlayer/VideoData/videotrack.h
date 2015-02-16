#ifndef VIDEOTRACK_H
#define VIDEOTRACK_H

#include "ivideotrack.h"
#include "ivideogop.h"
#include "videogop.h"
#include "helpers_template.h"
#include "xmlsupport.h"

#include <QFile>
#include <QSize>
#include <QXmlStreamWriter>
#include <QFileInfo>

class VideoWindow;

class VideoTrack : public XmlSupport, virtual public IVideoTrack
{
private:
    QString _trackName;
    QString _fileName;
    QString _fileType;
    QFileInfo _fileInfo;
    QSize _videoSize;
    VideoWindow *_assignedWindow;
    QList<IVideoGop*> _videoGops;
    QList<IVideoPicture*> _videoPictures;

public:
    VideoTrack();
    VideoTrack(QString trackName);
    VideoTrack(QString trackName, QString fileName, QString fileType);
    VideoTrack(const VideoTrack& original);
    virtual ~VideoTrack();

    virtual void setAssignedWindow( VideoWindow *window );
    void setTrackName(const QString trackName);
    void setFileName(const QString fileName);
    void setFileType(const QString fileType);
    void setFileInfo(const QFileInfo fileInfo);
    void setVideoSize(const QSize videoSize);

    virtual VideoWindow* getAssignedWindow();
    QString getTrackName() const;
    QString getFileName() const;
    QString getFileType() const;
    QFileInfo getFileInfo() const;
    QSize getVideoSize() const;
    virtual QList<IVideoGop*> getVideoGops() const;
    virtual QList<IVideoPicture*> getVideoPictureList() const;

    virtual void addVideoGop(IVideoGop *videoGop);
    virtual void addPictureToPictureList(IVideoPicture *picture);
    virtual int countGops();
    virtual int countPictures();
    virtual IVideoGop* findGopByTimestamp(quint64 timestamp) const;
    virtual IVideoGop* findGopByOffset(quint64 offset) const;
    virtual IVideoGop* findGopByPicture(const IVideoPicture& videoPicture) const;
    virtual IVideoPicture* findPicByTimestamp(quint64 timestamp) const;
    virtual IVideoPicture* findPicByOffset(quint64 offset) const;
    virtual int findTheHighestTimestamp();

    virtual void addMembersToXmlStreamWriter(QXmlStreamWriter *writer);
    virtual void getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token);

    virtual IVideoTrack* clone() const;
    virtual QString toString();

    VideoTrack& operator =(const VideoTrack& otherVideoTrack);
    IVideoGop* operator[](const int index);

private:
    virtual void clear();
};

#endif // VIDEOTRACK_H
