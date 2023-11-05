#if !defined(AFX_RICHPOI_H__A7F4387A_7730_4C3B_AA3E_AC128105F166__INCLUDED_)
#define AFX_RICHPOI_H__A7F4387A_7730_4C3B_AA3E_AC128105F166__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichPOI.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRichPOI dialog
#include "GIS_QueryByAtt.h"
class CRichPOI : public CDialog
{
// Construction
public:
	CRichPOI(CWnd* pParent = NULL);   // standard constructor
	CGIS_QueryByAtt* m_pQueryByAtt;
private:
	int m_nLevel1;
	void SetCityList();
	void ClearRichPOI();
	CPtrList m_listResult;

// Dialog Data
	//{{AFX_DATA(CRichPOI)
	enum { IDD = IDD_DIALOG_RICHPOI };
	CButton	m_ChangeLevel1;
	CListBox	m_ListRichPOI;
	CListBox	m_ListCity;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichPOI)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRichPOI)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonChangelevel1();
	afx_msg void OnDblclkListCity();
	afx_msg void OnDblclkListRichpoi();
	afx_msg void OnButtonMove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHPOI_H__A7F4387A_7730_4C3B_AA3E_AC128105F166__INCLUDED_)
