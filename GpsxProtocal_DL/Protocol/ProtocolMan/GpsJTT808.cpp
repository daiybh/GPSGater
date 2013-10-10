#include "stdafx.h"
#include "GpsJTT808.h"

GpsJTT808::GpsJTT808(void)
{
	m_MsgSN =0;
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
	OutBfr[Dptr++]=0x7e; //0字节搞成0x7e

	for(i=0;i<Length;i++)
	{//从0开始 就是内容 而不是 0x7e
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
		nCode =nCode^(pstrBuf[i]&0xFF);		
	}
	*strCheckCode=nCode&0xFF;
	//sprintf(strCheckCode,"%.2X",nCode&0xFF);
	return nCode&0xFF;
}

void buf2HexStr2(const char *pSrcbuf,char *pDestBuf,int nLen)
{
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
long GpsJTT808::getGpsInfo( char *pSrcbuf,int nbufLen,GPSINFO &gpsInfo )
{
	//7E 01 02 00 07 01 57 00 00 00 01 00 05 72 6F 70 65 6E 74 65 21 7E	
	BYTE *pTempBuf = new BYTE[nbufLen];
	memcpy(pTempBuf,pSrcbuf,nbufLen);

	int nRet = 1;
	int nBufferDataLen = max(strlen((char*)pTempBuf),nbufLen);
	int iTrueLen=1;
	for(;iTrueLen<nBufferDataLen;iTrueLen++){
		if(pTempBuf[iTrueLen]== 0x7e)break;
	}
	iTrueLen -=1;//去掉头尾
	if(iTrueLen<2){
		delete[] pTempBuf;	
		return -1;
	}
	int nFanzhuanyiLen = FanZhuanYi((char*)pTempBuf,iTrueLen+2);
	iTrueLen = nFanzhuanyiLen;
	const BYTE *pBuf =pTempBuf;
	//pBuf = buf+1;
	int nMask = *(pBuf+iTrueLen-1)&0xFF;

	char pCheckCode;
	int nCheckCode = getCheckCode_xor((char*)pBuf,iTrueLen-1,&pCheckCode);
	if(nMask !=nCheckCode)
	{
		delete[] pTempBuf;
		return -2;
	}

	
	tagMsgHead msgHead;
	msgHead.msgID = MAKEWORD(pBuf[1],pBuf[0]);

	int nAttribute = MAKEWORD(pBuf[3],pBuf[2]);
	msgHead.msgBodyAttribute =nAttribute;//(pBuf[3]<<8)|pBuf[2];
	

	const BYTE *pSim = pBuf+4;
	sprintf(msgHead.sim,"%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
		pSim[0]&0xFF,pSim[1]&0xFF,pSim[2]&0xFF,pSim[3]&0xFF,pSim[4]&0xFF,pSim[5]&0xFF);
	strcpy(gpsInfo.COMMADDR,msgHead.sim);
	msgHead.msgSN = MAKEWORD(pBuf[11],pBuf[10]);//pBuf[11]<<8+pBuf[10];
	int nHeadLen = 12;
	if(msgHead.msgBodyAttribute.bPaket)
	{
		msgHead.msgPaket.msgTotalPakets = MAKEWORD(pBuf[13],pBuf[12]);;
		msgHead.msgPaket.msgPaketIdx = MAKEWORD(pBuf[15],pBuf[14]);;
		nHeadLen = 16;
	}
	//根据msgid 处理msgboday
	nRet = diposMsgBody(msgHead,pBuf+nHeadLen,&gpsInfo);
	if(nRet==0)
	{
		//写成文件保留下来
	//	char pdestBuf[1024];
	//	ZeroMemory(pdestBuf,1024);
	//	buf2HexStr2(pSrcbuf,pdestBuf,nbufLen);

	//	WriteLog(_T("jtt808"),logLevelError,pdestBuf);
	}
	delete[] pTempBuf;
	return iTrueLen+1;
	//return nRet>0 ?(iTrueLen+1):0;
}

int GpsJTT808::getFullCmdLine(char *pDestBuf,tagMsgHead *pMsgHead,char *pstrMsgBody,int nMsgBodyLen)
{
/*
	msgHead.msgID=nMsgID;
	msgHead.msgSN = atoi(gpsInfo.SEQ);
	msgHead.msgBodyAttribute.bPaket=0;
	msgHead.msgBodyAttribute.Rev =0;
	msgHead.msgBodyAttribute.msgDecodeType=0;

	strcpy(msgHead.sim,gpsInfo.COMMADDR);//bcd[6]
*/	//msgHead.msgSN=atoi(gpsInfo.PreFix);
	char pTempBuf[60];
	ZeroMemory(pTempBuf,60);
	int nCurpos = 0;
	pTempBuf[nCurpos++] = 0x7E;
	pTempBuf[nCurpos++] =pMsgHead->msgID/256;
	pTempBuf[nCurpos++] = pMsgHead->msgID;
	nCurpos++;//msgBodyAttribute/256
	nCurpos++;//msgBodyAttribute
	for(int i=0;i<12;i+=2){			
		pTempBuf[nCurpos++]=((pMsgHead->sim[i]-0x30)*16)|(pMsgHead->sim[i+1]-0x30);			
	}
	pTempBuf[nCurpos++]=pMsgHead->msgSN/256;
	pTempBuf[nCurpos++]=pMsgHead->msgSN;
	///header End//////////////////////////
	//body
	//0 		终端消息流水号word
	//2			消息IDword
	//4			结果byte
	pMsgHead->msgBodyAttribute.msgBodyLen=nMsgBodyLen;

	//从13位开始是body
	memcpy(pTempBuf+nCurpos,pstrMsgBody,nMsgBodyLen);

	nCurpos+=nMsgBodyLen;

	WORD msgBodyAttribute = pMsgHead->msgBodyAttribute.getWordValue();
	pTempBuf[3] = msgBodyAttribute/256;
	pTempBuf[4] = msgBodyAttribute;

	getCheckCode_xor(pTempBuf+1,nCurpos,pTempBuf+nCurpos);
	memset(pDestBuf,0,nCurpos+5);
	int nLen = ZhuanYi(pTempBuf+1,pDestBuf,nCurpos);
	pDestBuf[nLen]=0x7E;
	return nLen+1;

}
BOOL GpsJTT808::getResMsg( char *strDestBuf,GPSINFO &gpsInfo )
{
	if(gpsInfo.nMsgID==MSG_LOGIN || gpsInfo.nMsgID== MSG_LOCATION){
		int nMsgID = atoi(gpsInfo.PreFix);
		if(nMsgID<1)return FALSE;
		//鉴权码 返回
		tagMsgHead msgHead;
		msgHead.msgID=nMsgID;
		msgHead.msgSN = MAKEWORD(gpsInfo.SEQ[1],gpsInfo.SEQ[0]);//atoi(gpsInfo.SEQ);
		msgHead.msgBodyAttribute.bPaket=0;
		msgHead.msgBodyAttribute.Rev =0;
		msgHead.msgBodyAttribute.msgDecodeType=0;

		memcpy(msgHead.sim,gpsInfo.COMMADDR,strlen(gpsInfo.COMMADDR));//bcd[6]
		//msgHead.msgSN=atoi(gpsInfo.PreFix);
/*		char pTempBuf[20];
		ZeroMemory(pTempBuf,20);
		pTempBuf[0] = 0x7E;
		pTempBuf[1] = msgHead.msgID/256;
		pTempBuf[2] = msgHead.msgID;
		int nCurpos = 5;
		for(int i=0;i<12;i+=2){			
			pTempBuf[nCurpos++]=((msgHead.sim[i]-0x30)*16)|(msgHead.sim[i+1]-0x30);			
		}


		pTempBuf[11]=msgHead.msgSN/256;
		pTempBuf[12]=msgHead.msgSN;



		pTempBuf[13]=msgHead.msgSN/256;
		pTempBuf[14]=msgHead.msgSN;
		msgHead.msgBodyAttribute.msgBodyLen=0;
		if(msgHead.msgID== 0x8001){
			msgHead.msgBodyAttribute.msgBodyLen=5;
			pTempBuf[15]=gpsInfo.SEQ[3];
			pTempBuf[16]=gpsInfo.SEQ[4];	
			pTempBuf[17]= 0;
			nCurpos = 17;
		}else if(msgHead.msgID== 0x8100){
			nCurpos=15;
			pTempBuf[nCurpos++]=0;
			//726f70656e7465
			char *pCode ="726f70656e7465";
			int sLen = strlen(pCode);			
			for(int i=0;i<sLen;i+=2)
			{
				pTempBuf[nCurpos++] =getbin(pCode[i])*16|getbin(pCode[i+1]);
			}

			msgHead.msgBodyAttribute.msgBodyLen=nCurpos-13;
			nCurpos--;
		}
		WORD msgBodyAttribute = msgHead.msgBodyAttribute.getWordValue();
		pTempBuf[3] = msgBodyAttribute/256;
		pTempBuf[4] = msgBodyAttribute;
		getCheckCode_xor(pTempBuf+1,nCurpos,&pTempBuf[nCurpos+1]);
		memset(strDestBuf,0,30);
		int nLen = ZhuanYi(pTempBuf+1,strDestBuf,nCurpos+1);
		strDestBuf[nLen]=0x7E;
		return nLen+1;

/**/
		///header End//////////////////////////
		//body
		//0 		终端消息流水号word
		//2			消息IDword
		//4			结果byte
		char pMsgBody[40];
		int nMsgBodyPos=0;
		pMsgBody[nMsgBodyPos++] = msgHead.msgSN/256;
		pMsgBody[nMsgBodyPos++] = msgHead.msgSN;
		
		if(msgHead.msgID== 0x8001){
			pMsgBody[nMsgBodyPos++] = gpsInfo.SEQ[3];
			pMsgBody[nMsgBodyPos++] = gpsInfo.SEQ[4];
			pMsgBody[nMsgBodyPos++] =0;
		}else if(msgHead.msgID == 0x8100){
			pMsgBody[nMsgBodyPos++] =0;
			//726f70656e7465
			char *pCode ="726f70656e7465";
			int sLen = strlen(pCode);			
			for(int i=0;i<sLen;i+=2){
				pMsgBody[nMsgBodyPos++] =getbin(pCode[i])*16|getbin(pCode[i+1]);
			}
		}
		msgHead.msgBodyAttribute.msgBodyLen = nMsgBodyPos;
		return getFullCmdLine(strDestBuf,&msgHead,pMsgBody,nMsgBodyPos);
		
	}
	return 0;
}

DWORD getDword(const BYTE *pMsgBody)
{
	DWORD dRet =0;
	//dRet = ((pMsgBody[0])*4096)|((pMsgBody[1])*256)|((pMsgBody[2])*16)|((pMsgBody[3]));
	DWORD d1,d2,d3,d4;
	d1=d2=d3=d4 =0;
	d1 = (pMsgBody[0]&0xff)*256*256*256;
	d2 = (pMsgBody[1]&0xff)*256*256;
	d3 = (pMsgBody[2]&0xff)*256;
	d4 = (pMsgBody[3]&0xff);
	dRet = d1|d2|d3|d4;
	return dRet;
}
int GpsJTT808::diposMsgBody( tagMsgHead msgHead,const BYTE *pMsgBody,GPSINFO *gpsInfo )
{
	int nRet = 0;
	gpsInfo->nMsgID=MSG_NULL;
	switch(msgHead.msgID){
		case 0x0102:
			//0 鉴权码 string 终端重连后上传鉴权码
			//需要平台应答
			gpsInfo->nMsgID =MSG_LOGIN;
			sprintf(gpsInfo->PreFix,"%d",0x8001);
			gpsInfo->bNeedWriteDataBase=false;
			nRet = 1;
			break;
		case 0x0100:
			//7e0100002f015728572436000100111111373034303444522d3230413120202020202020202020202020526f706530303101d4c14147323031310000527e
			gpsInfo->nMsgID =MSG_LOGIN;
			sprintf(gpsInfo->PreFix,"%d",0x8100);
			nRet =1;
			break;
		case 0x0200:
			{
			//位置基本信息，位置附加信息项列表
				/*
					01 2345 6789 012345678901 2345 67890123 45678901 23456789 01234567 8901 2345 6789 012345678901 23 45 6789 0123 4567 8901 23 45
					7e 0200 0026 015728572436 0172 00000000 00000003 01614058 06c2e200 00e4 0000 0000 130906100802 01 04 0000 0000 0302 0000 d7 7e
												   0 1 2 3  4 5 6 7  8 9 0 1  2 3 4 5  6 7  8 9  0 1  2 3 4 5 6 7  8  9  0 1  2 3  4 5  6 7  8  9  0123 456789012345 67 89 0123 4567 8901 2345 67 89
				    7e 0200 0026 015728572436 01db 00000000 00000003 01614058 06c2e200 00e4 0000 0000 130906100802 010400000000030200007d027e
				*/
				gpsInfo->nMsgID = MSG_LOCATION;
				sprintf(gpsInfo->PreFix,"%d",0x8001);

				struct LocInfo 
				{
					DWORD alarmFlag;
					DWORD status;
					DWORD latitude;
					DWORD longitude;
					WORD altitude;
					WORD speed;
					WORD direct;
					char time[6];
					void copy2GpsInfo(GPSINFO *gpsinfo)
					{
						gpsinfo->nWarnFlag=0;
						if(alarmFlag&0x02==0x02)
							gpsinfo->nWarnFlag|=WAR_OVERSPEED;

						/*if(alarmFlag&0x1000==0x1000)
							gpsinfo->nWarnFlag |=WAR_OVERSPEED;
						/**/
						if(alarmFlag&0x100000 == 0x100000)
							gpsinfo->nWarnFlag |=WAR_INZONE;

//						gpsinfo->nWarnFlag = alarmFlag;
						sprintf(gpsinfo->Latitude,"%f",(double(latitude))/1000000);

						sprintf(gpsinfo->Longitude,"%f",(double(longitude))/1000000);
						sprintf(gpsinfo->Altitude,"%d",altitude);
						sprintf(gpsinfo->Speed,"%f",(double(speed))/10);
						sprintf(gpsinfo->Heading,"%d",direct);
						sprintf(gpsinfo->Time,"20%s",time);
						gpsinfo->Time[19] = 0x01;
					}
				};
				LocInfo locationInfo;
				//locationInfo.alarmFlag = ((pMsgBody[0])*4096)|((pMsgBody[1])*256)|((pMsgBody[2])*16)|((pMsgBody[3]));
				locationInfo.alarmFlag = getDword(pMsgBody);

				
				//locationInfo.status = ((pMsgBody[4])*4096)|((pMsgBody[5])*256)|((pMsgBody[6])*16)|((pMsgBody[7]));
				locationInfo.status = getDword(pMsgBody+4);
				//locationInfo.latitude = ((pMsgBody[8])*4096)|((pMsgBody[9])*256)|((pMsgBody[10])*16)|((pMsgBody[11]));
				locationInfo.latitude =getDword(pMsgBody+8);				
				//locationInfo.longitude = ((pMsgBody[12])*4096)|((pMsgBody[13])*256)|((pMsgBody[14])*16)|((pMsgBody[15]));
				locationInfo.longitude =getDword(pMsgBody+12);				
				
				locationInfo.altitude = ((pMsgBody[16])*256)|((pMsgBody[17]));
				locationInfo.speed = ((pMsgBody[18])*256)|((pMsgBody[19]));
				locationInfo.direct =((pMsgBody[20])*256)|((pMsgBody[21]));

				const BYTE *pTime = pMsgBody+22;
				sprintf(locationInfo.time,"%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
					pTime[0]&0xFF,pTime[1]&0xFF,pTime[2]&0xFF,pTime[3]&0xFF,pTime[4]&0xFF,pTime[5]&0xFF);

				locationInfo.copy2GpsInfo(gpsInfo);
				gpsInfo->bValid = (locationInfo.status&0x2);

				//8-9:  00：空车；01：半载；10：保留；11：满载
				gpsInfo->Noload =(locationInfo.status>>8 )&0x3;
				
				const BYTE *pAddtionBuf = pMsgBody+28;
				struct AddtionInfo
				{
					int addtionID;
					int addtionLen;
				};
				AddtionInfo addtionInfo;
				addtionInfo.addtionID = pAddtionBuf[0];

				addtionInfo.addtionLen = pAddtionBuf[1];
				switch(addtionInfo.addtionID)
				{
				case 0x01:
					//c车上的里程数
					{
						double dMileage = getDword(pAddtionBuf+2);
						sprintf(gpsInfo->st_OBD_Info.Mileage ,"%f", (dMileage/10));
					}
					break;
				default:
					break;
				}

				//gpsInfo->Latitude;
				nRet = 1;
			}
			break;
		case 0x002:
			nRet = 1;
			break;
		default:
			{
				

			}
			break;
	}
	if(gpsInfo->nMsgID==MSG_LOGIN){
		gpsInfo->bValid=false;	
	}
	if(gpsInfo->nMsgID != MSG_NULL)
	{
		gpsInfo->SEQ[0] = msgHead.msgSN/256;
		gpsInfo->SEQ[1] = msgHead.msgSN;

		gpsInfo->SEQ[2] ='\0';

		gpsInfo->SEQ[3] = msgHead.msgID/256;
		gpsInfo->SEQ[4] = msgHead.msgID;
		nRet = 1;
	}
	return nRet;
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
	tagMsgHead msgHead;

	msgHead.msgID=0x8103;
	msgHead.msgSN =m_MsgSN++;
	msgHead.msgBodyAttribute.bPaket=0;
	msgHead.msgBodyAttribute.Rev =0;
	msgHead.msgBodyAttribute.msgDecodeType=0;
	
	memcpy(msgHead.sim,pGpsCommand->strSim,strlen(pGpsCommand->strSim));

	char pMsgBody[40];
	int nMsgBodyPos = 0;
	int nParamsCount=2;
	pMsgBody[nMsgBodyPos++] = nParamsCount;

	struct tagParam{
		DWORD dwParamID;
		BYTE  byteParamLen;
		char *pstrParam;
	};
	{//最大速度 km/h
		tagParam tagparam ;
		tagparam.dwParamID = 0x0055;
		tagparam.byteParamLen = sizeof(DWORD);
		pMsgBody[nMsgBodyPos++] = tagparam.dwParamID/256/256/256;
		pMsgBody[nMsgBodyPos++] = tagparam.dwParamID/256/256;
		pMsgBody[nMsgBodyPos++] = tagparam.dwParamID/256;
		pMsgBody[nMsgBodyPos++] = tagparam.dwParamID;

		pMsgBody[nMsgBodyPos++] = tagparam.byteParamLen;

		pMsgBody[nMsgBodyPos++] = nMaxSpeed/256/256/256;
		pMsgBody[nMsgBodyPos++] = nMaxSpeed/256/256;
		pMsgBody[nMsgBodyPos++] = nMaxSpeed/256;
		pMsgBody[nMsgBodyPos++] = nMaxSpeed;
	}
	{
		//持续时间 s
		tagParam tagparam ;
		tagparam.dwParamID = 0x0056;
		tagparam.byteParamLen = sizeof(DWORD);
		pMsgBody[nMsgBodyPos++] = tagparam.dwParamID/256/256/256;
		pMsgBody[nMsgBodyPos++] = tagparam.dwParamID/256/256;
		pMsgBody[nMsgBodyPos++] = tagparam.dwParamID/256;
		pMsgBody[nMsgBodyPos++] = tagparam.dwParamID;

		pMsgBody[nMsgBodyPos++] = tagparam.byteParamLen;

		pMsgBody[nMsgBodyPos++] = nContinue/256/256/256;
		pMsgBody[nMsgBodyPos++] = nContinue/256/256;
		pMsgBody[nMsgBodyPos++] = nContinue/256;
		pMsgBody[nMsgBodyPos++] = nContinue;
	}
	int nlen = getFullCmdLine(pGpsCommand->strCommandLine,&msgHead,pMsgBody,nMsgBodyPos);
	pGpsCommand->nLenCommandLine = nlen;
	return nlen;
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
