// GpsxConsole.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GpsxConsole.h"
#include "GpsxConsoleDlg.h"
#include "mdump.h"
#include "io.h"
#include "MGTOOls.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGpsxConsoleApp

BEGIN_MESSAGE_MAP(CGpsxConsoleApp, CWinApp)
	//{{AFX_MSG_MAP(CGpsxConsoleApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpsxConsoleApp construction

CGpsxConsoleApp::CGpsxConsoleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGpsxConsoleApp object

CGpsxConsoleApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGpsxConsoleApp initialization


#include "Psapi.h"
#include <tlhelp32.h>
#pragma  comment(lib,"Psapi.lib")
BOOL _IsProcessExist(CString strProPath)
{
	TCHAR buff[MAX_PATH];
	CString strTempPath;
	HANDLE hProcess = NULL;
	HANDLE hfindhandle=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32* info=new PROCESSENTRY32;
	memset(buff,0,MAX_PATH);
	info->dwSize=sizeof(PROCESSENTRY32);
	strProPath.MakeLower();
	if(Process32First(hfindhandle,info))        
	{
		hProcess=OpenProcess(PROCESS_QUERY_INFORMATION   |   PROCESS_VM_READ,FALSE,info->th32ProcessID);   
		if(hProcess != NULL)  
		{
			GetModuleFileNameEx(hProcess,(HMODULE)info->th32ModuleID,buff,MAX_PATH);
			strTempPath.Format(_T("%s"),buff);
			strTempPath.MakeLower();
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		if((strProPath == strTempPath))	// && (dwProcessID == info->th32ProcessID)
		{			
			CloseHandle(hfindhandle);
			delete info;
			return TRUE;
		}		
		while(Process32Next(hfindhandle,info)!=FALSE)
		{
			hProcess=OpenProcess(PROCESS_QUERY_INFORMATION   |   PROCESS_VM_READ,FALSE,info->th32ProcessID);   
			if(hProcess != NULL)  
			{
				GetModuleFileNameEx(hProcess,(HMODULE)info->th32ModuleID,buff,MAX_PATH);
				strTempPath.Format(_T("%s"),buff);
				strTempPath.MakeLower();
				CloseHandle(hProcess);
				hProcess = NULL;
			}			
			if((strProPath == strTempPath))	// 
			{	
				CloseHandle(hfindhandle);
				delete info;
				return TRUE;
			}
		}			
	}
	CloseHandle(hfindhandle);
	delete info;
	return FALSE;
}
BOOL _CreateProcess(CString strProcess,PROCESS_INFORMATION &pi,BOOL bService,CString strCmdLine)
{
	if (strProcess == _T(""))
	{
		return FALSE;
	}
	CString strProName(strProcess);
	int nPos = strProcess.ReverseFind('\\');
	if(nPos >= 0)
	{
		strProName = strProcess.Right(strProcess.GetLength()-nPos-1);
	}
	STARTUPINFO si;
	CString strInfo;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );				

	if (!bService)
	{
		if(!CreateProcess(NULL, // No module name (use command line). 
			strProcess.GetBuffer(0),				// Command line. 
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			FALSE,            // Set handle inheritance to FALSE. 
			0,                // No creation flags. 
			NULL,             // Use parent's environment block. 
			NULL,             // Use parent's starting directory. 
			&si,              // Pointer to STARTUPINFO structure.
			&pi ))             // Pointer to PROCESS_INFORMATION structure.
		{
			strInfo.Format(_T("Create Process(%s) Failed!"),strProName);
			strInfo.Format(_T("Create Process(%s) Failed!(strCmdLine:%s) %d"),strProName,strCmdLine,GetLastError());
			//WriteLog(_T("Channel_Control_Serive"),logLevelError,strInfo);	
			AfxMessageBox(strInfo);
			return FALSE;
		}
		strInfo.Format(_T("Create Process(%s_%d) Success!"),strProName,pi.dwProcessId);
		//_AddTrace(strInfo);
		strInfo.Format(_T("Create Process(%s_%d) Success!(strCmdLine:%s)"),strProName,pi.dwProcessId,strCmdLine);
		//WriteLog(_T("Channel_Control_Serive"),logLevelInfo,strInfo);
		return TRUE;
	}
	else		//---------Start Service-----------08-12-16 by YL--------------------------
	{
		/*
		CString strServiceName;
		strServiceName = strProName.Left(strProName.GetLength() - 4);

		SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);		
		if (hSCM != NULL)
		{
			SC_HANDLE hService = OpenService(hSCM, strServiceName, SERVICE_ALL_ACCESS);
			if (hService == NULL)
			{
				hService = ::CreateService(
					hSCM, strServiceName, strServiceName,
					SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
					SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
					strProcess, NULL, NULL, NULL, NULL, NULL);
			}
			if (hService == NULL)
			{
				CloseServiceHandle(hSCM);
				strInfo.Format(_T("Create Service(%s) Failed!"),strServiceName);
				//_AddTrace(strInfo);
				//WriteLog(_T("Channel_Control_Serive"),logLevelError,strInfo);	
				return FALSE;
			}
			if(!StartService(hService,NULL,NULL))
			{
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCM);
				strInfo.Format(_T("Start Service(%s) Failed!"),strServiceName);
				//_AddTrace(strInfo);
				//WriteLog(_T("Channel_Control_Serive"),logLevelError,strInfo);	
				return FALSE;
			}
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			strInfo.Format(_T("Start Service(%s) Success!"),strServiceName);
			//_AddTrace(strInfo);
			//WriteLog(_T("Channel_Control_Serive"),logLevelInfo,strInfo);
			return TRUE;
		}
		else
		{
			strInfo.Format(_T("OpenSCManager Failed when start Service(%s)!"),strServiceName);
			//_AddTrace(strInfo);
			//WriteLog(_T("Channel_Control_Serive"),logLevelError,strInfo);	
			return FALSE;
		}
		/**/
	}		 

}
BOOL CGpsxConsoleApp::InitInstance()
{
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	int nListenPort=GetPrivateProfileInt(_T("GPSSet"),_T("listenPort"),110,GetMgConfigFileName());

	CString strTmp;
	strTmp.Format(_T("GPSXCONSOLE_H__BA472566_78AA_%d"),nListenPort);
	if(OpenMutex(MUTEX_ALL_ACCESS,FALSE,strTmp))
	{
		return FALSE;
	}
	else
	{
		CreateMutex(NULL,FALSE,strTmp);
	}
// 	IDumper *pDumper = CreateDumper();
// 	if(pDumper)
// 		pDumper->SetExceptionFilter();

	//自动抓取错误dump
	CMiniDumper::SetExceptionFilter(MiniDumpWithFullMemory);

	CString strLogServer("sonaps.logger.service.exe");
	/*
	if(!_IsProcessExist(strLogServer)){
		PROCESS_INFORMATION pi;
		_CreateProcess(strLogServer,pi,FALSE,"");
	}/**/
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CGpsxConsoleDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
