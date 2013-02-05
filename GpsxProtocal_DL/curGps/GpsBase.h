/********************************************************************
	created:	2012/01/19
	created:	19:1:2012   15:35
	filename: 	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps\GpsBase.h
	file path:	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps
	file base:	GpsBase
	file ext:	h
	author:		areslion@qq.com
	
	purpose:	
*********************************************************************/
#ifndef GPSBASE_H
#define GPSBASE_H

#include "GpsData.h"

#define _T(x)  (x)

//virtual long getGpsInfo(char *buf,GPSINFO &gpsInfo)
//Return
enum{E_MSG_DISMACH=1,
E_MSG_UNKOWN};

#include "I_XMLParser.h"
class GPSClass
{
public:	
	GPSClass();	
	/*
	 *	把gps 上来的数据（buf)翻译到 struct gpsInfo 中
	 */
	virtual long getGpsInfo(char *buf,GPSINFO &gpsInfo)=0;
	virtual BOOL getResMsg(char *strBuf,GPSINFO &gpsInfo)=0;
	/*
	 *	把想要发送给gps 的数据翻译成gps 能识别的结构
	 * fromConsole_srcBuf 放的是 xml命令 需要xml 解析出来
	 * toGpsData_destBuf  目标buffer
	 * @return   目标buffer的大小
	 */
	virtual long getConsole2GPSData(const char *fromConsole_srcBuf,GPSCommand *pGpsCommand);
private:	
	void wlog(char *buf,int nLen,GPSINFO &gpsInfo,BOOL bToGps=TRUE);
	void wlog(char *logName,char *strlog);
	void wlog(char *logName,char *strlog,int nlen);
protected:
	virtual void getMsgID(char *buf,GPSINFO &gpsInfo)=0;
	const char * find_Comma(const char *srcStr,int &nLen);
	const char * getStr_betweenComma(const char *pBuf,char *destStr);
	void getCheckCode(char *strBuf,char *strCheckCode);
	int getCheckCode(char *pstrBuf,int nStrLen,char *strCheckCode);
	struct tm *GetLocalTime();
private:
	//处理命令相关-begin
	long handleCmd_SetArea(GPSCommand *gpsCommand);
	long handleCmd_overspeed(GPSCommand*pGpsCommand);
	long handleCmd_CacelArea(GPSCommand*pGpsCommand);
	long handleCmd_SetHeartTime(GPSCommand *pGpsCommand);
	long handleCmd_SetTimingTrack(GPSCommand*pGpsCommand);
	long handleCmd_SetTurnReport(GPSCommand*pGpsCommand);
	long handleCmd_SetGPRSParam(GPSCommand*pGpsCommand);
	long handleCmd_Set_BackGPRSServer(GPSCommand*pGpsCommand);
	long handleCmd_Get_ErrorCode(GPSCommand*pGpsCommand);
	long handleCmd_Set_VehicleType(GPSCommand*pGpsCommand);
	long handleCmd_Get_VehicleType(GPSCommand*pGpsCommand);
	long handleCmd_Set_OpenOBD(GPSCommand*pGpsCommand);
	long handleCmd_Set_OBDParam(GPSCommand*pGpsCommand);
	long handleCmd_Set_Oil_LowAlarm(GPSCommand*pGpsCommand);
	long handleCmd_Get_Device_Version_and_SN(GPSCommand*pGpsCommand);
	long handleCmd_Set_Reset_Mileage_and_Runtime(GPSCommand*pGpsCommand);


	///////////////
protected:
	//虚函数 需要各个派生者实现
	virtual long _handleCmd_overspeed(GPSCommand*pGpsCommand,int nMaxSpeed,int nMinSpeed,int nContinue,int nValid=1)=0;
	virtual long _handleCmd_SetArea(GPSCommand*pGpsCommand,TCHAR *pAreaID,TCHAR *palertType,TCHAR *pType,TCHAR *pLeftLat,TCHAR *prightlat,TCHAR *pleftlng,TCHAR *prightlng,TCHAR *pcenterlat,TCHAR *pcenterlng,TCHAR *pRadius)=0;
	virtual long _handleCmd_CacelArea(GPSCommand*pGpsCommand,TCHAR *pAreaID)=0;
	virtual long _handleCmd_SetHeartTime(GPSCommand*pGpsCommand,TCHAR *pInterval)=0;
	virtual long _handleCmd_SetTimingTrack(GPSCommand*pGpsCommand,TCHAR *pInterval,TCHAR*pReportCount)=0;
	virtual long _handleCmd_SetTurnReport(GPSCommand*pGpsCommand,TCHAR*pAngle)=0;
	virtual long _handleCmd_SetGPRSParam(GPSCommand*pGpsCommand,TCHAR*pLinkMod,TCHAR*pIP,TCHAR*pPort,TCHAR*pAPN,TCHAR*pAPN_UserName,TCHAR*pAPN_PassWord)=0;
	virtual long _handleCmd_Set_BackGPRSServer(GPSCommand*pGpsCommand,TCHAR*pIP,TCHAR*pPort)=0;
	virtual long _handleCmd_Get_ErrorCode(GPSCommand*pGpsCommand)=0;
	virtual long _handleCmd_Set_VehicleType(GPSCommand*pGpsCommand,TCHAR*pVehicleType)=0;
	virtual long _handleCmd_Get_VehicleType(GPSCommand*pGpsCommand)=0;
	virtual long _handleCmd_Set_OpenOBD(GPSCommand*pGpsCommand,TCHAR*pFlag)=0;
	virtual long _handleCmd_Set_OBDParam(GPSCommand*pGpsCommand,int nParamCount,void *pstArrParams)=0;
	virtual long _handleCmd_Set_Oil_LowAlarm(GPSCommand*pGpsCommand,TCHAR*poilvalue)=0;
	virtual long _handleCmd_Get_Device_Version_and_SN(GPSCommand*pGpsCommand,TCHAR*psetType)=0;
	virtual long _handleCmd_Set_Reset_Mileage_and_Runtime(GPSCommand*pGpsCommand)=0;
	//处理命令相关-end
protected:
	char	m_strConsole[64];
	char	m_strProtocal[64];
	char	m_strInterval[5];

	I_XMLParser		* m_pXmlParser;
};

#endif