#ifndef IVIDEOTRACKCOLLECTION_H
#define IVIDEOTRACKCOLLECTION_H

#include "videotrack.h"
#include "ixmlsupport.h"
#include <QString>

class IVideoTrackCollection : virtual public IXmlSupport
{
public:
    virtual ~IVideoTrackCollection() {}

    virtual void setVideoTracks(const QList<IVideoTrack*> videoTrack) = 0;
    virtual QList<IVideoTrack*> getVideoTracks() const = 0;

    virtual void addVideoTrack(IVideoTrack *videoTrack) = 0;
    virtual int count() = 0;

    virtual int findTheHighestTimestamp() = 0;

    virtual void addMembersToXmlStreamWriter(QXmlStreamWriter *writer) = 0;
    virtual void getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token) = 0;
    virtual void writeSeperateXMLFiles() = 0;
    virtual void readSeperateXMLFiles() = 0;

    virtual QString toString() const = 0;
};

#endif // IVIDEOTRACKCOLLECTION_H
