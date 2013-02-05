// CaptureTsOnUdp.h: interface for the CCaptureTsOnUdp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPTURETSONUDP_H__A4B76838_207B_425D_A778_FB322F1ED3B6__INCLUDED_)
#define AFX_CAPTURETSONUDP_H__A4B76838_207B_425D_A778_FB322F1ED3B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VBufferT.h"
#include "Udp.h"

struct CaptureTsOnUdp_Param
{
	char m_BindIp[MAX_PATH];			// 绑定地址
	UINT m_BindPort;					// 绑定端口
	char m_MulticastIp[MAX_PATH];		// 多播地址

	CaptureTsOnUdp_Param(void)
	{
		_snprintf(m_BindIp, MAX_PATH, "%s", ("127.0.0.1"));
		m_BindPort = 12345;

		_snprintf(m_MulticastIp, MAX_PATH, "%s", ("0.0.0.0"));
	};
};

typedef void (*CaptureTsOnUdp_GetDataCBFun) (BYTE* pBuffer, UINT nDataSize,char *pFromAddr,int nPort, void* pUserData);

class CCaptureTsOnUdp  
{
public:
	CCaptureTsOnUdp();

	virtual ~CCaptureTsOnUdp();
	
	BOOL Init(const CaptureTsOnUdp_Param& param,LPCTSTR lpLogName);
	
	BOOL	StartCapture(void);
	
	BOOL	SetDataOkEvent(HANDLE hDataOkEvent, UINT dataOkSize);	
	BOOL	SetGetDataCBFun(CaptureTsOnUdp_GetDataCBFun pFun, void* pUserData, UINT dataOkSize);

	ULONG	GetCaptureDataSize();
	
	BOOL	GetCaptureData(BYTE* pData, UINT dataSize);
	
	BOOL	StopCapture(void);

	BOOL	IsCapture();
	int		sendData(const char *pData,int nDataSize,const char *pAddr,int nPort );
private:
	static UINT __stdcall StartCaptureThread(void* pParam);
	void CaptureOnThread();

	int ReportError();
	
	BOOL IsUdpPacketInTs(const BYTE* pPacket, int packetSize);

	CaptureTsOnUdp_Param	m_CaptureParam;
	SOCKET			m_Socket;

	Udp*			m_pUdp;
	
	VBufferT		m_Buffer;

	HANDLE			m_hCaptureThread;
	HANDLE			m_hStopCaptureThread;

	HANDLE			m_hDataOkEvent;
	CaptureTsOnUdp_GetDataCBFun	m_pGetDataCBFun;
	void*			m_pUserData;
	UINT			m_DataOkSize;

	Buffer			m_Data;
	TCHAR			m_strLogName[MAX_PATH];
};

#endif // !defined(AFX_CAPTURETSONUDP_H__A4B76838_207B_425D_A778_FB322F1ED3B6__INCLUDED_)
