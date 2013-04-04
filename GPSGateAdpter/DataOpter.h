#pragma once
#include "GpsData.h"
#include "OracleOCI_o.h"
#include "TimeCount.h"
class DataOpter
{
public:
	DataOpter(void);
	~DataOpter(void);
	int Init();
	int StartWork();

	 int writedb(const GPSINFO *pGpsInfo);
	 int writeCommand(GPSCommand * pGpsCommand);


	 int StopWork();

	 int initTCP();

private:
	COracleOCI_o *m_pOracleOCI;
	CTimeCount m_timeCount;
};
