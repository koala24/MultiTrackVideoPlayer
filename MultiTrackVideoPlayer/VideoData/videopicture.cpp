#include "videopicture.h"
/**
 *
 */
VideoPicture::VideoPicture()
    : XmlSupport("VideoPictureDefaultXML"), _gop()
{
    _timestamp = 0;
    _offset = 0;
}
/**
 *
 */
VideoPicture::VideoPicture(quint64 timestamp, quint64 offset)
    : XmlSupport("VideoPictureDefaultXML"), _gop()
{
    this->_timestamp = timestamp;
    this->_offset = offset;
}
/**
 *
 */
VideoPicture::VideoPicture(quint64 timestamp, quint64 offset, IVideoGop *gop)
    : XmlSupport("VideoPictureDefaultXML"), _gop(gop)
{
    this->_timestamp = timestamp;
    this->_offset = offset;
}
/**
 *
 */
VideoPicture::VideoPicture(const VideoPicture& original)
    : XmlSupport(original)
{
    this->_timestamp = original._timestamp;
    this->_offset = original._offset;
    this->_gop = original._gop;
}
/**
 *
 */
VideoPicture::~VideoPicture()
{

}
/**
 *
 */
void VideoPicture::setTimestamp(const quint64 timestamp)
{
    this->_timestamp = timestamp;
}
/**
 *
 */
void VideoPicture::setOffset(const quint64 offset)
{
    this->_offset = offset;
}
/**
 * @brief VideoPicture::setVideoGop
 * @param gop
 */
void VideoPicture::setVideoGop(IVideoGop *gop)
{
    _gop = gop;
}
/**
 *
 */
quint64 VideoPicture::getTimestamp() const
{
    return _timestamp;
}
/**
 *
 */
quint64 VideoPicture::getOffset() const
{
    return _offset;
}
/**
 * @brief VideoPicture::getVideoGop
 * @return
 */
VideoGop* VideoPicture::getVideoGop() const
{
    return dynamic_cast<VideoGop *>( _gop );
}
/**
 *
 */
void VideoPicture::addMembersToXmlStreamWriter(QXmlStreamWriter *writer)
{
    writer->writeStartElement("VideoPicture");
    writer->writeTextElement("Offset", QString::number(_offset));
    writer->writeTextElement("Timestamp",  QString::number(_timestamp));
    writer->writeEndElement();
}
/**
 *
 */
void VideoPicture::getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token)
{
    if (token == QXmlStreamReader::StartElement && reader->name() == "VideoPicture")
    {
        while (reader->name() != "Timestamp")
        {
            token = reader->readNext();
            if (token == QXmlStreamReader::StartElement)
            {
                if (reader->name() == "Offset")
                {
                    reader->readNext();
                    _offset = reader->text().toString().toULongLong();
                }

                if (reader->name() == "Timestamp")
                {
                    reader->readNext();
                    _timestamp = reader->text().toString().toULongLong();
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
IVideoPicture* VideoPicture::clone() const
{
    return new VideoPicture((*this)._timestamp, (*this)._offset);
}
/**
 *
 */
QString VideoPicture::toString() const
{
    return "[VideoPicture] Offset: " + QString::number(_offset)
            + " Timestamp: " + QString::number(_timestamp)
            + " [/VideoPicture]";
}
/**
 *
 */
bool VideoPicture::equals(const IVideoPicture& videoPicture) const
{
    bool equals = false;
    const VideoPicture *object = dynamic_cast<const VideoPicture*>(&videoPicture);
    if (object != NULL)
    {
        equals = (_timestamp == object->_timestamp) && (_offset == object->_offset);
    }
    return equals;
}
/**
 *
 */
bool VideoPicture::operator==(const IVideoPicture& videoPicture) const
{
    bool equals = false;
    const VideoPicture *object = dynamic_cast<const VideoPicture*>(&videoPicture);
    if (object != NULL)
    {
        equals = (_timestamp == object->_timestamp) && (_offset == object->_offset);
    }
    return equals;
}
/**
 *
 */
bool VideoPicture::operator!=(const IVideoPicture& videoPicture) const
{
    return !(*this == videoPicture);
}
/**
 *
 */
bool VideoPicture::operator==(const VideoPicture& videoPicture) const
{
    return (_timestamp == videoPicture._timestamp) && (_offset == videoPicture._offset);
}
/**
 *
 */
bool VideoPicture::operator!=(const VideoPicture& videoPicture) const
{
    return !(*this == videoPicture);
}
/**
 *
 */
VideoPicture& VideoPicture::operator=(const VideoPicture& otherVideoPicture)
{
    if (this != &otherVideoPicture)
    {
        XmlSupport::operator=(otherVideoPicture);
        this->_timestamp = otherVideoPicture._timestamp;
        this->_offset = otherVideoPicture._offset;
        this->_gop = otherVideoPicture._gop;
    }
    return *this;
}
