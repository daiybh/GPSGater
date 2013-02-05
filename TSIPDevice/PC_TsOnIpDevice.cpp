#include "stdafx.h"
#include "PC_TsOnIpDevice.h"
#include "CardDataBaseInfo.h"
#include "CaptureTsOnRtp.h"
#include "CaptureTsOnUdp.h"
#include "ESBufferMan.h"
#include "MGBufferListEx.h"

static LPCTSTR const INI_FILE_NAME			= _T("MgCaptureCfg.ini");
static LPCTSTR const INI_DEVICE				= _T("TS_Config");
static LPCTSTR const INI_CHANNEL_COUNT		= _T("TsChannelCount");
static LPCTSTR const INI_CHANNEL			= _T("TsOnIpChannel");
static LPCTSTR const INI_CHANNEL_BIND_IP	= _T("BindIp");
static LPCTSTR const INI_CHANNEL_BIND_PORT	= _T("BindPort");
static LPCTSTR const INI_CHANNEL_MULTICAST_IP	= _T("MulticastIp");
static LPCTSTR const INI_CHANNEL_PROTOCOL	= _T("Protocol");
static LPCTSTR const INI_CHANNEL_SSRC		= _T("SSRC");

/************************************************************************/
/*  //获取插件对象 ,在使用完对象之后，必须相应的必须调用ReleasePluginObject函数，释放对象                                                                     */
/************************************************************************/
void TSDevice_GetPlugInObject(ITsDevice_Card** ppIStreamIn)
{
	CPC_TsOnIpDevice* pMGSB1000HDVStreameIn = new CPC_TsOnIpDevice();
	*ppIStreamIn = (ITsDevice_Card*)pMGSB1000HDVStreameIn;	
}

/************************************************************************/
/* 释放插件对象                                                                     */
/************************************************************************/
void TSDevice_ReleasePlugInObject(ITsDevice_Card* pIStreamIn)
{
	if (pIStreamIn != NULL)
	{
		delete (CPC_TsOnIpDevice*)pIStreamIn;
		pIStreamIn = NULL;
	}	
}
/************************************************************************/
/* 获取插件ID                                                                     */
/************************************************************************/
DeviceTYPE TSDevice_PluginID()
{
	return IP_DEVICE;
}

CPC_TsOnIpDevice::CPC_TsOnIpDevice()
{
	m_bInit = FALSE;
	m_RtpChannel = NULL;
	m_UdpChannel = NULL;
	m_pDataList = NULL;
	m_bUseConfig = TRUE;
	m_dwPostData = 0;
	WriteLog(LOGNAME,logLevelInfo,_T("CPC_TsOnIpDevice"));
}

CPC_TsOnIpDevice::~CPC_TsOnIpDevice()
{
	ReleaseDevice();
}

void CPC_TsOnIpDevice::SaveConfig()
{
	CString sIniFilePath;
	CString sValue;	
	CString sAppName;

	::GetModuleFileName(NULL, sIniFilePath.GetBuffer(MAX_PATH), MAX_PATH);
	::PathRemoveFileSpec( sIniFilePath.GetBuffer(MAX_PATH));
	sIniFilePath.ReleaseBuffer();

	sIniFilePath += _T("\\");
	sIniFilePath += INI_FILE_NAME;

	if (!PathFileExists(sIniFilePath))
	{
	//	printf("Do not finded the %s file.\n", t2a(sIniFilePath.GetString()).c_str());	
		CString strLog;
		strLog.Format(_T("SaveConfig failed.Do not finded the %s file. bindIP[%s] port[%d] m_sMulticastIp[%s]\n"), 
			sIniFilePath,m_TsOnIpChannelInfo.m_sBindIp,m_TsOnIpChannelInfo.m_nBindPort,m_TsOnIpChannelInfo.m_sMulticastIp);
		WriteLog(LOGNAME,logLevelInfo,strLog);
	}

	sIniFilePath =GetMgConfigFileName();
	TsOnIpChannelInfo *info=&m_TsOnIpChannelInfo;

	//sAppName.Format(_T("%s_%d"), INI_CHANNEL, m_nBoardIdx);
    sAppName = _T("TS_Config");
	::WritePrivateProfileString(sAppName, INI_CHANNEL_BIND_IP,info->m_sBindIp,sIniFilePath);

	CString strValue ;
	strValue.Format(_T("%d"),info->m_nBindPort);

	if (info->m_sMulticastIp.CompareNoCase(_T("0.0.0.0")) == 0)
	{
		info->m_sMulticastIp.Format(_T(""));
	}

	::WritePrivateProfileString(sAppName, INI_CHANNEL_BIND_PORT,strValue,sIniFilePath);
	::WritePrivateProfileString(sAppName, INI_CHANNEL_MULTICAST_IP,info->m_sMulticastIp,sIniFilePath);
	::WritePrivateProfileString(sAppName, INI_CHANNEL_PROTOCOL,info->m_sProtol,sIniFilePath);
}

BOOL CPC_TsOnIpDevice::InitDevice(int nListenPort)
{
	if (m_bInit)
		return TRUE;
	ReleaseDevice();	
	if(!m_bUseConfig)
	{
		m_bInit = TRUE;
		return TRUE;
	}
	CString sIniFilePath=GetMgConfigFileName();   
	CString sValue;
	CString sAppName;
	/*

	::GetModuleFileName(NULL, sIniFilePath.GetBuffer(MAX_PATH), MAX_PATH);
	::PathRemoveFileSpec( sIniFilePath.GetBuffer(MAX_PATH));
	sIniFilePath.ReleaseBuffer();

	sIniFilePath += _T("\\");
	sIniFilePath += INI_FILE_NAME;

	if (!PathFileExists(sIniFilePath))
	{	
		WriteLog(LOGNAME,logLevelError, _T("Don't find the %s.CPC_TsOnIpDevice::InitDevice()"),sIniFilePath);
		return FALSE;
	}
	  /**/

	m_nBoardIdx = 0;
	TsOnIpChannelInfo *info=&m_TsOnIpChannelInfo;	
	::GetPrivateProfileString( INI_DEVICE, INI_CHANNEL_BIND_IP, _T(""),
		sValue.GetBuffer(MAX_PATH), MAX_PATH, sIniFilePath);
	sValue.ReleaseBuffer();
	info->m_sBindIp = sValue.GetString();

	::GetPrivateProfileString( INI_DEVICE, INI_CHANNEL_BIND_PORT, _T(""),
		sValue.GetBuffer(MAX_PATH), MAX_PATH, sIniFilePath);
	sValue.ReleaseBuffer();
	info->m_nBindPort = _tstol(sValue);

	::GetPrivateProfileString( INI_DEVICE, INI_CHANNEL_MULTICAST_IP, _T(""),
		sValue.GetBuffer(MAX_PATH), MAX_PATH, sIniFilePath);
	sValue.ReleaseBuffer();
	info->m_sMulticastIp = sValue.GetString();

	::GetPrivateProfileString( INI_DEVICE, INI_CHANNEL_PROTOCOL, _T(""),
		sValue.GetBuffer(MAX_PATH), MAX_PATH, sIniFilePath);
	sValue.ReleaseBuffer();
	info->m_sProtol = sValue.GetString();

	if (info->m_sProtol == _T("RTP"))
	{
		::GetPrivateProfileString( INI_DEVICE, INI_CHANNEL_SSRC, _T(""),
			sValue.GetBuffer(MAX_PATH), MAX_PATH, sIniFilePath);
		sValue.ReleaseBuffer();
		if (sValue == _T("NULL"))
		{
			info->m_bSSRC = FALSE;
			info->m_nSSRC = 0;	
		}
		else
		{
			info->m_bSSRC = TRUE;
			info->m_nSSRC = _tstol(sValue);
		}
	}

	m_bInit = TRUE;
	return CreateInputChannel();
	return TRUE;
}

BOOL CPC_TsOnIpDevice::ReleaseDevice()
{
	ReleaseAllInputChannel();
	if(m_RtpChannel)
	{
		delete m_RtpChannel;
		m_RtpChannel = NULL;
	}

	if(m_UdpChannel)
	{
		delete m_UdpChannel;
		m_UdpChannel=NULL;
	}

	m_bInit = FALSE;
	return TRUE;
}

int CPC_TsOnIpDevice::CreateInputChannel(  )
{
	if(!m_bInit )
		return 0;
	_stprintf_s(m_strLogName,_countof(m_strLogName),_T("PC_TSOnIpDevice_[%s]_[%d]"),
		m_TsOnIpChannelInfo.m_sBindIp,
		m_TsOnIpChannelInfo.m_nBindPort);
	CString strLog;
	strLog.Format(_T("CPC_TsOnIpDevice::CreateInputChannel ip=%s,port=%d multicastIp=[%s]  protol=[%s]"),
			m_TsOnIpChannelInfo.m_sBindIp,m_TsOnIpChannelInfo.m_nBindPort,m_TsOnIpChannelInfo.m_sMulticastIp,m_TsOnIpChannelInfo.m_sProtol);
	WriteLog(LOGNAME,logLevelInfo,strLog);

	if (m_TsOnIpChannelInfo.m_sProtol == _T("RTP"))
	{
		CaptureTsOnRtp_Param param;
		_snprintf_s(param.m_BindIp, MAX_PATH,_countof(param.m_BindIp), "%s", CStringA(m_TsOnIpChannelInfo.m_sBindIp));
		param.m_BindPort = m_TsOnIpChannelInfo.m_nBindPort;
		_snprintf_s(param.m_MulticastIp, MAX_PATH,_countof(param.m_BindIp),"%s", CStringA(m_TsOnIpChannelInfo.m_sMulticastIp));

		param.m_bSSRC = m_TsOnIpChannelInfo.m_bSSRC;
		param.m_nSSRC = m_TsOnIpChannelInfo.m_nSSRC;

		CaptureTsOnRtp* pRtp = new CaptureTsOnRtp();
		if ( !pRtp->Init(param) )
		{
			CString strLog;
			strLog.Format(_T("Err:pRtp->Init() failed.ip=%s,port=%d"),param.m_BindIp,param.m_BindPort);
			WriteLog(LOGNAME,logLevelInfo,strLog);
			delete pRtp;
			return 0;
		}
		m_RtpChannel = pRtp;			
	}
	else
	{
		CaptureTsOnUdp_Param param;
		_snprintf_s(param.m_BindIp, MAX_PATH,_countof(param.m_BindIp), "%s", CStringA(m_TsOnIpChannelInfo.m_sBindIp));
		param.m_BindPort = m_TsOnIpChannelInfo.m_nBindPort;
		_snprintf_s(param.m_MulticastIp, MAX_PATH,_countof(param.m_BindIp), "%s", CStringA(m_TsOnIpChannelInfo.m_sMulticastIp));

		CCaptureTsOnUdp* pUdp = new CCaptureTsOnUdp();
		if ( !pUdp->Init(param,m_strLogName) )
		{
			WriteLog(LOGNAME,logLevelInfo,_T("Err:pUdp->Init() failed."));
			delete pUdp;
			return 0;
		}
		m_UdpChannel = pUdp;		
	}
	return TRUE;
}

int CPC_TsOnIpDevice::ReleaseInputChannel()
{
	if(!m_bInit)
		return 0;

	if(m_RtpChannel)
	{
		delete m_RtpChannel;
		m_RtpChannel =NULL;
	}
	if(m_UdpChannel)
	{
		delete m_UdpChannel;
		m_UdpChannel =NULL;
	}
	return 1;
}

int CPC_TsOnIpDevice::ReleaseAllInputChannel()
{
	if(m_RtpChannel)
	{
		delete m_RtpChannel;
		m_RtpChannel =NULL;
	}
	if(m_UdpChannel)
	{
		delete m_UdpChannel;
		m_UdpChannel =NULL;
	}

	return 1;
}

void CPC_TsOnIpDevice::SetMemShareBuffer(MGBufferListEx *pBufferList,DWORD dwBufferSize)
{
	m_pDataList = pBufferList;

	//ZKW modify 2011-09-22
	/*
	if(m_RtpChannel)
		m_RtpChannel->SetGetDataCBFun(GetDataCb,this,dwBufferSize-sizeof(CardDataBaseInfo));
	if(m_UdpChannel)
		m_UdpChannel->SetGetDataCBFun(GetDataCb, this, dwBufferSize-sizeof(CardDataBaseInfo));	
	/**/
	if(m_RtpChannel)
		m_RtpChannel->SetGetDataCBFun(GetDataCb,this,dwBufferSize);
	if(m_UdpChannel)
		m_UdpChannel->SetGetDataCBFun(GetDataCb, this, dwBufferSize);	
	//End modify
}

int CPC_TsOnIpDevice::StartInput()
{
	if( !m_bInit)
		if(!InitDevice(-1))
			return 0;

	if(m_RtpChannel)
		return m_RtpChannel->StartCapture();
	if(m_UdpChannel)
		return m_UdpChannel->StartCapture();

	return 0;
}
int CPC_TsOnIpDevice::sendData( const char *pData,int nDataSize,const char *pAddr,int nPort )
{
	if( !m_bInit)
		return 0;
	if(m_UdpChannel)
		return m_UdpChannel->sendData(pData,nDataSize,pAddr,nPort);

	return 0;

}

int CPC_TsOnIpDevice::sendData( const GPSGATEDATA *pGpsGateData )
{
return 0;
}

int CPC_TsOnIpDevice::StopInput()
{
	if( !m_bInit)
		return 0;

	if(m_RtpChannel)
		return m_RtpChannel->StopCapture();
	if(m_UdpChannel)
		return m_UdpChannel->StopCapture();

	return 0;
}

void CPC_TsOnIpDevice::GetData( BYTE* pBuffer, UINT nDataSize, char *pFromAddr,int nPort)
{	
	LISTITEMINFOEX *pItem = NULL;
	static DWORD dwEmptyCnt = 0;
	pItem =	m_pDataList->FetchEmptyHeadItem(nDataSize,0,0,0);
	if (pItem == NULL)
	{
		CString sLog;

		sLog.Format(_T("Err:Lost frame! Can't fetch empty item in CPC_TsOnIpDevice::GetData()! nDataSize=%d"),
			nDataSize);
		WriteLog(LOGNAME,logLevelInfo,sLog);
		return;
	}

	dwEmptyCnt = 0;
	memcpy_s(pItem->pBufV0,pItem->nBufSizeV0,pBuffer,nDataSize);
	pItem->nBufSizeUsedV0 = nDataSize;
	pItem->dwFieldIndex = m_dwPostData++;
//	strcpy(pItem->socketAddr,pFromAddr);
//	pItem->socketPort	= nPort;
	CString strTemp;

	CString strLog;
	strLog.Format(_T("GPSDATA:%d"),pItem->dwFieldIndex);
	for(int i=0;i<pItem->nBufSizeUsedV0;i++)
	{
		if(pItem->pBufV0[i]==NULL)
			break;
		strTemp.Format(_T("%c"),pItem->pBufV0[i]);
		strLog+= strTemp;
	}
	OutputDebugString(strLog);
	WriteLog(LOGNAME,logLevelInfo,strLog);
	m_pDataList->AddItem2DataTail(pItem);
}

CString CPC_TsOnIpDevice::GetChannelName()
{
	CString str;
	if (m_TsOnIpChannelInfo.m_sProtol==_T("RTP")&&m_RtpChannel==NULL || 
		m_TsOnIpChannelInfo.m_sProtol==_T("UDP")&&m_UdpChannel==NULL)
	{
		str.Format(_T("Failed:IP(%s)_Port(%d) (TS)(%s)"),m_TsOnIpChannelInfo.m_sBindIp,
		m_TsOnIpChannelInfo.m_nBindPort,m_TsOnIpChannelInfo.m_sProtol);
	}
	else
	{
		str.Format(_T("IP(%d)_Port(%d)(%s)(%s)(%s)"),
			m_nBoardIdx,m_TsOnIpChannelInfo.m_nBindPort,m_TsOnIpChannelInfo.m_sBindIp,
			m_TsOnIpChannelInfo.m_sMulticastIp,m_TsOnIpChannelInfo.m_sProtol );
	}
	
	return str;	
}

BOOL CPC_TsOnIpDevice::GetChannelInfo(TsOnIpChannelInfo &ChannelInfo) const
{
	ChannelInfo = m_TsOnIpChannelInfo;
	return TRUE;
}

BOOL CPC_TsOnIpDevice::SetChannelInfo(const TsOnIpChannelInfo &ChannelInfo)
{
	m_TsOnIpChannelInfo = ChannelInfo;
	SaveConfig();
	m_bUseConfig = FALSE;
	return TRUE;
}


