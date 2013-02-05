#include "StdAfx.h"
#include "MG_ToolsImp.h"
#include <conio.h>
#define USE_MSV_CONFIG_READ_API_EXP
#ifdef USE_MSV_CONFIG_READ_API_EXP
#include "MSVConfigReadAPIExp.h"
#endif


CMGTOOls::CMGTOOls(void)
{
	//absolute Config.
	{
		TCHAR strModuleFileName[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		GetModuleFileName(NULL,strModuleFileName,MAX_PATH);
		_tsplitpath_s(strModuleFileName, m_strModulePath,_countof(m_strModulePath), strPath,_countof(strPath),NULL,0,NULL,0);
		_tcscat_s(m_strModulePath,_countof(m_strModulePath),strPath);  
		_tcscpy(m_strMgConfigFile_Absolute_Path,m_strModulePath); 
		_tcscat_s(m_strMgConfigFile_Absolute_Path,_countof(m_strMgConfigFile_Absolute_Path),_T("MgCaptureCfg.ini"));
		
	}
	//relative Config .
   m_nChannelID = 0;
   m_bRelative = 0;
}

CMGTOOls::~CMGTOOls(void)
{
}


int CMGTOOls::GetConfig_Int( LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName )
{		  
	if(_tcslen(lpFileName) <1)
		return nDefault;
	if(!m_bRelative)
	{
#ifdef _UNICODE
		return GetPrivateProfileIntW(lpAppName,lpKeyName,nDefault,lpFileName);
#else
		return GetPrivateProfileIntA(lpAppName,lpKeyName,nDefault,lpFileName);
#endif
	}	
	return ReadConfigParamValue(lpAppName,lpKeyName,nDefault,split_Config_FileName(lpFileName));	
	return 0;

}

DWORD CMGTOOls::GetConfig_String( LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpDefault,LPTSTR lpReturnedString,DWORD nSize,LPCTSTR lpFileName )
{
	if(_tcslen(lpFileName) <1)
	{
		int nLen = _tcslen(lpDefault);
		if(nLen>0)
			_tcscpy(lpReturnedString,lpDefault);
		return nLen;
	}
	if(!m_bRelative)
#ifdef _UNICODE
		return GetPrivateProfileStringW(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
#else
		return GetPrivateProfileStringA(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
#endif
	
	CString strRet;
	strRet = ReadConfigParamValue(lpAppName,lpKeyName,lpDefault,split_Config_FileName(lpFileName));	
	_tcscpy(lpReturnedString,strRet);	
	//_tcscpy_s(lpReturnedString,strRet.GetLength(),strRet);
	if(lpReturnedString == lpDefault)
		return 0;
	return 1;
}

const TCHAR * GetStr2( const TCHAR*pSrc,const TCHAR*pFind,TCHAR *pValue )
{
	const TCHAR *pFindStr= _tcsstr(pSrc,pFind);
	CString str;
	int nOption = pFindStr-pSrc;
	if(pFindStr==pSrc)
	{
		_tcscpy(pValue,pSrc+_tcslen(pFind));
	}
	return pFindStr;
}

const TCHAR *pCfg=_T("CFG=");
const TCHAR *pID=_T("ID=");
const TCHAR *pRelative =_T("RELATIVE=")	;
const TCHAR *pHelp=_T("HELP");
const TCHAR *pMSGBox=_T("MSGBOX")   ;
void CMGTOOls::ParseParam(const TCHAR *pszParam)
{
	TCHAR pReturn[255];
	if(GetStr2(pszParam,pCfg,pReturn))
	{
		BOOL bRet=	LoadConfigParam(pReturn);
		//WriteLog(_T("MgAppLog"),logLevelInfo,_T("LoadConfigParam [%s]=%d"),pReturn,bRet);
	}
	else if(GetStr2(pszParam,pID,pReturn))
	{
		m_nChannelID = 		_ttoi(pReturn);
	}
	else if(GetStr2(pszParam,pRelative,pReturn))
	{
		m_bRelative = _ttoi(pReturn);
	}
	else if(GetStr2(pszParam,pHelp,pReturn))
	{
		CString strLog;
		strLog.Format(_T("Help\r\n id:  -id=[ChannnelID]\r\n是否采用Configuration 配置目录： -relative= [0:no 1:yes]"));
		AfxMessageBox(strLog);
	   exit(0);
	}
	else if(GetStr2(pszParam,pMSGBox,pReturn))
	{
		AfxMessageBox(AfxGetApp()->m_lpCmdLine);
	}
}
void CMGTOOls::Parse_CmdLine()
{	   
	TCHAR pTemp[255];
	for (int i = 1; i < __argc; i++)
	{
		LPCTSTR pszParam = __targv[i];
		BOOL bFlag = FALSE;
		BOOL bLast = ((i + 1) == __argc);
		if (pszParam[0] == '-' || pszParam[0] == '/')
		{
			// remove flag specifier
			bFlag = TRUE;
			++pszParam;
		}
		if(bFlag)
		{
			_tcscpy(pTemp,pszParam);
			makeUpper(pTemp);
			ParseParam(pTemp);
		}
	}
}
LPCTSTR CMGTOOls::split_Config_FileName(LPCTSTR lpFileName)
{
		if(!m_bRelative)
			return lpFileName;

		//只取文件名，不要路径
		LPCTSTR pStr = _tcsrchr(lpFileName,'\\');
		if(pStr==NULL)
			return lpFileName;
		return pStr+1;  
}
void CMGTOOls::makeUpper(TCHAR *pChar)
{
	for (int i=0;pChar[i]!=_T('\0');i++)
	{
		pChar[i] = _totupper(pChar[i]);
	}
}
#include "SelectDlg.h"
int CMGTOOls::WriteConfig_Int( LPCTSTR lpAppName, LPCTSTR lpKeyName, const int nValue, LPCTSTR lpFileName )
{
	if(_tcsstr(lpFileName,L"none"))	  
	{
		if(_tcsstr(lpAppName,L"StartLoad"))
		{		
			m_bRelative = 0;
			m_nChannelID = 0;
			Parse_CmdLine();
			//*
			//AFX_MANAGE_STATE(AfxGetStaticModuleState());
			CSelectDlg *dlg =new CSelectDlg;
			dlg->Create(IDD_SELECT_DLG);
			dlg->ShowWindow(SW_SHOW);
			/**/
			if(m_bRelative)		
			{
				LoadConfigParam(_T("\\configuration\\InGestServer\\global.ini"));
				_stprintf(m_strMgConfigFile_Relative_Path,_T("\\configuration\\InGestServer\\channel_%d\\MgCaptureCfg.ini"),m_nChannelID);
				LoadConfigParam(m_strMgConfigFile_Relative_Path);

				//加载自己需要的配置文件，多加载一次也无所谓
				_stprintf(m_strMgConfigFile_Relative_Path,_T("%s\\%s"),m_strModulePath,_T("FileEncodeTypeTable.CFG"));
				LoadConfigParam(m_strMgConfigFile_Relative_Path);

			}
		}
		return true;
	}
	if(!m_bRelative)
	{
		CString strValue;
		strValue.Format(_T("%d"),nValue);
		return WriteConfig_String(lpAppName,lpKeyName,strValue,lpFileName);
	}
	return WriteConfigParamValue(lpAppName,lpKeyName,nValue,lpFileName);
}

BOOL CMGTOOls::WriteConfig_String( LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpString,LPCTSTR lpFileName )
{
	if(!m_bRelative)
		return WritePrivateProfileStringW(lpAppName,lpKeyName,lpString,lpFileName);
	return WriteConfigParamValue(lpAppName,lpKeyName,lpString,lpFileName);
}

LPCTSTR CMGTOOls::Get_MgConfigFileName()
{	
	return m_strMgConfigFile_Absolute_Path;
}

int CMGTOOls::GetCurChannelID()
{
	return m_nChannelID;
}
