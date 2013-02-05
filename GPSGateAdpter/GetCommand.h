#pragma once
/*
 *	监听网络端口，用来获取控制命令
 */
#include "MGBufferListEx.h"
#include "ITsDevice.h"
#include "GpsData.h"
#include "I_XMLParser.h"
class GetCommand
{
public:
	GetCommand(void);
	~GetCommand(void);
	int StartWork();
	int getCmd(GPSCommand *pGpsCmd);
	int processXML( CString strCMd ,GPSCommand *pGpsCmd );
private:
	MGBufferListEx * m_pCommandList;
	ITsDevice_Card * m_pITSDevice_card;
	I_XMLParser		* m_pXmlParser;
};
