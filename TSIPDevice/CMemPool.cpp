#include "stdafx.h"
#include "CMemPool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CMemPool::CMemPool()
{
	m_pDataBuffer = NULL;
	m_pMemBuffer = NULL;
	m_pUnitUsed = NULL;
	m_dwUnitSum = 0;
	m_dwUnitSize = 1;
	m_dwFreePos = 0;
	m_dwTotalSize = 0;
}

CMemPool::~CMemPool()
{
	if (m_pUnitUsed != NULL)
	{
		delete []m_pUnitUsed;
		m_pUnitUsed = NULL;
	}
	if (m_pMemBuffer != NULL)
	{
		delete []m_pMemBuffer;
		m_pMemBuffer = NULL;
	}
}

BOOL CMemPool::Initialize(int nTotalSize, int nUnitSize, int nBLLength)
{
	//先初始化一整块内存
	m_unStructBufSize = sizeof(MEM_POOL_BUFFERCELL);
	m_dwTotalSize = nTotalSize;//不包含前面的结构体长度
	m_dwAllFreeSize = m_dwTotalSize;
	
	int nBufsize = m_unStructBufSize * nBLLength + m_dwTotalSize;
	m_pMemBuffer = new char[nBufsize];
	if(m_pMemBuffer == NULL)
	{
		return FALSE;
	}
	ZeroMemory(m_pMemBuffer,nBufsize);
	
	char * pSturct = m_pMemBuffer;
	char * pData = m_pMemBuffer+m_unStructBufSize*nBLLength;

	//格式化结构体
	for (int n=0;n<nBLLength;n++)
	{
		//取数据结构
		MEM_POOL_BUFFERCELL* pListItem = (MEM_POOL_BUFFERCELL*)pSturct;
		//数据项赋值
		pListItem->pBuffer = NULL;
		pListItem->pExtBuffer = NULL;
		pListItem->dwBufferPos = 0;
		pListItem->dwUnitNum = 0;
		pSturct += m_unStructBufSize;

		m_BufferList.AddTail((CObject*)pListItem);
	}
	m_nItemCount = nBLLength;//数据块前面结构体的总数

	//将一整块内存分割成小块，提高利用率	
	m_pDataBuffer = pData;
	m_dwUnitSize = nUnitSize;//每个小块的大小
	if (nUnitSize == 0)
		return FALSE;

	m_dwUnitSum = m_dwTotalSize / m_dwUnitSize;
	m_pUnitUsed = new DWORD[m_dwUnitSum];
	memset(m_pUnitUsed, 0, sizeof(DWORD)*m_dwUnitSum);
	
	m_dwFreePos = 0;
	return TRUE;
}

MEM_POOL_BUFFERCELL* CMemPool::GetEmptyBuffer( DWORD dwBufSize, int iRole )
{
	CSingleLock singlelock(&m_DataLock,TRUE);
	CString strLog;

	MEM_POOL_BUFFERCELL* pBufferCell = NULL;
	if (!m_BufferList.IsEmpty())
	{
		pBufferCell = (MEM_POOL_BUFFERCELL*)m_BufferList.RemoveHead();
		if (pBufferCell != NULL)
		{
			if (!CheckPtr(pBufferCell))
			{
				//如果此内存块校验失败，这个链表节点丢掉				
				strLog.Format(_T("CheckPtr(pBufferCell) failed. iRole=%d,m_dwAllFreeSize=%d, m_dwTotalSize=%d, NeedSize=%d, m_dwFreePos=%d"),
					iRole,m_dwAllFreeSize, m_dwTotalSize, dwBufSize, m_dwFreePos);
				WriteLog(_T("CMemPool"), logLevelWarring, strLog);
				return NULL;
			}
		}
		else
		{
			strLog.Format(_T("iRole=%d,pBufferCell==NULL.m_dwAllFreeSize=%d, m_dwTotalSize=%d, NeedSize=%d, m_dwFreePos=%d"),
				iRole,m_dwAllFreeSize, m_dwTotalSize, dwBufSize, m_dwFreePos);
			WriteLog(_T("CMemPool"), logLevelWarring, strLog);
			return NULL;
		}
	}
	else
	{
		strLog.Format(_T("iRole=%d,m_BufferList.IsEmpty() .m_dwAllFreeSize=%d, m_dwTotalSize=%d, NeedSize=%d, m_dwFreePos=%d"),
			iRole,m_dwAllFreeSize, m_dwTotalSize, dwBufSize, m_dwFreePos);
		WriteLog(_T("CMemPool"), logLevelWarring, strLog);
		return NULL;
	}	

	//链表节点取出来成功，开始取空闲数据地址	
	BOOL bFind = FALSE;
	DWORD dwUnitNeed = dwBufSize / m_dwUnitSize;
	if (dwBufSize % m_dwUnitSize != 0)
		++dwUnitNeed;

	DWORD dwStart = m_dwFreePos;
	DWORD dwFree = 0;
	
	for (; dwStart < m_dwUnitSum; ++dwStart)
	{
		if (m_pUnitUsed[dwStart] == 0)
		{
			if (++dwFree >= dwUnitNeed)
			{
				bFind = TRUE;				
				break;
			}			
		}
		else
		{			
			dwFree = 0;
		}
	}

	if (!bFind)
	{
		dwFree = 0;
		//如果从上次使用位置没找到足够内存，就从内存池头部开始找空闲的
		for (dwStart = 0; dwStart < m_dwFreePos; ++dwStart)
		{
			if (m_pUnitUsed[dwStart] == 0)
			{
				if (++dwFree >= dwUnitNeed)
				{
					bFind = TRUE;
					break;
				}			
			}
			else
			{
				dwFree = 0;
			}
		}
	}

	if (bFind)
	{
		m_dwFreePos = dwStart+1;	//更新空闲位置，提高查询效率
		pBufferCell->dwBufferPos = dwStart - (dwUnitNeed-1);	//返回开始位置，便于回收
		pBufferCell->dwUnitNum = dwUnitNeed;	//返回内存单元数，便于回收
		for (DWORD i=0; i<dwUnitNeed; ++i)
		{
			m_pUnitUsed[pBufferCell->dwBufferPos + i] = 1;//置内存标志
		}
		
		pBufferCell->pBuffer = m_pDataBuffer + pBufferCell->dwBufferPos * m_dwUnitSize;	//返回指针
		m_dwAllFreeSize -= dwUnitNeed * m_dwUnitSize;
	}
	else//找遍整个内存池，也没找到连续的符合要求的内存，则动用扩展内存动态分配
	{
		if (pBufferCell->pExtBuffer != NULL)
		{
			delete []pBufferCell->pExtBuffer;
		}
		pBufferCell->pExtBuffer = new char[dwBufSize];
	}

	strLog.Format(_T("iRole=%d,m_dwAllFreeSize=%d, m_dwTotalSize=%d, NeedSize=%d, EmptyBufferList=%d, dwBufferPos=%d, dwUnitNum=%d, pBufferCell=%d"),
		iRole,m_dwAllFreeSize, m_dwTotalSize, dwBufSize, m_BufferList.GetCount(), pBufferCell->dwBufferPos, pBufferCell->dwUnitNum, pBufferCell);
	//WriteLog(_T("CMemPool"), _T("GetEmptyBuffer"), strLog);

	return pBufferCell;
}

BOOL CMemPool::ReleaseBuffer( MEM_POOL_BUFFERCELL* pBuffer ,int iRole)
{
	CSingleLock singlelock(&m_DataLock,TRUE);

	CString strLog;
	if (!CheckPtr(pBuffer))
	{
		strLog.Format(_T("###CheckPtr(pBufferCell) failed. iRole=%d,m_dwAllFreeSize=%d, m_dwTotalSize=%d, EmptyBufferList=%d,pBuffer=%d"),
			iRole,m_dwAllFreeSize, m_dwTotalSize, m_BufferList.GetCount(),pBuffer);
		WriteLog(_T("CMemPool"), logLevelWarring, strLog);
		return FALSE;
	}

	if (pBuffer->pExtBuffer == NULL)
	{
		DWORD nUnitNum = pBuffer->dwUnitNum;
		DWORD nStartPos = pBuffer->dwBufferPos;
		if (nStartPos < 0 || nStartPos >= m_dwUnitSum)
		{
			return FALSE;
		}
		if ((nStartPos + nUnitNum > m_dwUnitSum) || (nStartPos + nUnitNum < 0))
		{
			return FALSE;
		}
		for (DWORD i=0; i<nUnitNum; ++i)
		{
			m_pUnitUsed[nStartPos + i] = 0;//复位标志
		}
		pBuffer->pBuffer = NULL;		
		pBuffer->dwBufferPos = 0;
		pBuffer->dwUnitNum = 0;

		m_dwAllFreeSize += nUnitNum * m_dwUnitSize;
		
		strLog.Format(_T("###iRole=%d,m_dwAllFreeSize=%d, m_dwTotalSize=%d, HaveFreedSize=%d, EmptyBufferList=%d, dwBufferPos=%d, dwUnitNum=%d, pBuffer=%d"),
			iRole,m_dwAllFreeSize, m_dwTotalSize, nUnitNum * m_dwUnitSize, m_BufferList.GetCount(), nStartPos, nUnitNum, pBuffer);
	//	WriteLog(_T("CMemPool"), _T("ReleaseBuffer"), strLog);
	}
	else//如果扩展链表不为空，说明当时取的时候就没取到内存池的，是单独new的
	{
		delete []pBuffer->pExtBuffer;
		pBuffer->pExtBuffer = NULL;
	}

	m_BufferList.AddTail((CObject*)pBuffer);
	return TRUE;
}

void CMemPool::ClearAllDataItem()
{
	memset(m_pUnitUsed, 0, sizeof(DWORD)*m_dwUnitSum);
	m_dwFreePos = 0;
	m_dwAllFreeSize = m_dwTotalSize;
}

BOOL CMemPool::CheckPtr(MEM_POOL_BUFFERCELL* pListItem)
{
	CString strLog;
	//检查指针是否合法
	UINT nDistance = (char*)pListItem - m_pMemBuffer;
	if((nDistance % m_unStructBufSize) != 0)
	{			
		strLog.Format(_T("m_dwAllFreeSize=%d, m_dwTotalSize=%d, EmptyBufferList=%d, nDistance=%d, m_unStructBufSize=%d"),
			m_dwAllFreeSize, m_dwTotalSize,m_BufferList.GetCount(), nDistance, m_unStructBufSize);
		WriteLog(_T("CMemPool"), logLevelWarring, strLog);
		return FALSE;
	}

	UINT nIndex = nDistance/m_unStructBufSize;
	if(nIndex >= m_nItemCount)
	{
		strLog.Format(_T("m_dwAllFreeSize=%d, m_dwTotalSize=%d, EmptyBufferList=%d, nDistance=%d, m_unStructBufSize=%d, nIndex=%d"),
			m_dwAllFreeSize, m_dwTotalSize,m_BufferList.GetCount(), nDistance, m_unStructBufSize, nIndex);
		WriteLog(_T("CMemPool"), logLevelWarring, strLog);
		return FALSE;
	}

	if ((char*)pListItem!=m_pMemBuffer+nIndex*m_unStructBufSize)
	{
		strLog.Format(_T("m_dwAllFreeSize=%d, m_dwTotalSize=%d, EmptyBufferList=%d, nDistance=%d, m_unStructBufSize=%d, nIndex=%d, pListItem=%d"),
			m_dwAllFreeSize, m_dwTotalSize,m_BufferList.GetCount(), nDistance, m_unStructBufSize, nIndex, pListItem);
		WriteLog(_T("CMemPool"),logLevelWarring, strLog);
		return FALSE;
	}

	return TRUE;
}