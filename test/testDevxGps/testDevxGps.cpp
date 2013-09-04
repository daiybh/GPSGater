// testDevxGps.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "testDevxGps.h"
#include "GpsBase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;
void test(char *pbuf,int nbufLen)
{
	//int nbufLen = strlen(pbuf);
	GPSINFO gpsInfo;
	GPSClass * gpsCurDevice = GPSClass::getProtocol(pbuf,&gpsInfo);
	if(gpsCurDevice==NULL){
		printf("Error,gpsCurDevice==null\r\n");
		return;
	}
	int nDevID=0;
	char * pName = gpsCurDevice->getProtocolName(nDevID);
	printf("protocol:%s:%x\r\n",pName,nDevID);
	int nRet = gpsCurDevice->getGpsInfo(pbuf,nbufLen,gpsInfo);
	printf("getGpsInfo-->ret[%d]\r\n",nRet);

	if(gpsCurDevice->getResMsg(pbuf,gpsInfo))
	{
		printf("starGps-当前报文 响应--SIM:%s\r\n",gpsInfo.COMMADDR);
	}
	else printf("starGps-当前报文不需要响应--SIM:%s\r\n",gpsInfo.COMMADDR);
}

int getbin(int x)
{
	if(x>='0' && x<='9')
		return x-'0';

	if(x>='A' && x<='F')
		return x-'A'+10;
	return x-'a'+10;
}
int HexToBin(char *pHex,char **ppBin)
{
	int nLen = strlen(pHex);
	if(*ppBin==NULL)
	{
		*ppBin =new char[nLen];
		ZeroMemory(*ppBin,nLen);
	}
	char *pBin = *ppBin;
	/*
	printf("nlen:%d\r\n%s\r\n",strlen(pHex),pHex);
	for (int i=0;i<nLen;i++)
	{		
			printf("%d",i%10);
	}
	printf("\r\n");
	for (int i=0;i<nLen;i++)
	{		
		if(i%10 ==0 )
			printf("%d",i/10);
		else
			printf(((i%10)==0)?"0":"-");
	}
	printf("\r\n");
	*/
	int nBinLen = 0;
	for(int i=0;i<nLen;)
	{
		char p10=pHex[i];
		if(p10==' '){
			i++;
			continue;
		}
		char p00=pHex[i+1];
		*pBin++ = getbin(p10)*16+getbin(p00);
		i+=2;
		nBinLen++;
	}
	return nBinLen;
}
void testJTT808()
{
	char *pbuf=new char[1024];
	ZeroMemory(pbuf,1024);
	strcpy(pbuf,"7E 01 02 00 07 01 57 00 00 00 01 00 05 72 6F 70 65 6E 74 65 21 7E");
	printf("%s\r\n",pbuf);
	char *pbin=NULL;
	int nBinLen = HexToBin(pbuf,&pbin);
	test(pbin,nBinLen);	
}
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	int nx = 2;
	int ny = 2<<8;
	int nz = 2>>8;
	int nu = 8<<2;
	testJTT808();
	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		//_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
	}
	
	return nRetCode;
}
