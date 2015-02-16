#ifndef IVIDEOPICTURE_H
#define IVIDEOPICTURE_H

#include <QtGlobal>
#include <QString>
#include <QXmlStreamWriter>

#include "ixmlsupport.h"


class IVideoPicture : virtual public IXmlSupport
{
public:
    virtual ~IVideoPicture() {}

    virtual void setTimestamp(const quint64 timestamp) = 0;
    virtual void setOffset(const quint64 offset) = 0;
    virtual quint64 getTimestamp() const = 0;
    virtual quint64 getOffset() const = 0;

    virtual IVideoPicture* clone() const = 0;
    virtual QString toString() const = 0;

    virtual bool equals(const IVideoPicture& videoPicture) const = 0;
    virtual bool operator==(const IVideoPicture& videoPicture) const = 0;
    virtual bool operator!=(const IVideoPicture& videoPicture) const = 0;
};

#endif // IVIDEOPICTURE_H
