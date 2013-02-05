// LogWriter.cpp: implementation of the CLogWriter class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LogWriter.h"
#include "atlconv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////

CLogWriter* g_pObject = NULL;
long g_nRefCount = 0;

LPVOID CreateLogWriter(CString strLogPathName)
{
	InterlockedIncrement(&g_nRefCount);
	if(NULL == g_pObject)
	{
		g_pObject = new CLogWriter;
		g_pObject->InitPath(strLogPathName);
	}
	return g_pObject;
}

void ReleaseLogWriter()
{
	InterlockedDecrement(&g_nRefCount);
	if(g_nRefCount <= 0)
	{
		if(NULL != g_pObject)
		{
			delete g_pObject;
			g_pObject = NULL;
		}
		g_nRefCount = 0;
	}
}

CLogWriter::CLogWriter()
{
	m_bOpen = FALSE;
	m_bInitPath = FALSE;
	m_tmOldTime = CTime::GetCurrentTime();
}


CLogWriter::~CLogWriter()
{
    if(m_bOpen)
	{
		try
		{
			if(m_bOpen)
				m_LogFile.Close();
		}
		catch(...)
		{
		}
	}
}

//WORD bTypeLog      运行日志0,错误日至1,调试日志2,警告日志3
//LPCTSTR strErrorMessage 错误信息描述
//LPCTSTR strFunName 出现错误的函数名
//int ErrorCode      错误信息吗,默认为-1
BOOL CLogWriter::WriteMGLog(WORD bTypeLog, LPCTSTR strErrorMessage,LPCTSTR strFunName,int ErrorCode)
{
	if(!m_bInitPath)
		return FALSE;
	//将错误字符串截断
	CString     strTempMessage = strErrorMessage;
	if(strTempMessage.GetLength()>254)
		strTempMessage = strTempMessage.Left(254);
    
	LPCTSTR   strMessage = (LPCTSTR)strTempMessage;

	CString		strFileName;
	SYSTEMTIME  tmSys;
	::GetLocalTime(&tmSys);
	strFileName.Format(_T("%d%02d%02d_Log.txt"),tmSys.wYear,tmSys.wMonth,tmSys.wDay);
	strFileName = m_strFilePath+strFileName;
   
	CString strTime; 
	strTime.Format(_T("%d-%02d-%02d %2d:%2d:%2d.%3d"),tmSys.wYear,tmSys.wMonth,tmSys.wDay,
												tmSys.wHour,tmSys.wMinute,tmSys.wSecond,tmSys.wMilliseconds);
	
	CString strText;
	CString strLogType;
	switch(bTypeLog)
	{
	case LOG_NORMAL:
		strLogType =  _T("RunLog  ");
  	   break;
   case LOG_ERROR:
	   strLogType =   _T("ErrorLog");
       break;
    case LOG_DEBUG:
	   strLogType =   _T("DebugLog");
#ifndef TZ_LOG_DEBUG
       return TRUE;
#endif
      
    case LOG_WARNING:
		strLogType =  _T("WarnLog ");
       break;

	default:	break;
	}

	char szBuffer[1024];
	ZeroMemory(szBuffer,sizeof(char)*1024);
#ifdef _UNICODE
	USES_CONVERSION;
	sprintf_s(szBuffer,_countof(szBuffer),"%s * %s * %s * %s \r\n",W2A(strTime),W2A(strLogType),W2A(strMessage),
												   W2A(strFunName?strFunName:_T("")));
#else
	sprintf(szBuffer,"%s * %s * %s * %s \r\n",strTime,strLogType,strMessage,
												 strFunName?strFunName:_T(""));
#endif
  
	m_csWriteLog.Lock();
	CFileException e;
	try
	{
		if(!m_bOpen)
		{
			m_LogFile.Open(strFileName,CFile::modeNoTruncate|CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone,&e);
			//DWORD erro=::GetLastError();
			m_bOpen = TRUE;
		}else
		{
			if(_IsDayChange())
			{
				m_LogFile.Close();
				m_LogFile.Open(strFileName,CFile::modeNoTruncate|CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone,&e);
				m_bOpen = TRUE;
			}
		}
		
		m_LogFile.SeekToEnd();
		m_LogFile.Write(szBuffer,strlen(szBuffer));
		
	}
	//catch(CFileException ex)//如果写日志文件出错了，怎么通知上层
	catch(...)//如果写日志文件出错了，怎么通知上层
	{
		//CFileException ex;
		//TCHAR   szCause[255];
		//ex.GetErrorMessage(szCause, 255);
		//CFileException ex;
		//TCHAR   szCause[255];
		//ex.GetErrorMessage(szCause, 255);
		CString strLog;
		int iErr = GetLastError();
		if (iErr == 112)
		{
			strLog = _T("写日志文件出错, 磁盘空间不足");
		}
		else
		{
			strLog.Format(_T("写日志文件出错, GetLastError()=%d"), iErr);
		}
		AfxMessageBox(strLog);

		m_csWriteLog.Unlock();
		return FALSE;
	}
	m_csWriteLog.Unlock();

	return TRUE;
}

void CLogWriter::InitPath(CString strLogPathName)
{
 	TCHAR szBuffer[256];
    ::GetModuleFileName(NULL,szBuffer,256);
	
	CString strPath = szBuffer;
	int nPos = strPath.ReverseFind('\\');
	if(nPos >= 0)
	{
		strPath = strPath.Left(nPos);
		strPath = strPath + _T("\\") + strLogPathName + _T("\\");
	}
	m_strFilePath = strPath;
	CreateDirectory(m_strFilePath,NULL);
	m_bInitPath = TRUE;

	_RemoveOutTimeLogFiles(); 
}

BOOL CLogWriter::_IsDayChange()
{
	CTime tmNow = CTime::GetCurrentTime();
    CTimeSpan ts = tmNow-m_tmOldTime;

	if(ts.GetDays() == 0)
	{
	   return FALSE;
	}

	m_tmOldTime  = tmNow;
   	return TRUE;
}

void CLogWriter::_RemoveOutTimeLogFiles() 
{
	CString		strFileName;
	SYSTEMTIME  tmCurrent,tmOneMonthAgo;
	::GetLocalTime(&tmCurrent);
	tmOneMonthAgo = tmCurrent;
	if(tmOneMonthAgo.wMonth == 1)
	{
		tmOneMonthAgo.wMonth = 12;
		tmOneMonthAgo.wYear--;
	}
	else
	{
		tmOneMonthAgo.wMonth--;
	}

	SYSTEMTIME tmDel = tmOneMonthAgo;
	for(int i= 0;i<50;i++)
	{
		if(tmDel.wDay == 1 )
		{
			tmDel.wDay = 31;
			if(tmDel.wMonth == 1)
			{
				tmDel.wMonth = 12;
				tmDel.wYear--;
			}
			else
			{
				tmDel.wMonth--;
			}
		}
		else
		{
			tmDel.wDay--;
		}
		strFileName.Format(_T("%d%02d%02d_Log.txt"),tmDel.wYear,tmDel.wMonth,tmDel.wDay);
		strFileName = m_strFilePath+strFileName;
		DeleteFile(strFileName);
	}
}