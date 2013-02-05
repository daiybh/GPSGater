#ifndef I_STREAM_PROCESS_H
#define I_STREAM_PROCESS_H


#ifdef I_STREAM_PROCESS_EXPORTS
#define I_STREAM_PROCESS_API __declspec(dllexport)
#else
#define I_STREAM_PROCESS_API __declspec(dllimport)
#endif

class IStreamProcess;

#ifdef __cplusplus
extern "C" 
{
#endif

	void I_STREAM_PROCESS_API StreamProcess_GetPlugInObject(IStreamProcess** ppIStreamProcess); //获取插件对象 ,在使用完对象之后，必须相应的必须调用ReleasePluginObject函数，释放对象
	void I_STREAM_PROCESS_API StreamProcess_ReleasePlugInObject(IStreamProcess* pIStreamProcess);//释放插件对象
	int  I_STREAM_PROCESS_API StreamProcess_PluginID();//获取插件ID

#ifdef  __cplusplus
}
#endif

#include "PC_TSStructDefine.h"
#include "TSBaseDefine.h"
#include "MGBufferListEx.h"
class  IStreamProcess
{
public:
	virtual int			Initialize(int nTSDataSize = 2000000)=0;
	virtual int			SetBufferList(MGBufferListEx *pDataList)=0;
	/*
	 *	重定位IP port
	 *  在TS收录中直接返回false
	 *  在RTMP 收录中返回true 并记录下URL等信息。

	 * @param LPCTSTR lpBindIp			[TS收录] 本地IP		  ;[RTMP]  URL
	 * @param int iBindPort				[TS收录] 组播流端口   ;[RTMP]  none
	 * @param LPCTSTR lpMulticastIp     [TS收录] 组播流IP	  ;[RTMP]  StreamType;"http","rtmp","rtsp"
	 * @param int nPgmID				[TS收录] 节目ID		  ;[RTMP]
	 * @param int nIPType				[TS收录]			  ;[RTMP]
	 * @return 0：在这个模块没有做relocate  1:在这个模块做了 Relocate，其他模块不用做。
	 */
	virtual int			Relocate(LPCTSTR lpBindIp,int iBindPort,LPCTSTR lpMulticastIp,int nPgmID,int nIPType)=0;

	virtual int			StartStreaming()=0;
	virtual int			SetStreamInfoNotifyCB(PC_TSStreamInfoNotifyCB pFunc, void* pUserData)=0;
	virtual int			SetPostESBufferCB(int nPID, PC_PostESDataCB2 pFunc, void* pUserData)=0;	
	virtual int			GetStreamInfo(TS_StreamInfo* pInfo)=0;
	virtual int			GetStreamBitrate(double& dBitrate)=0;
	virtual int			StopStreaming()=0;
	virtual int			ClearPoolData()=0;
	virtual int			GetStreamInfoNotifyCB(void** pFunc, void** pUserData)=0;
	virtual void		ReStartStreaming()=0;
	virtual void		SetChannelName(LPCTSTR lpChannelName)=0;
};

//收集插件函数
//插件函数
typedef  void (*GET_STREAMPROCESS_PLUGINOBJECT)(IStreamProcess**);
typedef  void (*RELEASE_STREAMPROCESS_PLUGINOBJECT)(IStreamProcess*);
typedef  int (*STREAMPROCESS_PLUGINID)();
#endif