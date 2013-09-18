/********************************************************************
	created:	2011/12/13
	created:	13:12:2011   14:57
	filename: 	E:\WORKSPACE\PROJECT\DEV-GPS\CODES\DLL_DEV_GPS\CURGPS\DevxGps\DevxGps.cpp
	file path:	E:\WORKSPACE\PROJECT\DEV-GPS\CODES\DLL_DEV_GPS\CURGPS\DevxGps
	file base:	DevxGps
	file ext:	cpp
	author:		areslion@qq.com
	
	purpose:	
*********************************************************************/
#include <memory.h>
#include <string.h>
#include "DevxGps.h"

#include "MGTOOls.h"
#define LOG_NAME "DevxGPS"

void Write_Log(char*pLogName,const char *pLogContent)
{
	::WriteLog(pLogName,logLevelError,pLogContent);

}
void Write_Log(const char *pLogContent)
{
	::WriteLog(LOG_NAME,logLevelError,pLogContent);
}

void buf2HexStr_devx(const char *pSrcbuf,int nSrcLen,char *pDestBuf,int nDestBufLen)
{
	int nCanCopyLen = min(nSrcLen,nDestBufLen);
	if(pSrcbuf[0] =='*')
	{
		memcpy(pDestBuf,pSrcbuf,nCanCopyLen);
		return;
	}

	//由于要变成双字节，所以占用空间要*2
	nCanCopyLen = min(nSrcLen,nDestBufLen/2);
	//if( nLen> 1024/2)	nLen = 1024;
	char *pPos = pDestBuf;
	for(int i=0;i<nCanCopyLen;i++)
	{
		sprintf(pPos,"%02x",(unsigned char)pSrcbuf[i]);
		pPos+=2;
	}	
}
HANDLE hTh_GetGPS = NULL;
HANDLE hTh_GetDB = NULL;
DWORD  IDThGPS = 0;
DWORD  IDThDB = 0;
DWORD WINAPI threadGetGPSData(LPVOID lpParameter);
DWORD WINAPI threadGetWriteDB(LPVOID lpParameter);


Protocal	Gdev;

unsigned __int64 *g_pi64LastDataTime =0;
unsigned __int64  g_i64MeiligaoCnt=0;
DWORD	*g_dwMapCount=0;

long start(unsigned __int64 &iLastDataTime,DWORD &dwMapCount)
{
	long nret = 0;
	Write_Log("devxGPS_start()");
	g_pi64LastDataTime = &iLastDataTime;
	g_dwMapCount = &dwMapCount;
	hTh_GetGPS = CreateThread(NULL,0,threadGetGPSData,NULL,0,&IDThGPS);
// 	Sleep(30000);//为测试异常暂时关闭从数据库取数据库模块
// 	hTh_GetDB = CreateThread(NULL,0,threadGetWriteDB,NULL,0,&IDThGPS);	
	
	return nret;
}


long stop()
{
	long nret = 0;

	TerminateThread(hTh_GetDB,0);
	TerminateThread(hTh_GetGPS,0);
	
	return nret;
}




#include <time.h>
DWORD WINAPI threadGetGPSData(LPVOID lpParameter)
{
	char *buf = NULL;
	char strFilename[20]="Console";
	int nlen = sizeof(GPSINFO)+20;
	nlen = 50*1024*1024;//与CGPS_Socket.m_pDataList 大小保持一致。
	buf = (char *)malloc(nlen);	
	if(!buf) 
	{
		MessageBox(NULL,("threadGetGPSData-申请buf空间失败"),("Warning"),0);
		return 0;
	}
	GPSCommand *gpsCommand = new GPSCommand();

	char *pTempBuffer = buf;//new char[1024*3];
	char *pstrCommandLine = pTempBuffer;//0---1024/2
	char *pCommandPara = pTempBuffer + 1024/2;//1024/2---1024
	char *pGpsGateDataBuf = pTempBuffer+1024;//1024---1024*3/2
	char *pCommandXMl  = pTempBuffer +1024+1024/2;//1024*3/2---1024*3
	GPSClass *pGpsClass = NULL;


	while (TRUE)
	{
		ZeroMemory(buf,nlen);
		int nRet = Gdev.startGPS(buf,nlen,pGpsGateDataBuf,100);

		//在这里获取处理命令来处理
		ZeroMemory(gpsCommand,sizeof(GPSCommand));
		gpsCommand->strCommandLine		=	pstrCommandLine;
		gpsCommand->commandParameters	=	pCommandPara;
		gpsCommand->pstrCommandXMl		=	pCommandXMl;
		Gdev.Process_Command(gpsCommand,pGpsGateDataBuf);

		Sleep(50);
	}
	
	if(buf!=NULL) free(buf);
	return 0;
}


long Protocal::stop()
{
	long nret = 0;
	return nret;
}





Protocal::Protocal()
{
	//m_pGps = NULL;
	memset(&m_gpsInfo,0x00,sizeof(GPSINFO));
	memset(m_strLogName,0x00,N_LEN);
	strcpy(m_strLogName,"aaaaa");
	strcpy(m_strConsole,"Console");
	InitializeCriticalSectionAndSpinCount(&m_mapLock, 0xFA0);
	
// 	m_gpsIPMap.clear();
}

GPSClass * Protocal::getCurGPSDevice(char *buf,GPSINFO&gpsInfo)
{
	GPSClass * gpsCurDevice=NULL;
	gpsCurDevice = GPSClass::getProtocol(buf,&gpsInfo);
	return gpsCurDevice;	
}
/*
bool Protocal::setCurGpsDev( char *buf,GPSINFO &gpsInfo )
{
	char	strTmp[100] = "";
	int		nlen = 0;

	if(buf[0]=='*'||buf[0]=='$'||buf[0]=='X')
	{
		gpsInfo.nDevID += GPS_RUIXING;
		gpsInfo.nDevID += GPSID_TID;
		m_pGps = &m_gpsRuiXing;

		if(buf[0]=='*') nlen = 77;
		else nlen = 32;
		memcpy(strTmp,buf,nlen);
		memcpy(buf,strTmp,nlen);
	}
	else if(0)
	{
		gpsInfo.nDevID += GPS_YOUHAO;
		gpsInfo.nDevID += GPSID_SIM;
		m_pGps = &m_gpsYouHao;
	}
	else return false;
	m_nDataLen = nlen;
	return true;
}
/**/
void buf2HexStr2(const char *pSrcbuf,char *pDestBuf,int nLen)
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
void wlog( char *buf,int nLen,GPSINFO &gpsInfo,BOOL bToGps/*=TRUE*/ )
{
	char strGpsInfo[1024]="";

	if(bToGps)	sprintf(strGpsInfo,"%s-","[Console]→[GPS]");
	else 
		sprintf(strGpsInfo,"%s-","[GPS]→[Console]");

	strcat(strGpsInfo,buf);
	if(!bToGps)
	{
		//把buf 转换成16进制显示
		buf2HexStr2(buf,strGpsInfo+strlen(strGpsInfo),nLen);
	}
	//TODO: NEEDLOG
	//wlog(gpsInfo.COMMADDR,strGpsInfo);
	Write_Log(gpsInfo.COMMADDR,strGpsInfo);
}
long Protocal::getGpsInfo( GPSClass *pCurGPSClass, char *buf,int nbufLen,GPSINFO &gpsInfo )
{
	long nret = 0;
	BOOL bToGps = FALSE;

	if(!strlen(buf)) bToGps = TRUE;
	//nret = m_pGps->getGpsInfo(buf,gpsInfo);
	nret = pCurGPSClass->getGpsInfo(buf,nbufLen,gpsInfo);
	//TODO: NEEDLOG
	//m_pGps->wlog(buf,m_nDataLen,gpsInfo,bToGps);	
	return nret;
}



long Protocal::SynchronGPSData(GPSClass **pCurGPSClass,char *pInData,int nLenInData,GPSINFO &gpsInfo)
{
	long	nret = 0;

	*pCurGPSClass = getCurGPSDevice(pInData,gpsInfo);
	if(*pCurGPSClass==NULL)
		return -1;
	//if(!setCurGpsDev(pInData,gpsInfo))	return 1;//txt do not fitable any protocal
	nret = getGpsInfo(*pCurGPSClass,pInData,nLenInData,gpsInfo);
	
	return nret<0?(nret-1000):nret;
}



//GPSGATEDATA gpsData
long Protocal::readGPS( GPSGATEDATA &gpsData,char *buf )
{
	long nret = 0;
	
	nret = getGPS(&gpsData,buf);
	return nret;
}

long Protocal::updateHaxiMap(char *strSIM,GPSGATEDATA gpsData)
{
	long nret = 0;
	char strTmp[512]="";
	
	pair<RUIX_MAP::iterator, bool> inserted;
	
	
	EnterCriticalSection(&m_mapLock);
	m_gpsRuiXMap.erase(strSIM);
	inserted = m_gpsRuiXMap.insert(RUIX_MAP::value_type(strSIM,gpsData));
	if(inserted.second==true) sprintf(strTmp,"updateHaxiMap-哈希表中成功插入SIM信息-%s",strSIM);
	else sprintf(strTmp,"updateHaxiMap-哈希表中插入SIM信息失败-%s",strSIM);
	//TODO: NEEDLOG
	//m_pGps->wlog("Console",strTmp);
	LeaveCriticalSection(&m_mapLock);
	return nret;
}

//BOOL getIPPort(char *strSIM,GPSGATEDATA gpsData);
BOOL Protocal:: getIPPort(char *strSIM,GPSGATEDATA &gpsData)
{
	BOOL bret = FALSE;
	char strTmp[100]="";
	char strFilname[20]="DevxGps.cpp";
	
	RUIX_MAP::iterator findItem;
	
	
	EnterCriticalSection(&m_mapLock);
	//m_gpsRuiXMap.

 	findItem = m_gpsRuiXMap.find(strSIM);
	if(findItem==m_gpsRuiXMap.end())
	{
		sprintf(strTmp,"getIPPort-在IP映射表中没有找到SIM的地址信息-SIM:%s",strSIM);
		//TODO: NEEDLOG
		//	m_pGps->wlog(m_strConsole,strTmp);
		Write_Log(strTmp);
		bret= FALSE;
	}
	else
	{
		gpsData =  findItem->second;
		sprintf(strTmp,"getIPPort-在IP映射表中找到信息-%s",strSIM);
		//TODO: NEEDLOG
		//	m_pGps->wlog(m_strConsole,strTmp);
		bret = TRUE;
	}
	LeaveCriticalSection(&m_mapLock);	
	
	return bret;	
}



BOOL Protocal::writeDataBase( GPSINFO gpsInfo )
{
	return writedb(&gpsInfo);
}

//long Protocal::startProcess_GPSData()
//{
//	const int nDataBufMaxSize=1024;
//	char *pDataBuf = new char[nDataBufMaxSize];
//
//	long nret = 0;
//	char strTmp[512]="";
//	char strFilename[20]="";
//	GPSINFO gpsInfo;
//	GPSGATEDATA  gpsData;
//
//	strcpy(strFilename,"Console");
//	memset(&gpsInfo,0x00,sizeof(GPSINFO));
//	memset(&gpsData,0x00,sizeof(GPSGATEDATA));
//	gpsData.pDatabuf = pDataBuf;
//	BOOL bExit=FALSE;
//
//	while(!bExit)
//	{
//		long nLenGpsData = readGPS(gpsData);
//		if(nLenGpsData<1)
//		{
//			returnStar(0,"",strFilename);
//			Sleep(50);
//			continue;
//		}
//
//		sprintf(strTmp,"[GPS]→[Console]-nDataLen:%d",gpsData.nDataLen);
//		m_pGps->wlog(strFilename,gpsData.pDatabuf,gpsData.nDataLen);
//
//		long nRet = 0;
//		nRet = SynchronGPSData(pDataBuf,gpsData.nDataLen,gpsInfo);
//		if(nRet<1)
//		{
//			returnStar(2,"startGPS-来自GPS报文队列的报文不符合报文的规范",strFilename);
//			continue;
//		}
//		nRet = updateHaxiMap(gpsInfo.COMMADDR,gpsData);
//
//		nRet = writeDataBase(gpsInfo);
//		if(nRet <0)
//		{
//			m_pGps->wlog(gpsInfo.COMMADDR,"[Protocal]→[DB]-Fail to write DB");
//			continue;
//		}
//
//		nRet = m_pGps->getResMsg(pDataBuf,gpsInfo);
//		if(nRet >0)
//		{
//			nRet = writeGPSx(gpsData);
//			if(nRet<1)
//			{
//				sprintf(strTmp,"[Console]→[GPS]%s-失败",gpsData.pDatabuf);			
//			}
//			else 
//				sprintf(strTmp,"[Console]→[GPS]-%s",gpsData.pDatabuf);
//			m_pGps->wlog(gpsInfo.COMMADDR,strTmp);
//		}
//		sprintf(strTmp,"starGps-当前报文不需要响应--SIM:%s",gpsInfo.COMMADDR);
//		
//
//	}
//	return 0;
//
//}
long Protocal::startGPS( char *buf,int nlen,char *pResBuf,int nResBufLen )
{
	long nret = 0;
	GPSGATEDATA  gpsData;	
	memset(&gpsData,0x00,sizeof(GPSGATEDATA));	
	gpsData.pDatabuf = buf;
	gpsData.nDataLen = nlen;
	int ntotalBufLen = 0;

	if((ntotalBufLen = readGPS(gpsData,buf)) <1) 
	{
		return -500;
	//	return returnStar(0,"",strFilename);
	}
	_time64((__time64_t*)g_pi64LastDataTime);
	int iTimeCount=0;
	char *pEndBuf = buf+ntotalBufLen;

	char *pDataBuf =buf;
	do{
		if(pDataBuf>=pEndBuf)
			break;
		if(gpsData.nDataLen<3)
			break;
		int nDataLen =0;
		if(doGpsData(pDataBuf,gpsData,nDataLen,iTimeCount,pResBuf,nResBufLen)==-2002)
			break;

		gpsData.nDataLen =gpsData.nDataLen-nDataLen;
		pDataBuf = pDataBuf+nDataLen;

		iTimeCount++;
	}while(1);
	return 1;
}
int Protocal::doGpsData( char *buf,GPSGATEDATA gpsData,int &nDataLen,int iTimeCount,char *pResBuf,int nResBufLen )
{
// 	strcpy(buf,"*HQ,6120108162,V1,044541,A,2624.7708,N,10317.3470,E,0.00,354,270312,FFFFFBFF#");
// 	sprintf(strTmp,"[GPS]→[Console]-nDataLen:%d",gpsData.nDataLen);
// 	m_pGps->wlog(strFilename,gpsData.pDatabuf,gpsData.nDataLen);
	//2461203243550703540304132504827200102438156e000123fffffbffff00c62461203243550704240304132504814400102438424e000079fffffbffff00c72461203243550704590304132504805800102438584e000136fffffbffff00c8
	int nret=0;
	GPSINFO gpsInfo;

	char strTmp[1024]="";
	int nLen_StrTmp=1024;
	memset(&gpsInfo,0x00,sizeof(GPSINFO));

	gpsInfo.bNeedWriteDataBase=true;
	GPSClass *pCurGPSClass=NULL;
	gpsData.pCurGPSClass =NULL;
	nDataLen =0;
	if((nret = SynchronGPSData(&pCurGPSClass,buf,gpsData.nDataLen,gpsInfo) )<0) 
	{
		if(iTimeCount==0)
		{
			Write_Log("startGPS-来自GPS报文队列的报文不符合报文的规范");		
			buf2HexStr_devx(buf,gpsData.nDataLen,strTmp,nLen_StrTmp);
			Write_Log(strTmp);
		}
		return -2002;
	//	return returnStar(2,"startGPS-来自GPS报文队列的报文不符合报文的规范",strFilename);
	}
	(*g_dwMapCount)++;
	//nret 就是实际使用数据长度
	nDataLen = nret;
	gpsData.pCurGPSClass = pCurGPSClass;
	//根据报文类型来决定下面的操作
	if(gpsInfo.nMsgID == MSG_LOGIN)
	{
		/*if(nret)*/ updateHaxiMap(gpsInfo.COMMADDR,gpsData);
	}
	else if(gpsInfo.nMsgID == MSG_TERMINALFEEDBACK)
	{
		sprintf(strTmp,"v4-->%s->%s",gpsInfo.CMDID,gpsInfo.CMDARGUS);
		//TODO: NEEDLOG
		//	m_pGps->wlog(gpsInfo.COMMADDR,strTmp);
		Write_Log(gpsInfo.COMMADDR,strTmp);
	}

	buf2HexStr_devx(buf,gpsData.nDataLen,strTmp,nLen_StrTmp);
	Write_Log("ts",strTmp);
	if(gpsInfo.nMsgID== MSG_NULL)	
	{
		buf2HexStr_devx(buf,gpsData.nDataLen,strTmp,nLen_StrTmp);
		Write_Log("msgNULL",strTmp);
	}
	else
	{
		//normal数据
		if(gpsInfo.bNeedWriteDataBase)
		{
			int nRet = writeDataBase(gpsInfo);
			if(nRet<1) 
			{
				sprintf(strTmp,"[Protocal]→[DB]-Fail to write DB.ret=%d,commandr=%s",nRet,gpsInfo.COMMADDR);
				//TODO: NEEDLOG
				//		m_pGps->wlog(gpsInfo.COMMADDR,strTmp);	

				Write_Log(strTmp);
				Write_Log(gpsInfo.COMMADDR,strTmp);		
				buf2HexStr_devx(buf,gpsData.nDataLen,strTmp,nLen_StrTmp);
				Write_Log(gpsInfo.COMMADDR,strTmp);
			}
			//else m_pGps->wlog(gpsInfo.COMMADDR,"[Protocal]→[DB]-success to write DB");
		}

		int nLen = pCurGPSClass->getResMsg(pResBuf,gpsInfo);
		if(nLen>0)
		{
			//gpsData.nDataLen = nLen;
			int nRet =writeGPSx(gpsData,pResBuf,nLen); 
			if(nRet<1)
			{
				sprintf(strTmp,"ret [Console]→[GPS]%s-失败.%x",gpsData.pDatabuf,nRet);			
			}
			else sprintf(strTmp,"ret [Console]→[GPS]-%s",gpsData.pDatabuf);			
			//TODO: NEEDLOG
			//		m_pGps->wlog(gpsInfo.COMMADDR,strTmp);
			Write_Log(gpsInfo.COMMADDR,strTmp);

			buf2HexStr_devx(buf,gpsData.nDataLen,strTmp,nLen_StrTmp);
			Write_Log(gpsInfo.COMMADDR,strTmp);

			buf2HexStr_devx(pResBuf,nLen,strTmp,nLen_StrTmp);
			Write_Log(gpsInfo.COMMADDR,strTmp);
		}
		else{
			sprintf(strTmp,"starGps-当前报文不需要响应--SIM:%s",gpsInfo.COMMADDR);
			//Write_Log(gpsInfo.COMMADDR,strTmp);
		}

	}
	return nret;	
}

long Protocal::start()
{
	return 0;
}

long Protocal::Process_Command(GPSCommand *gpsCommand,char *pGpsDataBuf)
{
	///从GPSGateAdpter读取命令
	//把命令（xml格式）放到GPS设备中解析，翻译成buffer
	//把buffer 通过socket 发送给 对应的gps
	
	int nRet = getCmd(gpsCommand);
	if(nRet < 1)
		return nRet;
	char strTmp[100];
	do{
		if(gpsCommand->commandType < cmdType_ToService)
		{
			GPSGATEDATA gpsData;

			//通过deviceID 获取 设备当前的 socket信息
			int iCnt = 0;
			BOOL bGetIPPort  = false;
			while(!bGetIPPort)
			{
				bGetIPPort = getIPPort(gpsCommand->strDevID,gpsData);
				if(iCnt++ > 5)
					break;
			}
			iCnt=0;
			while(!bGetIPPort)
			{
				bGetIPPort = getIPPort(gpsCommand->strSim,gpsData);
				if(iCnt++ > 5)
					break;
			}
			if(!bGetIPPort)
			{
				//长时间没有获取到对应sim 的ip，port，直接丢弃
				//这里把命令还回数据链去，下次再使用
				nRet = -2;
				break;
			}
			gpsData.pDatabuf = pGpsDataBuf;
			
			if(gpsCommand->nLenCommandLine <1)			
			{
				//从对应gps 设备中 翻译命令
				GPSClass *pCurGPSClass=(GPSClass*)gpsData.pCurGPSClass;
				nRet = pCurGPSClass->getConsole2GPSData(gpsCommand->pstrCommandXMl,gpsCommand);
				if(nRet<1)
				{
					nRet-=1000;
					break;
				}
				//gpsCommand->pstrCommandXMl;
				//gpsCommand->nLenCommandXML = nRet;
			}
			{
				memcpy(gpsData.pDatabuf,gpsCommand->strCommandLine,gpsCommand->nLenCommandLine);
				gpsData.nDataLen = gpsCommand->nLenCommandLine;
			}

			if(gpsCommand->commandType <cmdType_ToService)
			{
				nRet = writeGPSx(gpsData,gpsData.pDatabuf,gpsData.nDataLen);	
				if(nRet<1)
				{
					sprintf(strTmp,"[Console]→[GPS]%s-失败",gpsData.pDatabuf);			
				}
				else 
					sprintf(strTmp,"[Console]→[GPS]-%s",gpsData.pDatabuf);
			}
		}
		// 由于需要把收到的命令 放到数据库中，所以还是要调用WriteCommand
		//此命令是发给GPS网关自身的
		nRet= WriteCommand(gpsCommand);

	}while(0);

 	char strLog[100];
 	sprintf(strLog,"cmd:%d,ret:%d--",gpsCommand->commandType,nRet);
	//TODO: NEEDLOG
	//	m_pGps->wlog(m_strConsole,strLog);
	Write_Log(strLog);

	return nRet;
}
//GPSGATEDATA gpsData
/*
*	发送给gps 数据用到的结构体成员
	pGpsGateData->curSocketInfo.tcpSocketHandle;//在map中已经有值
	pGpsGateData->oldSocketInfo.tcpSocketHandle;//在map中已经有值
	pGpsGateData->pDatabuf,//需要单独构造
	pGpsGateData->nDataLen//需要单独构造
*/
long Protocal::writeGPSx(GPSGATEDATA gpsData,char *buf,int nLen)
{
	long nret = 0;
	
	//	nret = writeGPS(buf,addr,port);
	nret = writeGPS(&gpsData,buf,nLen);
	
	return nret;
}