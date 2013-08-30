#include "stdafx.h"
//#include "PC_TsOnIpDevice.h"
#include "SimpleDevice.h"
#include "Socket_IOCP.h"
int ITsDevice_Card::CreateInstance(ITsDevice_Card **pCard)
{
	if(pCard==NULL)
		return 0;
	bool pcIP=false;
	if(pcIP){
	//	*pCard = (ITsDevice_Card*)new CPC_TsOnIpDevice(); 
	}else{
		//*pCard = (ITsDevice_Card *)new SimpleDevice();
		*pCard = (ITsDevice_Card *)new Socket_IOCP();
	}
	if(*pCard)
		return 0;
	return 1;
}