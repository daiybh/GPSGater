// GpsTest.h : main header file for the GPSTEST application
//

#if !defined(AFX_GPSTEST_H__6DDA9634_5988_4D1F_BD9E_C6BCAEA8B548__INCLUDED_)
#define AFX_GPSTEST_H__6DDA9634_5988_4D1F_BD9E_C6BCAEA8B548__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGpsTestApp:
// See GpsTest.cpp for the implementation of this class
//

class CGpsTestApp : public CWinApp
{
public:
	CGpsTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGpsTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGpsTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GPSTEST_H__6DDA9634_5988_4D1F_BD9E_C6BCAEA8B548__INCLUDED_)
