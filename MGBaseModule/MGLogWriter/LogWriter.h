// LogWriter.h: interface for the CLogWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGWRITER_H__30A8C15E_E13E_42AA_BDD9_B2E3C7967A40__INCLUDED_)
#define AFX_LOGWRITER_H__30A8C15E_E13E_42AA_BDD9_B2E3C7967A40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxmt.h"

#define LOG_NORMAL	0
#define LOG_ERROR	1
#define LOG_DEBUG	2
#define LOG_WARNING 3


#ifdef _LOG_WRITER_EXPORT_DEFINE_
#define LOG_WRITER_EXPORT __declspec(dllexport)
#else
#define LOG_WRITER_EXPORT __declspec(dllimport)
#endif

class LOG_WRITER_EXPORT CLogWriter
{
friend LOG_WRITER_EXPORT LPVOID CreateLogWriter(CString strLogPathName);
public:
	CLogWriter();
	virtual		~CLogWriter();
	
	BOOL		WriteMGLog(WORD bTypeLog,LPCTSTR strMessage,LPCTSTR strFunName,int ErrorCode=-1);
	
private:
	void		InitPath(CString strLogPathName);
	BOOL		_IsDayChange();
	void		_RemoveOutTimeLogFiles(); 

	CString		m_strFilePath;
    CFile		m_LogFile;
	BOOL		m_bOpen;
	BOOL		m_bInitPath;
	CTime		m_tmOldTime;
	CCriticalSection m_csWriteLog;
};

//strLogPathName 仅仅是最后新建一级目录的名称
LPVOID LOG_WRITER_EXPORT CreateLogWriter(CString strLogPathName);
void  LOG_WRITER_EXPORT	ReleaseLogWriter();

#endif // !defined(AFX_LOGWRITER_H__30A8C15E_E13E_42AA_BDD9_B2E3C7967A40__INCLUDED_)
