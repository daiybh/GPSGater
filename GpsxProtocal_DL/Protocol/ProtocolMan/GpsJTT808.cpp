#include "stdafx.h"
#include "GpsJTT808.h"

GpsJTT808::GpsJTT808(void)
{
}

GpsJTT808::~GpsJTT808(void)
{
}

int FanZhuanYi(char *Bfr,int Length)
{
	int Dptr=0,i;
	for(i=1;i<Length-1;i++)
	{
		if(Bfr[i]==0x7d)
		{
			i++;
			if(Bfr[i]==0x02)
				Bfr[Dptr++]=0x7e;
			else if(Bfr[i]==0x01)
				Bfr[Dptr++]=0x7d;
		}
		else if(Bfr[i]==0x7e)
		{
			return Dptr;
		}
		else
			Bfr[Dptr++]=Bfr[i];
	}
	return Dptr;
}
int ZhuanYi(char* InBfr,char *OutBfr,int Length)
{
	int Dptr=0,i;
	OutBfr[Dptr++]=0x7e;
	for(i=0;i<Length;i++)
	{
		if(InBfr[i]==0x7e)
		{
			OutBfr[Dptr++]=0x7d;
			OutBfr[Dptr++]=0x02;
		}
		else if(InBfr[i]==0x7d)
		{
			OutBfr[Dptr++]=0x7d;
			OutBfr[Dptr++]=0x01;
		}
		else
			OutBfr[Dptr++]=InBfr[i];
	}
	return Dptr;
}
/*
标识位 消息头 消息体 检验码 标识位
0x7e							 0x7e

找到后一个0x7e
得到总长度
转义还原
校验
解析信息
*/
char *g_pbuf="7E 01 02 00 07 01 57 00 00 00 01 00 05 72 6F 70 65 6E 74 65 21 7E";

void RemoveBlank(char *pBuf)
{
	char *pSrc=pBuf;
	char *pDest = pBuf;
	char *pEnd = pBuf+strlen(pBuf);
	for (;pSrc!=pEnd,pDest!=pEnd;)
	{
		int x = *pSrc++;
		/*
		if(pSrc==' '){
			pSrc++;
		}
		else{
			memcpy(pDest,pSrc,1);		
			pSrc++;
			pDest++;
		}
		/**/
	}
}
int getCheckCode_xor(const char *pstrBuf,int nStrLen,char *strCheckCode)
{
	long nCode = 0;
	for(int i=0;i<nStrLen;i++)
	{
		nCode =nCode^(pstrBuf[i]);
	}
	sprintf(strCheckCode,"%.2X",nCode&0xFF);
	return nCode&0xFF;
}
long GpsJTT808::getGpsInfo( char *buf,int nbufLen,GPSINFO &gpsInfo )
{
	//7E 01 02 00 07 01 57 00 00 00 01 00 05 72 6F 70 65 6E 74 65 21 7E	
	int nRet = 1;
	int nBufferDataLen = max(strlen(buf),nbufLen);
	int iTrueLen=1;
	for(;iTrueLen<nBufferDataLen;iTrueLen++){
		if(buf[iTrueLen]== 0x7e)break;
	}
	iTrueLen -=1;//去掉头尾
	if(iTrueLen<2)return -1;
	FanZhuanYi(buf,iTrueLen+2);
	const char *pBuf =buf;
	//pBuf = buf+1;
	int nMask = *(pBuf+iTrueLen-1);

	char pCheckCode;
	int nCheckCode = getCheckCode_xor(pBuf,iTrueLen-1,&pCheckCode);
	if(nMask !=nCheckCode)
		return -2;

	
	tagMsgHead msgHead;
	msgHead.msgID = MAKEWORD(pBuf[1],pBuf[0]);

	int nAttribute = MAKEWORD(pBuf[3],pBuf[2]);
	msgHead.msgBodyAttribute =nAttribute;//(pBuf[3]<<8)|pBuf[2];

	const char *pSim = pBuf+4;
	sprintf(msgHead.sim,"%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
		pSim[0]&0xFF,pSim[1]&0xFF,pSim[2]&0xFF,pSim[3]&0xFF,pSim[4]&0xFF,pSim[5]&0xFF);
	strcpy(gpsInfo.COMMADDR,msgHead.sim);
	msgHead.msgSN = MAKEWORD(pBuf[11],pBuf[10]);//pBuf[11]<<8+pBuf[10];
	if(msgHead.msgBodyAttribute.bPaket)
	{
		msgHead.msgPaket.msgTotalPakets = MAKEWORD(pBuf[13],pBuf[12]);;
		msgHead.msgPaket.msgPaketIdx = MAKEWORD(pBuf[15],pBuf[14]);;
	}
	//根据msgid 处理msgboday
	diposMsgBody(msgHead,pBuf+16,&gpsInfo);

	return 1;
}

BOOL GpsJTT808::getResMsg( char *strBuf,GPSINFO &gpsInfo )
{
	if(gpsInfo.nMsgID==MSG_LOGIN){
		//鉴权码 返回
		tagMsgHead msgHead;
		msgHead.msgID=0x8001;
		msgHead.msgBodyAttribute.bPaket=0;
		msgHead.msgBodyAttribute.Rev =0;
		msgHead.msgBodyAttribute.msgDecodeType=0;

		strcpy(msgHead.sim,gpsInfo.COMMADDR);//bcd[6]
		msgHead.msgSN=atoi(gpsInfo.PreFix);

		char msgBody[1024];
		sprintf(msgBody,"%s%s%s",msgHead.msgSN,gpsInfo.SEQ,gpsInfo.Latitude);
		int nBodyLen = strlen(msgBody);
		msgHead.msgBodyAttribute.msgBodyLen=nBodyLen;

		//   msgID attri s------i-------m      msgSN   b----o--d----------y 
		//7E 01 02 00 07 01 57 00 00 00 01     00 01   72 6F 70 65 6E 74 65     25 7E
		//7e 80 01 
	}
	return 1;
}

int GpsJTT808::diposMsgBody( tagMsgHead msgHead,const char *pMsgBody,GPSINFO *gpsInfo )
{
	switch(msgHead.msgID){
		case 0x0102:
			//0 鉴权码 string 终端重连后上传鉴权码
			//需要平台应答
			gpsInfo->nMsgID =MSG_LOGIN;
			gpsInfo->bValid=false;
			
			/*
			回复需要：
				终端消息流水号word
				消息IDword
				结果byte
			*/
			sprintf(gpsInfo->PreFix,"%d",msgHead.msgSN);
			sprintf(gpsInfo->SEQ,"%d",msgHead.msgID);
			sprintf(gpsInfo->Latitude,"%d",0);

			break;
	}
	return 1;
}

BOOL GpsJTT808::isThisProtocol( char *buf ,GPSINFO *pGpsInfo)
{
	if(buf[0]==0x7E)
	{
		pGpsInfo->nDevID +=GPS_JTT808;
		pGpsInfo->nDevID +=GPSID_TID;
		return TRUE;
	}
	return FALSE;
}

long GpsJTT808::_handleCmd_overspeed( GPSCommand*pGpsCommand,int nMaxSpeed,int nMinSpeed,int nContinue,int nValid/*=1*/ )
{
	return 0;
}

long GpsJTT808::_handleCmd_SetArea( GPSCommand*pGpsCommand,TCHAR *pAreaID,TCHAR *palertType,TCHAR *pType,TCHAR *pLeftLat,TCHAR *prightlat,TCHAR *pleftlng,TCHAR *prightlng,TCHAR *pcenterlat,TCHAR *pcenterlng,TCHAR *pRadius )
{
	return 0;
}

long GpsJTT808::_handleCmd_CacelArea( GPSCommand*pGpsCommand,TCHAR *pAreaID )
{
	return 0;
}

long GpsJTT808::_handleCmd_SetHeartTime( GPSCommand*pGpsCommand,TCHAR *pInterval )
{

	return 1;
}


long GpsJTT808::_handleCmd_SetTimingTrack( GPSCommand*pGpsCommand,TCHAR *pInterval,TCHAR*pReportCount )
{
	return 1;
}

long GpsJTT808::_handleCmd_SetTurnReport( GPSCommand*pGpsCommand,TCHAR*pAngle )
{
	return 1;
}

long GpsJTT808::_handleCmd_SetGPRSParam( GPSCommand*pGpsCommand,TCHAR*pLinkMod,TCHAR*pIP,TCHAR*pPort,TCHAR*pAPN,TCHAR*pAPN_UserName,TCHAR*pAPN_PassWord )
{

	return 1;
}

long GpsJTT808::_handleCmd_Set_BackGPRSServer( GPSCommand*pGpsCommand,TCHAR*pIP,TCHAR*pPort )
{
	return 1;

}

long GpsJTT808::_handleCmd_Get_ErrorCode( GPSCommand*pGpsCommand )
{
	return 1;

}

long GpsJTT808::_handleCmd_Set_VehicleType( GPSCommand*pGpsCommand,TCHAR*pVehicleType )
{
	return 1;
}

long GpsJTT808::_handleCmd_Get_VehicleType( GPSCommand*pGpsCommand )
{

	return 1;
}

long GpsJTT808::_handleCmd_Set_OpenOBD( GPSCommand*pGpsCommand,TCHAR*pFlag )
{
	return 1;

}

long GpsJTT808::_handleCmd_Set_OBDParam( GPSCommand*pGpsCommand,int nParamCount,void *pstArrParams )
{
	return 1;
}

long GpsJTT808::_handleCmd_Set_Reset_Mileage_and_Runtime( GPSCommand*pGpsCommand )
{

	return 1;
}

long GpsJTT808::_handleCmd_Get_Device_Version_and_SN( GPSCommand*pGpsCommand,TCHAR*psetType )
{

	return 1;
}

long GpsJTT808::_handleCmd_Set_Oil_LowAlarm( GPSCommand*pGpsCommand,TCHAR*poilvalue )
{

	return 1;
}

void GpsJTT808::getMsgID( char *buf,GPSINFO &gpsInfo )
{

}

char* GpsJTT808::getProtocolName( int  &nDevID )
{
	nDevID = GPS_JTT808;
	return _T("GpsJTT808");
}
