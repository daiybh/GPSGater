#pragma once
#include "CMemPool.h"
typedef void (CALLBACK *RECV_CALLBACKFUNC)(DWORD dwStatus/*状态*/,BYTE *pClient, BYTE* pBuf, DWORD dwSize, LPVOID pUserData);

class GPS_Iocp
{
public:
	GPS_Iocp(void);
	~GPS_Iocp(void);
	int InitializeSocket();
	int				Listen(char* ulIP, unsigned short usPort);
	int				StartWork(char* ulIP, unsigned short usPort,RECV_CALLBACKFUNC pFunc,LPVOID pUserData);
	int				SendData(BYTE *pOldClient,BYTE *pClient,const BYTE*pBuf,DWORD dwBufLen);
private:
	void					ClearResources(BOOL bCloseListenEvent);
	static	DWORD WINAPI	_AcceptThread(void *pParam);//接受客户端请求线程
	static	DWORD WINAPI	_ServiceThread(void *pParam);//服务线程
	static 	DWORD WINAPI	_DoDetectThread(void *pParam);
	HANDLE					m_hDoDetect;
private:
	RECV_CALLBACKFUNC		m_pRecvCallBackFunc;
	LPVOID				m_pRecvCBUserData;				// 回调线程，释放线程，两个线程使用。！！！需要保护，使用m_RecvLock锁保护

	SOCKET					m_sListen;					//监听套接字
	DWORD					m_dwMemPoolSize;			//内存池大小
	BOOL					m_bRunning;
	HANDLE					m_hListenEvent;				//监听事件句柄
	HANDLE					m_hCompPort;				//完成端口句柄	
	HANDLE					m_hThread[35];				//子线程=监听线程+服务线程；先估计最多运行在一个16核的机器上，那线程数=16*2+2+1，不会溢出
	int						m_nThreadNum;				//实际线程数量
	CMemPool*				m_pMemPool;					//内存池
	unsigned long			m_ulLocalIP;
	unsigned short			m_usLocalPort;
};
