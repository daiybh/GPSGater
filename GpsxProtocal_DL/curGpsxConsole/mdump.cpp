#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
typedef unsigned long ULONG_PTR,*PULONG_PTR; 
//#define __out_ecount_opt(x)
#include "mdump.h"
#include "StackWalker.h"

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

//CMiniDumper theCrashDumper;
#define ARRSIZE(x)	(sizeof(x)/sizeof(x[0]))

static	MINIDUMP_TYPE	g_dwDmpType;
static  ULONGLONG		g_ullDmpFolderSize;
static	DWORD			g_dwDmpKeepDay;

void CMiniDumper::SetExceptionFilter(IN MINIDUMP_TYPE dmpType /* = MiniDumpNormal */, IN DWORD dwDmpFolderSize /* = 2*1024*1024 */, IN DWORD dwDmpKeepDay /* = 7 */)
{
	g_dwDmpType = dmpType;	
	g_ullDmpFolderSize = dwDmpFolderSize;
	g_dwDmpKeepDay = dwDmpKeepDay;
	
	// if this assert fires then you have two instances of CMiniDumper which is not allowed
	MINIDUMPWRITEDUMP pfnMiniDumpWriteDump = NULL;
	HMODULE hDbgHelpDll = GetDebugHelperDll((FARPROC*)&pfnMiniDumpWriteDump, false);
	if (hDbgHelpDll)
	{
		if (pfnMiniDumpWriteDump)
			SetUnhandledExceptionFilter(DebugMiniDumpFilter);
		FreeLibrary(hDbgHelpDll);
		hDbgHelpDll = NULL;
		pfnMiniDumpWriteDump = NULL;
	}
}

HMODULE CMiniDumper::GetDebugHelperDll(FARPROC* ppfnMiniDumpWriteDump, bool bShowErrors)
{
	*ppfnMiniDumpWriteDump = NULL;
	HMODULE hDll = LoadLibrary(_T("DBGHELP.DLL"));
	if (hDll == NULL)
	{
		if (bShowErrors) {
			// Do *NOT* localize that string (in fact, do not use MFC to load it)!
		//	MessageBox(NULL, _T("DBGHELP.DLL not found. Please install a DBGHELP.DLL."), m_szAppName, MB_ICONSTOP | MB_OK);
		}
	}
	else
	{
		*ppfnMiniDumpWriteDump = GetProcAddress(hDll, "MiniDumpWriteDump");
		if (*ppfnMiniDumpWriteDump == NULL)
		{
			if (bShowErrors) {
				// Do *NOT* localize that string (in fact, do not use MFC to load it)!
			//	MessageBox(NULL, _T("DBGHELP.DLL found is too old. Please upgrade to a newer version of DBGHELP.DLL."), m_szAppName, MB_ICONSTOP | MB_OK);
			}
		}
	}
	return hDll;
}
void CMiniDumper::GetDmpFilePath(TCHAR *pFileName)
{
	SYSTEMTIME curTime;
	GetLocalTime(&curTime);

	TCHAR szExefilePath[MAX_PATH];
	TCHAR szExeName[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	GetModuleFileName(NULL, szExefilePath, ARRSIZE(szExefilePath));	//path of current process
	_tsplitpath_s(szExefilePath, strDriver,_countof(strDriver), strPath,_countof(strPath),szExeName,_countof(szExeName),NULL,0);
	memset(pFileName,0,MAX_PATH*sizeof(TCHAR));
	_tcscat_s(pFileName, _MAX_PATH,strDriver);
	_tcscat_s(pFileName, _MAX_PATH,strPath);
	_tcscat_s(pFileName, _MAX_PATH,_T("Dumps\\"));
	MakeDir(pFileName);
	DeleteOldDMP(pFileName);//删除N天以前的dmp，防止把磁盘空间写爆了
	CheckDmpCapacity(pFileName);
	_tcscat_s(pFileName,_MAX_PATH,szExeName);
	
	TCHAR szTimestr[_MAX_PATH] ={0};


	_sntprintf_s(szTimestr ,_countof(szTimestr),ARRSIZE(szTimestr),_T("_%d_%.2d_%.2d_%.2d_%.2d_%.2d_%.2d_%.2d.dmp") ,															
				GetCurrentProcessId(),
		curTime.wYear,
		curTime.wMonth,
		curTime.wDay,
		curTime.wHour,
		curTime.wMinute,
		curTime.wSecond,
		curTime.wMilliseconds);
	_tcscat_s(pFileName,_MAX_PATH,szTimestr);	
}
void CMiniDumper::CallStackWalker(struct _EXCEPTION_POINTERS* pExceptionInfo,const TCHAR *pStrFileName)
{
	MyStackWalker sw(pStrFileName);
	sw.ShowCallstack(GetCurrentThread(), pExceptionInfo->ContextRecord, NULL, pExceptionInfo);
	
	sw.~MyStackWalker();
}
LONG CMiniDumper::DebugMiniDumpFilter(struct _EXCEPTION_POINTERS* pExceptionInfo)
{
	TCHAR szDumpPath[_MAX_PATH] = {0};
	GetDmpFilePath(szDumpPath);
	CallStackWalker(pExceptionInfo,szDumpPath);
	LONG lRetValue = EXCEPTION_CONTINUE_SEARCH;
	TCHAR szResult[_MAX_PATH + 1024] = {0};
	MINIDUMPWRITEDUMP pfnMiniDumpWriteDump = NULL;
	HMODULE hDll = GetDebugHelperDll((FARPROC*)&pfnMiniDumpWriteDump, false);
	if (hDll)
	{
		if (pfnMiniDumpWriteDump)
		{			
			// save a dump file
			// Do *NOT* localize that string (in fact, do not use MFC to load it)!
			{
				// Create full path for DUMP file

				//_tcsncat(szDumpPath, _T(".dmp"), ARRSIZE(szDumpPath) - 1);
				HANDLE hFile = CreateFile(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo = {0};
					ExInfo.ThreadId = GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

				//	BOOL bOK = (*pfnMiniDumpWriteDump)(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
					BOOL bOK = (*pfnMiniDumpWriteDump)(GetCurrentProcess(), GetCurrentProcessId(), hFile, g_dwDmpType, &ExInfo, NULL, NULL);
					if (bOK)
					{
					//	_sntprintf(szResult, ARRSIZE(szResult), _T("Saved dump file to \"%s\".\r\n\r\nPlease send this file together with a detailed bug report to dumps@emule-project.net !\r\n\r\nThank you for helping to improve eMule."), szDumpPath);
						lRetValue = EXCEPTION_EXECUTE_HANDLER;						
					}
					else
					{
					//	_sntprintf(szResult, ARRSIZE(szResult), _T("Failed to save dump file to \"%s\".\r\n\r\nError: %u"), szDumpPath, GetLastError());
					}
					CloseHandle(hFile);
				}
				else
				{
					// Do *NOT* localize that string (in fact, do not use MFC to load it)!
					_sntprintf_s(szResult, _countof(szResult),ARRSIZE(szResult), _T("Failed to create dump file \"%s\".\r\n\r\nError: %u"), szDumpPath, GetLastError());					
				}
			}
		}
		FreeLibrary(hDll);
		hDll = NULL;
		pfnMiniDumpWriteDump = NULL;
	}

	ExitProcess(0);
	return lRetValue;
}

///查找某路径是否存在，如果存在，则不做任何操作；如果不存在，创建此目录
BOOL CMiniDumper::MakeDir(CString strDir)
{
	if(strDir.IsEmpty())
		return FALSE;
	if (strDir[strDir.GetLength()-1] != _T('\\'))
		strDir += _T("\\");

	CString strDirTemp;
	int nStart = 0;	

	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	_tsplitpath_s(strDir, strDriver,_countof(strDriver), strPath,_countof(strPath), NULL,0, NULL,0);
	strDirTemp = strDriver;
	if (strDirTemp == _T(""))
		return FALSE;

	while(nStart != -1)
	{
		if(nStart != 0)
		{
			strDirTemp = strDir.Left(nStart);
			if (!FindDir(strDirTemp))
			{
				try
				{					
					if (!::CreateDirectory(strDirTemp, NULL))
					{						
						return FALSE;
					}
				}				
				catch (...)
				{					
					return FALSE;
				}
			}			
			nStart++;
		}
		nStart = strDir.Find(_T("\\"), nStart);
	}
	return TRUE;
}
#include <io.h>
//查找某路径是否存在
BOOL CMiniDumper::FindDir(CString strDir)
{
	if (_taccess((LPCTSTR)strDir, 0) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CMiniDumper::DeleteOldDMP(CString strDir)
{
	if (strDir == _T(""))
	{
		return FALSE;
	}
	CFileFind finder;
	CString strDMPName;
	strDMPName = strDir + _T("*.dmp");
	BOOL bOK = FALSE;
	CTime tmDMP;
	CTimeSpan tmSpan(g_dwDmpKeepDay,0,0,0);
	CTime tmNow = CTime::GetCurrentTime();

	bOK = finder.FindFile(strDMPName);
	while(bOK)
	{
		bOK = finder.FindNextFile();

		//finder.GetCreationTime(tmDMP);
        finder.GetLastWriteTime(tmDMP);
		if ((tmNow - tmDMP) > tmSpan)
		{
			DeleteFile(finder.GetFilePath());
		}
	}
	finder.Close();

	bOK = FALSE;
	strDMPName = strDir + _T("*.rpt");
	bOK = finder.FindFile(strDMPName);
	while(bOK)
	{
		bOK = finder.FindNextFile();

		finder.GetLastWriteTime(tmDMP);
		if ((tmNow - tmDMP) > tmSpan)
		{
			DeleteFile(finder.GetFilePath());
		}
	}
	finder.Close();

	return TRUE;
}

BOOL CMiniDumper::CheckDmpCapacity(CString strPath)
{
	struct strDmpAttribute
	{		
		ULONGLONG	dwDmpFileSize;//in KB
		CTime CreationTime;
		CString strDmpFilePath;
	};
	CArray<strDmpAttribute,strDmpAttribute> DmpFileArray;

	CString strDmpPath(strPath);
	strDmpPath += _T("*.dmp");
	CTime tmNow;
	CTimeSpan tmSpan;

	ULONGLONG ullAllDmpSize = 0;
	BOOL bInsert = FALSE;
	CFileFind FileFind;
	BOOL bOK = FALSE;
	bOK = FileFind.FindFile(strDmpPath);
	while(bOK)
	{
		bOK = FileFind.FindNextFile();

		strDmpAttribute dmpCell;
		dmpCell.strDmpFilePath = FileFind.GetFilePath();
		FileFind.GetCreationTime(dmpCell.CreationTime);
		dmpCell.dwDmpFileSize = (FileFind.GetLength() / 1024);//in KB

		tmNow = CTime::GetCurrentTime();
		tmSpan = tmNow - dmpCell.CreationTime;
		if (tmSpan.GetTotalSeconds() < 2)
		{
			continue;
		}

		ullAllDmpSize += dmpCell.dwDmpFileSize;

		for(int m=0;m<DmpFileArray.GetSize();m++)
		{
			if(dmpCell.CreationTime < DmpFileArray[m].CreationTime)	
			{
				DmpFileArray.InsertAt(m,dmpCell);
				bInsert = TRUE;
				break;
			}
		}
		if(!bInsert)
			DmpFileArray.Add(dmpCell);
	}
	FileFind.Close();

	while (ullAllDmpSize >= g_ullDmpFolderSize )//如果当前文件夹大小以及超过设置值了，就删除最旧的一个dmp
	{
		if(DmpFileArray.GetSize()<1)	//队列为空
		{
			return FALSE;
		}
		else
		{			
			if (ullAllDmpSize <= DmpFileArray[0].dwDmpFileSize)
			{
				ullAllDmpSize = 0;
			}
			else
			{
				ullAllDmpSize -= DmpFileArray[0].dwDmpFileSize;
			}
			
			DeleteFile(DmpFileArray[0].strDmpFilePath);				
			DmpFileArray.RemoveAt(0);
		}
	}

	return TRUE;
}