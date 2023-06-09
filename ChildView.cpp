// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "TRS80Emu.h"
#include "ChildView.h"

#include "Trs80Core.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_CANCELMODE()
	ON_WM_CREATE()
	ON_WM_CAPTURECHANGED()
	ON_WM_CHAR()
	ON_WM_CONTEXTMENU()
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
	CFont *oldfont;

	oldfont = dc.SelectObject(&m_font);
	if (oldfont)
	{
		PaintTrs80Display(&dc);
		dc.SelectObject(oldfont);
	}
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
	
	CDC *dc=GetDC();
	if (dc==NULL)
	{
		TRACE0("Failed to GetDC in view window\n");
		return -1;
	}

	//	if (!m_font.CreatePointFont(120, "trs80tt", dc))
	//	if (!m_font.CreatePointFont(120, "courier new", dc)) 

	if (!m_font.CreateFont(21, 10, 0, 0, FW_NORMAL, FALSE, FALSE, 
		FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, 
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH|TMPF_TRUETYPE, 
		"courier new"))
	{
		TRACE0("Failed to create font in view window\n");
		ReleaseDC(dc);
		return -1;
	}

	ReleaseDC(dc);
	
	return 0;
}

void CChildView::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CWnd ::OnCaptureChanged(pWnd);
}

void CChildView::OnChar(UINT c, UINT nRepCnt, UINT nFlags) 
{
	CDC *dc;
	CFont *oldfont;

	dc=this->GetDC();
	if (dc)
	{
		oldfont = dc->SelectObject(&m_font);
		if (oldfont)
		{
			if (ResumeTrs80Code(dc, (char)c)==SUSPEND_CODE_EXIT)
				PostQuitMessage(0);
			dc->SelectObject(oldfont);
		}
		this->ReleaseDC(dc);
	}
	
	CWnd ::OnChar(c, nRepCnt, nFlags);
}

void CChildView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	
}
