// GPSGateAdpter.h : GPSGateAdpter DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CGPSGateAdpterApp
// 有关此类实现的信息，请参阅 GPSGateAdpter.cpp
//
#include "GPS_Socket.h"
#include "DataOpter.h"
#include "GetCommand.h"
class CGPSGateAdpterApp : public CWinApp
{
public:
	CGPSGateAdpterApp();

// 重写
public:
	virtual BOOL InitInstance();
	void InitAdpter();

	DECLARE_MESSAGE_MAP()
private:
public:
	BOOL		  m_bInitAdpter;
	CGPS_Socket * m_pGPS_Socket;
	DataOpter   * m_pDataOpter;
	GetCommand	* m_pGetCommand;
};
