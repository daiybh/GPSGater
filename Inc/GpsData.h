/********************************************************************
	created:	2012/01/19
	created:	19:1:2012   15:29
	filename: 	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps\GpsClass.h
	file path:	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps
	file base:	GpsClass
	file ext:	h
	author:		areslion@qq.com
	
	purpose:	
*********************************************************************/
#ifndef GPSCLASS_H
#define GPSCLASS_H

#include <wtypes.h>
#include <stdio.h>

#define	N_LEN	20

//GPS device type
enum{GPS_NULL,
	GPS_YOUHAO=0x0001,
	GPS_RUIXING = 0x0002,
	GPS_MEITRACK = 0X0003,
	GPS_JTT808   = 0x0004,
	GPS_DEV_MASK = 0x00FF,

	GPSID_SIM = 0x0100,
	GPSID_TID = 0x0200,
	GPSID_MASK = 0xFF00
};




//UDP Type
enum{MSG_NULL,
//upload data
//////////////////////////////////////////////////////////////////////////
//瑞星达
MSG_TXT,
MSG_HEX_V,
MSG_HEX_X,

/////////////////////////////////////////////////////     GPS→中心
MSG_RES_GPS,


/////////////////////////////////////////////////////
MSG_TOCONSOLE,//GSP到Console的消息起始点
MSG_LOGIN,
MSG_LOGOFF,
MSG_MAITAIN,
MSG_LOCATION,
MSG_TERMINALFEEDBACK,
MSG_AUTONAVIGATION,


/////////////////////////////////////////////////////     中心→GPS
MSG_RES_CONSOLE,//GPS设备响应控制中心
MSG_RES_LOGIN,//登陆成功后响应报文,向终端表明登陆成功
MSG_RES_KEEPMAITAIN,//在收到维护连接报文的时候,应该往设备发送该响应报文,否则超过3倍间隔时间之后,GPS设备重新建立GPRS来链接
MSG_RES_LOGOFF,//离线报文,收到GPS设备的离线报文之后,必须用该响应消息进行响应


/////////////////////////////////////////////////////
MSG_TOGPS,//Console到GPS设备的设置消息
MSG_CALLSTOP,//呼叫停止
MSG_CALLONETIME,//点名呼叫
MSG_CALLNTIMES,//定次呼叫
MSG_CALLTIMER,//定时呼叫
MSG_SETGPS,//设置GPS参数
MSG_SETKEEPINTERVAL,//设置链路维护间隔
MSG_UNDEF//未定义，保留

};
typedef struct tagOBDInfo 
{
	char Horizontal_positioning_accuracy[N_LEN];	//<水平定位精度>,
	char Altitude[N_LEN];	//<海拔高度>,
	char Mileage[N_LEN];//<里程数>,
	char Runing_Time[N_LEN];//<运行时间>,
	char AGPS_Info[N_LEN];//<基站信息>,
	char status_InOutTake[N_LEN];//<输入输出口状态>,
	char outputValue[N_LEN];//<模拟量输入口值>,
	char RFID[N_LEN];//<RFID 号>/<图片名>/<围栏序号>,
	char Customize_Data[N_LEN];//<定制数据>,
	char Protocol_Version[N_LEN];//<协议版本 V>

	char Engine_speed[N_LEN];//<发动机转速>,
	char Engine_load[N_LEN];//<发动机计算负荷>,
	char Engine_coolant_temperature[N_LEN];//<发动机冷却液温度>,
	char Hundred_kilometers[N_LEN];//<百公里油耗>,
	char Intake_air_temperature[N_LEN];//<进气温度>,
	char Hydraulic[N_LEN];//<油压>,
	char Atmospheric_pressure[N_LEN];//<大气压力>,
	char Intake_pressure[N_LEN];//<进气管绝对压力>,
	char Air_flow[N_LEN];//<空气流量>,
	char Throttle_Position[N_LEN];//<节气门位置>,
	char Single_mileage[N_LEN];//<单次行驶里程>,
	char Remaining_fuel[N_LEN];//<百分比剩余油量>,
	char ErrorCode[N_LEN];//<故障码及冻结帧/就绪状态>
}stOBDInfo;
typedef struct tagGPSInfo
{
	int		nDevID;
	int		nMsgID;
	DWORD   nWarnFlag;
	char	PreFix[N_LEN];//The UDP head
	char	SEQ[N_LEN];//Serial number
	char	COMMADDR[N_LEN];//SIM NO.
	char	CMDID[N_LEN];//command ID
	char	CMDARGUS[N_LEN];//commad parameters
	char	Time[N_LEN];// 报文产生时间
	char	UTC[N_LEN];//
	char	Latitude[N_LEN];//纬度，单位为毫秒 16进制ASCII字符串
	char	Longitude[N_LEN];//经度，单位为毫秒 16进制ASCII字符串
	char	Altitude[N_LEN];//海拔高度，单位为毫秒 16进制ASCII字符串
	char	Heading[N_LEN];//方向，单位度，值范围0-360 16进制ASCII字符串
	char	Speed[N_LEN];//速度	16进制ASCII字符串	A6B	为单位m/h
	char	TransactionFlag[N_LEN];//报警状态字	以","分隔的一或多个16进制ASCII字符串
	char	OEMStatus[N_LEN];//扩展状态字
	char	StatusString[N_LEN];// 终端状态描述	以","分隔的终端描述信息
	char	VERFYCODE[N_LEN];// Verify codes
	char	CRLF[N_LEN];//CR+LF
	char    comPressFlag[N_LEN];//呼叫次数
	char    callTimes[N_LEN];//呼叫次数
	char    IntervalTime[N_LEN];//呼叫时间间隔
	char    IPV4[N_LEN];//监控中心IP地址
	char	PortNum[N_LEN];// 监控中心端口号
	char	APN[N_LEN];//移动分配的APN字符串
	int     Noload;//00：空车；01：半载；10：保留；11：满载
	bool bValid;//此数据是否有效
	bool bNeedWriteDataBase;
	stOBDInfo	st_OBD_Info;
	int		nHaveOBDInfo;//obd的有效个数
	tagGPSInfo()
	{
		nHaveOBDInfo=0;
		bValid = false;
		Noload = 0;
		bNeedWriteDataBase = true;
	}
}GPSINFO;
//*XX,YYYYYYYYYY,CMD,HHMMSS,PARA1,PARA2,…#
enum CmdType{
	cmdType_ToGPS=1,
	cmdType_ToService=0xFF,
	cmdType_ToService_OverSpeed=cmdType_ToService+1
};
typedef struct{
	CmdType	commandType;
	char	strDevID[N_LEN];
	char	strSim[N_LEN];
	char	strCMDID[N_LEN*2];//控制命令ID，字符串表示
	char	*commandParameters;//控制命令的参数，以‘，’隔开
	int		nLenCmdParameters;
	char	*strCommandLine;//翻译完成后的控制命令
	int		nLenCommandLine;//翻译完成后的控制命令长度
	char	*pstrCommandXMl;//xml命令字符串
	int		nLenCommandXML;
	void	*pVoid;
}GPSCommand;
//GPSINFO.nWarnFlag
enum
{
	WAR_OVERSPEED = 0x00000001,//超速报警,如果GPSINFO.nWarnFlag & WAR_OVERSPEED !=0,则表明超速
	WAR_OUTZONE	  = 0x00000002,//出界报警
	WAR_INZONE    = 0x00000004//入界报警
};

const int GPSINFOLEN = sizeof(GPSINFO);

long findstr(char *buf, char *targetstr,bool bLeft=true);
bool findStr(char *buf, char *targetstr,long &nlen);
bool fillStr(char *srcStr,char * dstStr, char *strTarget, long &nlen);
long getCRCVal( char *buf);
void getCRCPos( char *buf, long &xpos,long yPos );


#define FILLCR(a)	memcpy(a,"\r\n",2)
#define FillX(a,b)	if(fillStr(&buf[i],b,a,nlen)) i += nlen
#define UNFUN MessageBox(NULL,"暂不支持","Warning",MB_ICONWARNING);

#endif

