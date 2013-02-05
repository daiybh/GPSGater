#pragma once

#ifdef I_GPS_GATER_MAN_EXPORTS
#define I_GPSGATERMAN_API __declspec(dllexport)
#else
#define I_GPSGATERMAN_API __declspec(dllimport)

#endif

//以下为TSDevice管理者的基类定义
class I_GPSGATERMAN_API I_GPSGaterMan
{
public:
	BOOL static CreateInstance(I_GPSGaterMan **ppGPSGaterMan);
	/*
	 *	初始化
	 */
	virtual BOOL InitGPSGaterMan()=0;
	/*
	 *	反初始化
	 */
	virtual BOOL UnInitGPSGaterMan()=0;
	/*
	 *	开始工作
	 */
	virtual BOOL StartWork()=0;
	/*
	 * 停止工作
	*/
	virtual BOOL StopWork()=0;
};
