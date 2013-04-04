/********************************************************************
	created:	2011/12/13
	created:	13:12:2011   14:56
	filename: 	E:\WORKSPACE\PROJECT\DEV-GPS\CODES\DLL_DEV_GPS\CURGPS\DevxGps\DevxGps.h
	file path:	E:\WORKSPACE\PROJECT\DEV-GPS\CODES\DLL_DEV_GPS\CURGPS\DevxGps
	file base:	DevxGps
	file ext:	h
	author:		areslion@qq.com
	
	purpose:	
*********************************************************************/
#ifndef DEVX_GPS_H
#define DEVX_GPS_H

#include <stdio.h>
#pragma warning(disable:4786)
#include <string>
#include <map>   //包含头文件
using namespace std;

#include <wtypes.h>
#include "GpsData.h"
#include "GpsYouHao.h"
#include "GpsXingRui.h"
#include "GPS_MeiTrack.h"
#include "GpsBase.h"
#include "IGPSGateAdpter.h"

using namespace std;


#define	N_LEN	20

typedef struct
{
	char strIP[30];
	char strPort[20];
}COMPORT;

typedef map <string ,COMPORT > UDT_MAP;
typedef map <string ,GPSGATEDATA > RUIX_MAP;


class Protocal
{
public:
//	GPSClass	*m_pGps;
	char		m_strLogName[N_LEN];
	UDT_MAP		m_gpsIPMap;
	RUIX_MAP	m_gpsRuiXMap;
	CRITICAL_SECTION	m_mapLock;
	char		m_strConsole[20];


	Protocal();
	//////////////////////////////////////////////////////////////////////////
	long startGPS(char *buf,int nlen);
	//long startProcess_GPSData();
	long Process_Command(GPSCommand *gpsCommand,char *pGpsDataBuf);
	long startDB(char *buf, int nlen);

	long start();
	long stop();


	

protected:
private:
	GpsYouHao	m_gpsYouHao;
	GpsXingRui	m_gpsRuiXing;
	GPS_MeiTrack	m_gpsMeiTrack;
	GPSINFO		m_gpsInfo;
	int			m_nDataLen;
	

	//////////////////////////////////////////////////////////////////////////
	long readGPS(GPSGATEDATA &gpsData);
	long writeGPSx(char *buf,char *addr,char *port);
	long writeGPSx(GPSGATEDATA gpsData);
	long updateHaxiMap(char *strSIM,char *addr,char *port);
	long updateHaxiMap(char *strSIM,GPSGATEDATA gpsData);
	BOOL getIPPort(char *strSIM,char *addr,char *port);
	BOOL getIPPort(char *strSIM,GPSGATEDATA &gpsData);
	BOOL writeDataBase(GPSINFO gpsInfo);
	BOOL readDataBase(GPSINFO &gpsInfo);
	

	//////////////////////////////////////////////////////////////////////////
	//bool setCurGpsDev(char *buf,GPSINFO &gpsInfo );
	long getGpsInfo(GPSClass *pCurGPSClass, char *buf,GPSINFO &gpsInfo );
	long SynchronGPSData(GPSClass **pCurGPSClass,char *pInData,int nLenInData,GPSINFO &gpsInfo);
	GPSClass * getCurGPSDevice(char *buf,GPSINFO&gpsInfo);
	int doGpsData(char *buf,GPSGATEDATA gpsData,int &nDataLen,int iTimeCount);
};


#define DLL_TYPE extern "C" __declspec(dllexport)

DLL_TYPE long start(unsigned __int64 &iLastDataTime);
DLL_TYPE long stop();


#endif