   /***********************************
 Copyright?,2001－2005, SOBEY CO,.LTD
 File name: ESBufferMan.h
 Author: zyy
 Version: 1.0
 DateTime: 
 Description: 
 Others: 
 Class and Interface List: 
 History: 
 1 Date: 
   Author: 
   Modification:修改 ClearAllDataItem方法
 2 Date: 
   Author: 
   Modification:改AddItem2EmptyTail，实际加到链表头，减少页面错误
 3 Date:
   Author: 
   Modification:
   

  说明： 用文件映射的方式实现两进程共用循环链表。
***********************************/


#ifndef   __ESBufferMan_H__2005_0919_
#define   __ESBufferMan_H__2005_0919_


/************************************************************************/
/* 基类item
   CESBufferMan 和   CESBufferManProcess 管理的链表中的item项都是继承此类                                                                     */
/************************************************************************/
// typedef struct tagESListItem
// {
// 	int    nBufSize;     // 内存大小
// 	int    nBufUseSize;  // 内存使用大小 
// 	BYTE*  pBuf;         // 该项的内存指针 
// 	
// 	
// 	tagESListItem::tagESListItem()
// 	{
// 		pBuf = NULL;
// 		nBufSize = 0;
// 		nBufUseSize = 0;
// 	}
// }ESListItem, *pESListItem;

//////////////////////////////////////////////////////////////////////////

//
template <class T> 
inline T SafeGet(T &val)
{
	LONG lRet =	InterlockedExchangeAdd((LPLONG)&val, 0L);
	return (T)lRet;
}

template <class T> 
inline T SafeSet(T &val, T val1)
{
	LONG lRet = InterlockedExchange((LPLONG)&val, (LONG)val1);
	return (T)lRet;
}

//////////////////////////////////////////////////////////////////////////


#include <list>
using namespace std;
/************************************************************************/
/* 进程内循环链表                                                                     */
/************************************************************************/
template<class T>
class  CESBufferMan  
{
public:
	CESBufferMan();
	virtual ~CESBufferMan();
public:
	BOOL SetListCountAndSize(unsigned int nListCount,unsigned int nBufSize=0);  //nBufSize==0 ,表示不开劈内存buffer

	BOOL AddItem2EmptyHead(T *pItem);
	BOOL AddItem2DataHead(T *pItem);
	BOOL AddItem2DataTail(T* pItem);
	BOOL AddItem2EmptyTail(T* pItem);
	T*  FetchDataHeadItem();    //if false ,return NULL
	T*  FetchEmptyHeadItem();   //if false ,return NULL
	
	void  ClearAllDataItem();   //将所有数据链项移至空链

	HANDLE GetAddItem2DataTailEvt();
	int  GetSizeOfAllList();	//获取所有的链 长度
	int  GetSizeOfDataList();   //获取数据链长度
	int  GetSizeOfEmptyList();  //获取空链长度

	typename list<T>::iterator ManListBegin();  //point to the first Item of m_ManList
	typename list<T>::iterator ManListEnd();    //point just beyond the end of m_ManList
	typename list<T>::iterator DataListBegin() {	return m_DataList.begin(); }
	typename list<T>::iterator DataListEnd() { return m_DataList.end(); }
private:
	
#pragma  warning(push)
#pragma  warning(once: 4251)
	list<T>    m_ManList;
	list<T *>  m_DataList;
	list<T *>  m_EmptyList;
#pragma  warning(pop)
		
	CRITICAL_SECTION m_CriSectionDataList;
	CRITICAL_SECTION m_CriSectionEmptyList;

	HANDLE m_hAddItem2DataTailEvt;
protected:
	void ReleaseAllList();
};



#include <vector>
/************************************************************************/
/* 进程间循环链表                                                                     */
/************************************************************************/

template<class T>
class CESBufferManProcess  
{
#pragma pack(push,4)
	typedef struct tagShareMemoryHead
	{
		unsigned int nItemCount;         //整个share memory中的Item项总数
		unsigned int nShareMemorySize; 	 //整个share memory内存占用大小
		int nWritePos;                   //上一次写入的项位置
        int nReadPos;                    //上一次读出的项位置 
		unsigned int nIsFull;           //数据项是否为满  0 表示否，1表示是
        unsigned int nIsEmpty;          //数据项是否为空  0 表示否，1表示是
		LONG dwFirstFrm;               //标记内存中存放的起始帧      Add
		LONG dwLastFrm;                //标记内存中存放的结尾帧      Add
		LONG dwTaskID;                 //记录内存中存放的当前任务ID  Add
		LONG dwClipID;                 //记录内存中存放的当前分段ID  Add
		tagShareMemoryHead()
		{
			dwClipID = -1;
			dwTaskID = -1;
			dwFirstFrm = -1;
			dwLastFrm = -1;
			nItemCount = 0;
			nShareMemorySize = 0;
			nWritePos = -1;
			nReadPos = -1;
			nIsFull  = 0;
			nIsEmpty = 1;
		}
	}SHAREMEMORYHEAD;
#pragma pack(pop)
public:
	CESBufferManProcess();
	virtual ~CESBufferManProcess();
public:
	//nBufSize==0 ,表示不开劈内存buffer;LPCTSTR lpName 文件映射名
	BOOL CreateShareMemory(unsigned int nBufSzieTaskInfo,unsigned int nItemCount,unsigned int nBufSizeV0,unsigned int nBufSizeV1,unsigned int nBufSizeA0,unsigned int nBufSizeA1,LPCTSTR lpSHMName,LPCTSTR lpAddDataEvtName); //server function
	BOOL OpenShareMemory(unsigned int nBufSzieTaskInfo,unsigned int nItemCount,unsigned int nBufSizeV0,unsigned int nBufSizeV1,unsigned int nBufSizeA0,unsigned int nBufSizeA1,LPCTSTR lpSHMName,LPCTSTR lpAddDataEvtName);//client function 
	void FlushForWrite(LONG FrmIndex_min ,LONG FrmIndex_Max ,LONG TaskID ,LONG ClipID);
	void Reset();   //对共享内存中的数据项起始位置、长度置0。此操作会remove掉所有数据项
    BOOL CreateExtMemBuf(unsigned int nExtBufV0Size ,unsigned int nExtBufV1Size);
	T* GetEmptyItemForWrite();
	void FlushWrite();
	T* GetDataItemForRead();
	void FlushRead();
	HANDLE GetWriteDataEvt();
	BYTE * GetTaskInfoAddr();
public:
	//操作接口2
	BOOL AddItem2EmptyHead(T *pItem);
	BOOL AddItem2DataHead(T *pItem);
	BOOL AddItem2DataTail(T* pItem);
	BOOL AddItem2EmptyTail(T* pItem);
	T*  FetchDataHeadItem();    //if false ,return NULL
	T*  FetchEmptyHeadItem();   //if false ,return NULL

	void  ClearAllDataItem();   //将所有数据链项移至空链

	HANDLE GetAddItem2DataTailEvt();
private:
	int m_iWriteIndex;
	BOOL ReleaseShareMemory();
	int  FindEmptyItemForWrite(); //寻找用来添加数据的空项，如果没有空项，return -1
	int  FindDataItemForRead();

	HANDLE m_hWriteDataEvt; //当添加数据时会点亮此事件
    HANDLE m_hMapFile;

    //share memeory 的映射目的地址
	SHAREMEMORYHEAD * m_pShareMemoryHead; //share memeory head

/*	备注：ADD  共享内存映射到各个进程空间的地址大多数可能是不同的(虽然它们都指向
    同一块内存区域)，所以添加了两个vetor数组来保存共享内存映射到本进程的地址信息，
	便于在被另一个进程改变时重新在本进程内进行复位                                         */
	vector<T*> m_vectorSHMItem;
	vector<BYTE*> m_vectorPBuf;
	vector<BYTE*> m_vectorPExtBuf;
	vector<HANDLE> m_vectorExtHandle;
	int  m_iItemCount;
	BYTE *m_pTaskInfo;//taskinfo 缓存地址
};

//////////////////////////////////////////////////////////////////////////
// CESBufferMan


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template<class T>
CESBufferMan<T>::CESBufferMan()
{
    InitializeCriticalSection(&m_CriSectionDataList);
	InitializeCriticalSection(&m_CriSectionEmptyList);

	m_hAddItem2DataTailEvt = CreateEvent( NULL,FALSE,FALSE,NULL); 
}
template<class T>
CESBufferMan<T>::~CESBufferMan()
{
	ReleaseAllList();
	DeleteCriticalSection(&m_CriSectionDataList);
	DeleteCriticalSection(&m_CriSectionEmptyList);

	if (NULL!=m_hAddItem2DataTailEvt)
	{
		CloseHandle(m_hAddItem2DataTailEvt);
		m_hAddItem2DataTailEvt = NULL;
	}
}
/************************************************************************/
/* argument: int nListCount  链的长度  必须>0
             int nBufSize   <=0,不分配内存buffer空间;>0分配nBufSize大小的空间,并管理之                                                         */
/************************************************************************/
template<class T>
BOOL CESBufferMan<T>::SetListCountAndSize(unsigned int nListCount,unsigned int nBufSize)
{  
	if (nListCount<=0)
	{
		return FALSE;
	}
	
	if(m_ManList.size()>0)
		ReleaseAllList();

	EnterCriticalSection(&m_CriSectionEmptyList);
	for(int n=0;n<nListCount;n++)
	{
		BYTE *pBuff = NULL;
		if (nBufSize>0)
		{
			pBuff = new BYTE[nBufSize];
			if(pBuff==NULL)
			{
				//MessageBox(NULL, "BufferManage 分配内存失败.", NULL, MB_OK);
				LeaveCriticalSection(&m_CriSectionEmptyList);
				if(m_ManList.size()>0)
					ReleaseAllList();
				return FALSE;
			}
		}

		T item;
		item.pBuf = pBuff;
		item.nBufSize = nBufSize;
		item.nBufUseSize = 0;
		m_ManList.push_back(item);
		m_EmptyList.push_back( &(m_ManList.back()) );
	}

	LeaveCriticalSection(&m_CriSectionEmptyList);

	int nsize = m_ManList.size();
	int nsizeemptey = m_EmptyList.size();
	int nsizeData = m_DataList.size();
	return TRUE;
}
template<class T>
HANDLE CESBufferMan<T>::GetAddItem2DataTailEvt()
{
	return m_hAddItem2DataTailEvt;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
template<class T>
T*  CESBufferMan<T>::FetchEmptyHeadItem()
{
	T *pListItem = NULL;
    EnterCriticalSection(&m_CriSectionEmptyList);
	if(!m_EmptyList.empty())
	{
		pListItem = m_EmptyList.front();
		m_EmptyList.pop_front();
	}
    LeaveCriticalSection(&m_CriSectionEmptyList);
    return pListItem;
}

template<class T>
T* CESBufferMan<T>::FetchDataHeadItem()
{
	T* pListItem  = NULL;
	EnterCriticalSection(&m_CriSectionDataList);
	if(!m_DataList.empty())
	{
		pListItem = m_DataList.front();
		m_DataList.pop_front();
	}

    LeaveCriticalSection(&m_CriSectionDataList);
    return pListItem;
}
template<class T> 
BOOL CESBufferMan<T>::AddItem2DataTail(T* pItem)
{
	EnterCriticalSection(&m_CriSectionDataList);
	m_DataList.push_back(pItem);
	SetEvent(m_hAddItem2DataTailEvt); //signale event
    LeaveCriticalSection(&m_CriSectionDataList);
	return TRUE;
}
template<class T>
BOOL CESBufferMan<T>::AddItem2EmptyTail(T* pItem)
{
	EnterCriticalSection(&m_CriSectionEmptyList);
	m_EmptyList.push_back(pItem);
    LeaveCriticalSection(&m_CriSectionEmptyList);
	return TRUE;
}
template<class T>
BOOL CESBufferMan<T>::AddItem2DataHead(T *pItem)
{
	EnterCriticalSection(&m_CriSectionDataList);
	m_DataList.push_front(pItem);
    LeaveCriticalSection(&m_CriSectionDataList);
	return TRUE;
}
template<class T>
BOOL CESBufferMan<T>::AddItem2EmptyHead(T *pItem)
{
	EnterCriticalSection(&m_CriSectionEmptyList);
	m_EmptyList.push_front(pItem);
    LeaveCriticalSection(&m_CriSectionEmptyList);
	return TRUE;
}
template<class T>
void  CESBufferMan<T>::ClearAllDataItem()
{
	EnterCriticalSection(&m_CriSectionDataList);
	list<T *>  TempList(m_DataList);
	m_DataList.clear();
    LeaveCriticalSection(&m_CriSectionDataList);

    EnterCriticalSection(&m_CriSectionEmptyList);
/*	m_EmptyList.clear();
	list<ESListItem>::iterator iter;

	for(iter = m_ManList.begin(); iter != m_ManList.end(); iter++)
	{
		m_EmptyList.push_back(&(*iter));
	}*/
	m_EmptyList.merge(TempList);
    LeaveCriticalSection(&m_CriSectionEmptyList);
}



/************************************************************************/
/* description: 释放链表，并释放链中所有内存                                                                */
/************************************************************************/
template<class T>
void CESBufferMan<T>::ReleaseAllList()
{
	EnterCriticalSection(&m_CriSectionEmptyList);
	EnterCriticalSection(&m_CriSectionDataList);
	m_EmptyList.clear();    
	m_DataList.clear();
	int nsize = m_ManList.size();
	int nsizeemptey = m_EmptyList.size();
	int nsizeData = m_DataList.size();
    while(!m_ManList.empty())
	{
		T* pItem = (T*)&m_ManList.front();
		if(NULL!=pItem->pBuf)
			delete pItem->pBuf;
		pItem->pBuf = NULL;
		m_ManList.pop_front();
	}	
    LeaveCriticalSection(&m_CriSectionEmptyList);
	LeaveCriticalSection(&m_CriSectionDataList);

}
template<class T>
int  CESBufferMan<T>::GetSizeOfAllList()
{
	return (int)m_ManList.size();
}

template<class T>
int  CESBufferMan<T>::GetSizeOfDataList()
{
	int nCount = 0;
	EnterCriticalSection(&m_CriSectionDataList);
	nCount = (int)m_DataList.size();
    LeaveCriticalSection(&m_CriSectionDataList);
	return nCount;
}

template<class T>
int  CESBufferMan<T>::GetSizeOfEmptyList()
{
	int nCount = 0;
	EnterCriticalSection(&m_CriSectionEmptyList);
    nCount = (int)m_EmptyList.size();
	LeaveCriticalSection(&m_CriSectionEmptyList);
	return nCount;
}
//point to the first Item of m_ManList
template<class T>
typename list<T>::iterator CESBufferMan<T>::ManListBegin()
{
	return m_ManList.begin();
}
//point just beyond the end of m_ManList

template<class T>
typename list<T>::iterator CESBufferMan<T>::ManListEnd()
{
	return m_ManList.end();
}



//////////////////////////////////////////////////////////////////////////
//CESBufferManProcess
template<class T>
CESBufferManProcess<T>::CESBufferManProcess()
{
	m_pShareMemoryHead = NULL;
	m_hMapFile = NULL;
	m_hWriteDataEvt = NULL;
}
template<class T>
CESBufferManProcess<T>::~CESBufferManProcess()
{
	ReleaseShareMemory();
}
/************************************************************************/
/* argument:
      unsigned  int nItemCount    链长度
      unsigned  int nBufSize      链中需要单独开辟的buffer大小，如果nBufSize==0 ,表示不开劈内存buffer
      LPCTSTR   lpSHMName         保存到atom列表中的文件映射名
      LPCTSTR   lpAddDataEvtName  保存到atom列表中的事件名
   description: 开辟的sharememery中存放数据的格式 
               -----------------------------------------------------------------------------------------------
               || 结构SHAREMEMORYHEAD ＋ nItemCount个结构T  ＋  nItemCount个buffer，每个buffer大小为nBufSize  ||
			   ------------------------------------------------------------------------------------------------                                                   */
/************************************************************************/
template<class T>
BOOL CESBufferManProcess<T>::CreateShareMemory(unsigned int nBufSzieTaskInfo,unsigned int nItemCount,unsigned int nBufSizeV0,unsigned int nBufSizeV1,unsigned int nBufSizeA0,unsigned int nBufSizeA1,LPCTSTR lpSHMName,LPCTSTR lpAddDataEvtName)
{
	unsigned int nSHMSize = sizeof(SHAREMEMORYHEAD)+nBufSzieTaskInfo+nItemCount*sizeof(T)+nItemCount*(nBufSizeV0+nBufSizeV1+nBufSizeA0+nBufSizeA1);
	m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
		                               PAGE_READWRITE, 0,
		                               nSHMSize, lpSHMName);
	if (m_hMapFile==NULL) 
	{ 
		return false;
	}
	m_iItemCount = nItemCount;
	m_hWriteDataEvt = CreateEvent(NULL, FALSE, FALSE, lpAddDataEvtName);
	if (NULL==m_hWriteDataEvt)
	{
		return false;
	}
    //映射到本地
	m_pShareMemoryHead = (SHAREMEMORYHEAD*)MapViewOfFile(m_hMapFile, 
								FILE_MAP_WRITE | FILE_MAP_READ,
								0, 0,
								nSHMSize);
	if(m_pShareMemoryHead==NULL)
	{
		return false;
	}
	m_pTaskInfo = (BYTE*)m_pShareMemoryHead+sizeof(SHAREMEMORYHEAD);
	BYTE* pItemBegin = (BYTE*)m_pShareMemoryHead+sizeof(SHAREMEMORYHEAD)+nBufSzieTaskInfo;
	BYTE* pBufBegin  = (BYTE*)m_pShareMemoryHead+sizeof(SHAREMEMORYHEAD)+nBufSzieTaskInfo+nItemCount*sizeof(T);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//检查映射过来的内存内容是否正常
		//检查head 内容
		if ((m_pShareMemoryHead->nItemCount!=nItemCount)||(m_pShareMemoryHead->nShareMemorySize!=nSHMSize))
		{
			return false;
		}
		//将share memory 中的各个item项添加到数组中
		DWORD dwBufSize = nBufSizeV0+nBufSizeV1+nBufSizeA0+nBufSizeA1;
		for(int n=0;n<nItemCount;n++)
		{
			T* pItem =  (T*)(pItemBegin+n*sizeof(T));
			m_vectorSHMItem.push_back(pItem);
			if (dwBufSize>0)
			{
				m_vectorPBuf.push_back(pBufBegin+n*dwBufSize);
			}
			else
			{
				m_vectorPBuf.push_back(NULL);
			}
			m_vectorPExtBuf.push_back(NULL);
			m_vectorExtHandle.push_back(NULL);
		}	
		return true;
	}
	//初始化share memory head
	m_pShareMemoryHead->nItemCount = nItemCount;
	m_pShareMemoryHead->nShareMemorySize = nSHMSize;
	m_pShareMemoryHead->nWritePos = -1;
    m_pShareMemoryHead->nReadPos  = -1;
    m_pShareMemoryHead->nIsFull   = 0;
    m_pShareMemoryHead->nIsEmpty  = 1;
	m_pShareMemoryHead->dwClipID = -1;
	m_pShareMemoryHead->dwFirstFrm = -1;
	m_pShareMemoryHead->dwLastFrm = -1;
	m_pShareMemoryHead->dwTaskID = -1;
    //将share memory 中的各个item项添加到数组中
	//BYTE* pItemBegin = (BYTE*)m_pShareMemoryHead+sizeof(SHAREMEMORYHEAD);
	//BYTE* pBufBegin  = (BYTE*)m_pShareMemoryHead+sizeof(SHAREMEMORYHEAD)+nItemCount*sizeof(T);
	DWORD dwBufSize = nBufSizeV0+nBufSizeV1+nBufSizeA0+nBufSizeA1;
	for(int n=0;n<nItemCount;n++)
	{
		T* pItem =  (T*)(pItemBegin+n*sizeof(T));
		pItem->nBufSizeV0 =  nBufSizeV0;
		pItem->nBufSizeUsedV0 = 0;
		pItem->pBufV0 = NULL;
		pItem->nBufSizeV1 =  nBufSizeV1;
		pItem->nBufSizeUsedV1 = 0;
		pItem->pBufV1 = NULL;
		pItem->nBufSizeA0 =  nBufSizeA0;
		pItem->nBufSizeUsedA0 = 0;
		pItem->pBufA0 = NULL;
		pItem->nBufSizeA1 =  nBufSizeA1;
		pItem->nBufSizeUsedA1 = 0;
		pItem->pBufA1 = NULL;
		pItem->nExtBufV0Size = 0;
		pItem->nExtbufV0UseSize = 0;
		pItem->pExtBufV0 = NULL;
		pItem->nExtBufV1Size = 0;
		pItem->nExtBufV1UseSize = 0;
		pItem->pExtBufV1 = NULL;
		m_vectorSHMItem.push_back(pItem);
		if (dwBufSize>0)
		{
			m_vectorPBuf.push_back(pBufBegin+n*dwBufSize);
		}
		else
		{
			m_vectorPBuf.push_back(NULL);
		}
		m_vectorPExtBuf.push_back(NULL);
		m_vectorExtHandle.push_back(NULL);
	}
	return true;
}
/************************************************************************/
/* argument:
      unsigned  int nItemCount    链长度
      unsigned  int nBufSize      链中需要单独开辟的buffer大小，如果nBufSize==0 ,表示不开劈内存buffer
      LPCTSTR   lpSHMName         保存到atom列表中的文件映射名
      LPCTSTR   lpAddDataEvtName  保存到atom列表中的事件名
      !! 所有的参数 必须跟函数CreateShareMemory中的参数像同
     
   description: 开辟的sharememery中存放数据的格式 
               -----------------------------------------------------------------------------------------------
               || 结构SHAREMEMORYHEAD ＋ nItemCount个结构T  ＋  nItemCount个buffer，每个buffer大小为nBufSize  ||
			   ------------------------------------------------------------------------------------------------                                                   */
/************************************************************************/
template<class T>
BOOL CESBufferManProcess<T>::OpenShareMemory(unsigned int nBufSzieTaskInfo,unsigned int nItemCount,unsigned int nBufSizeV0,unsigned int nBufSizeV1,unsigned int nBufSizeA0,unsigned int nBufSizeA1,LPCTSTR lpSHMName,LPCTSTR lpAddDataEvtName)
{
	m_hMapFile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
									 FALSE,
									lpSHMName);
	if (m_hMapFile == NULL) 
	{ 
		return false;
	}
	m_iItemCount = nItemCount;
    m_hWriteDataEvt = OpenEvent(EVENT_MODIFY_STATE|SYNCHRONIZE, FALSE, lpAddDataEvtName);
    if (NULL==m_hWriteDataEvt)
	{
		return false;
    }
	//映射的文件大小
    unsigned int nSHMSize = sizeof(SHAREMEMORYHEAD)+nBufSzieTaskInfo+nItemCount*sizeof(T)+nItemCount*(nBufSizeV0+nBufSizeV1+nBufSizeA0+nBufSizeA1);
	//映射到本地
	m_pShareMemoryHead = (SHAREMEMORYHEAD*)MapViewOfFile(m_hMapFile, 
								FILE_MAP_WRITE | FILE_MAP_READ,
								0, 0,
								nSHMSize);
	if(m_pShareMemoryHead==NULL)
	{
		return false;
	}

	//检查映射过来的内存内容是否正常
	//检查head 内容
	if ((m_pShareMemoryHead->nItemCount!=nItemCount)||(m_pShareMemoryHead->nShareMemorySize!=nSHMSize))
	{
		return false;
	}
	m_pTaskInfo = (BYTE*)m_pShareMemoryHead+sizeof(SHAREMEMORYHEAD);
	//将share memory 中的各个item项添加到数组中
	BYTE* pItemBegin = (BYTE*)m_pShareMemoryHead+sizeof(SHAREMEMORYHEAD)+nBufSzieTaskInfo;
	BYTE* pBufBegin  = (BYTE*)m_pShareMemoryHead+sizeof(SHAREMEMORYHEAD)+nBufSzieTaskInfo+nItemCount*sizeof(T);
	DWORD dwBufSize = nBufSizeV0+nBufSizeV1+nBufSizeA0+nBufSizeA1;
	for(int n=0;n<nItemCount;n++)
	{
		T* pItem =  (T*)(pItemBegin+n*sizeof(T));
		m_vectorSHMItem.push_back(pItem);
		if (dwBufSize>0)
		{
			m_vectorPBuf.push_back(pBufBegin+n*dwBufSize);
		}
		else
		{
			m_vectorPBuf.push_back(NULL);
		}
		m_vectorPExtBuf.push_back(NULL);
		m_vectorExtHandle.push_back(NULL);
	}	
	return true;
}

template<class T>
BOOL CESBufferManProcess<T>::ReleaseShareMemory()
{
	if (m_pShareMemoryHead!=NULL)
	{
		if (!UnmapViewOfFile(m_pShareMemoryHead)) 
		{ 
			return false;
		}
        m_pShareMemoryHead = NULL;
	}
	if (NULL!=m_hMapFile)
	{
		CloseHandle(m_hMapFile);
		m_hMapFile = NULL;
	}
	if (NULL!=m_hWriteDataEvt)
	{
		CloseHandle(m_hWriteDataEvt);
		m_hWriteDataEvt = NULL;
	}
	for (int n = 0 ;n < m_iItemCount ; n++) //释放扩展内存   ADD
	{
		if (m_vectorPExtBuf[n] != NULL)
		{
			if (!UnmapViewOfFile(m_vectorPExtBuf[n]))
			{
				CString strLog;
				strLog.Format(_T("UnmapViewOfFile pExtBuf[%d] Failed") ,n);
				OutputDebugString(strLog);
				return FALSE;
			}
			m_vectorPExtBuf[n] = NULL;
		}

		if (m_vectorExtHandle[n] != NULL)
		{
			CloseHandle(m_vectorExtHandle[n]);
			m_vectorExtHandle[n] = NULL;
		}
	}
	return true;
}

template<class T>
void CESBufferManProcess<T>::Reset()
{
	SafeSet(m_pShareMemoryHead->nWritePos,(int)-1);
	SafeSet(m_pShareMemoryHead->nReadPos,(int)-1);
	SafeSet(m_pShareMemoryHead->nIsFull,(unsigned int)0);
	SafeSet(m_pShareMemoryHead->nIsEmpty,(unsigned int)1);
	SafeSet(m_pShareMemoryHead->dwClipID ,(LONG)-1);
	SafeSet(m_pShareMemoryHead->dwTaskID ,(LONG)-1);
	SafeSet(m_pShareMemoryHead->dwFirstFrm ,(LONG)-1);
	SafeSet(m_pShareMemoryHead->dwLastFrm ,(LONG)-1);

}

/************************************************************************/
/* FindEmptyItemForWrite() 是在添加数据的进程调用，所以保证了m_pShareMemoryHead->nWritePos 
   是不会改变的。尽管m_pShareMemoryHead->nReadPos随时在改变，
   但还是安全的                                                                 */
/************************************************************************/
template<class T>
int  CESBufferManProcess<T>::FindEmptyItemForWrite()
{
	unsigned int nIsFull = SafeGet(m_pShareMemoryHead->nIsFull);
    if (nIsFull==1)
	{
		return -1;
    }
	int nWritePos   = SafeGet(m_pShareMemoryHead->nWritePos);  
	unsigned int nIndex = (nWritePos+1)%m_pShareMemoryHead->nItemCount;
	return nIndex;
}
/************************************************************************/
/*FindDataItemForRead() 是在添加数据的进程调用，所以保证了m_pShareMemoryHead->nDataBegin 
   是不会改变的。尽管m_pShareMemoryHead->nDataEnd随时在改变，
   但还是安全的                                                                */
/************************************************************************/
template<class T>
int  CESBufferManProcess<T>::FindDataItemForRead()
{
    unsigned int nIsEmpty  = SafeGet(m_pShareMemoryHead->nIsEmpty);
	if (nIsEmpty==1)
	{
		return -1;
	}
    int nReadPos = SafeGet(m_pShareMemoryHead->nReadPos);	
	unsigned int nIndex = (nReadPos+1)%m_pShareMemoryHead->nItemCount;
	return nIndex;		

}

template<class T>
T* CESBufferManProcess<T>::GetEmptyItemForWrite()
{
	int nIndex = FindEmptyItemForWrite();
	if (nIndex<0)
	{
		return NULL;
	}
	m_iWriteIndex = nIndex;
	DWORD OffSet = 0;
	T* pEmptyItem = m_vectorSHMItem[nIndex];
    pEmptyItem->pBufV0 = m_vectorPBuf[nIndex];
	OffSet += pEmptyItem->nBufSizeV0;
	pEmptyItem->pBufV1 = m_vectorPBuf[nIndex]+OffSet;
	OffSet += pEmptyItem->nBufSizeV1;
	pEmptyItem->pBufA0 = m_vectorPBuf[nIndex]+OffSet;
	OffSet += pEmptyItem->nBufSizeA0;
    pEmptyItem->pBufA1 = m_vectorPBuf[nIndex]+OffSet;
	if (m_vectorPExtBuf[nIndex] != NULL)    //释放扩展BUF   ADD
	{
       UnmapViewOfFile(m_vectorPExtBuf[nIndex]);
	   m_vectorPExtBuf[nIndex] = NULL;
	   pEmptyItem->pExtBufV0 = NULL;
	   pEmptyItem->pExtBufV1 = NULL;
	   pEmptyItem->nExtBufV0Size = 0;
	   pEmptyItem->nExtbufV0UseSize = 0;
	   pEmptyItem->nExtBufV1Size = 0;
	   pEmptyItem->nExtBufV1UseSize = 0;
	}
	if (m_vectorExtHandle[nIndex] != NULL)
	{
		CloseHandle(m_vectorExtHandle[nIndex]);
		m_vectorExtHandle[nIndex] = NULL;
	}
	return pEmptyItem;
}

template<class T>
void CESBufferManProcess<T>::FlushWrite()
{
	int nIndex = FindEmptyItemForWrite();
	if (nIndex<0)
	{
		return;
	}
	int nReadPos = SafeGet(m_pShareMemoryHead->nReadPos);
	if((nIndex==nReadPos)||(nIndex-nReadPos==m_pShareMemoryHead->nItemCount))
	{
		SafeSet(m_pShareMemoryHead->nIsFull,(unsigned int)1);
	}
	SafeSet(m_pShareMemoryHead->nWritePos,nIndex);
	SafeSet(m_pShareMemoryHead->nIsEmpty,(unsigned int)0);
	SetEvent(m_hWriteDataEvt);
}


template<class T>
T* CESBufferManProcess<T>::GetDataItemForRead()
{
	int nIndex = FindDataItemForRead();
	if (nIndex<0)
	{
		return NULL;
	}
	DWORD OffSet = 0;
	T* pDataItem = m_vectorSHMItem[nIndex];
    pDataItem->pBufV0 = m_vectorPBuf[nIndex];
	OffSet += pDataItem->nBufSizeV0;
	pDataItem->pBufV1 = m_vectorPBuf[nIndex]+OffSet;
	OffSet += pDataItem->nBufSizeV1;
	pDataItem->pBufA0 = m_vectorPBuf[nIndex]+OffSet;
	OffSet += pDataItem->nBufSizeA0;
    pDataItem->pBufA1 = m_vectorPBuf[nIndex]+OffSet;
	if (pDataItem->nExtBufV0Size>0 || pDataItem->nExtBufV1Size>0)//本进程内定位扩展BUF地址   ADD
	{
		CString csName;
		csName.Format(_T("SOBEY_MSV_EXTBUF_NAME_%d") ,nIndex);
		m_vectorExtHandle[nIndex] = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE ,
			                             FALSE ,csName);
		if (m_vectorExtHandle[nIndex] == NULL)
		{
			CString strLog;
			strLog.Format(_T("Error: OpenFileMapping ExtBuf Failed. function: CESBufferManProcess<T>::GetDataItemForRead()"));
			OutputDebugString(strLog);
			return NULL;
		}
		m_vectorPExtBuf[nIndex] = (BYTE*)MapViewOfFile(m_vectorExtHandle[nIndex], 
			                       FILE_MAP_WRITE | FILE_MAP_READ,0, 0,
								   pDataItem->nExtBufV0Size+pDataItem->nExtBufV1Size);
		if (pDataItem->nExtBufV0Size > 0)
		{
		    pDataItem->pExtBufV0 = m_vectorPExtBuf[nIndex];
		}
		else
		{
			pDataItem->pExtBufV0 =  NULL;
		}
		if (pDataItem->nExtBufV1Size > 0)
		{
			pDataItem->pExtBufV1 = m_vectorPExtBuf[nIndex]+pDataItem->nExtBufV0Size;
		}
		else
		{
			pDataItem->pExtBufV1 =  NULL;
		}
	}
    return pDataItem;
}

template<class T>
void CESBufferManProcess<T>::FlushRead()
{
	int nIndex = FindDataItemForRead();
	if (nIndex<0)
	{
		return;
	}

	if (m_vectorPExtBuf[nIndex] != NULL)    //释放扩展BUF   ADD
	{
		UnmapViewOfFile(m_vectorPExtBuf[nIndex]);
		m_vectorPExtBuf[nIndex] = NULL;
	}
	if (m_vectorExtHandle[nIndex] != NULL)
	{
		CloseHandle(m_vectorExtHandle[nIndex]);
		m_vectorExtHandle[nIndex] = NULL;
	}

	int nWritePos = SafeGet(m_pShareMemoryHead->nWritePos);
    if (nWritePos == nIndex)
	{
		SafeSet(m_pShareMemoryHead->nIsEmpty,(unsigned int)1);
    }

	SafeSet(m_pShareMemoryHead->nReadPos,nIndex);
	SafeSet(m_pShareMemoryHead->nIsFull,(unsigned int)0);
}

template<class T>
void CESBufferManProcess<T>::FlushForWrite(LONG FrmIndex_min ,LONG FrmIndex_Max ,LONG TaskID ,LONG ClipID)
{
    SafeSet(m_pShareMemoryHead->dwClipID ,(LONG)ClipID);
	SafeSet(m_pShareMemoryHead->dwTaskID ,(LONG)TaskID);
	SafeSet(m_pShareMemoryHead->dwFirstFrm ,(LONG)FrmIndex_min);
	SafeSet(m_pShareMemoryHead->dwLastFrm ,(LONG)FrmIndex_Max);
}

//为了避免过多使用命名空间，V0,V1的扩展内存申请成一整块   ADD
template<class T>
BOOL CESBufferManProcess<T>::CreateExtMemBuf(unsigned int nExtBufV0Size ,unsigned int nExtBufV1Size)
{
	if (nExtBufV0Size+nExtBufV1Size <= 0)
	{
		return FALSE;
	}
    CString csName;
	csName.Format(_T("SOBEY_MSV_EXTBUF_NAME_%d") ,m_iWriteIndex);
	m_vectorExtHandle[m_iWriteIndex] = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
		                               PAGE_READWRITE, 0,
									   nExtBufV0Size+nExtBufV1Size, csName);
		                               
	if (m_vectorExtHandle[m_iWriteIndex] == NULL)
	{
		CString strLog;
		strLog.Format(_T("Error: CreateFileMapping ExtBuf Failed. function: CESBufferManProcess<T>::CreateExtMemBuf()"));
		OutputDebugString(strLog);
		return FALSE;
	}
	m_vectorPExtBuf[m_iWriteIndex] = (BYTE*)MapViewOfFile(m_vectorExtHandle[m_iWriteIndex], 
		                              FILE_MAP_WRITE | FILE_MAP_READ,0, 0,
							          nExtBufV0Size+nExtBufV1Size);
	T* pEmptyItem = m_vectorSHMItem[m_iWriteIndex];
	pEmptyItem->nExtBufV0Size = nExtBufV0Size;
	pEmptyItem->nExtBufV1Size = nExtBufV1Size;
	if (nExtBufV0Size > 0)  //V0使用扩展空间
	{
       pEmptyItem->pExtBufV0 = m_vectorPExtBuf[m_iWriteIndex];
	}
	else
	{
       pEmptyItem->pExtBufV0 = NULL;
	}

	if (nExtBufV1Size > 0)  //V1使用扩展空间
	{
		pEmptyItem->pExtBufV1 = m_vectorPExtBuf[m_iWriteIndex]+nExtBufV0Size;
	}
	else
	{
		pEmptyItem->pExtBufV1 = NULL;
	}
	return TRUE;
}


template<class T>
HANDLE CESBufferManProcess<T>::GetWriteDataEvt()
{
	return m_hWriteDataEvt;
}


template<class T>
BOOL CESBufferManProcess<T>::AddItem2EmptyHead(T *pItem)
{

}
template<class T>
BOOL CESBufferManProcess<T>::AddItem2DataHead(T *pItem)
{

}
template<class T>
BOOL CESBufferManProcess<T>::AddItem2DataTail(T* pItem)
{

}
template<class T>
BOOL CESBufferManProcess<T>::AddItem2EmptyTail(T* pItem)
{

}
template<class T>
T*  CESBufferManProcess<T>::FetchDataHeadItem()    //if false ,return NULL
{

}
template<class T>
T*  CESBufferManProcess<T>::FetchEmptyHeadItem()   //if false ,return NULL
{

}
template<class T>
void  CESBufferManProcess<T>::ClearAllDataItem()  //将所有数据链项移至空链
{

}
template<class T>
HANDLE CESBufferManProcess<T>::GetAddItem2DataTailEvt()
{

}
template<class T>
BYTE * CESBufferManProcess<T>::GetTaskInfoAddr()
{
	return  m_pTaskInfo;
}
#endif
