#include "videotrackcollection.h"
/**
 *
 */
VideoTrackCollection::VideoTrackCollection()
    : XmlSupport("VideoTrackCollectionDefaultXML")
{
    _videoTracks = QList<IVideoTrack*>();
}
/**
 *
 */
VideoTrackCollection::VideoTrackCollection(QList<IVideoTrack*> videoTracks)
    : XmlSupport("VideoTrackCollectionDefaultXML")
{
    _videoTracks = createDeepCopyOfQList(videoTracks);
}
/**
 *
 */
VideoTrackCollection::VideoTrackCollection(const VideoTrackCollection& original)
    : XmlSupport(original)
{
    _videoTracks = createDeepCopyOfQList(original._videoTracks);
}
/**
 *
 */
VideoTrackCollection::~VideoTrackCollection()
{
    clear();
}
/**
 *
 */
void VideoTrackCollection::setVideoTracks(const QList<IVideoTrack*> videoTrack)
{
    clear();
     _videoTracks = createDeepCopyOfQList(videoTrack);
}
/**
 *
 */
QList<IVideoTrack*> VideoTrackCollection::getVideoTracks() const
{
    return _videoTracks;
}
/**
 *
 */
void VideoTrackCollection::addVideoTrack(IVideoTrack *videoTrack)
{
    _videoTracks.append(videoTrack);
}
/**
 *
 */
int VideoTrackCollection::count()
{
    return _videoTracks.size();
}
/**
 * @brief VideoTrackCollection::findTheHighestTimestamp
 * @return
 */
int VideoTrackCollection::findTheHighestTimestamp()
{
    int ts = 0;
    int trackTs = 0;
    foreach (IVideoTrack* videoTrack, _videoTracks)
    {
        trackTs = videoTrack->findTheHighestTimestamp();
        if (trackTs > ts)
        {
            ts = trackTs;
        }
    }
    return ts;
}
/**
 *
 */
void VideoTrackCollection::addMembersToXmlStreamWriter(QXmlStreamWriter *writer)
{
    writer->writeStartElement("VideoTrackCollection");
    writer->writeAttribute("VideoTracks", QString::number(_videoTracks.size()));

    foreach (IVideoTrack* videoTrack, _videoTracks)
    {
        videoTrack->addMembersToXmlStreamWriter(writer);
    }

    writer->writeEndElement();
}
/**
 *
 */
void VideoTrackCollection::getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token)
{
    if (token == QXmlStreamReader::StartElement && reader->name() == "VideoTrack")
    {
        QXmlStreamAttributes attributes = reader->attributes();
        int videoTracksSize = attributes.value("VideoTracks").toString().toInt();

        clear();
        for (int i = 0; i < videoTracksSize; i++)
        {
            _videoTracks.append(new VideoTrack());
        }
        if (videoTracksSize > 0) {
            while (reader->name() != "VideoTrack") {
                token = reader->readNext();
                if (token == QXmlStreamReader::StartElement) {

                    if (reader->name() == "VideoTrack")
                    {
                        for (QList<IVideoTrack*>::const_iterator i = _videoTracks.constBegin(); i != _videoTracks.constEnd(); ++i)
                        {
                            (*i)->getMembersFromXmlStreamReader(reader, token);
                        }
                    }
                }
            }
        }
    } else {
        token = reader->readNext();
        if (token != QXmlStreamReader::EndDocument) {
            getMembersFromXmlStreamReader(reader, token);
        }
    }
}
/**
 *
 */
void VideoTrackCollection::writeSeperateXMLFiles()
{
    for (QList<IVideoTrack*>::const_iterator i = _videoTracks.constBegin(); i != _videoTracks.constEnd(); ++i)
    {
        (*i)->writeXMLFile();
    }
}
/**
 *
 */
void VideoTrackCollection::readSeperateXMLFiles()
{
    for (QList<IVideoTrack*>::const_iterator i = _videoTracks.constBegin(); i != _videoTracks.constEnd(); ++i)
    {
        (*i)->readXMLFile();
    }
}
/**
 *
 */
QString VideoTrackCollection::toString() const
{
    QString trackCollection = "[VideoTrackCollection]\n";

    for (QList<IVideoTrack*>::const_iterator i = _videoTracks.constBegin(); i != _videoTracks.constEnd(); ++i)
    {
        trackCollection += (*i)->toString() + "\n";
    }

    trackCollection += "[/VideoTrackCollection]";
    return trackCollection;
}
/**
 *
 */
VideoTrackCollection& VideoTrackCollection::operator =(const VideoTrackCollection& otherVideoTrackCollection)
{
    if (this != &otherVideoTrackCollection)
    {
        XmlSupport::operator=(otherVideoTrackCollection);
        clear();
        for (QList<IVideoTrack*>::const_iterator i = otherVideoTrackCollection._videoTracks.constBegin();
             i != otherVideoTrackCollection._videoTracks.constEnd(); ++i)
        {
            _videoTracks.append((*i));
        }
    }
    return *this;
}
/**
 *
 */
void VideoTrackCollection::clear()
{
    if (!_videoTracks.isEmpty())
    {
        qDeleteAll(_videoTracks);
        _videoTracks.clear();
    }
}
