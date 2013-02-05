#pragma once
#include "PC_TSStructDefine.h"
#include <vector>
using namespace std;

#ifdef PC_TSSTREAMBASE_EXPORTS
#define PC_TSSTREAMBASE_API __declspec(dllexport)
#else
#define PC_TSSTREAMBASE_API __declspec(dllimport)
#pragma  comment(lib,"TSStreamProcess.lib")
#endif

struct PC_THREADDATA;
struct TSDATA;
struct Transport_Packet;

class TSStreamProcessMan;
class MGBufferListEx;
class IStreamProcess;
class PC_TSSTREAMBASE_API CPC_TSStreamProcess_Interface
{
public:
	CPC_TSStreamProcess_Interface();
	virtual		~CPC_TSStreamProcess_Interface();
public:
	int			Initialize(int nTSDataSize = 2000000);
	int			SetBufferList(MGBufferListEx *pDataList);
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
	virtual int			Relocate(LPCTSTR lpBindIp,int iBindPort,LPCTSTR lpMulticastIp,int nPgmID,int nIPType);
	int			StartStreaming();
	int			SetStreamInfoNotifyCB(PC_TSStreamInfoNotifyCB pFunc, void* pUserData);
	int			SetPostESBufferCB(int nPID, PC_PostESDataCB2 pFunc, void* pUserData);	
	int			GetStreamInfo(TS_StreamInfo* pInfo);
	int			GetStreamBitrate(double& dBitrate);
	int			StopStreaming();
	int			ClearPoolData();
	int			GetStreamInfoNotifyCB(void** pFunc, void** pUserData);
	//static		int PSIInfoNotifyCB(TS_StreamInfo* pInfo, int nState, void* pParam);	
	void		ReStartStreaming();
	void		SetChannelName(LPCTSTR lpChannelName);
private:
	IStreamProcess *m_TsStreamProcess;
};