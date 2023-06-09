// TRS80Emu.h : main header file for the TRS80EMU application
//

#if !defined(AFX_MAINAPP_H__B893A132_12FC_4372_AD7E_3D5965266427__INCLUDED_)
#define AFX_MAINAPP_H__B893A132_12FC_4372_AD7E_3D5965266427__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "librarydlg.h"

/////////////////////////////////////////////////////////////////////////////
// CMainApp:
// See MainApp.cpp for the implementation of this class
//

class CMainApp : public CWinApp
{
public:
	CMainApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CMainApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppRun1();
	afx_msg void OnAppRun4();
	afx_msg void OnAppRun2();
	afx_msg void OnAppRun3();
	afx_msg void OnAppViewLib();
	afx_msg void OnUpdateAppViewLib(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// Member Variables

	CLibraryDlg m_library_dlg;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINAPP_H__B893A132_12FC_4372_AD7E_3D5965266427__INCLUDED_)
