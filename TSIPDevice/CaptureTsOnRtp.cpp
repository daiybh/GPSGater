// CaptureTsOnRtp.cpp: implementation of the CaptureTsOnRtp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CaptureTsOnRtp.h"
#include "TimeCount.h"

const UINT PACKET_SIZE = 188 * 7;				// 1316 Byte
const UINT PACKET_SIZE_MAX = 65535-RTP_HEADER_SIZE;				// 65K	Byte WinCE为10KB
const UINT BUFFER_SIZE = 1310720;				// 1024 * 1024 * 1.25 Byte
const UINT PAYLOAD_TYPE	= 33;					// 负载类型 MP2T

CaptureTsOnRtp::CaptureTsOnRtp()
{
	m_pGetDataCBFun = NULL;
	m_pUserData = NULL;
	m_hDataOkEvent = NULL;
	m_DataOkSize = 0;

	m_hStopCaptureThread = NULL;
	m_hCaptureThread = NULL;

	m_pRtp = NULL;
}

CaptureTsOnRtp::~CaptureTsOnRtp()
{
	StopCapture();
	
	if (m_pRtp)
		delete m_pRtp;
}

BOOL CaptureTsOnRtp::Init( const CaptureTsOnRtp_Param& param )
{
	if (m_hCaptureThread != NULL)
		return FALSE;

	_snprintf(m_CaptureParam.m_BindIp, MAX_PATH, "%s", param.m_BindIp);
	m_CaptureParam.m_BindPort = param.m_BindPort;
	_snprintf(m_CaptureParam.m_MulticastIp, MAX_PATH, "%s", param.m_MulticastIp);
	m_CaptureParam.m_bSSRC = param.m_bSSRC;
	m_CaptureParam.m_nSSRC = param.m_nSSRC;
	
	if (m_pRtp)
		delete m_pRtp;
	m_pRtp = new Rtp();
	
	if (!m_pRtp->Open(m_CaptureParam.m_BindIp, m_CaptureParam.m_BindPort, m_CaptureParam.m_MulticastIp))
		return FALSE;
	
	printf("Initialize rtp capture!\n");

	return TRUE;
}
	
BOOL CaptureTsOnRtp::StartCapture(void)
{
	if (m_hCaptureThread != NULL)
		return TRUE;
	
	m_hStopCaptureThread = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hCaptureThread = (HANDLE)_beginthreadex(NULL, 0, StartCaptureThread, this, 0, NULL);
	SuspendThread(m_hCaptureThread);
	SetThreadPriority(m_hCaptureThread,THREAD_PRIORITY_HIGHEST);
	ResumeThread(m_hCaptureThread);

	printf("Run rtp capture thread!\n");
	return TRUE;
}

BOOL CaptureTsOnRtp::SetDataOkEvent(HANDLE hDataOkEvent, UINT dataOkSize)
{
	if (m_hCaptureThread != NULL)
		return FALSE;

	m_hDataOkEvent = hDataOkEvent;
	m_DataOkSize = dataOkSize;

	return TRUE;
}

BOOL CaptureTsOnRtp::SetGetDataCBFun(CaptureTsOnRtp_GetDataCBFun pFun, void* pUserData, UINT dataOkSize)
{
	if (m_hCaptureThread != NULL)
		return FALSE;

	m_pGetDataCBFun = pFun;
	m_pUserData = pUserData;

	m_DataOkSize = dataOkSize;

	return TRUE;
}

	
ULONG CaptureTsOnRtp::GetCaptureDataSize()
{
	return m_Buffer.GetDataSize();
}
	
BOOL CaptureTsOnRtp::GetCaptureData(BYTE* pData, UINT dataSize)
{
	return m_Buffer.GetData(pData, dataSize);
}

BOOL CaptureTsOnRtp::StopCapture(void)
{
	if (m_hCaptureThread == NULL)
		return TRUE;

	SetEvent(m_hStopCaptureThread);
	WaitForSingleObject(m_hCaptureThread, INFINITE);
	
	CloseHandle(m_hCaptureThread);
	m_hCaptureThread = NULL;

	CloseHandle(m_hStopCaptureThread);
	m_hStopCaptureThread = NULL;

	m_Buffer.FreeBuffer();
	m_Data.FreeBuffer();

	printf("Stop rtp capture thread!\n");
	return TRUE;
}

UINT CaptureTsOnRtp::StartCaptureThread(void* pParam)
{
	CaptureTsOnRtp* pCaptureTsOnRtp = (CaptureTsOnRtp*)pParam;
	pCaptureTsOnRtp->CaptureOnThread();

	return 0;
}

void CaptureTsOnRtp::CaptureOnThread()
{
	PBYTE	pRecvData = new BYTE[PACKET_SIZE_MAX];
	DWORD	wait;
	int		recvSize;
	INT8	nPayloadType;
	UINT16	nSequenceNumber;
	INT32	nTimeStamp;
	INT32	nSsrc;
	CTimeCount tm;
	DWORD dwOldTm = GetTickCount();
	DWORD dwCurTm = GetTickCount();

	while (TRUE)
	{
		wait = WaitForSingleObject(m_hStopCaptureThread, 0);
		if (wait == WAIT_OBJECT_0)
			break;
		
		recvSize = m_pRtp->Read(pRecvData, PACKET_SIZE_MAX, &nPayloadType, 
			&nSequenceNumber, &nTimeStamp, &nSsrc);				
		if (recvSize <= 0)
			continue;

		if (nPayloadType != PAYLOAD_TYPE)
		{	
		//	OutputDebugString(L"uuuuu:RTP buffer is too large!111222");
			continue;
		}

		if (m_CaptureParam.m_bSSRC && m_CaptureParam.m_nSSRC != nSsrc)
		{
		//	OutputDebugString(L"uuuuu:RTP buffer is too large!111111");
			continue;
		}
		
		if (m_Buffer.GetDataSize() > 20*1024*1024)
		{
			printf("RTP buffer is too large!");
		//	OutputDebugString(L"uuuuu:RTP buffer is too large!");
			continue;
		}
		Buffer* p_packet_data;
		p_packet_data = m_Buffer.GetEmptyBuffer();
		if ( !p_packet_data )
		{
			p_packet_data = new Buffer();
		//	OutputDebugString(L"uuuuu:p_packet_data = new Buffer()");
		}

		dwCurTm = GetTickCount() - dwOldTm;
		p_packet_data->FillData(pRecvData, recvSize);
		m_Buffer.AddFullBuffer(p_packet_data);

		DWORD dwCurSize = m_Buffer.GetDataSize();
		if (m_hDataOkEvent && dwCurSize > m_DataOkSize)
		{
			SetEvent(m_hDataOkEvent);
		}
		else if (m_pGetDataCBFun && (dwCurSize > m_DataOkSize) || dwCurTm > 200)
		{
			dwOldTm = GetTickCount();
			if (dwCurSize <= m_DataOkSize)
			{
				m_Data.ExtendBuffer(dwCurSize);
				m_Buffer.GetData(m_Data.m_pBuffer, dwCurSize);

				m_pGetDataCBFun(m_Data.m_pBuffer, dwCurSize,NULL,0, m_pUserData);
			}
			else
			{
				m_Data.ExtendBuffer(m_DataOkSize);
				m_Buffer.GetData(m_Data.m_pBuffer, m_DataOkSize);

				m_pGetDataCBFun(m_Data.m_pBuffer, m_DataOkSize,NULL,0, m_pUserData);
			}			 
		}
	}
	
	delete []pRecvData;
}

BOOL CaptureTsOnRtp::IsCapture()
{
	if (m_hCaptureThread)
		return TRUE;
	return FALSE;
}