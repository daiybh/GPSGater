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

#define	N_LEN	20

//GPS device type
enum{GPS_NULL,
	GPS_YOUHAO	
};




typedef struct  
{
	// 	long	nID;
	char	YH[N_LEN];
	char	CM[N_LEN];
	char	SEQ[N_LEN];
	char	COMMADDR[N_LEN];
	char	CMDID[N_LEN];
	char	CMDARGUS[N_LEN];
	char	VERFYCODE[N_LEN];
	char	CRLF[N_LEN];
}YHCM;

typedef struct  
{
	// 	long	nID;
	int		nID;
	char	YH[N_LEN];
	char	AN[N_LEN];
	char	Commaddr[N_LEN];
	char	Time[N_LEN];
	char	UTC[N_LEN];
	char	Latitude[N_LEN];
	char	Longitude[N_LEN];
	char	Altitude[N_LEN];
	char	Heading[N_LEN];
	char	Speed[N_LEN];
	char	TransactionFlag[N_LEN];
	char	OEMStatus[N_LEN];
	char	StatusString[N_LEN];
	char	VERFYCODE[N_LEN];
	char	CRLF[N_LEN];
}YHAN;

typedef struct  
{
	// 	long	nID;
	char	YH[N_LEN];
	char	AN[N_LEN];
	char	Commaddr[N_LEN];
	char	UTC[N_LEN];
	char	Latitude[N_LEN];
	char	Longitude[N_LEN];
	char	Altitude[N_LEN];
	char	Heading[N_LEN];
	char	Speed[N_LEN];
	char	TransactionFlag[N_LEN];
	char	OEMStatus[N_LEN];
	char	StatusString[N_LEN];
	char	Time[N_LEN];
	char	VERFYCODE[N_LEN];
	char	CRLF[N_LEN];
}YHAP;


typedef struct  
{
	// 	long	nID;
	char	YH[N_LEN];
	char	AK[N_LEN];
	char	Commaddr[N_LEN];
	char	VERFYCODE[N_LEN];
	char	CRLF[N_LEN];
}YHAK;


//UDP Type
enum{MSG_NULL,
//upload data
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

typedef struct 
{
	int		nDevID;
	int		nMsgID;
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
}GPSINFO;


long findstr(char *buf, char *targetstr,bool bLeft=true);
bool findStr(char *buf, char *targetstr,long &nx,long &ny);
bool fillStr(char *srcStr,char * dstStr, char *strTarget, long &nx, long &ny);
long getCRCVal( char *buf);
void getCRCPos( char *buf, long &xpos,long yPos );


#define FILLCR(a)	memcpy(a,"\r\n",2)
#define FillX(a,b)	if(fillStr(&buf[i],b,a,nx,ny)) i = ny + 1


#endif

