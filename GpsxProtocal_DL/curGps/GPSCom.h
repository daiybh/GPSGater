/********************************************************************
	created:	2012/01/18
	created:	18:1:2012   14:19
	filename: 	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps\GPSCom.h
	file path:	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps
	file base:	GPSCom
	file ext:	h
	author:		areslion@qq.com
	
	purpose:	
*********************************************************************/
#ifndef GPSCOM_H
#define GPSCOM_H



#include "DevxGps.h"

//#define EXPORTCOM

//#ifdef EXPORTCOM
#define DLLCOM	extern "C" __declspec(dllexport)
// #else 
// #define DLLCOM	extern "C" __declspec(dllimport)
// #endif


DLLCOM void Test();

#endif