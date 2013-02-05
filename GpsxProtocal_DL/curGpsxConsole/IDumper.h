#ifndef I_INTERFACE_Dumper_H
#define I_INTERFACE_Dumper_H

#ifndef COMPILE_DEFINE_
	#ifdef _UNICODE
		#pragma comment(lib,"DumperU.lib")
	#else
		#pragma comment(lib,"Dumper.lib")
#endif
#define DUMPER_EXPORT __declspec(dllimport)
#else
#define DUMPER_EXPORT __declspec(dllexport)
#endif 
class DUMPER_EXPORT IDumper
{
public:
	virtual void SetExceptionFilter()=0;
	virtual int ExtendFunc(DWORD dwCMDID,int nParam = 0,LPVOID* pIn = NULL,LPVOID* pOut = NULL) = 0; 
};
DUMPER_EXPORT IDumper *  CreateDumper();
#endif //I_INTERFACE_Dumper_H