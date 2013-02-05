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


#define	N_LEN	20

#define DLL_TYPE extern "C" __declspec(dllimport)


DLL_TYPE long SynchronGPSData(char *pInData,GPSINFO &gpsInfo);


#endif