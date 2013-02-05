/********************************************************************
	created:	2009/04/02
	created:	2:4:2009   16:33
	filename: 	F:\workspace\pro\NantianPR.h
	file path:	F:\workspace\pro
	file base:	NantianPR
	file ext:	h
	author:		肖正龙
	
	purpose:	
*********************************************************************/
#ifndef NATIAN_PUB_H
#define NATIAN_PUB_H

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <assert.h>


//#define	DEBUG_VER
#define	OS_WINDOWS		
// #define OS_UNIX

//#define	  SD_DEBUF_VER		FALSE

//different client
#define	SCAN_VER_TW
#define SCAN_VER_GER	TRUE

#define K1			1024
#define K2			1024*2
#define K50			1024*50
#define K100		1024*100
#define M1			1024*1024


enum{SC_CAP_SCAN=0x0000,//driver scann
	 SC_CAP_AUTOEJECT,//Eject paper after scanning
	 SC_CAP_EJT,//Eject paper
	 SC_CAP_PRSTATU,//get the PR status, conected or not
	 SC_CAP_MASK=0xFFFF};



/************************************************************************/
/*  successfull operation */
#define		RC_NULL									0//
#define		SUCCESS_OPR								0//

/************************************************************************/

/************************************************************************/
/* PRType*/
/************************************************************************/
#define		SUPPORT_NULL				1//can not get the PR type
#define		SUPPORT_PR_2E				2//PR 2e
#define		SUPPORT_PR_9				3//PR 9
#define		SUPPORT_PR_SC				4//support scan




/************************************************************************/
/* communication port type*/
#define		PT_USB				11//USB port
#define		PT_LPT				12//LPT1
#define		PT_COM1				13//com1

/************************************************************************/

/************************************************************************/
/* PRExtended.dll*/
#define		ER_INBUF_NULL				20//the inbuf from the client program is NULL
#define		ER_MALLOC_INOUBUF			21//fail to malloc the in or out buf in C++
#define		ER_MALOC_OUTLENAR			22//fail to malloc the out length buf
/************************************************************************/


/************************************************************************/
/* PRExtended.dll*/
#define		RT_SEL_NSCAN			25//support scan







/************************************************************************/
//FTP
#define		FTP_S_TSL			30//sucess to translate the file
#define		FTP_F_TSL			31//fail to translate the file
#define		FTP_F_OPNIT			32//fail to call InternetOpen
#define     FTP_F_CNT			33//fall to connect to the server 
#define     FTP_F_OPNLOCF		34//fall to open the local file
#define     FTP_F_WTITF			35//fall write data to the server
#define     FTP_S_SERIFO		36//success to get serverinfo
#define     FPT_F_OPNCFGFLE		37//fail to open the server configuration
#define		FTP_F_LADFTPDLL		38//fail to load FtpTranslt.dll
#define		FTP_F_GETFUN		39//fail to get the FtpTranslt address
#define		UP_ERROR			45//error heppend when upload pic in GH way
#define		UP_ERRORSECOND		46//error heppend Twice when upload pic in GH way 
/************************************************************************/



/************************************************************************/
/* ERROR tab for port  */
#define		ER_S_OPEN_U								50//success to open the usb port
#define		ER_F_OPEN_U								51//fail to open the usb port
#define		ER_F_WRITE_D							52//fail to write data to usb
#define		ER_F_NO_PORT							53//can not find the specified port
#define		ER_F_LOAD_PORT							54//fail to load port.dll
#define		ER_F_GET_OPEN							55//fail to get the openu process address
#define		ER_F_GET_WRITE							56//fail to get the writeu process address
#define		ER_F_GET_CLOSE							57//fail to get the closeu process address
#define		ER_F_GET_READ							58//fail to get the readu process addresss
#define		ER_F_COM_OPN							59//fail to open the com1;
#define     ER_F_COM_WPT							60//fail to write through serial port
#define		ER_F_COM_RPT							61//fail to read vai serial port
#define		ER_F_COM_GTS							62//fail to get the com status
#define		ER_F_COM_SET							62//fail to get the com status

/************************************************************************/




/************************************************************************/
/*                                                                      */
/************************************************************************/
#define		ER_F_OPBMPF								65//bmpfile.file = fopen(bmpfile.strDataFileName,"wb");
#define		ER_F_SCANCMD							66//Fail to get the scan data or the scan cmmand is error



/************************************************************************/
/*  Error Tab PRSubExtenedUI.dll */
// THIS error table just for the  PRSubExtenedUI.dll used in shangdong gongshang bank
// of china.
#define		ER_LOAD_PTUSB							81//hSend = LoadLibrary("PrinterUsb");
#define		ER_GET_PROC								82//sendFile = (SendFun)GetProcAddress(hSend,"UsbSendFile");
#define		ER_LOAD_PRSCANNERSDGH					83//hScan = LoadLibrary("PRScanner_sdgh");
#define		ER_GET_PIC								84//getpic = (ScanFun)GetProcAddress(hScan,"getPicture");
#define		ER_GET_SCVALIDPROC						85//isValid = (ScanFun)GetProcAddress(hVlid,"isInvalidScanner");
#define		ER_F_SCANNER							86//have not find the scanner

/************************************************************************/

/************************************************************************/
/* Error tab for PRCmd.DLL;*/
/************************************************************************/
#define		ER_F_LAD_DLL							111//hDll = LoadLibrary("PRCmd");
#define		ER_F_GET_PRC							112//pNt = (NTFun)GetProcAddress(hDll,"writent");
#define		ER_F_FIND_FUN							113//can no find the specified fun

/************************************************************************/
/*ErrorTab  Translt.dll*/
// THIS error table just for the  Translt.dll used in shangdong gongshang bank
// of china.
#define		ER_OPEN_FILE							115
#define		ER_MALLOC_BUFFER						116
//#define		ERROR_OPEN_FILE							-143

#define		TSF_SSUSS								0//SUCCESS to transfort
#define		TSF_NONE								1//no transportable information
#define		TSF_FAIL								2//no transportable information

/************************************************************************/



/************************************************************************/
/*ErrorTab  PRScanner_sdgh.dlll*/
#define		ER_SCAN_INVALID_WID						130
/************************************************************************/


 



enum {PAP_empty,PAP_exist,PAP_aligned,PAP_inserted};//paper status


void dbbox(char str[]);
void dbInf(char *buf,int len);
void showIfo(char str[]);
bool getsyscfigpath(char *strPath);
void getsystime(char *strtime);
bool copyFile(char srcFile[],char dstFile[]);
void setLogFlag(bool bflag);


CRITICAL_SECTION cs_log;
// typedef	struct  
// {
// 	long	num;
// 	char	strER[100];	
// }ERRORMAPTAB;


// ERRORMAPTAB		ER_map[] = {
// {SUCCESS_OPR,"1.operatio success"},
// // {,""},
// // {,""},
// // {,""},
// // {,""},
// // {,""},
// // {,""},
// // {,""},
// // {,""},
// // {,""},
// // //{,""},
// {-999,""}
// };


// char *getERInfo(long num)
// {
// 	return "";
// }


char  strLogPath[K1]="\\TData\\log_scan.txt";
bool  bgLogFlag = true;


BOOL createFolder(char strPath[]);

void dbInf(char *buf,int len)
{
	if(!bgLogFlag)	return;

	FILE	*file;
	char	strPath[512];
	char	strTime[512];
	int		nstrLen = 512;
	int		nret = 0;

	struct tm *newtime;
	time_t	aclock;

	struct stat stFile;
	int		nFileSize = 0;

	memset(strTime,0x00,nstrLen);
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form
	strcpy(strTime,asctime( newtime ));


	memset(strPath,0x00,nstrLen);
	GetSystemDirectory(strPath,200);
// #ifdef DEBUG_VER
// 	strncat(strPath,"\\TData\\scannerdebug.log",strlen("\\TData\\scannerdebug.log"));
// #else
	strncat(strPath,strLogPath,strlen(strLogPath));
// #endif
	
	stat(strPath,&stFile);
	nFileSize = stFile.st_size;
	if (nFileSize>=1024*1024)
	{
		DeleteFile(strPath);
	}

	file = fopen(strPath,"a");


	if (file==NULL)
	{
		dbbox("NantianPR.h: 打开日志文件失败");
		return;
	}
	

	nret = fwrite(strTime,1,strlen(strTime),file);
	nret = fwrite(buf,1,len,file);
	nret = fwrite("\x0d\x0a\x0d\x0a",1,4,file);
	fclose(file);
}


void dbbox(char str[])
{
// #ifdef DEBUG_VER
	MessageBox(NULL,str,"调试信息",MB_OK);
// #endif

// 	dbInf(str,strlen(str));
}


void showIfo(char str[])
{	
	MessageBox(NULL,str,"Warning",MB_OK);	
}



void wInf(char *buf)
{
	if(!bgLogFlag)	return ;
#if  1//  XZL_TEST			001
// 	EnterCriticalSection(&cs_log);
	FILE	*file;
	char	strPath[512];
	char	strTime[512];
	int		nstrLen = 512;
	int		nret = 0;
	
	struct tm *newtime;
	time_t	aclock;
	
	struct stat stFile;
	int		nFileSize = 0;


	memset(strTime,0x00,nstrLen);
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form
	//strcpy(strTime,asctime( newtime ));
	sprintf(strTime,"%4.4d/%2.2d/%2.2d-%2.2d:%2.2d:%2.2d-",
	newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,
	newtime->tm_hour,newtime->tm_min,newtime->tm_sec);	
	
	
	memset(strPath,0x00,nstrLen);
// 	GetSystemDirectory(strPath,200);
	strcat(strPath,"d:\\GpsProtocal");
	if(!createFolder(strPath))
	{
		//dbbox("Nantian.h--创建日志文件夹失败");
		return ;
	}
	strcat(strPath,"\\");
	strncat(strPath,strLogPath,strlen(strLogPath));


	stat(strPath,&stFile);
	nFileSize = stFile.st_size;
	if (nFileSize>=1024*1024*1)
	{
		DeleteFile(strPath);
	}

	
	file = fopen(strPath,"ab");	
	if (file==NULL)
	{
 		//dbbox("NantianPR.h: 打开日志文件失败");
		return;
	}
	


	nret = fwrite(strTime,1,strlen(strTime),file);
	nret = fwrite(buf,1,strlen(buf),file);
	nret = fwrite("\x0d\x0a",1,2,file);
// 	_fcloseall();	
	fclose(file);
// 	LeaveCriticalSection(&cs_log);
#endif
}

void wInf(char *buf,int nLen)
{
	if(!bgLogFlag)	return ;
#if  1//  XZL_TEST			001
	// 	EnterCriticalSection(&cs_log);
	FILE	*file;
	char	strPath[512];
	char	strTime[512];
	int		nstrLen = 512;
	int		nret = 0;
	
	struct tm *newtime;
	time_t	aclock;
	
	struct stat stFile;
	int		nFileSize = 0;
	
	
	memset(strTime,0x00,nstrLen);
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form
	//strcpy(strTime,asctime( newtime ));
	sprintf(strTime,"%4.4d/%2.2d/%2.2d-%2.2d:%2.2d:%2.2d-",
		newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,
		newtime->tm_hour,newtime->tm_min,newtime->tm_sec);	
	
	
	memset(strPath,0x00,nstrLen);
	// 	GetSystemDirectory(strPath,200);
	strcat(strPath,"d:\\GpsProtocal");
	if(!createFolder(strPath))
	{
		//dbbox("Nantian.h--创建日志文件夹失败");
		return ;
	}
	strcat(strPath,"\\");
	strncat(strPath,strLogPath,strlen(strLogPath));
	
	
	stat(strPath,&stFile);
	nFileSize = stFile.st_size;
	if (nFileSize>=1024*1024*100)
	{
		DeleteFile(strPath);
	}
	
	
	file = fopen(strPath,"ab");	
	if (file==NULL)
	{
		//dbbox("NantianPR.h: 打开日志文件失败");
		return;
	}
	
	
	
	nret = fwrite(strTime,1,strlen(strTime),file);
	nret = fwrite(buf,1,nLen,file);
	nret = fwrite("\x0d\x0a",1,2,file);
	// 	_fcloseall();	
	fclose(file);
	// 	LeaveCriticalSection(&cs_log);
#endif
}



void wInf(char *buf,DWORD nerrorcode)
{
	if(!bgLogFlag)	return;
#if  1  //  XL_TEST  002
	FILE	*file;
	char	strPath[512];
	char	strTime[512];
	char	strTmp[1024];
	int		nstrLen = 512;
	int		nret = 0;
	
	struct tm *newtime;
	time_t	aclock;
	
	struct stat stFile;
	int		nFileSize = 0;
	
	memset(strTime,0x00,nstrLen);
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form
	strcpy(strTime,asctime( newtime ));
	
	
	memset(strPath,0x00,nstrLen);
	memset(strTmp,0x00,1024);
	GetSystemDirectory(strPath,200);
	strncat(strPath,strLogPath,strlen(strLogPath));
	
	stat(strPath,&stFile);
	nFileSize = stFile.st_size;
	if (nFileSize>=1024*1024)
	{
		DeleteFile(strPath);
	}
	
	file = fopen(strPath,"ab");
	
	
	if (file==NULL)
	{
		dbbox("NantianPR.h: 打开日志文件失败");// XZL_TEST
		return;
	}
	
	sprintf(strTmp,"---:%d",nerrorcode);
	nret = fwrite(strTime,1,strlen(strTime),file);
	nret = fwrite(buf,1,strlen(buf),file);
	nret = fwrite(strTmp,1,strlen(strTmp),file);
	nret = fwrite("\x0d\x0a\x0d\x0a",1,4,file);

	fclose(file);
// 	_fcloseall();;
#endif
}


void wInf(char *buf1,char *buf2,DWORD nerrorcode)
{
	if(!bgLogFlag)	return;
#if  1//  XZL_TEST  003
	FILE	*file;
	char	strPath[512];
	char	strTime[512];
	char	strTmp[1024];
	int		nstrLen = 512;
	int		nret = 0;
	
	struct tm *newtime;
	time_t	aclock;
	
	struct stat stFile;
	int		nFileSize = 0;
	
	memset(strTime,0x00,nstrLen);
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form
	strcpy(strTime,asctime( newtime ));
	
	
	memset(strPath,0x00,nstrLen);
	memset(strTmp,0x00,1024);
	GetSystemDirectory(strPath,200);
	// #ifdef DEBUG_VER
	// 	strncat(strPath,"\\TData\\scannerdebug.log",strlen("\\TData\\scannerdebug.log"));
	// #else
	strncat(strPath,strLogPath,strlen(strLogPath));
	// #endif
	
	stat(strPath,&stFile);
	nFileSize = stFile.st_size;
	if (nFileSize>=1024*1024)
	{
		DeleteFile(strPath);
	}
	
	file = fopen(strPath,"ab");
	
	
	if (file==NULL)
	{
		dbbox("NantianPR.h: 打开日志文件失败");//
		return;
	}
	
	sprintf(strTmp,"---:%d",nerrorcode);
	nret = fwrite(strTime,1,strlen(strTime),file);
	nret = fwrite(buf1,1,strlen(buf1),file);
	nret = fwrite("---",1,3,file);
	nret = fwrite(buf2,1,strlen(buf2),file);
	nret = fwrite(strTmp,1,strlen(strTmp),file);
	nret = fwrite("\x0d\x0a\x0d\x0a",1,4,file);
	
	// 	// XZL_TEST
	// 	getsystime(buf);
	// 	showIfo(buf);
	
	fclose(file);
#endif
}

void wInf(char *buf1,char *buf2/*,DWORD nerrorcode*/)
{
	if(!bgLogFlag)	return;
#if  1//  XZL_TEST  004
	FILE	*file;
	char	strPath[512];
	char	strTime[512];
	char	strTmp[1024];
	int		nstrLen = 512;
	int		nret = 0;
	
	struct tm *newtime;
	time_t	aclock;
	
	struct stat stFile;
	int		nFileSize = 0;
	
	memset(strTime,0x00,nstrLen);
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form
	strcpy(strTime,asctime( newtime ));
	
	
	memset(strPath,0x00,nstrLen);
	memset(strTmp,0x00,1024);
	GetSystemDirectory(strPath,200);
	// #ifdef DEBUG_VER
	// 	strncat(strPath,"\\TData\\scannerdebug.log",strlen("\\TData\\scannerdebug.log"));
	// #else
	strncat(strPath,strLogPath,strlen(strLogPath));
	// #endif
	
	stat(strPath,&stFile);
	nFileSize = stFile.st_size;
	if (nFileSize>=1024*1024)
	{
		DeleteFile(strPath);
	}
	
	file = fopen(strPath,"ab");
	
	
	if (file==NULL)
	{
		dbbox("NantianPR.h: 打开日志文件失败");// XZL_TEST
		return;
	}
	
	sprintf(strTmp,"--",buf2);
	nret = fwrite(strTime,1,strlen(strTime),file);
	nret = fwrite(buf1,1,strlen(buf1),file);
	nret = fwrite(strTmp,1,strlen(strTmp),file);
	nret = fwrite(buf2,1,strlen(buf2),file);
// 	nret = fwrite(strTmp,1,strlen(strTmp),file);
	nret = fwrite("\x0d\x0a\x0d\x0a",1,4,file);
	
	// 	// XZL_TEST
	// 	getsystime(buf);
	// 	showIfo(buf);
	
// 	_fcloseall();
	fclose(file);
#endif
}

void wInf(char *buf1,char *buf2,char *buf3/*,DWORD nerrorcode*/)
{
	if(!bgLogFlag)	return;

#if  1//  XZL_TEST   005
	FILE	*file;
	char	strPath[512];
	char	strTime[512];
	char	strTmp[1024];
	int		nstrLen = 512;
	int		nret = 0;
	
	struct tm *newtime;
	time_t	aclock;
	
	struct stat stFile;
	int		nFileSize = 0;
	
	memset(strTime,0x00,nstrLen);
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form
	strcpy(strTime,asctime( newtime ));
	
	
	memset(strPath,0x00,nstrLen);
	memset(strTmp,0x00,1024);
	GetSystemDirectory(strPath,200);
	// #ifdef DEBUG_VER
	// 	strncat(strPath,"\\TData\\scannerdebug.log",strlen("\\TData\\scannerdebug.log"));
	// #else
	strncat(strPath,strLogPath,strlen(strLogPath));
	// #endif
	
	stat(strPath,&stFile);
	nFileSize = stFile.st_size;
	if (nFileSize>=1024*1024)
	{
		DeleteFile(strPath);
	}
	
	file = fopen(strPath,"ab");
	
	
	if (file==NULL)
	{
		dbbox("NantianPR.h: 打开日志文件失败");// XZL_TEST
		return;
	}
	
	sprintf(strTmp,"--");
	nret = fwrite(strTime,1,strlen(strTime),file);
	nret = fwrite(buf1,1,strlen(buf1),file);
	nret = fwrite(strTmp,1,strlen(strTmp),file);
	nret = fwrite(buf2,1,strlen(buf2),file);
	nret = fwrite(strTmp,1,strlen(strTmp),file);
	nret = fwrite(buf3,1,strlen(buf3),file);
	// 	nret = fwrite(strTmp,1,strlen(strTmp),file);
	nret = fwrite("\x0d\x0a\x0d\x0a",1,4,file);
	
	// 	// XZL_TEST
	// 	getsystime(buf);
	// 	showIfo(buf);
	
	fclose(file);
// 	_fcloseall();
#endif
}




bool getsyscfigpath(char *strPath)
{
	char		strDst[512];
	int			nstrLen = 512;


	memset(strDst,0x00,nstrLen);
	GetSystemDirectory(strDst,200);
	strncat(strDst,strPath,strlen(strPath));


	strcpy(strPath,strDst);
	return true;

}


void getsystime(char *strtime)
{
	struct tm *newtime=NULL;
	time_t	aclock;

	memset(newtime,0x00,sizeof(newtime));
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock ); 
	sprintf(strtime,"%4.4d%2.2d%2.2d%2.2d%2.2d",
		newtime->tm_year,
		newtime->tm_mon,
		newtime->tm_mday,
		newtime->tm_hour,
		newtime->tm_min,
		newtime->tm_sec);
}


bool copyFile(char srcFile[],char dstFile[])
{
	FILE		*psrcFile = NULL;
	FILE		*pdstFile = NULL;
	char		strbuf[1024];
	UINT		nLen = 1024;
	UINT		nwed = 0;

	psrcFile = fopen(srcFile,"rb");
	psrcFile = fopen(dstFile,"wb");
	if(dstFile&&psrcFile)
	{
		fclose(psrcFile);
		fclose(pdstFile);
		return false;
	}

	memset(strbuf,0x00,1024);
	nLen = 1024;
	while(nLen==1024)
	{
		memset(strbuf,0x00,1024);
		nLen = fread(strbuf,1,1024,psrcFile);
		nwed = fwrite(strbuf,1,nLen,pdstFile);
		if(nwed!= nLen)
		{
			fclose(psrcFile);
			fclose(pdstFile);
			return false;
		}
	}

	fclose(psrcFile);
	fclose(pdstFile);

	return true;
}

void setLogFlag(bool bflag)
{
	bgLogFlag = bflag;
}

BOOL createFolder(char strPath[])
{
	BOOL	bret = FALSE;
	UINT	nret = 0;
	
	bret = CreateDirectory(strPath,NULL);	
	if(!bret)
	{
		nret = GetLastError();
		if(nret==183)
		{
			return true;//current directory already exists
		}
		else
		{
			return FALSE;
		}
	}
	return bret;
}


void renameLogName(char *strFileName)
{
	memset(strLogPath,0x00,K1);
	strcpy(strLogPath,strFileName);
}

#endif