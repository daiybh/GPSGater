// GpsxConsoleDlg.h : header file
//

#if !defined(AFX_GPSXCONSOLEDLG_H__58CCE9C6_C716_4019_9B3A_CAFB2688BEEC__INCLUDED_)
#define AFX_GPSXCONSOLEDLG_H__58CCE9C6_C716_4019_9B3A_CAFB2688BEEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Importfile.h"


/////////////////////////////////////////////////////////////////////////////
// CGpsxConsoleDlg dialog

class CGpsxConsoleDlg : public CDialog
{
// Construction
public:
	CGpsxConsoleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGpsxConsoleDlg)
	enum { IDD = IDD_GPSXCONSOLE_DIALOG };
	CProgressCtrl	m_ctlRun;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGpsxConsoleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGpsxConsoleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBstart();
	afx_msg void OnBstop();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString		m_strInfo;
public:
	BOOL	m_bRun;
	CTime	m_tmStart;

	~CGpsxConsoleDlg();
	void	setStatus(BOOL bFlag);
	void	initConsole();
	void	startMonitor();

	unsigned __int64  m_intLastDataTime;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GPSXCONSOLEDLG_H__58CCE9C6_C716_4019_9B3A_CAFB2688BEEC__INCLUDED_)
