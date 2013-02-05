/********************************************************************
	created:	2012/02/18
	created:	18:2:2012   15:52
	filename: 	E:\WORKSPACE\PROGRAM\PROJECT-DUANYB\DEV-GPS\CODES\GpsxConsole\Importfile.h
	file path:	E:\WORKSPACE\PROGRAM\PROJECT-DUANYB\DEV-GPS\CODES\GpsxConsole
	file base:	Importfile
	file ext:	h
	author:		areslion@qq.com
	
	purpose:	
*********************************************************************/
#ifndef IMPORTFILE_H
#define IMPORTFILE_H

#define DLL_TYPE extern "C" __declspec(dllexport)

DLL_TYPE long start(unsigned __int64 &iLastDataTime);
DLL_TYPE long stop();

#endif