#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include "h264parser.h"
#include "VideoData/videopicture.h"
#include "VideoData/videogop.h"
#include "VideoData/videotrack.h"

class Parser
{
private:
    Parser();
public:
    static void writeJPEGXMLFile(QString filename, int fps, QSize size);
    static void writeH264XMLFile(QString filename, int fps, QSize size);
    static void createXMLFile(QString filename, int fps, QSize size);
};

#endif // PARSER_H
