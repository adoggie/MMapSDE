#if !defined(AFX_DLGTESTFRAME_H__56D3CC47_6C40_4489_B400_C66F747FAB82__INCLUDED_)
#define AFX_DLGTESTFRAME_H__56D3CC47_6C40_4489_B400_C66F747FAB82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTestFrame.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgTestFrame dialog

class CDlgTestFrame : public CDialog
{
// Construction
public:
	CDlgTestFrame(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTestFrame)
	enum { IDD = IDD_DIALOG_TESTDLG };
	CListBox	m_ListBoxScript;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTestFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTestFrame)
	afx_msg void OnButtonSingle();
	afx_msg void OnButtonRun();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonMoveTo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTESTFRAME_H__56D3CC47_6C40_4489_B400_C66F747FAB82__INCLUDED_)
