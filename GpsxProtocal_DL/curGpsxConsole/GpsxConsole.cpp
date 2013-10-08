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
BOOL _KillProcess(DWORD dwProcessID)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);
	if(hProcess==NULL)return FALSE;

	printf("_KillProcess----processID %d- handle [%d]\r\n",dwProcessID,hProcess);
	return TerminateProcess(hProcess,0);
}
/*
 *	@return dwProcessID
 */
DWORD _IsProcessExist(CString strProPath,BOOL bFullpath=TRUE)
{
	TCHAR buff[MAX_PATH];
	CString strTempPath;
	HANDLE hProcess = NULL;
	HANDLE hfindhandle=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32* info=new PROCESSENTRY32;
	memset(buff,0,MAX_PATH);
	info->dwSize=sizeof(PROCESSENTRY32);
	strProPath.MakeLower();
	DWORD dwProcessID=0;
	if(Process32First(hfindhandle,info))        
	{
		do 
		{
			hProcess=OpenProcess(PROCESS_QUERY_INFORMATION   |   PROCESS_VM_READ,FALSE,info->th32ProcessID);   
			if(hProcess != NULL)  
			{
				GetModuleFileNameEx(hProcess,(HMODULE)info->th32ModuleID,buff,MAX_PATH);
				strTempPath.Format(_T("%s"),buff);
				strTempPath.MakeLower();
				BOOL bFind = FALSE;
				if(bFullpath && (strProPath == strTempPath))
					bFind = TRUE;
				else if(!bFullpath)
				{
					bFind = (strTempPath.Find(strProPath)>-1);
				}
				if(bFind)	// && (dwProcessID == info->th32ProcessID)
				{			
					dwProcessID = info->th32ProcessID;
					CloseHandle(hProcess);
					hProcess = NULL;
					break;
				}
				CloseHandle(hProcess);
				hProcess = NULL;	
			}
		} while (Process32Next(hfindhandle,info));
	}
	CloseHandle(hfindhandle);
	delete info;
	return dwProcessID;
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
	CTime ti = CTime::GetCurrentTime();
	printf("%s Create Process [%s_%d] sucess\r\n",ti.Format(_T("%Y-%m-%d %H:%M:%S")),strProName,pi.dwProcessId);

	//_AddTrace(strInfo);
	strInfo.Format(_T("Create Process(%s_%d) Success!(strCmdLine:%s)"),strProName,pi.dwProcessId,strCmdLine);
	//WriteLog(_T("Channel_Control_Serive"),logLevelInfo,strInfo);
	return TRUE;
}
#include "io.h"
void openCommandWindow(){
	int hCrt;
	FILE *hf;
	AllocConsole();

	hCrt = _open_osfhandle(
		(long)GetStdHandle(STD_OUTPUT_HANDLE),
		0x4000);
	hf = _fdopen(hCrt,"w");
	*stdout =*hf;
	int i = setvbuf(stdout,NULL,_IONBF,0);

}
#include <conio.h>
int getch2(long delay_seconds){
	int keyCode = 0;
	for(int i=0;i<delay_seconds/100;i++){
		if(_kbhit()){
			keyCode = getch();
			printf("k--%d\r\n",keyCode);
			break;
		}
		//printf("%d",i);
		Sleep(100);
	}
	return keyCode;
}
BOOL CGpsxConsoleApp::InitInstance()
{
	AfxEnableControlContainer();
	openCommandWindow();
	TCHAR result[MAX_PATH];
	GetModuleFileName(NULL,result,MAX_PATH);//daemon
	CString strProcess;
	strProcess.Format(_T("%s"),result);
	strProcess.MakeLower();
	int nRet = strProcess.Find("daemon.exe");
	if(__argc==1 && nRet>0 )//带参数
	{
		printf("守护进程\r\nPress 'E' to exit\r\n");
		nRet = strProcess.Find("daemon");
		strProcess.Delete(nRet,6);
		HANDLE hProcess=NULL;
		DWORD dwProcessID=0;
		while(1){
			if((dwProcessID=_IsProcessExist(strProcess))==0){
				int nRet = DeleteFile(strProcess);
				if(nRet || GetLastError()==2){
					if(CopyFile(result,strProcess,FALSE)){
						PROCESS_INFORMATION pi;
						_CreateProcess(strProcess,pi,FALSE,"");
						hProcess = pi.hProcess;
						dwProcessID = pi.dwProcessId;
					}else{
					}
				}else{
				//	printf("deletefile %d,%d\r\n",nRet,GetLastError());
				}
				//printf("pi-->%d-->%d\r\n",hProcess,pi.dwProcessId);
			}else{
				//printf("IS-->%d\r\n",hProcess);
			}
			//Sleep(30*1000);
			if(getch2(3*1000) == 101){
				_KillProcess(dwProcessID);
				int i=0;
				do{
					Sleep(i*100);
					BOOL bRet = DeleteFile(strProcess);
					printf("%s---%d\r\n",strProcess,bRet);
				}while(i++<4);
				return 1;
			}
		}
	}

	CString strLogServer("sonaps.logger.service.exe");
	
	//*
	HANDLE hProcess=NULL;
	DWORD dwProcessID = 0;
	if(!_IsProcessExist(strLogServer,FALSE)){
		PROCESS_INFORMATION pi;
		_CreateProcess(strLogServer,pi,FALSE,"");
	}/**/
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
