#if !defined(AFX_LIBRARYDLG_H__C4A9DD1C_D6F2_4DF5_B030_620E1880AF09__INCLUDED_)
#define AFX_LIBRARYDLG_H__C4A9DD1C_D6F2_4DF5_B030_620E1880AF09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LibraryDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLibraryDlg dialog

class CLibraryDlg : public CDialog
{
// Construction
public:
	CLibraryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLibraryDlg)
	enum { IDD = IDD_LIBRARY };
	CTreeCtrl	m_tree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLibraryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLibraryDlg)
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

struct LibraryNodeData
{
	LibraryNodeData  *next;
	char* name;
	short m_syssoft_level;				// Use ROM?  Use Dos?
	short m_rom_type;					// Which ROM type?
	short m_dos_type;					// Which DOS type?
	short m_boot_mode;					// What to do during boot
	short m_autorun_mode;				// If AutoRun, what to do
	__int64 m_trs80_clk_mhz;			// clock speed to run at
	bool m_lower_case_mod_installed;	// TRUE = use LC letters (bit 6 is RAM)
	bool m_run_real_speed;				// TRUE = run at default trs80 speed
	unsigned char* m_image;				// If AutoRun, image is here
	int m_size;							// Size of Image
};

struct LibraryBranchData
{
	LibraryBranchData  *next;
	char* name;
	LibraryNodeData* branch;

};

#endif // !defined(AFX_LIBRARYDLG_H__C4A9DD1C_D6F2_4DF5_B030_620E1880AF09__INCLUDED_)
