// SDETestView.cpp : implementation of the CSDETestView class
//

#include "stdafx.h"
#include "SDETest.h"
#include <math.h>

#include "SDETestDoc.h"
#include "SDETestView.h"

#include "GIS_Message.h"
#include "GIS_QueryByShape.h"
#include "GIS_QueryByAtt.h"
#include "IGIS_TOOL.h"
#include "IGIS_Query.h"
#include "GIS_Pathpoint.h"
#include "GIS_UserMap.h"
#include "GIS_UserLayer.h"
#include "GIS_FeatureRoute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MAP_LAYER_NUMBER = 25;
/////////////////////////////////////////////////////////////////////////////
// CSDETestView
#define DSquare(x) ((x)*(x))

IMPLEMENT_DYNCREATE(CSDETestView, CView)

BEGIN_MESSAGE_MAP(CSDETestView, CView)
	//{{AFX_MSG_MAP(CSDETestView)
	ON_COMMAND(ID_NEXTMAP, OnNextmap)
	ON_COMMAND(ID_OPENALL, OnOpenall)
	ON_COMMAND(ID_ZOOMOUT, OnZoomout)
	ON_COMMAND(ID_ZOOMIN, OnZoomin)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_CUT_LAYER, OnCutLayer)
	ON_COMMAND(ID_TOOL_EDIT, OnToolEdit)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_SDETOOL_NETWORK, OnSdetoolNetwork)
	ON_COMMAND(ID_SDETOOL_WAYPOINT, OnSdetoolWaypoint)
	ON_UPDATE_COMMAND_UI(ID_SDETOOL_WAYPOINT, OnUpdateSdetoolWaypoint)
	ON_COMMAND(ID_STARTN, OnStartn)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_VIEWMAP, OnViewmap)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDETestView construction/destruction

CSDETestView::CSDETestView()
{
	// TODO: add construction code here
	m_imageList.Create(IDB_BITMAP_CURSOR, 16, 0, RGB(198,195,198));
	m_nCount = 0;
//	m_pNetworkRoute = new CGIS_NetworkRoute;
	m_bnavi = FALSE;
	m_pFRoute = NULL;
	m_OldScaleLevel = 0;
	m_nAngleOld = -1;
/*
	m_pWayPoint1 = new CGIS_WayPoint;
	m_pWayPoint1->SetWPType( EN_WAYPOINTTYPE_ORIGIN );
	m_pNetworkRoute->GetWayPointList()->AddTail( m_pWayPoint1 );
	m_pWayPoint2 = new CGIS_WayPoint;
	m_pWayPoint2->SetWPType( EN_WAYPOINTTYPE_DESTINATION );//EN_WAYPOINTTYPE_INTERMEDIATE;
	m_pNetworkRoute->GetWayPointList()->AddTail( m_pWayPoint2 );
*/
//	m_pNetworkRoute = new CGIS_NetworkRoute;
//	CGIS_NetworkPathInfo *pPathInfo = new CGIS_NetworkPathInfo;
//	pPathInfo->SetVehicleType( EN_VEHICLETYPE_CAR );
//	pPathInfo->SetRoutePreference( EN_RUTPREF_FASTERTIME );
////	pPathInfo->SetRoutePreference( EN_RUTPREF_SHORTDIS );
//	m_pNetworkRoute->SetNetworkPathInfo( pPathInfo );

	m_pControl = NULL;

	m_PtNaviStart.dx = 0.0f;
	m_PtNaviStart.dy = 0.0f;
	m_ptNaviTo.dx = 0.0f;
	m_ptNaviTo.dy = 0.0f;

    m_ptCenter.dx = 121.2438;
    m_ptCenter.dy = 31.1068;

    m_bShowTip = TRUE;
}

CSDETestView::~CSDETestView()
{
	m_imageList.DeleteImageList();
	if(m_pControl)
	{
		delete m_pControl;
		m_pControl = NULL;
	}
}

BOOL CSDETestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSDETestView drawing

void CSDETestView::OnDraw(CDC* pDC)
{
	CSDETestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

    if(m_bShowTip)
	    ShowFindTip( m_CursorPt );

	ShowFocusRut( m_CursorPt );
	RECT TempPlotRect = GetViewPlot();
	CBitmap* TemppBackBMP = GetBackBMP();
	CDC* TemppBackDC = GetBackDC();
	if( !TemppBackDC )
		return;
	CBitmap* pOldBMP = (CBitmap*)(TemppBackDC->SelectObject(TemppBackBMP));
//	BeginWaitCursor();
	ST_GEO_PTXY xyCenter;
	ST_GEO_RECT rtOld = GetOldDrawRect();
	xyCenter.dx = (rtOld.dRight + rtOld.dLeft) / 2;
	xyCenter.dy = (rtOld.dBottom + rtOld.dTop) / 2;
	
	int Oldnx,Oldny;
	XyMapToView( xyCenter.dx,xyCenter.dy,Oldnx,Oldny );

	ST_GEO_PTXY xyCurrent;
	xyCurrent.dx = (NewDrawRect.dRight + NewDrawRect.dLeft) / 2;
	xyCurrent.dy = (NewDrawRect.dBottom + NewDrawRect.dTop) / 2;
	int Curnx, Curny;
	XyMapToView( xyCurrent.dx,xyCurrent.dy,Curnx,Curny );
	
 	SetbReDrawCar(FALSE);
	if( abs(Oldnx-Curnx)>=5 || abs(Oldny-Curny)>=5 || m_OldScaleLevel != GetCurrentScaleLevel() || m_nAngleOld != m_Angle)
	{
		DrawMap();
		m_OldScaleLevel = GetCurrentScaleLevel();
		m_nAngleOld = m_Angle;
	}
	else
	{
		SetbReDrawCar(TRUE);
		DrawMap();
	}
//	if( m_pNetworkRoute )
//	{
//		POSITION pos = m_pNetworkRoute->GetWayPointList()->GetHeadPosition();
//		CGIS_WayPoint* pTempWP;
//		int i = 1;
//		while(pos)
//		{
//			pTempWP = (CGIS_WayPoint*)m_pNetworkRoute->GetWayPointList()->GetNext(pos);
//			DrawFlag(pTempWP->GetWPPosition().dx,pTempWP->GetWPPosition().dy,i);
//			i++;
//		}
//	}
//	EndWaitCursor();
	pDC->BitBlt(0,0, (TempPlotRect.right - TempPlotRect.left)-4, (TempPlotRect.bottom - TempPlotRect.top)-4, 
			  TemppBackDC, 0, 0, SRCCOPY);	
	m_imageList.Draw(pDC, 0, m_CursorPt, ILD_TRANSPARENT);
//	if(m_CurPt.dx > FLOAT_ZERO && m_CurPt.dy > FLOAT_ZERO)
//	{
//		DrawCurPt();
//	}
//	MEMORYSTATUS mem;
//	GlobalMemoryStatus(&mem);
//	CString str;
//	str.Format( L"%d" , mem.dwAvailPhys );
//	pDC->SetTextColor( RGB(0,0,0));
//	pDC->DrawText(str,CRect(100,100,200,200),DT_CENTER );
	// TODO: Add your message handler code here
	// TODO: add draw code for native data here
//	NewRect(plotRect);
	int nX,nY;
	XyMapToView(m_PtNaviStart.dx,m_PtNaviStart.dy,nX,nY);	
	pDC->FillRect(&CRect(nX-4,nY-4,nX+4,nY+4),&CBrush(RGB(0,0,255)));

	XyMapToView(m_ptNaviTo.dx,m_ptNaviTo.dy,nX,nY);	
	pDC->FillRect(&CRect(nX-4,nY-4,nX+4,nY+4),&CBrush(RGB(0,255,0)));


}

/////////////////////////////////////////////////////////////////////////////
// CSDETestView diagnostics

#ifdef _DEBUG
void CSDETestView::AssertValid() const
{
	CView::AssertValid();
}

void CSDETestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSDETestDoc* CSDETestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSDETestDoc)));
	return (CSDETestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSDETestView message handlers

void CSDETestView::OnNextmap() 
{
	// TODO: Add your command handler code here	
}

void CSDETestView::OnOpenall() 
{
	// TODO: Add your command handler code here
}
CString CSDETestView::GetProPathName(CString strPathName)
{
	TCHAR tchDir = '\\';
	CString strTemp, strIniPathName, strIniFileName;
	GetModuleFileName(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();

	strIniPathName = strTemp.Left(strTemp.ReverseFind(tchDir) + 1);
	strIniFileName = strIniPathName + strPathName;

	return strIniFileName;
}

void CSDETestView::OnZoomout() 
{
	// TODO: Add your command handler code here
		m_CenterPt = m_CursorPt;
		ZoomInAt(m_CenterPt);
		m_CenterPt.x = plotRect.right/2;
		m_CenterPt.y = plotRect.bottom/2;
        m_CursorPt = m_CenterPt;
		Refresh();
}

void CSDETestView::OnZoomin() 
{
	// TODO: Add your command handler code here
		m_CenterPt = m_CursorPt;
		ZoomOutAt(m_CenterPt);
		m_CenterPt.x = plotRect.right/2;
		m_CenterPt.y = plotRect.bottom/2;
        m_CursorPt = m_CenterPt;
		Refresh();        
}
/*
void CSDETestView::OnPaneUpdate() 
{
	// TODO: Add your command handler code here
	CDC *pDC = GetDC();
	bMapDrawed = FALSE;
	OnDraw( pDC );
	
}
*/
void CSDETestView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ButtonDown(point);
	m_CursorPt = point;
	ST_GEO_PTXY pt;
	double dx,dy;
	XyViewToMap( point.x,point.y,dx,dy );
	pt.dx = dx;
	pt.dy = dy;
	SetCurPt( pt );
	CView::OnLButtonDown(nFlags, point);
}

void CSDETestView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ButtonUp(point);
	CView::OnLButtonUp(nFlags, point);
}

void CSDETestView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	MouseMove(point);
	CView::OnMouseMove(nFlags, point);
}
#include "QueryDlg.h"
void CSDETestView::OnCutLayer() 
{
	// TODO: Add your command handler code here
	m_pMapManager->CreateMap( NULL );
/*
	IGIS_Map* pMap;
	pMap = (IGIS_Map*)(m_pMapManager->GetFirstMap());
	CQueryDlg dlg( pMap, this );
	dlg.DoModal();
	return;
	CGIS_QueryByShape* pTempQuery;
	CGIS_QueryByAtt* pTempQueryByAtt;
	pTempQuery = (CGIS_QueryByShape*)pMap->GetTool(0);
	double dx = pMap->GetMapInfo()->GetMapRect().dLeft + (pMap->GetMapInfo()->GetMapRect().dRight - pMap->GetMapInfo()->GetMapRect().dLeft) / 2;
	double dy = pMap->GetMapInfo()->GetMapRect().dTop + (pMap->GetMapInfo()->GetMapRect().dBottom - pMap->GetMapInfo()->GetMapRect().dTop) / 2;
	ST_GEO_PTXY TempPT;
	TempPT.dx = dx;
	TempPT.dy = dy;
	CPtrList RList;
	RList.RemoveAll();
	pTempQueryByAtt = (CGIS_QueryByAtt*)pMap->GetTool(1);
	pTempQueryByAtt->QueryByAnnotation(RList,L"太平北路");
	CGIS_Feature* TempFea;
	POSITION pos = RList.GetHeadPosition();
	if(pos)
	{
		TempFea = (CGIS_Feature*)(RList.GetNext(pos));
	}
	CClientDC dc( this );
	CPen pen, *pPen;
	pen.CreatePen(PS_SOLID, 5, RGB(255,0,0) );
	pPen = dc.SelectObject( &pen );
	CGIS_FeaturePoly* pTempRut = (CGIS_FeaturePoly*)TempFea;
	int nx, ny;
	XyMapToView(pTempRut->GetPtList()[0].dx, pTempRut->GetPtList()[0].dy, nx, ny );
	dc.MoveTo( nx, ny );
	for(int i = 1 ; i < pTempRut->GetPtNum() ; i++)
	{
		XyMapToView(pTempRut->GetPtList()[i].dx, pTempRut->GetPtList()[i].dy, nx, ny);
		dc.LineTo( nx, ny );
	}
//	DrawLine(pTempRut->GetPtList(),pTempRut->GetRutInfo()->nPtNum);
	
//	pTempQuery->QueryByBuffer(RList,&TempPT,300,EN_OBJ_POLY);
	
	RList.RemoveAll();
	pTempQuery->QuerryByIntersect(RList,TempFea,EN_OBJ_ALL);
	POSITION pos1 = RList.GetHeadPosition();
	while(pos1)
	{
		TempFea = (CGIS_Feature*)(RList.GetNext(pos1));
	}
	*/
/*
	RList.RemoveAll();
	pTempQuery->QueryByContain(RList,TempFea);
	POSITION pos1 = RList.GetHeadPosition();
	while(pos1)
	{
		TempFea = (CGIS_Feature*)(RList.GetNext(pos1));

		int tt  = 0 ;
*/
		/*
		TempFea = (CGIS_Feature*)(RList.GetNext(pos1));
		TempFea = (CGIS_Feature*)(RList.GetNext(pos1));
		TempFea = (CGIS_Feature*)(RList.GetNext(pos1));
		CGIS_FeaturePoly* pTempPoly = (CGIS_FeaturePoly*)TempFea;

	POINT* pt = new POINT[pTempPoly->GetPtNum()];

	for(int i = 0; i < pTempPoly->GetPtNum(); i++)
	{
		XyMapToView(pTempPoly->GetPtList()[i].dx, pTempPoly->GetPtList()[i].dy, nx, ny);

		pt[i].x = nx;
		pt[i].y = ny;
	}
	
	if (pTempPoly->GetPartNum() == 1)
	{
		dc.Polygon(pt, pTempPoly->GetPtNum());
	}
	else
	{
		dc.PolyPolygon(pt, pTempPoly->GetPart(), pTempPoly->GetPartNum());
	}

	delete [] pt;
	pt = NULL;
	*/

//	}
}
#include "EditDlg.h"
void CSDETestView::OnToolEdit() 
{
	// TODO: Add your command handler code here
/*	IGIS_Map* pMap;
	pMap = m_pMapManager->GetFirstMap();
	m_pNetWork = pMap->GetNetWork();
	m_pNetWork->BuildTopo();
*/
/*	ASSERT( m_pMapManager );
	CEditDlg dlg( m_pMapManager, this);
	dlg.DoModal( );
	if( dlg.m_pMap )
	{
		IGIS_Map* pMap;
		m_MapSet->RemoveAll();
		pMap = dlg.m_pMap;
		if(pMap != NULL)
		{
			pMap->Attach(this);
			//用户设置的初始位置
			double dx = pMap->GetMapInfo()->m_mapRect.dLeft + (pMap->GetMapInfo()->m_mapRect.dRight - pMap->GetMapInfo()->m_mapRect.dLeft) / 2;
			double dy = pMap->GetMapInfo()->m_mapRect.dTop + (pMap->GetMapInfo()->m_mapRect.dBottom - pMap->GetMapInfo()->m_mapRect.dTop) / 2;
			//
			InitializePlot(dx,dy);
			Refresh();
		}
	}
*/
	MoveToCenter(GetCurPt().dx,GetCurPt().dy);

	Refresh();
//	NewRect(plotRect);
}
static int nn = 0;
void CSDETestView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
/*	double dx,dy; 
    XyViewToMap(point.x,point.y,dx,dy);
    m_pNetWork->AddFlagPt(dx,dy,NewDrawRect,m_pNetWork->m_pListFlagVtx);
	if( nn == 0 )
	{
		AfxMessageBox(L"Begin" );
		nn++;
	}
	else if( nn == 1 )
	{
		if( m_pNetWork->GetShortPath( ) )
			AfxMessageBox(L"Success" );
		AfxMessageBox( L"End" );
		m_pNetWork->m_pListFlagVtx->RemoveAll();
		nn = 0;
*/
/*		CDC *pDC = GetDC();
		CPen pen, *pOldPen;
		pen.CreatePen( PS_SOLID, 1, RGB(0,0,255) );
		POSITION pos = m_pNetWork->m_pListOptimalRut->GetHeadPosition();
		CPtrList *pRutList;
		while( pos )
		{
			pRutList = m_pNetWork->m_pListOptimalRut->GetNext( pos );
			POSITION ps = pRutList->GetHeadPosition();
			ST_GEO_RUT *pRut;
			while( ps )
			{
				pRut = pRutList->GetNext(ps);
			}
		}
	}
*/	
	CView::OnLButtonDblClk(nFlags, point);
}
#include "GIS_WayPoint.h"
#include "GIS_NetworkPathInfo.h"
#include "GIS_Network.h"
#include "GIS_NetworkRoute.h"
void CSDETestView::OnSdetoolNetwork() 
{
//	// TODO: Add your command handler code here
//	ST_GEO_PTXY pt;
//	pt.dx = 120.204;
//	pt.dy = 30.3025;
////	pt.dx = 115.415;
////	pt.dy = 39.4311;
////	pt.dx = 116.386;
////	pt.dy = 39.9511;
////	pt.dx = 117.283;
////	pt.dy = 40.1666;
//	CGIS_WayPoint* pWayPoint = new CGIS_WayPoint(1);
//	pWayPoint->SetWPType( EN_WAYPOINTTYPE_ORIGIN );
//	pWayPoint->SetWPPosition( pt );	
//	m_pNetworkRoute->GetWayPointList()->AddTail( pWayPoint );
//
//
//	pt.dx = 120.124;
//	pt.dy = 30.1025;
////	pt.dx = 120.249;
////	pt.dy = 30.2719;
////	pt.dx = 116.200;
////	pt.dy = 39.8891;
////	pt.dx = 117.299;
////	pt.dy = 40.1575;
//	CGIS_WayPoint* pWayPoint3 = new CGIS_WayPoint(2);
//	pWayPoint3->SetWPType( EN_WAYPOINTTYPE_DESTINATION );
//	pWayPoint3->SetWPPosition( pt );	
//	m_pNetworkRoute->GetWayPointList()->AddTail( pWayPoint3 );

/*	CGIS_NetworkRoute *pNetworkRoute = new CGIS_NetworkRoute;
	CGIS_WayPoint *pWayPoint1 = new CGIS_WayPoint;
	pWayPoint1->SetWPPosition( pt );
	pWayPoint1->SetWPType( EN_WAYPOINTTYPE_ORIGIN );
	pNetworkRoute->GetWayPointList()->AddTail( pWayPoint1 );
	CGIS_WayPoint *pWayPoint2 = new CGIS_WayPoint;
	pt.dx = 120.2119;
	pt.dy = 30.1823;
	pWayPoint2->SetWPPosition( pt );
	pWayPoint2->SetWPType( EN_WAYPOINTTYPE_DESTINATION );//EN_WAYPOINTTYPE_INTERMEDIATE;
	pNetworkRoute->GetWayPointList()->AddTail( pWayPoint2 );

	CGIS_NetworkPathInfo *pPathInfo = new CGIS_NetworkPathInfo;
	pPathInfo->SetVehicleType( EN_VEHICLETYPE_CAR );
	pPathInfo->SetRoutePreference( EN_RUTPREF_FASTERTIME );
//	pPathInfo->SetRoutePreference( EN_RUTPREF_SHORTDIS );
	pNetworkRoute->SetNetworkPathInfo( pPathInfo );

//	CGIS_NetWork *pNetwork = (CGIS_NetWork*)m_pControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );//new CGIS_NetWork;
	CGIS_NetWork *pNetwork = (CGIS_NetWork*)m_pControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );//new CGIS_NetWork;
//	pNetwork->SetMapManager( m_pMapManager );
	pNetwork->SetNetworkRoute( pNetworkRoute );
	pNetwork->CalculateOptimalRoute( );
*/
//	DWORD dwTime = GetTickCount();
//	m_pNetwork->SetNetworkRoute( m_pNetworkRoute );
//	m_pNetwork->CalculateOptimalRoute( );
//	TRACE( L"AllCalcTime:%d\n", GetTickCount()-dwTime );
	
//	CGIS_UserMap *pMap = (CGIS_UserMap*)m_pMapManager->GetMapByID( 1000 );
////	SetNetworkRoute(m_pNetworkRoute);
//	CGIS_UserLayer *pULayer;
//	if( pMap )
//	{
//		pULayer = pMap->GetUserLayer( EN_UMLAYERTYPE_ROUTE );
////		pULayer->DeleteFeature( m_pFRoute, pMap->GetMapInfo()->GetMapFilePath() );
//		pULayer->UpdateFeature( m_pFRoute, pMap->GetMapInfo()->GetMapFilePath() );
//	}
//	Refresh();
/*
	double dx,dy;
	XyViewToMap(plotRect.right / 2 +1,plotRect.bottom / 2 +1,dx,dy);
	MoveToCenter(dx,dy);
	Refresh();
*/
//	CGIS_QueryByShape *pTempQuery;
//	pTempQuery = (CGIS_QueryByShape*)m_pMap->GetTool(QUERY_BYSHARP);
//	CPtrList RList;
//	pTempQuery->QueryByBuffer(RList,&m_pt,15,EN_OBJ_RUT);
//	CString str;
//	str.Format(L"%d",RList.GetCount());
//	AfxMessageBox(str);
//	if(RList.GetCount() > 0)
//	{
//		CGIS_FeatureRut *pTempRut = (CGIS_FeatureRut*)RList.GetHead();
//		str =(CString)pTempRut->GetAnnotation();
//		AfxMessageBox(str);
//	}
//
	static int i = 0;
	if( i>12 )
		i = 0;
	else
	{
		i++;
	}
	SetAngle(30*i);
	Refresh();
}

void CSDETestView::OnSdetoolWaypoint() 
{
	// TODO: Add your command handler code here
	if( m_nCount ==  0 )
	{
		m_nCount = 1;
	}
	else if( m_nCount == 1 )
	{
		m_nCount = 0;
	}
}

void CSDETestView::OnUpdateSdetoolWaypoint(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	  pCmdUI->SetCheck(m_nCount == 0);
	
}

void CSDETestView::OnStartn() 
{
	// TODO: Add your command handler code here
//	UINT nID = m_pNetworkRoute->GetWPMaxID();
//	CGIS_WayPoint *pWPoint = new CGIS_WayPoint( nID );
//	pWPoint->SetWPAnnoation( "CM" );
//	pWPoint->SetWPAnnoNum( 2 );
//	pWPoint->SetWPPosition( m_ptCenter );
//	pWPoint->SetWPType( EN_WAYPOINTTYPE_INTERMEDIATE );
//	POSITION ps = m_pNetworkRoute->GetWayPointList()->GetHeadPosition();
//	m_pNetworkRoute->GetWayPointList()->InsertAfter( ps, pWPoint );
//	ps = m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetHeadPosition();
//	CGIS_Pathpoint *pPPoint = NULL;
//	while( ps )
//	{
//		pPPoint = (CGIS_Pathpoint*)m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetNext(ps);
//		if( pPPoint )
//			delete pPPoint;
//	}
//	m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->RemoveAll();
//	m_pNetwork->CalculateOptimalRoute( );
//	return ;

	if(m_pNetworkRoute->GetWayPointList()->GetCount() == 0 || m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetCount() == 0)
	{
		return;
	}
	if(!m_bnavi)
	{
		m_bnavi = TRUE;
		m_nTimerNav = SetTimer(1, 1000, NULL);
		ST_GEO_PTXY pt;
		pt.dx = ((CGIS_Pathpoint*)m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetHead())->GetPathpointPosition().dx;
		pt.dy = ((CGIS_Pathpoint*)m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetHead())->GetPathpointPosition().dy;
		SetCurPt(pt);
		m_nCurSelPtNo = m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetHeadPosition();
		m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetNext(m_nCurSelPtNo);
		m_pNetworkRoute->GetNetworkPath()->InitNavigate();
//		MoveToCenter(pt.dx,pt.dy);
//		Refresh();
	}
	else
	{
		m_bnavi = FALSE;
		KillTimer(m_nTimerNav);
		ST_GEO_PTXY pt;
		pt.dx = pt.dy = 0.0;
		SetCurPt(pt);
		m_nCurSelPtNo = NULL;
	}

//	double dx,dy;
//	XyViewToMap(plotRect.right / 2 +1,plotRect.bottom / 2 +1,dx,dy);
//	MoveToCenter(dx,dy);
//
	Refresh();
}
void CSDETestView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	static int iAngle = 0;

	double dbLong = 121.36184 + (double(rand()) / double(RAND_MAX) / 2 - 0.25);
	double dbLat  =  31.55938 + (double(rand()) / double(RAND_MAX) / 2 - 0.25);
	//int iAngle    = int((double(rand()) / double(RAND_MAX)) * 360.0);
	
	MoveToCenter(dbLong, dbLat);
	SetAngle(iAngle);
	Refresh();		
	
	iAngle += 5;

	if(iAngle >= 360)
		iAngle = 0;

	
/*	if(m_bnavi)
	{
		double dx, dy, dbDis,x,y;
		x = ((CGIS_Pathpoint*)m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetAt(m_nCurSelPtNo))->GetPathpointPosition().dx;
		y = ((CGIS_Pathpoint*)m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetAt(m_nCurSelPtNo))->GetPathpointPosition().dy;
		double dbCurLen = CalcDis( x,y, GetCurPt().dx,GetCurPt().dy);
//		m_dbAngle = CalcAng(x,y,GetCurPt().dx,GetCurPt().dy);
//		m_Angle = m_dbAngle * 57;
		CString strCRName, strNRName, strVoice;
		double dbLen, dbAngle, dbAng;
		ST_GEO_PTXY ptRtn;
		if(dbCurLen >= FN_LEN)
		{
			GetPointArc(GetCurPt().dx, GetCurPt().dy, x, y, FN_LEN/dbCurLen, dx, dy);
			ST_GEO_PTXY pt;
			pt.dx = (float)dx;
			pt.dy = (float)dy;
			
//			pt.dy += 0.0001;
			EnMatchRoad enMRes = MatchRoad( pt, ptRtn, EN_MATCH_REALNAV );
			SetCurPt(ptRtn);
			MoveToCenter( ptRtn.dx, ptRtn.dy );
//			SetCurPt( pt );
//			MoveToCenter( pt.dx, pt.dy );
// 			m_pNetworkRoute->GetNetworkPath()->GetNavigateVoice( pt, 1, strCRName, strNRName, dbLen, dbAngle, dbDis, dbAng, strVoice );
			m_Angle = CalcAng(x,y,GetCurPt().dx,GetCurPt().dy) * 57;
//			NewRect(plotRect);

		}
		else
		{
			ST_GEO_PTXY pt;
			pt.dx = x;
			pt.dy = y;
//			pt.dy += 0.0001;
			EnMatchRoad enMRes = MatchRoad( pt, ptRtn, EN_MATCH_REALNAV );
			SetCurPt(ptRtn);
//			SetCurPt( pt );
//			m_pNetworkRoute->GetNetworkPath()->GetNavigateVoice( pt, 1, strCRName, strNRName, dbLen, dbAngle, dbDis, dbAng, strVoice );
			m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetNext(m_nCurSelPtNo);
			if(!m_nCurSelPtNo)
			{
				MoveToCenter(GetCurPt().dx, GetCurPt().dy);
				Refresh();
				m_bnavi = FALSE;
				KillTimer(m_nTimerNav);				
				return;
			}
		}
		m_dbAngle = PI / 2;
		
	
//		MoveToCenter(m_CurPt.dx, m_CurPt.dy);
//		MoveToCenter(GetCurPt().dx, GetCurPt().dy);
		Refresh();
	}
	*/
	CView::OnTimer(nIDEvent);
}



int CSDETestView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
/*	CString temp = GetProPathName(L"\MAP\\");
//	CString temp = GetProPathName(L"\map");
//	CString temp = GetProPathName(L"\map");
//	BSTR tempB;
//	tempB = temp.AllocSysString();
	m_pControl = CSDEControler::GetSDEControl();
	m_pControl->InitSDE(temp);
	m_pMapManager = m_pControl->GetMapManager();
	if(m_pMapManager != NULL)
	{
		IGIS_Map* pMap;
		pMap = (IGIS_Map*)(m_pMapManager->GetFirstMap());
		while(pMap != NULL)
		{
			pMap->Attach(this);
			pMap = (IGIS_Map*)(m_pMapManager->GetNext(pMap));
		}
	}
//	m_pNetwork = (IGIS_NetWork*)m_pControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );//new CGIS_NetWork;

//	CRect rtClient;
//	GetClientRect(rtClient);
//	plotRect.right = rtClient.right+2;
//	plotRect.bottom = rtClient.bottom +2;

	AdjustBuf();
	m_CursorPt.x = plotRect.right / 2;
	m_CursorPt.y = plotRect.bottom / 2;
	if(m_pMapManager != NULL)
	{
		IGIS_Map* pMap;
		static int n = 0;
		if( n == 0 )
		{
			pMap = (IGIS_Map*)(m_pMapManager->GetFirstMap());
			n = 1;
		}
		else if( n == 1 )
		{
			pMap = (IGIS_Map*)(m_pMapManager->GetLastMap());
			n = 0;
		}
		if(pMap != NULL)
		{
			//用户设置的初始位置
			m_pMap = pMap;
//			double dx = pMap->GetMapInfo()->GetMapRect().dLeft + (pMap->GetMapInfo()->GetMapRect().dRight - pMap->GetMapInfo()->GetMapRect().dLeft) / 2;
//			double dy = pMap->GetMapInfo()->GetMapRect().dTop + (pMap->GetMapInfo()->GetMapRect().dBottom - pMap->GetMapInfo()->GetMapRect().dTop) / 2;
			//香港
			double dx = 114.1640;
			double dy = 22.32;
//			double dx = 114.16868;
//			double dy = 22.317844;
			//德国
//			double dx = 8.33318;
//			double dy = 49.863021;
//			double dx = 7.526139;
//			double dy = 49.416544;
			m_ptCenter.dx = dx;
			m_ptCenter.dy = dy;
			SetScale( dx, dy, 1 );

			Refresh();
		}
	}	// TODO: Add your specialized creation code here
	
	SetDrawStatic();
	SetMapParticular();



//	double dx1 = 50;
//	double dy1 = 31.1;
//	double dx2 = 120.2;
//	double dy2 = 45.2;
//	
//	DWORD dwTime = GetTickCount();
//	for( int i = 0 ; i < 10000 ; i++ )
//		CalcDist( dx1 , dy1 , dx2 , dy2 );
//	TRACE( L" DIS1: %d \n " , GetTickCount() - dwTime );
//	dwTime = GetTickCount();
//	for( i = 0 ; i < 10000 ; i++ )
//		CalcDist2( dx1 , dy1 , dx2 , dy2 );
//	TRACE( L" DIS2: %d \n " , GetTickCount() - dwTime );
//	dwTime = GetTickCount();
//	for( i = 0 ; i < 10000 ; i++ )
//		CalcDist3( dx1 , dy1 , dx2 , dy2 );
//	TRACE( L" DIS3: %d \n " , GetTickCount() - dwTime );

	*/

    m_pControl = CSDEControler::GetSDEControl();
	return 0;

}
void CSDETestView::GreatCircle(float PointA_Latitude, float PointA_Longitude,
		 float PointB_Latitude, float PointB_Longitude,
		 float *GreatCircleDistance, float *GreatCircleBearing)
{
    float Cos_PointA_Latitude;             /* Cosine of point A latitude. */
    float Cos_PointB_Latitude;             /* Cosine of point B latitude. */
    float Sin_PointA_Latitude;               /* Sine of point A latitude. */
    float Sin_PointB_Latitude;               /* Sine of point B latitude. */
    float Delta_Longitude;        /* Longitude differance B - A, radians. */
    float Cos_Delta_Longitude;              /* Cosine of delta longitude. */
    float Sin_Delta_Longitude;                /* Sine of delta longitude. */
    float Sin_Distance;                 /* Sine of great circle distance. */
    float Cos_Distance;               /* Cosine of great circle distance. */
    float Sin_Bearing;                   /* Sine of great circle bearing. */
    float Cos_Bearing;                 /* Cosine of great circle bearing. */
    float intermeadiate;                 /* An intermeadiate calculation. */

    /* Calculate some sines and cosines now to save time later. */

    Cos_PointA_Latitude = (float)cos(PointA_Latitude);
    Cos_PointB_Latitude = (float)cos(PointB_Latitude);
    Sin_PointA_Latitude = (float)sin(PointA_Latitude);
    Sin_PointB_Latitude = (float)sin(PointB_Latitude);
    Delta_Longitude = (float)(PointB_Longitude - PointA_Longitude);
    Cos_Delta_Longitude = (float)cos(Delta_Longitude);
    Sin_Delta_Longitude = (float)sin(Delta_Longitude);

    /* Get the cosine of the great circle distance. */

    Cos_Distance = Sin_PointA_Latitude*Sin_PointB_Latitude
	       + Cos_PointA_Latitude*Cos_PointB_Latitude*Cos_Delta_Longitude;

    /* Get the sine of the great circle distance. */

    intermeadiate = DSquare(Cos_PointB_Latitude*Sin_Delta_Longitude)
    + DSquare(Cos_PointA_Latitude*Sin_PointB_Latitude
	      - Sin_PointA_Latitude*Cos_PointB_Latitude*Cos_Delta_Longitude);

    Sin_Distance = (float)sqrt(fabs(intermeadiate));

    /* Get the the great circle distance. */

    if(fabs(Sin_Distance)>.0000001 || fabs(Cos_Distance)>.0000001)
		*GreatCircleDistance = (float)atan2(Sin_Distance,Cos_Distance);
    else
		*GreatCircleDistance = 0.0;                         /* Same point. */

    /* Get the sine of the great circle bearing. */

    Sin_Bearing = Sin_Delta_Longitude*Cos_PointB_Latitude;

    /* Get the cosine of the great circle bearing. */

    Cos_Bearing = Cos_PointA_Latitude*Sin_PointB_Latitude
	       - Sin_PointA_Latitude*Cos_PointB_Latitude*Cos_Delta_Longitude;

    /* Get the great circle bearing. */

    if(fabs(Sin_Bearing)>.0000001 || fabs(Cos_Bearing)>.0000001)
    {
		intermeadiate = (float)atan2(Sin_Bearing,Cos_Bearing);
		while(intermeadiate<0.0)
			intermeadiate += (float)(2.0*PI);
		*GreatCircleBearing = intermeadiate;
    }
    else
		*GreatCircleBearing = 0.0;            /* Due north or co-incident. */
}
double CSDETestView::CalcDist2(double dx1, double dy1, double dx2, double dy2)
{
	
    float a_dlat,a_dlog,b_dlat,b_dlog;
    float distance,bearing;
    a_dlat=(float)(dx1/180.0*PI);
    a_dlog=(float)(dy1/180.0*PI);
    b_dlat=(float)(dx2/180.0*PI);
    b_dlog=(float)(dy2/180.0*PI);
    GreatCircle(a_dlog,a_dlat,b_dlog,b_dlat,&distance,&bearing);
//    GreatCircle(a_dlat,a_dlog,b_dlat,b_dlog,&distance,&bearing);
    double dis2 =  (distance*180.0*60.0/PI*1852.0);
	return dis2;
}
double CSDETestView::CalcDist3(double dx1, double dy1, double dx2, double dy2)
{
	double D;
	double D1;

	dx1 = (dx1 * PI)/180.0;
	dy1 = (dy1 * PI)/180.0;
	dx2 = (dx2 * PI)/180.0;
	dy2 = (dy2 * PI)/180.0;

	D = sin(dy1)*sin(dy2) + cos(dy1)*cos(dy2)*cos(dx2-dx1);  //东经 北纬 正

	if(D > 1.0)												 //in wince possible >1.0
		D1 = 0.0;
	else if(D < -1.0)
		D1 = PI;
	else
		D1 = acos(D);

	D = D1 * (180.0/PI);

	double Dis3 = D * 111.23 *1000;									//经验值 111.23 kilometers/degree

	Dis3 = fabs(Dis3);

	return Dis3;	

} 
void CSDETestView::SetDrawStatic()
{
	IGIS_Map *pMap = NULL;
//	if(bDay)
	{
		SetBackColor(RGB(240,240,208));
		pMap= (IGIS_Map*)(m_pMapManager->GetFirstMap());
		while(pMap)
		{
			if(	pMap->GetMapInfo()->GetMapID() == MAPID_UM_NAVI )
			{
				pMap = (IGIS_Map*)m_pMapManager->GetNext( pMap );
				continue;
			}
			if( pMap->GetMapInfo()->GetMapID() == MAPID_MAP_NATION )
			{
				for( int j = 0 ; j < 2 ; j++ )
				{
					CGIS_LayerInfo *pLayerInfo = pMap->GetLayerInfo(j);
					switch(j)
					{
					case 0:
						SetMapColor(pLayerInfo,RGB(0,0,0),RGB(0,0,0));				
						break;
					case 1:
						SetMapColor(pLayerInfo,RGB(48,48,248),RGB(48,48,248));
						break;
					default:
						break;
					}
				}
				pMap = (IGIS_Map*)m_pMapManager->GetNext( pMap );
				continue;
			}
			for(int j=0; j<12; j++)
			{
				CGIS_LayerInfo *pLayerInfo = pMap->GetLayerInfo(j);
				switch(j) {
				case 0:
					SetMapColor(pLayerInfo,RGB(248,196,112),RGB(248,196,112));
					break;
				case 1:
					SetMapColor(pLayerInfo,RGB(240,248,160),RGB(240,248,160));
					break;
				case 2:
					SetMapColor(pLayerInfo,RGB(0,192,0),RGB(0,192,0));
					break;
				case 3:
					SetMapColor(pLayerInfo,RGB(48,48,248),RGB(48,48,248));
					break;
				case 4:
					SetMapColor(pLayerInfo,RGB(80,80,80),RGB(248,248,248));
					break;
				case 5:
					SetMapColor(pLayerInfo,RGB( 128,128,128),RGB( 176,176,176));
					break;
				case 6:
					SetMapColor(pLayerInfo,RGB( 184,112,0),RGB( 248,160,0));
					break;
				case 7:
					SetMapColor(pLayerInfo,RGB( 184,112,0),RGB( 248,160,0));
					break;
				case 8:
					SetMapColor(pLayerInfo,RGB( 184,112,0),RGB( 248,160,0));
					break;
				case 9:
					SetMapColor(pLayerInfo,RGB( 184,112,0),RGB( 248,160,0));
					break;
				case 10:
					SetMapColor(pLayerInfo,RGB( 184,112,0),RGB( 248,160,0));
					break;
				case 11:
					SetMapColor(pLayerInfo,RGB(160,16,16),RGB(200,24,32));
					break;
				}
			}
			SetbDayShow(TRUE);
			pMap = (IGIS_Map*)(m_pMapManager->GetNext(pMap));
		}
	}

	return ;
}
void CSDETestView::SetMapColor(CGIS_LayerInfo *layer, COLORREF brCol, COLORREF penCol)
{
	ST_GEO_BRUSH brush = layer->GetBrush();
	brush.nForeColor = brCol;
	ST_GEO_PEN   pen = layer->GetPen();
	if (penCol != -1)
	{
		pen.nColor = penCol;
	}
	layer->SetBrush(brush);
	layer->SetPen(pen);
}
void CSDETestView::SetMapParticular()
{
	int level[LAYER_PT_END+1];
	GetDefaultLevel(level);

	ASSERT(m_pMapManager != NULL); 
	if (m_pMapManager == NULL)
	{
		return;
	}
	IGIS_Map *pMap = (IGIS_Map*)(m_pMapManager->GetFirstMap());
	while(pMap)
	{
		if(pMap->GetMapInfo()->GetMapID() == MAPID_MAP_NATION	||
			pMap->GetMapInfo()->GetMapID() == MAPID_UM_NAVI )
		{
			pMap = (IGIS_Map*)m_pMapManager->GetNext( pMap );
			continue;
		}
		for(int j = 0 ; j <= LAYER_PT_END ; j++)
		{
			CGIS_LayerInfo *pLayerInfo = pMap->GetLayerInfo(j);
			double scale = GetScale(level[j]);
			pLayerInfo->SetMaxARScale( 1000000 );
			pLayerInfo->SetMaxGRScale( 1000000 );
			if(fabs(scale-0)<ZERO)
			{
				pLayerInfo->SetMinGRScale(0);
				pLayerInfo->SetMinARScale(0);
			}
			else if( fabs(scale-(-1))<ZERO )
			{
				pLayerInfo->SetMinGRScale(1000000000);
				pLayerInfo->SetMinARScale(1000000000);
			}
			else
			{
				pLayerInfo->SetMinGRScale(1852*60 / scale -10);
				pLayerInfo->SetMinARScale(1852*60 / scale -10);
			}
		}
		pMap = (IGIS_Map*)(m_pMapManager->GetNext(pMap));
	}
}
void CSDETestView::GetDefaultLevel(int *nLevel)
{
	if(nLevel)
	{
		nLevel[0] = 100;
		nLevel[1] = 2;
		nLevel[2] = 5;
		nLevel[3] = 100;
		nLevel[4] = 3;
		nLevel[5] = 3;
		nLevel[6] = 5;
		nLevel[7] = 5;
		nLevel[8] = 6;
		nLevel[9] = 7;
		nLevel[10] = 8;
		nLevel[11] = 100;
		nLevel[12] = 2;
		nLevel[13] = 2;
		nLevel[14] = 1;
		nLevel[15] = 2;
		nLevel[16] = 3;
		nLevel[17] = 3;
		nLevel[18] = 1;
		nLevel[19] = 1;
		nLevel[20] = 1;
		nLevel[21] = 2;
		nLevel[22] = 2;
		nLevel[23] = 2;
		nLevel[24] = 100;
		nLevel[25] = 1;
	}
}

void CSDETestView::OnFileOpen() 
{
	// TODO: Add your command handler code here
}

//点击显示地图按钮
void CSDETestView::OnViewmap() 
{
	// TODO: Add your command handler code here
	m_pMapManager = m_pControl->GetMapManager();
    ASSERT(m_pMapManager);
    
    //把视图attach到地图上.(这样地图数据在更新时可以通知到视)
    IGIS_Map* pMap;
    pMap = (IGIS_Map*)(m_pMapManager->GetFirstMap());
    while(pMap != NULL)
    {
        pMap->Attach(this);
        pMap = (IGIS_Map*)(m_pMapManager->GetNext(pMap));
    }

//	m_pNetwork = (IGIS_NetWork*)m_pControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );//new CGIS_NetWork;

//	CRect rtClient;
//	GetClientRect(rtClient);
//	plotRect.right = rtClient.right+2;
//	plotRect.bottom = rtClient.bottom +2;

	AdjustBuf(); //调整显示区域，初始化视图需要的多个设备资源
	m_CursorPt.x = plotRect.right / 2;
	m_CursorPt.y = plotRect.bottom / 2; //view的中心点
	if(m_pMapManager != NULL)
	{
		IGIS_Map* pMap;
		static int n = 0;
		if( n == 0 )
		{
			pMap = (IGIS_Map*)(m_pMapManager->GetFirstMap());
			n = 1;
		}
		else if( n == 1 )
		{
			pMap = (IGIS_Map*)(m_pMapManager->GetLastMap());
			n = 0;
		}
		if(pMap != NULL)
		{
			//用户设置的初始位置
			m_pMap = pMap;
//			double dx = pMap->GetMapInfo()->GetMapRect().dLeft + (pMap->GetMapInfo()->GetMapRect().dRight - pMap->GetMapInfo()->GetMapRect().dLeft) / 2;
//			double dy = pMap->GetMapInfo()->GetMapRect().dTop + (pMap->GetMapInfo()->GetMapRect().dBottom - pMap->GetMapInfo()->GetMapRect().dTop) / 2;
			//香港
//			double dx = 114.1640;
//			double dy = 22.32;
//			double dx = 114.16868;
//			double dy = 22.317844;
			//德国
//			double dx = 8.33318;
//			double dy = 49.863021;
//			double dx = 7.526139;
//			double dy = 49.416544;
			//上海
			double dx = 121.395139;
			double dy = 31.160859;
			//芜湖
			//double dx = 118.378143;
 			//double dy = 31.369787;
			//北京
			dx = 117.107574;
			dy = 39.117363;

			dx = 121.462959;
			dy = 31.241604;






            //////////////////////////////////////////////////////////////////////////
			m_ptCenter.dx = dx;
			m_ptCenter.dy = dy;


			ST_GEO_PTXY ptxy;
            ptxy.dx = dx;
			ptxy.dy = dy;
            SetCurPt( ptxy );
            double arScale[15] = 
            {
                0.200000f,
                0.600000f,
                1.000000f,
                2.000000f,
                4.000000f,
                8.000000f,
                15.000000f,
                30.000000f,
                60.000000f,
                120.000000f,
                300.000000f,
                600.000000f,
                1200.000000f,
                2000.000000f,
                5000.000000f,       
            };
            SetScale(arScale,15);
            static BOOL bFirst = TRUE;
            if(bFirst)
            {
                bFirst = FALSE;
			    SetScale( m_ptCenter.dx, m_ptCenter.dy, 3 );
            }
            else
            {
                double dx,dy;
                XyViewToMap(m_CursorPt.x,m_CursorPt.y,dx,dy);    
                m_ptCenter.dx = dx;
                m_ptCenter.dy = dy;
            }

			SetScale( m_ptCenter.dx, m_ptCenter.dy, 3 );
			Refresh();
		}
	}	// TODO: Add your specialized creation code here
	
	SetDrawStatic();
	SetMapParticular();



//	double dx1 = 50;
//	double dy1 = 31.1;
//	double dx2 = 120.2;
//	double dy2 = 45.2;
//	
//	DWORD dwTime = GetTickCount();
//	for( int i = 0 ; i < 10000 ; i++ )
//		CalcDist( dx1 , dy1 , dx2 , dy2 );
//	TRACE( L" DIS1: %d \n " , GetTickCount() - dwTime );
//	dwTime = GetTickCount();
//	for( i = 0 ; i < 10000 ; i++ )
//		CalcDist2( dx1 , dy1 , dx2 , dy2 );
//	TRACE( L" DIS2: %d \n " , GetTickCount() - dwTime );
//	dwTime = GetTickCount();
//	for( i = 0 ; i < 10000 ; i++ )
//		CalcDist3( dx1 , dy1 , dx2 , dy2 );
//	TRACE( L" DIS3: %d \n " , GetTickCount() - dwTime );
	Refresh();	
}

void CSDETestView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
    {
    case VK_LEFT:
        {
            double dx,dy,dx1, dy1, dx2,dy2;
            XyViewToMap(plotRect.left ,plotRect.top ,dx1,dy1);
            XyViewToMap(plotRect.right ,plotRect.bottom ,dx2,dy2);
            dx = dx1 + (dx2-dx1)/2; // 可视区域中心点地理坐标
            dy = dy1 + (dy2-dy1)/2;

            dx -= (dx2-dx1)/4; //中心点设置为offset: 1/4  
            MoveToCenter(dx,dy);
            Refresh();
        }
        break;
    case VK_RIGHT:
        {
            double dx,dy,dx1, dy1, dx2,dy2;
            XyViewToMap(plotRect.left ,plotRect.top ,dx1,dy1);
            XyViewToMap(plotRect.right ,plotRect.bottom ,dx2,dy2);
            dx = dx1 + (dx2-dx1)/2;
            dy = dy1 + (dy2-dy1)/2;

            dx += (dx2-dx1)/4;
            MoveToCenter(dx,dy);
            Refresh();
        }
        break;
    case VK_UP:
        {
            double dx,dy,dx1, dy1, dx2,dy2;
            XyViewToMap(plotRect.left ,plotRect.top ,dx1,dy1);
            XyViewToMap(plotRect.right ,plotRect.bottom ,dx2,dy2);
            dx = dx1 + (dx2-dx1)/2;
            dy = dy1 + (dy2-dy1)/2;

            dy -= (dy2-dy1)/4;
            MoveToCenter(dx,dy);
            Refresh();
        }
            break;
    case VK_DOWN:
        {
            double dx,dy,dx1, dy1, dx2,dy2;
            XyViewToMap(plotRect.left ,plotRect.top ,dx1,dy1);
            XyViewToMap(plotRect.right ,plotRect.bottom ,dx2,dy2);
            dx = dx1 + (dx2-dx1)/2;
            dy = dy1 + (dy2-dy1)/2;

            dy += (dy2-dy1)/4;
            MoveToCenter(dx,dy);
            Refresh();
        }
        break;
    case VK_HOME:
        OnZoomin();
        break;
    case VK_END:
        OnZoomout();
        break;
    default:
        break;
    }
    
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSDETestView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
    {
    case VK_LEFT:
         break;
    case VK_RIGHT:
        break;
    case VK_UP:
         break;
    case VK_DOWN:
        break;
    case VK_HOME:
        break;
    case VK_END:
        break;
    default:
        break;
    }
	
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}
