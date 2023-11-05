#if !defined(AFX_DLGTESTMAIN_H__785A8C59_D3AD_44D7_A0B2_3C6E883675FD__INCLUDED_)
#define AFX_DLGTESTMAIN_H__785A8C59_D3AD_44D7_A0B2_3C6E883675FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTestMain.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTestMain dialog

class CDlgTestMain : public CDialog
{
// Construction
public:
	CDlgTestMain(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTestMain)
	enum { IDD = IDD_DIALOG_MAIN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTestMain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTestMain)
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTESTMAIN_H__785A8C59_D3AD_44D7_A0B2_3C6E883675FD__INCLUDED_)
