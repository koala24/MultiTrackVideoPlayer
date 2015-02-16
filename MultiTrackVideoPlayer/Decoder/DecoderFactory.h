#ifndef DECODERFACTORY_H
#define DECODERFACTORY_H

#include "VideoData/videotrack.h"
#include "Decoder/IDecoder.h"

class DecoderFactory
{
public:
    static IDecoder* buildDecoder( VideoTrack *track );
};

#endif // DECODERFACTORY_H
