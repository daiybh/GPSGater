#include "StdAfx.h"
#include "SimpleDevice.h"
#include "MGBufferListEx.h"

SimpleDevice::SimpleDevice(void)
{
	WSADATA data;									//定义WSADATA结构体对象
	WORD	w = MAKEWORD(2, 2);						//定义版本号码
	WSAStartup(w, &data);	
}

SimpleDevice::~SimpleDevice(void)
{
}

#define MAX_SOCKET_CONNECT_CNT 70
BOOL SimpleDevice::InitDevice()
{
	int nBindPort_UDP ,nBindPort_TCP;
	nBindPort_TCP = nBindPort_UDP= 110;
	nBindPort_TCP = nBindPort_UDP=GetPrivateProfileInt(_T("GPSSet"),_T("listenPort"),110,GetMgConfigFileName());


	m_bNeedClearSocketList=false;
	m_MAX_Socket_Connenct_Cnt = GetPrivateProfileInt(_T("GPSSet"),_T("max_connect_cnt"),MAX_SOCKET_CONNECT_CNT,GetMgConfigFileName());
	// 绑定套接字
	memset((PVOID)&addr1, 0, sizeof(SOCKADDR_IN));
	addr1.sin_family = AF_INET;   
	addr1.sin_port = htons(nBindPort_UDP);
	//addr1.sin_addr.s_addr = inet_addr("116.255.143.68");

	addr1.sin_addr.S_un.S_addr = INADDR_ANY;

	m_Socket_UDP = socket(AF_INET, SOCK_DGRAM, 0);										//创建UDP套接字
	//WSAAsyncSelect(m_s1, m_hWnd, WM_SCOKET_RECV, FD_READ);						//将套接字设置为异步模式
	int iResult = bind(m_Socket_UDP, (sockaddr*)&addr1, sizeof(addr1));					//绑定套接字
	CString strLog;
	if(iResult==SOCKET_ERROR)
	{

		int isErr = WSAGetLastError();
		strLog.Format(_T("Bind udp port[%d] failed. lastErr=%d"),isErr);
		WriteLog(LOGNAME,logLevelError,strLog);
		return FALSE;	
	}
	m_Socket_TCP = socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN tcpAddr;
	tcpAddr.sin_family = AF_INET;
	tcpAddr.sin_port = htons(nBindPort_TCP);
	tcpAddr.sin_addr.s_addr= htonl(INADDR_ANY);
	
	iResult = bind(m_Socket_TCP,(sockaddr*)&tcpAddr,sizeof(tcpAddr));
	if(iResult == SOCKET_ERROR)
	{
		int isErr = WSAGetLastError();
		strLog.Format(_T("Bind tcp port[%d] failed. lastErr=%d"),isErr);
		WriteLog(LOGNAME,logLevelError,strLog);
		return FALSE;	
	}
	iResult = listen(m_Socket_TCP,10);
	if(iResult==SOCKET_ERROR)
	{
		int isErr = WSAGetLastError();
		strLog.Format(_T("listen tcp port[%d] failed. lastErr=%d"),isErr);
		WriteLog(LOGNAME,logLevelError,strLog);
		return FALSE;
	}

	DWORD nMode = 1;
	iResult = ioctlsocket(m_Socket_TCP,FIONBIO,&nMode);
	if(iResult == SOCKET_ERROR)
	{
		int isErr = WSAGetLastError();
		strLog.Format(_T("ioctlsocket tcp port[%d] failed. lastErr=%d"),isErr);
		WriteLog(LOGNAME,logLevelError,strLog);
		closesocket(m_Socket_TCP);
		WSACleanup();
		return FALSE;
	}


	I_XMLParser::CreateInstance(&m_pIXMLParser);
	return iResult!=SOCKET_ERROR;
}

BOOL SimpleDevice::ReleaseDevice()
{
	return 0;
}

int SimpleDevice::StartInput()
{
	m_bIsExit = FALSE;
	m_hStopCaptureThread = CreateEvent(NULL, FALSE, FALSE, NULL);

	//m_hCaptureThread_UDP = (HANDLE)_beginthreadex(NULL, 0, StartCaptureThread_UDP, this, 0, NULL);
	//SetThreadPriority(m_hCaptureThread_UDP,THREAD_PRIORITY_TIME_CRITICAL);

/*
	m_hAcceptThread_TCP = (HANDLE)_beginthreadex(NULL, 0, _AcceptConProc_TCP, this, 0, NULL);
	SetThreadPriority(m_hAcceptThread_TCP,THREAD_PRIORITY_TIME_CRITICAL);

	m_hRecvThread_TCP = (HANDLE)_beginthreadex(NULL, 0, _RecvDataProc, this, 0, NULL);
	SetThreadPriority(m_hRecvThread_TCP,THREAD_PRIORITY_TIME_CRITICAL);
*/
	m_hRecvThread_TCP = (HANDLE)_beginthreadex(NULL, 0, _RecvDataProc_Select, this, 0, NULL);

	CString strLog;
	strLog.Format(_T("SimpleDevice::StartInput  ===%x_%x"),m_hAcceptThread_TCP,m_hRecvThread_TCP);
	WriteLog(LOGNAME,logLevelInfo,strLog);
	return 1;
}

int SimpleDevice::StopInput()
{
	m_bIsExit = TRUE;
	SetEvent(m_hStopCaptureThread);
	WaitForSingleObject(m_hCaptureThread_UDP, INFINITE);

	CloseHandle(m_hCaptureThread_UDP);
	m_hCaptureThread_UDP = NULL;

	WaitForSingleObject(m_hAcceptThread_TCP, INFINITE);

	CloseHandle(m_hAcceptThread_TCP);
	m_hAcceptThread_TCP = NULL;

	WaitForSingleObject(m_hRecvThread_TCP, INFINITE);

	CloseHandle(m_hRecvThread_TCP);
	m_hRecvThread_TCP = NULL;



	CloseHandle(m_hStopCaptureThread);
	m_hStopCaptureThread = NULL;

	return 0;
}
struct GPS_Socket_Info 
{
	char socketType; //1:tcp 0:udp
	SOCKADDR_IN udpSocketIn;
	SOCKET   tcpSocketHandle;
};
int SimpleDevice::sendData( const char *pData,int nDataSize,const char *pAddr,int nPort )
{
	memset((PVOID)&m_SendAddr, 0, sizeof(SOCKADDR_IN));

//	SOCKADDR_IN *pSockAddr = (SOCKADDR_IN*)pAddr;
	m_SendAddr.sin_family = AF_INET;
	m_SendAddr.sin_port = nPort;
	m_SendAddr.sin_addr.s_addr = inet_addr(pAddr);
	int sendSize =0;
	//m_SendAddr = recvfromAddr;
	//memcpy(&m_SendAddr,pAddr,sizeof(m_SendAddr));
	//int sendSize = sendto(m_Socket, pData, nDataSize, 0, (SOCKADDR*)pAddr, sizeof(SOCKADDR_IN));
	//if (sendSize <= 0)
	//	return -1;
	sendSize = sendto(m_Socket_UDP, pData, nDataSize, 0, (SOCKADDR*)&m_SendAddr, sizeof(SOCKADDR_IN));
	if (sendSize <= 0)
		return -1;
	return sendSize;
	return 0;
}

BOOL isSame_SocketInfo(const SOCKETINFO *pCur,const SOCKETINFO *pOld)
{
	if(pOld->tcpSocketHandle==0)
		return TRUE;
	if(pCur->socketType !=pOld->socketType)
		return FALSE;
	if(pCur->tcpSocketHandle != pOld->tcpSocketHandle)
		return FALSE;
// 	if(pCur->udpSockAddr!= pOld->udpSockAddr)
// 		return FALSE;
	return TRUE;
}
int SimpleDevice::sendData( const GPSGATEDATA *pGpsGateData )
{
	//先发送，再判断是否要去掉oldsocket
	int curSocketType = pGpsGateData->curSocketInfo.socketType;
	if(curSocketType==2)//udp
	{
		int nSendSize =0;
		nSendSize = sendto(m_Socket_UDP,pGpsGateData->pDatabuf,pGpsGateData->nDataLen,0,(SOCKADDR*)&pGpsGateData->curSocketInfo.udpSockAddr,sizeof(SOCKADDR_IN));
		return nSendSize<=0?-1:nSendSize;
	}
	else if(curSocketType==1)//tcp
	{
		int nDataLen = strlen(pGpsGateData->pDatabuf);
		int nSendSize =send(pGpsGateData->curSocketInfo.tcpSocketHandle,pGpsGateData->pDatabuf,nDataLen,0);
		if(!isSame_SocketInfo(&pGpsGateData->curSocketInfo,&pGpsGateData->oldSocketInfo))
		{
			//从socetlist中删除 oldsocketinfo;
			SOCKET needDelSocket = pGpsGateData->oldSocketInfo.tcpSocketHandle;
			m_cCriticalSection.Lock();
			for(int i=0;i<m_aClientSocketList.GetSize();i++)
			{
				if(m_aClientSocketList.GetAt(i) == needDelSocket)
				{
					shutdown(needDelSocket,SB_BOTH);
					closesocket(needDelSocket);
					m_aClientSocketList.RemoveAt(i);
					break;
				}
			}
			m_cCriticalSection.Unlock();
		}
		if(nSendSize== SOCKET_ERROR)
		{
			return -1;
		}
		return nSendSize;

	}
	return 0;
}

void SimpleDevice::SetMemShareBuffer( MGBufferListEx *pBufferList,DWORD dwBufferSize )
{
	m_pDataList = pBufferList;
}

UINT SimpleDevice::StartCaptureThread_UDP(void* pParam)
{
	SimpleDevice* pCaptureTsOnUdp = (SimpleDevice*)pParam;
	pCaptureTsOnUdp->CaptureOnThread_UDP();
	return 0;
}
// void SimpleDevice::ReadData()
// {
// 
// 	int selectState;
// 	int recvSize;
// 
// 	int len = sizeof(SOCKADDR_IN);
// 
// 	selectState = Select(SELECT_MODE_READY, nTimeOut);
// 	if (SELECT_STATE_TIMEOUT == selectState)
// 		return 0;
// 
// 	if (SELECT_STATE_READY == selectState)
// 	{
// 		//recvSize = recv(m_Socket, (char*)pBuffer, readSize, 0);
// 		sockaddr_in   sa;
// 		int  fromlen = sizeof(sa);
// 		recvSize = recvfrom(m_Socket,(char*)pBuffer,readSize,0,(SOCKADDR *) &sa,&fromlen);
// 		*pFromAddr = inet_ntoa(sa.sin_addr);
// 		nFromPort = sa.sin_port;
// 		if (recvSize <= 0)
// 			return -1;
// 		return recvSize;
// 	}
// }
void SimpleDevice::CaptureOnThread_UDP()
{
	const DWORD PACKET_SIZE_MAX = 400;
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
		//sockaddr_in   sa;
		int  fromlen = sizeof(recvfromAddr);
		//recvSize = m_pUdp->Read(pRecvData, PACKET_SIZE_MAX);
		//recvSize = m_pUdp->Read_UDP(pRecvData, PACKET_SIZE_MAX,&pFromAddr,nFromPort);
		recvSize = recvfrom(m_Socket_UDP,(char*)pRecvData,PACKET_SIZE_MAX,0,(SOCKADDR *) &recvfromAddr,&fromlen);
		if (recvSize <= 0)
			continue;

		LISTITEMINFOEX *pItem = NULL;
		pItem =	m_pDataList->FetchEmptyHeadItem(recvSize,0,0,0);
		
		llDwRecvSize+=recvSize;
		dwNowTm = GetTickCount();
		if(dwNowTm -dwOldTm> 60*1000)
		{
			_stprintf(strLog,_T("recv--%I64d--%x"),llDwRecvSize,pItem);
			WriteLog(LOGNAME,logLevelInfo,strLog);
			dwOldTm = dwNowTm;
		}
		if (pItem == NULL)
		{
			continue;
		}
		
		memcpy_s(pItem->pBufV0,pItem->nBufSizeV0,pRecvData,recvSize);
		pItem->nBufSizeUsedV0 = recvSize;
	//	pItem->dwFieldIndex = m_dwPostData++;
		//*pFromAddr = inet_ntoa(sa.sin_addr);
		//nFromPort = sa.sin_port;
		//memcpy(pItem->socketAddr,&recvfromAddr,fromlen);
		//SOCKADDR_IN *pAddr =(SOCKADDR_IN*)pItem->socketAddr;
		memcpy(&pItem->curSocketInfo.udpSockAddr,&recvfromAddr,sizeof(recvfromAddr));
		pItem->curSocketInfo.socketType = 2;
		m_pDataList->AddItem2DataTail(pItem);
	}
}

UINT __stdcall SimpleDevice::_AcceptConProc_TCP( void* pParam )
{
	SimpleDevice *pThis = (SimpleDevice*)pParam;
	pThis->AcceptConProcThread();
	return 0;
}
void SimpleDevice::AcceptConProcThread()
{
	SOCKADDR_IN stClient;
	int nSize = sizeof(stClient);
	while(!m_bIsExit)
	{
		SOCKET sClient = accept(m_Socket_TCP,(sockaddr*)&stClient,&nSize);
		if(sClient!=INVALID_SOCKET)
		{
			CString strLog;
			CString ssIPA(inet_ntoa(stClient.sin_addr));
			strLog.Format(_T("accept %d_%s:%d"),sClient,ssIPA,ntohs(stClient.sin_port));
			OutputDebugString(strLog);
			WriteLog(LOGNAME,logLevelInfo,strLog);

			bool bNeedClear=false;

			m_cCriticalSection.Lock();
			//判断是否达到了最大连接数
			int icnt=m_aClientSocketList.GetSize();
			if(icnt > m_MAX_Socket_Connenct_Cnt)
				bNeedClear=true;
			m_cCriticalSection.Unlock();

			if(bNeedClear)
			{
				//这里需要一个大锁 来锁
				DWORD dTime = GetTickCount();
				strLog.Format(_T("\r\nbegin-%d--acceptSockets cnt=%d more than MaxCnt[%d],clear All\r\n"),dTime,icnt,m_MAX_Socket_Connenct_Cnt);
				OutputDebugString(strLog);
				WriteLog(LOGNAME,logLevelInfo,strLog);
				m_bNeedClearSocketList = TRUE;
				m_cClearSockets_CriticalSection.Lock();
				for(int i=0;i<icnt;i++)
				{
					closesocket(m_aClientSocketList.GetAt(i));
				}
				m_aClientSocketList.RemoveAll();
				m_cClearSockets_CriticalSection.Unlock();
	
				strLog.Format(_T("\r\nEnd-%d--acceptSockets cnt=%d more than MaxCnt[%d],clear All\r\n"),dTime,icnt,m_MAX_Socket_Connenct_Cnt);
				OutputDebugString(strLog);
				WriteLog(LOGNAME,logLevelInfo,strLog);
			}

			m_bNeedClearSocketList = FALSE;
			m_cCriticalSection.Lock();
			m_aClientSocketList.Add(sClient);
			m_cCriticalSection.Unlock();
		}
	}
}

UINT __stdcall SimpleDevice::_RecvDataProc( LPVOID lParam )
{
	SimpleDevice *pThis = (SimpleDevice*)lParam;
	pThis->RecvDataThread();
	return 0;
}

void SimpleDevice::RecvDataThread()
{
	fd_set fdread;
	int nRet = 0;
	int nRecv = 0;
	SOCKET sRead;
	TIMEVAL stTime;
	stTime.tv_sec = 1;
	stTime.tv_usec = 0;

#define RECVBUFFERSIZE 32768
	char * pRecvBuf = new char[RECVBUFFERSIZE];


	int  fromlen = sizeof(recvfromAddr);
	while(!m_bIsExit)
	{
		FD_ZERO(&fdread);
		if(m_Socket_UDP !=INVALID_SOCKET)
		{
			FD_SET(m_Socket_UDP,&fdread);
		}
		m_cClearSockets_CriticalSection.Lock();

		m_cCriticalSection.Lock();
		for(int i=0;i<m_aClientSocketList.GetSize();i++)
		{
			FD_SET(m_aClientSocketList.GetAt(i),&fdread);
		}
		m_cCriticalSection.Unlock();
		if((nRet = select(0,&fdread,NULL,NULL,&stTime))==SOCKET_ERROR)
		{
			m_cClearSockets_CriticalSection.Unlock();
			Sleep(10);
			continue;
		}
		else if(nRet>0)
		{
			if(FD_ISSET(m_Socket_UDP,&fdread))
			{
				int recvSize=0;
				recvSize = recvfrom(m_Socket_UDP,(char*)pRecvBuf,RECVBUFFERSIZE,0,(SOCKADDR *) &recvfromAddr,&fromlen);
				if(recvSize >0)
				{
					PasreTcpCmd(2,m_Socket_UDP,pRecvBuf,recvSize, &recvfromAddr);
				}
			}
			for (int i=0;i<m_aClientSocketList.GetSize();i++)
			{
				if(m_bNeedClearSocketList)
					break;
				m_cCriticalSection.Lock();
				if(i >= m_aClientSocketList.GetSize())
				{
					m_cCriticalSection.Unlock();
					continue;
				}
				sRead = m_aClientSocketList.GetAt(i);
				m_cCriticalSection.Unlock();

				if(FD_ISSET(sRead,&fdread))
				{
					nRecv = recv(sRead,pRecvBuf,RECVBUFFERSIZE,0);
					if(nRecv==0 || nRecv==SOCKET_ERROR)
					{
						closesocket(sRead);
						m_cCriticalSection.Lock();
						m_aClientSocketList.RemoveAt(i);
						m_cCriticalSection.Unlock();
					}
					else
						PasreTcpCmd(1,sRead,pRecvBuf,nRecv,NULL);
				}

			}
		}
		m_cClearSockets_CriticalSection.Unlock();
	}

}
int getASI(char hex)
{	
	if(hex >= 'a' && hex <='f' || hex>='A' && hex <='F')
		return (hex &0x0f)+9;
	else
		return hex &0x0f;

}
int hex2dec(const char *pHex)
{
	int nLen = strlen(pHex);
	int nResult=0;
	for(int i=0;i<nLen;i++)
	{
		nResult+= getASI(pHex[i]) *pow((double)16,(double)(nLen-i-1));
	}
	return nResult;
}
CString str2hex(char *pStr)
{
	int nLen = strlen(pStr);
	CString ss;
	for(int i=0;i<nLen;i++)
	{
		CString strTemp;
		strTemp.Format(_T("%c "),pStr[i]);
		ss += strTemp;
	}
	return ss;
}
void SimpleDevice::PasreTcpCmd(int socketType,SOCKET s,char *pRecvBuf,int nLen,SOCKADDR_IN* pSockAddr_in)
{
	CString ss;
	pRecvBuf[nLen]='\0';
// 	ss.Format(_T("\r\npasreTcpCmd(%s)--%s"),CString(pRecvBuf),str2hex(pRecvBuf));
// 	OutputDebugString(ss);

		LISTITEMINFOEX *pItem = NULL;
		pItem =	m_pDataList->FetchEmptyHeadItem(nLen,0,0,0);

		if (pItem == NULL)
		{
			return;
		}
		
		memcpy_s(pItem->pBufV0,pItem->nBufSizeV0,pRecvBuf,nLen);
		pItem->nBufSizeUsedV0 = nLen;
	//	pItem->dwFieldIndex = m_dwPostData++;
		//*pFromAddr = inet_ntoa(sa.sin_addr);
		//nFromPort = sa.sin_port;
		//memcpy(pItem->socketAddr,&recvfromAddr,fromlen);
		//SOCKADDR_IN *pAddr =(SOCKADDR_IN*)pItem->socketAddr;
		if(socketType==2)
		{
			memcpy(&pItem->curSocketInfo.udpSockAddr,pSockAddr_in,sizeof(SOCKADDR_IN));
		}
		else
		{
			pItem->curSocketInfo.tcpSocketHandle = s;
		}
		pItem->curSocketInfo.socketType = socketType;
		m_pDataList->AddItem2DataTail(pItem);
}

UINT __stdcall SimpleDevice::_RecvDataProc_Select( void* pParam )
{
	SimpleDevice *pThis = (SimpleDevice*)pParam;
	pThis->RecvDataProc_Select();
	return 0;
}
void outString(LPCTSTR lpContent)
{
	OutputDebugString(_T("\r\n"));
	OutputDebugString(lpContent);
	OutputDebugString(_T("\r\n"));
}
void SimpleDevice::RecvDataProc_Select()
{

	fd_set fdread;
	fd_set fdWrite;
	timeval tv = {10,0};
	int nLoopi = 0;
	int nConnNum = 0;

#define RECVBUFFERSIZE 32768
	char * pRecvBuf = new char[RECVBUFFERSIZE];


	SOCKADDR_IN stClient;
	int nSize = sizeof(stClient);
	CString strLog;
	while(true)
	{
		FD_ZERO(&fdread);
		FD_SET(m_Socket_TCP,&fdread);

		//m_cCriticalSection.Lock();

		for(int i=0;i<m_aClientSocketList.GetSize();i++)
		{
			FD_SET(m_aClientSocketList.GetAt(i),&fdread);
		}
		//m_cCriticalSection.Unlock();

		int nRet = 0;
		nRet = select(0,&fdread,NULL,NULL,&tv);
		if(nRet==0)
		{
			Sleep(10);
			continue;
		}
		else if(nRet<0)
		{
			AfxMessageBox(_T("nret=0"));
			break;
		}
		int nListCnt = m_aClientSocketList.GetSize();
		int nTIme= GetTickCount();
		for(int i=0;i<nListCnt;i++)
		{
			SOCKET sRead = m_aClientSocketList.GetAt(i);

			if(FD_ISSET(sRead,&fdread))
			{
				strLog.Format(_T("FD_ISSET----time:%d,listcnt=%d,%x"),nTIme,nListCnt,sRead);
				outString(strLog);
				memset(pRecvBuf,0,RECVBUFFERSIZE);

				int nRecv = recv(sRead,pRecvBuf,RECVBUFFERSIZE,0);
				if(nRecv==0 || nRecv==SOCKET_ERROR)
				{
					strLog.Format(_T("recv error--time:%d listcnt=%d (sread=%x,"),nTIme,nListCnt,sRead);
					outString(strLog);
					WriteLog(LOGNAME,logLevelError,strLog);
					closesocket(sRead);
					FD_CLR(sRead,&fdread);

					m_aClientSocketList.RemoveAt(i);
					nListCnt--;
				}
				else
					PasreTcpCmd(1,sRead,pRecvBuf,nRecv,NULL);
			}
		}

		//
		if(FD_ISSET(m_Socket_TCP,&fdread))
		{
			SOCKET sClient = accept(m_Socket_TCP,(sockaddr*)&stClient,&nSize);
			if(sClient == WSAEWOULDBLOCK)
			{
				strLog.Format(_T("非阻塞模式设定 accept 调用不正确"));
				outString(strLog);
				WriteLog(LOGNAME,logLevelError,strLog);
				continue;
			}
			else if(sClient != INVALID_SOCKET)
			{
				if(m_aClientSocketList.GetSize()>=m_MAX_Socket_Connenct_Cnt)
				{
					strLog.Format(_T("服务器连接数已满 %d"),m_MAX_Socket_Connenct_Cnt);
					outString(strLog);
					WriteLog(LOGNAME,logLevelError,strLog);
					for(int i=0;i<m_aClientSocketList.GetSize();i++)
					{
						SOCKET sRead = m_aClientSocketList.GetAt(i);
						closesocket(sRead);
						FD_CLR(sRead,&fdread);
					}
					m_aClientSocketList.RemoveAll();
				}

				m_aClientSocketList.Add(sClient);
				CString ssIPA(inet_ntoa(stClient.sin_addr));
				strLog.Format(_T("accept %x_%s:%d--cnt:%d"),sClient,ssIPA,ntohs(stClient.sin_port),m_aClientSocketList.GetSize());
				outString(strLog);
				WriteLog(LOGNAME,logLevelInfo,strLog);
			}
		}



	}//while(true)


}

