// SDETestView.h : interface of the CSDETestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDETESTVIEW_H__3B8CCA71_3E61_4C0E_B141_9155FEE817A2__INCLUDED_)
#define AFX_SDETESTVIEW_H__3B8CCA71_3E61_4C0E_B141_9155FEE817A2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "SDEControler.h"
#include "GeoView.h"
#include "GIS_NetWork.h"
#include "GIS_WayPoint.h"
#include "GIS_NetworkRoute.h"
#include "GIS_FeatureRoute.h"

class CSDETestView : public CGeoView
{
protected: // create from serialization only
	CSDETestView();
	DECLARE_DYNCREATE(CSDETestView)

// Attributes
public:
	CSDETestDoc* GetDocument();
	CString CSDETestView::GetProPathName(CString strPathName = "");
	void SetDrawStatic();
	void SetMapColor(CGIS_LayerInfo *layer, COLORREF brCol, COLORREF penCol);
	void SetMapParticular();
	void GetDefaultLevel(int *nLevel);
	double CalcDist2(double dx1, double dy1, double dx2, double dy2);
	double CalcDist3(double dx1, double dy1, double dx2, double dy2);
	void GreatCircle(float PointA_Latitude, float PointA_Longitude,
			 float PointB_Latitude, float PointB_Longitude,
			 float *GreatCircleDistance, float *GreatCircleBearing);
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDETestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	CPoint				m_CursorPt; // cursor position.

// Implementation
public:
	virtual ~CSDETestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CSDEControler* m_pControl;
	void*  m_pValue;
	IGIS_MapManager* m_pMapManager;
	IGIS_NetWork *m_pNetwork;
	CGIS_FeatureRoute *m_pFRoute;
//	CGIS_WayPoint *m_pWayPoint1;
//	CGIS_WayPoint *m_pWayPoint2;
//	CGIS_NetworkRoute *m_pNetworkRoute;
	int m_nCount;
	BOOL m_bnavi;
	UINT m_nTimerNav;
	POSITION  m_nCurSelPtNo;
	HCURSOR hCursor;
	ST_GEO_PTXY m_ptCenter;
	ST_GEO_PTXY m_pt1;
	IGIS_Map *m_pMap;
	ST_GEO_PTXY m_pt;
	int m_nAngleOld;
	CImageList			m_imageList;
//	CNetWork *m_pNetWork;
// Generated message map functions
protected:
	//{{AFX_MSG(CSDETestView)
	afx_msg void OnNextmap();
	afx_msg void OnOpenall();
	afx_msg void OnZoomout();
	afx_msg void OnZoomin();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCutLayer();
	afx_msg void OnToolEdit();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSdetoolNetwork();
	afx_msg void OnSdetoolWaypoint();
	afx_msg void OnUpdateSdetoolWaypoint(CCmdUI* pCmdUI);
	afx_msg void OnStartn();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();
	afx_msg void OnViewmap();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	ST_GEO_PTXY m_PtNaviStart;
	ST_GEO_PTXY m_ptNaviTo;

protected:
    BOOL m_bShowTip;
public:
    BOOL EnableShowTip(BOOL bEnable)
    {
        BOOL bOldState = m_bShowTip;
        m_bShowTip = bEnable;
        return bOldState;
    }
};

#ifndef _DEBUG  // debug version in SDETestView.cpp
inline CSDETestDoc* CSDETestView::GetDocument()
   { return (CSDETestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDETESTVIEW_H__3B8CCA71_3E61_4C0E_B141_9155FEE817A2__INCLUDED_)
