// ExpandMap.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "ExpandMap.h"
#include "GeoView.h"
#include "GIS_NetworkRoute.h"
#include "GIS_PathPoint.h"
#include "GIS_PathpointNode.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExpandMap dialog


CExpandMap::CExpandMap(CWnd* pParent /*=NULL*/)
	: CDialog(CExpandMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExpandMap)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_posCurrent = NULL;
	m_pDrawDC = NULL;
}


void CExpandMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpandMap)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExpandMap, CDialog)
	//{{AFX_MSG_MAP(CExpandMap)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, OnButtonPrevious)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_MOVETO, OnButtonMoveto)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExpandMap message handlers

void CExpandMap::OnButtonNext() 
{
	// TODO: Add your control notification handler code here
	CGeoView* pView = (CGeoView*)( (CMainFrame*)AfxGetMainWnd())->GetActiveView();
	CGIS_NetworkRoute* pNetworkRoute = pView->GetNetworkRoute();
	if( !pNetworkRoute )
		return;
	CGIS_PathpointNode* pPathPointNode = NULL;
	CGIS_Pathpoint*	pPathPoint = NULL;
	CPtrList* pPathPointList = pNetworkRoute->GetNetworkPath()->GetPathpointList();

	POSITION pos = m_posCurrent ;
	if( !m_posCurrent )
		pos = pPathPointList->GetHeadPosition();
	else
		pPathPoint = (CGIS_Pathpoint*)( pPathPointList->GetNext( pos ) );
		

	while( pos )
	{
		POSITION posOld = pos;
		pPathPoint = (CGIS_Pathpoint*)( pPathPointList->GetNext( pos ) );
		if( pPathPoint && pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE)
		{
			pPathPointNode = (CGIS_PathpointNode*)pPathPoint;
			ST_LANDINFO* pLandInfo =  pPathPointNode->GetLaneInfo();
			if( pLandInfo )
			{
				int kk = 10;
			}
			if( pPathPointNode )
			{
				m_posCurrent = posOld;
				m_pDrawDC = pView->GetExpandMap( m_posCurrent );
				break;
			}
		}
	}
	CDC* pDC = GetDC();
	if( m_pDrawDC )
	{
		pDC->BitBlt(0,35, 180,196+35 ,  m_pDrawDC, 0, 0, SRCCOPY);	
	}
	ReleaseDC( pDC );
}

void CExpandMap::OnButtonPrevious() 
{
	// TODO: Add your control notification handler code here
	CGeoView* pView = (CGeoView*)( (CMainFrame*)AfxGetMainWnd())->GetActiveView();
	CGIS_NetworkRoute* pNetworkRoute = pView->GetNetworkRoute();
	if( !pNetworkRoute )
		return;
	CGIS_PathpointNode* pPathPointNode = NULL;
	CGIS_Pathpoint*	pPathPoint = NULL;
	CPtrList* pPathPointList = pNetworkRoute->GetNetworkPath()->GetPathpointList();

	POSITION pos = m_posCurrent ;
	if( !m_posCurrent )
		return;
	else
		pPathPoint = (CGIS_Pathpoint*)( pPathPointList->GetPrev(pos) );
		

	while( pos )
	{
		POSITION posOld = pos;
		pPathPoint = (CGIS_Pathpoint*)( pPathPointList->GetPrev( pos ) );
		if( pPathPoint && pPathPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
		{
			pPathPointNode = (CGIS_PathpointNode*)pPathPoint;
			if( pPathPointNode )
			{
				m_posCurrent = posOld;
				m_pDrawDC = pView->GetExpandMap( m_posCurrent );
				break;
			}
		}
	}
	CDC* pDC = GetDC();
	if( m_pDrawDC )
	{
		pDC->BitBlt(0,35, 180,196+35 ,  m_pDrawDC, 0, 0, SRCCOPY);	
	}
	ReleaseDC( pDC );
}

void CExpandMap::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	if( m_pDrawDC )
	{
		dc.BitBlt(0,35, 180,196+35 ,  m_pDrawDC, 0, 0, SRCCOPY);	
	}
	
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CExpandMap::OnButtonMoveto() 
{
	// TODO: Add your control notification handler code here
	CGeoView* pView = (CGeoView*)( (CMainFrame*)AfxGetMainWnd())->GetActiveView();
	CGIS_NetworkRoute* pNetworkRoute = pView->GetNetworkRoute();
	if( !pNetworkRoute )
		return;
	if( !m_posCurrent )
		return;
	CGIS_Pathpoint*	pPathPoint = NULL;
	CPtrList* pPathPointList = pNetworkRoute->GetNetworkPath()->GetPathpointList();

	pPathPoint = (CGIS_Pathpoint*)pPathPointList->GetAt( m_posCurrent );
	
    pView->MoveToCenter(pPathPoint->GetPathpointPosition().dx,pPathPoint->GetPathpointPosition().dy);
	pView->Refresh();
	
}
