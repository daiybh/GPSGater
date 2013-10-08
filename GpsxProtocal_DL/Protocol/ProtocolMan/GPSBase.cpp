/********************************************************************
	created:	2012/01/19
	created:	19:1:2012   15:34
	filename: 	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps\GPSBase.cpp
	file path:	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps
	file base:	GPSBase
	file ext:	cpp
	author:		areslion@qq.com
	
	purpose:	
*********************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include "GpsBase.h"
//#include "NantianPR.h"

#include "MGTOOls.h"
#include <time.h>
#include "GpsJTT808.h"
#include "GpsXingRui.h"
#include "GPS_MeiTrack.h"
#include "GpsYouHao.h"

// long GPSClass::getGpsInfo( char *buf,GPSINFO &gpsInfo )
// {
// 	return 0;	
// }

GpsYouHao	m_gpsYouHao;
GpsXingRui	m_gpsRuiXing;
GPS_MeiTrack	m_gpsMeiTrack;
GpsJTT808		m_gpsJTT808;

int GPSClass::getbin(int x)
{
	if(x>='0' && x<='9')
		return x-'0';

	if(x>='A' && x<='F')
		return x-'A'+10;
	return x-'a'+10;
}
int GPSClass::HexToBin(char *pHex,char **ppBin)
{
	int nLen = strlen(pHex);
	char *pBin = *ppBin;
	int nBinLen = 0;
	for(int i=0;i<nLen;)
	{
		char p10=pHex[i];
		if(p10==' '){
			i++;
			continue;
		}
		char p00=pHex[i+1];
		*pBin++ = getbin(p10)*16+getbin(p00);
		i+=2;
		nBinLen++;
	}
	return nBinLen;
}
GPSClass * GPSClass::getProtocol( char *buf,GPSINFO *pGpsInfo )
{
	if(m_gpsMeiTrack.isThisProtocol(buf,pGpsInfo))return &m_gpsMeiTrack;
	else if(m_gpsRuiXing.isThisProtocol(buf,pGpsInfo))return &m_gpsRuiXing;
	else if(m_gpsJTT808.isThisProtocol(buf,pGpsInfo))return &m_gpsJTT808;
	return NULL;
}
int GPSClass::getCheckCode(const char *pstrBuf,int nStrLen,char *strCheckCode)
{
	long nCode = 0;
	for(int i=0;i<nStrLen;i++)
	{
		nCode += (pstrBuf[i]);
	}
	sprintf(strCheckCode,"%.2X",nCode&0xFF);
	return nCode&0xFF;
}
void GPSClass::getCheckCode( char *strBuf,char *strCheckCode )
{
	int i = 0;
	long nCode = 0;
	char strTmp[10]="";

	while(strBuf[i]!=0x00)
	{
		nCode += strBuf[i];
		i++;
	}
	sprintf(strTmp,"%x",nCode);
	//if(strlen(strCheckCode)%2) sprintf(strTmp,"0%s",strCheckCode);

	strcpy(strCheckCode,strupr(strTmp));
}


char strGPSTab[][30]={
"nDevID",
"nMsgID",
"PreFix[N_LEN]",//The UDP head
"SEQ[N_LEN]",//Serial number
"COMMADDR[N_LEN]",//SIM NO.
"CMDID[N_LEN]",//command ID
"CMDARGUS[N_LEN]",//commad parameters
"Time[N_LEN]",// 报文产生时间
"UTC[N_LEN]",//
"Latitude[N_LEN]",//纬度，单位为毫秒 16进制ASCII字符串
"Longitude[N_LEN]",//经度，单位为毫秒 16进制ASCII字符串
"Altitude[N_LEN]",//海拔高度，单位为毫秒 16进制ASCII字符串
"Heading[N_LEN]",//方向，单位度，值范围0-360 16进制ASCII字符串
"Speed[N_LEN]",//速度	16进制ASCII字符串	A6B	为单位m/h
"TransactionFlag[N_LEN]",//报警状态字	以","分隔的一或多个16进制ASCII字符串
"OEMStatus[N_LEN]",//扩展状态字
"StatusString[N_LEN]",// 终端状态描述	以","分隔的终端描述信息
"VERFYCODE[N_LEN]",// Verify codes
"CRLF[N_LEN]"//CR+LF
};

void buf2HexStr(const char *pSrcbuf,char *pDestBuf,int nLen)
{
	if(pSrcbuf[0] =='*')
		return;
	if( nLen> 1024/2)	nLen = 1024;
	char *pPos = pDestBuf;
	*pPos = '_';
	pPos++;
	for(int i=0;i<nLen;i++)
	{
		sprintf(pPos,"%02x",(unsigned char)pSrcbuf[i]);
		pPos+=2;
	}	
}

GPSClass::GPSClass()
{
	memset(m_strConsole,0x00,64);
	memset(m_strProtocal,0x00,64);
	memset(m_strInterval,0x00,5);


	strcpy(m_strConsole,"Console");
	strcpy(m_strInterval,"60");

	CoInitialize(NULL);
	I_XMLParser::CreateInstance(&m_pXmlParser);

	m_pXmlParser->Create();
	m_i64RecvCnt =0;
}

const char * GPSClass::find_Comma(const char *srcStr,int &nLen)
{	
	const char * pdest = strchr(srcStr,',');
	nLen = 0;
	if(pdest!=NULL)
	{
		nLen =  (int)(pdest - srcStr );//实际字符串的长度
		pdest+=1;

	}
	return pdest;
}
const char * GPSClass::getStr_betweenComma(const char *pBuf,char *destStr)
{
	//获取两个逗号之间的字符
	const char * pComma1;
	int nLen=0;
	pComma1 = find_Comma(pBuf,nLen);
	strncpy(destStr,pBuf,nLen);
	destStr[nLen]='\0';
	return pComma1;
}
struct tm *GPSClass::GetLocalTime()
{
	time_t utc_time;
	utc_time = time(NULL);
	struct tm* local_time =NULL;
	local_time = localtime(&utc_time);
	return local_time;
}
#include <math.h>
long GPSClass::handleCmd_SetArea(GPSCommand *pGpsCommand)
{
	TCHAR pTemp[1000];
	memset(pTemp,0,1000);
	//*XX,YYYYYYYYYY,S21,HHMMSS,Lx,M,D,Minlatitude,Maxlatitude,G,Minlongitude,Maxlongitude#
	//*HQ,6110916012,S21,130305,17,1,N,2245.318,2246.452,E,11233.232,11365.175#			
	/*
	*	
	-------------  right(max,max)
	|			|
	|			|
	|			|
	|			|
	-------------
	left(min,min)
	*/
	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	char alertType[5] ;
	strcpy(alertType,m_pXmlParser->GetNodeText(_T("alerttype"),pTemp));
	//默认为 出区域报警

	if(strcmp(alertType ,"1")==0)//进区域报警
	{
		//alertType= _T("3");
		//alertType= _T("4");
		strcpy(alertType,"4");
	}
	else
	{
		//alertType= _T("1");
		//alertType= _T("2");
		strcpy(alertType,"2");
	}

	char *pAreaID = pTemp;
	int nLen = m_pXmlParser->GetNodeText_len("areaid",pAreaID);	
	sprintf(pGpsCommand->strCMDID,"s21_1_%s",pAreaID);

	char *pLeftLat = pAreaID+nLen+1;
	nLen = m_pXmlParser->GetNodeText_len(_T("leftlat"),pLeftLat);


	char *pleftlng = pLeftLat+nLen+1;
	nLen = m_pXmlParser->GetNodeText_len(_T("leftlng"),pleftlng);

	char *prightlat = pleftlng+nLen+1;
	nLen = m_pXmlParser->GetNodeText_len(_T("rightlat"),prightlat);


	char *prightlng = prightlat+nLen+1;
	nLen = m_pXmlParser->GetNodeText_len(_T("rightlng"),prightlng);

	char *pType = prightlng+nLen+1;
	nLen = m_pXmlParser->GetNodeText_len(_T("type"),pType);


	char *pcenterlat = pType+nLen+1;
	nLen = m_pXmlParser->GetNodeText_len(_T("centerLocation_lat"),pcenterlat);

	char * pcenterlng = pcenterlat+nLen+1;
	nLen = m_pXmlParser->GetNodeText_len(_T("centerLocation_lng"),pcenterlng);

	char *pRadius = pcenterlng+nLen+1;
	nLen = m_pXmlParser->GetNodeText_len(_T("radius"),pRadius);

	{
		//可以考虑把矩形转换为圆形, 圆是矩形的外周圆（四个顶点在圆上）

		double dleftlat = atof(pLeftLat);
		double dleftlng = atof(pleftlng);
		double drightlat = atof(prightlat);
		double drightlng = atof(prightlng);
		//中心坐标
		//*
		double dcenterlat = (dleftlat+drightlat)/2;
		double dcenterlng = (dleftlng+drightlng)/2;

		//半径:
		//√（pleftlat-prightlat）2+(prightlng-pleftlng)2

		double dRadius = sqrt(pow((dleftlat-drightlat),2)+pow((drightlng-dleftlng),2));

		gcvt(dcenterlat,7,pcenterlat);
		nLen = strlen(pcenterlat);
		pcenterlng = pcenterlat+nLen+1;
		gcvt(dcenterlng,8,pcenterlng);
		nLen = strlen(pcenterlng);
		pRadius = pcenterlng+nLen+1;
		gcvt(dRadius,10,pRadius);

	}
	_handleCmd_SetArea(pGpsCommand,pAreaID,alertType,pType,pLeftLat,prightlat,pleftlng,prightlng,pcenterlat,pcenterlng,pRadius);
	

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1]='\0';

	memset(pTemp,0,10);
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));	
	pGpsCommand->pVoid = (int*)nRecordID;
	return pGpsCommand->nLenCommandLine;
}
long GPSClass::handleCmd_CacelArea(GPSCommand*pGpsCommand)
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	sprintf(pGpsCommand->strCMDID,"s21_0_%s",m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
	TCHAR *pAreaid = pTemp;
	m_pXmlParser->GetNodeText(_T("areaid"),pTemp);
	_handleCmd_CacelArea(pGpsCommand,pAreaid);
	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_DirectToGPS_command( GPSCommand *pGpsCommand )
{
//DirectToGPS_command

	TCHAR pTemp[100];
	//strcpy(pGpsCommand->strCommandLine,m_pXmlParser->GetNodeText(_T("command"),pTemp));
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+2);
	//<directtogps_command><deviceid></deviceid><command></command><toBin>1</toBin></directtogps_command>
	BOOL bNeedToBin = atoi(m_pXmlParser->GetNodeText("tobin",pTemp));
	char *pDestBuffer = pGpsCommand->strCommandLine;

	if(bNeedToBin) pDestBuffer = pTemp;

	int nLen = m_pXmlParser->GetNodeText_len(_T("command"),pDestBuffer);
	if(bNeedToBin){
		nLen = HexToBin(pDestBuffer,&pGpsCommand->strCommandLine);
	}

	pGpsCommand->nLenCommandLine = (nLen==1)?strlen(pGpsCommand->strCommandLine):nLen;

	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_overspeed(GPSCommand*pGpsCommand)
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToService+1);
	//*XX,YYYYYYYYYY,S14,HHMMSS,Max_speed,Min_speed,M,countinue #
	//*HQ,0000000000,S14,130305,100,10,1,3#			

	strcpy(pGpsCommand->strCMDID,"S14");

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText(_T("sim"),pTemp));			

	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));


	strcpy(pGpsCommand->strCMDID,(m_pXmlParser->GetNodeText(_T("id"),pTemp)));

	struct tag_OverSpeed 
	{
		int nMaxSpeed;
		int nMinSpeed;
		int nCountinue;
	};

	tag_OverSpeed *pstOverSpeed = new tag_OverSpeed;

	pstOverSpeed->nMaxSpeed = atoi(m_pXmlParser->GetNodeText(_T("maxspeed"),pTemp));
	pstOverSpeed->nMinSpeed = atoi(m_pXmlParser->GetNodeText(_T("minspeed"),pTemp));
	pstOverSpeed->nCountinue = atoi(m_pXmlParser->GetNodeText(_T("countinue"),pTemp));
	int nValid = atoi(m_pXmlParser->GetNodeText("m",pTemp));

	int nLen = _handleCmd_overspeed(pGpsCommand,pstOverSpeed->nMaxSpeed,pstOverSpeed->nMinSpeed,pstOverSpeed->nCountinue);

	
	pGpsCommand->pVoid = pstOverSpeed;	

	pGpsCommand->nLenCommandLine = (nLen==1)?strlen(pGpsCommand->strCommandLine):nLen;
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';			
	return pGpsCommand->nLenCommandLine;
}
long GPSClass::getConsole2GPSData( const char *fromConsole_srcBuf,GPSCommand *pGpsCommand )
{	
	TCHAR chXMlValue[100];
	if(!m_pXmlParser->Parse(fromConsole_srcBuf))
		return 0;
	int nLen = m_pXmlParser->GetRootName(chXMlValue);
	int nRet = -1;
	do{
		TCHAR*pTemp = chXMlValue;
		// 解析各种命令
		if(strcmp(chXMlValue,"directtogps_command")==0)
		{			
			nRet = handleCmd_DirectToGPS_command(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"setarea")==0)
		{
			nRet = handleCmd_SetArea(pGpsCommand);			
		}
		else if(strcmp(chXMlValue,"cancelarea")==0)
		{
			nRet = handleCmd_CacelArea(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"overspeed")==0)
		{
			nRet = handleCmd_overspeed(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"sethearttime")==0)
		{
			nRet = handleCmd_SetHeartTime(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"settimingtrack")==0)
		{
			nRet = handleCmd_SetTimingTrack(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"setturnreport")==0)
		{
			nRet = handleCmd_SetTurnReport(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"setgprsparam")==0)
		{
			nRet = handleCmd_SetGPRSParam(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"set_backgprsserver")==0)
		{
			nRet = handleCmd_Set_BackGPRSServer(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"get_errorcode")==0)
		{
			nRet = handleCmd_Get_ErrorCode(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"set_vehicletype")==0)
		{
			nRet = handleCmd_Set_VehicleType(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"get_vehicletype")==0)
		{
			nRet = handleCmd_Get_VehicleType(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"set_openobd")==0)
		{
			nRet = handleCmd_Set_OpenOBD(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"set_obdparam")==0)
		{
			nRet = handleCmd_Set_OBDParam(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"set_oil_lowalarm")==0)
		{
			nRet = handleCmd_Set_Oil_LowAlarm(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"get_device_version_and_sn")==0)
		{
			nRet = handleCmd_Get_Device_Version_and_SN(pGpsCommand);
		}
		else if(strcmp(chXMlValue,"set_reset_mileage_and_runtime")==0)
		{
			nRet = handleCmd_Set_Reset_Mileage_and_Runtime(pGpsCommand);
		}
		else 
		{
			break;
		}
	}while(0);

	return nRet;
}

long GPSClass::handleCmd_SetHeartTime( GPSCommand *pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	//sprintf(pGpsCommand->strCMDID,"s21_0_%s",m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
	TCHAR *pInterval = pTemp;
	m_pXmlParser->GetNodeText(_T("interval"),pTemp);
	_handleCmd_SetHeartTime(pGpsCommand,pInterval);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_SetTimingTrack( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	//sprintf(pGpsCommand->strCMDID,"s21_0_%s",m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
	TCHAR *pInterval = pTemp;
	int nLen = m_pXmlParser->GetNodeText_len(_T("interval"),pInterval);
	TCHAR *pReportCount = pInterval+nLen;
	nLen = m_pXmlParser->GetNodeText_len(_T("reportcount"),pReportCount);

	_handleCmd_SetTimingTrack(pGpsCommand,pInterval,pReportCount);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_SetTurnReport( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	//sprintf(pGpsCommand->strCMDID,"s21_0_%s",m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
	TCHAR *pAngle = pTemp;
	int nLen = m_pXmlParser->GetNodeText_len(_T("angle"),pAngle);
	
	_handleCmd_SetTurnReport(pGpsCommand,pAngle);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_SetGPRSParam( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	//sprintf(pGpsCommand->strCMDID,"s21_0_%s",m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
	TCHAR *pLinkMod = pTemp;
	int nLen = m_pXmlParser->GetNodeText_len(_T("linkmod"),pLinkMod);

	TCHAR *pIP = pLinkMod+nLen;
	nLen = m_pXmlParser->GetNodeText_len(_T("ip"),pIP);

	TCHAR *pPort = pIP+nLen;
	nLen = m_pXmlParser->GetNodeText_len(_T("port"),pPort);

	TCHAR *pAPN = pPort+nLen;
	nLen = m_pXmlParser->GetNodeText_len(_T("apn"),pAPN);

	TCHAR *pAPN_UserName = pAPN+nLen;
	nLen = m_pXmlParser->GetNodeText_len(_T("apn_username"),pAPN_UserName);

	TCHAR *pAPN_PassWord = pAPN_UserName+nLen;
	nLen = m_pXmlParser->GetNodeText_len(_T("apn_password"),pAPN_PassWord);

	_handleCmd_SetGPRSParam(pGpsCommand,pLinkMod,pIP,pPort,pAPN,pAPN_UserName,pAPN_PassWord);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_Set_BackGPRSServer( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	//sprintf(pGpsCommand->strCMDID,"s21_0_%s",m_pXmlParser->GetNodeText(_T("areaid"),pTemp));

	TCHAR *pIP = pTemp;
	int nLen = m_pXmlParser->GetNodeText_len(_T("ip"),pIP);

	TCHAR *pPort = pIP+nLen;
	nLen = m_pXmlParser->GetNodeText_len(_T("port"),pPort);

	_handleCmd_Set_BackGPRSServer(pGpsCommand,pIP,pPort);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_Get_ErrorCode( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));

	_handleCmd_Get_ErrorCode(pGpsCommand);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_Set_VehicleType( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));

	TCHAR *pVehicleType = pTemp;
	int nLen = m_pXmlParser->GetNodeText_len(_T("vehicletype"),pVehicleType);
	_handleCmd_Set_VehicleType(pGpsCommand,pVehicleType);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_Get_VehicleType( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));

	_handleCmd_Get_VehicleType(pGpsCommand);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_Set_OpenOBD( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	TCHAR *pFlag = pTemp;
	int nLen = m_pXmlParser->GetNodeText_len("flag",pFlag);
	_handleCmd_Set_OpenOBD(pGpsCommand,pFlag);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_Set_OBDParam( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	
	int nLen = m_pXmlParser->GetNodeText_len("paramcount",pTemp);
	int nParamCount = atoi(pTemp);
// 	int *pArrParams = new int[nParamCount*3];
// 	ZeroMemory(pArrParams,sizeof(int)*nParamCount*3);
	
	struct tagInnerStruct 
	{
		int nType;
		int value_xx;
		int value_yy;
	};
	tagInnerStruct *pstArrParams= new tagInnerStruct[nParamCount];
	
	int intArrParams[18][3];
	ZeroMemory(intArrParams,sizeof(int)*18*3);
	for (int i=0;i<nParamCount;i++)
	{
		int nType = atoi(m_pXmlParser->GetNodeText("type",pTemp,i));
		int value_xx = atoi(m_pXmlParser->GetNodeText("value_xx",pTemp,i));
		int value_yy = atoi(m_pXmlParser->GetNodeText("value_yy",pTemp,i));
		
		pstArrParams[i].nType = nType;
		pstArrParams[i].value_xx = value_xx;
		pstArrParams[i].value_yy = value_yy;		
		if(nType>0)
		{
			int idx = nType-1;
			idx=i;
			intArrParams[idx][0] = nType;
			intArrParams[idx][1] = value_xx;
			intArrParams[idx][2] = value_yy;
		}
	}
	_handleCmd_Set_OBDParam(pGpsCommand,nParamCount,pstArrParams);
	
	delete []pstArrParams;
	//_handleCmd_Set_OBDParam(pGpsCommand,nParamCount,intArrParams);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_Set_Oil_LowAlarm( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	TCHAR *poilvalue = pTemp;
	int nLen = m_pXmlParser->GetNodeText_len("oilvalue",poilvalue);
	_handleCmd_Set_Oil_LowAlarm(pGpsCommand,poilvalue);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_Set_Reset_Mileage_and_Runtime( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	
	_handleCmd_Set_Reset_Mileage_and_Runtime(pGpsCommand);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

long GPSClass::handleCmd_Get_Device_Version_and_SN( GPSCommand*pGpsCommand )
{
	TCHAR pTemp[100];
	pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);

	strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
	strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
	TCHAR *psetType = pTemp;
	int nLen = m_pXmlParser->GetNodeText_len("settype",psetType);
	_handleCmd_Get_Device_Version_and_SN(pGpsCommand,psetType);

	pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
	pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';	
	int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
	pGpsCommand->pVoid = (int*)nRecordID;		
	return pGpsCommand->nLenCommandLine;
}

void GPSClass::Write_Log( char*pLogName,const char *pLogContent )
{
	::WriteLog(pLogName,logLevelError,pLogContent);
}

void GPSClass::Write_Log( const char *pLogContent )
{
	::WriteLog(LOG_NAME,logLevelError,pLogContent);
}

