// NMTraceLog.cpp: implementation of the CNMTraceLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NMTraceLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 定义函数入口全局变量
static VOID (*g_pfn)(WCHAR* szModuleName,LONG  lLogLevel, WCHAR *fmt);
static BOOL (*g_pfnNMInit)();

HINSTANCE g_hinstPlugLib = NULL;
long g_lNmRefCount = 0;

WCHAR cChannel[4];
void CreateNMLogWriter()
{
	InterlockedIncrement(&g_lNmRefCount);
	if(g_hinstPlugLib == NULL)
	{
		LoadNmLoger();
	}
	return;
}

void ReleaseNMLogWriter()
{
	InterlockedDecrement(&g_lNmRefCount);
	if(g_lNmRefCount <= 0)
	{
		if(NULL != g_hinstPlugLib)
		{
			FreeLibrary(g_hinstPlugLib);
			g_hinstPlugLib = NULL;
		}
		g_lNmRefCount = 0;
	}
}


void LoadNmLoger()
{
	//初始化代码, 
	g_hinstPlugLib = LoadLibrary(_T("sonaps.logger.client.dll"));
	if (g_hinstPlugLib == NULL)
	{
		AfxMessageBox(_T("LoadLibrary(sonaps.logger.client.dll) Failed!"));
		return;
	}
	
	const char szFn[] = "NMTrace0";
	const char szNMInit[] = "NMInit";
	*(FARPROC*)&g_pfn = GetProcAddress(g_hinstPlugLib, szFn);
	*(FARPROC*)&g_pfnNMInit = GetProcAddress(g_hinstPlugLib, szNMInit);

	if (g_pfnNMInit != NULL)
	{
		g_pfnNMInit();
	}
	//通过配置文件获取channel ID
	TCHAR szPath[MAX_PATH];
	memset(szPath,0X00,MAX_PATH);
	GetModuleFileName(NULL,szPath,MAX_PATH);
	CString mfiledir;
	mfiledir = szPath;
	int nIndex = mfiledir.ReverseFind(_T('\\'));
	mfiledir = mfiledir.Left(nIndex+1);
	int	m_nChannel = GetPrivateProfileInt(_T("GPSSet"),_T("listenPort"),110, GetMgConfigFileName());
	{	
		m_nChannel =m_nChannel%100;
		cChannel[0] ='_';
		cChannel[1] = (m_nChannel/10)+48;	
		cChannel[2] =m_nChannel%10+48;
		cChannel[3] ='\0';
	}
	
}

void WriteLog(char* szModuleName,LONG  lLogLevel,const char *szLog)
{
	if(g_pfn !=NULL)
	{
		WCHAR szW_ModuleName[50];
		WCHAR szW_Log[200];

		MultiByteToWideChar(CP_ACP,0,szModuleName,-1,szW_ModuleName,50);		
		MultiByteToWideChar(CP_ACP,0,szLog,-1,szW_Log,200);
		WriteLog(szW_ModuleName,lLogLevel,szW_Log);
	}
}
//void WriteLog(WCHAR* szModuleName,LONG  lLogLevel, CString strLog)
void WriteLog(WCHAR* szModuleName,LONG  lLogLevel,const WCHAR *fmt, ...)
{
	if (g_pfn != NULL)
	{
		va_list marker;
		WCHAR szBuffer[102400]; 
		va_start( marker, fmt);
		_vsnwprintf(szBuffer, 102400, fmt, marker); 
		
		va_end( marker);
		WCHAR gSzModuleName[MAX_PATH];
		wcscpy(gSzModuleName,szModuleName);
		wcscat(gSzModuleName,cChannel);
		try{
			g_pfn(gSzModuleName, lLogLevel, szBuffer);
		}
		catch(...)		//当字符串里包含“%S”这样的转义字符时有可能会异常
		{

		}
	}
}