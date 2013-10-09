// GPSGateAdpter.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "GPSGateAdpter.h"
#include "IGPSGateAdpter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CGPSGateAdpterApp

BEGIN_MESSAGE_MAP(CGPSGateAdpterApp, CWinApp)
END_MESSAGE_MAP()

// CGPSGateAdpterApp 构造

CGPSGateAdpterApp::CGPSGateAdpterApp()
{

	CreateNMLogWriter();
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_bInitAdpter=FALSE;
	m_pGPS_Socket = NULL;
	m_pDataOpter  = NULL;
	m_pGetCommand = NULL;
}


// 唯一的一个 CGPSGateAdpterApp 对象

CGPSGateAdpterApp theApp;


// CGPSGateAdpterApp 初始化

#include <process.h>
void CGPSGateAdpterApp::InitAdpter()
{
	if(m_bInitAdpter)return;
	WriteLog(LOGNAME,logLevelError,_T("CGPSGateAdpterApp::InitInstance"));


	printf("begin new()\r\n");
	if(m_pGPS_Socket==NULL)
	{
		printf("m_pGPS_Socket = new CGPS_Socket();\r\n");
		m_pGPS_Socket = new CGPS_Socket();
	}
	if(m_pDataOpter==NULL)
	{
		printf("m_pDataOpter  = new DataOpter();\r\n");
		m_pDataOpter  = new DataOpter();
	}
	if(m_pGetCommand==NULL)
	{
		printf("m_pGetCommand = new GetCommand();\r\n");
		m_pGetCommand = new GetCommand();
	}
	printf("End new()\r\n");

	printf("begin m_pGPS_Socket->startwork()\r\n");
	int nRet = m_pGPS_Socket->StartWork();

	printf("begin m_pDataOpter->startwork()\r\n");
	int nRet1 = m_pDataOpter->StartWork();
	printf("begin m_pGetCommand->startwork()\r\n");
	int nRet2 = m_pGetCommand->StartWork();
	printf("begin startwork()\r\n");
	CString sLog;

	sLog.Format(_T("m_pGPS_Socket->StartWork()=%d--%d--%d"),nRet,nRet1,nRet2);
	WriteLog(LOGNAME,logLevelError,sLog);
	m_bInitAdpter = TRUE;
}
BOOL CGPSGateAdpterApp::InitInstance()
{
	CWinApp::InitInstance();
	m_bInitAdpter = FALSE;
	return TRUE;
}
long getGPS(char *buf,char *addr,char * cPort)
{
	theApp.InitAdpter();
	int dwPort=0;
	
	long nRet = theApp.m_pGPS_Socket->getGPS(buf,addr,dwPort);
	ltoa(dwPort,cPort,10);
	return nRet;
}

long writeGPS(const char *buf,const char *addr,const char * cPort)
{
	theApp.InitAdpter();
	return theApp.m_pGPS_Socket->writeGPS(buf,addr,atol(cPort));
}

long getGPS( GPSGATEDATA *pGpsData,char *buf)
{
	theApp.InitAdpter();
	return theApp.m_pGPS_Socket->getGPS(pGpsData,buf);
	return 0;
}
long writeGPS(const GPSGATEDATA * pGpsData,char *pDatabuf,int nDataLen)
{
	theApp.InitAdpter();
	return theApp.m_pGPS_Socket->writeGPS(pGpsData,pDatabuf,nDataLen);
}

 int writedb(const GPSINFO *pGpsInfo)
 {
	 theApp.InitAdpter();
	return theApp.m_pDataOpter->writedb(pGpsInfo);
	return 0;
}
 int WriteCommand(GPSCommand * pGpsCommand)
 {
	 theApp.InitAdpter();
	 return theApp.m_pDataOpter->writeCommand(pGpsCommand);
 }

int getCmd(GPSCommand *pGpsCmd)
{
	theApp.InitAdpter();
	return theApp.m_pGetCommand->getCmd(pGpsCmd);
}
