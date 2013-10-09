    // Parser.cpp: implementation of the CXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tinyXMLParser.h"
#include <ATLBASE.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




CTinyXMLParser::CTinyXMLParser()
{
	//m_nError = 0;
}

CTinyXMLParser::~CTinyXMLParser()
{
}

BOOL CTinyXMLParser::Create()
{
	m_pXMLDom=new tinyxml2::XMLDocument();
	return m_pXMLDom!=NULL;
}

BOOL CTinyXMLParser::Parse(LPCTSTR strXML)
{
	try
	{
		//BSTR bstr = strXML.AllocSysString();
		if(m_pXMLDom->Parse(strXML) == XML_NO_ERROR)
		{
			return TRUE;
		}
		//SysFreeString(bstr);
		m_strError = _T("loadXML 失败");
		m_nError = 11;
		return FALSE;
	}
	
	catch (...)
	{
		m_strError = _T("捕获到非COM异常");
		m_nError = 12;
		return FALSE;
	}
}

int CTinyXMLParser::GetNodeText_len(LPCTSTR strNodeName,TCHAR *pDestBuffer,int nNode/*=0*/)
{
	try
	{
		XMLElement * Rootelement =  m_pXMLDom->RootElement();
		XMLElement * pFindElement = Rootelement->FirstChildElement(strNodeName);
		
		const char * pValue = pFindElement->FirstChild()->Value();
		int nLen = strlen(pValue);
		if(pDestBuffer==NULL){
			pDestBuffer = new char[nLen+1];
		}
		strcpy(pDestBuffer,pValue);
		pDestBuffer[nLen+1]='\0';
		return nLen;
		/*
		HRESULT hr;
		MSXML2::IXMLDOMNodeListPtr List = m_pXMLDom->getElementsByTagName((_bstr_t)strNodeName);
		long lNodesCount = 0;
		hr = List->get_length(&lNodesCount);
		if(SUCCEEDED(hr) && (nNode<lNodesCount ))
		{
			MSXML2::IXMLDOMNodePtr Node = List->item[nNode];
			int nLen = _tcslen(Node->text);
			if(pDestBuffer==NULL)
			{
				pDestBuffer = new TCHAR[nLen+1];
			}
			_tcscpy(pDestBuffer,Node->text);
			pDestBuffer[nLen+1]='\0';
			return nLen;
		}
		*/
		return 0;
	}
	catch (...)
	{
	}
	return 0;
}

LPCSTR CTinyXMLParser::GetNodeText(LPCTSTR strNodeName,TCHAR *pDestBuffer,int nNode/*=0*/)
{
	int nLen = GetNodeText_len(strNodeName,pDestBuffer,nNode);
	return nLen>0?pDestBuffer:_T("");
}
BOOL CTinyXMLParser::GetRoot(LPCTSTR &strName, LPCTSTR strPara)
{
	try
	{
		XMLElement * Rootelement =  m_pXMLDom->RootElement();
		strName = Rootelement->Name();
		/*
		MSXML2::IXMLDOMElementPtr pRoot = m_pXMLDom->documentElement;

		CString ss;
		ss = ((LPCTSTR)pRoot->nodeName);
		strName = (LPCTSTR)pRoot->nodeName;
		_variant_t var = pRoot->getAttribute(_T("Interface"));
		strPara = (LPCTSTR)var.bstrVal;
		*/
		return TRUE;
	}
	catch (...)
	{
		return FALSE;
	}
}

int CTinyXMLParser::GetRootName(TCHAR*pRootName)
{	
	XMLElement * Rootelement =  m_pXMLDom->RootElement();
	_tcscpy(pRootName , Rootelement->Name());
	return strlen(pRootName);
	/*
	_bstr_t pNodeName =m_pXMLDom->documentElement->nodeName; 
	int nLen = _tcslen(pNodeName);
	if(pRootName==NULL)
		pRootName=new TCHAR[nLen+1];
	_tcscpy(pRootName, pNodeName);
	pRootName[nLen+1]='\0';
	return nLen;*/
	return 0;
}