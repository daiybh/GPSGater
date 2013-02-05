#ifndef _Sobey_Card_API_H_
#define _Sobey_Card_API_H_

#ifndef _XH_STATIC_EXPORTS_
#ifdef SobeyCardApi_EXPORTS
#define SobeyCardApi_EXPORT __declspec(dllexport)
#else
#define SobeyCardApi_EXPORT __declspec(dllimport)
#endif
#endif

#ifndef _XH_STATIC_EXPORTS_
class SobeyCardApi_EXPORT CSBInputChannel
#else
class CSBInputChannel
#endif
{
public:
	CSBInputChannel(int nBoardIdx,int nChannelIdx);
	~CSBInputChannel();

	channelStatus	GetChannelStatus();									//获得通道的状态
	SBStatus		SetChannelParam(inputChannelParam param);			//设置通道参数，打开通道
	SBStatus		GetInputSignalInfo(inportInformation &info);		//获得输入信号信息，设置通道参数口可用
	SBStatus		SwitchInputPort(videoPort vPort,audioPort aPort);	//切换输入端口
	SBStatus		Start();											//开始采集数据
	SBStatus		Pause();											//停止采集数据
	SBStatus		SetEEChannel(int nOutputChanNum,BOOL bEnable);		//设置环通通道
	SBStatus		AssignInterruptSource();
	SBStatus		GetInterruptInfo(interruptInfo &info);				//得到中断信息
	SBStatus		GetData(dataStruct &tData);							//得到采集数据
	SBStatus		GetErrReportInfo(warningInformation &info);			//得到错误信息
	void			Destroy();											//关闭通道
	CDialog			*GetDvbCodecDlg(HWND hWinParent = NULL,VOID *pPtr=NULL);
	CDialog			*GetControlDlg(HWND hWinParent = NULL,VOID *pPtr=NULL);	//备用接口
	SBStatus		_ExternFunc(DWORD nFunctionCode, int nParam=0, LPBYTE pSrc=NULL, LPBYTE pDst=NULL);
public:
	inputChannelParam m_param;
	interruptHandle	  m_handle;
	channelStatus	  m_nStatus;
	UINT			  m_nBoardIdx;
	int				  m_nChannelIdx;

	DWORD			  m_nCacheLen;
	dataStruct		  *m_pData[255];
};

#ifndef _XH_STATIC_EXPORTS_
class SobeyCardApi_EXPORT CSBOutputChannel
#else
class CSBOutputChannel
#endif
{
public:
	CSBOutputChannel(int nBoardIdx,int nChannelIdx);
	~CSBOutputChannel();

	channelStatus	GetChannelStatus();

	SBStatus	SetChannelParam(outputChannelParam param);
	SBStatus	InitMemory(const dataStruct *tData,int nFrames,BOOL bDisp=TRUE);
	SBStatus	InitMemory(BOOL bDisp=TRUE);
	SBStatus	Start(BOOL bSeek=FALSE);
	SBStatus	Pause(BOOL bPauseTo=FALSE,LONGLONG nFrame=0);
	SBStatus	SetOutSingal(outputSignalType type,
							int nVPortNum=0,int nAPortNum=0,
							int nOutChannelNum = 0,
							int nVTestNum=0,int nATestNum=0);
	SBStatus	ChangeSyncPort(syncPort	  eSyncPort);
	SBStatus	AssignInterruptSource();
	SBStatus	GetInterruptInfo(interruptInfo &info);
	SBStatus	SendData(const dataStruct &tData,BOOL bAck=TRUE);
	SBStatus	GetErrReportInfo(warningInformation &info);
	void		Destroy();
	CDialog		*GetDvbCodecDlg(HWND hWinParent = NULL,VOID *pPtr=NULL);
	CDialog		*GetControlDlg(HWND hWinParent = NULL,VOID *pPtr=NULL);

	SBStatus	_ExternFunc(DWORD nFunctionCode, int nParam=0,LPBYTE pSrc=NULL, LPBYTE pDst=NULL);
public:
	outputChannelParam	m_param;
	interruptHandle		m_handle;
	channelStatus		m_nStatus;
	int					m_nVideoPortNum;
	videoPort			m_videoPorts[10];
	int					m_nAudioPortNum;
	audioPort			m_audioPorts[10];
	UINT				m_nBoardIdx;
	int					m_nChannelIdx;

	DWORD				m_nCacheLen;
	dataStruct			*m_pData[255];
	DWORD				m_nInitLen;					
	//SetChannelParam后，返回给InitMem需要的内存尺寸。
};

#ifndef _XH_STATIC_EXPORTS_
class SobeyCardApi_EXPORT CSobeyCard
#else
class CSobeyCard
#endif
{
public:
	static int		GetBoardsCounts();
	static SBStatus GetBoardInfo(UINT nBoardIdx,boardInfo &info);

	CSobeyCard( );
	~CSobeyCard( );


	SBStatus	InitBoard(UINT  nBoardIdx);
	VOID		CloseBoard(UINT nBoardIdx);

	SBStatus	SetInterruptMode(interruptMode mode);

	SBStatus	ReadRegister(UINT nBoardIdx, DWORD nAddress,DWORD nReg, DWORD &nData);
	SBStatus	WriteRegister(UINT nBoardIdx,DWORD nAddress,DWORD nReg, DWORD  nData);
	SBStatus	GetBoardVersion(UINT nBoardIdx,versionInfomation &tVerInfo);
	SBStatus	GetSerialNum(UINT nBoardIdx,LONGLONG &nSerNum,DWORD &nDate);
	char*		GetErrorCode(DWORD nCode);

	SBStatus	LoadFactoryParam(UINT nBoardIdx,DWORD nCommand);
	SBStatus	AdjParameter( UINT nBoardIdx,DWORD nCommand,DWORD nValue);
	SBStatus	GetParameter( UINT nBoardIdx,DWORD nCommand,DWORD &nValue);
	SBStatus	LoadUserParam(UINT nBoardIdx,DWORD nCommand,DWORD nValue);
	SBStatus	SaveUserParam(UINT nBoardIdx,DWORD nCommand,DWORD nValue);
	CDialog		*GetControlDlg(HWND hWinParent = NULL,VOID *pPtr=NULL);

	//备用接口  nFunctionCode nParam 见 SobeyCardParam.h
	SBStatus			_ExternFunc(UINT nBoardIdx, DWORD nFunctionCode, int nParam=0, LPBYTE pSrc=NULL, LPBYTE pDst=NULL);
public:
	static int			    m_nBoardNum;
	static boardInfo 	    m_boardInfo[10];			//BoardIdx
	static int				m_nInputChannelNum[10];		//BoardIdx
	CSBInputChannel*		m_pInputChannel[10][10];	//BoardIdx,ChannelIdx
	static int				m_nOutPutChannelNum[10];	//BoardIdx
	CSBOutputChannel*		m_pOutputChannel[10][10];	//BoardIdx,ChannelIdx
};

#endif
