#ifndef VIDEOGOP_H
#define VIDEOGOP_H

#include "ivideogop.h"
#include "ivideopicture.h"
#include "videopicture.h"
#include "videotrack.h"
#include "xmlsupport.h"
#include <QString>
#include <QChar>
#include <QtGlobal>
#include <QList>
#include <QDebug>

#include "helpers_template.h"

class VideoGop : public XmlSupport, virtual public IVideoGop
{
private:
    quint64 _offset;
    quint64 _timestamp;
    IVideoTrack *_parentTrack;
    QList<IVideoPicture*> _videoPictures;

public:
    VideoGop();
    VideoGop(IVideoTrack *parent);
    VideoGop(quint64 offset, quint64 timestamp);
    VideoGop(const VideoGop& original);
    virtual ~VideoGop();

    virtual void setOffset(const quint64 offset);
    virtual void setTimestamp(const quint64 timestamp);
    virtual void setGroupOfVideoPictures(const QList<IVideoPicture*> gop);
    virtual quint64 getOffset() const;
    virtual quint64 getTimestamp() const;
    virtual QList<IVideoPicture*> getVideoPictures() const;
    virtual IVideoTrack *getParentTrack() const;

    virtual void addVideoPicture(IVideoPicture *videoPicture);
    virtual int countPictures();
    virtual bool contains(const IVideoPicture& videoPicture);
    virtual bool has(const IVideoPicture& videoPicture);
    virtual IVideoPicture* findPicByTimestamp(quint64 timestamp) const;
    virtual IVideoPicture* findPicByOffset(quint64 offset) const;
    virtual int findTheHighestTimestamp();

    virtual void addMembersToXmlStreamWriter(QXmlStreamWriter *writer);
    virtual void getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token);

    virtual VideoGop* clone() const;
    virtual QString toString() const;

    virtual bool operator==(const IVideoPicture& videoGop) const;
    virtual bool operator!=(const IVideoPicture& videoGop) const;
    virtual bool operator==(const IVideoGop& videoGop) const;
    virtual bool operator!=(const IVideoGop& videoGop) const;

    VideoGop& operator=(const VideoGop& otherVideoGop);

private:
    virtual void clear();
};

#endif // VIDEOGOP_H
