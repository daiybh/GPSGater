 // Parser.h: interface for the CXMLParser class.
// XMLParser class provides easy access to MS-DOM XML interface
// It now can only be used to parse XML text
// Written by maozhen	maozhen@sobey.com	ver:1.0
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSER_H__67C3EC40_0281_41CE_BD70_099D94696E29__INCLUDED_)
#define AFX_PARSER_H__67C3EC40_0281_41CE_BD70_099D94696E29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <VECTOR>
#import <msxml4.dll> 
using namespace MSXML2;
class  CXMLParser3  //:public I_XMLParser
{
public:
// 	CXMLParser(const CXMLParser& src);			// no implementation
// 	void operator = (const CXMLParser& src);	// no implementation

public:
	CXMLParser3();
	virtual ~CXMLParser3();

// Implementation
public:
	CString GetSecondLayerNodeName(int nIndex, CString strNodeName);
	BOOL GetNodesText(CString strNodeName, std::vector<CString>& vText);
	BOOL GetNodesXML(CString strNodeName,std::vector<CString>& vXML);
	BOOL SetNodeText(CString strNodeName, CString strText);
	CString GetXML(void);
	CString GetNodeXML(CString strNodeName);
	CString GetNodeXML(CString strNodeName,int nIndex);
	int GetLastError(CString& strError);
	BOOL GetRoot(CString &strName, CString &strPara);
	CString GetNodeAttribute(CString strNodeName,CString strAttributeName);
	CString GetNodeAttribute(CString strAttributeName);
	BOOL Create();
	BOOL Parse(CString strXML);
	CString GetRootName();
	CString GetNodeText(CString strNodeName);
	CString GetNodeText(int nIndex);
	CString GetNodeText(CString strNodeName,int nNode);
	int GetNodeCount() const;

	// 过滤XML文本中的非法字符，将其转义为相应的实体引用
	static CString FilterText(LPCTSTR lpszText);

protected:
	MSXML2::IXMLDOMDocument2Ptr m_pXMLDom;
	MSXML2::IXMLDOMNodeListPtr m_pNodeList;
private:
	int			m_nError;
	CString		m_strError;
};

#endif // !defined(AFX_PARSER_H__67C3EC40_0281_41CE_BD70_099D94696E29__INCLUDED_)
