#include "videogop.h"
/**
 *
 */
VideoGop::VideoGop()
    : XmlSupport("VideoGopDefaultXML")
{
    this->_offset = 0;
    this->_timestamp = 0;
    _videoPictures = QList<IVideoPicture*>();
    _parentTrack = NULL;
}

VideoGop::VideoGop(IVideoTrack *parent)
    : XmlSupport("VideoGopDefaultXML")
{
    this->_offset = 0;
    this->_timestamp = 0;
    _videoPictures = QList<IVideoPicture*>();
    _parentTrack = parent;
}
/**
 *
 */
VideoGop::VideoGop(quint64 offset, quint64 timestamp)
    : XmlSupport("VideoGopDefaultXML")
{
    this->_offset = offset;
    this->_timestamp = timestamp;
    _videoPictures = QList<IVideoPicture*>();
    _parentTrack = NULL;
}
/**
 *
 */
VideoGop::VideoGop(const VideoGop& original)
    : XmlSupport(original)
{
    _offset = original._offset;
    _timestamp = original._timestamp;
    _videoPictures = createDeepCopyOfQList(original._videoPictures);
}
/**
 *
 */
VideoGop::~VideoGop()
{
    clear();
}
/**
 *
 */
void VideoGop::setOffset(const quint64 offset)
{
    this->_offset = offset;
}
/**
 *
 */
void VideoGop::setTimestamp(const quint64 timestamp)
{
    this->_timestamp = timestamp;
}
/**
 *
 */
quint64 VideoGop::getOffset() const
{
    return _offset;
}
/**
 *
 */
quint64 VideoGop::getTimestamp() const
{
    return _timestamp;
}
/**
 *
 */
void VideoGop::setGroupOfVideoPictures(const QList<IVideoPicture*> gop)
{
    clear();
    _videoPictures = createDeepCopyOfQList(gop);
}
/**
 *
 */
QList<IVideoPicture*> VideoGop::getVideoPictures() const
{
    return _videoPictures;
}

IVideoTrack *VideoGop::getParentTrack() const {
    return _parentTrack;
}
/**
 *
 */
void VideoGop::addVideoPicture(IVideoPicture *videoPicture)
{
    _videoPictures.append(videoPicture);
}
/**
 *
 */
int VideoGop::countPictures()
{
    return _videoPictures.size();
}
/**
 *
 */
bool VideoGop::contains(const IVideoPicture& videoPicture)
{
    return _videoPictures.contains(videoPicture.clone());
}
/**
 *
 */
bool VideoGop::has(const IVideoPicture& videoPicture)
{
   bool has = false;
   for (QList<IVideoPicture*>::const_iterator i = _videoPictures.constBegin(); i != _videoPictures.constEnd() && !has; ++i)
   {
       if ((*i)->equals(videoPicture))
       {
           has = true;
       }
   }
   return has;
}
/**
 *
 */
IVideoPicture* VideoGop::findPicByTimestamp(quint64 timestamp) const
{
    IVideoPicture *videoPicture = NULL;
    for (QList<IVideoPicture*>::const_iterator i = _videoPictures.constBegin(); i != _videoPictures.constEnd() && videoPicture == NULL; ++i)
    {
        if ((*i)->getTimestamp() == timestamp)
        {
            videoPicture = (*i);
        }
    }
    return videoPicture;
}
/**
 *
 */
IVideoPicture* VideoGop::findPicByOffset(quint64 offset) const
{
    IVideoPicture *videoPicture = NULL;
    for (QList<IVideoPicture*>::const_iterator i = _videoPictures.constBegin(); i != _videoPictures.constEnd() && videoPicture == NULL; ++i)
    {
        if ((*i)->getOffset() == offset)
        {
            videoPicture = (*i);
        }
    }
    return videoPicture;
}
/**
 *
 */
int VideoGop::findTheHighestTimestamp()
{
    quint64 ts = 0;
    foreach (IVideoPicture* pic, _videoPictures)
    {
        if (pic->getTimestamp() > ts)
        {
            ts = pic->getTimestamp();
        }
    }
    return ts;
}
/**
 *
 */
void VideoGop::addMembersToXmlStreamWriter(QXmlStreamWriter *writer)
{
    writer->writeStartElement("VideoGop");
    writer->writeAttribute("VideoPictures", QString::number(_videoPictures.size()));
    //writer->writeAttribute("Timestamp", QString::number(_timestamp));
    //writer->writeAttribute("Offset", QString::number(_offset));

    foreach (IVideoPicture* pic, _videoPictures)
    {
        pic->addMembersToXmlStreamWriter(writer);
    }

    writer->writeEndElement();
}
/**
 *
 */
void VideoGop::getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token)
{
    if (token == QXmlStreamReader::StartElement && reader->name() == "VideoGop")
    {
        QXmlStreamAttributes attributes = reader->attributes();
        int videoPicturesSize = attributes.value("VideoPictures").toString().toInt();

        clear();
        for (int i = 0; i < videoPicturesSize; i++)
        {
            VideoPicture *pic = new VideoPicture();
            _parentTrack->addPictureToPictureList(pic);
            _videoPictures.append(pic);
        }

        _timestamp = attributes.value("Timestamp").toString().toLongLong();
        _offset = attributes.value("Offset").toString().toLongLong();

        if (videoPicturesSize > 0) {
            while (reader->name() != "VideoPicture")
            {
                token = reader->readNext();
                if (token == QXmlStreamReader::StartElement) {
                    if (reader->name() == "VideoPicture")
                    {
                        for (QList<IVideoPicture*>::const_iterator i = _videoPictures.begin(); i != _videoPictures.end(); ++i)
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
VideoGop* VideoGop::clone() const
{
    return new VideoGop(*this);
}
/**
 *
 */
QString VideoGop::toString() const
{
    QString gop = "[GroupOfPicture Offset: " + QString::number(_offset)
            + " Timestamp: " +  QString::number(_timestamp) + "]\n";

    for (QList<IVideoPicture*>::const_iterator i = _videoPictures.begin(); i != _videoPictures.end(); ++i)
    {
        gop += (*i)->toString() + "\n";
    }
    gop += "[/GroupOfPicture]";
    return gop;
}
/**
 *
 */
bool VideoGop::operator==(const IVideoPicture& videoPicture) const
{
    bool containsVideoPicture = false;
    const VideoPicture *object = dynamic_cast<const VideoPicture*>(&videoPicture);
    if (object != NULL)
    {
        for (QList<IVideoPicture*>::const_iterator i = _videoPictures.constBegin(); i != _videoPictures.constEnd() && !containsVideoPicture; ++i)
        {
            if (dynamic_cast<const VideoPicture*>(*i)->operator ==(*object))
            {
                containsVideoPicture = true;
            }
        }
    }
    return containsVideoPicture;
}
/**
 *
 */
bool VideoGop::operator!=(const IVideoPicture& videoPicture) const
{
    return !(*this == videoPicture);
}
/**
 *
 */
bool VideoGop::operator==(const IVideoGop& videoGop) const
{
    bool equals = false;
    const VideoGop *object = dynamic_cast<const VideoGop*>(&videoGop);
    if (object != NULL)
    {
        equals = (_timestamp == object->_timestamp)
                && (_offset == object->_offset)
                && (_videoPictures == object->_videoPictures);
    }
    return equals;
}
/**
 *
 */
bool VideoGop::operator!=(const IVideoGop& videoGop) const
{
    return !(*this == videoGop);
}
/**
 *
 */
VideoGop& VideoGop::operator=(const VideoGop& otherVideoGop)
{
    if (this != &otherVideoGop)
    {
        clear();
        XmlSupport::operator=(otherVideoGop);

        _offset = otherVideoGop._offset;
        _timestamp = otherVideoGop._timestamp;
        _videoPictures = createDeepCopyOfQList(otherVideoGop._videoPictures);
    }
    return *this;
}
/**
 *
 */
void VideoGop::clear()
{
    if (!_videoPictures.isEmpty()) {
        qDeleteAll(_videoPictures);
        _videoPictures.clear();
    }
}
