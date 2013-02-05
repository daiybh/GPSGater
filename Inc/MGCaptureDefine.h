 #ifndef   _MGCaptureDefine_H_    
#define   _MGCaptureDefine_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vfw.h"
#include <afxtempl.h>

//0x4D5846，用于蓝光引入不拆包直接写文件的标示
#define	 MXFNATIVE 'MXF'

#define  BytePerFrameAudio   6000  //每个帧音频的大小
#define  AUDIO_CHANNEL_COUNT 8 //声道数目

typedef BOOL CALLBACK ERROR_REPORT_CB(void*pParaCB,int nErrorCode,const WCHAR* wcsErrorString); 
typedef BOOL CALLBACK DRAW_PREVIEW_CB(int nWidth,int nHeight,BYTE* pDataBuffer);

//#define  VIDEO_WIDTH    720    //原始宽度
//#define  VIDEO_HEIGHT_P 576    //原始高度 ：PAL
//#define  VIDEO_HEIGHT_N 480    //原始高度 : NTSC
//#define  VIDEO_WIDTH_PREVIEW   320 //缩放后的预览宽度
//#define  VIDEO_HEIGHT_PREVIEW  240 //缩放后的预览高度

   
//Picture format defination
//The BiCompression in BITMAPIBFOHEADER Should match one of the following defines
#define MG_CLRMODEL_RGB  0x00000000		// RGB24 and RGB32
#define MG_CLRMODEL_UYVY 0x59565955		// UYVY 4:2:2, U0 Y0 V0 Y1
#define MG_CLRMODEL_YUY2 0x32595559		// YUY2 4:2:2, Y0 U0 Y1 V0
#define MG_CLRMODEL_IYUV 0x56555949		// YUV 4:2:0, one Y plane match 2*2 UV planes
#define MG_CLRMODEL_UNKNOWN 0xFFFFFFFF




//编码后的视频帧类型
#define MG_BUF_MPEG_UNKNOWN			0x1000
#define MG_BUF_RAW_FRAME			0x1001
#define MG_BUF_MPEG4_ES_KEY			0x01
#define MG_BUF_MPEG4_ES_NOTKEY			0x00
#define MG_BUF_MPEG2_I_PICTURE_BODY		0x1004
#define MG_BUF_MPEG2_P_PICTURE_BODY		0x1005
#define MG_BUF_MPEG2_B_PICTURE_BODY		0x1006

typedef struct stPerAudioInfo
{
	WORD nSize;
	WORD nType;	//0:Normal audio data   1:dolby-e data
	
	stPerAudioInfo()
	{
		nSize = 0;
		nType = 0;
	}
}PERAUDIOINFO;

typedef struct stAudioChannelInfo
{
	int          iAudioNum;
	PERAUDIOINFO stAudioInfo[AUDIO_CHANNEL_COUNT];
}AudioChannelInfo, *PAudioChannelInfo;

//制式
enum MG_VideoStandard
{
	MG_VS_PAL = 0,
	MG_VS_NTSC,
	MG_VS_SECOM,
	MG_VS_NOSINGNASL,
	MG_VS_UNKNOWN,
};
typedef enum _tagSTART_CAP_RET 
{
	START_CAP_SUCCESS = 0,		//成功
	PARAM_ERROR = 1,			//高标清参数错误
	INITIAL_ERROR = 2,			//初始化卡未完成
	SET_PARAM_ERROR = 3,		//参数设置错误
	TASK_RUN_ERROR = 4,			//已经有任务运行
	END_CAP_ERROR = 5,			//结束采集错误
	READY_CAP_ERROR = 6,		//准备采集错误
	PREPARE_CAP_ERROR = 7,		//预采集错误
	START_CAP_ERROR = 8,		//开始采集错误
	PARSE_ERROR = 9,			//参数解析错误
	QUERY_STATE_ERROR = 10,		//查询状态错误
	DBE_CHANNEL_ERROR = 11,	//dbe声道超出2个
	MP3_BITDEPTH_ERROR = 12,			//MP3位深不为16
	DBE_CHANNEL_JUMP = 13,	//dbe声道跳跃，错误
	DBE_CHANNEL_MORE = 14,
	DBE_BITDEPTH_ERROR = 15,
}START_CAP_RET;

//视频端口类型
typedef enum _tagMG_VidAdapt
{
	MG_VidCVBS = 0,
	MG_VidYC	,
	MG_VidYUV	,
	MG_VidSDI	,
	MG_VidESDI,
	MG_VidReserved1,
	MG_VidReserved2,
	MG_VidReserved3,
	MG_VidReserved4,
}MG_VidAdapt;

//音频频端口类型
typedef enum _tagMG_AudAdapt
{
	MG_NoAud = 0,
	MG_AnologeAud,
	MG_DataAud	,
	MG_AudReserved2,
	MG_AudReserved3,
	MG_AudReserved4,
}MG_AudAdapt;

// 写视频文件类型
enum MG_AVWriteType
{
	MG_WRITETYPE_UNKNOWN	= 0,
	MG_WRITETYPE_RIFF		= 1,
	MG_WRITETYPE_MATROX		= 2,
	MG_WRITETYPE_MAV		= 3,
	MG_WRITETYPE_GXF		= 4,
	MG_WRITETYPE_MXF		= 5,
	MG_WRITETYPE_PS			= 6,
	MG_WRITETYPE_WMV		= 7,
	MG_WRITETYPE_AVI2		= 8,	
	MG_WRITETYPE_MP4		= 70,
	MG_WRITETYPE_TS			= 189,
	MG_WRITETYPE_ISMV		= 191,
	MG_WRITETYPE_DONOTWRITEFILE = 0XFF
};

// 写音频文件类型
enum MG_AudioWriteType
{
	MG_AUDIOWRITETYPE_UNKNOWN	= 0,
	MG_AUDIOWRITETYPE_WAV		= 21,
	MG_AUDIOWRITETYPE_MP2		= 22,
	MG_AUDIOWRITETYPE_MP3		= 23,
	MG_AUDIOWRITETYPE_MXF		= 24,
	MG_AUDIOWRITETYPE_AAC		= 25,
	MG_AUDIOWRITETYPE_AC3		= 26,
	MG_AUDIOWRITETYPE_DBE		= 27,
	MG_AUDIOWRITETYPE_HEAAC		= 28,
};

//编码类型
enum MG_EncodeType
{
	MG_ENCTYPE_UNKNOWN	= 0,
	MG_ENCTYPE_MPEG2	= 1,
	MG_ENCTYPE_MPEG4	= 2,
	MG_ENCTYPE_MJPG		= 3,
	MG_ENCTYPE_DV		= 4,
	MG_ENCTYPE_ASF		= 5,
	MG_ENCTYPE_REAL		= 7,
	MG_ENCTYPE_WMV		= 21,		
	MG_ENCTYPE_DNxHD	= 55,
	MG_ENCTYPE_AVCIntra = 56,
	MG_ENCTYPE_H264		= 57,		
};

//二级编码类型
#define MG_MPEG4_MSV2  '24pm'
#define MG_MPEG4_MSV3  'v4pm'

#define	MG_MPEG2TYPE_UNKNOWN 0
#define	MG_MPEG2_SONY_IMX 1				
#define	MG_MPEG2_MATROX_I 2				
#define	MG_MPEG2_MATROX_IBP 3			

#define  MG_DVCAM_DVSD      0  
#define  MG_DVCPRO_DV25     1 
#define  MG_DVCPRO50_DV50   2

#define	MG_WMV_9  0
#define	MG_WMV_8  1

//文件格式
#define  MG_VFILE_MPEG2  'SEMM'   //     MPEG2 I or IBP: bih.biCompression = 'SEMM'
#define  MG_MPEG4_FILE_MSV2  '24pm'
#define  MG_MPEG4_FILE_MSV3  'v4pm'
#define  MG_FILE_DVCAM_DVSD  'dsvd'
#define  MG_FILE_DVCPRO_DV25  '52vd'
#define  MG_FILE_DVCPRO50_DV50 '05vd'
#define  MG_FILE_MJEP         'GPJM'
#define  MG_FILE_DNxHD        'NDVA'

// 流媒体
enum MG_ISMAType
{
	MG_ISMA_UNKNWON = 0,
	MG_ISMA_ASF,
	MG_ISMA_WMV8,
	MG_ISMA_WMV9,
	MG_ISMA_REAL8,
	MG_ISMA_REAL9,
};


//音频模式
enum MG_AudioMode
{
	MG_MODE_UNKNOWN = 0,
	MG_MODE_MONO,
	MG_MODE_STEREO,
};

//音频采样频率
enum MG_AudioFrequency
{
	MG_AF_8000  = 8000,
	MG_AF_32000 = 32000,
	MG_AF_44100 = 44100,
	MG_AF_48000 = 48000, 
};

//板卡状态
typedef enum _tagMgCardStatus
{
	statusCardNoInit = 0,
	statusCardInited,
	statusCardCapturing,
	statusCardCapturePause,
	statusCardTransfering,
	statusCardTransferSeeking,
	statusCardTransferPause,
	statusCardUnknownState,
	statusCardNotSet,
}MgCardStatus;


typedef struct _tagManualKeyFrame
{
	DWORD dwTaskFrameNo;
	DWORD dwTimeCode;
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwBitDepth;
	LPBYTE pPicData;
	
	_tagManualKeyFrame()
	{
		dwTaskFrameNo = -1;
		dwTimeCode = -1;
		dwWidth = 0;
		dwHeight = 0;
		dwBitDepth = 0;
		pPicData = NULL;
	}
}MANUALKEYFRAME;

//采集卡基本信息
typedef struct tagCardInfo  
{
	int  nCardID;	     	//卡ID
	int  nCardTypeID;       //卡类型ID
	BOOL bInitSuccess;      //初始化是否成功 TRUE－成功 FALSE － 失败 
	TCHAR  strCardDescription[MAX_PATH];  //卡描述
	float  fCardVersion;		  //卡版本信息
  
	tagCardInfo()
	{
		nCardID		= -1;
		nCardTypeID  = -1;
		bInitSuccess = FALSE;
		fCardVersion = 0;		 
	}
}CARDINFO,*PCARDINFO;
//编码插件参数
struct MGEncodePlugParam 
{
	////////////common
	int  mpeg_type;  //编码 操作 类型
    ////////////video
    bool video_enable; // =false for disable video stream
						// =true for enable video stream
	int  bit_rate;     // For MPEG-1 and MPEG-2:
                        // Bit rate in bits/sec.  130000 < bit_rate <100000000
	int  video_width;  // width of destination video
	int  video_height; // height of destination video
	int  video_stand;  //video stand 
	int  nFrameRateIndex;
	int  clrModel; //the type of compression for a compressed
	int  nAverageTimePerFrame; 
	int  iGopBCount;
	int  iGopPCount;
    
	////////////audio                   
    bool  audio_enable;// =false 不编码  （目前不进行音频编码！！！！！）
	                   // =true 编码
	int   audio_mode;  // For MPEG-1 and MPEG-2:  0: Mono,1: Stereo
	int   audio_frequency; // For MPEG-1 and MPEG-2:
                           // =44100 (Default) , Valid values are 32000, 44100, or 48000
	int  iVideoDataType;
    MGEncodePlugParam()
	{
		mpeg_type = -1;
		video_enable = TRUE; //
		bit_rate = 0;
		video_width = 0;
		video_height = 0;
		video_stand = MG_VS_PAL;
        clrModel = MG_CLRMODEL_UYVY;
		audio_enable = FALSE; //（目前不进行音频编码！！！！！）
		audio_mode = 1;
		audio_frequency = MG_AF_48000;

		iGopBCount = 2;
		iGopPCount = 4;
		iVideoDataType = -1;
	}
    
};
//编码参数
struct MGEncodeParam 
{
	BOOL bPath0;  //线路0（高质量） 720×576 或者 720×480
	BOOL bAudio0; //线路0 是否产生音频
	MG_EncodeType  nEncodeType0; //编码线路 0 编码类型
	MGEncodePlugParam EncodePlugParam0;
	BOOL bPath1;  //线路1 
	BOOL bAudio1; //线路1 是否产生音频
	MG_EncodeType  nEncodeType1; //编码线路 1 编码类型
	MGEncodePlugParam EncodePlugParam1;
};

struct MGWriteFilePlugParam
{	
	MG_VideoStandard vs;
	int  nFrameRateIndex;	// =1 for 23.97 f/sec  
							// =2 for 24 f/sec
							// =3 for 25 f/sec     
							// =4 for 29.97
							// =5 for 30 f/sec
    int video_Width; //宽度
    int video_height; //长度

	int  nAverageTimePerFrame;
	    
	TCHAR strFileNameV[MAX_PATH];	
	TCHAR strFileNameA[MAX_PATH];	
	
	MG_EncodeType  nEncodeType;	    //编码类型
	int  subEncodeType;			    //编码 操作 类型
	int  bit_rate;					// For MPEG-1 and MPEG-2:
									// Bit rate in bits/sec.  130000 < bit_rate <100000000

	MG_AudioWriteType enSrcAudioEncType;
	MG_AudioWriteType enDstAudioEncType; //音频编码类型
    
	DWORD  dwSamplesInA;                //输出采样率
	DWORD  dwSamplesOutA;			    //输出采样率
	DWORD  dwMp3RateA;                  //输出的MP3的码率
	int	   iAudioType;					//1:单声道；2：双声道
	int    iBitDepth;				    //音频采样位深	
    
	DWORD		dwGOPBCount;
	DWORD		dwGOPPCount;
	MGWriteFilePlugParam()
	{
		enSrcAudioEncType = MG_AUDIOWRITETYPE_WAV;
		enDstAudioEncType = MG_AUDIOWRITETYPE_WAV;
		iAudioType = 1;
	}
};



//文件参数
struct MGWriteFileParam
{	
    DWORD  dwCutClipFrame; //如果不需要分段 默认为0xfffffffe
	BOOL  bRetrospect; //
	DWORD  dwAudMaskA;//说明音频声道选择

	BOOL bPath0;  //线路0 是否写文件 （视频）
	BOOL bAudio0; //线路0 是否产生音频
	BOOL bAlone0; //线路0 是否产生独立音频 。 TRUE : 产生独立音频文件 ， FALSE : 音视频文件共用一个文件
    MG_AVWriteType AVWriteTypeV0; //V0写文件对象
	MGWriteFilePlugParam  plugParam0; //V0写文件插件参数 

	// ldw
	MG_AudioWriteType AudioWriteTypeA0; // A0写文件对象
	// ldw

	BOOL bPath1;  //线路1 是否写文件 （视频）
	BOOL bAudio1; //线路1 是否产生音频
	BOOL bAlone1; //线路1 是否产生独立音频 。 TRUE : 产生独立音频文件 ， FALSE : 音视频文件共用一个文件
    MG_AVWriteType AVWriteTypeV1; //V1写文件对象
	MGWriteFilePlugParam  plugParam1;//V1写文件插件参数
	
	// ldw
	MG_AudioWriteType AudioWriteTypeA1; // A1写文件对象
	// ldw

	MGWriteFileParam()
	{
		bPath0 ;  
		bAudio0 = TRUE; 
		bAlone0 = TRUE; 
		AVWriteTypeV0 = MG_WRITETYPE_UNKNOWN;

	// ldw
		AudioWriteTypeA0 = MG_AUDIOWRITETYPE_UNKNOWN;
	// ldw

		bPath1 = FALSE;
		bAudio1 = TRUE;
		bAlone1 = TRUE; 
		AVWriteTypeV1 = MG_WRITETYPE_UNKNOWN;

	// ldw
		AudioWriteTypeA1 = MG_AUDIOWRITETYPE_UNKNOWN;
	// ldw

	}
};

//采集控制参数,接口参数
struct MGCaptureCtrlParam
{ 
	int nTaskID;//任务ID

	MG_VideoStandard enumVideoST;//视频制式
	int  nFrameRateIndex;//帧率索引 // =1 for 23.97 f/sec  
					                // =2 for 24 f/sec
									// =3 for 25 f/sec     
									// =4 for 29.97
									// =5 for 30 f/sec
	int	nWidth;//视频宽度
	int	nHeight;//视频高度
	int	nWidthLow;//低码率宽度
	int	nHeightLow;//低码率高度 

	int nInOutCount;//任务中入点出点的对数。最多支持100对
	DWORD  dwInFrame[100];//开始帧 如果为0 ，立即开始. （该帧是需要采集的）
	DWORD  dwOutFrame[100];//任务总帧数 当任务采集的帧数到达此值 任务自动关闭，  如果不需要中层自动结束 设为0xfffffffe
	DWORD  dwTotalFrame[100];
	DWORD  dwCutClipFrame; //如果不需要分段 默认为0xfffffffe		
	BOOL  bRetrospect; //是否回朔采集 
	BOOL  bDetectKeyFrame;//是否启动转场帧识别

	int iGopBCount;
	int iGopPCount;

	BOOL bPath0;  //线路0 是否写文件 （视频）
	BOOL bAudio0; //线路0 是否产生音频
	BOOL bAlone0; //线路0 是否产生独立音频 。 TRUE : 产生独立音频文件 ， FALSE : 音视频文件共用一个文件
	TCHAR strFileNameV0[MAX_PATH];
	TCHAR strFileNameA0[MAX_PATH];
	MG_EncodeType  nEncodeType0; //编码线路 0 编码类型
	int  subEncodeType0;  //编码 操作 类型
	int  bit_rate0;     // For MPEG-1 and MPEG-2:
	// Bit rate in bits/sec.  130000 < bit_rate <100000000
	MG_AVWriteType AVWriteTypeV0; //V0写文件对象
	DWORD  dwAudMaskA0;//说明音频声道选择
	DWORD  dwSamplesOutA0;//输出采样率
	DWORD dwMp3RateA0; //输出的MP3的码率
	int   iAudBitDepth0;//音频采样位深
	MG_AudioWriteType AudioWriteTypeA0; //A0写文件对象 ,如果没有找到该对象，就直接使用wav	
	
	BOOL bPath1;  //线路1 是否写文件 （视频）
	BOOL bAudio1; //线路1 是否产生音频
	BOOL bAlone1; //线路1 是否产生独立音频 。 TRUE : 产生独立音频文件 ， FALSE : 音视频文件共用一个文件
	TCHAR strFileNameV1[MAX_PATH];
	TCHAR strFileNameA1[MAX_PATH];
	MG_EncodeType  nEncodeType1; //编码线路 1 编码类型
	int  subEncodeType1;  //编码 操作 类型
	int  bit_rate1;     // For MPEG-1 and MPEG-2:
	// Bit rate in bits/sec.  130000 < bit_rate <100000000
	MG_AVWriteType AVWriteTypeV1; //V1写文件对象
	DWORD  dwAudMaskA1;//说明音频声道选择
	DWORD  dwSamplesOutA1;//输出采样率
	DWORD dwMp3RateA1; //输出的MP3的码率
	int   iAudBitDepth1;//音频采样位深	
	MG_AudioWriteType AudioWriteTypeA1; //A1写文件对象 ,如果没有找到该对象，就直接使用wav

	//Add by wx 2009-11-23,采集参数添加节点，为了支持中层1440*1080采集的自适应识别
	DWORD	dwFirPicNum;	//刷新首帧缩略图的帧号
	DWORD	dwVideo_width;
	DWORD	dwVideo_height;
	WORD	 wCHROMA_FORMAT;	//_chroma_420 = 0,_chroma_422 = 1,_chroma_444 = 2,
   //Add over
	//高质量数据类型
	int  iHighVideoDataType;
	//低质量数据类型
	int  iLowVdeoDataType;

	//Add by zkw 2011-11-02
	
	int							AudioEncodeTypeA0;//A1编码格式
	int							AudioEncodeTypeA1;//A1编码格式
	
	

	TCHAR strStream_URL[255]; //流媒体url
	TCHAR strStream_Type[255];//流媒体类型

	DWORD dwRev[8];
	//End add
	MGCaptureCtrlParam()
	{
		iGopBCount = 2;
		iGopPCount = 4;

		bPath0 = TRUE; 
		bAudio0 = TRUE;
		bAlone0 = TRUE;
		strFileNameV0[0] = 0;
		strFileNameA0[0] = 0;
		nEncodeType0 = MG_ENCTYPE_UNKNOWN;
		subEncodeType0 = -1;  //编码 操作 类型
		bit_rate0 = -1;     // For MPEG-1 and MPEG-2:
		AVWriteTypeV0 = MG_WRITETYPE_UNKNOWN;
		dwSamplesOutA0 = 0;
		iAudBitDepth0 = 16;
		
		bPath1 = TRUE;
		bAudio1 = TRUE;
		bAlone1 = TRUE;
		strFileNameV1[0] = 0;
		strFileNameA1[0] = 0;
		nEncodeType1 = MG_ENCTYPE_UNKNOWN; //编码线路 1 编码类型
		subEncodeType1 = -1;  //编码 操作 类型
		bit_rate1 = -1;     // For MPEG-1 and MPEG-2:
		AVWriteTypeV1 = MG_WRITETYPE_UNKNOWN;
		dwSamplesOutA1 = 0;
		//zkw modify 2011-11-02
		iAudBitDepth1 = 16;//16;
		//End modify
		nInOutCount = 0;
		dwTotalFrame[0] = 0;
		dwFirPicNum = 0;
		dwVideo_width = 1920;
		dwVideo_height= 1080;
		wCHROMA_FORMAT = 1;
        iHighVideoDataType = -1;
		iLowVdeoDataType   = 0;

		
	}
};

//Add by wx for 4.5 Import
struct MGWriteFileParam_Plus
{	
    DWORD	dwCutClipFrame; //如果不需要分段 默认为0xfffffffe
	BOOL	bRetrospect; 
	DWORD	dwAudMaskA;//说明音频声道选择
	int		nUseImportMode;		//引入Import模式,0:非引入模式；1：用SDK写文件的引入模式；2：不拆包直接写MXF引入模式

	TCHAR strAudioName[8][MAX_PATH];		//for import use
	BOOL bPath0;  //线路0 是否写文件 （视频）
	BOOL bAudio0; //线路0 是否产生音频
	BOOL bAlone0; //线路0 是否产生独立音频 。 TRUE : 产生独立音频文件 ， FALSE : 音视频文件共用一个文件
    MG_AVWriteType AVWriteTypeV0; //V0写文件对象
	MGWriteFilePlugParam  plugParam0; //V0写文件插件参数 
		
	MG_AudioWriteType AudioWriteTypeA0; // A0写文件对象	
	
	BOOL bPath1;  //线路1 是否写文件 （视频）
	BOOL bAudio1; //线路1 是否产生音频
	BOOL bAlone1; //线路1 是否产生独立音频 。 TRUE : 产生独立音频文件 ， FALSE : 音视频文件共用一个文件
    MG_AVWriteType AVWriteTypeV1; //V1写文件对象
	MGWriteFilePlugParam  plugParam1;//V1写文件插件参数
		
	MG_AudioWriteType AudioWriteTypeA1; // A1写文件对象	

	//保留字段
	BYTE Reserve [128];
	DWORD dwReserve1;
	DWORD dwReserve2;
	
	MGWriteFileParam_Plus()
	{
		nUseImportMode = 0;
		bPath0 = TRUE;  
		bAudio0 = TRUE; 
		bAlone0 = TRUE; 
		AVWriteTypeV0 = MG_WRITETYPE_UNKNOWN;
		AudioWriteTypeA0 = MG_AUDIOWRITETYPE_UNKNOWN;				
		bPath1 = FALSE;
		bAudio1 = FALSE;
		bAlone1 = FALSE; 
		AVWriteTypeV1 = MG_WRITETYPE_UNKNOWN;				
		AudioWriteTypeA1 = MG_AUDIOWRITETYPE_UNKNOWN;				
	}
};

typedef struct _ClipFrameLog
{	
	DWORD			dwFrameBufferSize;	//此帧数据的总大小	
	DWORD			dwFrm_type;				//此帧视频数据帧类型
	DWORD			dwVSize;				//此帧数据里视频数据大小
	BOOL				bIsEnd;					//如果是最后一帧，置TRUE
	_ClipFrameLog()
	{
		dwFrameBufferSize = 0;
		dwFrm_type = MG_BUF_MPEG_UNKNOWN;
		dwVSize = 0;
		bIsEnd = FALSE;
	}
}ClipFrameLog;
//Add over

//在文件中，每一个ITEM项的阐述
struct  MGFileFrameItem  
{
	char   FrameHead[16];
	int nKeyFrame;  //<0:不是关键帧    >=0：是关键帧序号
    int nManualFrame; //<0:不是人工标记帧    >=0 ：人工标记帧序号
	int nManualFrameType;//人工标记帧类型

	BYTE*  pBufV0;     //数据指针---V0
	int  nBufSizeV0;  //V0的buffer个数
    BYTE*  pBufV1;     //数据指针---V1
	int  nBufSizeV1;  //V1的Buffer个数
	BYTE*  pBufA0;     //数据指针---A0
	int  nBufSizeA0;  //
	BYTE*  pBufA1;     //数据指针---A1
	int  nBufSizeA1;

	int  nFrameTypeV0;    //V0帧类型
	int  nFrameTypeV1;    //V1帧类型
	
	int nFileEndFlag ;    //文件结束标志
 
	MGFileFrameItem()
	{
		ZeroMemory(FrameHead,16);
		strcpy_s(FrameHead,_countof(FrameHead),"SobeyFrm");
		pBufV0 = NULL;     //数据指针---V0
		nBufSizeV0 = 0;  //V0的buffer个数
		pBufV1 = NULL;     //数据指针---V1
		nBufSizeV1 = 0;  //V1的Buffer个数
		pBufA0 = NULL;     //数据指针---A0
		nBufSizeA0 = 0;  //
		pBufA1 = NULL;     //数据指针---A1
		nBufSizeA1 = 0;

		nKeyFrame = -1;
		nManualFrame = -1;

		nFileEndFlag = 0;
	}
};

struct ASIStreamParam
{
	UINT	uProgrameNum;
	UINT	uPMTPid;
	UINT	uVideoPID;
	UINT	uAudioPID;
	int		nWidth;
	int		nHeight;
	int	    nChromaFormat;
	int		nProgressiveSequence;
	int		nVideoStandard;
	TCHAR	chServiceName[256];
	ASIStreamParam::ASIStreamParam()
	{
		uProgrameNum = 0;
		uVideoPID = 0;
		uAudioPID = 0;
		nWidth = 0;
		nHeight = 0;
	    nChromaFormat = 0;
		nProgressiveSequence = 0;
		nVideoStandard = 0;
		uPMTPid = 0;
		memset(chServiceName,0,sizeof(TCHAR)*256);
	}
};

typedef CArray<ASIStreamParam,ASIStreamParam> CStreamParamArray;

typedef enum VideoDataFormat
{
	VID_RGB = 0,
	VID_YUV,
	VID_YUYV,
	VID_UYVY
};
typedef struct Uploadinfo
{
  TCHAR strTaskName[100]; 	//上载任务名称
  int nTaskID;	      	//上载任务ID	
  int nTrimIn;	      	//任务入点	
  int nTrimOut;	      	//任务出点	
  int state;
  MGCaptureCtrlParam captureParam; 	//采集参数
}UploadinfoMG;

enum eTaskStatus
{
	StreamIn_TaskBegin,
    StreamIn_StopTask, 
    StreamIn_TaskEnd,
	StreamIn_TaskBlackCutClip,

	Encode_ThreadBegin,
	Encode_TaskBegin,
    Encode_TaskEnd,
	Encode_TaskBlackCutClip,
	Encode_ThreadEnd,

	Write_ThreadBegin,
	Write_TaskBegin,
    Write_TaskEnd,
	Write_TaskBlackCutClip,
	Write_ThreadEnd,

	Audit_Begin,
};
struct Format_Black
{
	DWORD dwFormat;		//SDI信号制式
	BOOL bIsBlack;		//是否是黑场
};
//MGStreamIn回调基类
class IMGStreamInCallBack
{
public:
	virtual void StreamIn_ERROR_CB(int nTaskID,int nErrorCode,int nErrorGrade,CString strErrorString)=0;
    virtual void StreamIn_TaskStatus_CB(int nTaskID,eTaskStatus eStatus,CString strGUID = _T(""))=0;
    virtual void StreamIn_Preview_CB(int nWidth,int nHeight,BYTE* pDataBuffer)=0;
};

//X1StreamIn回调基类
class IStreamInCallBack
{
public:
	virtual void Encode_ERROR_CB(int nTaskID,int nErrorCode,int nErrorGrade,CString strErrorString)=0;
    virtual void Encode_TaskStatus_CB(int nTaskID,eTaskStatus eStatus)=0;
};

//Encode回调类
class IMGEncodeCallBack
{
public:
    virtual void Encode_ERROR_CB(int nTaskID,int nErrorCode,int nErrorGrade,CString strErrorString)=0;
    virtual void Encode_TaskStatus_CB(int nTaskID,eTaskStatus eStatus)=0;
};
//写文件回调
class IMGWriteCallBack
{
public:
    virtual void Write_ERROR_CB(int nTaskID,int nErrorCode,int nErrorGrade,CString strErrorString)=0;
    virtual void Write_TaskStatus_CB(int nTaskID,eTaskStatus eStatus,int iTaskClipSum=0)=0;	
};

class IMGCaptureCtrlCallBack
{
public:
    virtual void CaptureCtrl_ERROR_CB(int nTaskID,int nErrorCode,int nErrorGrade,CString strErrorString)=0;
    virtual void CaptureCtrl_TaskStatus_CB(int nTaskID,eTaskStatus eStatus,int iTaskClipSum=0,CString strGUID = _T(""))=0;
	virtual void CaptureCtrl_Preview_CB(int nWidth,int nHeight,BYTE* pDataBuffer)=0;
};

typedef struct _stBUFFERCELL
{	
	DWORD	dwBufferPos;
	DWORD	dwUnitNum;

	_stBUFFERCELL()
	{				
		dwBufferPos = 0;
		dwUnitNum	= 0;		
	}
}BUFFERCELL;

struct ListItemInfoBase
{
	LPBYTE	pBufV0;
	LPBYTE	pBufA0;
	LPBYTE	pBufV1;
	LPBYTE	pBufA1;

	DWORD  nBufSizeUsedV0; //V0 buffer占用Byte数
	DWORD  nBufSizeUsedA0; //A0 buffer占用Byte数
	DWORD  nBufSizeUsedV1; //V1 buffer占用Byte数	
	DWORD  nBufSizeUsedA1; //A1 buffer占用Byte数

	BUFFERCELL stBufInfo[4];//V0/A0/V1/A1	
};
#endif