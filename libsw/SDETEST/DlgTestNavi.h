#if !defined(AFX_DLGTESTNAVI_H__5593F393_ECB4_41FF_9428_592218775E3F__INCLUDED_)
#define AFX_DLGTESTNAVI_H__5593F393_ECB4_41FF_9428_592218775E3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTestNavi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTestNavi dialog
#include "GeoGlobe.h"
#include "GIS_NetworkPathInfo.h"

class CDlgTestNavi : public CDialog
{
// Construction
public:
	CDlgTestNavi(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTestNavi)
	enum { IDD = IDD_DIALOG_TESTNAVI };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CGIS_NetworkPathInfo *m_pNWPInfo;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTestNavi)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTestNavi)
	afx_msg void OnButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTESTNAVI_H__5593F393_ECB4_41FF_9428_592218775E3F__INCLUDED_)
