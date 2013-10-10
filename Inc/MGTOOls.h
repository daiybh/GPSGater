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
inline void GetPrivateProfileStringA_(
							   __in_opt LPCSTR lpAppName,
							   __in_opt LPCSTR lpKeyName,
							   __in_opt LPCSTR lpDefault,
							   __out_ecount_part_opt(nSize, return + 1) LPSTR lpReturnedString,
							   __in     DWORD nSize,
							   __in_opt LPCSTR lpFileName)
{
	int nRet = ::GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
	printf("GetPricStr:%d-lpAppName:%s,lpKeyName:%s-Default:%s--return:%s---last:%d\r\n",nRet,lpAppName,lpKeyName,lpDefault,lpReturnedString,GetLastError());
	if(GetLastError()!=0)
		WritePrivateProfileString(lpAppName,lpKeyName,lpDefault,lpFileName);

}
inline int GetPrivateProfileIntA_(
					  __in     LPCSTR lpAppName,
					  __in     LPCSTR lpKeyName,
					  __in     INT nDefault,
					  __in_opt LPCSTR lpFileName
					  )
{
	char sValue[10];
	sprintf(sValue,"%d",nDefault);

	int nRet =::GetPrivateProfileInt(lpAppName,lpKeyName,nDefault,lpFileName);

	printf("GetPricInt:%d-lpAppName:%s,lpKeyName:%s--last:%d\r\n",nRet,lpAppName,lpKeyName,GetLastError());
	if(GetLastError()!=0)
		WritePrivateProfileString(lpAppName,lpKeyName,sValue,lpFileName);
	return nRet;
}
inline int GetChannelID()
{
	return 0;
}
#define GetPrivateProfileInt  GetPrivateProfileIntA_
#define GetPrivateProfileString GetPrivateProfileStringA_
#endif //define MG_TOOLS_H