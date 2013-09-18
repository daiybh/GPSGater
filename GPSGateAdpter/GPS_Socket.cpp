#include "StdAfx.h"
#include "GPS_Socket.h"
#include <process.h>
#pragma comment(lib,"TSIPDevice.lib")
CGPS_Socket::CGPS_Socket(void)
{
}

CGPS_Socket::~CGPS_Socket(void)
{
}

int CGPS_Socket::StartWork()
{
	ITsDevice_Card::CreateInstance(&m_pITSDevice_card);

	m_pDataList = new MGBufferListEx();
	m_pDataList->Initialize(50*1024*1024,100,524200);
	int nListenPort=GetPrivateProfileInt(_T("GPSSet"),_T("listenPort"),110,GetMgConfigFileName());

	if(!m_pITSDevice_card->InitDevice(nListenPort))
		return -2;
	m_pITSDevice_card->SetMemShareBuffer(m_pDataList,100);
	if(m_pITSDevice_card->StartInput()<=0)
		return -3;


//	HANDLE m_hCaptureThread = (HANDLE)_beginthreadex(NULL, 0, StartCaptureThread, this, 0, NULL);

	return 1;
}

UINT __stdcall CGPS_Socket::StartCaptureThread( void* pParam )
{
	CGPS_Socket *pThis = (CGPS_Socket*)pParam;
	pThis->CaptureOnThread();
	return 1;
}

void CGPS_Socket::CaptureOnThread()
{
	CString strLog;
	char *pData=NULL;
	while(1)
	{
		LISTITEMINFOEX *pItem = m_pDataList->FetchDataHeadItem();
		if(pItem==NULL)
		{
			Sleep(100);
			continue;
		}
		CString strTemp;
		strLog.Format(_T("%d"),pItem->dwFieldIndex);
		for(int i=0;i<pItem->nBufSizeUsedV0;i++)
		{
			if(pItem->pBufV0[i]==NULL)
				break;
			strTemp.Format(_T("%c"),pItem->pBufV0[i]);
			strLog+= strTemp;
		}
		OutputDebugString(strLog);
		pData = (char*)pItem->pBufV0;
		m_pDataList->AddItem2EmptyTail(pItem);
	}
}
INT64 g_int64Cnt=0;
long CGPS_Socket::getGPS( char *buf,char *addr,int &nPort )
{
	buf[0]='\0';
	addr[0]='\0';
	nPort =0;
	LISTITEMINFOEX *pItem = m_pDataList->FetchDataHeadItem();
	if(g_int64Cnt++ %100==0)
	{
		CString strLog;
		strLog.Format(_T("getGps--%I64d  %x"),g_int64Cnt,pItem);
		WriteLog(LOGNAME,logLevelInfo,strLog);
	}
	if(pItem==NULL)
		return 0;
	memcpy(buf ,pItem->pBufV0,pItem->nBufSizeUsedV0);
	buf[pItem->nBufSizeUsedV0+1] ='\0';
// 	memcpy(addr,pItem->curSocketInfo.udpSockAddr,16);
// 	nPort = pItem->socketPort;
	DWORD dwRet = pItem->nBufSizeUsedV0;
	m_pDataList->AddItem2EmptyTail(pItem);
	return dwRet;
}

long CGPS_Socket::getGPS( GPSGATEDATA *pGpsData ,char *buf)
{
	LISTITEMINFOEX *pItem = m_pDataList->FetchDataHeadItem();
	if(g_int64Cnt++ %1000==0)
	{
		CString strLog;
		strLog.Format(_T("getGps--%I64d  %x---list{%d,%d}"),g_int64Cnt,pItem,m_pDataList->GetSizeOfDataList(),m_pDataList->GetSizeOfEmptyList());
		OutputDebugString(strLog);
		//WriteLog(LOGNAME,logLevelInfo,strLog);
	}
	if(pItem==NULL)
		return 0;
	DWORD dwCanCopyLen = min(pGpsData->nDataLen-1,pItem->nBufSizeUsedV0);
	memcpy(buf ,pItem->pBufV0,dwCanCopyLen);
	buf[dwCanCopyLen+1] ='\0';

	memcpy(&pGpsData->curSocketInfo,&pItem->curSocketInfo,sizeof(SOCKETINFO));
	pGpsData->nDataLen = dwCanCopyLen;
	m_pDataList->AddItem2EmptyTail(pItem);
	return dwCanCopyLen;

}

long CGPS_Socket::writeGPS( const char *buf,const char *addr,int nPort )
{
	//nPort = 193;
	long nRet =m_pITSDevice_card->sendData(buf,strlen(buf),addr,nPort);
	CString strLog;
	CString sAddr(addr),sBuf(buf);
	strLog.Format(_T("writeGPS %s,%s,%d---ret=%d "),sBuf,sAddr,nPort,nRet);
	OutputDebugString(strLog);
	WriteLog(_T("GPS_Inteface"),logLevelInfo,strLog);
	return nRet;
}

long CGPS_Socket::writeGPS( const GPSGATEDATA * pGpsData ,char *pDatabuf,int nDataLen)
{
	long nRet =m_pITSDevice_card->sendData(pGpsData,pDatabuf,nDataLen);
//	CString strLog;
// 	CString sAddr(addr),sBuf(buf);
// 	strLog.Format(_T("writeGPS %s,%s,%d---ret=%d "),sBuf,sAddr,nPort,nRet);
// 	OutputDebugString(strLog);
// 	WriteLog(_T("GPS_Inteface"),logLevelInfo,strLog);
	return nRet;	
}
