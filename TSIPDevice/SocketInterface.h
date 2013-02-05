#ifndef SOCKET_INTERFACE_H
#define SOCKET_INTERFACE_H


#ifdef SOCKET_INTERFACE_EXPORTS
#define SOCKET_INTERFACE_API extern "C" __declspec(dllexport)
#else
#define SOCKET_INTERFACE_API extern "C" __declspec(dllimport)
#endif
#include "GpsData.h"

SOCKET_INTERFACE_API int startGPSWork();

SOCKET_INTERFACE_API long getGPS(char *buf,char *addr,char *cPort);

SOCKET_INTERFACE_API long writeGPS(const char *buf,const char *addr,const char * cPort); 

SOCKET_INTERFACE_API int stopGPSWork();



SOCKET_INTERFACE_API int startDBWork();

SOCKET_INTERFACE_API BOOL writedb(const GPSInfo *pGpsInfo);

SOCKET_INTERFACE_API BOOL getCmd(GPSInfo *pGpsInfo);

SOCKET_INTERFACE_API int stopDBWork();





#endif
