// TRS80Emu.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TRS80Emu.h"

#include "MainFrm.h"

#include "Trs80Core.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTRS80EmuApp

BEGIN_MESSAGE_MAP(CTRS80EmuApp, CWinApp)
	//{{AFX_MSG_MAP(CTRS80EmuApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_RUN1, OnAppRun1)
	ON_COMMAND(ID_APP_RUN4, OnAppRun4)
	ON_COMMAND(ID_APP_RUN2, OnAppRun2)
	ON_COMMAND(ID_APP_RUN3, OnAppRun3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTRS80EmuApp construction

CTRS80EmuApp::CTRS80EmuApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTRS80EmuApp object

CTRS80EmuApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTRS80EmuApp initialization

BOOL CTRS80EmuApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("AW Software"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// Resize Window (hasn't been shown yet), to match size of our font
	// Adjust Window size based on font to be 64 X 16 

	CRect rect;
	CSize size;
	CFont *oldfont;

	CClientDC dc(&pFrame->m_wndView);	// client DC

	oldfont = dc.SelectObject(&pFrame->m_wndView.m_font);
	size = dc.GetTextExtent("X",1);	// font is fixed, so any char will work
	if (oldfont)
		dc.SelectObject(oldfont);

	pFrame->GetWindowRect(rect);
	rect.SetRect(rect.left, 
			 	 rect.top, 
				 rect.left+size.cx*64+WIN_X_OFFSET*2, 
				 rect.top+size.cy*16+WIN_Y_OFFSET*2);

	//CalcWindowRect(frame_rect);  Doesnt work (too small), I Don't know why
	::AdjustWindowRectEx(rect, pFrame->GetStyle(), TRUE, pFrame->GetExStyle());
	pFrame->MoveWindow(rect); 

	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	// Run Germany 1941 on first boot up
	OnAppRun1();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTRS80EmuApp message handlers





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
		// No message handlers
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

// App command to run the dialog
void CTRS80EmuApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CTRS80EmuApp message handlers

void CTRS80EmuApp::OnAppRun1() 
{
	CDC *dc;
	CFont *oldfont;
	CChildView *win;

	extern unsigned char GermanyData[];
	extern int GermanyDataSize;

	win = &(((CMainFrame *)m_pMainWnd)->m_wndView);
	dc=win->GetDC();
	oldfont = dc->SelectObject(&(win->m_font));
	if (oldfont)
	{
		RunTrs80Code(dc, GermanyData, GermanyDataSize);
		dc->SelectObject(oldfont);
	}
	win->ReleaseDC(dc);
}


void CTRS80EmuApp::OnAppRun4() 
{
	CDC *dc;
	CFont *oldfont;
	CChildView *win;
	extern unsigned char QuestData[];
	extern int QuestDataSize;

	win = &(((CMainFrame *)m_pMainWnd)->m_wndView);
	dc=win->GetDC();
	oldfont = dc->SelectObject(&(win->m_font));
	if (oldfont)
	{
		RunTrs80Code(dc, QuestData, QuestDataSize);
		dc->SelectObject(oldfont);
	}
	win->ReleaseDC(dc);
}

void CTRS80EmuApp::OnAppRun2() 
{
	CDC *dc;
	CFont *oldfont;
	CChildView *win;
	extern unsigned char AsteroidData[];
	extern int AsteroidDataSize;

	win = &(((CMainFrame *)m_pMainWnd)->m_wndView);
	dc=win->GetDC();
	oldfont = dc->SelectObject(&(win->m_font));
	if (oldfont)
	{
		RunTrs80Code(dc, AsteroidData, AsteroidDataSize);
		dc->SelectObject(oldfont);
	}
	win->ReleaseDC(dc);
}

void CTRS80EmuApp::OnAppRun3() 
{
	CDC *dc;
	CFont *oldfont;
	CChildView *win;
	extern unsigned char TimeData[];
	extern int TimeDataSize;

	win = &(((CMainFrame *)m_pMainWnd)->m_wndView);
	dc=win->GetDC();
	oldfont = dc->SelectObject(&(win->m_font));
	if (oldfont)
	{
		RunTrs80Code(dc, TimeData, TimeDataSize);
//		RunTrs80Basic(dc);
		dc->SelectObject(oldfont);
	}
	win->ReleaseDC(dc);
}
