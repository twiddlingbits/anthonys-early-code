// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include <Afxwin.h>
#include "MainApp.h"
#include "ChildView.h"

#include "WinTrs80Thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#if 0		// Code to draw a splitter bar out of CSplitterWnd::OnDrawSplitter()

	case splitBar:
		if (!afxData.bWin4)
		{
			pDC->Draw3dRect(rect, afxData.clrBtnHilite, afxData.clrBtnShadow);
			rect.InflateRect(-CX_BORDER, -CY_BORDER);
		}
		break;

	default:
		ASSERT(FALSE);  // unknown splitter type
	}

	// fill the middle
	COLORREF clr = afxData.clrBtnFace;
	pDC->FillSolidRect(rect, clr);
}

#endif 


/////////////////////////////////////////////////////////////////////////////
// CChildView


CChildView::CChildView()
{
	m_trs80_thread = NULL;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_CANCELMODE()
	ON_WM_CREATE()
	ON_WM_CAPTURECHANGED()
	ON_WM_CHAR()
	ON_WM_CONTEXTMENU()
	ON_WM_DROPFILES()
	ON_WM_COPYDATA()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_KEYUP()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;

	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (m_trs80_thread)
		m_trs80_thread->Paint(&dc);

	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnCancelMode() 
{
	CWnd ::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CChildView::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CWnd ::OnCaptureChanged(pWnd);
}

void CChildView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	
}

void CChildView::OnDropFiles(HDROP hDropInfo) 
{
	// should detect and warn if multiple files dropped in future...
	TCHAR szFileName[_MAX_PATH];
	::DragQueryFile(hDropInfo, 0, szFileName, _MAX_PATH);
	::DragFinish(hDropInfo);

	FILE* fp = fopen(szFileName, "rb");
	if (fp==NULL)
	{
		::AfxMessageBox("Unable to open file");
		return;
	}

	unsigned char* buf = m_trs80_config.m_dsk_image;	// use as disk image as temp storage

	size_t size = fread(buf, sizeof (buf[0]), sizeof (m_trs80_config.m_dsk_image), fp);

	if (size == 0)
	{
		::AfxMessageBox("Error reading file");
		return;
	}

	if (size >= sizeof(m_trs80_config.m_dsk_image))
	{
		::AfxMessageBox("Invalid File type -- Must be CMD, BAS, or DSK (JV1 only)");
		return;
	}
	
	fclose(fp);

	if (m_trs80_thread)
	{
		m_trs80_thread->KillThreadAsync();
		WaitForSingleObject(m_trs80_thread->m_hThread, INFINITE);
		// delete m_trs80_thread; CWindowThread is automaticly deleteing the object
	}
	
	if (stricmp(szFileName+strlen(szFileName)-4, ".dsk")==0)
	{
		m_trs80_config.m_syssoft_level = TRS80_SYSSOFT_DOS;	
		m_trs80_config.m_dos_type = 	TRS80_DOSTYPE_NEW_A;			
		m_trs80_config.m_boot_mode = TRS80_BOOTMODE_NORMAL;	
		m_trs80_config.m_load_drive1 = TRUE;
		
		// memcpy(m_trs80_config.m_dsk_image, buf, size); Already there

		if (size % 256 !=0)
		{
			::AfxMessageBox("Invalid File type -- only JV1 .dsk files supported");
			return;
		}


	}
	else
	{
		m_trs80_config.m_syssoft_level = TRS80_SYSSOFT_ROM;
		m_trs80_config.m_boot_mode = TRS80_BOOTMODE_AUTORUN;
		m_trs80_config.m_image = buf;
		m_trs80_config.m_size = size;

		if (stricmp(szFileName+strlen(szFileName)-4, ".bas")==0)
			m_trs80_config.m_autorun_mode = TRS80_AUTORUNMODE_ROMBASIC;
		else
			m_trs80_config.m_autorun_mode = TRS80_AUTORUNMODE_CMD;
	}


	m_trs80_thread = new CWinTrs80Thread(this, &m_trs80_thread, &m_trs80_config);
	if (m_trs80_thread)
		m_trs80_thread->CreateThread(0);	

	return;
}

BOOL CChildView::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CWnd ::OnCopyData(pWnd, pCopyDataStruct);
}

void CChildView::OnDestroy() 
{
	CWnd ::OnDestroy();
	
	if (m_trs80_thread)
	{
		m_trs80_thread->KillThreadAsync();
		WaitForSingleObject(m_trs80_thread->m_hThread, INFINITE);
		// delete m_trs80_thread; CWindowThread is automaticly deleteing the object
		m_trs80_thread = NULL;
	}
	
}

void CChildView::OnKeyDown(UINT vk, UINT nRepCnt, UINT nFlags) 
{
	if (m_trs80_thread)
	{
		if (vk == VK_SHIFT)
			m_trs80_thread->SetCharDown(TRS80_KEY_SHIFT, nFlags & 0xFF);

		else if (vk == VK_CONTROL)
			m_trs80_thread->SetCharDown(TRS80_KEY_CTRL, nFlags & 0xFF);
		
		else if (vk==0x1b || vk==VK_PAUSE)	// ESC or pause/break
			m_trs80_thread->SetCharDown(TRS80_KEY_BREAK, nFlags & 0xFF);
		
		else if (vk == VK_LEFT || vk == 8)	// bs or left
			m_trs80_thread->SetCharDown(TRS80_KEY_LEFT, nFlags & 0xFF);
		
		else if (vk == VK_RIGHT )
			m_trs80_thread->SetCharDown(TRS80_KEY_RIGHT, nFlags & 0xFF);
		
		else if (vk == VK_UP)
			m_trs80_thread->SetCharDown(TRS80_KEY_UP, nFlags & 0xFF);
		
		else if (vk == VK_DOWN)
			m_trs80_thread->SetCharDown(TRS80_KEY_DOWN, nFlags & 0xFF);
		
		else if (vk == VK_HOME)
			m_trs80_thread->SetCharDown(TRS80_KEY_CLEAR, nFlags & 0xFF);
	}
	CWnd ::OnKeyDown(vk, nRepCnt, nFlags);
}


void CChildView::OnChar(UINT c, UINT nRepCnt, UINT nFlags) 
{
	if (c == 8)	// handeled by OnKeyDown
		return;

	if (m_trs80_thread)
		m_trs80_thread->SetCharDown(c, nFlags & 0xFF);	// Bug in Win docs, scan code is in LSB
	
	CWnd ::OnChar(c, nRepCnt, nFlags);
}


void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_trs80_thread)
	{
		if (nChar == VK_SHIFT)
			m_trs80_thread->SetCharUp(TRS80_KEY_SHIFT, nFlags & 0xFF);

		else if (nChar == VK_CONTROL)
			m_trs80_thread->SetCharUp(TRS80_KEY_CTRL, nFlags & 0xFF);

		else
			m_trs80_thread->SetCharUp(0, nFlags&0xFF);	
	}
	
	CWnd ::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CWnd ::OnEraseBkgnd(pDC);
}
