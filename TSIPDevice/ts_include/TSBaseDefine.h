#pragma once

#define SYNC_HEADER	0x47
#define PTS_CLOCK	90000

#define	PAT_PID			0x00
#define CAT_PID			0x01
#define TSDT_PID		0x02	
#define NIT_ST_PID		0x10
#define SDT_BAT_ST_PID	0x11
#define EIT_ST_PID		0x12
#define RST_ST_PID		0x13
#define TDT_TOT_ST_PID	0x14
#define NET_SYNC_PID	0x15
#define BAND_PID		0x1C
#define TEST_PID		0x1D
#define DIT_PID			0x1E
#define SIT_PID			0x1F
#define EMT_PID			0x20
#define NULL_PACKET_PID	0x1FFF

enum TableId 
{
	PAT_TABLE_ID			= 0x00,	// program_association_section
	CAT_TABLE_ID			= 0x01,	// conditional_access_section //
	PMT_TABLE_ID			= 0x02,	// TS_program_map_section //
	TSDT_TABLE_ID			= 0x03,	// TS_description_section //
	SDT_TABLE_ID			= 0x04,	// ISO_IEC_14496_scene_description_section //
	ODT_TABLE_ID			= 0x05,	// ISO_IEC_14496_object_descriptor_section //

	SDT_ACTUAL_TABLE_ID		= 0x42,	// service_description_section - actual_transport_stream //
	SDT_OTHER_TABLE_ID		= 0x46,	// service_description_section - other_transport_stream //
	BAT_TABLE_ID				= 0x4A,	// bouquet_association_section //

	RESERVED_TABLE_ID		= 0xFF
};

//stream type
enum StreamType 
{
	STT_RESERVED			= 0x00,
	STT_MPEG1_VIDEO			= 0x01,		// MPEG-1 video
	STT_MPEG2_VIDEO			= 0x02,		// MPEG-2 video
	STT_MPEG1_AUDIO			= 0x03,		// MPEG-1 audio
	STT_MPEG2_AUDIO			= 0x04,		// MPEG-2 audio
	STT_MPEG2_PRIVATE_SECTIONS	= 0x05,
	STT_MPEG2_PES_PRIVATE_DATA	= 0x06,	///DVB standard:	Descriptor Tag :0x6A  AC-3 audio, 0x73 dts audio

	STT_PRIVATE_STREAM		= 0x80,
	STT_AC3_STREAM			= 0x81,
	STT_H264_VIDEO          = 0x85,
	STT_AAC_AUDIO           = 0x86,
	STT_PCM_AUDIO			= 0x87,
};


/* code definition */
#define PICTURE_START_CODE			0x100
#define SLICE_START_CODE_MIN		0x101
#define SLICE_START_CODE_MAX		0x1AF
#define USER_DATA_START_CODE		0x1B2
#define SEQUENCE_START_CODE			0x1B3
#define EXTENSION_START_CODE		0x1B5
#define SEQUENCE_END_CODE			0x1B7
#define GROUP_START_CODE			0x1B8
////// Table 6-2. extension_start_code_identifier codes.
#define SEQUENCE_EXTENSION_ID					1
#define SEQUENCE_DISPLAY_EXTENSION_ID			2
#define QUANT_MATRIX_EXTENSION_ID				3
#define COPYRIGHT_EXTENSION_ID					4
#define SEQUENCE_SCALABLE_EXTENSION_ID			5
#define PICTURE_DISPLAY_EXTENSION_ID			7
#define PICTURE_CODING_EXTENSION_ID				8
#define PICTURE_SPATIAL_SCALABLE_EXTENSION_ID	9
#define PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID	10

#define SYSTEM_END_CODE				0x1B9
#define PACK_START_CODE				0x1BA
#define SYSTEM_START_CODE			0x1BB

#define PRIVATE_STREAM_1			0xBD
#define PADDING_STREAM				0xBE
#define PRIVATE_STREAM_2			0xBF
#define AUDIO_ELEMENTARY_STREAM_0	0xC0
#define AUDIO_ELEMENTARY_STREAM_MAX	0xDF
#define VIDEO_ELEMENTARY_STREAM_0	0xE0
#define VIDEO_ELEMENTARY_STREAM_MAX	0xEF


#ifndef MPEG_FRAME_TYPE_I
#define MPEG_FRAME_TYPE_I	1
#define MPEG_FRAME_TYPE_P	2
#define MPEG_FRAME_TYPE_B	3
#endif

// Picture structures
#define TOP_FIELD		1
#define BOTTOM_FIELD	2
#define FRAME_PICTURE	3

// flags for MPEGSampleInfo.bitflags
#define PICTUREFLAG_RFF		1	// repeat_first_field
#define PICTUREFLAG_TFF		2	// top_field_first
#define PICTUREFLAG_PF		4	// progressive_frame

// Chroma formats
#define CHROMA420       1
#define CHROMA422       2
#define CHROMA444       3

//aspect_ratio
//   0 = Square Pels
//   1 = 4:3 Display
//   2 = 16:9 Display
//   3 = 2.21:1 Display

#define FORMAT_UNKNOWN		0
#define FORMAT_MPEG1VIDEO	1
#define FORMAT_MPEG2VIDEO	2
#define FORMAT_AC3			3
#define FORMAT_MPA			4
#define FORMAT_MPEG2Ext		5
#define FORMAT_LPCM			6
#define FORMAT_DTS			7
#define FORMAT_SDDS			8


enum ts_packet_mode
{
	TS_PACKET_LOST = 0,
	TS_PACKET_188  = 188,
	TS_PACKET_204  = 204, 
};

#pragma pack(push,1)
struct Transport_Packet
{
	BYTE	sync_byte					: 8;
	BYTE	pidH						: 5; //d0 - d4
	BYTE    transport_priority			: 1; //d5
	BYTE	payload_unit_start_indicator: 1; //d6
	BYTE	transport_error_indicator	: 1; //d7
	BYTE	pidL						: 8;
	BYTE	continuity_counter			: 4; //d0 - d3
	BYTE	adaptation_field_control	: 2; //d4 - d5
	BYTE	transport_scrambling_control: 2; //d6 - d7

	WORD pid()
	{
		return ((pidH << 8) | pidL);
	}
	BYTE adaptation_field_length()
	{
		if(transport_error_indicator)
			return 0;
		if(adaptation_field_control & 2)
			return *((LPBYTE)this+4);
		return 0;
	}
	LPBYTE get_adaptation_field()
	{
		if(transport_error_indicator)
			return NULL;
		if(adaptation_field_length())
			return ((LPBYTE)this + 5);
		else
			return NULL;
	}
	LPBYTE	get_payload_field()
	{
		if(transport_error_indicator)
			return NULL;
		if(adaptation_field_control & 1)
		{
			if(adaptation_field_control & 2)
			{
				int n = adaptation_field_length();
				return ((LPBYTE)this + 5 + n);
			}
			else
				return ((LPBYTE)this+4);
		}
		else
			return NULL;
	}
	LPBYTE	psi_table_section()
	{
		if(transport_error_indicator)
			return NULL;
		LPBYTE pTemp = get_payload_field();
		if(pTemp == NULL)
			return pTemp;
		if(payload_unit_start_indicator)
		{
			BYTE pointer_filed = *pTemp;
			return (pTemp + 1 + pointer_filed);
		}
		return pTemp;
	}
};

struct Adaptation_Field
{
	BYTE	adaptation_field_extension_flag		: 1; //d0
	BYTE	transport_private_data_flag			: 1; //d1
	BYTE	splicing_point_flag					: 1; //d2
	BYTE	OPCR_flag							: 1; //d3
	BYTE	PCR_flag							: 1; //d4
	BYTE	elementary_stream_priority_indicator: 1; //d5
	BYTE	random_access_indicator				: 1; //d6
	BYTE	discontinuity_indicator				: 1; //d7
	LONGLONG	program_clock_refence_base()
	{
		if(PCR_flag)
		{
			LPBYTE	pbuf = (LPBYTE)this+1;
			return ((pbuf[0]<<25) | (pbuf[1]<<17) | (pbuf[2]<<9) | (pbuf[3]<<1)  | (pbuf[4]>>7)) & 0x1ffffffff;
		}
		return 0;
	}
	LONGLONG   program_clock_refence_extension()
	{
		if(PCR_flag)
		{
			LPBYTE	pbuf = (LPBYTE)this+6;
			return (((pbuf[0] & 1)<<8) | pbuf[1]) & 0x1FF;
		}
		return 0;
	}
	LONGLONG   program_clock_refence()
	{
		return program_clock_refence_base()*300 + program_clock_refence_extension();
	}
	LONGLONG	original_program_clock_refence_base()
	{
		if(OPCR_flag)
		{
			LPBYTE pBuf = PCR_flag ? (LPBYTE)this + 7 : (LPBYTE)this + 1;
			return ((pBuf[0]<<25)|(pBuf[1]<<17)|(pBuf[2]<<9)|(pBuf[3]<<1)|(pBuf[4]>>7));
		}
		return 0;
	}
	LONGLONG   original_program_clock_refence_extension()
	{
		if(OPCR_flag)
		{
			LPBYTE pBuf = PCR_flag ? (LPBYTE)this + 12 : (LPBYTE)this + 6;
			return (((pBuf[0] & 1)<<8) | pBuf[1]);
		}
		return 0;
	}

};

struct Program_Association_Section
{
	struct program_section
	{
		BYTE	program_numberH		: 8;
		BYTE	program_numberL		: 8;
		BYTE	PID_H				: 5;
		BYTE	reserved			: 3;
		BYTE	PID_L				: 8;
		WORD	program_number()
		{
			return (program_numberH<<8)|program_numberL;
		}
		WORD	pid()
		{
			return (PID_H << 8) | PID_L;
		}
	};
	BYTE	table_id					: 8;
	BYTE	section_lengthH				: 4; //b0-3
	BYTE	reservedA					: 2; //b4-5
	BYTE	zero						: 1; //b6
	BYTE	setction_syntax_indicator	: 1; //b7
	BYTE	section_lengthL				: 8;
	BYTE	transport_stream_idH		: 8;
	BYTE	transport_stream_idL		: 8;
	BYTE	current_next_indicator		: 1; //b0
	BYTE	version_number				: 5; //b1-5
	BYTE	reservedB					: 2; //b6-7
	BYTE	section_number				: 8;
	BYTE	last_section_number			: 8;
	program_section	program[100];

	WORD	section_length()
	{	return ( (section_lengthH & 0x3 ) << 8) | section_lengthL;		}
	WORD transport_stream_id()
	{	return transport_stream_idH<<8|transport_stream_idL;	}
	int	program_number()
	{
		//(-5: header length after section_length; -4: crc length)
		return (section_length() - 5 - 4) / 4;
	}
};

struct Program_Map_Section
{
	BYTE	table_id					: 8;
	BYTE	section_lengthH				: 4;
	BYTE	reservedA					: 2;
	BYTE	zero						: 1;
	BYTE	section_syntax_indicator	: 1;
	BYTE	section_lengthL				: 8;
	BYTE	program_numberH				: 8;
	BYTE	program_numberL				: 8;
	BYTE	current_next_indicator		: 1;
	BYTE	version_number				: 5;
	BYTE	reservedB					: 2;
	BYTE	section_number				: 8;
	BYTE	last_section_number			: 8;
	BYTE	PCR_PIDH					: 5;
	BYTE	reservedC					: 3;
	BYTE	PCR_PIDL					: 8;
	BYTE	program_info_lengthH		: 4;
	BYTE	reservedD					: 4;
	BYTE	program_info_lengtL			: 8;

	WORD	section_length()
	{	return (section_lengthH << 8) | section_lengthL;}
	WORD	PCR_PID()
	{	return (PCR_PIDH<< 8)|PCR_PIDL ;	}
	WORD	program_number()
	{	return program_numberH << 8 | program_numberL; }
	WORD	program_info_length()
	{	return (program_info_lengthH << 8) | program_info_lengtL;}
	LPBYTE	descriptor()
	{	return (LPBYTE)this+12;						}
	LPBYTE	es_section()
	{	return (LPBYTE)this+12+program_info_length();}
};

struct es_section
{
	BYTE	stream_type;
	BYTE	elementary_PIDH	: 5;
	BYTE	reservedA		: 3;
	BYTE	elementary_PIDL;
	BYTE	ES_info_lengthH	: 4;
	BYTE	reservedB		: 4;
	BYTE	ES_info_lengthL;
	WORD	elementary_PID()
	{	return (elementary_PIDH << 8) |elementary_PIDL ; }
	WORD	ES_info_length()
	{	return (ES_info_lengthH << 8) | ES_info_lengthL; }
	LPBYTE	descriptor()
	{	return (LPBYTE)this+5;	}
	LPBYTE	next_section()
	{	return (LPBYTE)this	+ 5 + ES_info_length();	}
};

struct Service_descriptor
{
	BYTE	descriptor_tag;
	BYTE	descriptor_length;
	BYTE	service_type;
	BYTE	service_provider_name_length;
	LPBYTE	service_provider_name()
	{	return (LPBYTE)this+4;}
	BYTE	service_name_length()
	{	return *((LPBYTE)this+4+service_provider_name_length);	}
	LPBYTE	service_name()
	{	return (LPBYTE)this+5+service_provider_name_length;		}
	LPBYTE	next_service_descriptor()
	{	return (LPBYTE)this+2+descriptor_length;}
};

struct Service_section
{
	BYTE	service_idH					: 8;
	BYTE	service_idL					: 8;
	BYTE	EIT_present_following_flag	: 1;
	BYTE	EIT_schedule_flag			: 1;
	BYTE	reserved_future_use			: 6;
	BYTE	descriptors_loop_lengthH	: 4;
	BYTE	free_CA_mode				: 1;
	BYTE	running_status				: 3;
	BYTE	descriptors_loop_lengthL	: 8;
	WORD	service_id()
	{	return service_idH<<8|service_idL;	}
	WORD	descriptors_loop_length()
	{	return descriptors_loop_lengthH<<8|descriptors_loop_lengthL;}
	LPBYTE	service_descriptor()
	{	return (LPBYTE)this+5;}
	LPBYTE	next_service_section()
	{	return (LPBYTE)this+5+descriptors_loop_length();}
};

struct Service_description_header
{
	WORD	table_id					: 8;
	WORD	section_lengthH				: 4;
	WORD	reservedA					: 2;
	WORD	reserved_future_useA		: 1;
	WORD	section_syntax_indicator	: 1;
	WORD	section_lengthL				: 8;
	WORD	transport_stream_idH		: 8;
	WORD	transport_stream_idL		: 8;
	WORD	current_next_indicator		: 1;
	WORD	verison_number				: 5;
	WORD	reservedB					: 2;
	WORD	section_number				: 8;
	WORD	last_section_number			: 8;
	WORD	original_network_idH		: 8;
	WORD	original_network_idL		: 8;
	WORD	reserved_future_useB		: 8;
	WORD	section_length()
	{	return (section_lengthH << 8) |section_lengthL;	}
	WORD	transport_stream_id()
	{	return transport_stream_idH<<8|transport_stream_idL;}
	WORD	original_network_id()
	{	return original_network_idH<<8|original_network_idL;}
	LPBYTE	data_section()
	{	return (LPBYTE) this+11;	}
};

struct Service_description_section
{
	Service_description_header	Header;
	BYTE						pSectionData[1024];
	int							nDataSize;
	Service_description_section()
	{
		nDataSize = 0;
	}
	void add_data(LPBYTE pBuffer,int length)
	{
		if((nDataSize + length) > (Header.section_length()-8))
		{
			length = Header.section_length()-8;
			memcpy(pSectionData+nDataSize,pBuffer,length-nDataSize);
			nDataSize = length;
		}
		else
		{
			memcpy(pSectionData+nDataSize,pBuffer,length);
			nDataSize += length;
		}
	}
};

#pragma  pack(pop)

