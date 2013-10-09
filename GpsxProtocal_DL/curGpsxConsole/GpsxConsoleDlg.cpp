// GpsxConsoleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GpsxConsole.h"
#include "GpsxConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

typedef long (__stdcall *LoadLibrary_start)( unsigned __int64 &iLastDataTime,DWORD &dwMapCount);
typedef long (__stdcall *LoadLibrary_stop)();
LoadLibrary_start llstart=NULL;
LoadLibrary_stop  llstop=NULL;
HMODULE m_hDevxGps=NULL;
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
// CGpsxConsoleDlg dialog

CGpsxConsoleDlg::CGpsxConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGpsxConsoleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGpsxConsoleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CGpsxConsoleDlg::~CGpsxConsoleDlg()
{	
}

void CGpsxConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGpsxConsoleDlg)
	DDX_Control(pDX, IDC_PROGRUN, m_ctlRun);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGpsxConsoleDlg, CDialog)
	//{{AFX_MSG_MAP(CGpsxConsoleDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BSTART, OnBstart)
	ON_BN_CLICKED(IDC_BSTOP, OnBstop)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpsxConsoleDlg message handlers

BOOL CGpsxConsoleDlg::OnInitDialog()
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
	initConsole();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGpsxConsoleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGpsxConsoleDlg::OnPaint() 
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
HCURSOR CGpsxConsoleDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGpsxConsoleDlg::OnBstart() 
{
	// TODO: Add your control notification handler code heres	
	startMonitor();
}

void CGpsxConsoleDlg::OnBstop() 
{
	// TODO: Add your control notification handler code here
	if(llstop)llstop();
	m_bRun = false;
	setStatus(m_bRun);
}

void CGpsxConsoleDlg::setStatus( BOOL bFlag )
{
	UINT	nTmRun = 1;
	CString	strTm = "";

	GetDlgItem(IDC_BSTART)->EnableWindow(!bFlag);
	GetDlgItem(IDC_BSTOP)->EnableWindow(bFlag);

	
	if(bFlag)
	{
		m_tmStart = CTime::GetCurrentTime();
		strTm.Format("起始时间:%d年%d月%d日%d时%d分%d秒",m_tmStart.GetYear(),m_tmStart.GetMonth(),m_tmStart.GetDay(),m_tmStart.GetHour(),m_tmStart.GetMinute(),m_tmStart.GetSecond());
		SetDlgItemText(IDC_SSTARTTIME,strTm);
		SetTimer(nTmRun,1000,NULL);
	}
	else 
	{
		KillTimer(nTmRun);
		m_ctlRun.SetPos(0);
	}
}


void CGpsxConsoleDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);

	CTime tt(m_intLastDataTime);
	CString ss ;
	ss.Format("lastData_%s--%I64d--%d",tt.Format("%Y-%m-%d %H:%M:%S"),m_intLastDataTime,m_dwCount);
//	OutputDebugString(ss);
	SetWindowText(ss);
	CTimeSpan	nTmSpan = m_tmStart - CTime::GetCurrentTime();	
	m_strInfo.Format("运行时间:%d天%d小时%d分%d秒",nTmSpan.GetDays(),nTmSpan.GetHours(),nTmSpan.GetMinutes(),nTmSpan.GetSeconds());
	
	
	//m_strInfo.Format("%I64d",m_intLastDataTime);
	SetDlgItemText(IDC_EINFO,m_strInfo);
	
	m_ctlRun.StepIt();
}

void CGpsxConsoleDlg::initConsole()
{
	m_ctlRun.SetRange(1,60);
	m_ctlRun.SetStep(6);
	startMonitor(); 	
}

void CGpsxConsoleDlg::startMonitor()
{
	m_intLastDataTime = 0;
	m_dwCount = 0;
	if(m_hDevxGps==NULL)
	{
		m_hDevxGps= LoadLibrary("DevxGps.dll");
	}
	if(m_hDevxGps==NULL)
	{
		CString strTm;
		strTm.Format("LoadLibrary devxGps.dll failed.%d",GetLastError());
		SetDlgItemText(IDC_EINFO,strTm);
		return;
	}
	if(llstart==NULL)
		llstart = (LoadLibrary_start) GetProcAddress(m_hDevxGps, "start" );
	if(llstop ==NULL)
		llstop = (LoadLibrary_stop) GetProcAddress(m_hDevxGps, "stop" );
	if(llstart==NULL || llstop==NULL)
	{
		CString strTm;
		strTm.Format("GetProcAddress from devxGps.dll failed.%d.llstart=%x llstop=%x",GetLastError(),llstart,llstop);
		SetDlgItemText(IDC_EINFO,strTm);

		FreeLibrary(m_hDevxGps);
		m_hDevxGps = NULL;
		return;
	}

	llstart(m_intLastDataTime,m_dwCount);
	m_bRun = true;
	setStatus(m_bRun);	
}
