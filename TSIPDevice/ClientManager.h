#if !defined(AFX_CLIENTMANAGER_H__688E1208_4C99_4411_938C_D8B547910E87__INCLUDED_)
#define AFX_CLIENTMANAGER_H__688E1208_4C99_4411_938C_D8B547910E87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "ClientContext.h"
using namespace std;

typedef struct tagThreadData
{
	SOCKET			socket;
	SOCKADDR_IN		addrClient;	
	CClientContext* pClient;
} _ClientData;

typedef map<DWORD, _ClientData, less<DWORD>, allocator<_ClientData> > ClientMap;


//单件模式
class CClientManager  
{
public:
	virtual ~CClientManager();
public:
	static CClientManager *GetClientManager(void);	//得到管理客户端对象指针
	static void		ReleaseManager(void);				//释放管理客户端对象占有资源
	void			ProcessIO(byte type, CClientContext* pClient, DWORD dwIOSize);	//I/O处理
	void			AddClient(_ClientData clientdata);		//增加客户端
	void			DeleteClient(CClientContext *pClient, BOOL bDelete);	//删除客户端
	void			DeleteAllClient(void);					//删除所有客户端
	BOOL			FindClientAndLock(CClientContext *pClient);
	void			LockClient(CClientContext *pClient);
	void			UnlockClient(CClientContext *pClient);
	DWORD			GetClientCount(){return m_dwClientCount;}
	__int64			GetAllClientCount(){return m_i64dAllClientCount;}
	DWORD			DetectLiveTime();
private:
	ClientMap		m_clientMap;							//管理客户端映射，对应一个唯一的CommID
	CClientManager();
	CClientManager(const CClientManager& other);
	CClientManager& operator = (CClientManager &other);
	static CClientManager *m_pClientMgr;
	CCriticalSection m_csClientMapLock;							//保护客户端链表对象
	DWORD			m_dwClientCount;
	__int64			m_i64dAllClientCount;
};

#endif // !defined(AFX_CLIENTMANAGER_H__688E1208_4C99_4411_938C_D8B547910E87__INCLUDED_)
