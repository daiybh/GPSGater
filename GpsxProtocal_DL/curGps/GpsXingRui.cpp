// GpsXingRui.cpp: implementation of the GpsXingRui class.
//
//////////////////////////////////////////////////////////////////////

#include "GpsXingRui.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GpsXingRui::GpsXingRui()
{
}

GpsXingRui::~GpsXingRui()
{
}


long GpsXingRui::getGpsInfo( char *buf,GPSINFO &gpsInfo )
{
	long nret = 0;
	char strDown[512]="";
	BOOL bToGps = TRUE;
	int	nDev = 0x00;


	if(!gpsInfo.nMsgID)
	{
		getMsgID(buf,gpsInfo);
		bToGps = FALSE;
	}
	if(gpsInfo.nMsgID==MSG_NULL)	return E_MSG_DISMACH;

	switch (gpsInfo.nMsgID)
	{//Download data
	///////////////////////////////////////////////////////////////////     GPS→中心
	case MSG_TXT:
		nret = mapTxtInfo(buf,gpsInfo);
		break;
	case MSG_HEX_V:
		nret = mapHexVInfo(buf,gpsInfo);
		break;
	case MSG_HEX_X:
		nret = mapHexXInfo(buf,gpsInfo);
		break;
	///////////////////////////////////////////////////////////////////     中心→GPS
	case MSG_CALLTIMER:
		strcpy(buf,"*");
		//strcat(buf,",");
		strcat(buf,gpsInfo.PreFix);//XX,YYYYYYYYYY
		strcat(buf,",");
		strcat(buf,gpsInfo.COMMADDR);
		strcat(buf,",");
		strcat(buf,"D1");
		strcat(buf,",");
		strncat(buf,&gpsInfo.Time[6],6);
		strcat(buf,",");
		strcat(buf,gpsInfo.IntervalTime);
		strcat(buf,",");
		strcat(buf,"1");
		strcat(buf,",");
		strcat(buf,"#");
		break;
	}


	nDev = gpsInfo.nDevID&GPS_DEV_MASK;
	if(nDev==GPS_YOUHAO&&nDev>MSG_TOGPS&&nDev<MSG_UNDEF)
	{//format command to the gps device
		strcpy(buf,strDown);
	}

	return nret;	
}

void GpsXingRui::getMsgID( char *buf,GPSINFO &gpsInfo )
{
	if (!strncmp(buf,"*",1)) gpsInfo.nMsgID = MSG_TXT;
	else if (!strncmp(buf,"$",1)) gpsInfo.nMsgID = MSG_HEX_V;
	else if (!strncmp(buf,"X",1)) gpsInfo.nMsgID = MSG_HEX_X;
	else gpsInfo.nMsgID = MSG_NULL;
}




BOOL GpsXingRui::getResMsg( char *strBuf,GPSINFO &gpsInfo )
{
	return FALSE;
	BOOL bret = FALSE;	

	if(!strncmp(&strBuf[15],"V1",2))
	{
		bret = TRUE;
		gpsInfo.nMsgID = MSG_CALLTIMER;
		strcpy(gpsInfo.IntervalTime,m_strInterval);
		getGpsInfo(strBuf,gpsInfo);
		return bret;
	}


	return bret;
}



long GpsXingRui::mapHexXInfo( char *buf,GPSINFO &gpsInfo )
{
	long nret = 1;

	//TODO: NEEDLOG
	//	wlog(m_strConsole,"GpsXingRui::mapHexXInfo-undefine function");	
	return nret;
}



long processLogIn_MSG(char *buf,GPSINFO &gpsInfo)
{
return 0;
}

long GpsXingRui::mapTxtInfo2( char *buf ,GPSINFO &gpsInfo)
{
	//*HQ,5110323079,V1,091755,A,2459.3651,N,10243.2260,E,0.00,210,180412,FFFFFBFF#
	//*HQ,6120108253,V1,235955,V,0000.0000,N,00000.0000,W,0.00,000,050180,FFFFFBFF#

	//*HQ,6110916012,V4,S21,17,1,130305,050316,N,2245.318,2246.452,E,11233.232,11365.175,FFFFFBFF#
	//*HQ,6110916012,V4,S14,100,10,1,3,130305,050316,A,2212.8745,N,11346.6574,E,14.28,028,220902,FFFFFBFF#
	//信息头,deviceID,信息类型
	//根据信息类型来处理剩余字节
	const char * pdest=NULL;
	int nLen1  = 0;
	int nRetLen = strlen(buf);
	pdest = find_Comma(buf,nLen1);//找第一个 ,
	if(pdest==NULL)
	{
		return -2;
	}
	char stemp[512];
	pdest = getStr_betweenComma(pdest,gpsInfo.COMMADDR);//获取deviceID 6110916012
	pdest = getStr_betweenComma(pdest,stemp);//获取信息类型    V4	
	char *plwrFlag = strlwr(stemp);
	if(strcmp(plwrFlag,"v4")==0)
	{
		//v4命令回馈 ，最后会加 FFFFFBFF，用这个来验证
	//S21,17,1,130305,050316,N,2245.318,2246.452,E,11233.232,11365.175,FFFFFBFF#
	//S14,100,10,1,3,130305,050316,A,2212.8745,N,11346.6574,E,14.28,028,220902,FFFFFBFF#
		//gps对 服务器命令的响应
		gpsInfo.nMsgID = MSG_TERMINALFEEDBACK;
		//收到命令 解析出 命令的
		pdest = getStr_betweenComma(pdest,gpsInfo.CMDID);

		strncpy(gpsInfo.CMDARGUS,pdest,strlen(pdest)-10/*strlen(",FFFFFBFF#")*/);
		//这里组合命令
		strcpy(gpsInfo.Time,"20120720133920");
		char *pCmdID = strlwr(gpsInfo.CMDID);
		if(strcmp(pCmdID,"s21")==0)
		{
			char *pAreaID = stemp;
			pdest = getStr_betweenComma(pdest,pAreaID);
			char *pAdd = stemp+200;
			pdest = getStr_betweenComma(pdest,pAdd);
			sprintf(gpsInfo.CMDID,"s21_%s_%s",pAdd,pAreaID);
		}


		
		//pdest = getStr_betweenComma(pdest,gpsInfo.CMDARGUS);
		//需要根据命令格式 来解析出命令参数
		
		//直接解析出当前命令，当前命令的状态，

	}
	else 
	{
		//登录数据
		gpsInfo.nMsgID = MSG_LOGIN;
		char	str_HHMMSS[10];

		pdest = getStr_betweenComma(pdest,str_HHMMSS);
		pdest = getStr_betweenComma(pdest,stemp);//SFlag

		if(!strncmp(stemp,"A",1))//Valid GPS data
		{
			gpsInfo.bValid = true;
			pdest = getStr_betweenComma(pdest,gpsInfo.Latitude);

			char	DFlag[2]="";
			char	GFlag[2]="";

			pdest = getStr_betweenComma(pdest,DFlag);//D

			convertTitude(gpsInfo.Latitude,DFlag[0]=='N'?true:false,false,false);
			pdest = getStr_betweenComma(pdest,gpsInfo.Longitude);//longitude

			pdest = getStr_betweenComma(pdest,GFlag);//G

			convertTitude(gpsInfo.Longitude,GFlag[0]=='E'?true:false,true,false);

			pdest = getStr_betweenComma(pdest,gpsInfo.Speed);//Speed

			pdest = getStr_betweenComma(pdest,gpsInfo.Heading);//Direction

		}
		else
		{
			gpsInfo.bValid = false;
			// 		FillX(",",strTmp);// fetch data
			// 		else return 2;
			// 		FillX(",",strTmp);// fetch data
			// 		else return 2;
			// 		FillX(",",strTmp);// fetch data
			// 		else return 2;
			// 		FillX(",",strTmp);// fetch data
			// 		else return 2;
			//return 2;
			pdest = getStr_betweenComma(pdest,stemp);
			pdest = getStr_betweenComma(pdest,stemp);
			pdest = getStr_betweenComma(pdest,stemp);
			pdest = getStr_betweenComma(pdest,stemp);
			pdest = getStr_betweenComma(pdest,stemp);
			pdest = getStr_betweenComma(pdest,stemp);			
		}
		pdest = getStr_betweenComma(pdest,stemp);//DDMMYY

		sprintf(gpsInfo.Time,"20%c%c%c%c%c%c%s",
			stemp[4],stemp[5],stemp[2],stemp[3],stemp[0],stemp[1],str_HHMMSS);
		/*
		strncpy(strTmp,&strTmp1[4],2);//YY
		strTmp1[4]=strTmp1[0];
		strTmp1[5]=strTmp1[1];
		strTmp1[0]=strTmp[0];
		strTmp1[1]=strTmp[1];
		strcat(strTmp1,gpsInfo.Time);
		strcpy(gpsInfo.Time,strTmp1);//YYMMDDHHMMSS
		/**/
		//pdest = getStr_betweenComma(pdest,stemp);//FFFFFBFF#//vehicle_status
		strcpy(stemp,pdest);
		stemp[8]=0;
		DWORD dT;
		sscanf(stemp,"%x",&dT);
		dT &=0xFF;

		if((dT&0x04)==0x00) gpsInfo.nWarnFlag |= WAR_OVERSPEED;
		if((dT&0x10)==0x00) gpsInfo.nWarnFlag |= WAR_INZONE;
		if((dT&0x80)==0x00) gpsInfo.nWarnFlag |= WAR_OUTZONE;

	}
	return nRetLen;
}

/************************************
// Method:    mapTxtInfo
// FullName:  GpsXingRui::mapTxtInfo
// Access:    protected 
// Returns:   long
// Qualifier:
// Parameter: char * buf
// Parameter: GPSINFO & gpsInfo
*XX,YYYYYYYYYY,V1,HHMMSS,S,latitude,D,longitude,G,speed,direction,DDMMYY,vehicle_status#
************************************/
long GpsXingRui::mapTxtInfo( char *buf,GPSINFO &gpsInfo )
{
	return mapTxtInfo2(buf,gpsInfo);
	//*HQ,5110323079,V1,091755,A,2459.3651,N,10243.2260,E,0.00,210,180412,FFFFFBFF#
	//*HQ,6120108253,V1,235955,V,0000.0000,N,00000.0000,W,0.00,000,050180,FFFFFBFF#
	long	nlen = 0;
	long	i = 1;
	char	strTmp[512]="";
	char	strTmp1[512]="";
	char	DFlag[2]="";
	char	GFlag[2]="";

	FillX(",",strTmp);//manufacturer 1
	else return -2;
	strcat(gpsInfo.PreFix,strTmp);

	FillX(",",gpsInfo.COMMADDR);//manufacturer 2
	else return -2;
// 	strcat(gpsInfo.PreFix,strTmp);

	FillX(",",strTmp);// gps info type v1
	else return -2;
	if(strncmp(strTmp,"V1",2)) gpsInfo.nMsgID = MSG_LOGIN;
	if(strncmp(strTmp,"V4",2))
	{
		//gps对 服务器命令的响应
		gpsInfo.nMsgID = MSG_TERMINALFEEDBACK;
	}

	char	str_HHMMSS[10];
	ZeroMemory(str_HHMMSS,10);
	FillX(",",str_HHMMSS);//HHMMSS
	else return -2;

	FillX(",",strTmp);// SFlag
	else return -2;
	if(!strncmp(strTmp,"A",1))//Valid GPS data
	{
		
		FillX(",",gpsInfo.Latitude);//Latitude
		else return -2;
		FillX(",",DFlag);//D
		else return -2;
		convertTitude(gpsInfo.Latitude,DFlag[0]=='N'?true:false,false,false);
		
		FillX(",",gpsInfo.Longitude);//longitude
		else return -2;
		FillX(",",GFlag);//G
		else return -2;
		convertTitude(gpsInfo.Longitude,GFlag[0]=='E'?true:false,true,false);
	}
	else
	{
// 		FillX(",",strTmp);// fetch data
// 		else return 2;
// 		FillX(",",strTmp);// fetch data
// 		else return 2;
// 		FillX(",",strTmp);// fetch data
// 		else return 2;
// 		FillX(",",strTmp);// fetch data
// 		else return 2;
		return 2;
	}

	FillX(",",gpsInfo.Speed);//Speed
	else return -2;

	FillX(",",gpsInfo.Heading);//Direction
	else return -2;

	FillX(",",strTmp1);//DDMMYY
	else return -2;

	sprintf(gpsInfo.Time,"20%c%c%c%c%c%c%s",
		strTmp1[4],strTmp1[5],strTmp1[2],strTmp1[3],strTmp1[0],strTmp1[1],str_HHMMSS);
/*
	strncpy(strTmp,&strTmp1[4],2);//YY
	strTmp1[4]=strTmp1[0];
	strTmp1[5]=strTmp1[1];
	strTmp1[0]=strTmp[0];
	strTmp1[1]=strTmp[1];
	strcat(strTmp1,gpsInfo.Time);
	strcpy(gpsInfo.Time,strTmp1);//YYMMDDHHMMSS
/**/
	FillX("#",strTmp);//vehicle_status
	if((strTmp[3]&0x04)==0x04) gpsInfo.nWarnFlag |= WAR_OVERSPEED;
	if((strTmp[3]&0x10)==0x10) gpsInfo.nWarnFlag |= WAR_INZONE;
	if((strTmp[3]&0x80)==0x80) gpsInfo.nWarnFlag |= WAR_OUTZONE;
	
	return 1;
}



long GpsXingRui::mapHexVInfo( char *pbuf,GPSINFO &gpsInfo )
{
	long	nlen = 0;
	long	i = 1;
	char	strTmp[512]="";
	//unsigned char	buf[65]="";

	// 00000000000000011111111111111112222222222222223333333333333333444444444
	// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
	//2451103230790839471704122500295400102438754e000344fffffbffff0033
	//2461203243620235000404132500116100102378131c000000fffffbffff006e	

	//验证数据有效性
	//buf[21] bit1，1：A，0：V

	//memcpy(buf,pbuf,32);
	//int av = buf[21]&0x2;
	gpsInfo.bValid = ((pbuf[0x15]&0x2)==0x2);
	/*if(av==0)
		return -2;
/**/
	sprintf(strTmp,"%2.2x%2.2x%2.2x%2.2x%2.2x",pbuf[1]&0xFF,pbuf[2]&0xFF,pbuf[3]&0xFF,pbuf[4]&0xFF,pbuf[5]&0xFF);//SIM NO
	strcpy(gpsInfo.COMMADDR,strTmp);

	


	if(gpsInfo.bValid)
	{
		sprintf(strTmp,"20%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",pbuf[11]&0xFF,pbuf[10]&0xFF,pbuf[9]&0xFF,pbuf[6]&0xFF,pbuf[7]&0xFF,pbuf[8]&0xFF);//YYYYMMDDHHMMSS
		strcpy(gpsInfo.Time,strTmp);
		sprintf(strTmp,"%2.2x%2.2x%2.2x%2.2x0000",pbuf[12]&0xFF,pbuf[13]&0xFF,pbuf[14]&0xFF,pbuf[15]&0xFF);//Latitude
		strcpy(gpsInfo.Latitude,strTmp);
		convertTitude(gpsInfo.Latitude,(pbuf[21]&0x04)==0x04,false,true);

		sprintf(strTmp,"%2.2x%2.2x%2.2x%2.2x%2.2x0000",pbuf[17]&0xFF,pbuf[18]&0xFF,pbuf[19]&0xFF,pbuf[20]&0xFF,pbuf[21]&0xF0);//Longtitude
		strcpy(gpsInfo.Longitude,strTmp);
		convertTitude(gpsInfo.Longitude,(pbuf[21]&0x08)==0x08,true,true);

		sprintf(strTmp,"%2.2x%2.2x%2.2x",pbuf[22]&0xFF,pbuf[23]&0xFF,pbuf[24]&0xFF);//Speed+Head
		strncpy(gpsInfo.Speed,strTmp,3);//speed
		strncpy(gpsInfo.Heading,&strTmp[3],3);//head

		sprintf(strTmp,"%2.2x%2.2x%2.2x%2.2x",pbuf[0x19]&0xFF,pbuf[0x1a]&0xFF,pbuf[0x1b]&0xFF,pbuf[0x1c]&0xFF);//Vehicle status
		if((pbuf[0x1c]&0x04)!=0x04) gpsInfo.nWarnFlag |= WAR_OVERSPEED;
		if((pbuf[0x1c]&0x10)!=0x10) gpsInfo.nWarnFlag |= WAR_INZONE;
		if((pbuf[0x1c]&0x80)!=0x80) gpsInfo.nWarnFlag |= WAR_OUTZONE;
	}
	sprintf(gpsInfo.VERFYCODE,"%2.2x",pbuf[0x1f]&0xFF);

	return 32;
}



//************************************
// Method:    convertTitude
// FullName:  GpsXingRui::convertTitude
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: char * strTitude
// Parameter: bool bPositive	是否为负数 东西经  南北纬
// Parameter: bool bLongtitude	是否是经度
// Parameter: bool bVmode		TRUE:没有带小数点 FALSE: 带有小数点
//************************************
void GpsXingRui::convertTitude( char *strTitude,bool bPositive,bool bLongtitude ,bool bVmode)
{
	char strTmp1[10]="";
	char strTmp2[10]="";
	char strTmp3[20]="";
	int nvala = 0;
	int nvalb = 0;
	double nfval = 0;

	if(bLongtitude)//经度DDDFF.FFFF
	{
		//0123456789
		//10243.8754
		//102438754
		//strcpy(strTmp1,bPositive==true?"":"-");
		strncpy(strTmp1,strTitude,3);//DDD
		//strncpy(strTmp2,&strTitude[bVmode?3:4],2);//FF
		strncpy(strTmp2,&strTitude[3],2);//FF
		strncat(strTmp3,&strTitude[bVmode?5:6],4);//.FFFF
	}
	else//latitude DDFF.FFFF
	{
		//01234567
		//2500.2954
		//25002954
		strncpy(strTmp1,strTitude,2);
//		strncpy(strTmp2,&strTitude[bVmode?2:3],2);
		strncpy(strTmp2,&strTitude[2],2);
		strncat(strTmp3,&strTitude[bVmode?4:5],4);
	}
	nvala = atoi(strTmp1);
	nvalb = atoi(strTmp2);
	nfval = (double)atoi(strTmp3)*0.0001+nvalb;
	nfval = nfval/60.0;
	nfval = nvala + nfval;
	sprintf(strTmp3,"%s%0.8f",bPositive==true?"":"-",nfval);

	strcpy(strTitude,strTmp3);
}
#include <time.h>
// long GpsXingRui::getConsole2GPSData( const char *fromConsole_srcBuf,GPSCommand *pGpsCommand)
// {
// 	TCHAR chXMlValue[100];
// 	if(!m_pXmlParser->Parse(fromConsole_srcBuf))
// 		return 0;
// 	int nLen = m_pXmlParser->GetRootName(chXMlValue);
// 	int nRet = -1;
// 	do{
// 		time_t utc_time;
// 		utc_time = time(NULL);
// 		struct tm* local_time =NULL;
// 		local_time = localtime(&utc_time);
// 
// 		TCHAR*pTemp = chXMlValue;
// 		// 解析各种命令
// 		if(strcmp(chXMlValue,"setarea")==0)
// 		{
// 			//*XX,YYYYYYYYYY,S21,HHMMSS,Lx,M,D,Minlatitude,Maxlatitude,G,Minlongitude,Maxlongitude#
// 			//*HQ,6110916012,S21,130305,17,1,N,2245.318,2246.452,E,11233.232,11365.175#			
// 			/*
// 			 *	
// 				-------------  right(max,max)
// 				|			|
// 				|			|
// 				|			|
// 				|			|
// 				-------------
// 				left(min,min)
// 			 */
// 			//CString strDeviceID =m_pXmlParser->GetNodeText(_T("sim"),pTemp);
// 			
// 			strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
// 			strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
// 	/*
// 			_tcscpy(pGpsCommand->strCMDID,(m_pXmlParser->GetNodeText(_T("id"),pTemp)));
// 
// 			strcpy(pGpsCommand->strCMDID,"S21");
// 
// 			_tcscpy(pGpsCommand->strCMDID,(m_pXmlParser->GetNodeText(_T("type"),pTemp)));
// 		/**/
// 			char alertType[5] ;
// 			strcpy(alertType,m_pXmlParser->GetNodeText(_T("alerttype"),pTemp));
// 			//默认为 出区域报警
// 			
// 			if(strcmp(alertType ,"1")==0)//进区域报警
// 			{
// 				//alertType= _T("3");
// 				//alertType= _T("4");
// 				strcpy(alertType,"4");
// 			}
// 			else
// 			{
// 				//alertType= _T("1");
// 				//alertType= _T("2");
// 				strcpy(alertType,"2");
// 			}
// 			
// 			//copyData(pGpsCommand->strCMDID,());
// 			//CString strCmdID ;
// 			//strCmdID.Format(_T("s21_1_%s"),m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
// 			//copyData(pGpsCommand->strCMDID,strCmdID);
// 
// 			sprintf(pGpsCommand->strCMDID,"s21_1_%s",m_pXmlParser->GetNodeText("areaid",pTemp));
// 			//formatParam
// 			/*
// 			CString sParam;
// 			//				 围栏号	条件 纬度标志
// 			//               Lx,	M,	D,		Minlatitude,Maxlatitude,G,Minlongitude,Maxlongitude#
// 			sParam.Format(_T("%s,%s,%s,%s,%s,%s,%s,%s"),
// 				m_pXmlParser->GetNodeText(_T("areaid"),pTemp),
// 				alertType,
// 				_T("N"),
// 				m_pXmlParser->GetNodeText(_T("leftlat"),pTemp),
// 				m_pXmlParser->GetNodeText(_T("rightlat"),pTemp),
// 				_T("E"),
// 				m_pXmlParser->GetNodeText(_T("leftlng"),pTemp),
// 				m_pXmlParser->GetNodeText(_T("rightlng"),pTemp));
// 			//pGpsCommand->nLenCmdParameters =  copyData(pGpsCommand->commandParameters,sParam);
// 			/**/
// 			sprintf(pGpsCommand->commandParameters,_T("%s,%s,%s,%s,%s,%s,%s,%s"),
// 				m_pXmlParser->GetNodeText(_T("areaid"),pTemp),
// 				alertType,
// 				_T("N"),
// 				m_pXmlParser->GetNodeText(_T("leftlat"),pTemp),
// 				m_pXmlParser->GetNodeText(_T("rightlat"),pTemp),
// 				_T("E"),
// 				m_pXmlParser->GetNodeText(_T("leftlng"),pTemp),
// 				m_pXmlParser->GetNodeText(_T("rightlng"),pTemp));
// 
// 			//CTime curTime = CTime::GetCurrentTime();
// 			//strCmdLine.Format(_T("*HQ,%s,S21,%s,%s#"),strDeviceID,curTime.Format(_T("%H%M%S")),sParam);
// 			//pGpsCommand->nLenCommandLine = copyData(pGpsCommand->strCommandLine,strCmdLine);
// 			
// 
// 			sprintf(pGpsCommand->strCommandLine,_T("*HQ,%s,S21,%d%d%d,%s#"),
// 				pGpsCommand->strDevID,
// 				//curTime.Format(_T("%H%M%S")),
// 				local_time->tm_hour,
// 				local_time->tm_min,
// 				local_time->tm_sec,
// 				pGpsCommand->commandParameters);
// 			
// 			pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
// 			pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1]='\0';
// 
// 			memset(pTemp,0,10);
// 			//int nRecordID = CoverStringToInt(m_pXmlParser->GetNodeText(_T("id"),pTemp));
// 			int nRecordID = atoi(pTemp);
// 			pGpsCommand->pVoid = (int*)nRecordID;			
// 			nRet = 1;
// 		}
// 		else if(strcmp(chXMlValue,"cancelarea")==0)
// 		{
// 			pGpsCommand->commandType = CmdType(cmdType_ToGPS+1);
// 
// 			strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText("sim",pTemp));
// 
// 			//strDeviceID =m_pXmlParser->GetNodeText(_T("deviceid"),pTemp);
// 			//copyData(pGpsCommand->strDevID,strDeviceID);
// 			strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
// 			
// 			//CString strCmdID ;
// 			//strCmdID.Format(_T("s21_0_%s"),m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
// 			//copyData(pGpsCommand->strCMDID,strCmdID);
// 			sprintf(pGpsCommand->strCMDID,"s21_0_%s",m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
// 			//*XX,YYYYYYYYYY,S21,HHMMSS,Lx,M,D,Minlatitude,Maxlatitude,G,Minlongitude,Maxlongitude#
// 			//*HQ,6110916012,S21,130305,17,1,N,2245.318,2246.452,E,11233.232,11365.175#
// 			//CTime curTime = CTime::GetCurrentTime();
// //			strCmdLine.Format(_T("*HQ,%s,S21,%s,%s,0,N,2245.318,2246.452,E,11233.232,11365.175#"),strDeviceID,curTime.Format(_T("%H%M%S")),
// //				m_pXmlParser->GetNodeText(_T("areaid"),pTemp)	);
// //			pGpsCommand->nLenCommandLine = copyData(pGpsCommand->strCommandLine,strCmdLine);
// 			sprintf(pGpsCommand->strCommandLine,"*HQ,%s,S21,%d%d%d,%s,0,N,2245.318,2246.452,E,11233.232,11365.175#"
// 				                                ,pGpsCommand->strDevID,
// 												//curTime.Format(_T("%H%M%S")),
// 												local_time->tm_hour,
// 												local_time->tm_min,
// 												local_time->tm_sec,
// 												m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
// 
// 			int nRecordID = atoi(m_pXmlParser->GetNodeText(_T("id"),pTemp));
// 			pGpsCommand->pVoid = (int*)nRecordID;
// 			nRet = 1;
// /*
// 			<?xml version="1.0"?>
// 				<CancelArea>
// 				<ID>
// 				<!--区域ID-->
// 				</ID>
// 				<SIM>
// 				<!--sim卡号-->
// 				</SIM>
// 				<DeviceID>
// 				</DeviceID>
// 				<AreaID>
// 				</AreaID>
// 				</CancelArea>
// */
// 		}
// 		else if(strcmp(chXMlValue,"overspeed")==0)
// 		{
// 
// 			pGpsCommand->commandType = CmdType(cmdType_ToService+1);
// 			//*XX,YYYYYYYYYY,S14,HHMMSS,Max_speed,Min_speed,M,countinue #
// 			//*HQ,0000000000,S14,130305,100,10,1,3#			
// 
// 			strcpy(pGpsCommand->strCMDID,"S14");
// 
// 			strcpy(pGpsCommand->strSim,m_pXmlParser->GetNodeText(_T("sim"),pTemp));			
// 			
// 			strcpy(pGpsCommand->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
// 			
// 
// 			strcpy(pGpsCommand->strCMDID,(m_pXmlParser->GetNodeText(_T("id"),pTemp)));
// 			
// 			struct tag_OverSpeed 
// 			{
// 				int nMaxSpeed;
// 				int nMinSpeed;
// 				int nCountinue;
// 			};
// 			
// 			tag_OverSpeed *pstOverSpeed = new tag_OverSpeed;
// 
// 			pstOverSpeed->nMaxSpeed = atoi(m_pXmlParser->GetNodeText(_T("maxspeed"),pTemp));
// 			pstOverSpeed->nMinSpeed = atoi(m_pXmlParser->GetNodeText(_T("minspeed"),pTemp));
// 			pstOverSpeed->nCountinue = atoi(m_pXmlParser->GetNodeText(_T("countinue"),pTemp));
// 			
// 			pGpsCommand->pVoid = pstOverSpeed;
// 			//formatParam
// 
// 			sprintf(pGpsCommand->commandParameters,_T("%s,%s,%s,%s"),
// 				m_pXmlParser->GetNodeText(_T("maxspeed"),pTemp),
// 			m_pXmlParser->GetNodeText(_T("minspeed"),pTemp),
// 			m_pXmlParser->GetNodeText(_T("m"),pTemp),
// 			m_pXmlParser->GetNodeText(_T("countinue"),pTemp));
// 
// 			
// 			pGpsCommand->nLenCmdParameters =strlen(pGpsCommand->commandParameters);
// 
// 			
// 			sprintf(pGpsCommand->strCommandLine,_T("*HQ,%s,S14,%d%d%d,%s#"),
// 				pGpsCommand->strDevID,
// 				local_time->tm_hour,
// 				local_time->tm_min,
// 				local_time->tm_sec,
// 				pGpsCommand->commandParameters);
// 
// 			pGpsCommand->nLenCommandLine = strlen(pGpsCommand->strCommandLine);
// 			pGpsCommand->strCommandLine[pGpsCommand->nLenCommandLine+1] = '\0';
// 			
// 			nRet = 1;
// 		}
// 		else
// 		{
// 			break;
// 		}
// 	}while(0);
// 
// 	return nRet;
// }
long GpsXingRui::_handleCmd_overspeed(GPSCommand*pGpsCommand,int nMaxSpeed,int nMinSpeed,int nContinue,int nValid)
{
	//formatParam
	sprintf(pGpsCommand->commandParameters,_T("%d,%d,%d,%d"),nMaxSpeed,nMinSpeed,nContinue,nValid);


	pGpsCommand->nLenCmdParameters =strlen(pGpsCommand->commandParameters);

	struct tm* local_time = GetLocalTime();
	sprintf(pGpsCommand->strCommandLine,_T("*HQ,%s,S14,%d%d%d,%s#"),
		pGpsCommand->strDevID,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec,
		pGpsCommand->commandParameters);
	return 1;
}

long GpsXingRui::_handleCmd_SetArea( GPSCommand*pGpsCommand,TCHAR *pAreaID,TCHAR *palertType,TCHAR *pType,TCHAR *pLeftLat,TCHAR *prightlat,TCHAR *pleftlng,TCHAR *prightlng,TCHAR *pcenterlat,TCHAR *pcenterlng,TCHAR *pRadius )
{/*
	sprintf(pGpsCommand->commandParameters,_T("%s,%s,%s,%s,%s,%s,%s,%s"),
		m_pXmlParser->GetNodeText(_T("areaid"),pTemp),
		alertType,
		_T("N"),
		m_pXmlParser->GetNodeText(_T("leftlat"),pTemp),
		m_pXmlParser->GetNodeText(_T("rightlat"),pTemp),
		_T("E"),
		m_pXmlParser->GetNodeText(_T("leftlng"),pTemp),
		m_pXmlParser->GetNodeText(_T("rightlng"),pTemp));
	/**/
	sprintf(pGpsCommand->commandParameters,_T("%s,%s,%s,%s,%s,%s,%s,%s"),
		pAreaID,
		palertType,
		_T("N"),
		pLeftLat,
		prightlat,
		_T("E"),
		pleftlng,
		prightlng);

	int nLen = strlen(pGpsCommand->commandParameters);
	pGpsCommand->commandParameters[nLen+1]='\0';

	struct tm* local_time = GetLocalTime();
	sprintf(pGpsCommand->strCommandLine,_T("*HQ,%s,S21,%d%d%d,%s#"),
		pGpsCommand->strDevID,
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec,
		pGpsCommand->commandParameters);
	return 1;
}

long GpsXingRui::_handleCmd_CacelArea( GPSCommand*pGpsCommand,TCHAR *pAreaID )
{
	struct tm* local_time = GetLocalTime();
	sprintf(pGpsCommand->strCommandLine,"*HQ,%s,S21,%d%d%d,%s,0,N,2245.318,2246.452,E,11233.232,11365.175#"
		,pGpsCommand->strDevID,
		//curTime.Format(_T("%H%M%S")),
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec,
		pAreaID);
	return 1;
}

long GpsXingRui::_handleCmd_SetHeartTime( GPSCommand*pGpsCommand,TCHAR *pInterval )
{
	return 1;
}

long GpsXingRui::_handleCmd_SetTimingTrack( GPSCommand*pGpsCommand,TCHAR *pInterval,TCHAR*pReportCount )
{
return 1;
}

long GpsXingRui::_handleCmd_SetTurnReport( GPSCommand*pGpsCommand,TCHAR*pAngle )
{
	return 1;
}

long GpsXingRui::_handleCmd_SetGPRSParam( GPSCommand*pGpsCommand,TCHAR*pLinkMod,TCHAR*pIP,TCHAR*pPort,TCHAR*pAPN,TCHAR*pAPN_UserName,TCHAR*pAPN_PassWord )
{
	return 1;
}

long GpsXingRui::_handleCmd_Set_BackGPRSServer( GPSCommand*pGpsCommand,TCHAR*pIP,TCHAR*pPort )
{
	return 1;

}

long GpsXingRui::_handleCmd_Get_ErrorCode( GPSCommand*pGpsCommand )
{
	return 1;

}

long GpsXingRui::_handleCmd_Set_VehicleType( GPSCommand*pGpsCommand,TCHAR*pVehicleType )
{
	return 1;
}

long GpsXingRui::_handleCmd_Get_VehicleType( GPSCommand*pGpsCommand )
{
	return 1;
}

long GpsXingRui::_handleCmd_Set_OpenOBD( GPSCommand*pGpsCommand,TCHAR*pFlag )
{
	return 1;
}

long GpsXingRui::_handleCmd_Set_OBDParam( GPSCommand*pGpsCommand,int nParamCount,void *pstArrParams )
{
return 1;
}

long GpsXingRui::_handleCmd_Get_Device_Version_and_SN( GPSCommand*pGpsCommand,TCHAR*psetType )
{

	return 1;
}

long GpsXingRui::_handleCmd_Set_Oil_LowAlarm( GPSCommand*pGpsCommand,TCHAR*poilvalue )
{

	return 1;
}

long GpsXingRui::_handleCmd_Set_Reset_Mileage_and_Runtime( GPSCommand*pGpsCommand )
{

	return 1;
}
