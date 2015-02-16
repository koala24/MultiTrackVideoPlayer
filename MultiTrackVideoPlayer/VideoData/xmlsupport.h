#ifndef XMLSUPPORT_H
#define XMLSUPPORT_H

#include "ixmlsupport.h"
#include <QString>
#include <QFile>
#include <QXmlStreamWriter>

class XmlSupport : virtual public IXmlSupport
{
private:
    QString _fileName;
public:
    XmlSupport();
    XmlSupport(QString fileName);

    void setFileName(const QString fileName);
    QString getFileName() const;

    virtual void addMembersToXmlStreamWriter(QXmlStreamWriter *writer) = 0;
    virtual void getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token) = 0;

    virtual void writeXMLFile(QFile *file);
    virtual void writeXMLFile(const QString fileName);
    virtual void writeXMLFile();
    virtual bool readXMLFile(QFile *file);
    virtual bool readXMLFile(const QString fileName);
    virtual bool readXMLFile();
};

#endif // XMLSUPPORT_H
