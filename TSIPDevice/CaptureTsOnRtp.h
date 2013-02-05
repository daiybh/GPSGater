// CaptureTsOnRtp.h: interface for the CaptureTsOnRtp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPTURETSONRTP_H__6603B579_4A8E_4B19_8F6A_9B809F16E1B8__INCLUDED_)
#define AFX_CAPTURETSONRTP_H__6603B579_4A8E_4B19_8F6A_9B809F16E1B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VBufferT.h"
#include "Rtp.h"

struct CaptureTsOnRtp_Param
{
	char	m_BindIp[MAX_PATH];					// 绑定地址
	UINT	m_BindPort;							// 绑定端口
	char	m_MulticastIp[MAX_PATH];			// 多播地址

	BOOL	m_bSSRC;							// 是否验证SSRC
	INT32	m_nSSRC;							// SSRC

	CaptureTsOnRtp_Param(void)
	{
		_snprintf(m_BindIp, MAX_PATH, "%s", ("127.0.0.1"));
		m_BindPort = 12345;
		_snprintf(m_MulticastIp, MAX_PATH, "%s", ("0.0.0.0"));

		m_bSSRC = FALSE;
		m_nSSRC = 0;
	}
};

typedef void (*CaptureTsOnRtp_GetDataCBFun) (BYTE* pBuffer, UINT nDataSize,char *pFromAddr,int nPort, void* pUserData);

class CaptureTsOnRtp  
{
public:
	CaptureTsOnRtp();
	virtual ~CaptureTsOnRtp();
	
	BOOL Init(const CaptureTsOnRtp_Param& param);
	
	BOOL	StartCapture(void);
	
	BOOL	SetDataOkEvent(HANDLE hDataOkEvent, UINT dataOkSize);

	BOOL	SetGetDataCBFun(CaptureTsOnRtp_GetDataCBFun pFun, void* pUserData, UINT dataOkSize);
	
	ULONG	GetCaptureDataSize(void);
	
	BOOL	GetCaptureData(BYTE* pData, UINT dataSize);
	
	BOOL	StopCapture(void);

	BOOL	IsCapture();

private:
	static UINT __stdcall StartCaptureThread(void* pParam);
	void CaptureOnThread();

	HANDLE			m_hStopCaptureThread;
	HANDLE			m_hCaptureThread;

	HANDLE			m_hDataOkEvent;
	CaptureTsOnRtp_GetDataCBFun	m_pGetDataCBFun;
	void*			m_pUserData;
	UINT			m_DataOkSize;

	VBufferT		m_Buffer;
	CaptureTsOnRtp_Param	m_CaptureParam;

	Rtp*			m_pRtp;

	Buffer			m_Data;
};

#endif // !defined(AFX_CAPTURETSONRTP_H__6603B579_4A8E_4B19_8F6A_9B809F16E1B8__INCLUDED_)
