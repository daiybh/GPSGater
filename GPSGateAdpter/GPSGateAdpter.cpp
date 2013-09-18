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
	m_pGPS_Socket = new CGPS_Socket();
	m_pDataOpter  = new DataOpter();
	m_pGetCommand = new GetCommand();
}


// 唯一的一个 CGPSGateAdpterApp 对象

CGPSGateAdpterApp theApp;


// CGPSGateAdpterApp 初始化

#include <process.h>
BOOL CGPSGateAdpterApp::InitInstance()
{
	WriteLog(LOGNAME,logLevelError,_T("CGPSGateAdpterApp::InitInstance"));
	CWinApp::InitInstance();
	int nRet = m_pGPS_Socket->StartWork();
	int nRet1 = m_pDataOpter->StartWork();
	int nRet2 = m_pGetCommand->StartWork();
	CString sLog;

	sLog.Format(_T("m_pGPS_Socket->StartWork()=%d--%d--%d"),nRet,nRet1,nRet2);
	WriteLog(LOGNAME,logLevelError,sLog);
	return TRUE;
}

long getGPS(char *buf,char *addr,char * cPort)
{
	int dwPort=0;
	
	long nRet = theApp.m_pGPS_Socket->getGPS(buf,addr,dwPort);
	ltoa(dwPort,cPort,10);
	return nRet;
}

long writeGPS(const char *buf,const char *addr,const char * cPort)
{
	return theApp.m_pGPS_Socket->writeGPS(buf,addr,atol(cPort));
}

long getGPS( GPSGATEDATA *pGpsData,char *buf)
{
	return theApp.m_pGPS_Socket->getGPS(pGpsData,buf);
	return 0;
}
long writeGPS(const GPSGATEDATA * pGpsData,char *pDatabuf,int nDataLen)
{
	return theApp.m_pGPS_Socket->writeGPS(pGpsData,pDatabuf,nDataLen);
}

 int writedb(const GPSINFO *pGpsInfo)
{
	return theApp.m_pDataOpter->writedb(pGpsInfo);
	return 0;
}
 int WriteCommand(GPSCommand * pGpsCommand)
 {
	 return theApp.m_pDataOpter->writeCommand(pGpsCommand);
 }

int getCmd(GPSCommand *pGpsCmd)
{
	return theApp.m_pGetCommand->getCmd(pGpsCmd);
}
