#ifndef IVIDEOGOP_H
#define IVIDEOGOP_H

#include <QtGlobal>
#include <QString>
#include "ivideotrack.h"
#include "ivideopicture.h"
#include "ixmlsupport.h"

class IVideoTrack;

class IVideoGop : virtual public IXmlSupport
{
public:
    virtual ~IVideoGop() {}

    virtual void setOffset(const quint64 offset) = 0;
    virtual void setTimestamp(const quint64 timestamp) = 0;
    virtual void setGroupOfVideoPictures(const QList<IVideoPicture*> gop) = 0;
    virtual quint64 getOffset() const = 0;
    virtual quint64 getTimestamp() const = 0;
    virtual QList<IVideoPicture*> getVideoPictures() const = 0;
    virtual IVideoTrack *getParentTrack() const = 0;

    virtual void addVideoPicture(IVideoPicture *videoPicture) = 0;
    virtual int countPictures() = 0;
    virtual bool contains(const IVideoPicture& videoPicture) = 0;
    virtual bool has(const IVideoPicture& videoPicture) = 0;
    virtual IVideoPicture* findPicByTimestamp(quint64 timestamp) const = 0;
    virtual IVideoPicture* findPicByOffset(quint64 offset) const = 0;
    virtual int findTheHighestTimestamp() = 0;

    virtual IVideoGop* clone() const = 0;
    virtual QString toString() const = 0;

    virtual bool operator==(const IVideoPicture& videoGop) const = 0;
    virtual bool operator!=(const IVideoPicture& videoGop) const = 0;
    virtual bool operator==(const IVideoGop& videoGop) const = 0;
    virtual bool operator!=(const IVideoGop& videoGop) const = 0;

};

#endif // IVIDEOGOP_H
