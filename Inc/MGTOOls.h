#ifndef MG_TOOLS_H
#define MG_TOOLS_H
#pragma once



#define MSV_INGEST_CFG_NAME											_T("MgCaptureCfg.ini")
#define MSV_PUBLIC_CFG_NAME											_T("Public.ini")
#define MSV_TEMP_CFG_NAME											 _T("MSVCFG.CFG")
#define MSV_FILE_ENCODE_TYPE_TABLE_CFG_NAME			_T("FileEncodeTypeTable.CFG")

#define LOGNAME _T("TS")

#ifndef _LOG_WRITER_EXPORT_DEFINE_
#include "NMTraceLog.h"
#pragma comment(lib,"MGLogWriterU.lib")
#endif

static TCHAR m_strMgConfigFile_Absolute_Path[MAX_PATH]={'0'};
inline LPCTSTR GetMgConfigFileName()
{	
	if(m_strMgConfigFile_Absolute_Path[0]=='0')
	{
		TCHAR strModuleFileName[MAX_PATH];
		TCHAR m_strModulePath[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		GetModuleFileName(NULL,strModuleFileName,MAX_PATH);

#ifdef _UNICODE
		_tsplitpath_s(strModuleFileName, m_strModulePath,_countof(m_strModulePath), strPath,_countof(strPath),NULL,0,NULL,0);
		_tcscat_s(m_strModulePath,_countof(m_strModulePath),strPath);  
		_tcscpy(m_strMgConfigFile_Absolute_Path,m_strModulePath); 
		_tcscat_s(m_strMgConfigFile_Absolute_Path,_countof(m_strMgConfigFile_Absolute_Path),_T("MgCaptureCfg.ini"));
#else
		_splitpath_s(strModuleFileName, m_strModulePath,_countof(m_strModulePath), strPath,_countof(strPath),NULL,0,NULL,0);
		strcat_s(m_strModulePath,_countof(m_strModulePath),strPath);  
		strcpy(m_strMgConfigFile_Absolute_Path,m_strModulePath); 
		strcat_s(m_strMgConfigFile_Absolute_Path,_countof(m_strMgConfigFile_Absolute_Path),("MgCaptureCfg.ini"));

#endif
	}
	return m_strMgConfigFile_Absolute_Path;
}
inline int GetChannelID()
{
	return 0;
}

#endif //define MG_TOOLS_H