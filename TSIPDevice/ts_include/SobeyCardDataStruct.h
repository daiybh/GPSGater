#ifndef _SOBEY_CARD_DATA_STRUCT_H_
#define _SOBEY_CARD_DATA_STRUCT_H_

#define AUDIO_MAX_CHANNEL		16
#define VIDEO_MAX_VBI_CONUT		10

typedef enum _tagVideoPortType		//视频端口物理类型枚举
{
	videoPortNone = 0,
	videoComposite,
	videoYC,
	videoComponent,
	videoSDI,
	videoType0,
	videoType1,
	videoType2,
	videoType3,
}videoPortType;

typedef enum _tagAudioPortType		//音频端口物理类型枚举
{
	audioPortNone = 0,
	audioAnalog,
	audioAesEbu,
	audioSdi,
	audioType0,
	audioType1,
	audioType2,
	audioType3,
}audioPortType;

typedef enum _tagVideoSyncMode		//同步模式枚举
{
	syncRef,
	syncInput,
	SyncFree,
	syncType0,
	syncType1,
	syncType2,
	syncType3,
}videoSyncMode;

typedef enum _tagVideoDataFormat	//视频数据格式枚举
{
	videoYUV	= 0,
	videoUYVY,
	videoYUYV,
	videoFormat0,
	videoFormat1,
	videoFormat2,
	videoFormat3,
}videoDataFormat;

typedef enum _tagVideoStandard		//电视视频格式枚举
{
	VideoUnknow = 0,
	SD_PAL,
	SD_Ntsc30,
	SD_Ntsc29_97,
	SD_Secam,
	HD_1920_1080_48I,
	HD_1920_1080_47_95I,
	HD_1920_1080_50I,
	HD_1920_1080_60I,
	HD_1920_1080_59_94I,

	HD_1920_1080_24P,    //=10
	HD_1920_1080_23_97P,
	HD_1920_1080_25P,
	HD_1920_1080_30P,
	HD_1920_1080_29_97P,
	HD_1920_1080_50P,
	HD_1920_1080_60P,
	HD_1920_1080_59_94P,
	HD_1080_720_24P,
	HD_1080_720_23_97P, //19

	HD_1080_720_25P,	//20
	HD_1080_720_30P,
	HD_1080_720_29_97P,
	HD_1080_720_50P,
	HD_1080_720_60P,
	HD_1080_720_59_94P,
	videoStandard0,
	videoStandard1,
	videoStandard2,
	videoStandard3,
}videoStandard;

typedef enum _tagChannelStatus		//通道状态枚举
{
	ChannelNoInit = 0,
	ChannelInited,
	ChannelCapturing,
	ChannelCapturePause,
	ChannelPrepareTransfer,
	ChannelTransfering,
	ChannelTransferSeeking,
	ChannelTransferPause,
	ChannelStatus0,
	ChannelStatus1,
	ChannelStatus2,
	ChannelStatus3,
}channelStatus;

typedef enum _tagAudioSampleBits	//音频采样量化Bit枚举
{
	audio32Bits,
	audio24Bits,
	audio20Bits,
	audio16Bits
}audioSampleBits;

typedef enum _tagAudioSampleFrequency	//音频采样率枚举
{
	audio48K = 0,
	audio44K1,
	audio32K,
	audioSample0,
	audioSample1,
	audioSample2, 
	audioSample3
}audioSampleFrequency;

typedef enum _tagChannelMode			//通道工作模式枚举
{
	vidAudMode= 0,
	dvbMode,
	workMode0,
	workMode1,
	workMode2,
	workMode3,
}channelMode;

typedef struct _tagVersionInfomation	//版本控制结构描述
{
	_tagVersionInfomation()
	{
		description = "No description";
	};
	CString str;
	BYTE	nYear;
	BYTE	nMonth;
	BYTE	nDay;
	LPSTR	description;
	void*	pReserved;
}versionInfomation,*pVersionInfomation;

typedef struct _tagVideoPort			//视频端口结构描述
{
	_tagVideoPort()
	{
		eType	 = videoPortNone;
		nPortNum = 0;
		nUsed	 = 0;
		int i=0;
		for(i=0; i<6; i++)
			bSuportMode[i] = FALSE;
		for(i=0; i<30; i++)
			bSupportStandart[i] = FALSE;
		for(i=0; i<10; i++)
			bChannel[i] = FALSE;
		eCurMode	= vidAudMode;
		eFormat		= videoYUV;

		nSrc		= 0xFF;
		BindAudioPort = 0xFF;
		nGroupNumber = 0;
		description = "No description";
	}
	videoPortType	eType;				//视频端口物理类型
	BOOL			bSuportMode[6];		//对于channelMode工作模式的支持
	BOOL			bSupportStandart[30];	//端口对于视频制式的支持
	int				nPortNum;			//端口序号

	BOOL			bChannel[10];		//当前正在使用该端口的通道
	int				nUsed;				//当前是否已被使用
	channelMode		eCurMode;			//当前工作模式
	videoDataFormat eFormat;			//视频数据格式

	int				nSrc;
	int				BindAudioPort;
	int				nGroupNumber;

	LPSTR			description;	//对于此端口的字符描述
	void*			pReserved;			//备用
}videoPort;

typedef struct _tagAudioPort			//音频端口结构描述
{
	_tagAudioPort()
	{
		eType			= audioPortNone;
		nPortNum		= 0;
		nMaxChannelCount= 0;
		bUsed			= FALSE;
		nSrc			= 0xFF;
		BindVideoPort	= 0xFF;
		nGroupNumber	= 0;
		description = "No description";
	}
	audioPortType	eType;				//音频端口物理类型
	int				nPortNum;			//端口序号
	int				nMaxChannelCount;	//端口支持的最大音频声道数
	BOOL			bUsed;				//当前是否已被使用
	LPSTR			description;		//对于此端口的字符描述

	int				nSrc;
	int				BindVideoPort;
	int				nGroupNumber;

	void*			pReserved;			//备用
}audioPort;

typedef struct _tagSyncPort				//同步端口描述
{
	_tagSyncPort()
	{
		eMode	= syncRef;
		nPortNum= 0;
		bUsed = FALSE;
		description = "No description";
	}
	videoSyncMode	eMode;				//同步模式
	int				nPortNum;			//端口序号
	BOOL			bUsed;				//当前是否已被使用
	LPSTR			description;		//对于此端口的字符描述
	void*			pReserved;			//备用
}syncPort;

typedef enum  _tagOutputSignalType
{
	signalPlay=0,			//本通道的回放信号
	signalInport,			//来源输入端口的环通信号
	signalOtherOutChannel,	//其他输出口的回放信号
	signalTest,				//测试信号
}outputSignalType;

typedef enum	_tagInterrutMode
{
	intFeild = 0,
	intFrame,
	interuptMode0,
	interuptMode1,
	interuptMode2,
	interuptMode3,
}interruptMode;

typedef struct _tagBoardCapability		//板卡处理能力描述
{
	_tagBoardCapability()
	{
		bDvbCapblity		= FALSE;
		vidCapPortNum		= 0;
		audCapPortNum		= 0;
		vidTransPortNum		= 0;
		audTransPortNum		= 0;
		syncPortNum			= 0;
		for(int i=0; i<30; i++)
			videoStandart[i] = FALSE;
		nMaxInChannelCount	= 0;
		nMaxOutChannelCount	= 0;
		bInOutSimul			= FALSE;
		description			= "No description";
	}
	BOOL		 bDvbCapblity;		 //是否支持DVB
	int			 nMaxInChannelCount; //最大支持输入通道数目
	int			 nMaxOutChannelCount;//最大支持输出通道数目
	BOOL		 bInOutSimul;		 //输入输出同时进行

	BOOL		 videoStandart[30];	//支持的视频信号格式
	int			 vidCapPortNum;		//视频输入端口数目
	int			 audCapPortNum;		//音频输入端口数目
	videoPort	 vidCapPort[100];	//视频输入端口
	audioPort	 audCapPort[100];	//音频输入端口

	int			 vidTransPortNum;	//视频输出端口数目
	int			 audTransPortNum;	//音频输出端口数目
	videoPort	 vidTransPort[100];	//视频输出端口
	audioPort	 audTransPort[100];	//音频输出端口

	int			 syncPortNum;		//同步端口数目
	syncPort	 syncPort[100];		//同步输出端口

	DWORD		  Capblity0[100];	//备用
	DWORD		  Capblity1[100];	//备用
	LPSTR		  description;		//处理能力字符描述
	void*		  pReserved;		//备用
}boardCapability;

typedef struct _tagVBIInfo
{
	_tagVBIInfo()
	{
		nType = 0;
		nDID  = 0;
		nSDID = 0;
	}
	DWORD	nType;
	DWORD	nDID;
	DWORD	nSDID;
}vbiInfo;

typedef struct _tagVBIDataStruct
{
	_tagVBIDataStruct()
	{
		bActive = FALSE;
		pData	= NULL;
		nLen	= 0;
	}
	vbiInfo		info;
	LPBYTE		pData;
	DWORD		nLen;
	BOOL		bActive;
}vbiData;

typedef struct _tagInportInformation			//输入通道信息描述
{
	_tagInportInformation()
	{
		description = "No description";
	};
	videoStandard			eStd;				//输入视频格式
	audioSampleBits			audBits;			//音频数据量化深度
	audioSampleFrequency	eSample;			//音频采样率
	LPSTR					description;		//输入信息字符描述
	void*					pReserved;			//备用
}inportInformation;

typedef struct _tagInputChannelParam			//输入通道参数
{
	_tagInputChannelParam()
	{
		eStd		= SD_PAL;
		nVbiNum		= 0;
		bEnableD10	= FALSE;
		description = "No description";
	}
	channelMode	  eMode;						//通道工作模式
	videoPort	  eVideoPort;					//输入视频通道
	audioPort	  eAudioPort;					//输入音频通道
	videoStandard eStd;							//(未用)
	videoDataFormat eFormat;					//视频数据格式
	audioSampleBits audBits;					//音频量化深度
	BOOL		  bEnableD10;					//D10使能（对于SD信号，有效）
	DWORD		  nCacheLen;					//板卡Chache深度
	int			  nVbiNum;
	vbiInfo		  vbiParam[10];
	LPSTR		  description;					//输入通道参数字符描述
	void*		  pReserved;					//备用
}inputChannelParam;

typedef struct _tagInterruptInfo					//场中断信息
{
	_tagInterruptInfo()
	{
		description = "No description";
	};
	LONGLONG	 nCounter;						//场中断计数值
	DWORD		 nCacheLen;						//当前板卡可用Cache深度
	LPSTR		 description;					//场中断字符描述
	DWORD		 nInfo0;
	DWORD		 nInfo1;
	DWORD		 nInfo2;
	DWORD		 nInfo3;
	void*		 pReserved;						//备用
}interruptInfo;

typedef struct _tagOutputChannelParam			//输出通道参数
{
	_tagOutputChannelParam()
	{
		eMode			= vidAudMode;
		nVbiNum			= 0;
		nDvbRate		= 0.0;
		bEnableD10		= FALSE;
		description		= "No description";
		nCacheLen		= 5;
	}
	channelMode				eMode;				//工作模式
	syncPort				eSyncPort;			//同步端口
	double					nDvbRate;
	videoStandard			eStd[100];			//各端口输出信号格式
	videoStandard			eVidStd;			//输出信号视频格式
	BOOL					bEnableD10;			//D10使能（对于SD信号，有效）
	videoDataFormat			eFormat;			//输出视频数据格式
	audioSampleBits			audBits;			//音频量化深度
	audioSampleFrequency	eAudSample;			//音频采样率
	DWORD					nCacheLen;			//SDK循环Buffer深度
	int						nVbiNum;
	vbiInfo					vbiParam[10];
	LPSTR					description;		//输出通道参数字符描述
	void*					pReserved;			//扩展使用请见最新SobeyCardParam.h文件中的描述
}outputChannelParam;
	
typedef struct _tagWarningInformation			//报警信息描述
{
	_tagWarningInformation()
	{
		bFanWarning		= FALSE;
		bSyncWarning	= FALSE;
		bInportWarning	= FALSE;
		bOutportWarning = FALSE;
		for(int i=0; i<10; i++)
			bOtherWarning[i] = FALSE;
	}
	BOOL	bFanWarning;					//风扇报警
	BOOL	bSyncWarning;					//同步报警
	BOOL	bInportWarning;					//输入信号报警
	BOOL	bOutportWarning;				//输出报警
	BOOL	bOtherWarning[10];				//备用报警信号
	DWORD	nWarningData[15];				//备用
	void*	pReserved;						//备用
}warningInformation;

typedef struct _tagVideoDataStruct			//视频数据描述
{
	_tagVideoDataStruct()
	{
		nVbiNum		= 0;
		nPicWide	= 0;
		nPicHigh	= 0;
		pUyvy		= NULL;
		pY			= NULL;
		pU			= NULL;
		pV			= NULL;
		pReserved	= NULL;
	}
	LPBYTE	pUyvy;							//UYVY数据格式指针（在videoDataFormat为videoUYVY使用）
	LPBYTE	pY;								//YUV数据格式Y指针（在videoDataFormat为videoYUV使用）
	LPBYTE	pU;								//YUV数据格式U指针（在videoDataFormat为videoYUV使用）
	LPBYTE	pV;								//YUV数据格式V指针（在videoDataFormat为videoYUV使用）
	videoStandard std;						//该数据的制式信息
	DWORD	nPicWide;						//图像宽度
	DWORD	nPicHigh;						//图像高度
	int		nVbiNum;
	vbiData	VbiData[10];
	void*	pReserved;						//备用
}videoDataStruct,*pVideoDataStruct;

typedef struct _tagAudioDataStruct			//音频声道数据描述
{
	_tagAudioDataStruct()
	{
		pReserved		= NULL;
		pData			= NULL;
		nSampleCount	= 0;
		nAudioUvValue	= 0;
	}
	LPBYTE	pData;							//音频数据指针
	DWORD	nSampleCount;					//包含采样点的数据
	DWORD	nAudioUvValue;					//UV表值
	void*	pReserved;						//备用
}audioDataStruct,*pAudioDataStruct;

typedef struct _tagDataStruct				//通道数据结构
{
	_tagDataStruct()
	{
		bActive		= FALSE;
		pDvbSrc		= NULL;
		pReserved	= NULL;
		bInfo		= FALSE;
	}
	LPBYTE				pDvbSrc;			//DVB数据指针（在通道工作模式为dvb模式下有效）
	DWORD				nDvbBytes;			//DVB数据长度（在通道工作模式为dvb模式下有效）

	videoDataStruct		tVidSrc;			//视频数据描述（在通道工作模式为AV模式下有效）
	audioDataStruct		tAudSrc[AUDIO_MAX_CHANNEL];//音频数据描述（在通道工作模式为AV模式下有效）

	LONGLONG			nCounter;			//数据计数值
	BOOL				bActive;			//有效标记
	BOOL				bInfo;
	DWORD				nInfoValue;
	void*				pReserved;			//备用
}dataStruct;

typedef struct _tagInterruptHandle			//中断描述
{
	_tagInterruptHandle()
	{
		hErrReportInterrupt = NULL;
		hBufferInterrupt	= NULL;
		hInterrupt			= NULL;
		for(int i=0; i<10; i++)
			hReserved[i]	= NULL;
	}
	HANDLE				hErrReportInterrupt;	//警告报告中断
	HANDLE				hBufferInterrupt;		//采集Bufer满/发送Bufer空 中断
	HANDLE				hInterrupt;				//采集/发送 场中断
	HANDLE				hReserved[10];			//备用
}interruptHandle;

typedef struct _tagBoardInfo				//板卡描述
{
	_tagBoardInfo()
	{
		description = "No description";
		nBoardNum	= 0;
		nType		= 0;
		pMemPtr		= NULL;
		nMemSize	= 0;
		pReserved	= NULL;
	}
	UINT			nBoardNum;				//板卡序号
	DWORD			nType;					//板卡类型号（见SobeyCardBoardType.h）
	LPSTR			description;			//板卡字符描述
	boardCapability	capability;				//板卡处理能力描述

	DWORD	 nMemSize;						//板卡所需内存尺寸
	LPBYTE	 pMemPtr;						//板卡内存指针
	DWORD	 nMemAddr;						//板卡内存物理地址
	void*	 pReserved;						//备用
}boardInfo;

#endif
