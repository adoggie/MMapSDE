#if !defined(AFX_NAVITESTBAR_H__61E95125_0162_4982_9B86_7D9347BCAE81__INCLUDED_)
#define AFX_NAVITESTBAR_H__61E95125_0162_4982_9B86_7D9347BCAE81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NaviTestBar.h : header file
//
class CSDETestView;
/////////////////////////////////////////////////////////////////////////////
// CNaviTestBar dialog

class CNaviTestBar : public CDialogBar
{
	DECLARE_DYNCREATE(CNaviTestBar)
// Construction
public:
	void SetMapViewSrc(CWnd* pWnd);
	CNaviTestBar();   // standard constructor
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
// Dialog Data
	//{{AFX_DATA(CNaviTestBar)
	enum { IDD = IDD_NAVITEST_BAR };
	CString	m_strCommand;
	CString	m_strLogLat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNaviTestBar)
	public:
	virtual BOOL Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL InitDialogBar();
	CSDETestView* m_pMapView;

	// Generated message map functions
	//{{AFX_MSG(CNaviTestBar)
	afx_msg void OnButtonMoveTo();
	afx_msg void OnButtonRun();
	afx_msg void OnButtonGetLatlog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAVITESTBAR_H__61E95125_0162_4982_9B86_7D9347BCAE81__INCLUDED_)
