// RichPOI.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "RichPOI.h"
#include "SDEControler.h"
#include "IGIS_MapManager.h"
#include "../MainFrm.h"
#include "GeoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRichPOI dialog


CRichPOI::CRichPOI(CWnd* pParent /*=NULL*/)
	: CDialog(CRichPOI::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRichPOI)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pQueryByAtt = NULL;
	m_nLevel1 = 16777216;
}


void CRichPOI::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRichPOI)
	DDX_Control(pDX, IDC_BUTTON_CHANGELEVEL1, m_ChangeLevel1);
	DDX_Control(pDX, IDC_LIST_RICHPOI, m_ListRichPOI);
	DDX_Control(pDX, IDC_LIST_CITY, m_ListCity);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRichPOI, CDialog)
	//{{AFX_MSG_MAP(CRichPOI)
	ON_BN_CLICKED(IDC_BUTTON_CHANGELEVEL1, OnButtonChangelevel1)
	ON_LBN_DBLCLK(IDC_LIST_CITY, OnDblclkListCity)
	ON_LBN_DBLCLK(IDC_LIST_RICHPOI, OnDblclkListRichpoi)
	ON_BN_CLICKED(IDC_BUTTON_MOVE, OnButtonMove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichPOI message handlers

BOOL CRichPOI::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	if( !m_pQueryByAtt )
	{
		CSDEControler* pControl = NULL;
		pControl = CSDEControler::GetSDEControl();
		IGIS_MapManager* pMapManager = pControl->GetMapManager();
		if( pMapManager != NULL)
		{
			IGIS_Map* pMap;
			pMap = (IGIS_Map*)(pMapManager->GetFirstMap());
			m_pQueryByAtt = (CGIS_QueryByAtt*)pMap->GetTool( QUERY_BYATT );
		}
	}
	SetCityList();
	if( m_nLevel1 == 16777216 )
		m_ChangeLevel1.SetWindowText( L"奇瑞经销商");
	else
		m_ChangeLevel1.SetWindowText( L"奇瑞服务站");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRichPOI::OnButtonChangelevel1() 
{
	// TODO: Add your control notification handler code here
	if( m_nLevel1 == 16777216 )
	{
		m_nLevel1 = 33554432;
		m_ChangeLevel1.SetWindowText( L"奇瑞服务站");
	}
	else
	{
		m_nLevel1 = 16777216;
		m_ChangeLevel1.SetWindowText( L"奇瑞经销商");
	}

	SetCityList();
	UpdateData();
}
void CRichPOI::SetCityList()
{
	m_ListCity.ResetContent();
	m_ListRichPOI.ResetContent();
	CPtrList* pLevelList = m_pQueryByAtt->GetLevelList();
	RICHPOI_LEVEL1 *pRichPOILevel1;
	RICHPOI_LEVEL2 *pRichPOILevel2;
	RICHPOI_LEVEL3 *pRichPOILevel3;
	int nItem = 0;
	if( pLevelList )
	{
		pRichPOILevel1 = (RICHPOI_LEVEL1*)pLevelList->GetHead();
		if( m_nLevel1 == 33554432 )
			pRichPOILevel1 = (RICHPOI_LEVEL1*)pLevelList->GetTail();
		POSITION pos1 = pRichPOILevel1->pLevel2List.GetHeadPosition();
		while( pos1 )
		{
			pRichPOILevel2 = (RICHPOI_LEVEL2*)( pRichPOILevel1->pLevel2List.GetNext(pos1) );
			POSITION pos = pRichPOILevel2->pLevel3List.GetHeadPosition();
			while( pos )
			{
				pRichPOILevel3 = (RICHPOI_LEVEL3*)( pRichPOILevel2->pLevel3List.GetNext( pos ) );
				m_ListCity.AddString( pRichPOILevel3->strLevel3Name );
			}
		}
		pos1 = pRichPOILevel1->pLevel2List.GetHeadPosition();
		nItem = 0;
		while( pos1 )
		{
			pRichPOILevel2 = (RICHPOI_LEVEL2*)( pRichPOILevel1->pLevel2List.GetNext(pos1) );
			POSITION pos = pRichPOILevel2->pLevel3List.GetHeadPosition();
			while( pos )
			{
				pRichPOILevel3 = (RICHPOI_LEVEL3*)( pRichPOILevel2->pLevel3List.GetNext( pos ) );
				m_ListCity.SetItemData( nItem,(int)pRichPOILevel3->dwLevel3ID );
				nItem++;
			}
		}
	}
}

void CRichPOI::OnDblclkListCity() 
{
	// TODO: Add your control notification handler code here
	m_ListRichPOI.ResetContent();
	ClearRichPOI();
	int nCueSel = m_ListCity.GetCurSel();
	int nSelCityID = m_ListCity.GetItemData( nCueSel );

	m_pQueryByAtt->GetRichPOIByCityID( m_listResult , m_nLevel1 , nSelCityID );

	RICHPOI_INFO* pRichPOIInfo = NULL;
	POSITION pos = m_listResult.GetHeadPosition();
	while( pos )
	{
		pRichPOIInfo = (RICHPOI_INFO*)m_listResult.GetNext( pos );
		m_ListRichPOI.AddString( pRichPOIInfo->strRichPoiName );
	}
	pos = m_listResult.GetHeadPosition();
	int nItem = 0;
	POSITION posSave;
	while( pos )
	{
		posSave = pos;
		pRichPOIInfo = (RICHPOI_INFO*)m_listResult.GetNext( pos );
		m_ListRichPOI.SetItemData( nItem , (int)posSave );
		nItem++;
	}

	UpdateData();
}

void CRichPOI::OnDblclkListRichpoi() 
{
	// TODO: Add your control notification handler code here
	int nSelItem = m_ListRichPOI.GetCurSel();
	POSITION pos = (POSITION)m_ListRichPOI.GetItemData( nSelItem );
	RICHPOI_INFO* pRichPOIInfo = (RICHPOI_INFO*)m_listResult.GetAt( pos );
	CString strText;
	m_pQueryByAtt->GetRichPOIText( *pRichPOIInfo , strText );
	
	AfxMessageBox( strText );
	
}
void CRichPOI::ClearRichPOI()
{
	POSITION pos = m_listResult.GetHeadPosition();
	RICHPOI_INFO* pRichPOi = NULL;
	while( pos )
	{
		pRichPOi = (RICHPOI_INFO*)m_listResult.GetNext( pos );
		delete pRichPOi;
		pRichPOi = NULL;
	}
	m_listResult.RemoveAll();
}


void CRichPOI::OnButtonMove() 
{
	// TODO: Add your control notification handler code here
	CGeoView* pView = (CGeoView*)( (CMainFrame*)AfxGetMainWnd())->GetActiveView();
 	int nSelItem = m_ListRichPOI.GetCurSel();
	POSITION pos = (POSITION)m_ListRichPOI.GetItemData( nSelItem );
	RICHPOI_INFO* pRichPOIInfo = (RICHPOI_INFO*)m_listResult.GetAt( pos );
    pView->MoveToCenter(pRichPOIInfo->fLon,pRichPOIInfo->fLat);
   pView->Refresh();
	
}
