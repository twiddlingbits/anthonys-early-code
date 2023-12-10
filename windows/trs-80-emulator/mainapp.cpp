// MainApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MainApp.h"

#include "MainFrm.h"

#include "WinTrs80Thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainApp

BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
	//{{AFX_MSG_MAP(CMainApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_RUN1, OnAppRun1)
	ON_COMMAND(ID_APP_RUN4, OnAppRun4)
	ON_COMMAND(ID_APP_RUN2, OnAppRun2)
	ON_COMMAND(ID_APP_RUN3, OnAppRun3)
	ON_COMMAND(ID_APP_VIEW_LIB, OnAppViewLib)
	ON_UPDATE_COMMAND_UI(ID_APP_VIEW_LIB, OnUpdateAppViewLib)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainApp construction

CMainApp::CMainApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMainApp object

CMainApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainApp initialization

BOOL CMainApp::InitInstance()
{
	DDSURFACEDESC       ddsd; 
	LPDIRECTDRAWCLIPPER pcClipper;
    HRESULT             ddrval; 


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
	SetRegistryKey(_T("TRS80 EMU 3"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE);

	// Resize Window (hasn't been shown yet), to match size of our font
	// Adjust Window size based on font to be 64 X 16 

	CRect rect;
	CSize size;

	CClientDC dc(&pFrame->m_wndView);	// client DC

	CWinTrs80::GetDisplaySize(&dc, &size);  // width & height, in pixels, of draw area (no framing)

	pFrame->m_wndView.GetWindowRect(rect);	// get entire window size (display area + boarders)
	rect.SetRect(rect.left, 
			 	 rect.top, 
				 rect.left+size.cx, 
				 rect.top+size.cy);

	// Remember the client wndow (display) coordinates relative to the screen for direct draw use
	pFrame->m_wndView.m_direct_draw_into_rect = rect;

	//CalcWindowRect(frame_rect);  Doesnt work (too small), I Don't know why
	::AdjustWindowRectEx(rect, pFrame->GetStyle(), TRUE, pFrame->GetExStyle());
	pFrame->MoveWindow(rect, FALSE); 

	// Accept drag & drop files in client window
	pFrame->m_wndView.DragAcceptFiles();
	
	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

    /* 
     * Set up direct draw objects
     */ 
    ddrval = DirectDrawCreate( NULL, &pFrame->m_wndView.lpDD, NULL ); 
    if( ddrval == DD_OK ) 
    { 

		// Get exclusive mode 
		ddrval = pFrame->m_wndView.lpDD->SetCooperativeLevel( pFrame->m_wndView.m_hWnd, DDSCL_NORMAL); 
		if(ddrval == DD_OK ) 
		{ 
			 
			// Create the primary surface
			ZeroMemory( &ddsd, sizeof( ddsd ) );
			ddsd.dwSize         = sizeof( ddsd );
			ddsd.dwFlags        = DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			ddrval = pFrame->m_wndView.lpDD->CreateSurface( &ddsd, &pFrame->m_wndView.lpDDSPrimary, NULL ); 
			if(ddrval == DD_OK ) 
			{
				// Create the backbuffer surface
				ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
				ddsd.dwWidth        = pFrame->m_wndView.m_direct_draw_into_rect.Width();
				ddsd.dwHeight       = pFrame->m_wndView.m_direct_draw_into_rect.Height();

				ddrval = pFrame->m_wndView.lpDD->CreateSurface( &ddsd, &pFrame->m_wndView.lpDDSBack, NULL ); 
				if(ddrval == DD_OK ) 
				{
					// Erase the background
					DDBLTFX ddbltfx;
					ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
					ddbltfx.dwSize      = sizeof(ddbltfx);
					ddbltfx.dwFillColor = RGB(0,0,0);
					pFrame->m_wndView.lpDDSBack->Blt( NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );


					ddrval = pFrame->m_wndView.lpDD->CreateClipper( 0, &pcClipper, NULL ); 
					if(ddrval == DD_OK ) 
					{

						ddrval = pcClipper->SetHWnd( 0, pFrame->m_wndView.m_hWnd );
						if(ddrval == DD_OK ) 
						{
							ddrval = pFrame->m_wndView.lpDDSPrimary->SetClipper( pcClipper );
						}
						pcClipper->Release();
					}
				}
			}

		} 
    } 
 
	if(ddrval != DD_OK ) 
	{
		char buf[256];

		wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval ); 
		TRACE0(buf);

		if( pFrame->m_wndView.lpDD != NULL ) 
		{ 
			if( pFrame->m_wndView.lpDDSPrimary != NULL ) 
			{ 
				pFrame->m_wndView.lpDDSPrimary->Release(); 
				pFrame->m_wndView.lpDDSPrimary = NULL; 
			} 
			pFrame->m_wndView.lpDD->Release(); 
			pFrame->m_wndView.lpDD = NULL; 
		} 
		return FALSE;
	}


	return TRUE;

}

/////////////////////////////////////////////////////////////////////////////
// CMainApp message handlers





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
void CMainApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMainApp message handlers

void CMainApp::OnAppRun1() 
{
	CChildView *win;
	extern unsigned char GermanyData[];
	extern int GermanyDataSize;

	win = &(((CMainFrame *)m_pMainWnd)->m_wndView);

	if (win->m_trs80_thread)
	{
		win->m_trs80_thread->KillThreadAsync();
		WaitForSingleObject(win->m_trs80_thread->m_hThread, INFINITE);
		// delete m_trs80_thread; CWindowThread is automaticly deleteing the object
	}
	

	win->m_trs80_config.m_syssoft_level = TRS80_SYSSOFT_ROM;
	win->m_trs80_config.m_boot_mode = TRS80_BOOTMODE_AUTORUN;
	win->m_trs80_config.m_autorun_mode = TRS80_AUTORUNMODE_CMD;
	win->m_trs80_config.m_image = GermanyData;
	win->m_trs80_config.m_size = GermanyDataSize;
	
	win->m_trs80_thread = new CWinTrs80Thread(win, &(win->m_trs80_thread), &win->m_trs80_config);
	if (win->m_trs80_thread)
		win->m_trs80_thread->CreateThread(0);
}

/////////////////////////////////////////////////////////////////////////////

void CMainApp::OnAppRun2() 
{

	CChildView *win;
	extern unsigned char AsteroidData[];
	extern int AsteroidDataSize;


	win = &(((CMainFrame *)m_pMainWnd)->m_wndView);

	if (win->m_trs80_thread)
	{
		win->m_trs80_thread->KillThreadAsync();
		WaitForSingleObject(win->m_trs80_thread->m_hThread, INFINITE);
		// delete m_trs80_thread; CWindowThread is automaticly deleteing the object
	}
	
	win->m_trs80_config.m_syssoft_level = TRS80_SYSSOFT_ROM;
	win->m_trs80_config.m_boot_mode = TRS80_BOOTMODE_AUTORUN;
	win->m_trs80_config.m_autorun_mode = TRS80_AUTORUNMODE_CMD;
	win->m_trs80_config.m_image = AsteroidData;
	win->m_trs80_config.m_size = AsteroidDataSize;
	
	win->m_trs80_thread = new CWinTrs80Thread(win, &(win->m_trs80_thread), &win->m_trs80_config);
	if (win->m_trs80_thread)
		win->m_trs80_thread->CreateThread(0);

	return;
}

/////////////////////////////////////////////////////////////////////////////

void CMainApp::OnAppRun3() 
{
	CChildView *win;
	extern unsigned char TimeData[];
	extern int TimeDataSize;

	win = &(((CMainFrame *)m_pMainWnd)->m_wndView);

	if (win->m_trs80_thread)
	{
		win->m_trs80_thread->KillThreadAsync();
		WaitForSingleObject(win->m_trs80_thread->m_hThread, INFINITE);
		// delete m_trs80_thread; CWindowThread is automaticly deleteing the object
	}

	win->m_trs80_config.m_syssoft_level = TRS80_SYSSOFT_DOS;
	win->m_trs80_config.m_boot_mode = TRS80_BOOTMODE_NORMAL;
	
	win->m_trs80_thread = new CWinTrs80Thread(win, &(win->m_trs80_thread), &win->m_trs80_config);
	if (win->m_trs80_thread == NULL)
		return;

	ASSERT_VALID(win->m_trs80_thread);

	if (win->m_trs80_thread->CreateThread(0)==NULL)
	{
		TRACE0("Error creating thread");
		return;
	}

	return;

}

/////////////////////////////////////////////////////////////////////////////

void CMainApp::OnAppRun4() 
{
	CChildView *win;
	extern unsigned char QuestData[];
	extern int QuestDataSize;

	win = &(((CMainFrame *)m_pMainWnd)->m_wndView);

	if (win->m_trs80_thread)
	{
		win->m_trs80_thread->KillThreadAsync();
		WaitForSingleObject(win->m_trs80_thread->m_hThread, INFINITE);
		// delete m_trs80_thread; CWindowThread is automaticly deleteing the object
	}	
	
	win->m_trs80_config.m_syssoft_level = TRS80_SYSSOFT_ROM;
	win->m_trs80_config.m_boot_mode = TRS80_BOOTMODE_AUTORUN;
	win->m_trs80_config.m_autorun_mode = TRS80_AUTORUNMODE_CMD;
	win->m_trs80_config.m_image = QuestData;
	win->m_trs80_config.m_size = QuestDataSize;
	
	win->m_trs80_thread = new CWinTrs80Thread(win, &(win->m_trs80_thread), &win->m_trs80_config);
	if (win->m_trs80_thread)
		win->m_trs80_thread->CreateThread(0);

	return;

}

/////////////////////////////////////////////////////////////////////////////
void CMainApp::OnAppViewLib() 
{
	if (m_library_dlg.GetSafeHwnd()==NULL)
		m_library_dlg.Create(CLibraryDlg::IDD);
	else
		m_library_dlg.DestroyWindow();
}

void CMainApp::OnUpdateAppViewLib(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(!(m_library_dlg.GetSafeHwnd()==0));
	
}
