#pragma once
#include <afxtempl.h>

/*
#ifdef _UNICODE
	#ifdef _DEBUG
		#pragma comment(lib, "ReadConfigValueDLLUD.lib")
	#else
		#pragma comment(lib, "ReadConfigValueDLLU.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "ReadConfigValueDLLD.lib")
	#else
		#pragma comment(lib, "ReadConfigValueDLL.lib")
	#endif
#endif
*/
#pragma comment(lib, "ReadConfigValueDLLU.lib")

//////////////////////////////////////////////////////////////////////////
//配置参数读取和设置

/*
函数说明:加载配置文件
参数说明:
strConfigPathname:配置文件名称
返回值:
0 	失败
1 	成功
*/
AFX_EXT_API BOOL	LoadConfigParam(LPCWSTR strConfigPathname);

AFX_EXT_API BOOL	LoadConfigParamA(char* strConfigPathname);


/*
函数说明:读取值为字符串的配置项
参数说明:
lpszSectionName	: Section的名称
lpszItemName	: Item 的名称
lpszDefault		: 字符默认值
bAutoCreate		: 是否自动创建，暂无效.	

返回值:	返回字符串配置值	

*/
AFX_EXT_API LPCWSTR ReadConfigParamValue(const LPCWSTR lpszSectionName,const LPCWSTR lpszItemName,const LPCWSTR lpszDefault,const LPCWSTR lpszFileName);


AFX_EXT_API char* ReadConfigParamValueA(const char* lpszSectionName,const char* lpszItemName,const char* lpszDefault,const char* lpszFileName);


/*
函数说明:读取值为数字的配置项
参数说明:
lpszSectionName	: Section的名称
lpszItemName	: Item 的名称
lpszDefault		: 数字默认值
bAutoCreate		: 是否自动创建，暂无效.	
返回值:	返回数字配置值		
*/
AFX_EXT_API int		ReadConfigParamValue(const LPCWSTR lpszSectionName,const LPCWSTR lpszItemName,const int nDefault,const LPCWSTR lpszFileName);

AFX_EXT_API int		ReadConfigParamValueA(const char* lpszSectionName,const char* lpszItemName,const int nDefault,const char* lpszFileName);


/*
函数说明:写值为字符串的配置项
参数说明:
lpszSectionName	: Section的名称
lpszItemName	: Item 的名称
lpszDefault		: 默认值
bWrite2File		: 是否写到文件，一般不用	
返回值:	0 	失败
1 	成功
*/
AFX_EXT_API BOOL	WriteConfigParamValue(const LPCWSTR lpszSectionName,const LPCWSTR lpszItemName,const LPCWSTR lpszItemValue,const LPCWSTR lpszFileName);


AFX_EXT_API BOOL	WriteConfigParamValueA(const char* lpszSectionName,const char* lpszItemName,const char* lpszItemValue, const char* lpszFileName);


/*
函数说明:写值为数字的配置项
参数说明:
lpszSectionName	: Section的名称
lpszItemName	: Item 的名称
lpszDefault		: 默认值
bAutoCreate		: 是否写到文件，一般不用		
返回值:	0 	失败
1 	成功
*/
AFX_EXT_API BOOL	WriteConfigParamValue(const LPCWSTR lpszSectionName,const LPCWSTR lpszItemName,const int nItemValue,const LPCWSTR lpszFileName);

AFX_EXT_API BOOL	WriteConfigParamValueA(const char* lpszSectionName,const char* lpszItemName,const int nItemValue,const char* lpszFileName);


/*
函数说明:保存配置项到文件
参数说明:
返回值:	0 	失败
1 	成功
*/
AFX_EXT_API BOOL	SaveConfigParam();
