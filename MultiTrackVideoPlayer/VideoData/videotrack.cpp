#include "videotrack.h"
#include "gui/VideoWindow.h"
/**
 *
 */
VideoTrack::VideoTrack()
    : XmlSupport("VideoTrackDefaultXML"), _trackName("none"), _fileName("none"), _fileType(' '), _fileInfo(), _videoSize(1, 1),
      _videoPictures()
{
    _videoGops = QList<IVideoGop*>();
    _assignedWindow = 0;
}
/**
 *
 */
VideoTrack::VideoTrack(QString trackName)
    : XmlSupport("VideoTrackDefaultXML"), _trackName(trackName), _fileName("none"), _fileType(' '), _fileInfo(), _videoSize(0, 0),
      _videoPictures()
{
    _videoGops = QList<IVideoGop*>();
    _assignedWindow = 0;
}
/**
 *
 */
VideoTrack::VideoTrack(QString trackName, QString fileName, QString fileType)
    : XmlSupport(fileName), _trackName(trackName), _fileName(fileName), _fileType(fileType), _fileInfo(), _videoSize(0, 0),
      _videoPictures()
{
    _videoGops = QList<IVideoGop*>();
    _assignedWindow = 0;
}
/**
 *
 */
VideoTrack::VideoTrack(const VideoTrack& original)
    : XmlSupport(original), _trackName(original._trackName), _fileName(original._fileName), _fileType(original._fileType), _fileInfo(original._fileInfo), _videoSize(original._videoSize),
    _videoPictures()
{
    _videoGops = createDeepCopyOfQList(original._videoGops);
    _assignedWindow = 0;
}
/**
 *
 */
VideoTrack::~VideoTrack()
{
    clear();
}

void VideoTrack::setAssignedWindow(VideoWindow *window) {
    _assignedWindow = window;
}
/**
 *
 */
void VideoTrack::setTrackName(const QString trackName)
{
    this->_trackName = trackName;
}
/**
 *
 */
void VideoTrack::setFileName(const QString fileName)
{
    XmlSupport::setFileName(fileName);
    this->_fileName = fileName;
}
/**
 *
 */
void VideoTrack::setFileType(const QString fileType)
{
    this->_fileType = fileType;
}
/**
 *
 */
void VideoTrack::setFileInfo(const QFileInfo fileInfo)
{
    _fileInfo = fileInfo;
}
/**
 *
 */
void VideoTrack::setVideoSize(const QSize videoSize)
{
    this->_videoSize = videoSize;
}

VideoWindow *VideoTrack::getAssignedWindow() {
    return _assignedWindow;
}
/**
 *
 */
QString VideoTrack::getTrackName() const
{
    return _trackName;
}
/**
 *
 */
QString VideoTrack::getFileName() const
{
    return _fileName;
}
/**
 *
 */
QString VideoTrack::getFileType() const
{
    return _fileType;
}
/**
 *
 */
QFileInfo VideoTrack::getFileInfo() const
{
    return _fileInfo;
}
/**
 *
 */
QSize VideoTrack::getVideoSize() const
{
    return _videoSize;
}
/**
 *
 */
QList<IVideoGop*> VideoTrack::getVideoGops() const
{
    return _videoGops;
}

QList<IVideoPicture *> VideoTrack::getVideoPictureList() const {
    return _videoPictures;
}
/**
 *
 */
void VideoTrack::addVideoGop(IVideoGop *videoGop)
{
    _videoGops.append(videoGop);
}

void VideoTrack::addPictureToPictureList(IVideoPicture *picture) {
    _videoPictures.append( picture );
}
/**
 *
 */
int VideoTrack::countGops()
{
    return _videoGops.size();
}
/**
 *
 */
int VideoTrack::countPictures()
{
    int count = 0;
    for (QList<IVideoGop*>::const_iterator i = _videoGops.constBegin(); i != _videoGops.constEnd(); ++i)
    {
        count += (*i)->countPictures();
    }
    return count;
}
/**
 *
 */
IVideoGop* VideoTrack::findGopByTimestamp(quint64 timestamp) const
{
    IVideoGop *gop = NULL;
    for (QList<IVideoGop*>::const_iterator i = _videoGops.constBegin(); i != _videoGops.constEnd() && gop == NULL; ++i)
    {
        if ((*i)->getTimestamp() == timestamp)
        {
            gop = (*i);
        }
    }
    return gop;
}
/**
 *
 */
IVideoGop* VideoTrack::findGopByOffset(quint64 offset) const
{
    IVideoGop *gop = NULL;
    for (QList<IVideoGop*>::const_iterator i = _videoGops.constBegin(); i != _videoGops.constEnd() && gop == NULL; ++i)
    {
        if ((*i)->getOffset() == offset)
        {
            gop = (*i);
        }
    }
    return gop;
}
/**
 *
 */
IVideoGop* VideoTrack::findGopByPicture(const IVideoPicture& videoPicture) const
{
    IVideoGop *gop = NULL;
    for (QList<IVideoGop*>::const_iterator i = _videoGops.constBegin(); i != _videoGops.constEnd() && gop == NULL; ++i)
    {
        if ((*i)->has(videoPicture))
        {
            gop = (*i);
        }
    }
    return gop;
}
/**
 *
 */
IVideoPicture* VideoTrack::findPicByTimestamp(quint64 timestamp) const
{
    IVideoPicture *videoPicture = NULL;
    for (QList<IVideoGop*>::const_iterator i = _videoGops.constBegin(); i != _videoGops.constEnd() && videoPicture == NULL; ++i)
    {
        videoPicture = (*i)->findPicByTimestamp(timestamp);
    }
    return videoPicture;
}
/**
 *
 */
IVideoPicture* VideoTrack::findPicByOffset(quint64 offset) const
{
    IVideoPicture *videoPicture = NULL;
    for (QList<IVideoGop*>::const_iterator i = _videoGops.constBegin(); i != _videoGops.constEnd() && videoPicture == NULL; ++i)
    {
        videoPicture = (*i)->findPicByOffset(offset);
    }
    return videoPicture;
}
/**
 * @brief VideoTrack::findTheHighestTimestamp
 * @return
 */
int VideoTrack::findTheHighestTimestamp()
{
    int ts = 0;
    int gopTs = 0;
    foreach (IVideoGop* gop, _videoGops)
    {
        gopTs = gop->findTheHighestTimestamp();
        if (gopTs > ts)
        {
            ts = gopTs;
        }
    }
    return ts;
}
/**
 *
 */
void VideoTrack::addMembersToXmlStreamWriter(QXmlStreamWriter *writer)
{
    writer->writeStartElement("VideoTrack");
    writer->writeAttribute("VideoGops", QString::number(_videoGops.size()));

    writer->writeTextElement("TrackName", _trackName);
    writer->writeTextElement("FileName", _fileName);
    writer->writeTextElement("FileType", QString(_fileType));
    writer->writeTextElement("VideoHeight", QString::number(_videoSize.height()));
    writer->writeTextElement("VideoWidth", QString::number(_videoSize.width()));

    foreach (IVideoGop* gop, _videoGops)
    {
        gop->addMembersToXmlStreamWriter(writer);
    }

    writer->writeEndElement();
}
/**
 *
 */
void VideoTrack::getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token)
{
    if (token == QXmlStreamReader::StartElement && reader->name() == "VideoTrack")
    {
        QXmlStreamAttributes attributes = reader->attributes();
        int videoGopsSize = attributes.value("VideoGops").toString().toInt();

        clear();
        for (int i = 0; i < videoGopsSize; i++)
        {
            _videoGops.append(new VideoGop(this));
        }
        if (videoGopsSize > 0) {
            while (reader->name() != "VideoGop") {
                token = reader->readNext();
                if (token == QXmlStreamReader::StartElement) {
                    if (reader->name() == "TrackName")
                    {
                        reader->readNext();
                        _trackName = reader->text().toString();
                    }

                    if (reader->name() == "FileName")
                    {
                        reader->readNext();
                        _fileName = reader->text().toString();
                    }

                    if (reader->name() == "FileType")
                    {
                        reader->readNext();
                        _fileType = reader->text().toString()[0];
                    }

                    if (reader->name() == "VideoHeight")
                    {
                        reader->readNext();
                        _videoSize.setHeight(reader->text().toString().toInt());
                    }

                    if (reader->name() == "VideoWidth")
                    {
                        reader->readNext();
                        _videoSize.setWidth(reader->text().toString().toInt());
                    }

                    if (reader->name() == "VideoGop")
                    {
                        for (QList<IVideoGop*>::const_iterator i = _videoGops.constBegin(); i != _videoGops.constEnd(); ++i)
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
IVideoTrack* VideoTrack::clone() const
{
    return new VideoTrack(*this);
}
/**
 *
 */
QString VideoTrack::toString()
{
    QString track = "[VideoTrack Trackname: " + _trackName + "]\n";

    for (QList<IVideoGop*>::iterator i = _videoGops.begin(); i != _videoGops.end(); ++i)
    {
        track += (*i)->toString() + "\n";
    }

    track += "[/VideoTrack]";
    return track;
}
/**
 *
 */
VideoTrack& VideoTrack::operator =(const VideoTrack& otherVideoTrack)
{
    if (this != &otherVideoTrack)
    {
        XmlSupport::operator=(otherVideoTrack);
        clear();
        _trackName = otherVideoTrack._trackName;
        _fileName = otherVideoTrack._fileName;
        _fileType = otherVideoTrack._fileType;
        _fileInfo = otherVideoTrack._fileInfo;
        _videoSize = otherVideoTrack._videoSize;
        _videoGops = createDeepCopyOfQList(otherVideoTrack._videoGops);
        _videoPictures = createDeepCopyOfQList(otherVideoTrack._videoPictures);
    }
    return *this;
}
/**
 *
 */
void VideoTrack::clear()
{
    if (!_videoGops.isEmpty()) {
        qDeleteAll(_videoGops);
        _videoGops.clear();
    }

    if (!_videoPictures.isEmpty())
        _videoPictures.clear();

    if (_assignedWindow != 0)
        delete _assignedWindow;
}
