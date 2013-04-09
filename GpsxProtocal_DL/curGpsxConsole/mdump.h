#ifndef C_MINIDUMPER_H
#define  C_MINIDUMPER_H
#pragma once

#include "dbghelp.h"
struct _EXCEPTION_POINTERS;

class CMiniDumper
{
public:
	static void SetExceptionFilter(	IN MINIDUMP_TYPE	dmpType			= MiniDumpNormal,
									IN DWORD			dwDmpFolderSize	= 2*1024*1024,//设置存放Dmp的文件夹的大小(KB)，防止dmp过多，默认为2G
									IN DWORD			dwDmpKeepDay	= 7);	//设置自动删除距离当前日期多久的Dmp，默认为7天

private:
	static	BOOL DeleteOldDMP(CString strDir);
	static	BOOL CheckDmpCapacity(CString strPath);
	static	BOOL MakeDir(CString strDir);
	static	BOOL FindDir(CString strDir);
	static HMODULE GetDebugHelperDll(FARPROC* ppfnMiniDumpWriteDump, bool bShowErrors);
	static LONG WINAPI DebugMiniDumpFilter(struct _EXCEPTION_POINTERS* pExceptionInfo);
	static void GetDmpFilePath(TCHAR *pFileName);
	static void CallStackWalker(struct _EXCEPTION_POINTERS* pExceptionInfo,const TCHAR *pStrFileName);
public:

};
//extern CMiniDumper theCrashDumper;

#endif C_MINIDUMPER_H