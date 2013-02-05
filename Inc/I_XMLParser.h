#pragma  once
#ifdef I_XML_PARSER_EXPORTS
#define I_XML_PARSER_API __declspec(dllexport)
#else
#define I_XML_PARSER_API __declspec(dllimport)
#pragma comment(lib,"XMLParser.lib")
#endif

#include <VECTOR>
class I_XML_PARSER_API I_XMLParser  
{
public:
	BOOL static CreateInstance(I_XMLParser **ppIXMLParser);
	virtual BOOL Create() =0;
	virtual BOOL Parse(LPCTSTR strXML) =0;
	virtual LPCSTR GetNodeText(LPCTSTR strNodeName,TCHAR *pDestBuffer,int nNode=0) =0;
	virtual int GetNodeText_len(LPCTSTR strNodeName,TCHAR *pDestBuffer,int nNode=0)=0;
	virtual int GetRootName(TCHAR*pRootName) =0;
private:
	/*
	virtual BOOL GetRoot(LPCTSTR &strName, LPCTSTR strPara) =0;
	virtual CString GetSecondLayerNodeName(int nIndex, CString strNodeName) =0;
	virtual BOOL GetNodesText(CString strNodeName, std::vector<CString>& vText) =0;
	virtual BOOL GetNodesXML(CString strNodeName,std::vector<CString>& vXML) =0;
	virtual BOOL SetNodeText(CString strNodeName, CString strText) =0;
	virtual CString GetXML(void) =0;
	virtual CString GetNodeXML(CString strNodeName) =0;
	virtual CString GetNodeXML(CString strNodeName,int nIndex) =0;
	virtual int GetLastError(CString& strError) =0;
	virtual CString GetNodeAttribute(CString strNodeName,CString strAttributeName) =0;
	virtual CString GetNodeAttribute(CString strAttributeName) =0;	
	virtual CString GetNodeText(int nIndex) =0;
	virtual CString GetNodeText(CString strNodeName,int nNode) =0;
	virtual int GetNodeCount() const =0;
	/**/
	
};
