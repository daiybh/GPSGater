// testDevxGps.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "testDevxGps.h"
#include "GpsBase.h"
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog/logging.h"
#pragma comment(lib,"E:\\workspace\\OpenSource\\glog-0.3.3\\Release\\libglog.lib")
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
void testJTT808(char *pbuf)
{
	if(pbuf==NULL)
	{
		pbuf=new char[1024];
		ZeroMemory(pbuf,1024);
		strcpy(pbuf,"7E 01 02 00 07 01 57 00 00 00 01 00 05 72 6F 70 65 6E 74 65 21 7E");
		strcpy(pbuf,"7E 01 02 00 07 01 57 00 00 00 01 00 06 72 6F 70 65 6E 74 65 22 7E");
		strcpy(pbuf,"7e 01 02 00 07 01 57 00 00 00 01 0d ad 72 6f 70 65 6e 74 65 84 7e");
		strcpy(pbuf,"7e01000023015728572436000e00111111373034303444522d3230413120526f706530303101d4c14147323031310000517e");
		strcpy(pbuf,"7e 0200 0026 015728572436 0172 00000000 00000003 01614058 06c2e200 00e4 0000 0000 130906100802 01 04 0000 0000 0302 0000 d7 7e");
		strcpy(pbuf,"7e8001000501572857243600010001010200bc7e0000");
		strcpy(pbuf,"7e0200002201479747688102940000000000000001a71bdfe0a71bdfe0006f00000000130331204253010400000aa9117e");
		strcpy(pbuf,"7e0200 0022 014797476881 107d 010000000000000003017d0228cc061f25de005c0000008c130918162838010400000aa93a7e");
		strcpy(pbuf,"7e0200 0022 014787807837 0392 0000000000000302017d01c2eb061fe92400590000005f13092312064601040000000a5d7e");
		//strcpy(pbuf,"")
	}
	printf("%s\r\n",pbuf);
	char *pbin=NULL;
	int nBinLen = HexToBin(pbuf,&pbin);
	test(pbin,nBinLen);	
}
void xor2(){
	char x[300];
	ZeroMemory(x,300);
	strcpy(x,"800100050157000000010007010200");

	int xorv=0;
	for(int i=0;i<strlen(x);i++){
		xorv =xorv^(x[i]-0x30);
	}
	printf("%x\r\n",xorv);
}
void xor(){
	xor2();
	char x[20];
	ZeroMemory(x,20);
	int i =0;
	//8001 0005 015700000001 0007 0007800100     52
	//8001 0005 015700000001 0124 010200
	//8001 0005 015700000001 0007 010200 d7
	x[i++] = 0x80;
	x[i++] = 0x01;
	x[i++] =  0x00;
	x[i++] = 0x05;

	x[i++] =  0x01;
	x[i++] = 0x57;
	x[i++] = 0x00;
	x[i++] = 0x00;
	x[i++] = 0x00;
	x[i++] = 0x01;

	x[i++] = 0x00;
	x[i++] = 0x07;

	x[i++] = 0x01;
	x[i++] = 0x02;
	x[i++] = 0x00;
	char *ppBuf=new char[200];
	strcpy(ppBuf,"02 22 01 47 87 47 77 94 13 03 01 7e 49 9c 06 1f 04 a0 55 51 13 09 18 12 18 12 01 04 1b");
	//strcpy(ppBuf,"02 22 01 47 87 47 77 94 13 03 01 7e 49 9c 06 1f 04 a0 55 51 13 09 18 12 18 12 01 04 1b");
	strcpy(ppBuf,"0200002201478747779400130000000000000003017d02499c061f04a000550000005113091812181201040000001b");
	char *pBin = NULL;
	int nRet = HexToBin(ppBuf,&pBin);
	int xorv=0;
	for(int y=0;y<nRet;y++){
		xorv =xorv^pBin[y];
	}
	BYTE B[10];
	*(B) = 0x12345;
	ZeroMemory(B,10);
	int yyy = sizeof(WORD);
	memcpy(B,x,sizeof(DWORD));
	
	printf("%x--%x\r\n",xorv&0xff,(*((DWORD*)(B)) ));
}
DWORD getDword(char *pMsgBody)
{
	DWORD dRet =0;
	DWORD d1,d2,d3,d4;
	d1=d2=d3=d4 =0;
	d1 = pMsgBody[0]*256*256*256;
	d2 = pMsgBody[1]*256*256;
	d3 = pMsgBody[2]*256;
	d4 = pMsgBody[3];
	dRet = ((pMsgBody[0])*4096)|((pMsgBody[1])*256)|((pMsgBody[2])*16)|((pMsgBody[3]));
	dRet = d1|d2|d3|d4;
	return dRet;
}
void testMod(char *pSrc)
{
	char *pBin = NULL;
	int nBinLen = HexToBin(pSrc,&pBin);
	nBinLen = 1;
	DWORD dW = getDword(pBin);
}
void testMod()
{
	//00000003 01614058 06c2e200
	//status   JingDu   WeiDu
	testMod("00000003");
	testMod("01614058");
	testMod("06c2e200");
}
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
// 	FLAGS_log_dir="D:\\NMLOG2\\";
// 	google::InitGoogleLogging(argv[0]);
// 	//google::SetLogDestination(0,"info.log");
// 	//google::SetLogDestination(google::GLOG_ERROR,"error.log");
// 	google::SetLogFilenameExtension("log");
// 	google::FlushLogFiles(google::GLOG_INFO);
// 	//FLAGS_log_dir=".";
// 	LOG(INFO)<<"hello glog";
// 	LOG(ERROR)<<"ERR: glog";

	int nRetCode = 0;
	int nx = 2;
	int ny = 2<<8;
	int nz = 2>>8;
	int nu = 8<<2;
	testJTT808(argc>1?argv[1]:NULL);
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
