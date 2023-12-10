#include "stdafx.h"
#include "MainApp.h"
#include "ChildView.h"
#include "MainFrm.h"
#include "WinTrs80.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//**************************************************

CWinTrs80::CWinTrs80()
{

	// If this is changed, change GetDisplaySize
	if (!m_font.CreateFont(21, 10, 0, 0, FW_NORMAL, FALSE, FALSE, 
		FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, 
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH|TMPF_TRUETYPE, 
		"courier new"))
	{
		TRACE0("Failed to create font in CWinTrs80\n");
	}

	for (int i=0; i < sizeof(m_is_key_down); i++)
		m_is_key_down[i] = 0;

}

//*************************************************

CWinTrs80::~CWinTrs80()
{

}

//**************************************************

void CWinTrs80::AttachCDC(CDC *dc)
{
	m_dc = dc;

	// select our custom font into the cdc
	m_oldfont = dc->SelectObject(&m_font);
	if (!m_oldfont)
	{
		TRACE0("Error selecting font in CWinTrs80::AttachCDC()\n");
	}

	// cache some stuff for speed
	m_char_size = dc->GetTextExtent("X",1);	// font is fixed, so any char will work
	// Calc each cell seperately to avoid rounding errors
	m_cell_w1 = m_char_size.cx / 2;  
	m_cell_w2 = m_char_size.cx - m_cell_w1;  
	m_cell_h1 = m_char_size.cy / 3;
	m_cell_h2 = m_char_size.cy / 3;
	m_cell_h3 = m_char_size.cy - m_cell_h1 - m_cell_h2;
	m_color = dc->GetTextColor();
}

//**************************************************

void CWinTrs80::DetachCDC()
{
	// restore original font
	if (m_oldfont)
		m_dc->SelectObject(m_oldfont);

	m_dc = NULL;
}

//**************************************************
// static function
void CWinTrs80::GetDisplaySize(CDC *dc, CSize *size)
{
	CRect rect;
	CFont *oldfont;
	CFont display_font;

	// If this is changed, change CWinTrs80 contructor as well
	if (!display_font.CreateFont(21, 10, 0, 0, FW_NORMAL, FALSE, FALSE, 
		FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, 
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH|TMPF_TRUETYPE, 
		"courier new"))
	{
		TRACE0("Failed to create font in CWinTrs80\n");
	}

	
	oldfont = dc->SelectObject(&display_font);
	ASSERT(oldfont);
	*size = dc->GetTextExtent("X",1);	// font is fixed, so any char will work
	if (oldfont)
		dc->SelectObject(oldfont);

	size->cx=size->cx*64+WIN_X_OFFSET*2; 
	size->cy=size->cy*16+WIN_Y_OFFSET*2;
}

//**************************************************
//
// ASCII chars except for TRS80_KEY_ which are mapped in to ASCII
//
void CWinTrs80::SetCharDown(unsigned char c, unsigned char oem_code)
{
		m_is_key_down[c] = 1;
		m_key_oem_code[c] = oem_code;

		// TRACE("key down %x %x\n", c, oem_code);
}

//**************************************************

//
// WM_KEYUP messages, so there are lots of them
//
void CWinTrs80::SetCharUp(unsigned char c, unsigned char oem_code)
{
	// TRACE("key up %x %x\n", c, oem_code);

	if (c == TRS80_KEY_SHIFT)
		m_is_key_down[TRS80_KEY_SHIFT] = 0;

	else if (c == TRS80_KEY_CTRL)
		m_is_key_down[TRS80_KEY_CTRL] = 0;

	else
	{
		for (short i=TRS80_KEY_LAST_SPECIAL+1; i < 256; i++)
			if (m_is_key_down[i] &&  m_key_oem_code[i]==oem_code)
			{
				m_is_key_down[i] = 0;
				break;
			}

			if (i >= 256)
			{
				TRACE("CWinTrs80::SetCharU Error:  Can't find oem_code %x\n", oem_code);
			}
	}
}

//**************************************************

unsigned char* CWinTrs80::PeekKeyStatus()
{
	return m_is_key_down;
}

//**************************************************

void CWinTrs80::Paint(CDC* dc)
{
	CFont *oldfont;

	oldfont = dc->SelectObject(&m_font);
	if (!oldfont)
	{
		TRACE0("Error selecting font in CWinTrs80::Paint()\n");
	}

	for (int i=0; i < 1024; i++)
		DrawChar(dc, i, m_memory[0x3C00+i]);

	if (oldfont)
		dc->SelectObject(oldfont);

}

//**************************************************

void CWinTrs80::DrawChar(unsigned short offset, unsigned char value)
{
	DrawChar(m_dc, offset, value);
}

//**************************************************

void CWinTrs80::DrawChar(CDC* dc, unsigned short offset, unsigned char value)
{
	if (value&128 || value==32)
	{
		DrawTrs80Graphic(dc, offset, value);
	}
	else
	{
		int x, y;

		x = WIN_X_OFFSET + (offset%64)*m_char_size.cx;
		y = WIN_X_OFFSET + (offset/64)*m_char_size.cy;

		if (!m_config->m_lower_case_mod_installed)
		{
			if ((value & 32) == 0)	// When No lowercase mod installed
				value |= 64;		// BIT6 = NOT (BIT5 OR BIT7)
			else // this part not needed -- why?
				value &= (~64);		// BIT6 = NOT (BIT5 OR BIT7)
		}
		dc->TextOut(x, y, (char *)&value, 1);
		::GdiFlush();
	}
}

//**************************************************

void CWinTrs80::DrawTrs80Graphic(CDC* dc, unsigned short offset, unsigned char val)
{
	int x, y;
	COLORREF bg;

	x = WIN_X_OFFSET + (offset%64)*m_char_size.cx;
	y = WIN_X_OFFSET + (offset/64)*m_char_size.cy;

	bg = dc->GetBkColor();
	dc->FillSolidRect(x, y, m_char_size.cx, m_char_size.cy, bg);

	if (val == 32)
		return;

	if (val&1)
		dc->FillSolidRect(x, y, m_cell_w1, m_cell_h1, m_color);

	y=y+m_cell_h1;

	if (val&4)
		dc->FillSolidRect(x, y, m_cell_w1, m_cell_h2, m_color);

	y=y+m_cell_h2;

	if (val&16)
		dc->FillSolidRect(x, y, m_cell_w1, m_cell_h3, m_color);

	x=x+m_cell_w1;

	if (val&32)
		dc->FillSolidRect(x, y, m_cell_w2, m_cell_h3, m_color);

	y=y-m_cell_h2;

	if (val&8)
		dc->FillSolidRect(x, y, m_cell_w2, m_cell_h2, m_color);

	y=y-m_cell_h1;

	if (val&2)
		dc->FillSolidRect(x, y, m_cell_w2, m_cell_h1, m_color);

	dc->SetBkColor(bg);

}

//
// Tell the system timer to measure time from this point on ward
//
void CWinTrs80::ResetHostElapsedTime()
{
	LARGE_INTEGER lpFrequency;
	VERIFY(QueryPerformanceFrequency(&lpFrequency));

	m_sys_freq = lpFrequency.QuadPart;
	QueryPerformanceCounter(&m_sys_time_start);
}

//
// Return the number of usecs that have elapsed since the last call to ResetHostElapsedTime()
// Used to throttle the Z80 speed to the speed the TRS80 originally ran at
//
__int64 CWinTrs80::GetHostElapsedTime()
{
	LARGE_INTEGER sys_time_now;
	__int64 elapsed_sys_time;

	VERIFY(QueryPerformanceCounter(&sys_time_now));
	elapsed_sys_time = sys_time_now.QuadPart - m_sys_time_start.QuadPart;
	elapsed_sys_time = (elapsed_sys_time*1000000)/m_sys_freq;

	return elapsed_sys_time;
}

//
// Draw into the back buffer but once each sleep (nominally 10ms), update the display
//

void CWinTrs80::AboutToSleep()
{
	extern CMainApp theApp;
	CChildView *view;
	HRESULT ddrval; 
	HDC hdc;

	view = (CChildView *)&(((CMainFrame *)theApp.m_pMainWnd)->m_wndView);\

	view->lpDDSBack->ReleaseDC(*m_dc); 
	m_dc->Detach();

	ddrval = view->lpDDSPrimary->Blt(&view->m_direct_draw_into_rect, view->lpDDSBack, NULL, DDBLT_WAIT, NULL );

	view->lpDDSBack->GetDC(&hdc);
	m_dc->Attach(hdc);
}
