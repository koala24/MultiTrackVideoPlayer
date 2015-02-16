#include "h264parser.h"
/**
 * @brief H264Parser::H264Parser
 */
H264Parser::H264Parser()
{
}
/**
 * @brief H264Parser::parseNonIDRpicture
 * @param rbsp
 * @return
 */
//int H264Parser::parseNonIDRpicture(Rbsp &rbsp)
//{
//    return 0;
//}
/**
 * @brief H264Parser::parseIDRpicture
 * @param rbsp
 * @return
 */
//int H264Parser::parseIDRpicture(Rbsp &rbsp)
//{
//    return 0;
//}
/**
 * @brief H264Parser::parseSEImessage
 * @param rbsp
 * @return
 */
int H264Parser::parseSEImessage(Rbsp &rbsp)
{
    int payloadType = 0;
    while (true)
    {
         quint32 nalByte = rbsp.readBits(8);
         if (nalByte == 0xFF)
         {
             payloadType += 255;
         }
         else
         {
             payloadType += nalByte;
             break;
         }
    }

    int payloadSize = 0;
    while (true)
    {
         quint32 nalByte = rbsp.readBits(8);
         if (nalByte == 0xFF)
         {
             payloadSize += 255;
         }
         else
         {
             payloadSize += nalByte;
             break;
         }
    }

    //printf("payloadType=%2d; payloadSize=%2d payload=", payloadType, payloadSize );
    for (int i = 0; i < payloadSize; i++)
    {
         /* quint32 nalByte = */ rbsp.readBits(8);
         //printf("0x%02x ", nalByte );
    }
    return 0;
}
/**
 * @brief H264Parser::parseSEI
 * @param rbsp
 * @return
 */
int H264Parser::parseSEI(Rbsp &rbsp)
{
    while (rbsp.moreRbspData())
    {
        parseSEImessage(rbsp);
    }
    return 0;
}
/**
 * @brief H264Parser::parseSPS
 * @param rbsp
 * @return
 */
//int H264Parser::parseSPS(Rbsp &rbsp)
//{
//    return 0;
//}
/**
 * @brief H264Parser::parsePPS
 * @param rbsp
 * @return
 */
//int H264Parser::parsePPS(Rbsp &rbsp)
//{
//    return 0;
//}
/**
 * @brief H264Parser::parseAU
 * @param rbsp
 * @return
 */
int H264Parser::parseAU(Rbsp &rbsp)
{
    /* quint32 primaryPicType = */ rbsp.readBits(3);
    return 0;
}
/**
 * @brief H264Parser::nalUnitHeaderSvcExtension
 * @param bitstream
 * @return
 */
int H264Parser::nalUnitHeaderSvcExtension(Bitstream &bitstream)
{
    int svc_extension_flag       = bitstream.readBits(1);
    if (svc_extension_flag == 1)
    {
        /* int idr_flag                 = */ bitstream.readBits(1);
        /* int priority_id              = */ bitstream.readBits(6);

        /* int no_inter_layer_pred_flag = */ bitstream.readBits(1);	// DM814x: 1 i.e. no inter-layer prediction
        /* int dependency_id            = */ bitstream.readBits(3);	// shall be 0 in prefix-NAL
        /* int quality_id               = */ bitstream.readBits(4);

        /* int temporal_id              = */ bitstream.readBits(3);	// DM814x: 0..3 (for 4 temporal layers)
        /* int use_ref_base_pic_flag    = */ bitstream.readBits(1);
        /* int discardable_flag         = */ bitstream.readBits(1);
        /* int output_flag              = */ bitstream.readBits(1);	// DM814x: 1
        /* int reserved_three_2bits     = */ bitstream.readBits(2);
        //printf("SVC_extension: priority_id=%d; quality_id=%d; temporal_id=%d\n", priority_id, quality_id, temporal_id );
    }
    else
    {
        //nal_unit_header_mvc_extension
        assert(0);
    }
    return 0;
}
/**
 * @brief H264Parser::parseSVCprefixNAL
 * @param rbsp
 * @return
 */
//int H264Parser::parseSVCprefixNAL(Rbsp &rbsp)
//{
//    return 0;
//}
/**
 * @brief H264Parser::parseNALU
 * @param bitstream
 * @return
 */
OffsetType H264Parser::parseNALU(Bitstream &bitstream)
{
    OffsetType ot;

    while (bitstream.nextBits(24) != 0x000001 && bitstream.nextBits(32) != 0x00000001)
    {
        uint32_t leading_zero_8bits = bitstream.readBits(8);
        assert(leading_zero_8bits == 0x00);
    }

    if ( bitstream.nextBits(24) != 0x000001)
    {
        uint32_t leading_zero_8bits = bitstream.readBits(8);
        assert(leading_zero_8bits == 0x00);
    }

    uint32_t start_code_prefix_one_3bytes = bitstream.readBits(24);
    assert(start_code_prefix_one_3bytes == 0x000001);

    // printf("offset %10d: NAL unit ", bitstream.byte_offset() );
    ot._offset = bitstream.getByteOffset();

    /* uint32_t forbidden_zero_bit = */ bitstream.readBits(1);
    /* uint32_t nal_ref_idc = */ bitstream.readBits(2);
    uint32_t nal_unit_type = bitstream.readBits(5);

    if(nal_unit_type == 14 || nal_unit_type == 20) // three more header bytes follow!
    {
        nalUnitHeaderSvcExtension(bitstream);
    }

    // rbsp syntax starts here
    Rbsp rbsp;

    while (!bitstream.eof() && bitstream.nextBits(24) != 0x000001 && bitstream.nextBits(32) != 0x00000001)
    {
        if (!bitstream.eof() && bitstream.nextBits(24) == 0x000003)
        {	// actually not eof but "NAL contains >=3 bytes must be checked"
            rbsp.appendByte(bitstream.readBits(8));
            rbsp.appendByte(bitstream.readBits(8));
            uint32_t emulation_prevention_three_byte = bitstream.readBits(8);
            assert(emulation_prevention_three_byte == 0x03);
        }
        else
        {
            rbsp.appendByte(bitstream.readBits(8));
        }
    }

    switch(nal_unit_type)
    {
    case 1:
  //		parse_nonIDRpicture(rbsp);
        break;
    case 5:
  //		parse_IDRpicture(rbsp);
        break;
    case 6:
  //		parse_SEI(rbsp);
        break;
    case 7:
  //		parse_SPS(rbsp);
        break;
    case 8:
  //		parse_PPS(rbsp);
        break;
    case 9:
  //		parse_AU(rbsp);
        break;
    case 14:
  //		parse_SVC_prefixNAL(rbsp);
        break;
    default:
        printf("offset %10d: unhandled nal_unit_type=%2d\n", bitstream.getByteOffset(), nal_unit_type);
    }
    ot._type = nal_unit_type;
    return ot;
}
/**
 * @brief H264Parser::parseAnnexB
 * @param file
 * @return
 */
QVector<OffsetType> H264Parser::parseAnnexB(QFile *file)
{
    Bitstream bitstream(file);
    QVector<OffsetType> offsets;

    while (!bitstream.eof())
    {
        OffsetType ot = parseNALU(bitstream);
        if (ot._type)
        {
            offsets.append(ot);
        }
    }
    return offsets;
}

