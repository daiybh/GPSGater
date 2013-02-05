#pragma once

#include "Connection.h"
//#include <iostream.h>
#include <stdlib.h>


#include "GpsData.h"
struct DataInfo 
{
	//这些信息 只在gps数据上来时更新
	DataInfo(){};
	DataInfo(double longitude,double latitude,DWORD time)
	{
		doubleLongitude = longitude;
		doubleLatitude = latitude;
		dLastTime = time;
	};
	double doubleLongitude;
	double doubleLatitude;
	int		nHeading;
	DWORD dLastTime;
	DWORD dwMaxSpeed;
	char	vehicheID[20]; 
};
//map中只保存当前车相同命令的最后一条。
struct CommandItem{	
	int recordID;
	int nCmdType;
	char devID[20];
	char simID[20];
	char vehicle_ID[20];
	char cmdID[20];
	char cmdParam[20];
};

#include <map>
#include <afxmt.h>

typedef enum _tagJudge_ret
{
	RET_INVALID=-1,  //位置信息无效
	RET_NO_CAR=-3,	//没有这个车	不填入数据库
	RET_SAME_LOCATION=-4,//判断 与上次的坐标是否一样，一样就不加入表
	RET_NORMAL=1,	//此数据正常，可以填入到数据库
	RET_OVERSPEED=2,	//此数据正常，但是速度超速了。
	RET_OVERAREA_OUT=3,	// 此数据正常，但是越界了，
	RET_OVERAREA_IN=4,	//此数据正常，但是进入了不该进入的区域
	RET_GPS_ANSWER=5,	//此数据为gps回答数据，速度坐标等信息不可用
	RET_PIAOYI=6,	//有这个车，但是当前位置，速度为0，属于漂移坐标 不填入数据库


}JUDGE_RET;

class COracleOCI_o
{
public:
	COracleOCI_o(void);
	~COracleOCI_o(void);
	
	int Init(char *user,char*pwd,char*serverName,BOOL bInsertAsNewVehicle,DWORD dwUpdateVehicleTime,char *serverAddr=NULL);
	int InsertData( const GPSINFO *pGpsInfo ,double doubleLongitude,double doubleLatitude,CStringA s_RecvTime ,CStringA str_CurTime,int iState);
	int InsertData(char *pInsertDataSQL);
	int CreateTable(struct tm* pCurGPSTime);
	int Updata(const GPSINFO *pGpsInfo ,double doubleLongitude,double doubleLatitude,CStringA s_RecvTime,CStringA str_CurTime,JUDGE_RET judge_ret);
	void Close();
	int getInfos_Form_VEHICLE();
	/*
 *	@return -1 位置信息为0
			-2	有这个车，但是当前位置，速度为0，属于漂移坐标 不填入数据库
			-3	没有这个车	不填入数据库
			1	此数据正常，可以填入到数据库
			2	此数据正常，但是速度超速了。
			3   此数据正常，但是越界了，
			4   此数据正常，但是进入了不该进入的区域
			5	此数据为gps回答数据，速度坐标等信息不可用

 */
	JUDGE_RET judge_GPSData(const GPSINFO* pGpsInfo,const INT64 *iSim,int nSpeed,double doubleLongitude,double doubleLatitude,DataInfo **pDI);
//	int WriteData( const GPSINFO *pGpsInfo );
	int WriteData( const GPSINFO *pGpsInfo );
	int writeCommand(GPSCommand * pGpsCommand);
	void printf_ErrorLog(LPCTSTR lpExtInfo);
	int Insert_As_NewVehicle(const char *pTid,const char*pSim);
	int Insert_OverSpeed_Data( const GPSINFO *pGpsInfo ,double doubleLongitude,double doubleLatitude,CStringA s_RecvTime ,CStringA str_CurTime,DataInfo *pDI);
	int Update_CommandState(char *pVehicleID);
	int Excute_DeleteSQL(char *pDeleteSQL);
	DataInfo * GetVehicleInfo(const char *pStrDevID);
	int Insert_Transgress_Data( const GPSINFO *pGpsInfo ,double doubleLongitude,double doubleLatitude,CStringA s_RecvTime ,CStringA str_CurTime,DataInfo *pDI );
	int _DoCreateTable(const char*createTableSQL);
	int Insert_ODBInfo_Data(const GPSINFO *pGpsInfo,CStringA str_CurTime);
	CConnection conn;
private:

	char m_strDate[8];
	char m_strCreateTableSQL[1024*2];
	char m_strInsertDataSQL[1024*2];
	char m_strUpdateDataSQL[1024*2];
	char m_strUpdateDataSQL_ForCmd[1024*2];
	char m_strDeleteDataSQL_ForCmd[1024*2];
private:
	std::map<INT64,DataInfo*> m_DataInfoMap;
	CArray<CommandItem*,CommandItem*> m_ArrayCommandItem;


	struct tm	m_tm_TableNameTime;
	time_t		m_time_t_TableNameTime;
	__int64		m_i64d_GetVehicleInfo;//从vehicle获取信息的时间 since 1970.01.01 00:00:00
	DWORD		m_dwUpdateVehicleTime;

	BOOL m_bInsert_As_NewVehicle;

	BOOL m_bInitOCI;
public:
	DWORD		m_dwLimit_MinSpeed;//作为漂移的最小速度阈值
	DWORD		m_dwLimit_MaxDistance;//漂移判断的 距离，大于这个值就认为漂移。

};
