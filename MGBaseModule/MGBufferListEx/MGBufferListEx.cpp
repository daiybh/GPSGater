// MGBufferListEx.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MGBufferPoolList.h"
#include "MGBufferListEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MGBufferListEx::MGBufferListEx()
:m_pBufferPoolListEx(NULL)
{
	m_pBufferPoolListEx = new BufferPoolListEx<LISTITEMINFOEX>();
}

MGBufferListEx::~MGBufferListEx()
{
	if (m_pBufferPoolListEx != NULL)
	{
		delete m_pBufferPoolListEx;
		m_pBufferPoolListEx = NULL;
	}
}

BOOL MGBufferListEx::Initialize(DWORD nTotalSize, DWORD nUnitSize, DWORD nBLLength)
{
	return (m_pBufferPoolListEx == NULL)?FALSE:(m_pBufferPoolListEx->Initialize(nTotalSize, nUnitSize, nBLLength));
}

HANDLE MGBufferListEx::GetAddItem2DataTailEvt()
{
	return (m_pBufferPoolListEx == NULL)?INVALID_HANDLE_VALUE:(m_pBufferPoolListEx->GetAddItem2DataTailEvt()); 
}

BOOL MGBufferListEx::AddItem2DataHead(LISTITEMINFOEX *pItem, BOOL bCheckNoneUse)
{
	return (m_pBufferPoolListEx == NULL)?FALSE:(m_pBufferPoolListEx->AddItem2DataHead(pItem, bCheckNoneUse));
}

BOOL MGBufferListEx::AddItem2DataTail(IN LISTITEMINFOEX *pItem, BOOL bCheckNoneUse)
{
	return (m_pBufferPoolListEx == NULL)?FALSE:(m_pBufferPoolListEx->AddItem2DataTail(pItem, bCheckNoneUse));
}

BOOL MGBufferListEx::AddItem2EmptyHead(IN LISTITEMINFOEX *pItem)
{
	return (m_pBufferPoolListEx == NULL)?FALSE:(m_pBufferPoolListEx->AddItem2EmptyHead(pItem));
}

BOOL MGBufferListEx::AddItem2EmptyTail(IN LISTITEMINFOEX *pItem)
{
	return (m_pBufferPoolListEx == NULL)?FALSE:(m_pBufferPoolListEx->AddItem2EmptyTail(pItem));
}

LISTITEMINFOEX*	MGBufferListEx::FetchEmptyHeadItem(DWORD dwDesireSizeV0 /* = 0 */, DWORD dwDesireSizeA0 /* = 0 */, DWORD dwDesireSizeV1 /* = 0 */, DWORD dwDesireSizeA1 /* = 0 */)
{
	return (m_pBufferPoolListEx == NULL)?NULL:(m_pBufferPoolListEx->FetchEmptyHeadItem(dwDesireSizeV0,dwDesireSizeV1,dwDesireSizeA0,dwDesireSizeA1));
}

LISTITEMINFOEX*	MGBufferListEx::FetchDataHeadItem()
{
	return (m_pBufferPoolListEx == NULL)?NULL:(m_pBufferPoolListEx->FetchDataHeadItem());
}

void MGBufferListEx::ClearAllDataItem()
{
	return (m_pBufferPoolListEx == NULL)?NULL:(m_pBufferPoolListEx->ClearAllDataItem());
}

int MGBufferListEx::GetSizeOfDataList()
{
	return (m_pBufferPoolListEx == NULL)?0:(m_pBufferPoolListEx->GetSizeOfDataList());
}

int MGBufferListEx::GetSizeOfEmptyList()
{
	return (m_pBufferPoolListEx == NULL)?0:(m_pBufferPoolListEx->GetSizeOfEmptyList());
}

void MGBufferListEx::GetBufferEmptySize(OUT DWORD &dwEmptySize, OUT DWORD &dwAllSize)
{
	if (m_pBufferPoolListEx == NULL)
	{
		dwEmptySize = 0;
		dwAllSize = 0;
	}
	else
	{
		m_pBufferPoolListEx->GetBufferEmptySize(dwEmptySize, dwAllSize);
	}
}

int MGBufferListEx::GetSizeOfAllList()
{
	return (m_pBufferPoolListEx == NULL)?0:(m_pBufferPoolListEx->GetSizeOfAllList());
}

void MGBufferListEx::InitListItemInfo(LISTITEMINFOEX* pItemDes,LISTITEMINFOEX* pItemSrc)
{
	pItemDes->nFlag = pItemSrc->nFlag;
	int nSize = sizeof(GPSGATEDATA);
	memcpy(&pItemDes->curSocketInfo,&pItemSrc->curSocketInfo,nSize);
	pItemDes->dwFieldIndex = pItemSrc->dwFieldIndex;
	/*
	pItemDes->nFrameTypeV0 = pItemSrc->nFrameTypeV0;
	pItemDes->nFrameTypeV1 = pItemSrc->nFrameTypeV1;
	pItemDes->dwSysTime = pItemSrc->dwSysTime;
	pItemDes->nControlFlag = pItemSrc->nControlFlag;
	pItemDes->nItemStatus = pItemSrc->nItemStatus;
	pItemDes->nKeyFrame0 = pItemSrc->nKeyFrame0;
	pItemDes->nKeyFrame1 = pItemSrc->nKeyFrame1;
	pItemDes->nManualFrame = pItemSrc->nManualFrame;
	pItemDes->nManualFrameType = pItemSrc->nManualFrameType;
	pItemDes->nKeyFrameCupClip = pItemSrc->nKeyFrameCupClip;
	pItemDes->nFieldFrame = pItemSrc->nFieldFrame;
	pItemDes->nFieldFrameType = pItemSrc->nFieldFrameType;
	pItemDes->nRetrospectLength = pItemSrc->nRetrospectLength;
	pItemDes->enumVideoST = pItemSrc->enumVideoST;
	pItemDes->nFrameRateIndex = pItemSrc->nFrameRateIndex;
	pItemDes->nWidth = pItemSrc->nWidth;
	pItemDes->nHeight = pItemSrc->nHeight;
	pItemDes->nWidthLow = pItemSrc->nWidthLow;
	pItemDes->nHeightLow = pItemSrc->nHeightLow;
	pItemDes->m_dwVITC = pItemSrc->m_dwVITC;
	pItemDes->m_dwLTC = pItemSrc->m_dwLTC;

	pItemDes->iHighVideoDataType = pItemSrc->iHighVideoDataType;
	pItemDes->iLowVideoDataType  = pItemSrc->iLowVideoDataType;
	pItemDes->iSrcAudioDepth     = pItemSrc->iSrcAudioDepth;
	/**/
}