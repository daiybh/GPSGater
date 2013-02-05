#include "stdafx.h"
#include "ClientManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CClientManager* CClientManager::m_pClientMgr = NULL;

CClientManager::CClientManager()
{
	m_pClientMgr = NULL; 
}

CClientManager::~CClientManager()
{
	if (!m_clientMap.empty())
	{
		DeleteAllClient();
	}	
}
/* 
 * 得到客户端管理对象指针
 */
CClientManager * CClientManager::GetClientManager( void )
{
	return (m_pClientMgr == NULL)? m_pClientMgr = new CClientManager:m_pClientMgr;
}
/* 
 * 增加客户端
 */
void CClientManager::AddClient( _ClientData clientdata )
{	
	CSingleLock slock(&m_csClientMapLock, TRUE);
	m_clientMap.insert(ClientMap::value_type((DWORD)clientdata.socket, clientdata));
}

/* 
 * 删除客户端
 */
void CClientManager::DeleteClient( CClientContext *pClient, BOOL bDelete )
{
	CSingleLock slock(&m_csClientMapLock, TRUE);

	ClientMap::iterator iter;
	_ClientData ClientData;

	// walk through the events and threads and close them all
	for(iter = m_clientMap.begin(); iter != m_clientMap.end(); iter++)
	{
		ClientData = (*iter).second;
		if (ClientData.pClient == pClient)
		{
			if (bDelete)
			{
				delete pClient;
				ClientData.pClient = NULL;
			}	
			m_clientMap.erase(iter);
			break;
		}		
 	}
	
}

/* 
 * 删除所有客户端
 */
void CClientManager::DeleteAllClient( void )
{
	CSingleLock slock(&m_csClientMapLock, TRUE);	
	//删除所有客户端链表
// 	ClientMap::iterator iter;
// 	_ClientData ClientData;
// 
// 	for(iter = m_clientMap.begin(); iter != m_clientMap.end(); iter++)
// 	{
// 		ClientData = (*iter).second;		
// 		delete ClientData.pClient;
// 		ClientData.pClient = NULL;				
// 	}	

	//不删除管理起来的所有连接对象，由使用者自己释放
	m_clientMap.clear();
}

/* 
 * 释放内存
 */
void CClientManager::ReleaseManager( void )
{
	if (NULL != m_pClientMgr)
	{
		delete m_pClientMgr;
		m_pClientMgr = NULL;
	}	
}

/* 
 * I/O处理
 */
void CClientManager::ProcessIO( byte type, CClientContext* pClient, DWORD dwIOSize )
{
	switch(type)//重叠操作类型
	{
	case IOReadHead:
		{
	//		pClient->OnRecvHeadCompleted(dwIOSize);
			break;
		}
	case IOReadBody://读数据
		{
		//	pClient->OnRecvBodyCompleted(dwIOSize);
			break;				
		}
	case IOWrite:
		{
			pClient->OnSendCompleted(dwIOSize);
			break;
		}
	case IORead_GPS_Body:
		{
			pClient->OnRecvDataCompleted(dwIOSize);
			break;
		}
	default:break;
	}		
}

BOOL CClientManager::FindClientAndLock(CClientContext *pClient)
{
	CSingleLock slock(&m_csClientMapLock, TRUE);

	ClientMap::iterator iter;
	_ClientData ClientData;
	
	for(iter = m_clientMap.begin(); iter != m_clientMap.end(); iter++)
	{
		ClientData = (*iter).second;
		if (ClientData.pClient == pClient)
		{
			if(!pClient->m_bOk) return FALSE;
			LockClient(pClient);
			return TRUE;
		}		
	}
	return FALSE;
}

void CClientManager::LockClient(CClientContext *pClient)
{
// 	CString strLog;
// 	strLog.Format(_T("CClientManager::LockClient %x"),pClient);
// 	OutputDebugString(strLog);
	pClient->LockClient();
}

void CClientManager::UnlockClient(CClientContext *pClient)
{
// 	CString strLog;
// 	strLog.Format(_T("CClientManager::UnlockClient %x"),pClient);
// 	OutputDebugString(strLog);
	pClient->UnlockClient();
}