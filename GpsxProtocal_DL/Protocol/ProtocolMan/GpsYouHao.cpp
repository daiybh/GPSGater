// GpsYouHao.cpp: implementation of the GpsYouHao class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GpsYouHao.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GpsYouHao::GpsYouHao()
{
}

GpsYouHao::~GpsYouHao()
{
}


/************************************
// Method:    getGpsInfo
// FullName:  GpsYouHao::getGpsInfo
// Access:    public 
// Returns:   long
// Qualifier:
// Parameter: char * buf
// Parameter: GPSINFO & gpsInfo
Rerurn:
0	success to get information
1	the message type is not match
3	the GPSINFO parameter is not correct
************************************/
long GpsYouHao::getGpsInfo(char *buf,int nbufLen,GPSINFO &gpsInfo)
{
	long nret = 0;
// 	GPSINFO	gpsInfo;
	char strDown[512]="";
	BOOL bToGps = TRUE;

// 	memset(&gpsInfo,0x00,GPSINFOLEN);

	if(!gpsInfo.nMsgID)
	{
		getMsgID(buf,gpsInfo);
		bToGps = FALSE;
	}
	if(gpsInfo.nMsgID==MSG_NULL)	return E_MSG_DISMACH;

	switch (gpsInfo.nMsgID)
	{//Download data
	///////////////////////////////////////////////////////////////////     GPS→中心
	case MSG_LOGIN:
		nret = sychroLogInfo(buf,gpsInfo );
		break;
	case MSG_LOGOFF:
		nret = sychroLogOffInfo(buf,gpsInfo );
		break;
	case MSG_MAITAIN:
		nret = sychroMaitainInfo(buf,gpsInfo);
		break;
	case MSG_LOCATION:
		nret = sychroLocationInfo(buf,gpsInfo);
		break;
	case MSG_TERMINALFEEDBACK:
		nret = sychroTerminalFeedbackInfo(buf,gpsInfo );
		break;
	case MSG_AUTONAVIGATION:
		nret = sychroAutoNavigation(buf,gpsInfo);
		break;


	//////////////////////////////////////////////////////////////////////////     中心→GPS
	case MSG_RES_LOGIN:
		//YH{space}CR{space}:COMMADDR|N{space}VERFYCODE\r\n
		strcpy(buf,"YH CR :");
		strcat(buf,gpsInfo.COMMADDR);
		strcat(buf,"|N ");
		getCheckCode(&buf[7],strDown);
		strcat(buf,strDown);
		strcat(buf,"\x0D\x0A");
		break;
	case MSG_RES_KEEPMAITAIN:
		//YH{space}CR{space}:COMMADDR|K{space}VERFYCODE\r\n
		strcpy(buf,"YH CR :");
		strcat(buf,gpsInfo.COMMADDR);
		strcat(buf,"|K ");
		getCheckCode(&buf[7],strDown);
		strcat(buf,strDown);
		strcat(buf,"\x0D\x0A");
		break;
	case MSG_RES_LOGOFF:
		//YH{space}CR{space}:COMMADDR|T{space}VERFYCODE\r\n
		strcpy(buf,"YH CR :");
		strcat(buf,gpsInfo.COMMADDR);
		strcat(buf,"|T ");
		getCheckCode(&buf[7],strDown);
		strcat(buf,strDown);
		strcat(buf,"\x0D\x0A");
		break;
	case MSG_CALLSTOP:
		initDownData(gpsInfo);
		strcpy(gpsInfo.CMDID,"1");
		strcpy(gpsInfo.CMDARGUS,"0");
		break;
	case MSG_CALLONETIME:
		initDownData(gpsInfo);
		strcpy(gpsInfo.CMDID,"2");
		strcpy(gpsInfo.CMDARGUS,"0");
		break;
	case MSG_CALLNTIMES:
		initDownData(gpsInfo);
		strcpy(gpsInfo.CMDID,"3");
		strcpy(gpsInfo.CMDARGUS,gpsInfo.comPressFlag);//
		strcat(gpsInfo.CMDARGUS,";");//
		strcat(gpsInfo.CMDARGUS,gpsInfo.IntervalTime);//
		strcat(gpsInfo.CMDARGUS,";");//
		strcat(gpsInfo.CMDARGUS,gpsInfo.callTimes);//
		strcat(gpsInfo.CMDARGUS,";");//
		break;
	case MSG_CALLTIMER:
		initDownData(gpsInfo);
		strcpy(gpsInfo.CMDID,"2");
		strcpy(gpsInfo.CMDARGUS,gpsInfo.comPressFlag);//
		strcat(gpsInfo.CMDARGUS,";");//
		strcat(gpsInfo.CMDARGUS,gpsInfo.IntervalTime);//
		strcat(gpsInfo.CMDARGUS,"!");//
		strcat(gpsInfo.CMDARGUS,gpsInfo.IntervalTime);//
		break;
	case MSG_SETGPS:
		initDownData(gpsInfo);
        strcpy(gpsInfo.CMDID,"270");
		strcpy(gpsInfo.CMDARGUS,gpsInfo.IPV4);//
		strcat(gpsInfo.CMDARGUS,";");//
		strcat(gpsInfo.CMDARGUS,gpsInfo.PortNum);//
		strcat(gpsInfo.CMDARGUS,";");//
		strcat(gpsInfo.CMDARGUS,gpsInfo.APN);//
		break;
	case MSG_SETKEEPINTERVAL:
		initDownData(gpsInfo);
        strcpy(gpsInfo.CMDID,"271");
		strcpy(gpsInfo.CMDARGUS,gpsInfo.IntervalTime);//
		break;
	}

	if(gpsInfo.nMsgID>MSG_TOGPS&&gpsInfo.nMsgID<MSG_UNDEF)
	{
		formatDownloaData(strDown,gpsInfo);
		strcpy(buf,strDown);
	}
	

	return nret;
}



void GpsYouHao::getMsgID( char *buf,GPSINFO &gpsInfo )
{
	if (!strncmp(buf,"YH AN :",7)) gpsInfo.nMsgID = MSG_LOGIN;
	else if (!strncmp(buf,"YH AT :",7)) gpsInfo.nMsgID = MSG_LOGOFF;
	else if (!strncmp(buf,"YH AK :",7)) gpsInfo.nMsgID = MSG_MAITAIN;
	else if (!strncmp(buf,"YH AP :",7)) gpsInfo.nMsgID = MSG_LOCATION;
	else if (!strncmp(buf,"YH AR :",7)) gpsInfo.nMsgID = MSG_TERMINALFEEDBACK;	
	else gpsInfo.nMsgID = MSG_NULL;

}




/************************************
// Method:    sychroLogInfo
// FullName:  GpsYouHao::sychroLogInfo
// Access:    public 
// Returns:   long
// Qualifier:
// Parameter: char * buf
// Parameter: GPSINFO & gpsInfo
************************************/
long GpsYouHao::sychroLogInfo( char *buf,GPSINFO &gpsInfo )
{
	long	nlen = 0;
	long	i = 0;


	FillX(":",gpsInfo.PreFix);
	else return 2;

	FillX(" ",gpsInfo.COMMADDR);
	else return 2;
	
	FillX("|",gpsInfo.UTC);
	else return 2;
	
	FillX("|",gpsInfo.Latitude);//Latitude
	else return 2;
	
	FillX("|",gpsInfo.Longitude);//longtitude
	else return 2;
	
	FillX("|",gpsInfo.Altitude);//altitude
	else return 2;
	
	FillX("|",gpsInfo.Heading);//heading
	else return 2;
	
	FillX("|",gpsInfo.Speed);//speeding
	else return 2;
	
	FillX("|",gpsInfo.TransactionFlag);//TransactionFlag
	else return 2;
	
	FillX("|",gpsInfo.OEMStatus);//OEMStatus
	else return 2;
	
	FillX(" ",gpsInfo.StatusString);//StatuString
	else return 2;
	
	FillX("\x0d\x0a",gpsInfo.VERFYCODE);//Verfycode
	else return 2;

	strncpy(gpsInfo.CRLF,"\x0d\0a",2);
	
	
	return 0;
}

long GpsYouHao::sychroLogOffInfo( char *buf,GPSINFO &gpsInfo )
{
	return sychroLogInfo(buf,gpsInfo);
}


//YH{space}AK{space}:COMADDR{space}VERFYCODE\r\n
long GpsYouHao::sychroMaitainInfo( char *buf,GPSINFO &gpsInfo )
{
	long	nlen = 0;
	long	i = 0;
	
	
	FillX(":",gpsInfo.PreFix);
	else return 2;
	
	FillX(" ",gpsInfo.COMMADDR);
	else return 2;	
	
	FillX("\x0d\x0a",gpsInfo.VERFYCODE);//Verfycode
	else return 2;
	
	strncpy(gpsInfo.CRLF,"\x0d\0a",2);
	
	
	return 0;	
}


//YH{space}AP{space}:COMADDR{space}TIME|Latitude|Longitude|Altitude|Heading|Speed
//|TransactionFlag|OEMStatus|StatusString{space}VERFYCODE\r\n
long GpsYouHao::sychroLocationInfo( char *buf,GPSINFO &gpsInfo )
{
	long	nlen = 0;
	long	i = 0;
	
	
	FillX(":",gpsInfo.PreFix);
	else return 2;
	
	FillX(" ",gpsInfo.COMMADDR);
	else return 2;
	
	FillX("|",gpsInfo.Time);
	else return 2;
	
	FillX("|",gpsInfo.Latitude);//Latitude
	else return 2;
	
	FillX("|",gpsInfo.Longitude);//longtitude
	else return 2;
	
	FillX("|",gpsInfo.Altitude);//altitude
	else return 2;
	
	FillX("|",gpsInfo.Heading);//heading
	else return 2;
	
	FillX("|",gpsInfo.Speed);//speeding
	else return 2;
	
	FillX("|",gpsInfo.TransactionFlag);//TransactionFlag
	else return 2;
	
	FillX("|",gpsInfo.OEMStatus);//OEMStatus
	else return 2;
	
	FillX(" ",gpsInfo.StatusString);//StatuString
	else return 2;
	
	FillX("\x0d\x0a",gpsInfo.VERFYCODE);//Verfycode
	else return 2;
	
	strncpy(gpsInfo.CRLF,"\x0d\0a",2);
	
	
	return 0;
}


//YH{space}AR{space}:COMADDR|CMDID|CMDARGUS VERFYCODE\r\n
long GpsYouHao::sychroTerminalFeedbackInfo( char *buf,GPSINFO &gpsInfo )
{
	long	nlen = 0;
	long	i = 0;
	
	
	FillX(":",gpsInfo.PreFix);
	else return 2;
	
	FillX("|",gpsInfo.COMMADDR);
	else return 2;
	
	FillX("|",gpsInfo.CMDID);
	else return 2;
	
	FillX(" ",gpsInfo.CMDARGUS);//Latitude
	else return 2;
	
	FillX("\x0d\x0a",gpsInfo.VERFYCODE);//Verfycode
	else return 2;
	
	strncpy(gpsInfo.CRLF,"\x0d\0a",2);
	
	
	return 0;	
}

//技术文档定义不清晰
long GpsYouHao::sychroAutoNavigation( char *buf,GPSINFO &gpsInfo )
{
	//UNFUN
	return 0;
}


//YH{space}CM{space}SEQ{space}:COMMADDR|CMDID|CMDARGUS{space}VERIFYCODE\r\n
long GpsYouHao::sychroDownData( char *buf,GPSINFO &gpsInfo )
{
	long	nlen = 0;
	long	i = 0;
	
	
	FillX(":",gpsInfo.PreFix);
	else return 2;
	
	FillX("|",gpsInfo.COMMADDR);
	else return 2;
	
	FillX("|",gpsInfo.CMDID);
	else return 2;
	
	FillX(" ",gpsInfo.CMDARGUS);//Latitude
	else return 2;

	FillX("\x0d\x0a",gpsInfo.VERFYCODE);//Verfycode
	else return 2;
	
	strncpy(gpsInfo.CRLF,"\x0d\0a",2);

	return 0;	
}


//YH{space}CM{space}SEQ{space}:COMMADDR|CMDID|CMDARGUS{space}VERIFYCODE\r\n
void GpsYouHao::formatDownloaData(char *buf,GPSINFO &gpsInfo)
{
	int nPos = 0;
	char strCheckCode[10]="";

	strcpy(buf,gpsInfo.PreFix);
	//strcat(buf,gpsInfo.);
	strcat(buf,gpsInfo.SEQ);
	strcat(buf," :");
	nPos = strlen(buf);
	strcat(buf,gpsInfo.COMMADDR);
	strcat(buf,"|");
	strcat(buf,gpsInfo.CMDID);
	strcat(buf,"|");
	strcat(buf,gpsInfo.CMDARGUS);
	strcat(buf," ");
	getCheckCode(&buf[nPos],strCheckCode);
	strcat(buf,strCheckCode);
	strcat(buf,"\x0D\x0A");
}


void GpsYouHao::initDownData( GPSINFO &gpsInfo )
{
	strcpy(gpsInfo.PreFix,"YH CM ");
	strcpy(gpsInfo.SEQ,gpsInfo.SEQ);
	strcpy(gpsInfo.COMMADDR,gpsInfo.COMMADDR);
}

BOOL GpsYouHao::getResMsg( char *strBuf,GPSINFO &gpsInfo )
{
	BOOL bret = TRUE;

	switch(gpsInfo.nMsgID)
	{
	case MSG_LOGIN:
		gpsInfo.nMsgID = MSG_RES_LOGIN;		
		break;
	case MSG_MAITAIN:
		gpsInfo.nMsgID = MSG_RES_KEEPMAITAIN;
		break;
	case MSG_LOGOFF:
		gpsInfo.nMsgID = MSG_RES_LOGOFF;
		break;
	default:
		bret = FALSE;
	}

	getGpsInfo(strBuf,strlen(strBuf),gpsInfo);	
	return bret;
}

long GpsYouHao::_handleCmd_overspeed( GPSCommand*pGpsCommand,int nMaxSpeed,int nMinSpeed,int nContinue,int nValid/*=1*/ )
{
	return 0;
}

long GpsYouHao::_handleCmd_SetArea( GPSCommand*pGpsCommand,TCHAR *pAreaID,TCHAR *palertType,TCHAR *pType,TCHAR *pLeftLat,TCHAR *prightlat,TCHAR *pleftlng,TCHAR *prightlng,TCHAR *pcenterlat,TCHAR *pcenterlng,TCHAR *pRadius )
{
	return 0;
}

long GpsYouHao::_handleCmd_CacelArea( GPSCommand*pGpsCommand,TCHAR *pAreaID )
{
	return 0;
}

long GpsYouHao::_handleCmd_SetHeartTime( GPSCommand*pGpsCommand,TCHAR *pInterval )
{

	return 1;
}


long GpsYouHao::_handleCmd_SetTimingTrack( GPSCommand*pGpsCommand,TCHAR *pInterval,TCHAR*pReportCount )
{
	return 1;
}

long GpsYouHao::_handleCmd_SetTurnReport( GPSCommand*pGpsCommand,TCHAR*pAngle )
{
	return 1;
}

long GpsYouHao::_handleCmd_SetGPRSParam( GPSCommand*pGpsCommand,TCHAR*pLinkMod,TCHAR*pIP,TCHAR*pPort,TCHAR*pAPN,TCHAR*pAPN_UserName,TCHAR*pAPN_PassWord )
{

	return 1;
}

long GpsYouHao::_handleCmd_Set_BackGPRSServer( GPSCommand*pGpsCommand,TCHAR*pIP,TCHAR*pPort )
{
	return 1;

}

long GpsYouHao::_handleCmd_Get_ErrorCode( GPSCommand*pGpsCommand )
{
	return 1;

}

long GpsYouHao::_handleCmd_Set_VehicleType( GPSCommand*pGpsCommand,TCHAR*pVehicleType )
{
	return 1;
}

long GpsYouHao::_handleCmd_Get_VehicleType( GPSCommand*pGpsCommand )
{

	return 1;
}

long GpsYouHao::_handleCmd_Set_OpenOBD( GPSCommand*pGpsCommand,TCHAR*pFlag )
{
	return 1;

}

long GpsYouHao::_handleCmd_Set_OBDParam( GPSCommand*pGpsCommand,int nParamCount,void *pstArrParams )
{
return 1;
}

long GpsYouHao::_handleCmd_Set_Reset_Mileage_and_Runtime( GPSCommand*pGpsCommand )
{

	return 1;
}

long GpsYouHao::_handleCmd_Get_Device_Version_and_SN( GPSCommand*pGpsCommand,TCHAR*psetType )
{

	return 1;
}

long GpsYouHao::_handleCmd_Set_Oil_LowAlarm( GPSCommand*pGpsCommand,TCHAR*poilvalue )
{

	return 1;
}

BOOL GpsYouHao::isThisProtocol( char *buf ,GPSINFO *pGpsInfo)
{

	return FALSE;
}

char* GpsYouHao::getProtocolName( int  &nDevID )
{
	nDevID = GPS_YOUHAO;
	return _T("gps_youhao");
}

