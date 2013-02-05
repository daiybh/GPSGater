#pragma once
#include "gpsbase.h"

class GPS_MeiTrack :
	public GPSClass
{
public:
	GPS_MeiTrack(void);
	~GPS_MeiTrack(void);

	virtual long getGpsInfo(char *buf,GPSINFO &gpsInfo);
	virtual BOOL getResMsg(char *strBuf,GPSINFO &gpsInfo);
private:
	virtual void getMsgID(char *buf,GPSINFO &gpsInfo);
protected:
	virtual long _handleCmd_overspeed(GPSCommand*pGpsCommand,int nMaxSpeed,int nMinSpeed,int nContinue,int nValid=1);
	virtual long _handleCmd_SetArea(GPSCommand*pGpsCommand,TCHAR *pAreaID,TCHAR *palertType,TCHAR *pType,TCHAR *pLeftLat,TCHAR *prightlat,TCHAR *pleftlng,TCHAR *prightlng,TCHAR *pcenterlat,TCHAR *pcenterlng,TCHAR *pRadius);
	virtual long _handleCmd_CacelArea(GPSCommand*pGpsCommand,TCHAR *pAreaID);
	virtual long _handleCmd_SetHeartTime(GPSCommand*pGpsCommand,TCHAR *pInterval);
	virtual long _handleCmd_SetTimingTrack(GPSCommand*pGpsCommand,TCHAR *pInterval,TCHAR*pReportCount);
	virtual long _handleCmd_SetTurnReport(GPSCommand*pGpsCommand,TCHAR*pAngle);
	virtual long _handleCmd_SetGPRSParam(GPSCommand*pGpsCommand,TCHAR*pLinkMod,TCHAR*pIP,TCHAR*pPort,TCHAR*pAPN,TCHAR*pAPN_UserName,TCHAR*pAPN_PassWord);
	virtual long _handleCmd_Set_BackGPRSServer(GPSCommand*pGpsCommand,TCHAR*pIP,TCHAR*pPort);
	virtual long _handleCmd_Get_ErrorCode(GPSCommand*pGpsCommand);
	virtual long _handleCmd_Set_VehicleType(GPSCommand*pGpsCommand,TCHAR*pVehicleType);
	virtual long _handleCmd_Get_VehicleType(GPSCommand*pGpsCommand);
	virtual long _handleCmd_Set_OpenOBD(GPSCommand*pGpsCommand,TCHAR*pFlag);
	virtual long _handleCmd_Set_OBDParam(GPSCommand*pGpsCommand,int nParamCount,void *pstArrParams);
	virtual long _handleCmd_Set_Oil_LowAlarm(GPSCommand*pGpsCommand,TCHAR*poilvalue);
	virtual long _handleCmd_Get_Device_Version_and_SN(GPSCommand*pGpsCommand,TCHAR*psetType);
	virtual long _handleCmd_Set_Reset_Mileage_and_Runtime(GPSCommand*pGpsCommand);

private:
	void getFullCommandLine(TCHAR *pCommandLine,TCHAR *pParameters,TCHAR *pCmdCH);
};
