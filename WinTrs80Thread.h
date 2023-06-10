#if !defined(AFX_WinTrs80Thread_H__C4652160_C9A7_40A1_9018_6D973E3FE8FE__INCLUDED_)
#define AFX_WinTrs80Thread_H__C4652160_C9A7_40A1_9018_6D973E3FE8FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinTrs80Thread.h : header file
//

#include "WinTrs80.h"

/////////////////////////////////////////////////////////////////////////////
// CWinTrs80Thread thread

class CWinTrs80Thread : public CWinThread
{
public:
//	DECLARE_DYNCREATE(CWinTrs80Thread)
	DECLARE_DYNAMIC(CWinTrs80Thread)

	CWinTrs80Thread(CWnd *client_wnd, CWinTrs80Thread**, CTrs80Configurator *config);
	void Paint(CDC*);
	void SetCharUp(unsigned char c, unsigned char oem);
	void SetCharDown(unsigned char c, unsigned char oem);
	void KillThreadAsync();
	void GetDisplaySize(CDC *dc, CSize *size);

private:
	HWND m_client_hwnd;
	CWinTrs80 m_trs80;
	CDC m_cdc;
	CWinTrs80Thread** m_ptr_to_me;
	CTrs80Configurator *m_config;

public:
	virtual ~CWinTrs80Thread();

protected:
		virtual BOOL InitInstance();
		virtual int ExitInstance();

	// Generated message map functions
	//{{AFX_MSG(CWinTrs80Thread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG	

		DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WinTrs80Thread_H__C4652160_C9A7_40A1_9018_6D973E3FE8FE__INCLUDED_)


