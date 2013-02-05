#pragma once

#include "itsdevice.h"
#include "GPS_Iocp.h"
class Socket_IOCP:
	public ITsDevice_Card
{
public:
	Socket_IOCP(void);
	~Socket_IOCP(void);
	/*
	 *	初始化硬件设备
	 * @param nCardID  板卡号（在系统“设备管理器”中的排列位置，从0开始）
	 */
	virtual BOOL	InitDevice(int nListenPort) ;
	virtual BOOL	ReleaseDevice() ;

	/*
	 *	开始输入数据
	 */
	virtual int		StartInput() ;
	/*
	 *	停止输出数据
	 */
	virtual int		StopInput() ;

	virtual int sendData(const char *pData,int nDataSize,const char *pAddr,int nPort);
	virtual int sendData(const GPSGATEDATA *pGpsGateData);
	virtual void SetMemShareBuffer(MGBufferListEx *pBufferList,DWORD dwBufferSize) ;
	static void CALLBACK RecvCB(DWORD dwStatus/*状态*/,BYTE *pClient, BYTE* pBuf, DWORD dwSize, LPVOID pUserData);

private:
	GPS_Iocp * m_pGPS_Iocp;

	MGBufferListEx				*m_pDataList;
};
