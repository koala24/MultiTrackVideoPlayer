#include "videotrackcollection.h"
#include "videotrack.h"
#include "videogop.h"

#include <QString>
#include <QChar>
#include <QDir>
#include <QtCore>

#include <iostream>
#include <vector>
#include <iterator>
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>


typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

class OffsetType{
public:
    int offset;
    int type;
};

// Annex B stream (byte oriented)
class Bitstream
{
public:
	Bitstream( FILE *f ) :
		f(f),
		bits_in_buffer(0),
		bytes_read(0),
		has_eof(false)
	{
	}

	// return next n bits of input stream without advancing stream pointer
	uint32_t next_bits(int n)
	{
		assert( 0 <= n && n <= 32 );

		while( bits_in_buffer < n )
		{
			assert( bits_in_buffer <= 24 );
			int c = fgetc(f);
			if(c==EOF)
			{
				if( feof(f) )
				{
                    // printf("EOF reached\n");
					has_eof = true;
					break;
				}
				else 
				{
					int err = ferror(f);
					printf("fgetc error %d\n", err );
					has_eof = true;
					break;
				}
			}
			bytes_read++;

			bit_buffer <<= 8;
			bit_buffer |= (uint32_t)(0xFF & c);
			bits_in_buffer += 8;
		}

		return bit_buffer >> (bits_in_buffer-n);
	}

	uint32_t read_bits(int n)
	{
		uint32_t result = next_bits(n);
		bits_in_buffer -= n;
		bit_buffer &= (1UL<<bits_in_buffer)-1; // mask out leading bits
		return result;
	}

	int byte_offset()
	{
		return bytes_read - bits_in_buffer/8; 
	}

	bool eof()
	{
		return has_eof;
	}

private:
	uint32_t bit_buffer; 
	int bits_in_buffer;
	int bytes_read;
	FILE *f;
	bool has_eof;
};

class Rbsp // raw byte sequence payload
{
public:
	Rbsp() :
		bytes_read(0),
		bytes_in_buffer(0),
		bit_buffer(0),
		bits_read(0),
		bits_in_buffer(0),
		rbsp_size(10000000)
	{
		rbsp = new uint8_t[rbsp_size];
	}

	~Rbsp()
	{
		delete[] rbsp;
	}

	// rbsp must be filled by calling appendByte first
	// once next_bits or read_bits was called, no more bytes must be appended
	void appendByte(uint8_t byte)
	{
		assert( bytes_in_buffer < rbsp_size ); // this is lean but mean!
		rbsp[bytes_in_buffer++] = byte;
	}

	// return next n bits of input stream without advancing stream pointer
	uint32_t next_bits(int n)
	{
		assert( 0 <= n && n <= 32 );

		while( bits_in_buffer < n )
		{
			assert( bits_in_buffer <= 24 );
			int c = get_byte();
			if(c==EOF)
			{
                // printf("EOF reached\n");
				break;
			}

			bit_buffer <<= 8;
			bit_buffer |= (uint32_t)(0xFF & c);
			bits_in_buffer += 8;
		}

		return bit_buffer >> (bits_in_buffer-n);
	}

	uint32_t read_bits(int n)
	{
		uint32_t result = next_bits(n);
		bits_in_buffer -= n;
		bit_buffer &= (1UL<<bits_in_buffer)-1; // mask out leading bits
		bits_read += n;
		return result;
	}

	int bits_in_rbsp()
	{
		int result = 8*bytes_in_buffer;
		uint8_t last_byte = rbsp[bytes_in_buffer-1];
		assert(last_byte!=0x00); 
		while( !(last_byte & 1) )
		{
			result--;
			last_byte>>=1;
		}
		result--; // trailing 1 bit
		return result;
	}

	bool more_rbsp_data()
	{
		return bits_read < bits_in_rbsp();
	}

protected:
	int get_byte()
	{
		if( bytes_read < bytes_in_buffer )
			return rbsp[bytes_read++];
		return EOF;
	}

private:
	const int rbsp_size;
	uint8_t *rbsp;
	int bytes_in_buffer;
	int bytes_read;
	int bits_read;
	int bits_in_buffer;
	uint32_t bit_buffer; 
};

int parse_nonIDRpicture(Rbsp &rbsp)
{
	printf("picture\n" );
	return 0;
}

int parse_IDRpicture(Rbsp &rbsp)
{
	printf("IDR picture\n" );
	return 0;
}

int parse_SEI_message(Rbsp &rbsp)
{
	printf("     SEI message: " );

	int payloadType = 0;
	for(;;)
	{
		 uint32_t nal_byte = rbsp.read_bits(8);
		 if(nal_byte==0xFF)
			 payloadType += 255;
		 else
		 {
			 payloadType += nal_byte;
			 break;
		 }
	}

	int payloadSize = 0;
	for(;;)
	{
		 uint32_t nal_byte = rbsp.read_bits(8);
		 if(nal_byte==0xFF)
			 payloadSize += 255;
		 else
		 {
			 payloadSize += nal_byte;
			 break;
		 }
	}

	printf("payloadType=%2d; payloadSize=%2d payload=", payloadType, payloadSize );
	for( int i=0; i<payloadSize; i++ )
	{
		 uint32_t nal_byte = rbsp.read_bits(8);
		 printf("0x%02x ", nal_byte );
	}
	printf("\n");

	return 0;
}

int parse_SEI(Rbsp &rbsp)
{
	printf("SEI:\n" );
	while( rbsp.more_rbsp_data() )
	{
		parse_SEI_message(rbsp);
	}
	return 0;
}

int parse_SPS(Rbsp &rbsp)
{
	printf("SPS\n");
	return 0;
}

int parse_PPS(Rbsp &rbsp)
{
	printf("PPS\n");
	return 0;
}

int parse_AU(Rbsp &rbsp)
{
	uint32_t primary_pic_type = rbsp.read_bits(3);
	printf("AU primary_pic_type=%d\n", primary_pic_type );
	return 0;
}

int nal_unit_header_svc_extension(Bitstream &bitstream)
{
	int svc_extension_flag       = bitstream.read_bits(1);
	if(svc_extension_flag==1)
	{
		//
		int idr_flag                 = bitstream.read_bits(1);
		int priority_id              = bitstream.read_bits(6);

		int no_inter_layer_pred_flag = bitstream.read_bits(1);	// DM814x: 1 i.e. no inter-layer prediction
		int dependency_id            = bitstream.read_bits(3);	// shall be 0 in prefix-NAL
		int quality_id               = bitstream.read_bits(4);
		
		int temporal_id              = bitstream.read_bits(3);	// DM814x: 0..3 (for 4 temporal layers)
		int use_ref_base_pic_flag    = bitstream.read_bits(1);
		int discardable_flag         = bitstream.read_bits(1);
		int output_flag              = bitstream.read_bits(1);	// DM814x: 1 
		int reserved_three_2bits     = bitstream.read_bits(2);
		printf("SVC_extension: priority_id=%d; quality_id=%d; temporal_id=%d\n", priority_id, quality_id, temporal_id );
	}
	else
	{
		//nal_unit_header_mvc_extension
		assert(0);
	}
	return 0;
}

int parse_SVC_prefixNAL(Rbsp &rbsp)
{
	//printf("SVC prefix NAL: ");
	//printf("\n");
	return 0;
}

OffsetType parseNALU(Bitstream &bitstream)
{
  OffsetType ot;

	while( bitstream.next_bits(24) != 0x000001 && bitstream.next_bits(32) != 0x00000001 )
	{
		 uint32_t leading_zero_8bits = bitstream.read_bits(8);
		 assert(leading_zero_8bits==0x00);
	}
	if( bitstream.next_bits(24) != 0x000001 )
	{
		 uint32_t leading_zero_8bits = bitstream.read_bits(8);
		 assert(leading_zero_8bits==0x00);
	}
	
	uint32_t start_code_prefix_one_3bytes =  bitstream.read_bits(24);
	assert( start_code_prefix_one_3bytes==0x000001 );

    // printf("offset %10d: NAL unit ", bitstream.byte_offset() );
  ot.offset = bitstream.byte_offset();

	uint32_t forbidden_zero_bit = bitstream.read_bits(1);
	uint32_t nal_ref_idc = bitstream.read_bits(2);
	uint32_t nal_unit_type = bitstream.read_bits(5);
	
	if(nal_unit_type==14 || nal_unit_type==20) // three more header bytes follow!
	{
		nal_unit_header_svc_extension(bitstream);
	}

	// rbsp syntax starts here
	Rbsp rbsp;

	while( !bitstream.eof() && bitstream.next_bits(24) != 0x000001 && bitstream.next_bits(32) != 0x00000001 )
	{
		if( !bitstream.eof() && bitstream.next_bits(24) == 0x000003 )
		{	// actually not eof but "NAL contains >=3 bytes must be checked"
			rbsp.appendByte( bitstream.read_bits(8) );
			rbsp.appendByte( bitstream.read_bits(8) );
			uint32_t emulation_prevention_three_byte = bitstream.read_bits(8);
			assert(emulation_prevention_three_byte==0x03);
		}
		else
		{
			rbsp.appendByte( bitstream.read_bits(8) );
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
        printf("offset %10d: unhandled nal_unit_type=%2d\n", bitstream.byte_offset(), nal_unit_type );
	}

  ot.type = nal_unit_type;
  //qDebug() << nal_unit_type << ot.offset << ot.type;
  return ot;
}

QVector<OffsetType> parseAnnexB(FILE *f)
{
	Bitstream bitstream(f);
    QVector<OffsetType> offsets;

	while( !bitstream.eof() )
	{
        OffsetType ot = parseNALU(bitstream);
        if(ot.type)
            offsets.append(ot);
	}
    return offsets;
}

void write_xml_stream_file(QString outfile, QString name, int fps, QSize size, FILE *f){
    QChar type;
    qreal msec = 1000.0 / (qreal)fps;
    uint timestamp = 0;
    uint i = 0;

    QVector<OffsetType> offsettype = parseAnnexB(f);

    VideoTrack *vt = new VideoTrack(outfile, name, type);
    IVideoGop *gop;
    IVideoPicture *picture;

    for(QVector<OffsetType>::const_iterator it = offsettype.constBegin(); 
        it != offsettype.constEnd(); 
        ++it, ++i){

      timestamp = qRound(msec * i);
      if(it->type != 1){
        gop = new VideoGop();
        picture = new VideoPicture(timestamp, it->offset - 4);
        gop->addVideoPicture(picture);
        while(it->type != 1){
          it++;
        }
        i++;
      }
      timestamp = qRound(msec * i);
      if(it->type == 1){ //non-IDR
        picture = new VideoPicture(timestamp, it->offset - 4);
        gop->addVideoPicture(picture);
      }

      if((it + 1)->type != 1){
        vt->addVideoGop(gop);
      }

      if(it == offsettype.constEnd())
        vt->addVideoGop(gop);
    }

    vt->setVideoSize(size);
    vt->setFileType(QString("H264"));
    vt->setTrackName(name);
    vt->writeXMLFile();
    delete vt;
}


int main( int argc, char *argv[] )
{
	FILE *f;

    if(argc!=4){
        printf("usage: %s <input-file> <outputfile> <fps>\n", argv[0]);
		return -1;
	}
	f = fopen(argv[1], "rb");
    QFile qfile(argv[1]);
    QFileInfo fileInfo(qfile.fileName());
    QString filename(fileInfo.fileName());

	if(f==NULL){
		perror("open input file");
		return EXIT_FAILURE;
	}
    QString outfile(argv[2]);

    int fps = atoi(argv[3]);
    QSize size(640, 480);

    write_xml_stream_file(outfile, filename, fps, size, f);

    fclose(f);

	return 0;
}
