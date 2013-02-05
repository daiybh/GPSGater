// GpsYouHao.h: interface for the GpsYouHao class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPSYOUHAO_H__DA1DC83A_3A9F_4940_BC7D_B20865DB233D__INCLUDED_)
#define AFX_GPSYOUHAO_H__DA1DC83A_3A9F_4940_BC7D_B20865DB233D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GpsBase.h"

class GpsYouHao : public  GPSClass
{
public:
	GpsYouHao();
	~GpsYouHao();

	virtual long getGpsInfo(char *buf,GPSINFO &gpsInfo);
	virtual BOOL getResMsg(char *strBuf,GPSINFO &gpsInfo);

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
	
	virtual void getMsgID(char *buf,GPSINFO &gpsInfo);
	
	void formatDownloaData(char *buf,GPSINFO &gpsInfo);
	long sychroLocationInfo( char *buf,GPSINFO &gpsInfo );	
	long sychroDownData(char *buf,GPSINFO &gpsInfo);
	long sychroTerminalFeedbackInfo( char *buf,GPSINFO &gpsInfo );	
	long sychroLogInfo( char *buf,GPSINFO &gpsInfo );
	long sychroLogOffInfo( char *buf,GPSINFO &gpsInfo );	
	long sychroAutoNavigation( char *buf,GPSINFO &gpsInfo );
	long sychroMaitainInfo( char *buf,GPSINFO &gpsInfo );
	inline  void initDownData(GPSINFO &gpsInfo);

};

#endif // !defined(AFX_GPSYOUHAO_H__DA1DC83A_3A9F_4940_BC7D_B20865DB233D__INCLUDED_)
