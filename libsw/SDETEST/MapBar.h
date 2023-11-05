#if !defined(AFX_MAPBAR_H__AC4ED217_A89F_43F9_84D4_F35672D741E1__INCLUDED_)
#define AFX_MAPBAR_H__AC4ED217_A89F_43F9_84D4_F35672D741E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapBar.h : header file
//

class CSDETestView;
class CAutoMoveByRoute;
class CSimulNavInfo;
/////////////////////////////////////////////////////////////////////////////
// CMapBar dialog

class CMapBar : public CDialogBar
{
	DECLARE_DYNCREATE(CMapBar)
// Construction
public:
	void SetMapViewSrc(CWnd* pWnd);
	CMapBar();   // standard constructor
	~CMapBar();
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
// Dialog Data
	//{{AFX_DATA(CMapBar)
	enum { IDD = IDD_MAP_BAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapBar)
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
	//{{AFX_MSG(CMapBar)
	afx_msg void OnBtnZoomin();
	afx_msg void OnBtnZoomout();
	afx_msg void OnBtnViewmap();
	afx_msg void OnBtnOrigin();
	afx_msg void OnBtnNavi();
	afx_msg void OnBtnExit();
	afx_msg void OnBtnCheck();
	afx_msg void OnButtonTest();
	afx_msg void OnBtnCalculateDist();
	afx_msg void OnButtonRichpoi();
	afx_msg void OnButtonExpandmap();
	afx_msg void OnButtonMapbarNav();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	ST_GEO_PTXY m_ptOrigin;

	CAutoMoveByRoute *m_pAutoMoveByRoute;
	CSimulNavInfo *m_pSimulNavInfo;
	BOOL m_bSimulNav;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPBAR_H__AC4ED217_A89F_43F9_84D4_F35672D741E1__INCLUDED_)
