#ifndef I_GATE_INTERFACE_DLL_H
#define I_GATE_INTERFACE_DLL_H


#ifdef GATE_INTERFACE_EXPORTS
#define GATE_INTERFACE_API __declspec(dllexport)
#else
#define GATE_INTERFACE_API __declspec(dllimport)
#pragma comment(lib,"GPSGateAdpter.lib")
#endif
#include "GpsData.h"
#include "GpsGateDatadef.h"
extern "C" {
	/*
	 * 获取gps报文
	 * @param buf  报文内容
	 * @param addr udp地址
	 * @param cPort udp端口
	 * @return long buf的长度  0 为没有报文
	**/
//	GATE_INTERFACE_API long getGPS( char *buf,char *addr,char * cPort);
	GATE_INTERFACE_API long getGPS( GPSGATEDATA *pGpsData,char *buf);

	/*
	 * 往gps写报文数据
	 * @param buf  报文内容
	 * @param addr udp地址
	 * @param cport udp端口
	 * @return long   >0 发送成功  <1 发送失败
	**/
//	GATE_INTERFACE_API long writeGPS(const char *buf,const char *addr,const char * cPort);
	GATE_INTERFACE_API long writeGPS(const GPSGATEDATA * pGpsData,char *pDatabuf,int nDataLen);

	/*
	 * 报解析完毕的信息 写入数据库
	 * @param pGpsInfo
	 * @return  >0 写入成功  <1 写入失败
	**/
	GATE_INTERFACE_API int writedb(const GPSINFO *pGpsInfo);

	/*
	 * 获取应用层 往gps下发的命令
	 * @param pGpsInfo
	 * @return >0 获取成功  <1 获取失败
	**/
	GATE_INTERFACE_API int getCmd(GPSCommand *pGpsCommand);

	/*
	 * web 往gpsGater下发的设置指令
	 * @param pGpsInfo
	 * @return >0 获取成功  <1 获取失败
	**/
	GATE_INTERFACE_API int WriteCommand(GPSCommand *pGpsCommand);



};
#endif

//
/*
 long GATE_INTERFACE_API getGPS(const char *buf,const char *addr,int nPort);

/**/