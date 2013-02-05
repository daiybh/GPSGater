#ifndef GPS_GATE_DATA_DEF_H
#define GPS_GATE_DATA_DEF_H


typedef struct
{
	char socketType;//0: 此结构体为无效数据，1：tcp数据 2：udp数据

	SOCKADDR_IN udpSockAddr;

	SOCKET tcpSocketHandle;

	BYTE   reserve[128];//预留128，以后扩展使用。

}SOCKETINFO;

typedef  struct
{

	char *pDatabuf;
	int   nDataLen;

	SOCKETINFO curSocketInfo;

	SOCKETINFO oldSocketInfo;

}GPSGATEDATA;




#endif

