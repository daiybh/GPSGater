// CaptureTsOnUdp.cpp: implementation of the CCaptureTsOnUdp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CaptureTsOnUdp.h"

const int PACKET_SIZE = 188 * 7;						// 1316 Byte
const int PACKET_SIZE_MAX = 65535;						// 64K	Byte WinCE为10KB
const int BUFFER_SIZE = 1310720;						// 1024 * 1024 * 1.25 Byte

const int SOCKET_BUFFER_SIZE = 1310720;					// 1024 * 1024 * 1.25 Byte;
const int SOCKET_TIMEOUT = 10;							// 0.01s

const int RTP_HEADER_SIZE = 12;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCaptureTsOnUdp::CCaptureTsOnUdp()
{
	m_pGetDataCBFun = NULL;
	m_pUserData = NULL;
	m_hDataOkEvent	= NULL;
	m_DataOkSize	= 0;

	m_hCaptureThread		= NULL;
	m_hStopCaptureThread	= NULL;

	m_pUdp = NULL;
}

CCaptureTsOnUdp::~CCaptureTsOnUdp()
{
	StopCapture();

	if (m_pUdp)
		delete m_pUdp;	
}

BOOL CCaptureTsOnUdp::Init( const CaptureTsOnUdp_Param& param ,LPCTSTR lpLogName)
{
	if (m_hCaptureThread != NULL)
		return FALSE;

	_snprintf(m_CaptureParam.m_BindIp, MAX_PATH, "%s", param.m_BindIp);
	m_CaptureParam.m_BindPort = param.m_BindPort;
	_snprintf(m_CaptureParam.m_MulticastIp, MAX_PATH, "%s", param.m_MulticastIp);
	
	//_tcscpy(m_strLogName,lpLogName);
	_tcscpy(m_strLogName,_T("tsonUdp"));

	if (m_pUdp)
		delete m_pUdp;
	m_pUdp = new Udp();

	if (!m_pUdp->Open(m_CaptureParam.m_BindIp, m_CaptureParam.m_BindPort, m_CaptureParam.m_MulticastIp))
		return FALSE;

	//TODO:为什么这里有个 magic IP &port
	if (!m_pUdp->Connect("172.16.0.2", 5000))
		return FALSE;

	printf("Initialize udp capture!\n");

	return TRUE;
}
	
BOOL CCaptureTsOnUdp::StartCapture(void)
{
	if (m_hCaptureThread != NULL)
		return FALSE;
	
	m_hStopCaptureThread = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hCaptureThread = (HANDLE)_beginthreadex(NULL, 0, StartCaptureThread, this, 0, NULL);
	SetThreadPriority(m_hCaptureThread,THREAD_PRIORITY_TIME_CRITICAL);

	printf("Run udp capture thread!\n");
		
	return TRUE;
}
	
BOOL CCaptureTsOnUdp::SetDataOkEvent(HANDLE hDataOkEvent, UINT dataOkSize)
{
	if (m_hCaptureThread != NULL)
		return FALSE;

	m_hDataOkEvent = hDataOkEvent;
	m_DataOkSize = dataOkSize;

	return TRUE;
}

BOOL CCaptureTsOnUdp::SetGetDataCBFun(CaptureTsOnUdp_GetDataCBFun pFun, void* pUserData, UINT dataOkSize)
{
	if (m_hCaptureThread != NULL)
		return FALSE;

	m_pGetDataCBFun = pFun;
	m_pUserData = pUserData;

	m_DataOkSize = dataOkSize;

	return TRUE;
}

ULONG CCaptureTsOnUdp::GetCaptureDataSize()
{
	return m_Buffer.GetDataSize();
}
	
BOOL CCaptureTsOnUdp::GetCaptureData(BYTE* pData, UINT dataSize)
{
	return m_Buffer.GetData(pData, dataSize);
}
	
BOOL CCaptureTsOnUdp::StopCapture(void)
{
	SetEvent(m_hStopCaptureThread);
	WaitForSingleObject(m_hCaptureThread, INFINITE);

	CloseHandle(m_hCaptureThread);
	m_hCaptureThread = NULL;

	CloseHandle(m_hStopCaptureThread);
	m_hStopCaptureThread = NULL;

	m_Buffer.FreeBuffer();
	m_Data.FreeBuffer();

	printf("Stop udp capture thread!\n");
	
	return TRUE;
}

UINT CCaptureTsOnUdp::StartCaptureThread(void* pParam)
{
	CCaptureTsOnUdp* pCaptureTsOnUdp = (CCaptureTsOnUdp*)pParam;
	pCaptureTsOnUdp->CaptureOnThread();
	return 0;
}

void CCaptureTsOnUdp::CaptureOnThread()
{
	PBYTE	pRecvData = new BYTE[PACKET_SIZE_MAX];

	DWORD	wait;
	int		recvSize;
	DWORD dwOldTm = GetTickCount();
	DWORD dwCurTm = GetTickCount();
	LONGLONG llDwRecvSize=0;
	DWORD dwNowTm = GetTickCount();
	TCHAR strLog[MAX_PATH];
	char *pFromAddr;
	int nFromPort;
	while (TRUE)
	{
		wait = WaitForSingleObject(m_hStopCaptureThread, 0);
		if (wait == WAIT_OBJECT_0)
			break;
		
		//recvSize = m_pUdp->Read(pRecvData, PACKET_SIZE_MAX);
		recvSize = m_pUdp->Read_UDP(pRecvData, PACKET_SIZE_MAX,&pFromAddr,nFromPort);
		if (recvSize <= 0)
			continue;
		llDwRecvSize+=recvSize;
// 		if(GetTickCount() - dwNowTm > 120000)
// 		{
// 			dwNowTm =GetTickCount();
// 			//TODO:记录一下日志
// 			//!  receive data
// 			_stprintf_s(strLog,_countof(strLog),_T("udp: now=%d,recSize=%I64d"),dwNowTm,llDwRecvSize);
// 			WriteLog(m_strLogName,logLevelInfo,strLog);
// 		}
		if(m_pGetDataCBFun)
			m_pGetDataCBFun(pRecvData,recvSize,pFromAddr,nFromPort,m_pUserData);
	//		m_pGetDataCBFun(m_Data.m_pBuffer, dwCurSize, m_pUserData);
// 
// 		if ( !IsUdpPacketInTs(pRecvData, recvSize) )
// 		{
// 			_stprintf_s(strLog,_countof(strLog),_T("Throw a UDP Packet!(The Packet isn't in TS.)---CCaptureTsOnUdp::CaptureOnThread"));
// 			WriteLog(m_strLogName,logLevelWarring,strLog);
// 			continue;
// 		}
// 		
// 		UINT	  uBufferSize =	m_Buffer.GetDataSize();
// 		if (uBufferSize > 5*1024*1024)
// 		{
// 			_stprintf_s(strLog,_countof(strLog),_T("Throw a UDP Packet!(UDP buffer is too large,Buffer Size=%d.)---CCaptureTsOnUdp::CaptureOnThread"),uBufferSize);
// 			WriteLog(m_strLogName,logLevelWarring,strLog);
// 			continue;
// 		}
// 
// 		Buffer* p_packet_data;
// 		p_packet_data = m_Buffer.GetEmptyBuffer();
// 		if ( !p_packet_data )
// 			p_packet_data = new Buffer();
// 
// 		dwCurTm = GetTickCount() - dwOldTm;
// 		p_packet_data->FillData(pRecvData, recvSize);
// 		m_Buffer.AddFullBuffer(p_packet_data);
// 		
// 		DWORD dwCurSize = m_Buffer.GetDataSize();
// 		if (m_pGetDataCBFun /*&& (dwCurSize>m_DataOkSize || dwCurTm > 200)*/)			//200ms
// 		{
// 			dwOldTm = GetTickCount();
// 			if(dwCurSize > m_DataOkSize)
// 				dwCurSize = m_DataOkSize;
// 			m_Data.ExtendBuffer(dwCurSize);
// 			m_Buffer.GetData(m_Data.m_pBuffer, dwCurSize);
// 			m_pGetDataCBFun(m_Data.m_pBuffer, dwCurSize, m_pUserData);
// 		}
		
	}
	
	delete []pRecvData;
}

BOOL CCaptureTsOnUdp::IsUdpPacketInTs(const BYTE* pPacket, int packetSize)
{
	return TRUE;
    if ( packetSize < RTP_HEADER_SIZE || packetSize < 2)
        return FALSE;

	if ( pPacket[0] == 0x47 )
		return TRUE;

	int rtpVersion  = ( pPacket[0] & 0xC0 ) >> 6;
    int payloadType = ( pPacket[1] & 0x7F );

    if( rtpVersion == 2 )			//RTP packet version
    {
		if (payloadType == 33 )		//TS payload type
		{
			return FALSE;
		}
    }

	return TRUE;
}

BOOL CCaptureTsOnUdp::IsCapture()
{
	if (m_hCaptureThread)
		return TRUE;
	return FALSE;
}

int CCaptureTsOnUdp::sendData( const char *pData,int nDataSize,const char *pAddr,int nPort )
{
	return m_pUdp->Write(pAddr,nPort,(BYTE*)pData,nDataSize);
}

