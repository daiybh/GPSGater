 // Parser.h: interface for the CXMLParser class.
// XMLParser class provides easy access to MS-DOM XML interface
// It now can only be used to parse XML text
// Written by maozhen	maozhen@sobey.com	ver:1.0
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C_TINYXMLPARSER_H__67C3EC40_0281_41CE_BD70_099D94696E29__INCLUDED_)
#define AFX_C_TINYXMLPARSER_H__67C3EC40_0281_41CE_BD70_099D94696E29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "I_XMLParser.h"
#include "tinyxml2.h"

using namespace tinyxml2;
class  CTinyXMLParser  :public I_XMLParser
{
public:
// 	CXMLParser(const CXMLParser& src);			// no implementation
// 	void operator = (const CXMLParser& src);	// no implementation

public:
	CTinyXMLParser();
	virtual ~CTinyXMLParser();

	BOOL Create();
	BOOL Parse(LPCTSTR strXML);
	LPCSTR GetNodeText(LPCTSTR strNodeName,TCHAR *pDestBuffer,int nNode=0);

	BOOL GetRoot(LPCTSTR &strName, LPCTSTR strPara);
	int GetRootName(TCHAR*pRootName);
	int GetNodeText_len(LPCTSTR strNodeName,TCHAR *pDestBuffer,int nNode=0);
// Implementation
private:

protected:
	tinyxml2::XMLDocument *m_pXMLDom;
private:
	int			m_nError;
	CString		m_strError;
};

#endif // !defined(AFX_C_TINYXMLPARSER_H__67C3EC40_0281_41CE_BD70_099D94696E29__INCLUDED_)
