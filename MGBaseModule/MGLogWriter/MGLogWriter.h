// MGLogWriter.h : main header file for the MGLOGWRITER DLL
//

#if !defined(AFX_MGLOGWRITER_H__7B55151F_2434_4AF5_8B32_AADA53AA2FF2__INCLUDED_)
#define AFX_MGLOGWRITER_H__7B55151F_2434_4AF5_8B32_AADA53AA2FF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMGLogWriterApp
// See MGLogWriter.cpp for the implementation of this class
//

class CMGLogWriterApp : public CWinApp
{
public:
	CMGLogWriterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMGLogWriterApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMGLogWriterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MGLOGWRITER_H__7B55151F_2434_4AF5_8B32_AADA53AA2FF2__INCLUDED_)
