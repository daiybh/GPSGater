#pragma once

#include "Socket.h"
#include <WS2tcpip.h>

using namespace std;

class Udp : public Socket
{
public:

	Udp(UINT mtu = 1500);
	virtual ~Udp();

	virtual BOOL Open(string bindIp = "", int bindPort = 0, string multicastIp = "");

	virtual BOOL Connect(string connectIp, int connectPort);
	virtual int Write(const char *connectIp,int connectPort,PBYTE pBuffer, UINT16 bufferSize, UINT nTimeOut = 500000);

	virtual int  Read(BYTE* pBuffer, UINT16 bufferSize, UINT nTimeOut = 500000);
	virtual int  Read_UDP(BYTE* pBuffer, int bufferSize,char **pFromAddr,int &nFromPort, UINT nTimeOut=500000); //0.5sec

	virtual int  Write(PBYTE pBuffer, UINT16 bufferSize, UINT nTimeOut = 500000);

protected:

	BOOL SetMulticast(PCSTR ip_addr);

	BOOL	m_isConnect;
};
