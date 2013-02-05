// MGLogLimitOutput.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MGLogLimitOutput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLogLimitOutput::CLogLimitOutput()
{
	m_dwCount = 0;
	m_dwLimitCount = 50;
}
CLogLimitOutput::~CLogLimitOutput()
{

}
BOOL CLogLimitOutput::IsOutputLog()
{
	BOOL iRet(FALSE);
	if((m_dwCount %  m_dwLimitCount) == 0)
	{
		iRet =  TRUE;
	}
	m_dwCount++;

	if(m_dwCount == 0xFFFFFFFF)
	{
		m_dwCount = 0;
	}
	return iRet;
}
DWORD CLogLimitOutput::GetTotalCount()
{
	return m_dwCount;
}

void CLogLimitOutput::SetLimitCount(DWORD dwLimitCount)
{
	m_dwLimitCount = dwLimitCount;
}