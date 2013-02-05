// GpsxConsole.h : main header file for the GPSXCONSOLE application
//

#if !defined(AFX_GPSXCONSOLE_H__BA472566_78AA_4C41_A530_8C7D27CBB0A8__INCLUDED_)
#define AFX_GPSXCONSOLE_H__BA472566_78AA_4C41_A530_8C7D27CBB0A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGpsxConsoleApp:
// See GpsxConsole.cpp for the implementation of this class
//

class CGpsxConsoleApp : public CWinApp
{
public:
	CGpsxConsoleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGpsxConsoleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGpsxConsoleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GPSXCONSOLE_H__BA472566_78AA_4C41_A530_8C7D27CBB0A8__INCLUDED_)
