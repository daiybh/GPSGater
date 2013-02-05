// GpsTestDlg.h : header file
//

#if !defined(AFX_GPSTESTDLG_H__F0148B18_FAF6_4161_AA54_F69033BBABF3__INCLUDED_)
#define AFX_GPSTESTDLG_H__F0148B18_FAF6_4161_AA54_F69033BBABF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GpsData.h"
#include "DevxGps.h"
/////////////////////////////////////////////////////////////////////////////
// CGpsTestDlg dialog


class CGpsTestDlg : public CDialog
{
// Construction
public:
	CGpsTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGpsTestDlg)
	enum { IDD = IDD_GPSTEST_DIALOG };
	CComboBox	m_ctlCmmd;
	CString	m_strInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGpsTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGpsTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtest();
	afx_msg void OnRup();
	afx_msg void OnRdown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GPSTESTDLG_H__F0148B18_FAF6_4161_AA54_F69033BBABF3__INCLUDED_)
