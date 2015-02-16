#include "DecoderFactory.h"

#include "Decoder/H264Decoder.h"
#include "Decoder/JpegDecoder.h"

IDecoder* DecoderFactory::buildDecoder( VideoTrack *track ) {
    if ( track->getFileType() == "H" )
        return new H264Decoder( track->getFileInfo().absolutePath(), track );
    else if ( track->getFileType() == "J" )
        return new JpegDecoder( track->getFileInfo().absolutePath(), track );
    else
        return 0;
}
