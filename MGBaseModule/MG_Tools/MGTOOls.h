#ifndef MG_TOOLS_H
#define MG_TOOLS_H
#pragma once



#define MSV_INGEST_CFG_NAME											_T("MgCaptureCfg.ini")
#define MSV_PUBLIC_CFG_NAME											_T("Public.ini")
#define MSV_TEMP_CFG_NAME											 _T("MSVCFG.CFG")
#define MSV_FILE_ENCODE_TYPE_TABLE_CFG_NAME			_T("FileEncodeTypeTable.CFG")

#define LOGNAME _T("TS")


#ifdef _MG_TOOLS_EXPORT_DEFINE_
#define MG_TOOLS_EXPORT __declspec(dllexport)
#else
#define MG_TOOLS_EXPORT __declspec(dllimport)
#pragma comment(lib,"MG_Tools.lib")

#ifndef _LOG_WRITER_EXPORT_DEFINE_
#include "NMTraceLog.h"
#pragma comment(lib,"MGLogWriterU.lib")
#endif
#endif
namespace MSV_BASE_TOOLS
{
	//版本
	#define VERSION_STR_555 _T("MSV555")
	#define VERSION_STR_4211P  _T("MSV4211")
	enum MSV_VERSION{VERSION_4211P=0,VERSION_555};

	int MG_TOOLS_EXPORT GetConfig_Int(  LPCTSTR lpAppName,  LPCTSTR lpKeyName,  INT nDefault,  LPCTSTR lpFileName );
	DWORD MG_TOOLS_EXPORT GetConfig_String(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpDefault,LPTSTR lpReturnedString,DWORD nSize,LPCTSTR lpFileName);
	int MG_TOOLS_EXPORT WriteConfig_Int(LPCTSTR lpAppName,  LPCTSTR lpKeyName,  const int nValue,  LPCTSTR lpFileName );

	BOOL MG_TOOLS_EXPORT WriteConfig_String(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpString,LPCTSTR lpFileName);
	int MG_TOOLS_EXPORT GetChannelID();
	int MG_TOOLS_EXPORT GetCtrlPort();
	MSV_VERSION MG_TOOLS_EXPORT GetMSVVersionID();
	LPCTSTR MG_TOOLS_EXPORT GetMSVVersionStr();
	//获取配置文件名
	LPCTSTR MG_TOOLS_EXPORT GetMgConfigFileName();
	LPCTSTR MG_TOOLS_EXPORT GetMSVPublicCfgName();
	void MG_TOOLS_EXPORT makeUpper(TCHAR *pChar);

	void MG_TOOLS_EXPORT getMemroy2log(LPCTSTR functionName);
	UINT MG_TOOLS_EXPORT GetPrivateProfileInt_From_Global( __in LPCTSTR lpAppName, __in LPCTSTR lpKeyName, __in INT nDefault, __in_opt LPCTSTR lpFileName);
	DWORD MG_TOOLS_EXPORT GetPrivateProfileString_From_Global( __in_opt LPCTSTR lpAppName, __in_opt LPCTSTR lpKeyName,
		__in_opt LPCTSTR lpDefault, __out LPTSTR lpReturnedString,
		__in DWORD nSize, __in_opt LPCTSTR lpFileName );

}

//对外接口的暴露，目前使用宏来代替
#ifdef GetPrivateProfileInt	
#undef GetPrivateProfileInt	
#endif

#define GetPrivateProfileInt	MSV_BASE_TOOLS::GetConfig_Int

#ifdef GetPrivateProfileString
#undef  GetPrivateProfileString
#endif

#define GetPrivateProfileString MSV_BASE_TOOLS::GetConfig_String

#ifdef WritePrivateProfileString
#undef WritePrivateProfileString
#endif

#define WritePrivateProfileString MSV_BASE_TOOLS::WriteConfig_String

#ifdef WritePrivateProfileInt
#undef WritePrivateProfileInt
#endif

#define WritePrivateProfileInt MSV_BASE_TOOLS::WriteConfig_Int


#define GetPrivateProfileIntFromGlobal MSV_BASE_TOOLS::GetPrivateProfileInt_From_Global
#define GetPrivateProfileStringFromGlobal MSV_BASE_TOOLS::GetPrivateProfileString_From_Global

#define GetMgConfigFileName MSV_BASE_TOOLS::GetMgConfigFileName
#define GetMSVVersionID		MSV_BASE_TOOLS::GetMSVVersionID
#define GetMSVVersionStr	MSV_BASE_TOOLS::GetMSVVersionStr
#define GetCtrlPort			MSV_BASE_TOOLS::GetCtrlPort
#define GetMSVPublicCfgName MSV_BASE_TOOLS::GetMSVPublicCfgName
#define GetChannelID		MSV_BASE_TOOLS::GetChannelID

#define MemUsage_2_Log MSV_BASE_TOOLS::getMemroy2log

#endif //define MG_TOOLS_H