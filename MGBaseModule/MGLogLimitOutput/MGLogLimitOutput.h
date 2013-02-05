#if !defined(AFX_LOGOUTLIMITOUTPUT_H__E55421E_FBA3_4C45_BAEA_050268F07DC0__INCLUDED_)
#define AFX_LOGOUTLIMITOUTPUT_H__E55421E_FBA3_4C45_BAEA_050268F07DC0__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _MG_LOGLIMITOUTPUT_
#ifdef _DEBUG   
#pragma comment(lib,"MGLogLimitOutputU.lib")
#else
#pragma comment(lib,"MGLogLimitOutputU.lib")
#endif
#endif

#ifdef _MG_LOGLIMITOUTPUT_
#define MG_LOGLIMITOUTPUT_EXPORT_MODE __declspec(dllexport)
#else
#define MG_LOGLIMITOUTPUT_EXPORT_MODE __declspec(dllimport)
#endif

class MG_LOGLIMITOUTPUT_EXPORT_MODE CLogLimitOutput  
{
public:
	CLogLimitOutput();
	virtual ~CLogLimitOutput();
public:
	void  SetLimitCount(DWORD dwLimitCount);
	BOOL  IsOutputLog();
	DWORD GetTotalCount();

private:
	DWORD m_dwCount;
	DWORD m_dwLimitCount;
};

#endif