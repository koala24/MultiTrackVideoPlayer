#ifndef IVIDEOTRACK_H
#define IVIDEOTRACK_H

#include "ivideopicture.h"
#include "ivideogop.h"
#include "ixmlsupport.h"

#include <QtGlobal>
#include <QString>
#include <QFile>
#include <QFileInfo>

class IVideoGop;


class IVideoTrack : virtual public IXmlSupport
{
public:
    virtual ~IVideoTrack() {}

    virtual void setTrackName(const QString trackName) = 0;
    virtual void setFileName(const QString fileName) = 0;
    virtual void setFileType(const QString fileType) = 0;
    virtual void setFileInfo(const QFileInfo fileInfo) = 0;
    virtual void setVideoSize(const QSize videoSize) = 0;

    virtual QString getTrackName() const = 0;
    virtual QString getFileName() const = 0;
    virtual QString getFileType() const = 0;
    virtual QFileInfo getFileInfo() const = 0;
    virtual QSize getVideoSize() const = 0;
    virtual QList<IVideoGop*> getVideoGops() const = 0;
    virtual QList<IVideoPicture*> getVideoPictureList() const = 0;

    virtual void addVideoGop(IVideoGop *videoGop) = 0;
    virtual void addPictureToPictureList(IVideoPicture *picture) = 0;
    virtual int countGops() = 0;
    virtual int countPictures() = 0;
    virtual IVideoGop* findGopByTimestamp(quint64 timestamp) const = 0;
    virtual IVideoGop* findGopByOffset(quint64 offset) const = 0;
    virtual IVideoGop* findGopByPicture(const IVideoPicture& videoPicture) const = 0;
    virtual IVideoPicture* findPicByTimestamp(quint64 timestamp) const = 0;
    virtual IVideoPicture* findPicByOffset(quint64 offset) const = 0;
    virtual int findTheHighestTimestamp() = 0;

    virtual IVideoTrack* clone() const = 0;
    virtual QString toString() = 0;

};

#endif // IVIDEOTRACK_H
