#include "StdAfx.h"
#include "GPS_Iocp.h"
#include "ClientContext.h"
#include "ClientManager.h"

#define  MAX_IOCP_THREAD	(35)
GPS_Iocp::GPS_Iocp(void)
{
}

GPS_Iocp::~GPS_Iocp(void)
{
}

int GPS_Iocp::InitializeSocket()
{
	CString strLog;
	WSADATA wsaData;	
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{				
		strLog.Format(_T("WSAStartup() Failed. Err=%d. GTMIOCP::InitliazeSocket()"), WSAGetLastError());
		WriteLog(SERVERLOGNAME, logLevelWarring, strLog);
		return FALSE;
	}	

	WriteLog(SERVERLOGNAME, logLevelInfo, _T("InitliazeSocket() Successed. GTMIOCP::InitliazeSocket()"));
	OutputDebugString( _T("InitliazeSocket() Successed. GTMIOCP::InitliazeSocket()"));
	return TRUE;
}

int GPS_Iocp::Listen( char* ulIP, unsigned short usPort )
{
	int iErrcode = 0;
	CString strLog;

	//创建套接字
	if ((m_sListen = WSASocket(AF_INET,
		SOCK_STREAM,
		0,
		NULL, 
		0,
		WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{		
		strLog.Format( _T("WSASocket() Failed.Err=%d. GTMIOCP::InitliazeSocket()"), WSAGetLastError());
		WriteLog(SERVERLOGNAME, logLevelWarring, strLog);
		return FALSE;
	} 

	//绑定套接字
	SOCKADDR_IN	servAddr;//服务器地址
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;//htonl(ulIP);
	servAddr.sin_port = htons(usPort);	

	CString strIP;
	//strIP.Format(_T("%s"), A2W(inet_ntoa(servAddr.sin_addr)));
	//strIP.Format(_T("%s"),A2W(ulIP));

	if (bind(m_sListen, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		iErrcode = WSAGetLastError();

		strLog.Format(_T("bind() Failed.Err=%d.IP=%s.Port=%d. GTMIOCP::Listen"), 
			iErrcode, strIP, usPort);
		WriteLog(SERVERLOGNAME, logLevelWarring, strLog);

		ClearResources(FALSE);
		return iErrcode;
	}
	//监听
	if(listen(m_sListen, SOMAXCONN) == SOCKET_ERROR)
	{
		iErrcode = WSAGetLastError();

		strLog.Format(_T("listen() Failed.Err=%d.IP=%s.Port=%d. GTMIOCP::Listen"),
			iErrcode,strIP, usPort);
		WriteLog(SERVERLOGNAME,logLevelWarring, strLog);

		ClearResources(FALSE);
		return iErrcode;
	}
	struct sockaddr_in sin;
	int len = sizeof(sin);
	if(getsockname(m_sListen, (struct sockaddr *)&sin, &len) != 0)
	{
		printf("getsockname() error:%s\n", strerror(errno));
		ClearResources(FALSE);
		return WSAGetLastError();
	}
	DWORD dwPort = ntohs(sin.sin_port);


	m_bRunning = TRUE;

	//创建接受客户端连接事件对象
	m_hListenEvent = WSACreateEvent();
	if ( m_hListenEvent == WSA_INVALID_EVENT )
	{	
		iErrcode = WSAGetLastError();

		strLog.Format(_T("WSACreateEvent() Failed.Err=%d.IP=%s.Port=%d. GTMIOCP::Listen"),
			iErrcode, strIP, usPort);
		WriteLog(SERVERLOGNAME, logLevelWarring, strLog);

		ClearResources(FALSE);
		return iErrcode;
	}

	//为监听套接字注册FD_ACCEPT网络事件	
	if ( WSAEventSelect(m_sListen, m_hListenEvent, FD_ACCEPT) == SOCKET_ERROR )
	{
		iErrcode = WSAGetLastError();

		strLog.Format(_T("WSAEventSelect() Failed.Err=%d.IP=%s.Port=%d. GTMIOCP::Listen"),
			iErrcode, strIP, usPort);
		WriteLog(SERVERLOGNAME, logLevelWarring, strLog);

		ClearResources(TRUE);
		return iErrcode;
	}

	// 创建完成端口
	if ((m_hCompPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
	{
		iErrcode = WSAGetLastError();

		strLog.Format(_T("CreateIoCompletionPort() Failed.Err=%d.IP=%s.Port=%d. GTMIOCP::Listen"),
			iErrcode, strIP, usPort);
		WriteLog(SERVERLOGNAME, logLevelWarring, strLog);

		ClearResources(TRUE);
		return iErrcode;
	}

	//创建内存池
	m_pMemPool = new CMemPool;
	m_dwMemPoolSize = 1024*10;
	if(!m_pMemPool->Initialize(m_dwMemPoolSize, 500, 200))
	{
		return 1;
	}

//	m_ulLocalIP = ulIP;
	m_usLocalPort = usPort;

	//创建接受客户端请求线程
	DWORD dwThreadID = 0;	
	m_hThread[0] = CreateThread(NULL, 
		0,
		_AcceptThread,
		this, 
		0,  
		&dwThreadID);
	if (NULL == m_hThread)
	{
		iErrcode = GetLastError();

		strLog.Format(_T("CreateThread(_AcceptThread) Failed.Err=%d.IP=%s.Port=%d. GTMIOCP::Listen"),
			iErrcode, strIP, usPort);
		WriteLog(SERVERLOGNAME, logLevelWarring, strLog);

		ClearResources(TRUE);
		return iErrcode;
	}
	m_nThreadNum = 1;

	// 获取CPU数量
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	DWORD dwThreadSum = SystemInfo.dwNumberOfProcessors * 2 + 2;//经验值
    if(dwThreadSum > MAX_IOCP_THREAD)
	{
		dwThreadSum = MAX_IOCP_THREAD;
	}

	strLog.Format(_T("CreateThread Count(%d),Current Processors number(%d) GTMIOCP::Listen."),dwThreadSum,SystemInfo.dwNumberOfProcessors);
	WriteLog(SERVERLOGNAME, logLevelInfo, strLog);

	// 创建服务线程
	for(DWORD i = m_nThreadNum; i < dwThreadSum; i++)
	{
		if ((m_hThread[m_nThreadNum++] = CreateThread(NULL, 
			0,
			_ServiceThread,
			this,
			0, 
			&dwThreadID)) == NULL)
		{
			iErrcode = GetLastError();

			strLog.Format(_T("CreateThread(_ServiceThread) Failed.Err=%d.IP=%s.Port=%d.ThreadNum=%d. GTMIOCP::Listen"),
				iErrcode, strIP, usPort, i);
			WriteLog(SERVERLOGNAME, logLevelWarring, strLog);

			ClearResources(TRUE);
			return iErrcode;
		}		
	}

	m_hDoDetect = CreateEvent(0,0,0,"dodetect");
	CreateThread(NULL,
		0,
		_DoDetectThread,
		this,
		0,
		&dwThreadID);
	strLog.Format(_T("GTMCreateServer() Successed. _ServiceThread dwThreadSum=%d. IP=%s.Port=%d. GTMIOCP::Listen"),
		dwThreadSum,strIP, usPort);
	WriteLog(SERVERLOGNAME,logLevelInfo, strLog);

	return 0;

}


void GPS_Iocp::ClearResources(BOOL bCloseListenEvent)
{
	if (bCloseListenEvent)
	{
		if (m_hListenEvent != WSA_INVALID_EVENT)
		{
			WSACloseEvent(m_hListenEvent);	//关闭事件
			m_hListenEvent = WSA_INVALID_EVENT;
		}		
	}

	if (m_sListen != INVALID_SOCKET)
	{
		closesocket(m_sListen);
		m_sListen = INVALID_SOCKET;
	}	
}
DWORD WINAPI GPS_Iocp::_AcceptThread( void *pParam )
{	
	GPS_Iocp		*pGTMIOCP = (GPS_Iocp*)pParam;		//主窗口指针
	HANDLE		hComPort = pGTMIOCP->m_hCompPort;	//完成端口
	SOCKET		sListen = pGTMIOCP->m_sListen;		//监听套接字
	SOCKET		sAccept = INVALID_SOCKET;			//接受套接字
	int			iErrorCode;
	CString		strLog;
	CString		strIP;
	TCHAR		wcIP[MAX_PATH];
	DWORD		dwAcceptID=0;

	while(pGTMIOCP->m_bRunning)
	{
		DWORD dwRet;
		dwRet = WSAWaitForMultipleEvents(1,			//等待网络事件
			&pGTMIOCP->m_hListenEvent,
			FALSE,
			100,
			FALSE);		
		if(!pGTMIOCP->m_bRunning)					//服务器停止服务
			break;

		if (dwRet == WSA_WAIT_TIMEOUT)				//函数调用超时
			continue;

		WSANETWORKEVENTS events;					//查看发生的网络事件
		int nRet = WSAEnumNetworkEvents(pGTMIOCP->m_sListen,
			pGTMIOCP->m_hListenEvent,//事件对象被重置
			&events);		
		if (nRet == SOCKET_ERROR)
		{		
			iErrorCode = WSAGetLastError();

			strLog.Format(_T("WSAEnumNetworkEvents() failed. Err=%d."),iErrorCode);
			WriteLog(SERVERLOGNAME,logLevelWarring, strLog);

			break;
		}

		if ( events.lNetworkEvents & FD_ACCEPT)		//发生FD_ACCEPT网络事件
		{
			if ( events.iErrorCode[FD_ACCEPT_BIT] == 0 && pGTMIOCP->m_bRunning)
			{
				//接受客户端请求
				SOCKADDR_IN servAddr;
				int	serAddrLen = sizeof(servAddr);	   
				if ((sAccept = WSAAccept(sListen, 
					(SOCKADDR*)&servAddr,
					&serAddrLen,
					NULL, 
					0)) == SOCKET_ERROR)
				{					
					iErrorCode = WSAGetLastError();

					strLog.Format(_T("WSAAccept() failed. Err=%d."),iErrorCode);
					WriteLog(SERVERLOGNAME, logLevelWarring, strLog);

					break;
				}

				char *pIP = inet_ntoa(servAddr.sin_addr);				
#ifdef _UNICODE
				size_t convertedChars = 0;
				mbstowcs_s(&convertedChars, wcIP,  (strlen(pIP) + 1), pIP, _TRUNCATE);
				strIP.Format(_T("%s"), wcIP);				 
#else
				strIP.Format(_T("%s"), pIP);
#endif

				//创建客户端节点
				CClientContext *pClient = new CClientContext(dwAcceptID++,sAccept,pGTMIOCP, pGTMIOCP->m_pMemPool);

				//strLog.Format(_T("WSAAccept() one client. IP=%s.Port=%d. client=%x"),strIP, ntohs(servAddr.sin_port),pClient);
				//WriteLog(SERVERLOGNAME, logLevelInfo, strLog);
				OutputDebugString(strLog);
				
				pClient->SetConnectionInfo(pGTMIOCP->m_ulLocalIP, pGTMIOCP->m_usLocalPort, ntohl(servAddr.sin_addr.s_addr), ntohs(servAddr.sin_port));
				pClient->SetReceiveCallBack(pGTMIOCP->m_pRecvCallBackFunc,pGTMIOCP->m_pRecvCBUserData);

				if (CreateIoCompletionPort((HANDLE)sAccept,	//套接字与完成端口关联起来
					hComPort,
					(DWORD) pClient,//完成键
					0) == NULL)
				{
					iErrorCode = GetLastError();

					strLog.Format(_T("CreateIoCompletionPort() failed. Err=%d.IP=%s.Port=%d."),
						iErrorCode, strIP, servAddr.sin_port);
					WriteLog(SERVERLOGNAME,logLevelWarring, strLog);

					return -1;
				}

				_ClientData clientdata;
				clientdata.socket = sAccept;
				clientdata.addrClient = servAddr;				
				clientdata.pClient = pClient;

		//		WSARecv(sAccept,
//					)
				//加入管理客户端链表
				CClientManager *pClientMgr = CClientManager::GetClientManager();
				pClientMgr->AddClient(clientdata);


				//回调给上层，接收到一个连接
				pClient->AsyncRecvData();
				//pGTMIOCP->m_pConnectCBFunc(STATUS_OK, pClient, pGTMIOCP->m_pConnectCBUserData);								
			}
		}		
	}

	//退出线程，释放资源
	WriteLog(SERVERLOGNAME, logLevelInfo, _T("Exit GTMIOCP::_AcceptThread()."));
	return 0;
}
DWORD WINAPI GPS_Iocp::_DoDetectThread(void *pParam){

	GPS_Iocp *pGTMIOCP = (GPS_Iocp*)pParam;//主窗口指针
	CString	strLog;

	DWORD dwClientCount=0;
	DWORD dwStartTime = GetTickCount();
	__int64 i64d_AllDeleteCount =0;
	while(1){
		WaitForSingleObject(pGTMIOCP->m_hDoDetect,30*1000);
		dwClientCount = CClientManager::GetClientManager()->GetClientCount();
		DWORD dwDeleteCount = CClientManager::GetClientManager()->DetectLiveTime();
		i64d_AllDeleteCount+=dwDeleteCount;
			printf("dectected[%d]--del[ALL:%I64d,%d]---%d--all:%I64d\n",GetTickCount()-dwStartTime,i64d_AllDeleteCount,dwDeleteCount,dwClientCount,CClientManager::GetClientManager()->GetAllClientCount());
	}
	return 1;
}
DWORD WINAPI GPS_Iocp::_ServiceThread( void *pParam )
{
	GPS_Iocp *pGTMIOCP = (GPS_Iocp*)pParam;//主窗口指针
	HANDLE	hComPort = pGTMIOCP->m_hCompPort;//完成端口
	CString	strLog;

	DWORD			dwIoSize;		//传输字节数
	CClientContext	*pClient;		//客户端指针
	LPOVERLAPPED	lpOverlapped;	//重叠结构指针
	bool			bExit = FALSE;	//服务线程退出
	DWORD			dNow = GetTickCount();
	while (!bExit)
	{
		dwIoSize = -1;
		lpOverlapped = NULL;
		pClient = NULL;
		//等待I/O操作结果
		BOOL bIORet = GetQueuedCompletionStatus(hComPort,
			&dwIoSize,
			(LPDWORD) &pClient,
			&lpOverlapped,
			INFINITE);

		//失败的操作完成			
		if ((FALSE == bIORet && NULL != pClient) || dwIoSize == 0)
		{		
			//需要删除连接
			//printf("_ServiceThread ioret=%d iosize=%d--gtmiocp:%x--pClient:%x\n",bIORet,dwIoSize,pGTMIOCP,pClient);

			CClientManager *pClientMgr = CClientManager::GetClientManager();

			strLog.Format(_T("XXXServiceThread recv client=%x bok=%d,iosize=%d ioret=%d count:%d\n"),pClient,pClient->m_bOk,dwIoSize,bIORet,pClientMgr->GetClientCount());
			OutputDebugString(strLog);
			if (pClient != NULL && pClientMgr->FindClientAndLock(pClient))
			{				
				int iErr = GetLastError();
				//客户端断开,回调上层出错，上层负责delete pClient				
				//strLog.Format(_T("Client break off. Delete from ClientMap. SOCKET=%d. Err=%d. dwIoSize=%d. ThreadID=%d"),
				//	pClient->m_s, iErr, dwIoSize, GetCurrentThreadId());
				//WriteLog(SERVERLOGNAME, logLevelWarring, strLog);
				//printf(strLog);
				pClient->m_bOk = FALSE;
				pClient->SetCloseCB(STATUS_REMOTECLOSED);	
				pClientMgr->UnlockClient(pClient);
				pClientMgr->DeleteClient(pClient,TRUE);
			}
			pClient = NULL;
		}
		//成功的操作完成
		if(bIORet && lpOverlapped && pClient) 
		{				
			//获取扩展重叠结构指针
			PIO_OPERATION_DATA pIO = CONTAINING_RECORD(lpOverlapped, IO_OPERATION_DATA,overlapped);

			CClientManager *pClientMgr = CClientManager::GetClientManager();
			BOOL bRtn = pClientMgr->FindClientAndLock(pClient);
			//pClientMgr->UnlockClient(pClient);

			if (bRtn)
			{
				pClientMgr->ProcessIO(pIO->type, pClient, dwIoSize);	
				pClientMgr->UnlockClient(pClient);
			}
			else
			{
				strLog.Format(_T("Client(%d) have been deleted when receiving. ThreadID=%d"),
					pClient, GetCurrentThreadId());
				WriteLog(SERVERLOGNAME, logLevelWarring, strLog);

				pClient = NULL;
			}
		}	
		//服务器退出，析构里主动投递pClient == NULL和lpOverlapped == NULL的请求
		if(pClient == NULL&& lpOverlapped == NULL && !pGTMIOCP->m_bRunning)
		{			
			strLog.Format(_T("Server shutdown. Exit thread(%d). GTMIOCP::_ServiceThread"), GetCurrentThreadId());
			WriteLog(SERVERLOGNAME, logLevelWarring, strLog);

			bExit = TRUE;
		}		
	}
	return 0;
}

int GPS_Iocp::StartWork( char* ulIP, unsigned short usPort,RECV_CALLBACKFUNC pFunc,LPVOID pUserData )
{
	printf("\nstartwork--port=%d ip=%s\n",usPort,ulIP);
	m_pRecvCallBackFunc = NULL;
	m_pRecvCBUserData = NULL;
	int nRet = -1;
	nRet = InitializeSocket();
	if(nRet!=TRUE)
		return FALSE;
	nRet = Listen(ulIP,usPort);
	if(nRet ==0)
	{
		m_pRecvCallBackFunc = pFunc;
		m_pRecvCBUserData	= pUserData;
		return TRUE;
	}
	return FALSE;
}

int GPS_Iocp::SendData(BYTE *pOldClient, BYTE *pClient,const BYTE*pBuf,DWORD dwBufLen )
{
	CClientManager *pClientMgr = CClientManager::GetClientManager();
	if(pOldClient!=NULL &&pOldClient !=pClient)
	{
		//删除OldClient 
		CClientContext *pOldClientContext = (CClientContext *) pOldClient;
		//pClientMgr->DeleteClient(pOldClientContext,TRUE);

	}
	//不用删除OldClient
	CClientContext *pClientContext = (CClientContext *) pClient;
	BOOL bRtn = pClientMgr->FindClientAndLock(pClientContext);
	int nRet = 1;
	if(bRtn)
	{
		nRet = pClientContext->SendData(pBuf,dwBufLen);
		pClientMgr->UnlockClient(pClientContext);
	}
	return (nRet==0)?1:(-nRet);
}
