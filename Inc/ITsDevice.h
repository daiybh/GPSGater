#pragma once

#ifdef I_TSDEVICEMANAGER_EXPORTS
#define I_TSDEVICEMANAGER_API __declspec(dllexport)
#else
#define I_TSDEVICEMANAGER_API __declspec(dllimport)
#endif
#include "GpsGateDatadef.h"
class MGBufferListEx;
//以下为TSDevice管理者的基类定义
class I_TSDEVICEMANAGER_API ITSDeviceManager
{
public:
	BOOL static CreateInstance(ITSDeviceManager **ppTSDeviceManager);
	/*
	 *	初始化
	 */
	virtual BOOL InitDeviceMan()=0;

	virtual void SetBufferList(MGBufferListEx *pBufferList) = 0;
	/*
	 *	反初始化
	 */
	virtual BOOL UnInitDeviceMan()=0;
	/*
	 *	开始工作
	 */
	virtual BOOL StartWork()=0;
	/*
	 * 停止工作
	*/
	virtual BOOL StopWork()=0;

	virtual int GetBindPort()=0;
	virtual CString GetChannelName()=0;
	/*
	 *	重置某个通道的IP
	 */
	virtual int Relocate(LPCTSTR lpBindIp,int iBindPort,LPCTSTR lpMulticastIp,int nIPType)=0;//nIPType=0:UDP;nIPType=1:RTP
};

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------

//以下为TSDevice的基类定义
struct TsOnIpChannelInfo
{
	CString			m_sBindIp;
	int				m_nBindPort;
	CString			m_sMulticastIp;

	CString			m_sProtol;
	BOOL			m_bSSRC;
	int				m_nSSRC;

	TsOnIpChannelInfo()
	{
		m_sBindIp = _T("127.0.0.1");
		m_nBindPort = 0;
		m_sMulticastIp = _T("0.0.0.0");

		m_sProtol = _T("RTP");
		m_bSSRC = FALSE;
		m_nSSRC = 0;
	}
};

enum DeviceTYPE
{
	ASI_DEVICE	= 1,
	IP_DEVICE	= 2
};

struct tag_DeviceInfo
{
	DeviceTYPE DeviceType;//设备类型
	int nAnalyzeType;//分析通道的类型
	int nDeviceIdx;//设备id
	int nDeviceTypeID;// 同一类型设备的ID，在系统“设备管理器”中的排列位置，从0开始
	int nChannelCount;//此设备具有的通道个数
};

#ifdef _ITSDEVICE_EXPORT_
#define TSDEVICE_EXPORT __declspec(dllexport)
#else
#define TSDEVICE_EXPORT __declspec(dllimport)
#endif

class ITsDevice_Card;

#ifdef __cplusplus
extern "C" 
{
#endif

	void TSDEVICE_EXPORT TSDevice_GetPlugInObject(ITsDevice_Card** ppITsDevice_Card); //获取插件对象 ,在使用完对象之后，必须相应的必须调用ReleasePluginObject函数，释放对象
	void TSDEVICE_EXPORT TSDevice_ReleasePlugInObject(ITsDevice_Card* pITsDevice_Card);//释放插件对象
	DeviceTYPE  TSDEVICE_EXPORT TSDevice_PluginID();//获取插件ID

#ifdef  __cplusplus
}
#endif

static const int TS_BUFFER_SIZE = 1024*1024*2;

class TSDEVICE_EXPORT ITsDevice_Card
{
public:
	static int CreateInstance(ITsDevice_Card **pCard);
	/*
	 *	初始化硬件设备
	 * @param nCardID  板卡号（在系统“设备管理器”中的排列位置，从0开始）
	 */
	virtual BOOL	InitDevice(int nListenPort) = 0;
	virtual BOOL	ReleaseDevice() = 0;

	/*
	 *	开始输入数据
	 */
	virtual int		StartInput() = 0;
	/*
	 *	停止输出数据
	 */
	virtual int		StopInput() = 0;

	virtual int sendData(const char *pData,int nDataSize,const char *pAddr,int nPort)=0;
	virtual int sendData(const GPSGATEDATA *pGpsGateData)=0;
	virtual void SetMemShareBuffer(MGBufferListEx *pBufferList,DWORD dwBufferSize) = 0;

};

//收集插件函数
//插件函数
typedef  void (*GETPLUGINOBJECT)(ITsDevice_Card**);
typedef  void (*RELEASEPLUGINOBJECT)(ITsDevice_Card*);
typedef  DeviceTYPE (*PLUGINID)();
typedef struct tagPLUGIN_CELL_TSDEVICE
{
	HINSTANCE	hPlugInstance;
	DeviceTYPE  nPlugInType;
	GETPLUGINOBJECT GetPlugInObject;
	RELEASEPLUGINOBJECT ReleasePlugInObject;
}PLUGIN_CELL_TSDEVICE, *PPLUGIN_CELL_TSDEVICE;