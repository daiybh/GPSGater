#include "StdAfx.h"
#include "Socket_IOCP.h"
#include "MGBufferListEx.h"

Socket_IOCP::Socket_IOCP(void)
{
	m_pGPS_Iocp =NULL;
	m_pDataList =NULL;
}

Socket_IOCP::~Socket_IOCP(void)
{
}

BOOL Socket_IOCP::InitDevice(int nListenPort)
{
	int nBindPort_UDP ,nBindPort_TCP;
	//nBindPort_TCP = nBindPort_UDP= 110;
	//nBindPort_TCP = nBindPort_UDP=GetPrivateProfileInt(_T("GPSSet"),_T("listenPort"),110,GetMgConfigFileName());
	nBindPort_TCP = nBindPort_UDP=nListenPort;
	m_pGPS_Iocp = new GPS_Iocp();
	return 
		m_pGPS_Iocp->StartWork(NULL,nBindPort_TCP,RecvCB,this);
}

void buf2HexStr(const char *pSrcbuf,char *pDestBuf,int nLen)
{
	if( nLen> 1024/2)	nLen = 1024;
	char *pPos = pDestBuf;
	for(int i=0;i<nLen;i++)
	{
		sprintf(pPos,"%02x",(unsigned char)pSrcbuf[i]);
		pPos+=2;
	}	
}
void Socket_IOCP::RecvCB(DWORD dwStatus/*״̬*/,BYTE *pClient, BYTE* pBuf, DWORD dwSize, LPVOID pUserData)
{
	//printf("recvCB_%I64d_client=%x,pbuf=%s,dwSize=%d\r\n",g_iRecvCnt++,pClient,pBuf,dwSize);
	if(dwSize>1024)
	{
		//非法数据
		CString  slog;
		slog.Format(_T("recvCB_Error_size:%d\r\n"),dwSize);
		OutputDebugString(slog);
	//	return;
	}

	Socket_IOCP *pThis = (Socket_IOCP*)pUserData;
	LISTITEMINFOEX *pItem = NULL;
	int nwaitCnt = 300;
	while(pItem==NULL && nwaitCnt-->0)
	{
		pItem=pThis->m_pDataList->FetchEmptyHeadItem(dwSize,0,0,0);
		if(pItem ==NULL)
			Sleep(10);
	}
	CString strLog;
	if(pItem==NULL)
	{
		strLog.Format(_T("Lost Data %s--%d,dataCnt=%d"),CStringA(pBuf),dwSize,pThis->m_pDataList->GetSizeOfDataList());
		OutputDebugString(strLog);
		return;
	}
	//if(dwSize >100)dwSize = 100;

// 	char g_tempbuf[1024];
// 	ZeroMemory(g_tempbuf,1024);
// 	buf2HexStr((const char*)pBuf,g_tempbuf,dwSize);
// 	CStringA ss;
// 	ss.Format(("recvCB__client=%x,pbuf=%s---%s,dwSize=%d--%d"),pClient,CStringA(pBuf),CStringA(g_tempbuf),dwSize,pThis->m_pDataList->GetSizeOfDataList());
// 	OutputDebugStringA(ss);
// 	CString ss1;
// 	ss1.Format(_T("recvCB__client=%x,pbuf=%s---%s,dwSize=%d--%d"),pClient,CStringA(pBuf),CStringA(g_tempbuf),dwSize,pThis->m_pDataList->GetSizeOfDataList());
// 
// 	WriteLog(_T("iocp"),logLevelInfo,ss1);

	memcpy(pItem->pBufV0,pBuf,dwSize);
	pItem->nBufSizeUsedV0 = dwSize;
	//memcpy(&pItem->curSocketInfo.tcpSocketHandle,&pClient,sizeof(DWORD));
	pItem->curSocketInfo.tcpSocketHandle =(DWORD) pClient;
	
	pThis->m_pDataList->AddItem2DataTail(pItem);
	
}
BOOL Socket_IOCP::ReleaseDevice()
{
	return 1;
}

int Socket_IOCP::StartInput()
{
	return 1;
}

int Socket_IOCP::StopInput()
{
	return 0;
}

int Socket_IOCP::sendData( const char *pData,int nDataSize,const char *pAddr,int nPort )
{
	return 0;
}

int Socket_IOCP::sendData( const GPSGATEDATA *pGpsGateData )
{

	BYTE *pcur =(BYTE*) pGpsGateData->curSocketInfo.tcpSocketHandle;
	BYTE *pOld =(BYTE*) pGpsGateData->oldSocketInfo.tcpSocketHandle;
	
	return m_pGPS_Iocp->SendData(pOld,
		pcur,
		(BYTE*)pGpsGateData->pDatabuf,pGpsGateData->nDataLen);
	return 1;
}

void Socket_IOCP::SetMemShareBuffer( MGBufferListEx *pBufferList,DWORD dwBufferSize )
{
	m_pDataList = pBufferList;
}
