#include "StdAfx.h"
#include "OracleOCI_o.h"

#if defined(WIN32)
#pragma comment(lib,"oci.lib")
#endif

#include <MATH.H>

double mS_2_dMMM(DWORD dMs)
{
	float fposd = dMs / 3600000;
	int dd=(int)fposd;//去掉小数
	float ff = dMs/60000.0;
	int mm=((int)ff)%60;
	double fkk = ff-(int)(ff);

	double dResult=0.0;
	dResult = dd*100+mm+fkk;
	return dResult/100.0;
}
int getASI(char hex)
{	
	if(hex >= 'a' && hex <='f' || hex>='A' && hex <='F')
		return (hex &0x0f)+9;
	else
		return hex &0x0f;

}
int hex2dec(const char *pHex)
{
	int nLen = strlen(pHex);
	int nResult=0;
	for(int i=0;i<nLen;i++)
	{
		nResult+= getASI(pHex[i]) *pow((double)16,(double)(nLen-i-1));
	}
	return nResult;
}
char g_TimeBuf[22];
#define CHAR_2_iNT(x) (x-'0') 
char * changeTime(const char *pGPSTime,CTime &GpsTime)
{
	//120223075300
	//20120223075300
	int nYear,nMonth,nDay,nHour,nMinute,nSecond;

	if(pGPSTime[0]=='2' && pGPSTime[1]=='0' /*&& pGPSTime[2]=='1'*/)
	{	
		nYear	= CHAR_2_iNT(pGPSTime[0])*1000+CHAR_2_iNT(pGPSTime[1])*100+CHAR_2_iNT(pGPSTime[2])*10+CHAR_2_iNT(pGPSTime[3]);
		nMonth	= CHAR_2_iNT(pGPSTime[4])*10+CHAR_2_iNT(pGPSTime[5]);
		nDay	= CHAR_2_iNT(pGPSTime[6])*10+CHAR_2_iNT(pGPSTime[7]);
		nHour	= CHAR_2_iNT(pGPSTime[8])*10+CHAR_2_iNT(pGPSTime[9]);
		nMinute	= CHAR_2_iNT(pGPSTime[10])*10+CHAR_2_iNT(pGPSTime[11]);
		nSecond	= CHAR_2_iNT(pGPSTime[12])*10+CHAR_2_iNT(pGPSTime[13]);
	}
	else
	{
		nYear	= 2*1000+0*100+CHAR_2_iNT(pGPSTime[0])*10+CHAR_2_iNT(pGPSTime[1]);
		nMonth	= CHAR_2_iNT(pGPSTime[2])*10+CHAR_2_iNT(pGPSTime[3]);
		nDay	= CHAR_2_iNT(pGPSTime[4])*10+CHAR_2_iNT(pGPSTime[5]);
		nHour	= CHAR_2_iNT(pGPSTime[6])*10+CHAR_2_iNT(pGPSTime[7]);
		nMinute	= CHAR_2_iNT(pGPSTime[8])*10+CHAR_2_iNT(pGPSTime[9]);
		nSecond	= CHAR_2_iNT(pGPSTime[10])*10+CHAR_2_iNT(pGPSTime[11]);

	}
	CTime tim(nYear,nMonth,nDay,nHour,nMinute,nSecond);
	if(pGPSTime[19]!=0x01)
		tim +=CTimeSpan(0,8,0,0);

	sprintf(g_TimeBuf,"%d-%d-%d %d:%d:%d",
		tim.GetYear(),//年
		tim.GetMonth(),//月
		tim.GetDay(),//日
		tim.GetHour(),//时
		tim.GetMinute(),//分
		tim.GetSecond());//秒
	GpsTime = tim;
	return g_TimeBuf;
}

COracleOCI_o::COracleOCI_o(void)
{
	m_bInsert_As_NewVehicle = FALSE;
	m_time_t_TableNameTime = 0;
	strcpy(m_strDate,"Null");
	m_i64d_GetVehicleInfo =0;
	ZeroMemory(&m_tm_TableNameTime,sizeof(m_tm_TableNameTime));
	m_bInitOCI = FALSE;
	m_dwLimit_MinSpeed = 0;
}

COracleOCI_o::~COracleOCI_o(void)
{
	printf("~~~~~COracleOCI_o");
}
void COracleOCI_o::Close()
{

	printf("COracleOCI_o~~~~~Close--1");
	conn.disconnect();
	printf("COracleOCI_o~~~~~Close--2");
}
int COracleOCI_o::Init( char *user,char*pwd,char*serverName,BOOL bInsertAsNewVehicle,DWORD dwUpdateVehicleTime,char *serverAddr/*=NULL*/ )
{
	m_bInitOCI=FALSE;
	int nRet =-1;
	if(serverAddr==NULL)
		nRet = conn.connect(serverName, user,pwd);
	else
		nRet = conn.connect(serverAddr,serverName,user,pwd);


	CStringA s;
	s.Format("nret=%d,usr=%s,pwd=%s,ser=%s\r\nerrMSG=%s",nRet,user,pwd,serverName,conn.getErrorMsg());
	//	::MessageBoxA(NULL,s,"cap",MB_YESNO);
	WriteLog(LOGNAME,logLevelError,CString(s));

	m_dwUpdateVehicleTime = dwUpdateVehicleTime;
	m_bInsert_As_NewVehicle = bInsertAsNewVehicle;
	if(nRet <1)
	{
		CStringA s;
		s.Format("nret=%d,usr=%s,pwd=%s,ser=%s\r\nerrMSG=%s",nRet,user,pwd,serverName,conn.getErrorMsg());
	//	::MessageBoxA(NULL,s,"cap",MB_YESNO);
		WriteLog(LOGNAME,logLevelError,CString(s));
		m_bInitOCI = FALSE;
	}
	else{
		//		nRet =CreateTable();
		m_bInitOCI = TRUE;
		getInfos_Form_VEHICLE();
	}
	return nRet;
}

double Distance(double Latitude1, double Longitude1, double Latitude2, double Longitude2)
{
	double jl_jd = 102834.74258026089786013677476285;
	double jl_wd = 111712.69150641055729984301412873;
	double b = abs((Longitude1 - Longitude2) * jl_jd);
	double a = abs((Latitude1 - Latitude2) * jl_wd);
	return sqrt((a * a + b * b));

}

bool checkGPS_Latitude(double dNow_Longitude,double dNow_Latitude,double dPre_Longtitude,double dPre_Latitude,DWORD dwLitmitMaxDistance)
{
	double dRet =Distance(dNow_Latitude,dNow_Longitude,dPre_Latitude,dPre_Longtitude);
	if(dRet<dwLitmitMaxDistance)
	{
	}
	return dRet>dwLitmitMaxDistance;
}

/*
 *	@return -1 位置信息为0
			-2	有这个车，但是当前位置，速度为0，属于漂移坐标 不填入数据库
			-3	没有这个车	不填入数据库
			1	此数据正常，可以填入到数据库
			2	此数据正常，但是速度超速了。
			3   此数据正常，但是越界了，
			4   此数据正常，但是进入了不该进入的区域
			5	此数据为gps回答数据，速度坐标等信息不可用

 */
JUDGE_RET COracleOCI_o::judge_GPSData( const GPSINFO* pGpsInfo,const INT64 *iSim,int nSpeed,double doubleLongitude,double doubleLatitude,DataInfo **ppDI )
{
	
	DataInfo *pDI = GetVehicleInfo(pGpsInfo->COMMADDR);
	bool bDebug=false;
	if(bDebug)
	{
		pDI = new DataInfo();
		pDI->doubleLatitude= 0.0;
		pDI->doubleLongitude = 0.0;
	}
	if(pDI==NULL)
	{	
		return RET_NO_CAR;
	}
	*ppDI = pDI;		
	//如果这个只是MSG_TERMINALFEEDBACK 那么直接返回5
	if(pGpsInfo->nMsgID == MSG_TERMINALFEEDBACK)
	{
		return RET_GPS_ANSWER;
	}
	if(!pGpsInfo->bValid)
	{
		CString strLog;
		strLog.Format("RET_INVALID %s pgsinfo-bvalid=false",pGpsInfo->COMMADDR);
		OutputDebugString(strLog);
		return RET_INVALID;
	}
	if(0==(int)doubleLatitude || 0== (int)doubleLongitude)
	{

		CString strLog;
		strLog.Format("RET_INVALID %s 0==(int)doubleLatitude || 0== (int)doubleLongitude",pGpsInfo->COMMADDR);
		OutputDebugString(strLog);
		return RET_INVALID;
	}
	//判断这个车是否有报警信息
	if(pGpsInfo->nWarnFlag>0)
	{
		if(pGpsInfo->nWarnFlag&WAR_OUTZONE)
		{
			return RET_OVERAREA_OUT;
		}
		if(pGpsInfo->nWarnFlag&WAR_INZONE)
		{
			return RET_OVERAREA_IN;
		}
		if(pGpsInfo->nWarnFlag&WAR_OVERSPEED)
			return RET_OVERSPEED;
	}
	int nCurHeading = atoi(pGpsInfo->Heading);
	//判断 与上次的坐标是否一样，一样就不加入表
	int nscale = 1000000;
	if((pDI->nHeading ==nCurHeading) && ( int(pDI->doubleLatitude*nscale) == int(doubleLatitude*nscale)) && (int(pDI->doubleLongitude*nscale) == int(doubleLongitude*nscale) ))
	{
		return RET_SAME_LOCATION;
	}
/*
	//速度是否在合法范围内
	if(nSpeed > m_dwLimit_MinSpeed )
	{
		if(pDI->dwMaxSpeed >0 && nSpeed > pDI->dwMaxSpeed)
		{
			CString strLog;
			strLog.Format(_T("overspeed now:%d,max:%d,tid:%I64d"),nSpeed,pDI->dwMaxSpeed,*iSim);
			WriteLog(_T("overspeed"),logLevelError,strLog);
			return RET_OVERSPEED;
		}
		return RET_NORMAL;
	}
*/
	//判断是否要检查飘逸
	if((pGpsInfo->nDevID &GPS_MEITRACK)==GPS_MEITRACK)
	{
		pDI->doubleLatitude = doubleLatitude;
		pDI->doubleLongitude = doubleLongitude;
		pDI->nHeading = nCurHeading;
		pDI->dLastUpdate_LocationTime = GetTickCount();
		return RET_NORMAL;
	}
	//检查是否属于漂移
	bool bRet = checkGPS_Latitude(doubleLongitude,doubleLatitude,pDI->doubleLongitude,pDI->doubleLatitude,m_dwLimit_MaxDistance);
// 	CString strLog;
// 	strLog.Format(_T("[checkGPS]%I64d checkGPS_latitude(%f,%f,%f,%f,%d,speed=%d)=%d"),*iSim,doubleLongitude,doubleLatitude,pDI->doubleLongitude,pDI->doubleLatitude,m_dwLimit_MaxDistance,nSpeed,bRet);
// 	OutputDebugString(strLog);
	if(bRet)
	{
		pDI->doubleLatitude = doubleLatitude;
		pDI->doubleLongitude = doubleLongitude;
		pDI->dLastUpdate_LocationTime = GetTickCount();
		pDI->nHeading = nCurHeading;
		return RET_NORMAL;
	}
	return RET_PIAOYI;
}
BOOL prepare_Execute_SQL(CConnection conn,char *pSQL)
{

	sword status = OCIStmtPrepare(conn.mystmthp, conn.myerrhp, (const unsigned    char * )pSQL,
		(ub4) strlen((char *) pSQL),
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
	if(status!=OCI_SUCCESS)
		return FALSE;

	status = OCIStmtExecute(conn.mysvchp, conn.mystmthp, conn.myerrhp, 0, 0, NULL, NULL, OCI_DEFAULT);
	if(status!=OCI_SUCCESS)
		return FALSE;
	return TRUE;
}
int COracleOCI_o::getInfos_Form_VEHICLE()
{
	if(!m_bInitOCI) return 0;
	if(GetTickCount()-m_i64d_GetVehicleInfo < m_dwUpdateVehicleTime)
	{
		CString strLog;
		strLog.Format("getInfos_Form_VEHICLE failed.now[%d]-last[%i64d] <setTime[%d]",GetTickCount(),m_i64d_GetVehicleInfo,m_dwUpdateVehicleTime);
		OutputDebugString(strLog);
		return -1;
	}
	m_i64d_GetVehicleInfo = GetTickCount();
	if(m_dwUpdateVehicleTime ==0)
		m_i64d_GetVehicleInfo = 0x9FFFFFFFFFFFFFFF;
	char *ins = "SELECT VEHICLE_ID,SIM,FLAG,tid,UP_SPEED  FROM VEHICLE";
	sword status;
	OCIDefine *defhp1,*defhp2,*defhp3,*defhp4,*defhp5;
	OCIError *errhp;

	char szSim[20],szVehicleID[50],szFlag[20],szTID[50],szUpSpeed[20];

	char szID[20] = "11";
	OCIBind* bndhp1 = NULL;
	OCIBind* bndhp2 = NULL;
	OCIBind* bndhp3 = NULL;
	OCIBind* bndhp4 = NULL;
	OCIBind* bndhp5 = NULL;

	OutputDebugString(ins);
// 	status = OCIStmtPrepare(conn.mystmthp, conn.myerrhp, (const unsigned    char * )ins,
// 		(ub4) strlen((char *) ins),
// 		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
// 
// 
// 	status = OCIStmtExecute(conn.mysvchp, conn.mystmthp, conn.myerrhp, 0, 0, NULL, NULL, OCI_DEFAULT);
	BOOL bHadUpSpeed=TRUE;
	if(!prepare_Execute_SQL(conn,ins))	
	{
		//
		char *ins2 = "SELECT VEHICLE_ID,SIM,FLAG,tid  FROM VEHICLE";
		if(!prepare_Execute_SQL(conn,ins2))
		{
			printf_ErrorLog(_T("OCIStmtExecute"));
			WriteLog(LOGNAME,logLevelError,CString(ins2));
			return 0;
		}
	}

/*
	OCIBindByName(conn.mystmthp,&bndhp1,errhp,(text*)":v1",(sb4)strlen((char*)":v1"),
		(dvoid*)&szID,(sb4)sizeof(szID),SQLT_STR,(dvoid*)0,(ub2*)0,(ub2*)0,(ub4)0,(ub4*)0,(ub4)OCI_DEFAULT);
	
	OCIBindByName(conn.mystmthp, &bnd2p, conn.myerrhp, (text *) ":D7A7T7A",
		-1, (dvoid *) m_strDate,
		32 , SQLT_STR, (dvoid *) pp, 
		(ub2 *) 0, (ub2 *) 0, (ub4) 3, (ub4 *) 0, OCI_DEFAULT);
	/**/

	status = OCIDefineByPos(conn.mystmthp,&defhp1,conn.myerrhp,1,szVehicleID,sizeof(szVehicleID),SQLT_STR,0,0,0,OCI_DEFAULT);
	status = OCIDefineByPos(conn.mystmthp,&defhp2,conn.myerrhp,2,szSim,sizeof(szSim),SQLT_STR,0,0,0,OCI_DEFAULT);
	status = OCIDefineByPos(conn.mystmthp,&defhp3,conn.myerrhp,3,szFlag,sizeof(szFlag),SQLT_STR,0,0,0,OCI_DEFAULT);
	status = OCIDefineByPos(conn.mystmthp,&defhp4,conn.myerrhp,4,szTID,sizeof(szTID),SQLT_STR,0,0,0,OCI_DEFAULT);
	if(bHadUpSpeed)
		status = OCIDefineByPos(conn.mystmthp,&defhp5,conn.myerrhp,5,szUpSpeed,sizeof(szUpSpeed),SQLT_STR,0,0,0,OCI_DEFAULT);

	//status = OCIStmtFetch(conn.mystmthp, conn.myerrhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
	int nRet = 0;
	m_DataInfoMap.clear();
	CString strLog;
	int nCnt =0;

	strLog.Format(_T("VEHICLE Info:"));
	//WriteLog(LOGNAME,logLevelError,strLog);
	TCHAR wcTemp[255];
	CString strSim,strTid,strVehicleID,strFlag,strUpSpeed;
	while(OCI_NO_DATA!=OCIStmtFetch(conn.mystmthp,conn.myerrhp,1,OCI_FETCH_NEXT,OCI_DEFAULT))
	{
#ifdef _UNICODE
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wcTemp,  (strlen(szSim) + 1), szSim, _TRUNCATE);
		strSim.Format(_T("%s"), wcTemp);

		mbstowcs_s(&convertedChars, wcTemp,  (strlen(szTID) + 1), szTID, _TRUNCATE);
		strTid.Format(_T("%s"), wcTemp);

		mbstowcs_s(&convertedChars, wcTemp,  (strlen(szVehicleID) + 1), szVehicleID, _TRUNCATE);
		strVehicleID.Format(_T("%s"), wcTemp);

		mbstowcs_s(&convertedChars, wcTemp,  (strlen(szFlag) + 1), szFlag, _TRUNCATE);
		strFlag.Format(_T("%s"), wcTemp);
#else
		strSim.Format(_T("%s"), szSim);
		strTid.Format(_T("%s"), szTID);
		strVehicleID.Format(_T("%s"), szVehicleID);
		strFlag.Format(_T("%s"), szFlag);
#endif
		int maxSpeed = 100;
		if(bHadUpSpeed)
		{

#ifdef _UNICODE
			mbstowcs_s(&convertedChars, wcTemp,  (strlen(szUpSpeed) + 1), szUpSpeed, _TRUNCATE);
			strUpSpeed.Format(_T("%s"), wcTemp);
			maxSpeed = _wtoi(wcTemp);
#else
			strUpSpeed.Format(_T("%s"), szUpSpeed);
			maxSpeed = atoi(szUpSpeed);
#endif
		}
		if(maxSpeed==0)
			maxSpeed =100;

		


		strLog.Format(_T("VEHICLE %d--sim:%s------ID:%s----flag:%s,tid:%s,upspeed:%s"),nCnt++,strSim,strVehicleID,strFlag,strTid,strUpSpeed);
		//AfxMessageBox(CString(strLog));
		//WriteLog(LOGNAME,logLevelInfo,strLog);
		OutputDebugString(strLog);
		if(szSim[0]!='\0')
		{
			DataInfo* dataInfo = new DataInfo();
			strcpy(dataInfo->vehicheID,szVehicleID);
			dataInfo->dwMaxSpeed = maxSpeed;
			m_DataInfoMap[_atoi64(szSim)] = dataInfo;
		}

		if(szTID[0]!='\0' && strcmp(szSim,szTID)!=0)
		{
			DataInfo* dataInfo1 = new DataInfo();
			dataInfo1->dwMaxSpeed = maxSpeed;
			strcpy(dataInfo1->vehicheID,szVehicleID);
			m_DataInfoMap[_atoi64(szTID)] = dataInfo1;		
		}
	}
	//id[0]=1;id[1]=2;
	//这里一次可以插入2行数据
	//int nSzie = m_DataInfoMap.size();
	return 1;	
}
#include <time.h>
int COracleOCI_o::CreateTable(struct tm* pCurGPSTime)
{
	if(!m_bInitOCI) return 0;

	char * ins ="CREATE TABLE \"";
	char * insEnd ="\" (\"SIM\" VARCHAR2(48 BYTE) NULL ,\
		    		\"TID\" VARCHAR2(48 BYTE) NULL ,\
				   \"GPS_DATE\" DATE NULL ,\
				   \"RECV_DATE\" DATE NULL ,\
				   \"LNG\" NUMBER(15,10) NULL ,\
				   \"LAT\" NUMBER(15,10) NULL ,\
				   \"VEO\" NUMBER(10,6) NULL ,\
				   \"DIRECT\" NUMBER(3) NULL ,\
				   \"AV\" VARCHAR2(1 BYTE) NULL ,\
				   \"ISTATE\" NUMBER(1) NULL ,\
				   \"CSTATE\" VARCHAR2(200 BYTE) NULL ,\
				   \"POS_INFO\" VARCHAR2(250 BYTE) NULL ,\
				   \"DISTANCE\" NUMBER(10,2) NULL ,\
				   \"DIFFERENCE_DATE\" DATE NULL ,\
				   \"TEMPERATURE\" NUMBER(5,1) NULL ,\
				   \"OIL\" NUMBER(10,2) NULL ,\
				   \"LASTRUN_DATE\" DATE NULL ,\
				   \"LNG2\" NUMBER(10,6) NULL ,\
				   \"LAT2\" NUMBER(10,6) NULL ,\
				   \"TOTAL_DISTANCE\" NUMBER(10,2) NULL ,\
				   \"PROID\" NUMBER(11) NULL ,\
				   \"TASKID\" NUMBER(11) NULL ,\
				   \"VAL1\" NUMBER(11) NULL ,\
				   \"VAL2\" NUMBER(11) NULL ,\
				   \"MILEAGE\" NUMBER(9,2) NULL,\
				   \"NOLOAD\" NUMBER(3) NULL)";
	m_tm_TableNameTime.tm_year = pCurGPSTime->tm_year;
	m_tm_TableNameTime.tm_mon = pCurGPSTime->tm_mon;
	m_tm_TableNameTime.tm_mday = pCurGPSTime->tm_mday;
	m_tm_TableNameTime.tm_hour = pCurGPSTime->tm_hour;
	m_tm_TableNameTime.tm_min = pCurGPSTime->tm_min;
	m_tm_TableNameTime.tm_sec = pCurGPSTime->tm_sec;
	sprintf(m_strDate,"%d%.2d%.2d",pCurGPSTime->tm_year+1900,pCurGPSTime->tm_mon+1,pCurGPSTime->tm_mday);
	{
		//生成 插入 更新的sql语句 使用变量绑定方式操作
	}
	char strSQL[1024];
	sprintf(strSQL,"%sGPS_%s%s",ins,m_strDate,insEnd);
	int nRet1 =  _DoCreateTable(strSQL);
sprintf(strSQL,"create table \"OBD_DETAIL%s\" (TID        VARCHAR2(48),\
		SAVED_DATE DATE,\
		OBD_TYPE   NUMBER(2),\
		CONTENT    VARCHAR2(1000))",m_strDate);
	int nRet2 = _DoCreateTable(strSQL);
	return (nRet2&&nRet1);
}
int COracleOCI_o::_DoCreateTable(const char*createTableSQL)
{
	OutputDebugString(createTableSQL);

	WriteLog(LOGNAME,logLevelError,createTableSQL);
	sword status;
	OCIBind  *bnd1p = (OCIBind *) 0;             /* the first bind handle */
	OCIBind  *bnd2p = (OCIBind *) 0;             /* the second bind handle */

	sb2 nu[3] = {0,0,-1};  
	sb2* pp= nu;  
	OCIStmtPrepare(conn.mystmthp, conn.myerrhp, (const unsigned    char * )createTableSQL,
		(ub4) strlen((char *) createTableSQL),
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
	OCIStmtExecute(conn.mysvchp, conn.mystmthp, conn.myerrhp, 1, 0, NULL, NULL, OCI_DEFAULT);
	return conn.commit();
}
void COracleOCI_o::printf_ErrorLog(LPCTSTR lpExtInfo)
{
	m_bInitOCI=FALSE;
	sb4 errcodep;
	char errbuf[1024]={0};


	OCIErrorGet(conn.myerrhp, (ub4)1, NULL, &errcodep, (text*)errbuf, (ub4)sizeof(errbuf), (ub4)OCI_HTYPE_ERROR);
	//		sprintf(errMsg,"OCIServerAttach error[%s]!/n",errbuf);
	CStringA strLog;
	strLog.Format("oci Error---%s --[%s]",lpExtInfo,errbuf);
	CString ss(strLog);

	WriteLog(LOGNAME,logLevelError,ss);
}
int COracleOCI_o::Insert_As_NewVehicle( const char *pTid,const char*pSim )
{
	if(!m_bInitOCI) return 0;
	CStringA s;
	CTime tm;
	tm = CTime::GetCurrentTime();
	
// 	s.Format("%d%.2d%.2d%.2d%.2d%.2d",
// 		tm.GetYear(),tm.GetMonth(),tm.GetDay(),
// 		tm.GetHour(),tm.GetSecond(),tm.GetMinute(),tm.getn);

	SYSTEMTIME   t; 
	GetLocalTime(&t); 
	s.Format("%d%.2d%.2d%.2d%.2d%.2d%.4d",
		t.wYear,t.wMonth,t.wDay,
		t.wHour,t.wMinute,t.wSecond,t.wMilliseconds);

	//INSERT into VEHICLE (VEHICLE_id,tid) VALUES('2','98733')
	//tid sim addtime state = 2
	sprintf(m_strInsertDataSQL,"INSERT into VEHICLE (VEHICLE_id,tid,sim,state,ADDED_DATE) VALUES('%s','%s','%s','2',TO_DATE('%d-%.2d-%.2d %.2d:%.2d:%.2d', 'YYYY-MM-DD HH24:MI:SS'))",
		s,		
		pTid,pSim,
		t.wYear,t.wMonth,t.wDay,
		t.wHour,t.wMinute,t.wSecond
		);
	if(1== DoInsertSQL(m_strInsertDataSQL))
	{
		m_DataInfoMap[_atoi64(pSim)] = new DataInfo();
		if (pSim != pTid)
		{
			m_DataInfoMap[_atoi64(pTid)] = new DataInfo();
		}
	}
	return 1;
}
int COracleOCI_o::Excute_DeleteSQL(char *pDeleteSQL)
{
	if(!m_bInitOCI) return 0;
 sword status;
		OCIBind  *bnd1p = (OCIBind *) 0;             /* the first bind handle */
	OCIBind  *bnd2p = (OCIBind *) 0;             /* the second bind handle */
	
	OutputDebugStringA(pDeleteSQL);
	OutputDebugString(_T("\r\n"));
	status = OCIStmtPrepare(conn.mystmthp, conn.myerrhp, (const unsigned    char * )pDeleteSQL,
		(ub4) strlen((char *) pDeleteSQL),
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

	if (status != OCI_SUCCESS)
	{
		printf_ErrorLog(_T("OCIStmtPrepare"));
		return -2;
	}
	status = OCIStmtExecute(conn.mysvchp, conn.mystmthp, conn.myerrhp, 1, 0, NULL, NULL, OCI_DEFAULT);

	if (status != OCI_SUCCESS)
	{
		printf_ErrorLog(_T("OCIStmtExecute"));
		WriteLog(LOGNAME,logLevelError,CString(pDeleteSQL));
		return -3;
	}
	conn.commit();
	return 1;

}
int COracleOCI_o::Updata( const GPSINFO *pGpsInfo ,double doubleLongitude,double doubleLatitude,CStringA s_RecvTime,CStringA str_CurTime,JUDGE_RET judge_ret )
{
	if(!m_bInitOCI) return 0;
	sword status;

	//	更新 state lng lat direaction speed TERMINAL_DATE SERVER_DATE
	//char * ins = "insert into student (id, name) values (:ID, :NAME)";  
	//	char * ins = "INSERT INTO GPS_:D8A7T6E3 (GPS_ID,MEMBER_ID)VALUES('1765866567',:ID)";
	BOOL	bSim	=	pGpsInfo->nDevID &0x100;
	BOOL	bTid	=	pGpsInfo->nDevID&0x200;
	int iState = 0;
	// 0 表示定位正常  （包含漂移的 修改gps和坐标 和状态位）
	//	1 表示有数据但不定位（数据无效，不修改坐标 要修改gps时间和状态位）

	bool bUpateLocInfo = true;

	if(RET_INVALID == judge_ret)
	{
		iState = 1;
		bUpateLocInfo= false;
	}
	char tempBuffer[1024];
	tempBuffer[0]='\0';
	if(bUpateLocInfo)
	{
		sprintf(tempBuffer,"LNG=%.4f,LAT=%.4f,DIRECTION=%s,SPEED=%s,noload=%d,mileage=%s,", 
			doubleLongitude,
			doubleLatitude,
			pGpsInfo->Heading,
			pGpsInfo->Speed,
			pGpsInfo->Noload,
			pGpsInfo->st_OBD_Info.Mileage);
	}

	sprintf(m_strUpdateDataSQL,"update VEHICLE set \
							   %s \
							   TERMINAL_DATE=TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),\
							   SERVER_DATE=TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),gps_state=%d  WHERE %s='%s'",
							   tempBuffer,
							   s_RecvTime,
							   str_CurTime,
							   iState,
							   bSim?"SIM":"tid",pGpsInfo->COMMADDR);

	
	OutputDebugStringA(m_strUpdateDataSQL);
	OutputDebugString(_T("\r\n"));

	status = OCIStmtPrepare(conn.mystmthp, conn.myerrhp, (const unsigned    char * )m_strUpdateDataSQL,
		(ub4) strlen((char *) m_strUpdateDataSQL),
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

	if (status != OCI_SUCCESS)
	{
		printf_ErrorLog(_T("OCIStmtPrepare"));
		WriteLog(LOGNAME,logLevelError,CString(m_strUpdateDataSQL));
		return -2;
	}
	status = OCIStmtExecute(conn.mysvchp, conn.mystmthp, conn.myerrhp, 1, 0, NULL, NULL, OCI_DEFAULT);

	if (status != OCI_SUCCESS)
	{
		printf_ErrorLog(_T("OCIStmtExecute"));
		WriteLog(LOGNAME,logLevelError,CString(m_strUpdateDataSQL));
		return -3;
	}
	conn.commit();
	return 1;

}
INT64 g_WriteData_Cnt =0;
/*
	数据有效性判断
	1 time长度小于12     120315090923
	2 commanddr[0]='\0'
	3 pGpsInfo->Longitude[0]=='\0' || pGpsInfo->Latitude[0]=='\0'
	4 pGpsInfo->Heading[0]=='\0'
	-----------------------
	5 Longitude=0 Latitude =0  的不要？
	6 过期数据（当前服务器时间之前x小时）直接入库，不进VEHICLE，不进行漂移判断？
	  ---如果要进行漂移判断，需要明确上来的数据是否是顺序的，
	  ---即： 过期数据-过期数据-过期数据-过期数据--即时数据-即时数据-即时数据
	7 
*/
int IsValidData(const GPSINFO *pGpsInfo)
{
	if(pGpsInfo->bValid)
	{
		if(strlen(pGpsInfo->Time) <12)
			return -1;
	}
	if(pGpsInfo->COMMADDR[0]=='\0')
		return -2;
// 	if(pGpsInfo->Longitude[0]=='\0' || pGpsInfo->Latitude[0]=='\0')
// 	{
// 		return -3;
// 	}
	//if(pGpsInfo->Heading[0]=='\0')
	//	return -4;
	return 1;

}
#define MAX_TIME (60*60*24)
#define MIN_TIME -(60*60*24)
BOOL NeedCreateNewTable(struct tm  preTableGpsTime,struct tm * curGpsTime)
{	
	if(preTableGpsTime.tm_mday != curGpsTime->tm_mday 
		|| preTableGpsTime.tm_mon!=curGpsTime->tm_mon
		|| preTableGpsTime.tm_year !=curGpsTime->tm_year)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL NeedCreateNewTable(struct tm * tTableTime)
{
	time_t rawtime;

	time ( &rawtime );
	struct tm * t= localtime ( &rawtime );
	if(t->tm_mday != tTableTime->tm_mday)
		return TRUE;
	return FALSE;
}

__int64 g_recvCnt = 0;
/*
													GPS_20120813																VEHICLE		
			
												"ISTATE		"											插入数据				"gps_state					更新车辆GPS时间				更新车辆坐标
											（0 表示正常 1表示不正常 2 表示漂移）								0 表示定位正常 包含漂移的 修改gps和坐标 和状态位）
																																	1 表示有数据但不定位"	
正常数据									0															是							0								是										是
正常数据-漂移							2															是							0								是										否
非法数据（数据有效位（A/V））	null														否							1								是										否
/**/
int COracleOCI_o::WriteData( const GPSINFO *pGpsInfo )
{
	DWORD dInTime = GetTickCount();

	int nRet = IsValidData(pGpsInfo);
	if(nRet <0)
	{
		CString strLog;
		strLog.Format(_T("IsValidData nRet=%d--dIntime=%d"),nRet,dInTime);
		//WriteLog(LOGNAME,logLevelInfo,strLog);
		OutputDebugString(strLog);
		return nRet;
	}
	getInfos_Form_VEHICLE();

	//由于map 的key 使用 int64 所以 deviceID 需要转换为 int64
	INT64 iSim = _atoi64(pGpsInfo->COMMADDR);
	//由于需要判断 坐标的大小，所以需要转换为double
	double doubleLongitude = atof(pGpsInfo->Longitude);
	double doubleLatitude  = atof(pGpsInfo->Latitude);
	int		nSpeed				= atoi(pGpsInfo->Speed);	
	DataInfo *pDI = NULL;
	
	JUDGE_RET judge_ret = judge_GPSData(pGpsInfo,&iSim,nSpeed,doubleLongitude,doubleLatitude,&pDI);
	//0 应答数据 不使用										RET_GPS_ANSWER
	nRet = 0;


	CTime gpsTime=CTime::GetCurrentTime();
	CTime time_Servers=CTime::GetCurrentTime();
	CStringA str_time_Servers (time_Servers.Format(_T("%Y-%m-%d %H:%M:%S")));

	//char * str_GpsTime=changeTime(pGpsInfo->Time,gpsTime);
	CStringA str_GpsTime(str_time_Servers);
	if(pGpsInfo->bValid)
	{
		str_GpsTime =CStringA(changeTime(pGpsInfo->Time,gpsTime));
	}
	//2 有车，数据无效  只更新vehicle中的时间		RET_INVALID
	//3 有车，数据正常，一切正常						RET_NORMAL
	//4 有车，数据正常，判定为漂移						RET_PIAOYI
	//5 有车，数据正常，越界数据						RET_OVERAREA_OUT
	//6 有车，数据正常，进入了不该进入的区域		RET_OVERAREA_IN
	//7 有车，数据正常，超速								RET_OVERSPEED
	do{
		if(RET_NO_CAR == judge_ret)
		{
			//1 先找车，如果没有这个车就丢数据				RET_NO_CAR
			if(m_bInsert_As_NewVehicle)
			{
				BOOL bSim = pGpsInfo->nDevID&0x100;
				BOOL bTid = pGpsInfo->nDevID&0x200;
				Insert_As_NewVehicle(bTid?pGpsInfo->COMMADDR:"",bSim?pGpsInfo->COMMADDR:"");
			}
			nRet = -3;
			break;
		}
		else if(RET_SAME_LOCATION == judge_ret)
		{
			//不做任何数据库操作。
			nRet = 1;
			break;
		}
		else if(RET_GPS_ANSWER == judge_ret)
		{
			//nRet = ;
			//gps的应答数据，需要从 m_ArrayCommandItem 中去除对应的命令
			//由于到这里可以知道 vehicle_ID 所以可以通过 vehicle_id来查找。
			CommandItem* cmdItem =NULL;
			int iPos = -1;
			for(int i=0;i<m_ArrayCommandItem.GetSize();i++)
			{
				cmdItem = m_ArrayCommandItem.GetAt(i);
				if(strcmp(cmdItem->vehicle_ID,pDI->vehicheID)==0)
				{
					//车相等，但是还需要查找对应的命令
					if(strcmp(cmdItem->cmdID , pGpsInfo->CMDID)==0)
					{
						//就是这个命令
						iPos = i;
						break;
					}
				}
			}
			if(iPos==-1)
			{
				break;
			}
			if(cmdItem->nCmdType == cmtType_ToGPS_Setarea)
			//执行删除数据的操作
			{
				sprintf(m_strDeleteDataSQL_ForCmd,"delete REGIONL_SET WHERE id=%d",cmdItem->recordID);

				nRet = Excute_DeleteSQL(m_strDeleteDataSQL_ForCmd);
			}

			CString strLog;
			strLog.Format(_T("data-gps_CallBack,msgID=%s,nRet=%d,%s"),pGpsInfo->CMDID,nRet,CString(pGpsInfo->CMDARGUS));
			WriteLog(LOGNAME,logLevelInfo,strLog);
			break;
		}		
		else if(RET_NORMAL == judge_ret || RET_INVALID == judge_ret ||RET_PIAOYI == judge_ret)
		{
			//判断是否要创建新表了

			struct tm pCurGpsTm,t2;
			gpsTime.GetLocalTm(&pCurGpsTm);

			if(NeedCreateNewTable(m_tm_TableNameTime,&pCurGpsTm))
			{

				int nCreateRet = CreateTable(&pCurGpsTm);
				CString strLog;
				strLog.Format(_T("sim:%s-->NeedCreateTable-->pre:[%d-%d-%d] cur:[%d-%d-%d]---nRet=%d"),
					pGpsInfo->COMMADDR,
					m_tm_TableNameTime.tm_year,m_tm_TableNameTime.tm_mon,m_tm_TableNameTime.tm_mday,
					pCurGpsTm.tm_year,pCurGpsTm.tm_mon,pCurGpsTm.tm_mday,nCreateRet);
				WriteLog(LOGNAME,logLevelInfo,strLog);
				OutputDebugString(strLog);
			}

			//ISTATE 0 表示正常 1表示不正常 2 表示漂移
			int iState = 0;
			if( RET_NORMAL == judge_ret)
			{
				;
			}
			else if(RET_PIAOYI == judge_ret)
			{
				iState = 2;
			}
			else if(RET_INVALID == judge_ret)
			{
				iState = 3;
				nRet = 1;
			}
			if(iState != 3)
			{
				nRet = InsertData(pGpsInfo,doubleLongitude,doubleLatitude,str_GpsTime,str_time_Servers,iState);
				Insert_ODBInfo_Data(pGpsInfo,str_time_Servers);
			}
			if(nRet < 1)
			{
				nRet = nRet -1000;
				//插入数据失败
				break;
			}
			/*
			vehicle
			gps_state  int 
			0 表示定位正常  （包含漂移的 修改gps和坐标 和状态位）
			1 表示有数据但不定位（数据无效，不修改坐标 要修改gps时间和状态位）
			/**/
			//判断		gpsTime 小于	pDI->dLastUpdate_SQLTime  ，就不更新车位置
			CString s;
			s.Format("pDI->dLastUpdate_SQLTime[%I64d]=== gpsTime.GetTime()[%I64d]",
				pDI->dLastUpdate_SQLTime , gpsTime.GetTime());
			OutputDebugString(s);
			if(pDI->dLastUpdate_SQLTime <= gpsTime.GetTime()){
				nRet =Updata(pGpsInfo,doubleLongitude,doubleLatitude,str_GpsTime,str_time_Servers,judge_ret);
				if(nRet <1){
					nRet = nRet -2000;
				}else{
					pDI->dLastUpdate_SQLTime = gpsTime.GetTime();
				}
			}
		}
		else if(RET_OVERAREA_OUT==judge_ret || RET_OVERAREA_IN==judge_ret)
		{
			//越界报警
			nRet = Insert_Transgress_Data(pGpsInfo,doubleLongitude,doubleLatitude,str_GpsTime,str_time_Servers,pDI);
			if(nRet < 1)
				nRet = nRet -4000;
			break;
		}
		else if(RET_OVERSPEED==judge_ret)
		{
			//超速 插入超速表
			//这里需要车ID
			nRet = Insert_OverSpeed_Data(pGpsInfo,doubleLongitude,doubleLatitude,str_GpsTime,str_time_Servers,pDI);
			if(nRet < 1)
				nRet = nRet -3000;
			break;
		}
	}while(0);

	if(g_WriteData_Cnt++ %100==0 || nRet < 1)
	{

		CString strLog;
		strLog.Format(_T("sim=%I64d-bValid=%d--judge_ret=%d--lng=%.4f  lat=%.4f speed=%s gpsTime=%s recTime=%s g_WriteData_Cnt=%I64d--nret=%d,m_bInitOCI=%d"),
			iSim,pGpsInfo->bValid,
			judge_ret,doubleLongitude,doubleLatitude,
			CString(pGpsInfo->Speed),
			CString(pGpsInfo->Time),CString(str_GpsTime),g_WriteData_Cnt,nRet,m_bInitOCI);
		WriteLog(LOGNAME,logLevelInfo,strLog);	

		struct tm pCurGpsTm,t2;
		gpsTime.GetLocalTm(&pCurGpsTm);
		int nNeedCreate = NeedCreateNewTable(m_tm_TableNameTime,&pCurGpsTm);
		strLog.Format(_T("sim=%I64d time-->pre:[%d-%d-%d] cur:[%d-%d-%d]---date:%s--%d"),
			iSim,
			m_tm_TableNameTime.tm_year,m_tm_TableNameTime.tm_mon,m_tm_TableNameTime.tm_mday,
			pCurGpsTm.tm_year,pCurGpsTm.tm_mon,pCurGpsTm.tm_mday,
			CString(m_strDate),
			nNeedCreate);
		WriteLog(LOGNAME,logLevelInfo,strLog);
	}
	return nRet;
}
int COracleOCI_o::Insert_ODBInfo_Data(const GPSINFO *pGpsInfo,CStringA str_CurTime)
{
	/*
	TID        VARCHAR2(48),
		SAVED_DATE DATE,
		OBD_TYPE   NUMBER(2),
		CONTENT    VARCHAR2(1000)
/**/
	if(pGpsInfo->nHaveOBDInfo==0) return 0;
	const tagOBDInfo *pstOBDInfo = &(pGpsInfo->st_OBD_Info);

//	obd_tyep 1是元征 2是美丽高
	int nOBD_Type = 2;
	char odbInfo;

	/*
	<发动机转速>,<发动机计算负荷>,<发动机冷却液温度>,
	<百公里油耗>,<进气温度>,<油压>,<大气压力>,<进气管绝
		对压力>,<空气流量>,<节气门位置>,<单次行驶里程>,<百分比剩余油量>,<故障码及冻结帧/就绪状态>
	
	*/
	/*
	sprintf(odbInfo,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
		pstOBDInfo->Engine_speed,
		pstOBDInfo->Engine_load,
		pstOBDInfo->Engine_coolant_temperature,
		pstOBDInfo->Hundred_kilometers,
		pstOBDInfo->Intake_air_temperature,
		pstOBDInfo->Hydraulic,
		pstOBDInfo->Atmospheric_pressure,
		pstOBDInfo->Intake_pressure,
		pstOBDInfo->Air_flow,
		pstOBDInfo->Throttle_Position,
		pstOBDInfo->Single_mileage,
		pstOBDInfo->Remaining_fuel,
		pstOBDInfo->ErrorCode
/**/
	sprintf(m_strInsertDataSQL,"Insert into  OBD_DETAIL%s \
							          (TID,saved_date,obd_type,content) \
									  Values('%s',TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),'%d','%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s')",
									  m_strDate,
									  pGpsInfo->COMMADDR,
									  str_CurTime,
									  nOBD_Type,
									  pstOBDInfo->Engine_speed,
									  pstOBDInfo->Engine_load,
									  pstOBDInfo->Engine_coolant_temperature,
									  pstOBDInfo->Hundred_kilometers,
									  pstOBDInfo->Intake_air_temperature,
									  pstOBDInfo->Hydraulic,
									  pstOBDInfo->Atmospheric_pressure,
									  pstOBDInfo->Intake_pressure,
									  pstOBDInfo->Air_flow,
									  pstOBDInfo->Throttle_Position,
									  pstOBDInfo->Single_mileage,
									  pstOBDInfo->Remaining_fuel,
									  pstOBDInfo->ErrorCode);
	return DoInsertSQL(m_strInsertDataSQL);
}
int COracleOCI_o::Insert_Transgress_Data( const GPSINFO *pGpsInfo ,double doubleLongitude,double doubleLatitude,CStringA s_RecvTime ,CStringA str_CurTime,DataInfo *pDI )
{
	BOOL bSim	=	pGpsInfo->nDevID&0x0100;
	BOOL bTid	=	pGpsInfo->nDevID&0x0200;

	sprintf(m_strInsertDataSQL,"Insert into TRANSGRESS_ALERTING (TRANSGRESS_ALERTING_ID,VEHICLEID,GPSTIME,RECVTIME,LNG,LAT,veo,direct) \
							   Values(emp_sequence.nextval,\
							   '%s',\
							   TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),\
							   TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),    \
							   %.5f, %.5f,%s,%s)",
							   pDI->vehicheID,
							   s_RecvTime,
							   str_CurTime,
							   doubleLongitude,doubleLatitude,
							   pGpsInfo->Speed,
							   pGpsInfo->Heading);

	return DoInsertSQL(m_strInsertDataSQL);
}
int COracleOCI_o::Insert_OverSpeed_Data( const GPSINFO *pGpsInfo ,double doubleLongitude,double doubleLatitude,CStringA s_RecvTime ,CStringA str_CurTime,DataInfo *pDI )
{
	BOOL bSim	=	pGpsInfo->nDevID&0x0100;
	BOOL bTid	=	pGpsInfo->nDevID&0x0200;

	sprintf(m_strInsertDataSQL,"Insert into OVERSPEED_ALERTING (OVERSPEED_ALERTING_ID,vhcid,CREATETIME,BEGIN_TIME,MAXOVERSP,LNG,LAT) \
							   Values(emp_sequence.nextval,'%s',TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),    \
							   %s,%.5f, %.5f)",
							   pDI->vehicheID,
							   str_CurTime,
							   s_RecvTime,
							   pGpsInfo->Speed,
							   doubleLongitude,doubleLatitude);

	return DoInsertSQL(m_strInsertDataSQL);
}

int COracleOCI_o::InsertData( const GPSINFO *pGpsInfo ,double doubleLongitude,double doubleLatitude,CStringA s_RecvTime ,CStringA str_CurTime,int iState)
{
	
	BOOL bSim	=	pGpsInfo->nDevID&0x0100;
	BOOL bTid	=	pGpsInfo->nDevID&0x0200;


	sprintf(m_strInsertDataSQL,"Insert into GPS_%s (SIM,tid,GPS_DATE,RECV_DATE,LNG,LAT,VEO,DIRECT,TASKID,ISTATE,NOLOAD,mileage) \
							   Values('%s','%s',  TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),    \
							   TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),     \
							   %.5f, %.5f, %s, %s,%d,%d,%d,%d)",
							   m_strDate,
							   bSim?pGpsInfo->COMMADDR:"",
							   bSim?"":pGpsInfo->COMMADDR,
							   s_RecvTime,
							   str_CurTime,
							   doubleLongitude,doubleLatitude,
							   pGpsInfo->Speed,
							   (pGpsInfo->Heading),
							   hex2dec(pGpsInfo->VERFYCODE),
							   iState,
							   pGpsInfo->Noload,
							   pGpsInfo->st_OBD_Info.Mileage);

	if(DoInsertSQL(m_strInsertDataSQL,FALSE)==1)
		return 1;

	sprintf(m_strInsertDataSQL,"Insert into GPS_%s (SIM,tid,GPS_DATE,RECV_DATE,LNG,LAT,VEO,DIRECT,TASKID,ISTATE,NOLOAD) \
							   Values('%s','%s',  TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),    \
							   TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),     \
							   %.5f, %.5f, %s, %s,%d,%d,%d)",
							   m_strDate,
							   bSim?pGpsInfo->COMMADDR:"",
							   bSim?"":pGpsInfo->COMMADDR,
							  s_RecvTime,
							  str_CurTime,
							  doubleLongitude,doubleLatitude,
							  pGpsInfo->Speed,
							  (pGpsInfo->Heading),
							  hex2dec(pGpsInfo->VERFYCODE),iState,pGpsInfo->Noload);

	if(DoInsertSQL(m_strInsertDataSQL,FALSE)==1)return 1;

	sprintf(m_strInsertDataSQL,"Insert into GPS_%s (SIM,tid,GPS_DATE,RECV_DATE,LNG,LAT,VEO,DIRECT,TASKID,ISTATE) \
							   Values('%s','%s',  TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),    \
							   TO_DATE('%s', 'YYYY-MM-DD HH24:MI:SS'),     \
							   %.5f, %.5f, %s, %s,%d,%d)",
							   m_strDate,
							   bSim?pGpsInfo->COMMADDR:"",
							   bSim?"":pGpsInfo->COMMADDR,
							   s_RecvTime,
							   str_CurTime,
							   doubleLongitude,doubleLatitude,
							   pGpsInfo->Speed,
							   (pGpsInfo->Heading),
							   hex2dec(pGpsInfo->VERFYCODE),
							   iState);
	return DoInsertSQL(m_strInsertDataSQL);
}
int COracleOCI_o::DoInsertSQL(char *pInsertDataSQL,BOOL bShowErrorInfo/*=TRUE*/)
{
	if(!m_bInitOCI) return 0;
	OutputDebugStringA(pInsertDataSQL);
	OutputDebugString(_T("\r\n"));
	sword status;
	OCIBind  *bnd1p = (OCIBind *) 0;             /* the first bind handle */
	OCIBind  *bnd2p = (OCIBind *) 0;             /* the second bind handle */

	status = OCIStmtPrepare(conn.mystmthp, conn.myerrhp, (const unsigned    char * )pInsertDataSQL,
		(ub4) strlen(pInsertDataSQL),
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
	if (status != OCI_SUCCESS)
	{
		if(bShowErrorInfo)
		{
			printf_ErrorLog(_T("OCIStmtExecute"));
			WriteLog(LOGNAME,logLevelError,CString(m_strInsertDataSQL));
		}
		return -2;
	}
	

	//id[0]=1;id[1]=2;
	//这里一次可以插入2行数据
	status = OCIStmtExecute(conn.mysvchp, conn.mystmthp, conn.myerrhp, 1, 0, NULL, NULL, OCI_DEFAULT);

	if (status != OCI_SUCCESS)
	{
		if(bShowErrorInfo)
		{
			printf_ErrorLog(_T("OCIStmtExecute"));
			WriteLog(LOGNAME,logLevelError,CString(m_strInsertDataSQL));
		}
		return -3;
	}
	conn.commit();
	return 1;
}
int COracleOCI_o::Update_CommandState(char * pUpdateSql)
{
	sword status;
	status = OCIStmtPrepare(conn.mystmthp, conn.myerrhp, (const unsigned    char * )pUpdateSql,
		(ub4) strlen(pUpdateSql),
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

	if (status != OCI_SUCCESS)
	{
		printf_ErrorLog(_T("OCIStmtPrepare"));
		WriteLog(LOGNAME,logLevelError,CString(pUpdateSql));
		return -2;
	}
	status = OCIStmtExecute(conn.mysvchp, conn.mystmthp, conn.myerrhp, 1, 0, NULL, NULL, OCI_DEFAULT);

	if (status != OCI_SUCCESS)
	{
		printf_ErrorLog(_T("OCIStmtExecute"));
		WriteLog(LOGNAME,logLevelError,CString(pUpdateSql));
		return -3;
	}

// 	if(1!=prepare_Execute_SQL(conn,m_strUpdateDataSQL_ForCmd))
// 	{
// 		printf_ErrorLog(L"Update_CommandState");
// 		WriteLog(LOGNAME,logLevelError,CString(m_strUpdateDataSQL_ForCmd));
// 	}
	//WriteLog(LOGNAME,logLevelError,CString(m_strUpdateDataSQL_ForCmd));
	conn.commit();
	return 1;

}
DataInfo * COracleOCI_o::GetVehicleInfo( const char *pStrDevID )
{
	INT64 iSim = _atoi64(pStrDevID);
	std::map<INT64,DataInfo*>::iterator it = m_DataInfoMap.find(iSim);

	if(it!=m_DataInfoMap.end())
	{
		return (DataInfo*)it->second;
	}
	return NULL;
}
int COracleOCI_o::writeCommand( GPSCommand * pGpsCommand )
{
	CmdType cmdType = pGpsCommand->commandType;
	int nRet = 0;
	if(cmdType == cmdType_ToService_OverSpeed)
	{
		//超速设置
		//需要找到对应的车辆，然后更新内存数据
	//	strDevID
		DataInfo *pDI = GetVehicleInfo(pGpsCommand->strDevID);
		if(pDI==NULL)
			pDI = GetVehicleInfo(pGpsCommand->strSim);

		if(pDI!=NULL)
		{
			struct tag_OverSpeed 
			{
				int nMaxSpeed;
				int nMinSpeed;
				int nCountinue;
			};
			tag_OverSpeed *pOVerSpeed = (tag_OverSpeed*)pGpsCommand->pVoid;			
			CString strLog;
			strLog.Format(_T("writeCommand %s overspeed old:%d,new:%d"),pGpsCommand->strDevID,pDI->dwMaxSpeed,pOVerSpeed->nMaxSpeed);
			WriteLog(_T("TSCmd"),logLevelError,strLog);
			pDI->dwMaxSpeed = pOVerSpeed->nMaxSpeed;


			sprintf(m_strUpdateDataSQL_ForCmd,"update VEHICLE set SETSTATE=1 WHERE VEHICLE_ID='%s'",								   
				pDI->vehicheID);

			nRet = Update_CommandState(m_strUpdateDataSQL_ForCmd);

			delete pGpsCommand->pVoid;
			pGpsCommand->pVoid = NULL;			
		}
	}
	else if(cmdType == cmtType_ToGPS_Setarea)//暂时作为越界命令来使用
	{
		//DataInfo *pDI = GetVehicleInfo(pGpsCommand->strDevID);
		//if(pDI==NULL)return -1;
		int nRecordID = (int)pGpsCommand->pVoid ;
		//收到越界命令，需要通知界面UI层等待。如果gps 操作成功 就把这条记录删除掉。否则执行失败。
		sprintf(m_strUpdateDataSQL_ForCmd,"update REGIONL_SET set STATE=1 WHERE id=%d",nRecordID);
		nRet = Update_CommandState(m_strUpdateDataSQL_ForCmd);
	} 
	else if(cmdType == CmdType(cmdType_ToGPS+1))
	{
		int nRecordID = (int)pGpsCommand->pVoid ;

		//在这里把命令保存到map中去
		CommandItem* cmdItem=new CommandItem();
		cmdItem->nCmdType = cmdType;
		strcpy(cmdItem->devID,pGpsCommand->strDevID);
		strcpy(cmdItem->simID,pGpsCommand->strSim);
		DataInfo *pDI = GetVehicleInfo(pGpsCommand->strDevID);
		if(pDI==NULL)
			pDI = GetVehicleInfo(pGpsCommand->strSim);
		if(pDI ==NULL)
			return 0;

		strcpy(cmdItem->vehicle_ID,pDI->vehicheID);
		cmdItem->recordID = nRecordID;
		strcpy(cmdItem->cmdID ,pGpsCommand->strCMDID);

		memcpy(cmdItem->cmdParam,pGpsCommand->commandParameters,pGpsCommand->nLenCmdParameters);
		m_ArrayCommandItem.Add(cmdItem);


	}
	return nRet;
}
