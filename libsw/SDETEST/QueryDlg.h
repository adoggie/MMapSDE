#if !defined(AFX_QUERYDLG_H__B9EB4D08_81A0_41B7_BA80_EA2D16758156__INCLUDED_)
#define AFX_QUERYDLG_H__B9EB4D08_81A0_41B7_BA80_EA2D16758156__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQueryDlg dialog
#include "IGIS_MAP.H"
class CQueryDlg : public CDialog
{
// Construction
public:
	CQueryDlg(IGIS_Map* pmap,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQueryDlg)
	enum { IDD = IDD_DIALOG1 };
	float	m_dy1;
	int		m_buffer;
	int		m_type;
	UINT	m_type1;
	float	m_dx1;
	CString	m_FeatureName;
	CString	m_FeatureName1;
	//}}AFX_DATA

public:
	IGIS_Map* pMap;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQueryDlg)
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYDLG_H__B9EB4D08_81A0_41B7_BA80_EA2D16758156__INCLUDED_)
