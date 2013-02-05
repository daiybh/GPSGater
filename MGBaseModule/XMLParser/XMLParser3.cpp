    // Parser.cpp: implementation of the CXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLParser3.h"
#include <ATLBASE.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL I_XMLParser::CreateInstance(I_XMLParser **ppIXMLParser)
{
	if(ppIXMLParser==NULL)
		return FALSE;
	*ppIXMLParser = (I_XMLParser *)new CXMLParser3();
	if(NULL == *ppIXMLParser)return FALSE;
	return TRUE;
}



CXMLParser3::CXMLParser3()
{
	//m_nError = 0;
}

CXMLParser3::~CXMLParser3()
{
}

BOOL CXMLParser3::Create()
{
	try
	{
		HRESULT hr = m_pXMLDom.CreateInstance(__uuidof(DOMDocument40));
		if(FAILED(hr))
			return FALSE;
		
		m_pXMLDom->async = VARIANT_FALSE;
		m_pXMLDom->validateOnParse = VARIANT_FALSE;
		m_pXMLDom->resolveExternals = VARIANT_FALSE;
		return TRUE;
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
}

BOOL CXMLParser3::Parse(LPCTSTR strXML)
{
	try
	{
		//BSTR bstr = strXML.AllocSysString();
		if(m_pXMLDom->loadXML((_bstr_t)strXML) == VARIANT_TRUE)
		{
			BSTR bstrNodesName = m_pXMLDom->documentElement->nodeName +  _T("/*");
			m_pNodeList = m_pXMLDom->selectNodes(bstrNodesName);
			SysFreeString(bstrNodesName);
		//	SysFreeString(bstr);
			return TRUE;
		}
		//SysFreeString(bstr);
		m_strError = _T("loadXML 失败");
		m_nError = 11;
		return FALSE;
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return FALSE;
	}
	catch (...)
	{
		m_strError = _T("捕获到非COM异常");
		m_nError = 12;
		return FALSE;
	}
}

int CXMLParser3::GetNodeText_len(LPCTSTR strNodeName,TCHAR *pDestBuffer,int nNode/*=0*/)
{
	try
	{
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
		return 0;
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return 0;
	}
	catch (...)
	{
	}
	return 0;
}

LPCSTR CXMLParser3::GetNodeText(LPCTSTR strNodeName,TCHAR *pDestBuffer,int nNode/*=0*/)
{
	int nLen = GetNodeText_len(strNodeName,pDestBuffer,nNode);
	return nLen>0?pDestBuffer:_T("");
}
BOOL CXMLParser3::GetRoot(LPCTSTR &strName, LPCTSTR strPara)
{
	try
	{
		MSXML2::IXMLDOMElementPtr pRoot = m_pXMLDom->documentElement;

		CString ss;
		ss = ((LPCTSTR)pRoot->nodeName);
		strName = (LPCTSTR)pRoot->nodeName;
		_variant_t var = pRoot->getAttribute(_T("Interface"));
		strPara = (LPCTSTR)var.bstrVal;
		return TRUE;
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
}

int CXMLParser3::GetRootName(TCHAR*pRootName)
{	
	_bstr_t pNodeName =m_pXMLDom->documentElement->nodeName; 
	int nLen = _tcslen(pNodeName);
	if(pRootName==NULL)
		pRootName=new TCHAR[nLen+1];
	_tcscpy(pRootName, pNodeName);
	pRootName[nLen+1]='\0';
	return nLen;
}
/////////////////////////////////////////////////////////////
/*

CString CXMLParser3::GetNodeText(CString strNodeName,int nNode)
{
	try
	{
		HRESULT hr;
		MSXML2::IXMLDOMNodeListPtr List = m_pXMLDom->getElementsByTagName((_bstr_t)strNodeName);
		long lNodesCount = 0;
		hr = List->get_length(&lNodesCount);
		if(!SUCCEEDED(hr))
		{
			return _T("");
		}
		if(nNode<lNodesCount )
		{
			MSXML2::IXMLDOMNodePtr Node = List->item[nNode];
			return (LPCTSTR)Node->text;
		}
		return _T("");
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return _T("");
	}
	catch (...)
	{
		return _T("");
	}
}

CString CXMLParser3::GetNodeText(int nIndex)
{
	if(nIndex >= m_pNodeList->length)
		return _T("");
	return (LPCTSTR)m_pNodeList->item[nIndex]->text;
}

int CXMLParser3::GetNodeCount() const
{
	return m_pNodeList->length;
}

CString CXMLParser3::FilterText(LPCTSTR lpszText)
{
	ASSERT(lpszText);
	CString strText(lpszText);

	for(int i = 0; i < strText.GetLength(); i++)
	{
		switch(strText.GetAt(i))
		{
		case '\'':
			strText.Delete(i);
			strText.Insert(i, _T("&apos;"));
			i += 5;
			break;
		case '\"':
			strText.Delete(i);
			strText.Insert(i, _T("&quot;"));
			i += 5;
			break;
		case '<':
			strText.Delete(i);
			strText.Insert(i, _T("&lt;"));
			i += 3;
			break;
		case '>':
			strText.Delete(i);
			strText.Insert(i, _T("&gt;"));
			i += 3;
			break;
		case '&':
			strText.Delete(i);
			strText.Insert(i, _T("&amp;"));
			i += 4;
			break;
		default:
			break;
		}
	}

	return strText;
}
CString CXMLParser3::GetNodeAttribute(CString strAttributeName)
{
	try
	{
		MSXML2::IXMLDOMElementPtr pRoot = m_pXMLDom->documentElement;
		_bstr_t bstrt(strAttributeName);
		_variant_t var =pRoot->getAttribute(bstrt);
		return (LPCTSTR)var.bstrVal;
	}catch(...)
	{
		return _T("");
	}
	return _T("");
}
CString CXMLParser3::GetNodeAttribute(CString strNodeName, CString strAttributeName)
{
	try
	{
		m_pNodeList->reset();
		MSXML2::IXMLDOMNodePtr pNode = NULL;
		CString szTemp;
		MSXML2::IXMLDOMNodePtr pAttributeNode = NULL;
		while(pNode = m_pNodeList->nextNode())
		{
			szTemp.Format(_T("%s"),(LPCTSTR)pNode->nodeName);
			if(strNodeName ==szTemp )
			{				
				MSXML2::IXMLDOMNamedNodeMapPtr pMap;
				pMap=pNode->Getattributes();
				long lLength=pMap->Getlength();
				long lIndex=0;
				for(lIndex=0;lIndex<lLength;lIndex++)
				{
					pAttributeNode=pMap->Getitem(lIndex);
					szTemp.Format(_T("%s"),(LPCTSTR)pAttributeNode->nodeName);
					if(strAttributeName==szTemp)
					{
						return (LPCTSTR)pAttributeNode->text;
					}
				}
			}
		}
		return _T("");
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return _T("");
	}
	catch (...)
	{
		return _T("");
	}
}




int CXMLParser3::GetLastError(CString &strError)
{
	strError = m_strError;
	return m_nError;
}
CString CXMLParser3::GetNodeXML(CString strNodeName,int nIndex)
{
	try
	{
		HRESULT hr;
		MSXML2::IXMLDOMNodeListPtr List = m_pXMLDom->getElementsByTagName((_bstr_t)strNodeName);
		long lNodesCount = 0;
		hr = List->get_length(&lNodesCount);
		if(!SUCCEEDED(hr))
		{
			return _T("");
		}
		if(lNodesCount<nIndex)
		{
			return _T("");
		}

		MSXML2::IXMLDOMNodePtr Node = List->item[nIndex];
		return (LPCTSTR)Node->xml;
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return _T("");
	}
	catch (...)
	{
		return _T("");
	}
	return _T("");
}
CString CXMLParser3::GetNodeXML(CString strNodeName)
{
	try
	{
		HRESULT hr;
		MSXML2::IXMLDOMNodeListPtr List = m_pXMLDom->getElementsByTagName((_bstr_t)strNodeName);
		long lNodesCount = 0;
		hr = List->get_length(&lNodesCount);
		if(!SUCCEEDED(hr))
		{
			return _T("");
		}
		if(lNodesCount == 1)
		{
			MSXML2::IXMLDOMNodePtr Node = List->item[0];
			return (LPCTSTR)Node->xml;
		}
		else if(lNodesCount>1)
		{
			for (int i=0;i<lNodesCount;i++)
			{
				MSXML2::IXMLDOMNodePtr Node = List->item[i];
				CString str = (LPCTSTR)Node->xml;
				AfxMessageBox(str);
			}
		}
		return _T("");
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return _T("");
	}
	catch (...)
	{
		return _T("");
	}
}

CString CXMLParser3::GetXML()
{
	try
	{
		_bstr_t bstr= m_pXMLDom->xml;
		return (TCHAR*)bstr;
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return _T("");
	}
	catch (...)
	{
		return _T("");
	}
}

BOOL CXMLParser3::SetNodeText(CString strNodeName, CString strText)
{
	try
	{
		HRESULT hr;
		MSXML2::IXMLDOMNodeListPtr List = m_pXMLDom->getElementsByTagName((_bstr_t)strNodeName);
		long lNodesCount = 0;
		hr = List->get_length(&lNodesCount);
		if(!SUCCEEDED(hr))
		{
			return FALSE;
		}
		if(lNodesCount == 1)
		{
			MSXML2::IXMLDOMNodePtr Node = List->item[0];
			_bstr_t bstr = strText.AllocSysString();
			Node->text = bstr;
			SysFreeString(bstr);
			return TRUE;
		}
		return FALSE;
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
}
BOOL CXMLParser3::GetNodesXML(CString strNodeName,std::vector<CString>& vXML)
{
	vXML.clear();
	
	try
	{
		HRESULT hr;
		MSXML2::IXMLDOMNodeListPtr List = m_pXMLDom->getElementsByTagName((_bstr_t)strNodeName);
		long lNodesCount = 0;
		hr = List->get_length(&lNodesCount);
		if(!SUCCEEDED(hr))
		{
			return FALSE;
		}
		for(long i=0;i<lNodesCount;i++)
		{
			MSXML2::IXMLDOMNodePtr Node = List->item[i];
			vXML.push_back((LPCTSTR)Node->xml);
		}
		return TRUE;
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}	
}
BOOL CXMLParser3::GetNodesText(CString strNodeName, std::vector<CString>& vText)
{
	vText.clear();

	try
	{
		HRESULT hr;
		MSXML2::IXMLDOMNodeListPtr List = m_pXMLDom->getElementsByTagName((_bstr_t)strNodeName);
		long lNodesCount = 0;
		hr = List->get_length(&lNodesCount);
		if(!SUCCEEDED(hr))
		{
			return FALSE;
		}
		for(long i=0;i<lNodesCount;i++)
		{
			MSXML2::IXMLDOMNodePtr Node = List->item[i];
			vText.push_back((LPCTSTR)Node->text);
		}
		return TRUE;
	}
	catch (_com_error &e)
	{
		m_nError = e.Error();
		m_strError = e.ErrorMessage();
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}	
}
CString CXMLParser3::GetSecondLayerNodeName(int nIndex, CString strNodeName)
{
	if(nIndex >= m_pNodeList->length)
		return _T("");
	strNodeName.MakeLower();
	MSXML2::IXMLDOMNodePtr pNode = m_pNodeList->item[nIndex];
	MSXML2::IXMLDOMNodePtr pSecondNode=pNode->GetfirstChild();
	CString szTemp;
	while(pSecondNode)
	{		
		szTemp.Format(_T("%s"),(LPCTSTR)pSecondNode->nodeName);
		szTemp.MakeLower();
		if(strNodeName ==szTemp )
		{	
			return (LPCTSTR)pSecondNode->text;
		}
		pSecondNode=pSecondNode->GetnextSibling();
		
	}
	return _T("");
}

/**/