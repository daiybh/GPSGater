// GpsTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GpsTest.h"
#include "GpsTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpsTestDlg dialog

CGpsTestDlg::CGpsTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGpsTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGpsTestDlg)
	m_strInfo = _T("YH AN :15608891962 090506224204|0|0|0|0|0|80000000|00000000,00000000|0 E9D\x0d\x0a");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGpsTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGpsTestDlg)
	DDX_Control(pDX, IDC_CBCMMD, m_ctlCmmd);
	DDX_Text(pDX, IDC_EINFO, m_strInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGpsTestDlg, CDialog)
	//{{AFX_MSG_MAP(CGpsTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTEST, OnBtest)
	ON_BN_CLICKED(IDC_RUP, OnRup)
	ON_BN_CLICKED(IDC_RDOWN, OnRdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpsTestDlg message handlers

BOOL CGpsTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_ctlCmmd.SetCurSel(0);
	CheckRadioButton(IDC_RUP,IDC_RDOWN,IDC_RUP);
	
	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGpsTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGpsTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGpsTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void CGpsTestDlg::OnBtest() 
{
	GPSINFO gpsInfo;
	long nret = 0;

	memset(&gpsInfo,0x00,sizeof(GPSINFO));
	UpdateData();
	if(GetCheckedRadioButton(IDC_RUP,IDC_RDOWN)==IDC_RDOWN)
	{
		switch (m_ctlCmmd.GetCurSel())
		{
		case 0://停止呼叫
			gpsInfo.nMsgID = MSG_CALLSTOP;
			strcpy(gpsInfo.COMMADDR,"13888327795");
			strcpy(gpsInfo.SEQ,"3");
			break;
		case 1://链接维护响应
			gpsInfo.nMsgID = MSG_RES_KEEPMAITAIN;
			strcpy(gpsInfo.COMMADDR,"13888327795");
			strcpy(gpsInfo.SEQ,"3");
			break;
		case 2://离线响应
			gpsInfo.nMsgID = MSG_RES_LOGOFF;
			strcpy(gpsInfo.COMMADDR,"13888327795");
			strcpy(gpsInfo.SEQ,"3");
			break;
		case 3://登陆响应
			gpsInfo.nMsgID = MSG_RES_LOGIN;
			strcpy(gpsInfo.COMMADDR,"13888327795");
			strcpy(gpsInfo.SEQ,"3");
			break;
		case 4://连接维护
			gpsInfo.nMsgID = MSG_SETKEEPINTERVAL;
			strcpy(gpsInfo.COMMADDR,"13888327795");
			strcpy(gpsInfo.SEQ,"3");
			strcpy(gpsInfo.IntervalTime,"1E");
			break;
		case 5://设置GPS参数
			gpsInfo.nMsgID = MSG_SETGPS;
			strcpy(gpsInfo.COMMADDR,"13888327795");
			strcpy(gpsInfo.SEQ,"3");
			strcpy(gpsInfo.IntervalTime,"1E");
			strcpy(gpsInfo.IPV4,"192.168.1.1");
			strcpy(gpsInfo.PortNum,"80");
			strcpy(gpsInfo.APN,"CMNET");
			break;
		case 6://定时呼叫
			gpsInfo.nMsgID = MSG_CALLTIMER;
			strcpy(gpsInfo.COMMADDR,"13888327795");
			strcpy(gpsInfo.SEQ,"3");
			strcpy(gpsInfo.IntervalTime,"1E");
			strcpy(gpsInfo.comPressFlag,"0");			
			break;
		case 7://定次呼叫
			gpsInfo.nMsgID = MSG_CALLNTIMES;
			strcpy(gpsInfo.COMMADDR,"13888327795");
			strcpy(gpsInfo.SEQ,"3");
			strcpy(gpsInfo.IntervalTime,"A");
			strcpy(gpsInfo.comPressFlag,"0");	
			strcpy(gpsInfo.callTimes,"3");
			break;
		case 8://点名呼叫
			gpsInfo.nMsgID = MSG_CALLONETIME;
			strcpy(gpsInfo.COMMADDR,"13888327795");
			strcpy(gpsInfo.SEQ,"3");
			strcpy(gpsInfo.comPressFlag,"0");
			break;
		}
	}
	nret = SynchronGPSData(m_strInfo.GetBuffer(0),gpsInfo);
	SetDlgItemText(IDC_EINFO,m_strInfo);
}

void CGpsTestDlg::OnRup() 
{
	CheckRadioButton(IDC_RUP,IDC_RDOWN,IDC_RUP);
	SetDlgItemText(IDC_EINFO,"YH AN :15608891962 090506224204|0|0|0|0|0|80000000|00000000,00000000|0 E9D\x0d\x0a");
}

void CGpsTestDlg::OnRdown() 
{
	CheckRadioButton(IDC_RUP,IDC_RDOWN,IDC_RDOWN);
	SetDlgItemText(IDC_EINFO,"");
}
