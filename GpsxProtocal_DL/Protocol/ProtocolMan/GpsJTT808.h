#pragma once
#include "gpsbase.h"

class GpsJTT808 :
	public GPSClass
{
public:
	GpsJTT808(void);
	~GpsJTT808(void);
	virtual BOOL isThisProtocol(char *buf,GPSINFO *pGpsInfo);
	virtual char* getProtocolName(int  &nDevID);
	/*
	 *	把gps 上来的数据（buf)翻译到 struct gpsInfo 中
	 */
	virtual long getGpsInfo(char *buf,int nbufLen,GPSINFO &gpsInfo);
	virtual BOOL getResMsg(char *strBuf,GPSINFO &gpsInfo);

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
	struct tagMsgPaket{
		WORD msgTotalPakets;
		WORD msgPaketIdx;
	};
	struct tagMsgBodyAttribute{
		tagMsgBodyAttribute(){tagMsgBodyAttribute(0);};
		tagMsgBodyAttribute(WORD wAttribute){
			this->msgBodyLen = wAttribute&0x3FF;
			this->msgDecodeType=(wAttribute>>10)&0x7;
			this->bPaket = (wAttribute>>13)&0x1;
			this->Rev=(wAttribute>>14)&3;
		};
		WORD getWordValue(){return msgBodyLen|(msgDecodeType<<10)|(bPaket<<13)|(Rev<<14);}
		int msgBodyLen:10;
		int msgDecodeType:3;
		int bPaket:1;
		int Rev:2;
	};
	struct tagMsgHead{
		WORD msgID;
		tagMsgBodyAttribute msgBodyAttribute;
		char sim[12];
		WORD msgSN;
		tagMsgPaket msgPaket;
		tagMsgHead(){
			ZeroMemory(this,sizeof(tagMsgHead));
		}
	};
	int diposMsgBody(tagMsgHead msgHead,const BYTE *pMsgBody,GPSINFO *gpsInfo);
	int getFullCmdLine(char *pDestBuf,tagMsgHead *pMsgHead,char *pstrMsgBody,int nMsgBodyLen);
	DWORD m_MsgSN;//全局消息流水号
};
