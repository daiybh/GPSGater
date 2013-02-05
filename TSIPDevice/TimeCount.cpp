 // CTimeCount.cpp

#include "stdafx.h"
#include <atlstr.h>
#include "TimeCount.h"

CTimeCount::CTimeCount()
{
	::QueryPerformanceFrequency(&m_lgFrequency);
	Reset();
}

CTimeCount::~CTimeCount()
{
}

void
CTimeCount::Reset(void)
{
	::QueryPerformanceCounter(&m_lgStart);
}

double CTimeCount::GetCountTime(BOOL bMS)
{
	LARGE_INTEGER lgEnd;
	::QueryPerformanceCounter(&lgEnd);

	CString str;
	double dSecond = (double)(lgEnd.QuadPart - m_lgStart.QuadPart) / m_lgFrequency.QuadPart;
	if(bMS)
		dSecond *= 1000.0;

	//str.Format("Elapsed time is: %f\n", dSecond);

	return dSecond;
}