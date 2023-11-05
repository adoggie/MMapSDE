#if !defined(AFX_DLGDATATEST_H__94BC950C_F51A_43E5_84A3_AC6F2918C81A__INCLUDED_)
#define AFX_DLGDATATEST_H__94BC950C_F51A_43E5_84A3_AC6F2918C81A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDataTest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDataTest dialog

class CDlgDataTest : public CDialog
{
// Construction
public:
	CDlgDataTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDataTest)
	enum { IDD = IDD_DIALOG_DATA_TEST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDataTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDataTest)
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDATATEST_H__94BC950C_F51A_43E5_84A3_AC6F2918C81A__INCLUDED_)
