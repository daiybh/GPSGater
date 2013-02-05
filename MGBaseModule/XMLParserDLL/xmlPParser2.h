#pragma  once
#include "I_XMLParser.h"

class CXMLParser2 :public I_XMLParser
{
public:
	//CXMLParser2();
	//virtual ~CXMLParser2();
	bool Init(){return false;};
	//*
	CString GetSecondLayerNodeName(int nIndex, CString strNodeName){return _T("l");};
	BOOL GetNodesText(CString strNodeName, std::vector<CString>& vText){return FALSE;};
	BOOL GetNodesXML(CString strNodeName,std::vector<CString>& vXML){return FALSE;};
	BOOL SetNodeText(CString strNodeName, CString strText){return FALSE;};
	CString GetXML(void){return _T("l");};
	CString GetNodeXML(CString strNodeName){return _T("l");};
	CString GetNodeXML(CString strNodeName,int nIndex){return _T("l");};
	int GetLastError(CString& strError){return FALSE;};
	BOOL GetRoot(CString &strName, CString &strPara){return FALSE;};
	CString GetNodeAttribute(CString strNodeName,CString strAttributeName){return _T("l");};
	CString GetNodeAttribute(CString strAttributeName){return _T("l");};
	BOOL Create(){return FALSE;};
	BOOL Parse(CString strXML){return FALSE;};
	CString GetRootName(){return _T("l");};
	CString GetNodeText(CString strNodeName){return _T("l");};
	CString GetNodeText(int nIndex){return _T("l");};
	CString GetNodeText(CString strNodeName,int nNode){return _T("l");};
	int GetNodeCount() const{return FALSE;};

	// 过滤XML文本中的非法字符，将其转义为相应的实体引用
	CString FilterText(LPCTSTR lpszText){return _T("");};
	/**/
private:
	int			m_nError;
	CString		m_strError;
};