#pragma once
#pragma pack(push,1)

enum SUB_STREAM_TYPE
{
	SUB_STREAM_NONE = 0,
	SUB_STREAM_AC3 = 1,
	SUB_STREAM_LPCM = 2,
	SUB_STREAM_DTS = 3,
	SUB_STREAM_MPEGAUDIO = 4,
	SUB_STREAM_SUBPICTURE = 10,
};

typedef struct _stStreamFormatInfo
{
	int		nBitrate;
	union 
	{
		struct ///for video
		{
			WORD	nWidth;
			WORD	nHeight;
			BYTE	progressive_sequence;
			BYTE	chroma_format;
			WORD	nRate;
			WORD	nScale;
			BYTE	picture_structure;
			BYTE	bitflags;
			BYTE	Profile;
			BYTE	Level;
			BYTE	nBCount;
			BYTE	nPCount;
			BYTE	closed_gop;
			BYTE	aspect_ratio;
		};
		struct ///for audio
		{
			int		nSampleFrequency;
			BYTE	sub_stream_type;
			BYTE	Channel;
			BYTE	Version;
			BYTE	Layer;
			BYTE	SampleBits;
			BYTE	bCBR;
		};
	};
}StreamFormatInfo;

struct ES_Info 
{
	WORD				stream_type;
	WORD				sub_stream_type;
	WORD				elementary_PID;
	WORD				CA_PID;
	BYTE				scrambling_control;
	StreamFormatInfo	esFormat;
	ES_Info()
	{
		memset(this,0,sizeof(ES_Info));
	}
};

struct program_info
{
	program_info()
	{
		memset(this,0,sizeof(program_info));
	}
	WORD			program_number;
	WORD			program_map_PID;
	WORD			PCR_PID;
	WORD			es_count;
	ES_Info			es_info[20];
	char			service_name[255];
};

struct TS_StreamInfo
{
	TS_StreamInfo()
	{
		memset(this,0,sizeof(TS_StreamInfo));
	}
	WORD	transport_stream_id;
	DWORD	dwTSBitRate;///bps
	int		nCount;
	program_info	program[100];
};

typedef struct _stESSampleInfo
{
	unsigned long		size;
	unsigned long		StartPTS;
	union 
	{
		struct ///for video,
		{
			WORD	nWidth;
			WORD	nHeight;
			BYTE	progressive_sequence;
			BYTE	chroma_format;
			WORD	nRate;
			WORD	nScale;

			BYTE	frame_type;
			BYTE	picture_structure;
			BYTE	bitflags;
			BYTE	fields;
		};
		struct ///for audio
		{
			int		nSampleFrequency;
			int		nBitrate;
			BYTE	sub_stream_type;
			BYTE	Channel;
			BYTE	MPEGVersion;
			BYTE	Layer;
			WORD	wPCMSamples;
		};
	};
	_stESSampleInfo()
	{
		memset(this, 0, sizeof(_stESSampleInfo) );
	}
}ESSampleInfo;

enum VIDEOENCODETYPE
{
	V_TYPE_H264		= 0XA1,
	V_TYPE_MPEG2	= 0XA2,
};
enum AUDIOENCODETYPE
{
	A_TYPE_AAC	= 0xB1,
	A_TYPE_AC3	= 0XB2,
	A_TYPE_MPEG = 0xB3,
	A_TYPE_MP2  = 0xB4,
	A_TYPE_PCM  = 0xB5,
};

struct ESINFO_H264
{
	DWORD	dwChannelID;
	int		program_map_PID;
	DWORD	dwAudioChannelNum;//当前音频声道数
	VIDEOENCODETYPE Vtype;
	AUDIOENCODETYPE Atype;
	ES_Info stVideoInfo;
	ES_Info stAudioInfo[8];
};
#pragma  pack(pop)


enum PC_TSINFO_NOTIFY_TYPE
{
	PC_TSINFO_POST_PGMLIST = 0,
	PC_TSINFO_PGMLIST_CHANNING = 1,
	PC_TSINFO_INFOMATION = 2,
	PC_TSINFO_VIDEO_MULTIFIELD = 3,
	PC_TSINFO_PGM_LOST=4,
	PC_TSINFO_PGM_LOST2=5,
};
typedef int (*PC_TSStreamInfoNotifyCB)(int nType, void* pNotifyParam, int nNotifyParamSize, void* pParam);
typedef int (*PC_PostESDataCB)(int nPID, LPBYTE pData, ESSampleInfo* pSampleInfo, void *pUserData);
//时间信息
/*
 *	 //悲剧，需要在回调中加一个结构体，
 *    又不忍心加载 SBT_TSDataStruct.h
 */
struct TimeInf_shit
{
	unsigned __int64 pts;	// 播放时间
	unsigned __int64 dts;	// 解码时间
};
typedef int (*PC_PostESDataCB2)(int pid, int prg_id, char * buffer, int buffer_len, 
						  int error_code,
						  void *user_data, BOOL end, TimeInf_shit *p_time_inf,
						  void *reserved_inf, int reserved_inf_size);
