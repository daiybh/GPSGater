#include "StdAfx.h"
#include "MGTOOls.h"
#include "MG_ToolsImp.h"

#include <shlwapi.h>


typedef long long           int64_t; 
typedef unsigned long long uint64_t; 
#include <Psapi.h>
#pragma comment(lib,"psapi.lib")

CMGTOOls g_MG_TOOLs;

namespace MSV_BASE_TOOLS
{
inline int GetConfig_Int(  LPCTSTR lpAppName,  LPCTSTR lpKeyName,  INT nDefault,  LPCTSTR lpFileName )
{		
	return g_MG_TOOLs.GetConfig_Int(lpAppName,lpKeyName,nDefault,lpFileName);
}
inline DWORD GetConfig_String(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpDefault,LPTSTR lpReturnedString,DWORD nSize,LPCTSTR lpFileName		)
{		
	return g_MG_TOOLs.GetConfig_String(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
}

inline int WriteConfig_Int(LPCTSTR lpAppName,  LPCTSTR lpKeyName,  const int nValue,  LPCTSTR lpFileName )
{
	return g_MG_TOOLs.WriteConfig_Int(lpAppName,lpKeyName,nValue,lpFileName);
}

inline BOOL WriteConfig_String(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpString,LPCTSTR lpFileName)
{
	return g_MG_TOOLs.WriteConfig_String(lpAppName,lpKeyName,lpString,lpFileName);
}


inline UINT GetPrivateProfileInt_From_Global( __in LPCTSTR lpAppName, __in LPCTSTR lpKeyName, __in INT nDefault, __in_opt LPCTSTR lpFileName)
{
	CString strFileName(lpFileName);
	PathRemoveFileSpec(strFileName.GetBuffer(MAX_PATH));
	strFileName.ReleaseBuffer();
	strFileName += _T( "\\..\\config\\global.ini ");
	return GetConfig_Int(lpAppName,lpKeyName,nDefault,strFileName);
}

inline DWORD GetPrivateProfileString_From_Global( __in_opt LPCTSTR lpAppName, __in_opt LPCTSTR lpKeyName,
										__in_opt LPCTSTR lpDefault, __out LPTSTR lpReturnedString,
										__in DWORD nSize, __in_opt LPCTSTR lpFileName )
{
	CString strFileName(lpFileName);
	PathRemoveFileSpec(strFileName.GetBuffer(MAX_PATH));
	strFileName.ReleaseBuffer();
	strFileName += _T( "\\..\\config\\global.ini ");
	return GetConfig_String(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,strFileName);
}
//////////////////////////////////////////////////////////////////////////
// 获取通道
//////////////////////////////////////////////////////////////////////////
inline int GetChannelID()
{
	return g_MG_TOOLs.GetCurChannelID();
	return GetConfig_Int(_T("BASE_CONFIGURATION"),_T("ChannelID"),VERSION_4211P,MSV_INGEST_CFG_NAME);
}
//////////////////////////////////////////////////////////////////////////
//获取高清模式还是 标清模式
//////////////////////////////////////////////////////////////////////////
// 	inline int GetVideoSize()
// 	{
// 		return GetConfig_Int(_T("DEFAULT_CONFIGRATION"),_T("VideoSize"),2,MSV_INGEST_CFG_NAME);
// 	}
//////////////////////////////////////////////////////////////////////////
//获取控制端口
//////////////////////////////////////////////////////////////////////////
inline int GetCtrlPort()
{
	TCHAR strKey[20];
	_stprintf(strKey,_T("Ingest_Port%d"),GetChannelID());
	return GetConfig_Int(_T("USERS_CONFIGURATION"),strKey,3100,MSV_PUBLIC_CFG_NAME);
}

//////////////////////////////////////////////////////////////////////////
//获取解释性配置的后缀
//////////////////////////////////////////////////////////////////////////
inline LPCTSTR GetMSVVersionStr()
{
	if(GetMSVVersionID()==VERSION_4211P)
		return _T("_4211");
	else
		return _T("_555");
}
void makeUpper(TCHAR *pChar)
{
	g_MG_TOOLs.makeUpper(pChar);
}

inline MSV_VERSION GetMSVVersionID()
{
	TCHAR strVersion[255];
	GetConfig_String(_T("PUBLIC_CONFIGURATION"),_T("version"),VERSION_STR_4211P,strVersion,255,MSV_PUBLIC_CFG_NAME);
	makeUpper(strVersion);
	if(_tcsstr(strVersion,VERSION_STR_4211P)==strVersion)
		return VERSION_4211P;
	else if(_tcsstr(strVersion,VERSION_STR_555)==strVersion)
		return VERSION_555;
	return VERSION_4211P;
}



LPCTSTR GetMSVPublicCfgName(){return MSV_PUBLIC_CFG_NAME;}

LPCTSTR GetMgConfigFileName()
{
	return g_MG_TOOLs.Get_MgConfigFileName();	
}
inline int get_memory_usage(uint64_t* mem, uint64_t* vmem) 
{ 
	PROCESS_MEMORY_COUNTERS pmc; 
	if(GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) 
	{ 
		if(mem) *mem = pmc.WorkingSetSize; 
		if(vmem) *vmem = pmc.PagefileUsage; 
		return 0; 
	} 
	return -1; 
} 
inline void getMemroy2log(LPCTSTR functionName)
{
	uint64_t mem=0,vmem=0;
	get_memory_usage(&mem,&vmem);
	CString s;
	s.Format(L"memory--mem:%I64d,,vMem:%I64d-------%s",mem,vmem,functionName);
	//WriteLog(L"memLog",logLevelError,s);
}


}