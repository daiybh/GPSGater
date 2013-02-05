#pragma once
#include "MGCaptureDefine.h"
#include "GpsGateDatadef.h"
#ifdef _MG_BUFFER_LISTEX_EXPORT_DEFINE_
#define MG_BUFFER_LISTEX_EXPORT __declspec(dllexport)
#else
#define MG_BUFFER_LISTEX_EXPORT __declspec(dllimport)
#pragma comment(lib,"MGBufferListExU.lib")
#endif
//当前的tagListItemInfoEx是收录使用的，如果是其它模块要自己定制，请重新从ListItemInfoBase派生出来，再送给BufferListEx
typedef struct tagListItemInfoEx : public ListItemInfoBase
{
	int		nFlag;			//1：任务开始，携带任务参数  0：一般数据项  2:任务结束 
	DWORD	dwFieldIndex;
	SOCKETINFO	curSocketInfo;

	int  nBufSizeV0;
	int  nBufSizeV1;
	int  nBufSizeA0;
	int  nBufSizeA1;


	tagListItemInfoEx()
	{
		dwFieldIndex =0;
		pBufV0 = NULL;
		pBufA0 = NULL;
		pBufV1 = NULL;
		pBufA1 = NULL;

		nBufSizeUsedV0 = 0;
		nBufSizeUsedV1 = 0;
		nBufSizeUsedA0 = 0;
		nBufSizeUsedA1 = 0;
		memset(&curSocketInfo,0,sizeof(curSocketInfo));
	}
}LISTITEMINFOEX,*pLISTITEMINFOEX;

template<class T> class BufferPoolListEx;
class MG_BUFFER_LISTEX_EXPORT MGBufferListEx
{
public:
	MGBufferListEx();
	virtual ~MGBufferListEx();
public:
	BOOL	Initialize(DWORD nTotalSize, DWORD nUnitSize, DWORD nBLLength);
	HANDLE	GetAddItem2DataTailEvt();

	BOOL	AddItem2EmptyTail(IN LISTITEMINFOEX *pItem);
	BOOL	AddItem2EmptyHead(IN LISTITEMINFOEX *pItem);
	BOOL	AddItem2DataHead(IN LISTITEMINFOEX *pItem, BOOL bCheckNoneUse = FALSE);
	BOOL	AddItem2DataTail(IN LISTITEMINFOEX *pItem, BOOL bCheckNoneUse = FALSE);	

	LISTITEMINFOEX*		FetchDataHeadItem(); 
	LISTITEMINFOEX*		FetchEmptyHeadItem(	DWORD dwDesireSizeV0,											
											DWORD dwDesireSizeA0,
											DWORD dwDesireSizeV1,
											DWORD dwDesireSizeA1); 

	void	ClearAllDataItem();	

	int		GetSizeOfAllList();	//获取所有的链 长度
	int		GetSizeOfEmptyList();  //获取空链长度
	int		GetSizeOfDataList();   //获取数据链长度
	void	GetBufferEmptySize(OUT DWORD &dwEmptySize, OUT DWORD &dwAllSize);	//获取内存池空闲大小和总大小

	static void InitListItemInfo(LISTITEMINFOEX* pItemDes,LISTITEMINFOEX* pItemSrc);
private:
	BufferPoolListEx<LISTITEMINFOEX> *m_pBufferPoolListEx;
};