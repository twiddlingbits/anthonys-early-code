// LibraryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibraryDlg.h"
#include "MainApp.h"
#include "MainFrm.h"
#include "WinTrs80Thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLibraryDlg dialog


CLibraryDlg::CLibraryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLibraryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLibraryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLibraryDlg)
	DDX_Control(pDX, IDC_TREE1, m_tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLibraryDlg, CDialog)
	//{{AFX_MSG_MAP(CLibraryDlg)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnDblclkTree1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLibraryDlg message handlers

void CLibraryDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	DestroyWindow();
	CDialog::OnClose();
}

BOOL CLibraryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	HTREEITEM item;
	extern LibraryBranchData* LibraryDataRoot;

	for (LibraryBranchData* root_level = LibraryDataRoot; root_level; root_level=root_level->next)
	{
		item = m_tree.InsertItem(TVIF_TEXT , root_level->name, 0, 0, 0, 0, 0, TVI_ROOT, TVI_LAST);
		for (LibraryNodeData* node = root_level->branch; node; node=node->next)
		{
		    m_tree.InsertItem(TVIF_PARAM|TVIF_TEXT , node->name, 0, 0, 0, 0, (LPARAM)node, item, TVI_LAST);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CLibraryDlg::OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CChildView *win;
	LibraryNodeData* node;

	CMainApp *theApp=(CMainApp*)AfxGetApp();

	win = &(((CMainFrame *)theApp->m_pMainWnd)->m_wndView);

	if (win->m_trs80_thread)
	{
		win->m_trs80_thread->KillThreadAsync();
		WaitForSingleObject(win->m_trs80_thread->m_hThread, INFINITE);
		// delete m_trs80_thread; CWindowThread is automaticly deleteing the object
	}
	
	node = (LibraryNodeData*)m_tree.GetItemData(m_tree.GetSelectedItem());

	win->m_trs80_config.m_syssoft_level		=node->m_syssoft_level;	
	win->m_trs80_config.m_rom_type			=node->m_rom_type;		
	win->m_trs80_config.m_dos_type			=node->m_dos_type;		
	win->m_trs80_config.m_boot_mode			=node->m_boot_mode;				
	win->m_trs80_config.m_autorun_mode		=node->m_autorun_mode;			
	win->m_trs80_config.m_trs80_clk_mhz		=node->m_trs80_clk_mhz;		
	win->m_trs80_config.m_lower_case_mod_installed=node->m_lower_case_mod_installed;
	win->m_trs80_config.m_run_real_speed	=node->m_run_real_speed;			
	win->m_trs80_config.m_image				=node->m_image;			
	win->m_trs80_config.m_size				=node->m_size;							
	
	win->m_trs80_thread = new CWinTrs80Thread(win, &(win->m_trs80_thread), &win->m_trs80_config);
	if (win->m_trs80_thread)
		win->m_trs80_thread->CreateThread(0);

	*pResult = 0;
}
