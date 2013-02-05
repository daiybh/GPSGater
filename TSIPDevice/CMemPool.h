#pragma once

typedef struct _st_Mem_POOL_BUFFERCELL
{
	char*			pBuffer;
	char*			pExtBuffer;
	DWORD			dwBufferPos;
	DWORD			dwUnitNum;

	_st_Mem_POOL_BUFFERCELL()
	{
		pBuffer = NULL;
		pExtBuffer = NULL;
		dwBufferPos = 0;
		dwUnitNum = 0;
	}
}MEM_POOL_BUFFERCELL;

#include <afxmt.h>
class CMemPool  
{
public:
	CMemPool();
	virtual ~CMemPool();
public:
	BOOL		Initialize(int nTotalSize, int nUnitSize, int nBLLength);
	MEM_POOL_BUFFERCELL* GetEmptyBuffer(DWORD dwBufSize, int iRole);
	BOOL		ReleaseBuffer(MEM_POOL_BUFFERCELL* pBuffer, int iRole);
	void		ClearAllDataItem();
private:
	char*	m_pDataBuffer;
	DWORD*	m_pUnitUsed;//0:未使用		1：正在使用
	DWORD	m_dwUnitSum;
	DWORD	m_dwUnitSize;
	DWORD	m_dwTotalSize;
	DWORD	m_dwFreePos;
	DWORD	m_dwAllFreeSize; //内存池当前可用的大小
private:
	UINT m_unStructBufSize;//每个结构占用的mem大小	
	char* m_pMemBuffer;
	UINT m_nItemCount;
	CCriticalSection m_DataLock;
	CObList m_BufferList;
private:
	BOOL CheckPtr(MEM_POOL_BUFFERCELL* pListItem);
};


