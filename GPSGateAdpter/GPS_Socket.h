#pragma once

#include "MGBufferListEx.h"
#include "ITsDevice.h"
#include "GpsGateDatadef.h"
class CGPS_Socket
{
public:
	CGPS_Socket(void);
	~CGPS_Socket(void);
	int StartWork();

	long getGPS(char *buf,char *addr,int &nPort);
	long getGPS( GPSGATEDATA *pGpsData);

	long writeGPS(const char *buf,const char *addr,int nPort);

	long writeGPS(const GPSGATEDATA * pGpsData);


	static UINT __stdcall StartCaptureThread(void* pParam);
	void CaptureOnThread();
private:
	ITsDevice_Card * m_pITSDevice_card;

	MGBufferListEx *m_pDataList;

};
