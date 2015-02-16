#ifndef VIDEOPICTURE_H
#define VIDEOPICTURE_H

#include <QtGlobal>
#include "videogop.h"
#include "ivideogop.h"
#include "ivideopicture.h"
#include <QDebug>
#include "xmlsupport.h"

class VideoGop;

class VideoPicture : public XmlSupport, virtual public IVideoPicture
{
private:
    quint64 _timestamp;
    quint64 _offset;
    IVideoGop *_gop;

public:
    VideoPicture();
    VideoPicture(quint64 timestamp, quint64 offset);
    VideoPicture(quint64 timestamp, quint64 offset, IVideoGop *gop);
    VideoPicture(const VideoPicture& original);
    virtual ~VideoPicture();

    virtual void setTimestamp(const quint64 timestamp);
    virtual void setOffset(const quint64 offset);
    void setVideoGop(IVideoGop *gop);
    virtual quint64 getTimestamp() const;
    virtual quint64 getOffset() const;
    VideoGop* getVideoGop() const;

    virtual void addMembersToXmlStreamWriter(QXmlStreamWriter *writer);
    virtual void getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token);

    virtual IVideoPicture* clone() const;
    virtual QString toString() const;

    virtual bool equals(const IVideoPicture& videoPicture) const;
    virtual bool operator==(const IVideoPicture& videoPicture) const;
    virtual bool operator!=(const IVideoPicture& videoPicture) const;
    virtual bool operator==(const VideoPicture& videoPicture) const;
    virtual bool operator!=(const VideoPicture& videoPicture) const;

    VideoPicture& operator=(const VideoPicture& otherVideoPicture);
};

#endif // VIDEOPICTURE_H

