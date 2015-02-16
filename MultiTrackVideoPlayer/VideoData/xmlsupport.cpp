#include "xmlsupport.h"

/**
 *
 */
XmlSupport::XmlSupport() : _fileName("default")
{
}
/**
 *
 */
XmlSupport::XmlSupport(QString fileName) : _fileName(fileName)
{
}
/**
 *
 */
void XmlSupport::setFileName(const QString fileName)
{
    _fileName = fileName;
}
/**
 *
 */
QString XmlSupport::getFileName() const
{
    return _fileName;
}
/**
 *
 */
void XmlSupport::writeXMLFile(QFile *file)
{
    QXmlStreamWriter *xmlWriter = new QXmlStreamWriter();
    xmlWriter->setAutoFormatting(true);
    xmlWriter->setDevice(file);
    xmlWriter->writeStartDocument();
    addMembersToXmlStreamWriter(xmlWriter);
    xmlWriter->writeEndDocument();
    delete xmlWriter;
}
/**
 *
 */
void XmlSupport::writeXMLFile(const QString fileName)
{
    QFile file(fileName + ".xml");
    if (file.open(QIODevice::WriteOnly))
    {
        writeXMLFile(&file);
        file.close();
    }
}
/**
 *
 */
void XmlSupport::writeXMLFile()
{
    writeXMLFile(_fileName);
}
/**
 *  @return true, wenn Datei erfolgreich gelesen wurde
 */
bool XmlSupport::readXMLFile(QFile *file)
{
    bool error = false;
    QXmlStreamReader *xmlReader = new QXmlStreamReader();
    xmlReader->setDevice(file);

    while (!xmlReader->atEnd() && !(error = xmlReader->hasError())) {
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        if (token != QXmlStreamReader::StartDocument) {
            getMembersFromXmlStreamReader(xmlReader, token);
        }
    }

    delete xmlReader;
    return !error;
}
/**
 * @return true, wenn Datei erfolgreich gelesen wurde
 */
bool XmlSupport::readXMLFile(const QString fileName)
{
    bool done = false;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        done = readXMLFile(&file);
        file.close();
    }
    return done;
}
/**
 * @return true, wenn Datei erfolgreich gelesen wurde
 */
bool XmlSupport::readXMLFile()
{
    return readXMLFile(_fileName);
}
