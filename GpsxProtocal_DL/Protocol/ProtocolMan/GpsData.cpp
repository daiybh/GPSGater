/********************************************************************
	created:	2012/01/23
	created:	23:1:2012   8:55
	filename: 	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps\GpsData.cpp
	file path:	F:\workspace\project\Project-DuanYB\dev-gps\codes\Dll_Dev_GPS\curGps\DevxGps
	file base:	GpsData
	file ext:	cpp
	author:		areslion@qq.com
	
	purpose:	
*********************************************************************/
#include "stdafx.h"
#include "GpsData.h"


bool fillStr( char *srcStr,char * dstStr, char *strTarget, long &nlen )
{
	if(findStr(srcStr,strTarget,nlen))
	{
		strncpy(dstStr,srcStr,nlen);
		nlen = nlen +1;
		return true;
	}
	return false;
}



bool findStr( char *buf, char *targetstr,long &ny )
{
	int	yLen = strlen(targetstr);

	ny = 0;
	if(yLen<=0||yLen>(long)strlen(buf))	return false;

	for (;ny<(long)strlen(buf);++ny)
	{
		if(!strncmp(&buf[ny],targetstr,strlen(targetstr)))
		{
			return true;
		}
	}

	return false;
}



/************************************
// Method:    findstr
// FullName:  Protocal::findstr
// Access:    private 
// Returns:   long
// Qualifier:
// Parameter: char * buf
// Parameter: char * targetstr
// Parameter: bool bLeft
bLeft: 
	true: search from left
	false: search form right
Return:
	-1: fail to find the specified string
	else: success to find the specifide string
************************************/
long findstr( char *buf, char *targetstr,bool bLeft/*=TRUE*/ )
{
	long	xpos = -1;
	long	xLen = strlen(targetstr);
	long	yLen = strlen(buf);
	
	if(xLen>yLen)	return -1;
	
	if(bLeft)
	{
		for (long a=0;a<yLen;++a)
		{
			if(!memcmp(&buf[a],targetstr,xLen))
			{
				xpos = a;
				break;
			}
		}	
	}
	else
	{
		for (long a=yLen-xLen;a<yLen;a--)
		{
			if(!memcmp(&buf[a],targetstr,xLen))
			{
				xpos = a;
				break;
			}
		}
	}

	return xpos;	
}

/************************************
// Method:    getCRC
// FullName:  Protocal::getCRC
// Access:    public 
// Returns:   long
// Qualifier:
// Parameter: char * buf
************************************/
long getCRCVal( char *buf)
{
	long	nret = 0;
	long	xpos = 0;
	long	ypos =0;

	getCRCPos(buf,xpos,ypos);
	for(long i=xpos;i<ypos;++i)
	{
		nret += buf[i];
	}

	return nret;
}




/************************************
// Method:    getCRCPos
// FullName:  Protocal::getCRCPos
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: char * buf
// Parameter: long & xpos
// Parameter: long yPos
buf: txt
xpos: initial position of CRC
ypos: ending position of CRC
************************************/
void getCRCPos( char *buf, long &xpos,long yPos )
{
	bool	bret = false;	

	xpos = findstr(buf," :",true);
	yPos = findstr(buf," ", false);

	if(xpos<0||yPos<0)	xpos = yPos = 0;
}
