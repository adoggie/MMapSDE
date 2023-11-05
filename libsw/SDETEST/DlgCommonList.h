#if !defined(AFX_DLGCOMMONLIST_H__28879EBB_A480_4C28_A2F5_DAD79059A39D__INCLUDED_)
#define AFX_DLGCOMMONLIST_H__28879EBB_A480_4C28_A2F5_DAD79059A39D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCommonList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCommonList dialog

class CDlgCommonList : public CDialog
{
// Construction
public:
	CDlgCommonList(CWnd* pParent,CPtrList* pContentList);   // standard constructor
    int GetSelect(){return m_nSelectItem;};
// Dialog Data
	//{{AFX_DATA(CDlgCommonList)
	enum { IDD = IDD_DIALOG_COMMON_LIST };
	CListCtrl	mlstMain;
	//}}AFX_DATA
    CPtrList* m_pContentList;
    int m_nSelectItem;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCommonList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCommonList)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkCommonList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOMMONLIST_H__28879EBB_A480_4C28_A2F5_DAD79059A39D__INCLUDED_)
