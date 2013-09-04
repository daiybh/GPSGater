// GpsXingRui.h: interface for the GpsXingRui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPSXINGRUI_H__AA7BA406_4507_4EAD_9994_BFE3C96BF08E__INCLUDED_)
#define AFX_GPSXINGRUI_H__AA7BA406_4507_4EAD_9994_BFE3C96BF08E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GpsBase.h"

class GpsXingRui   : public  GPSClass
{

public:
	GpsXingRui();
	virtual ~GpsXingRui();
	virtual BOOL isThisProtocol(char *buf,GPSINFO *pGpsInfo);
	virtual char* getProtocolName(int  &nDevID);

	virtual long getGpsInfo(char *buf,int nbufLen,GPSINFO &gpsInfo);
	virtual BOOL getResMsg(char *strBuf,GPSINFO &gpsInfo);	
private:
	virtual void getMsgID(char *buf,GPSINFO &gpsInfo);

	inline void convertTitude(char *strTitude,bool bPositive,bool bLongtitude,bool bVmode);
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
protected:
	

	long mapTxtInfo2( char *buf,GPSINFO &gpsInfo );
	long mapTxtInfo( char *buf,GPSINFO &gpsInfo );
	long mapHexXInfo( char *buf,GPSINFO &gpsInfo );
	long mapHexVInfo( char *buf,GPSINFO &gpsInfo );

};

#endif // !defined(AFX_GPSXINGRUI_H__AA7BA406_4507_4EAD_9994_BFE3C96BF08E__INCLUDED_)
