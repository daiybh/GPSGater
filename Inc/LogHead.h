__declspec(dllimport) VOID NMTrace(WCHAR* szModuleName,LONG  lLogLevel,const WCHAR *fmt, ...);
__declspec(dllimport) VOID NMTrace0(WCHAR* szModuleName,LONG  lLogLevel ,WCHAR *szLog);

__declspec(dllimport) VOID NMTraceEx1(WCHAR* szModuleName,LONG  lLogLevel,LONG lLogErrorCode,const WCHAR *fmt, ...);
__declspec(dllimport) VOID NMTrace0Ex1(WCHAR* szModuleName,LONG  lLogLevel ,LONG lLogErrorCode,WCHAR *szLog);

__declspec(dllimport) VOID NMTraceEx2(WCHAR* szModuleName,WCHAR* szFuncationName,LONG lThreadID,LONG  lLogLevel,LONG lLogErrorCode,const WCHAR *fmt, ...);
__declspec(dllimport) VOID NMTrace0Ex2(WCHAR* szModuleName,WCHAR* szFuncationName,LONG lThreadID,LONG  lLogLevel ,LONG lLogErrorCode,WCHAR *szLog);

enum NMLogLevel{logLevelFatefulError = 1, logLevelError,logLevelWarring,
				logLevelInfo,logLevelDebug,logLevelMemory};