// DlgTestNavi.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "DlgTestNavi.h"

#include "SDEControler.h"
#include "GIS_PathpointMark.h"
#include "GIS_PathpointNode.h"
#include "GIS_UserMap.h"
#include "GIS_FeatureRoute.h"
#include "BlockID.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTestNavi dialog


CDlgTestNavi::CDlgTestNavi(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTestNavi::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTestNavi)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pNWPInfo = new CGIS_NetworkPathInfo( _T("D:\\SDE\\SDE_Aboard\\SDETest\\Debug\\map\\voice.txt") );
	m_pNWPInfo->SetVehicleType(EN_VEHICLETYPE_CAR);
	m_pNWPInfo->SetRoutePreference(EN_RUTPREF_FASTERTIME);
}


void CDlgTestNavi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTestNavi)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTestNavi, CDialog)
	//{{AFX_MSG_MAP(CDlgTestNavi)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTestNavi message handlers

void CDlgTestNavi::OnButton1() 
{
	// TODO: Add your control notification handler code here
	BOOL	bSuccess = FALSE;	//航线计算成功标志.TRUE-成功
	//取得SDE控制器
	CSDEControler* pControl = CSDEControler::GetSDEControl();
	ASSERT(pControl);

	//取得航线计算工具
	IGIS_NetWork *pNetwork = (IGIS_NetWork*)pControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );
	ASSERT(pNetwork);

	CGIS_WayPoint * pStart = new CGIS_WayPoint(0);
	CGIS_WayPoint * pEnd = new CGIS_WayPoint(1);

	ST_GEO_PTXY ptStart,ptEnd;
//	ptStart.dx	= 114.1640f;
//	ptStart.dy	= 22.3220f;
//	ptEnd.dx	= 114.1644f;
//  ptEnd.dy	= 22.3228f;
	ptStart.dx	= 114.1640f;
	ptStart.dy	= 22.3230f;
	ptEnd.dx	= 114.162048f;
 	ptEnd.dy	= 22.320475f;


//	ptStart.dx	= 114.055606f;
//	ptStart.dy	= 22.385580f;
//	ptEnd.dx	= 114.062534f;
//	ptEnd.dy	= 22.407626f;

//此值可用来做8公里上的测试
//	ptStart.dx	= 114.192637f;
//	ptStart.dy	= 22.257686f;
//	ptEnd.dx	= 114.021355f;
//	ptEnd.dy	= 22.448821f;
// 
 
	pStart->SetWPAnnoation("1234");
	pStart->SetWPAnnoNum(5);
	pStart->SetWPPosition( ptStart );
	pStart->SetWPType(EN_WAYPOINTTYPE_ORIGIN);

	pEnd->SetWPAnnoation("5678");
	pEnd->SetWPAnnoNum(5);
	pEnd->SetWPPosition( ptEnd );
	pEnd->SetWPType(EN_WAYPOINTTYPE_DESTINATION);

	CGIS_FeatureRoute * pRoute = new CGIS_FeatureRoute;
	CGIS_NetworkRoute * pNWRoute = pRoute->GetNetworkRoute();

	CPtrList * pList = pNWRoute->GetWayPointList();
	pList->AddHead( pStart );
	pList->AddTail( pEnd );

	CGIS_NetworkPathInfo* pPathInfo	=new CGIS_NetworkPathInfo(L"");
	pPathInfo->SetRoutePreference( EN_RUTPREF_FASTERTIME );
	pNWRoute->SetNetworkPathInfo( pPathInfo );

	//如果该航线已经计算过，则删除相关信息
	if( pNWRoute->GetNWRouteState() )
	{
		CPtrList *pPPList = pNWRoute->GetNetworkPath()->GetPathpointList();
		VERIFY( pPPList );
		POSITION pos = pPPList->GetHeadPosition();
		while( pos )
		{
			CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPPList->GetNext(pos);
			if( pPPoint )
				delete pPPoint;
		}
		pPPList->RemoveAll();
	}

	//开始计算航线
	pNetwork->SetNetworkRoute(pNWRoute);

	CGIS_WayPoint * pHead = NULL;


//	pNWRoute->SetNetworkPathInfo( m_pPathInfo );

	if ( !pNetwork->CalculateOptimalRoute( 0 ) )
	{
		bSuccess = FALSE;

	}

}

BOOL CDlgTestNavi::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTestNavi::OnButton2() 
{
	// TODO: Add your control notification handler code here

}

void CDlgTestNavi::OnButton3() 
{

	//LLRECT rtTotal(113.173,22.8091,115.106,21.4610);
	LLRECT rtTotal(114.160,22.3234,114.166,22.3183);
	LLRECT rtBreadth;
	// TODO: Add your control notification handler code here
	VERIFY(GetRectByID(0X10000001,rtTotal, rtBreadth));
           
}
