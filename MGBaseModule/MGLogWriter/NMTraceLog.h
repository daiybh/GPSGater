// NMTraceLog.h: interface for the CNMTraceLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NMTRACELOG_H__FFE9683E_18EF_43CC_8A7A_705386717199__INCLUDED_)
#define AFX_NMTRACELOG_H__FFE9683E_18EF_43CC_8A7A_705386717199__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "loghead.h"

#ifdef _LOG_WRITER_EXPORT_DEFINE_
#define LOG_WRITER_EXPORT __declspec(dllexport)
#else
#define LOG_WRITER_EXPORT __declspec(dllimport)
#pragma comment(lib,"MGLogWriterU.lib")
#endif
void  LOG_WRITER_EXPORT CreateNMLogWriter();
void  LOG_WRITER_EXPORT	ReleaseNMLogWriter();

static void LoadNmLoger();
//void AFX_EXT_CLASS WriteLog(WCHAR* szModuleName,LONG  lLogLevel, CString strLog);
void LOG_WRITER_EXPORT WriteLog(WCHAR* szModuleName,LONG  lLogLevel,const WCHAR *fmt, ...);
void LOG_WRITER_EXPORT WriteLog(char* szModuleName,LONG  lLogLevel,const char *szLog);


#endif // !defined(AFX_NMTRACELOG_H__FFE9683E_18EF_43CC_8A7A_705386717199__INCLUDED_)
