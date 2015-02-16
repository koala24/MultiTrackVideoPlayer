#ifndef IXMLSUPPORT_H
#define IXMLSUPPORT_H

#include <QFile>
#include <QXmlStreamWriter>

class IXmlSupport {
public:
    virtual void addMembersToXmlStreamWriter(QXmlStreamWriter *writer) = 0;
    virtual void getMembersFromXmlStreamReader(QXmlStreamReader *reader, QXmlStreamReader::TokenType token) = 0;

    virtual void writeXMLFile(QFile *file) = 0;
    virtual void writeXMLFile(const QString fileName) = 0;
    virtual void writeXMLFile() = 0;
    virtual bool readXMLFile(QFile *file) = 0;
    virtual bool readXMLFile(const QString fileName) = 0;
    virtual bool readXMLFile() = 0;
};

#endif // IXMLSUPPORT_H
