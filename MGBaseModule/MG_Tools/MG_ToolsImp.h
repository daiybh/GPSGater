#ifndef MG_TOOLS_IMP_H
#define MG_TOOLS_IMP_H
class CMGTOOls
{
public:
	CMGTOOls(void);
	~CMGTOOls(void);

	int GetConfig_Int(  LPCTSTR lpAppName,  LPCTSTR lpKeyName,  INT nDefault,  LPCTSTR lpFileName );
	DWORD GetConfig_String(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpDefault,LPTSTR lpReturnedString,DWORD nSize,LPCTSTR lpFileName		);

	int WriteConfig_Int(LPCTSTR lpAppName,  LPCTSTR lpKeyName,  const int nValue,  LPCTSTR lpFileName );
	BOOL WriteConfig_String(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpString,LPCTSTR lpFileName);
	LPCTSTR Get_MgConfigFileName();	   
	void makeUpper(TCHAR *pChar);

	int GetCurChannelID();
private:	   
	void Parse_CmdLine();
	void ParseParam(const TCHAR *pszParam);
	LPCTSTR split_Config_FileName(LPCTSTR lpFileName);
	TCHAR m_strMgConfigFile_Relative_Path[MAX_PATH];
	TCHAR m_strMgConfigFile_Absolute_Path[MAX_PATH];
	TCHAR m_strModulePath[MAX_PATH];
	int m_nChannelID;
	int m_bRelative;

};

#endif