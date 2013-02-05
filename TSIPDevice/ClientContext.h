#if !defined(AFX_CLIENTCONTEXT_H__DE6218FD_8A39_46C1_A9D2_EB6BC7082509__INCLUDED_)
#define AFX_CLIENTCONTEXT_H__DE6218FD_8A39_46C1_A9D2_EB6BC7082509__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "gps_iocp.h"

//call back 回调函数状态码定义（DWORD dwStatus)
#define		STATUS_OK				0XF0000001	//正常
#define		STATUS_REMOTECLOSED		0XF0000010	//对方断开或关闭
#define		STATUS_RECEIVEDATAERROR	0XF0000020	//接收到数据时没发现包头，回调错误.

enum NetworkType
{
	NetworkType_TCP = 1000,
	NetworkType_WCF = 1001,
	NetworkType_UDP = 1002,
};
#define		DATALEN				1000000							//缓冲区长度:1M
#define		OVERLAPPEDPLUSLEN	(sizeof(IO_OPERATION_DATA))			//扩展重叠结构长度

typedef void (CALLBACK *RECV_CALLBACKFUNC)(DWORD dwStatus/*状态*/,BYTE *pClient, BYTE* pBuf, DWORD dwSize, LPVOID pUserData);
typedef struct _tag_Packet_Head
{
	DWORD dwTcpHead;                // eg: 0x00aa0155
	DWORD dwPacketSize;            //Size of Packet
}_Packet_Head;

//I/O操作类型
#define		IOReadHead			9							//接收
#define		IOReadBody			10							//接收
#define		IOWrite				11							//发送
#define		IORead_GPS_Body		12

#define TCP_HEAD	0xF0a0B155
#define HEADSIZE	sizeof(_Packet_Head)
#define GPS_DATA_SIZE 1024

// I/O 操作数据结构
typedef	struct _io_operation_data 
{
	OVERLAPPED	overlapped;					//重叠结构
	//	char		recvBuf[DATALEN];			//接收数据缓冲区
	WSABUF      databuf;					//WSABUF 数据结构
	byte		type;						//操作类型
}IO_OPERATION_DATA, *PIO_OPERATION_DATA;


class CClientContext //: public INetworkConnector
{
public:
	CClientContext(SOCKET s, GPS_Iocp* pGTMIOCP, CMemPool* pMemPool);
	virtual ~CClientContext();
public:
	virtual NetworkType GetType();
	virtual	int			Connect(unsigned long ulIP, unsigned short usPort);
	virtual BOOL		SetReceiveCallBack(RECV_CALLBACKFUNC pFunc, LPVOID pUserData);
	virtual int SendData(const BYTE* pBuf, DWORD dwSize);
	virtual int			Startwork();	
	virtual	int			Stopwork();
	virtual int			GetConnectionInfo(OUT unsigned long &ulLocalIP,
										OUT unsigned short &usLocalPort,
										OUT unsigned long &ulRemoteIP,
										OUT unsigned short &usRemotePort);
	void				LockClient(void);
	void				UnlockClient(void);
public:
	int					SyncSendData(BYTE* pBuf, DWORD dwSize);//发送数据
	int AsynSendData(const BYTE* pBuf, DWORD dwSize);//发送数据
	void				OnSendCompleted(DWORD dwIOSize);		//发送数据完毕	

	BOOL				AsyncRecvData();
	void				OnRecvDataCompleted( DWORD dwIOSize );	//接收包体完毕	

	void				SetCloseCB(DWORD dwCode);
	void				SetConnectionInfo(unsigned long ulLocalIP,
										unsigned short usLocalPort,
										unsigned long ulRemoteIP,
										unsigned short usRemotePort);


public:
	BOOL				m_bOk;
	SOCKET				m_s;				// 发送线程，释放线程，两个线程使用。！！！需要保护，使用m_SendLock锁保护
private:
	IO_OPERATION_DATA	m_iIO;			// 读扩展重叠结构数据结构
	IO_OPERATION_DATA	m_oIO;			// 读扩展重叠结构数据结构
	CCriticalSection	m_SendLock;		// 发送数据锁
	CCriticalSection	m_RecvLock;		// 接受数据锁
	unsigned long		m_ulLocalIP;
	unsigned short		m_usLocalPort;
	unsigned long		m_ulRemoteIP;
	unsigned short		m_usRemotePort;


	GPS_Iocp*			m_pGTMIOCP;			// 主窗口指针， 不需要保护
	char*				m_pRecvBuffer;		// 临时存储指针，由于是单线程，所以不需要保护
	DWORD				m_dwPacketSize;		// 由于是单线程，所以不需要保护

	DWORD				m_dwReceivedSize;	// 单线程操作，不需要保护
	CMemPool*			m_pMemPool;			// 单线程操作，不需要保护
	RECV_CALLBACKFUNC	m_pRecvCBFunc;		// 回调线程，释放线程，两个线程使用。！！！需要保护，使用m_RecvLock锁保护
	LPVOID				m_pRecvCBUserData;	// 回调线程，释放线程，两个线程使用。！！！需要保护，使用m_RecvLock锁保护
	MEM_POOL_BUFFERCELL*			m_pRecvBufferCell;	// 接受数据线程，释放线程，两个线程使用。！！！需要保护，使用m_RecvLock锁保护
	char				m_cRecvHeader[HEADSIZE];
	char				m_cRecvDataBuffer[GPS_DATA_SIZE];
	__int64				m_i64RecvCnt;
};

#endif // !defined(AFX_CLIENTCONTEXT_H__DE6218FD_8A39_46C1_A9D2_EB6BC7082509__INCLUDED_)
