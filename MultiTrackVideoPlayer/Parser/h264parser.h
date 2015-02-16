#ifndef H264PARSER_H
#define H264PARSER_H

#include "bitstream.h"
#include "rbsp.h"
#include "offsettype.h"
#include "VideoData/videotrack.h"

#include <QFile>
#include <QFileInfo>
#include <QtGlobal>
#include <QVector>

class H264Parser
{
private:
    H264Parser();
public:
    static int parseNonIDRpicture(Rbsp &rbsp);
//    static int parseIDRpicture(Rbsp &rbsp);
    static int parseSEImessage(Rbsp &rbsp);
    static int parseSEI(Rbsp &rbsp);
//    static int parseSPS(Rbsp &rbsp);
//    static int parsePPS(Rbsp &rbsp);
    static int parseAU(Rbsp &rbsp);
    static int nalUnitHeaderSvcExtension(Bitstream &bitstream);
//    static int parseSVCprefixNAL(Rbsp &rbsp);
    static OffsetType parseNALU(Bitstream &bitstream);
    static QVector<OffsetType> parseAnnexB(QFile *file);

};

#endif // H264PARSER_H
