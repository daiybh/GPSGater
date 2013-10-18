#include "StdAfx.h"
#include "DataOpter.h"

DataOpter::DataOpter(void)
{
}

DataOpter::~DataOpter(void)
{
}

int DataOpter::StartWork()
{
	return Init();
	return 0;
}

int DataOpter::writedb( const GPSINFO *pGpsInfo )
{
	//AfxMessageBox(_T("writeedb--1"));
	if(m_pOracleOCI)
	{
		m_timeCount.Reset();

		int nRet = m_pOracleOCI->WriteData(pGpsInfo);
// 		double dTIme = m_timeCount.GetCountTime(true);
// 		CString slog;
// 		slog.Format(_T("writedb const time=%f"),dTIme);
// 		WriteLog(_T("dvxgps_time"),logLevelError,slog);
		return nRet;
	}
	WriteLog(LOGNAME,logLevelError,_T("DataOpter::writedb --m_pOracleOCI=NULL"));
	//AfxMessageBox(_T("writeedb--2"));
	return 0;

}
int DataOpter::writeCommand(GPSCommand * pGpsCommand)
{
	if(m_pOracleOCI)
		return m_pOracleOCI->writeCommand(pGpsCommand);
	return 0;
}


int DataOpter::StopWork()
{
	return 0;

}

int DataOpter::initTCP()
{
	CString strError;
	SOCKET m_sock;
	int m_port = 1983;
	m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(m_sock == INVALID_SOCKET)
		return FALSE;
	SOCKADDR_IN tcpaddr;
	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_port = htons(m_port);
	tcpaddr.sin_addr.s_addr= htonl(INADDR_ANY);
	int ret = bind(m_sock,(SOCKADDR*)&tcpaddr,sizeof(tcpaddr));
	if(ret!=0)
	{
		strError.Format(_T("TCP bind Error! Error Code=%d."),WSAGetLastError());
		WriteLog(_T("ServerSDKService"),logLevelError,strError);		
		return FALSE;
	}
	unsigned long param = 0;
	ret = ioctlsocket(m_sock, FIONBIO, &param);

	if(ret != 0)
	{
		strError.Format(_T("ioctlsocket Error! Error Code = %d."), WSAGetLastError());
		WriteLog(_T("ServerSDKService"),logLevelError,strError);
		return FALSE;
	}
	ret = listen(m_sock, 6);
	if(ret != 0)
	{
		strError.Format(_T("listen Error! Error Code = %d."), WSAGetLastError());
		WriteLog(_T("ServerSDKService"),logLevelError,strError);
		return FALSE;
	}
	return 1;
}
int DataOpter::Init()
{
	//initTCP();
	m_pOracleOCI = new COracleOCI_o();
	//GetPrivateProfileStringA("")

	char g_user[31] = "hylogistics"; 
	char g_pass_word[31] = "hylogistics"; 
	char g_servername[31] = "kliffv"; 
	char g_serverAddr[31]	="127.0.0.1";
	char strModuleFileName[MAX_PATH],m_strModulePath[MAX_PATH];
	char strPath[MAX_PATH];
	char m_strMgConfigFile_Absolute_Path[MAX_PATH];
	GetModuleFileNameA(NULL,strModuleFileName,MAX_PATH);
	_splitpath_s(strModuleFileName, m_strModulePath,_countof(m_strModulePath), strPath,_countof(strPath),NULL,0,NULL,0);
	
	strcat_s(m_strModulePath,_countof(m_strModulePath),strPath);  
	strcpy_s(m_strMgConfigFile_Absolute_Path,m_strModulePath); 
	strcat_s(m_strMgConfigFile_Absolute_Path,_countof(m_strMgConfigFile_Absolute_Path),("MgCaptureCfg.ini"));

	GetPrivateProfileString("Oracle","user","hylogistics",g_user,31,m_strMgConfigFile_Absolute_Path);
	GetPrivateProfileString("Oracle","password","hylogistics",g_pass_word,31,m_strMgConfigFile_Absolute_Path);
	GetPrivateProfileString("Oracle","SID","kliffv",g_servername,31,m_strMgConfigFile_Absolute_Path);
	GetPrivateProfileString("Oracle","serverAddr","127.0.0.1",g_serverAddr,31,m_strMgConfigFile_Absolute_Path);

	char *pServer = g_serverAddr;
	if(g_serverAddr=="null")
		pServer =NULL;
	BOOL bInsert = GetPrivateProfileInt("Oracle","insertAsNew",0,m_strMgConfigFile_Absolute_Path);
	DWORD dwUpdateVehicleTime = GetPrivateProfileInt("Oracle","updateVehicle",30,m_strMgConfigFile_Absolute_Path);

	dwUpdateVehicleTime = dwUpdateVehicleTime*60*1000;

	m_pOracleOCI->m_dwLimit_MinSpeed = GetPrivateProfileInt("Oracle","limit_minSpeed",7,m_strMgConfigFile_Absolute_Path);

	m_pOracleOCI->m_dwLimit_MaxDistance = (m_pOracleOCI->m_dwLimit_MinSpeed*30*1000)/3600;
	if(m_pOracleOCI->m_dwLimit_MinSpeed <=0)
		m_pOracleOCI->m_dwLimit_MaxDistance = 200;

	

	return m_pOracleOCI->Init(g_user,g_pass_word,g_servername,bInsert,dwUpdateVehicleTime,pServer);
	
	return 1;
}
