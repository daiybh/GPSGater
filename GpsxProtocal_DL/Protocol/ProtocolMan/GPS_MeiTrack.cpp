#include "stdafx.h"
#include "GPS_MeiTrack.h"

GPS_MeiTrack::GPS_MeiTrack(void)
{
}

GPS_MeiTrack::~GPS_MeiTrack(void)
{
}
/*$$<数据包标识符><数据长度>,<IMEI>,<指令类型>,<事件代码>,<(-)纬度>,<(-)经度>,<日期和时间>,<定位状
态>,<卫星个数>,<GSM 信号强度>,<速度>,<方向>,<水平定位精度>,<海拔高度>,<里程数>,<运行时间>,<基站信
息>,<输入输出口状态>,<模拟量输入口值>,<RFID 号>/<图片名>/<围栏序号>,<定制数据>,<协议版本 V>,<发动
机转速>,<发动机计算负荷>,<发动机冷却液温度>,<百公里油耗>,<进气温度>,<油压>,<大气压力>,<进气管绝
对压力>,<空气流量>,<节气门位置>,<单次行驶里程>,<百分比剩余油量>,<故障码及冻结帧/就绪状态><*检验
码>\r\n 
/**/
long GPS_MeiTrack::getGpsInfo( char *buf,int nbufLen,GPSINFO &gpsInfo )
{
	int nRet = 1;
	const char *pBuf = buf;
	int nBufferDataLen = strlen(buf);
	do{
		//$$A126,863070010099660,AAA,029,22.513211,114.058928,121022092220,V,0,00,0,0,0,0,11872388,363251,000|0|0000|0000,0000,|||09C3|0000,,*FE
		//$$D175,863070010099660,AAA,144,22.513211,114.058928,121022092543,V,0,25,0,0,0,0,11872388,363251,460|0|2792|0E89,0000,|||0A1F|037E,,,50,10454,39,79,,73,1878.9,113,119,3,46,0,46,*F5
		//$$S175,863070010099660,AAA,144,22.513211,114.058928,121022155026,V,0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		//$$
		pBuf+=2;
		const char *pstrMark = pBuf;
		//S
		//<数据包标识符>//1  个字节。类型为 ASCII 码，值从 0x41 至 0x7A
		if(*pstrMark<0x41 || *pstrMark > 0x7A)
		{
			//这个数据有问题	
			nRet = -1;
			break;
		}
		strncpy(gpsInfo.CMDID,pstrMark,1);
		pBuf++;		
		//<数据长度>,//数据长度是从分隔符“,”开始，到“\r\n”结束符的长度。	类型为十进制。
		int nDataLen  = 0;
		int nLen = 0;
		char strTemp[512];

		//175,863070010099660,AAA,144,22.513211,114.058928,121022155026,V,0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		pBuf = getStr_betweenComma(pBuf,strTemp);
		//pBuf->863070010099660,AAA,144,22.513211,114.058928,121022155026,V,0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		nDataLen = atoi(strTemp)-1;
		nBufferDataLen = nDataLen+6;
		const char * pCheckCode =NULL;
		{			
			//判断数据长度 是否正确
			//判断最后是否是 \r\n
			const char *pLast = pBuf+nDataLen;			
			if( (*(pLast-2) !=0x0d) ||  (*(pLast-1) !=0x0a))
			{
				//数据无效
				nRet = -3;
				break;
			}
			//校验数据有效性
			memcpy(strTemp,(pLast-4),2);
			strTemp[2]='\0';
			int nCheckCode = getCheckCode(buf,nDataLen+3,strTemp+3);//3=7-4
			if(strcmp(strTemp,strTemp+3)!=0)
			{
				nRet = -4;
				break;
			}
			pCheckCode = pLast-5;
		}
		//<IMEI>863070010099660
		pBuf = getStr_betweenComma(pBuf,gpsInfo.COMMADDR);
		//pBuf->AAA,144,22.513211,114.058928,121022155026,V,0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		//<指令类型>AAA,
		//char commandType[5];

		strncpy(gpsInfo.CMDID,pstrMark,1);
		char *pCommandType = gpsInfo.CMDID+1;
		pBuf = getStr_betweenComma(pBuf,pCommandType);
		//pBuf->144,22.513211,114.058928,121022155026,V,0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		gpsInfo.nMsgID = MSG_LOGIN;//AAA时 是正常报文数据
		if(strncmp(pCommandType,"AAA",3)!=0)
		{
			//自动事件报告
			//gpsInfo.nMsgID=;
			gpsInfo.nMsgID = MSG_TERMINALFEEDBACK;
		}
		//<事件代码>,144,
		char eventCode[5];
		memset(eventCode,0,5);
		pBuf = getStr_betweenComma(pBuf,eventCode);
		{
			//如果是MSG_TERMINALFEEDBACK 那么这里只需要判断是否是 OK即可。
			if(gpsInfo.nMsgID == MSG_TERMINALFEEDBACK)
			{
				gpsInfo.nWarnFlag=0;//失败
				if(strncmp(eventCode,"OK",2)==0)
				{
					gpsInfo.nWarnFlag = 1;//成功
				}
				break;//不再继续下面的代码
			}
		}
		//pBuf->22.513211,114.058928,121022155026,V,0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		//报警代码
		int nEventCode = atoi(eventCode);
		//19 WAR_OVERSPEED
		//20	WAR_INZONE
		//21	WAR_OUTZONE
		gpsInfo.nWarnFlag=0;
		switch(nEventCode)
		{
		case 19:
			gpsInfo.nWarnFlag |=WAR_OVERSPEED;
			break;
		case 20:
			gpsInfo.nWarnFlag |=WAR_INZONE;
			break;
		case 21:
			gpsInfo.nWarnFlag |=WAR_OUTZONE;
			break;
		}
		//<(-)纬度>,22.513211,
		pBuf = getStr_betweenComma(pBuf,gpsInfo.Latitude);
		//pBuf->114.058928,121022155026,V,0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		//<(-)经度>114.058928,
		pBuf = getStr_betweenComma(pBuf,gpsInfo.Longitude);
		//pBuf->121022155026,V,0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n

		//,<日期和时间>,121022155026,
		strcpy(gpsInfo.Time,"20");
		pBuf = getStr_betweenComma(pBuf,gpsInfo.Time+2);
		//pBuf->V,0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		//<定位状态>,V,
		pBuf = getStr_betweenComma(pBuf,strTemp);
		//pBuf->0,26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		gpsInfo.bValid = !strncmp(strTemp,"A",1);
		//<卫星个数>,0,
		pBuf = getStr_betweenComma(pBuf,strTemp);
		//pBuf->26,0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n

		if(pBuf==pCheckCode)
		{
			break;
		}
		//<GSM 信号强度>,26,
		pBuf = getStr_betweenComma(pBuf,strTemp);
		//pBuf->0,0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n

		if(pBuf==pCheckCode)
		{
			break;
		}
		//<速度>,0,
		pBuf = getStr_betweenComma(pBuf,gpsInfo.Speed);
		//pBuf->0,0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n

		if(pBuf==pCheckCode)
		{
			break;
		}
		//<方向>,0,
		pBuf = getStr_betweenComma(pBuf,gpsInfo.Heading);
		//pBuf->0,0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n

		if(pBuf==pCheckCode)
		{
			break;
		}


		//obd信息
		gpsInfo.nHaveOBDInfo = 0;
		stOBDInfo *pOBDInfo = &(gpsInfo.st_OBD_Info);
		//<水平定位精度>,0,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Horizontal_positioning_accuracy);
		//pBuf->0,12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n

		if(pBuf==pCheckCode)
		{
			break;
		}
		//<海拔高度>,0,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Altitude);
		//pBuf->12445866,386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n

		if(pBuf==pCheckCode)
		{
			break;
		}
		//<里程数>,12445866,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Mileage);
		//pBuf->386301,460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n

		if(pBuf==pCheckCode)
		{
			break;
		}
		//<运行时间>,386301,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Runing_Time);
		//pBuf->460|0|2792|0E89,0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}

		//<基站信息>,460|0|2792|0E89,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->AGPS_Info);
		//pBuf->0000,|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}

		//<输入输出口状态>,0000,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->status_InOutTake);
		//pBuf->|||0A2A|0381,,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}

		//<模拟量输入口值>,|||0A2A|0381,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->outputValue);
		//pBuf->,,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}

		//<RFID 号>/<图片名>/<围栏序号>, ,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->RFID);
		//pBuf->,50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}

		//<定制数据>,  ,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Customize_Data);
		//pBuf->50,10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}

		//<协议版本 V>,50,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Protocol_Version);
		//pBuf->10218,39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}

		//<发动	机转速>,10218,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Engine_speed);
		//pBuf->39,79,,73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;

		//<发动机计算负荷>,39,79,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Engine_load);
		//pBuf->,73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;

		//<发动机冷却液温度>, ,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Engine_coolant_temperature);
		//pBuf->73,1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;

		//<百公里油耗>,73,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Hundred_kilometers);
		//pBuf->1878.9,113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;

		//<进气温度>,1878.9,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Intake_air_temperature);
		//pBuf->113,119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;

		//<油压>,113,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Hydraulic);
		//pBuf->119,1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;

		//<大气压力>,119,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Atmospheric_pressure);
		//pBuf->1,46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;

		//<进气管绝对压力>,1,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Intake_pressure);
		//pBuf->46,0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;

		//<空气流量>,46,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Air_flow);
		//pBuf->0,46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;
		//<节气门位置>,0,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Throttle_Position);
		//pBuf->46,*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;
		//<单次行驶里程>,46,
		pBuf = getStr_betweenComma(pBuf,pOBDInfo->Single_mileage);
		//pBuf->*E5\r\n
		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;
		//<百分比剩余油量>,
		pBuf = getStr_betweenComma(pBuf,strTemp);

		if(pBuf==pCheckCode)
		{
			break;
		}
		gpsInfo.nHaveOBDInfo++;
		//<故障码及冻结帧/就绪状态>
		pBuf = getStr_betweenComma(pBuf,strTemp);

		if(pBuf==pCheckCode)
		{
			break;
		}
		
		gpsInfo.nHaveOBDInfo++;

	}while(0);
	
	return (nRet>0)?nBufferDataLen:nRet;
}

BOOL GPS_MeiTrack::getResMsg( char *strBuf,GPSINFO &gpsInfo )
{
	return FALSE;
}

void GPS_MeiTrack::getMsgID( char *buf,GPSINFO &gpsInfo )
{

}
void GPS_MeiTrack::getFullCommandLine(TCHAR *pCommandLine,TCHAR *pParameters,TCHAR *pCmdCH)
{
	int nLen = strlen(pParameters)+5;//要加上<*检验码>\r\n  的长度	
	sprintf(pCommandLine,_T("@@%s%d%s*"),
		pCmdCH,
		nLen,
		pParameters);
	//计算出校验码
	nLen = strlen(pCommandLine);
	getCheckCode(pCommandLine,nLen,pCommandLine+nLen);
	strcat(pCommandLine,"\r\n");	
}
long GPS_MeiTrack::_handleCmd_overspeed( GPSCommand*pGpsCommand,int nMaxSpeed,int nMinSpeed,int nContinue,int nValid/*=1*/ )
{
	//@@<数据包标识符><数据长度>,<IMEI>,<指令类型>,<指令内容><*检验码>\r\n
	//formatParam	
	//@@P28,353358017784062,B07,60*05\r\n 
	sprintf(pGpsCommand->commandParameters,",%s,B07,%d*",pGpsCommand->strDevID,nMaxSpeed);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"P");	
	return 1;
}
#include <math.h>
#define PI 3.141592653589793
double rad(double d)
{
	return d*PI/180.0;
}
double round(double d)
{
	return floor(d + 0.5);
}
double getDistance(double dlat1,double dlng1,double dlat2,double dlng2)
{
	double radlat1 = rad(dlat1);
	double radlat2 = rad(dlat2);

	double a = radlat1-radlat2;
	double b = rad(dlng1)-rad(dlng2);

	double s = 2*asin(sqrt(pow(sin(a/2),2) +cos(radlat1)*cos(radlat2)*pow(sin(b/2),2)));
	s = s*6378.137;
	s = round(s*10000)/10000;
	return s;
}
long GPS_MeiTrack::_handleCmd_SetArea( GPSCommand*pGpsCommand,TCHAR *pAreaID,TCHAR *palertType,TCHAR *pType,TCHAR *pLeftLat,TCHAR *prightlat,TCHAR *pleftlng,TCHAR *prightlng,TCHAR *pcenterlat,TCHAR *pcenterlng,TCHAR *pRadius )
{
	//{
		//可以考虑把矩形转换为圆形, 圆是矩形的外周圆（四个顶点在圆上）
		TCHAR pTemp[1000];
		memset(pTemp,0,100);
		strcpy(pTemp,pLeftLat);
		double dDDD_leftlat =coverLatitude(pTemp);
		double dleftlat = atof(pTemp);

		memset(pTemp,0,100);
		strcpy(pTemp,pleftlng);
		double dDDD_leftlng = coverLongitude(pTemp);
		double dleftlng = atof(pTemp);

		memset(pTemp,0,100);
		strcpy(pTemp,prightlat);
		double dDDD_rightlat = coverLatitude(pTemp);
		double drightlat = atof(pTemp);

		memset(pTemp,0,100);
		strcpy(pTemp,prightlng);
		double dDDD_rightlng = coverLongitude(pTemp);
		double drightlng = atof(pTemp);
		//中心坐标
		//*
		double dcenterlat = (dleftlat+drightlat)/2;
		double dcenterlng = (dleftlng+drightlng)/2;

		//半径:
		//外接园
		//√（pleftlat-prightlat）2+(prightlng-pleftlng)2
		double dRadius = sqrt(pow((dleftlat-drightlat),2)+pow((drightlng-dleftlng),2));
		//内切圆
		//半径为最长边的一半
		double dlat = drightlat-dleftlat;
		double dlng = drightlng-dleftlng;
		double dRadius2 = ((dlat>dlng)?dlat:dlng)/2;

		//这里需要把DDMMmmm转为 度

		double dRadius3 = getDistance(dDDD_leftlat,dDDD_leftlng,dDDD_rightlat,dDDD_rightlng);
		dRadius3 = dRadius3/2.0;

		/*
		TCHAR *pCenterLat=pTemp;
		gcvt(dcenterlat,7,pCenterLat);
		int nLen = strlen(pCenterLat);
		TCHAR*pCenterlng = pCenterLat+nLen+1;
		gcvt(dcenterlng,8,pCenterlng);
		nLen = strlen(pCenterlng);
		TCHAR*pRAdius = pCenterlng+nLen+1;
		gcvt(dRadius,10,pRAdius);
		*/

	//}

	//@@H55,353358017784062,B05,1,22.91319,114.07988,1000,0,1*31\r\n
	//GPRS 发送内容  @@H55,353358017784062,B05,1,22.91319,114.07988,1000,0,1*31\r\n 
	//GPRS 设置回复  $$H28,353358017784062,B05,OK*F7\r\n
	strcpy(pGpsCommand->strCMDID,"H_B05");
	BOOL bIn= (strcmp(palertType,"4")==0);
//	coverLatitude(pcenterlat);
//	coverLongitude(pcenterlng);

	sprintf(pGpsCommand->commandParameters,",%s,B05,%s,%.6f,%.6f,%.6f,%d,%d",
		pGpsCommand->strDevID,
		pAreaID,
		dcenterlat,
		dcenterlng,
		dRadius,
		bIn,
		!bIn);
	sprintf(pGpsCommand->commandParameters,",863070010827342,B05,1,22.913190,114.079880,1000,0,1");
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"H");	
	return 1;
}

long GPS_MeiTrack::_handleCmd_CacelArea( GPSCommand*pGpsCommand,TCHAR *pAreaID )
{
	//GPRS 发送内容  @@J27,353358017784062,B06,1*C8\r\n 
	//GPRS 设置回复  $$J28,353358017784062,B06,OK*FA\r\n 	
	{
		//$$J28,353358017784062,B06,OK*FA\r\n 反馈值
		//提取 J_B06
		strcpy(pGpsCommand->strCMDID,"J_B06");
	}
	sprintf(pGpsCommand->commandParameters,",%s,B06,%s",pGpsCommand->strDevID,pAreaID);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"J");		
	return 1;
}

long GPS_MeiTrack::_handleCmd_SetHeartTime( GPSCommand*pGpsCommand,TCHAR *pInterval )
{
	//GPRS 发送内容  @@S28,353358017784062,A11,10*FD\r\n 
	//GPRS 设置回复  $$S28,353358017784062,A11,OK*FE\r\n 
	strcpy(pGpsCommand->strCMDID,"S_A11");
	sprintf(pGpsCommand->commandParameters,",%s,A11,%s",pGpsCommand->strDevID,pInterval);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"S");		
	return 1;
}

long GPS_MeiTrack::_handleCmd_SetTimingTrack( GPSCommand*pGpsCommand,TCHAR *pInterval,TCHAR*pReportCount )
{
	//GPRS 发送内容  @@V29,353358017784062,A12,6,0*33\r\n 
	//GPRS 设置回复  $$V28,353358017784062,A12,OK*02\r\n
	strcpy(pGpsCommand->strCMDID,"V_A12");
	sprintf(pGpsCommand->commandParameters,",%s,A12,%s,%s",pGpsCommand->strDevID,pInterval,pReportCount);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"V");
	return 1;
}

long GPS_MeiTrack::_handleCmd_SetTurnReport( GPSCommand*pGpsCommand,TCHAR*pAngle )
{
	//GPRS 发送内容  @@X29,353358017784062,A13,120*37\r\n 
	//GPRS 设置回复  $$X28,353358017784062,A13,OK*05\r\n
	strcpy(pGpsCommand->strCMDID,"X_A13");
	sprintf(pGpsCommand->commandParameters,",%s,A13,%s",pGpsCommand->strDevID,pAngle);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"X");
	return 1;
}

long GPS_MeiTrack::_handleCmd_SetGPRSParam( GPSCommand*pGpsCommand,TCHAR*pLinkMod,TCHAR*pIP,TCHAR*pPort,TCHAR*pAPN,TCHAR*pAPN_UserName,TCHAR*pAPN_PassWord )
{
	//GPRS 发送内容  @@H53,353358017784062,A21,1, www.trackingmate.com,8500,CMNET,,*FB\r\n 
	//GPRS 设置回复  $$H28,353358017784062,A21,OK*F4\r\n
	strcpy(pGpsCommand->strCMDID,"H_A21");
	sprintf(pGpsCommand->commandParameters,",%s,A21,%s,%s,%s,%s,%s,%s",
											pGpsCommand->strDevID,
											pLinkMod,
											pIP,
											pPort,
											pAPN,
											pAPN_UserName,
											pAPN_PassWord);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"H");
	return 1;
}

long GPS_MeiTrack::_handleCmd_Set_BackGPRSServer( GPSCommand*pGpsCommand,TCHAR*pIP,TCHAR*pPort )
{
	//GPRS 发送内容  @@S43,353358017784062,A23,112.91.12.222,8500*16\r\n 
	//GPRS 设置回复  $$S28,353358017784062,A23,OK*01\r\n	
	strcpy(pGpsCommand->strCMDID,"S_A23");	
	sprintf(pGpsCommand->commandParameters,",%s,A23,%s,%s",
											pGpsCommand->strDevID,
											pIP,
											pPort);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"S");

	return 1;

}

long GPS_MeiTrack::_handleCmd_Get_ErrorCode( GPSCommand*pGpsCommand )
{
	//GPRS 发送内容  @@V29,353358017784062, D42*33\r\n 
	//GPRS 设置回复  $$V28,353358017784062,D42,…*02\r\n
	strcpy(pGpsCommand->strCMDID,"V_D42");	
	sprintf(pGpsCommand->commandParameters,",%s,D42",pGpsCommand->strDevID);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"V");
	return 1;
}

long GPS_MeiTrack::_handleCmd_Set_VehicleType( GPSCommand*pGpsCommand,TCHAR*pVehicleType )
{	
	//GPRS 发送内容  @@V29,353358017784062, D49,2*33\r\n 
	//GPRS 设置回复  $$V28,353358017784062,D49,OK*02\r\n
	strcpy(pGpsCommand->strCMDID,"V_D49");	
	sprintf(pGpsCommand->commandParameters,",%s,D49,%s",pGpsCommand->strDevID,pVehicleType);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"V");
	return 1;
}

long GPS_MeiTrack::_handleCmd_Get_VehicleType( GPSCommand*pGpsCommand )
{
	//GPRS 发送内容  @@V29,353358017784062, D50*33\r\n 
	//GPRS 设置回复  $$V28,353358017784062, D50,2*02\r\n
	strcpy(pGpsCommand->strCMDID,"V_D50");	
	sprintf(pGpsCommand->commandParameters,",%s,D50",pGpsCommand->strDevID);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"V");
	return 1;
}

long GPS_MeiTrack::_handleCmd_Set_OpenOBD( GPSCommand*pGpsCommand,TCHAR*pFlag )
{
	//GPRS 发送内容  @@V29,353358017784062, D51,1*33\r\n 
	//GPRS 设置回复  $$V28,353358017784062,D51,OK*02\r\n

	strcpy(pGpsCommand->strCMDID,"V_D51");	
	sprintf(pGpsCommand->commandParameters,",%s,D51,%s",pGpsCommand->strDevID,pFlag);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"V");
	return 1;
}

long GPS_MeiTrack::_handleCmd_Set_OBDParam( GPSCommand*pGpsCommand,int nParamCount,void *pstArrParams )
{
	//GPRS 发送内容  @@V29,353358017784062, D52,1,2,20,2,85,105*33\r\n 
	//GPRS 设置回复  $$V28,353358017784062,D52,OK*02\r\n
	if(nParamCount==0)
		return 0;

	strcpy(pGpsCommand->strCMDID,"V_D52");	
	sprintf(pGpsCommand->commandParameters,",%s,D52",pGpsCommand->strDevID);
	TCHAR chTemp[100];
	struct tagInnerStruct 
	{
		int nType;
		int value_xx;
		int value_yy;
	};

	tagInnerStruct *ppstArrParams= (tagInnerStruct*)pstArrParams;
	for (int i=0;i<nParamCount;i++)
	{
		sprintf(chTemp,",%d,%d,%d",ppstArrParams->nType,ppstArrParams->value_xx,ppstArrParams->value_yy);
		strcat(pGpsCommand->commandParameters,chTemp);
	}
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"V");

	return 1;
}

long GPS_MeiTrack::_handleCmd_Set_Reset_Mileage_and_Runtime( GPSCommand*pGpsCommand )
{
	//GPRS 发送内容  @@D27,353358017784062,F06,1*C6\r\n 
	//GPRS 设置回复  $$D28,353358017784062,F06,OK*F8\r\n
	strcpy(pGpsCommand->strCMDID,"D_F06");	
	sprintf(pGpsCommand->commandParameters,",%s,F06",pGpsCommand->strDevID);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"D");

	return 1;
}

long GPS_MeiTrack::_handleCmd_Get_Device_Version_and_SN( GPSCommand*pGpsCommand,TCHAR*psetType )
{
	//GPRS 发送内容  @@W25,353358017784062,E91*7D\r\n 
	//GPRS 设置回复  $$W38,353358017784062,FWV1.00,12345678*1C\r\n

	strcpy(pGpsCommand->strCMDID,"W_E91");	
	sprintf(pGpsCommand->commandParameters,",%s,E91,%s",pGpsCommand->strDevID,psetType);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"W");

	return 1;
}

long GPS_MeiTrack::_handleCmd_Set_Oil_LowAlarm( GPSCommand*pGpsCommand,TCHAR*poilvalue )
{

	//GPRS 发送内容  @@V29,353358017784062, D63,30*33\r\n 
	//GPRS 设置回复  $$V28,353358017784062,D63,OK*02\r\n

	strcpy(pGpsCommand->strCMDID,"V_D63");	
	sprintf(pGpsCommand->commandParameters,",%s,D63,%s",pGpsCommand->strDevID,poilvalue);
	getFullCommandLine(pGpsCommand->strCommandLine,pGpsCommand->commandParameters,"V");

	return 1;
}

BOOL GPS_MeiTrack::isThisProtocol( char *buf ,GPSINFO *pGpsInfo)
{
	if(buf[0]=='$' && buf[1]=='$')
	{
		pGpsInfo->nDevID +=GPS_MEITRACK;
		pGpsInfo->nDevID +=GPSID_TID;
		if(m_i64RecvCnt++ %100==0)
		{
			Write_Log("meiligao",buf);
		}
		return TRUE;
	}
	return FALSE;
}

char* GPS_MeiTrack::getProtocolName( int  &nDevID )
{
	nDevID = GPS_MEITRACK;
	return _T("gps_meitrack");
}
