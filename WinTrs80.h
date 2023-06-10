// WinTrs80.h: interface for the CWinTrs80 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINTRS80_H__00875B60_0A60_4754_98C3_EBF4B4B427E7__INCLUDED_)
#define AFX_WINTRS80_H__00875B60_0A60_4754_98C3_EBF4B4B427E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Trs80Emulator.h"

class CWinTrs80 : public CTrs80Emulator
{
public:
	CWinTrs80();
	virtual ~CWinTrs80();
	void AttachCDC(CDC* dc);	
	void DetachCDC();
	void SetCharUp(unsigned char c, unsigned char oem);
	void SetCharDown(unsigned char c, unsigned char oem);
	void Paint(CDC* dc);

	// Static functions that are not tied to a specifc instance
	static void GetDisplaySize(CDC *dc, CSize *size);

	// Implemented overides from base class CTrs80Emulator
	virtual void DrawChar(unsigned short offset, unsigned char value);
	virtual unsigned char* PeekKeyStatus();
	virtual __int64 GetHostElapsedTime();
	virtual void ResetHostElapsedTime();
	virtual void AboutToSleep();

protected:
	// Implementation functions
	void DrawTrs80Graphic(CDC*, unsigned short offset, unsigned char value);
	void DrawChar(CDC* dc, unsigned short offset, unsigned char value);

private:
	CFont m_font;
	CFont *m_oldfont;
	CDC *m_dc;
	CSize m_char_size;
	int m_cell_w1;
	int m_cell_w2;
	int m_cell_h1;
	int m_cell_h2;
	int m_cell_h3;
	COLORREF m_color;
	unsigned char m_is_key_down[256];		// ASCII array, 1 if key is down
	unsigned char m_key_oem_code[256];		// oem scan code for this ascii key
	__int64 m_sys_freq;
	LARGE_INTEGER m_sys_time_start;
};

#define WIN_X_OFFSET 10
#define WIN_Y_OFFSET 10

#endif // !defined(AFX_WINTRS80_H__00875B60_0A60_4754_98C3_EBF4B4B427E7__INCLUDED_)
