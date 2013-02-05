#pragma once

#ifdef PC_ESSTREAMPROCESS_EXPORTS
#define PC_ESSTREAMPROCESS_API __declspec(dllexport)
#else
#define PC_ESSTREAMPROCESS_API __declspec(dllimport)
#pragma  comment(lib,"ESStreamProcess.lib")
#endif

#include "PC_TSStructDefine.h"

class MGBufferListEx;
class CPC_ESStreamSync_H264;
class PC_ESSTREAMPROCESS_API CPC_ESStreamSync_Interface
{
public:
	CPC_ESStreamSync_Interface(void);
	virtual		~CPC_ESStreamSync_Interface(void);
public:
	void		SetOutBufferList(MGBufferListEx *pBufferList);
	int			SetStream(const ESINFO_H264 &stEsinfo);	
	/*
		开始干活
	 * @param nPgmID -1:预监  >0 开始任务
	 */
	int			StartProcess(int nPgmID);
	int			StopProcess();
	BOOL		IsProcess();	
	int			SetStreamInfoNotifyCB(PC_TSStreamInfoNotifyCB pFunc, void* pUserData);
	static		int ReceiveEsDataCB(int pid, int prg_id, char * buffer, int buffer_len, 
									int error_code,
									void *user_data, BOOL end,TimeInf_shit *p_time_inf, 
									void *reserved_inf, int reserved_inf_size);
private:
	CPC_ESStreamSync_H264 *m_pEsStreamSyncH264;
};

