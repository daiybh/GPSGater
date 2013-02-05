#pragma once
#include <mmsystem.h>
#include <afxtempl.h>
#include "ITsDevice.h"

#define MAX_AUDIO_CHANNEL 8
class CaptureTsOnRtp;
class CCaptureTsOnUdp;
class  CPC_TsOnIpDevice:public ITsDevice_Card
{
public:
	CPC_TsOnIpDevice();
	virtual ~CPC_TsOnIpDevice();
	virtual BOOL	InitDevice(int nListenPort);
	virtual BOOL	ReleaseDevice();	
	virtual int		CreateInputChannel();
	virtual int		ReleaseInputChannel();
	virtual int		ReleaseAllInputChannel();
	virtual void	SetMemShareBuffer(MGBufferListEx *pBufferList,DWORD dwBufferSize);
	virtual int		StartInput();
	virtual int		StopInput();
	virtual int		GetBindPort(){return this->m_TsOnIpChannelInfo.m_nBindPort;};	
	virtual CString GetChannelName();	
	virtual BOOL	GetChannelInfo(TsOnIpChannelInfo &ChannelInfo) const;
	virtual BOOL	SetChannelInfo(const TsOnIpChannelInfo &ChannelInfo);
	virtual int sendData(const char *pData,int nDataSize,const char *pAddr,int nPort);
	virtual int sendData(const GPSGATEDATA *pGpsGateData);
protected:
	static void GetDataCb(BYTE* pBuffer, UINT nDataSize,char *pFromAddr,int nPort, void* pUserData)
	{
		CPC_TsOnIpDevice* pInfo = (CPC_TsOnIpDevice*)pUserData;
		pInfo->GetData(pBuffer, nDataSize,pFromAddr,nPort);
	};
	void GetData(BYTE* pBuffer, UINT nDataSize,char *pFromAddr,int nPort);
	void SaveConfig();
private:
	TsOnIpChannelInfo			m_TsOnIpChannelInfo;	
	CaptureTsOnRtp				*m_RtpChannel;		
	CCaptureTsOnUdp				*m_UdpChannel;
	BOOL						m_bInit;
	int							m_nBoardIdx;
	TCHAR						m_strLogName[MAX_PATH];
	MGBufferListEx				*m_pDataList;
	BOOL						m_bUseConfig;
	DWORD						m_dwPostData;
};
