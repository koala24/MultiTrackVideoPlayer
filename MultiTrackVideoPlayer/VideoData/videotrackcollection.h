#ifndef VIDEOTRACKCOLLECTION_H
#define VIDEOTRACKCOLLECTION_H

#include "ivideotrackcollection.h"
#include "ivideotrack.h"
#include "videotrack.h"
#include "xmlsupport.h"
#include "helpers_template.h"

#include <QList>

class VideoTrackCollection : public XmlSupport, virtual public IVideoTrackCollection
{
private:
    QList<IVideoTrack*> _videoTracks;

public:
    VideoTrackCollection();
    VideoTrackCollection(QList<IVideoTrack*> videoTracks);
    VideoTrackCollection(const VideoTrackCollection& original);
    virtual ~VideoTrackCollection();

    virtual void setVideoTracks(const QList<IVideoTrack*> videoTrack);
    virtual QList<IVideoTrack*> getVideoTracks() const;

    virtual void addVideoTrack(IVideoTrack *videoTrack);
    virtual int count();

    virtual int findTheHighestTimestamp();

    virtual void addMembersToXmlStreamWriter(QXmlStreamWriter *writer);
    virtual void getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token);
    virtual void writeSeperateXMLFiles();
    virtual void readSeperateXMLFiles();

    virtual QString toString() const ;

    VideoTrackCollection& operator =(const VideoTrackCollection& otherVideoTrackCollection);

private:
    void clear();
};

#endif // VIDEOTRACKCOLLECTION_H
