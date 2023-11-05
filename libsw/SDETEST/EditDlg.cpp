// EditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "EditDlg.h"

#include "GIS_LayerTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDlg dialog


CEditDlg::CEditDlg(IGIS_MapManager *pMapManager, CWnd* pParent /*=NULL*/)
	: CDialog(CEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditDlg)
	m_strMName = _T("shanghai");
	m_dbLeft = 120.046f;
	m_dbTop = 30.1703f;
	m_dbRight = 120.307f;
	m_dbBotton = 30.3763f;
	m_strFName = _T("\\map\\shanghai\\shanghai.mp2");
	m_strFieldName = _T("mingcheng");
	m_nSize = 16;
	m_strValue = _T("chengming");
	m_strDefValue = _T("mingcheng");
	m_strLName = _T("uselayer");
	m_strLFName = _T("\\map\\shanghai\\shanghai1.lay");
	m_strFeatureName = _T("chengming");
	m_dbX = 120.17646f;
	m_dbY = 30.2733f;
	//}}AFX_DATA_INIT
	m_pMapManager = pMapManager;
	m_pMap = NULL;
	m_pLayer = NULL;
	m_pFeature = NULL;

}


void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDlg)
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ctlTCom);
	DDX_Control(pDX, IDC_LIST_FIELDS, m_ctlFList);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMName);
	DDX_Text(pDX, IDC_EDIT2, m_dbLeft);
	DDX_Text(pDX, IDC_EDIT3, m_dbTop);
	DDX_Text(pDX, IDC_EDIT7, m_dbRight);
	DDX_Text(pDX, IDC_EDIT4, m_dbBotton);
	DDX_Text(pDX, IDC_EDIT5, m_strFName);
	DDX_Text(pDX, IDC_EDIT_FIELDNAME, m_strFieldName);
	DDX_Text(pDX, IDC_EDIT_FIELDSIZE, m_nSize);
	DDX_Text(pDX, IDC_EDIT_FIELDVALUE, m_strValue);
	DDX_Text(pDX, IDC_EDIT_FIELDDEFVALUE, m_strDefValue);
	DDX_Text(pDX, IDC_EDIT_LAYNAME, m_strLName);
	DDX_Text(pDX, IDC_EDIT_LAYFNAME, m_strLFName);
	DDX_Text(pDX, IDC_EDIT_FEATURENAME, m_strFeatureName);
	DDX_Text(pDX, IDC_EDIT_X, m_dbX);
	DDX_Text(pDX, IDC_EDIT_Y, m_dbY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialog)
	//{{AFX_MSG_MAP(CEditDlg)
	ON_BN_CLICKED(IDC_BUTTON_MAP, OnButtonCreateMap)
	ON_BN_CLICKED(IDC_BUTTON_FEATURE3, OnButtonCreateField)
	ON_BN_CLICKED(IDC_BUTTON_LAYER, OnButtonCreateLayer)
	ON_BN_CLICKED(IDC_BUTTON_LAYER1, OnButtonDeleteLayer)
	ON_BN_CLICKED(IDC_BUTTON_FEATURE, OnButtonCreateFeature)
	ON_BN_CLICKED(IDC_BUTTON_FEATURE1, OnButtonDeleteFeature)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditDlg message handlers

void CEditDlg::OnButtonCreateMap() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
/*	CGIS_MapInfo *pMInfo = new CGIS_MapInfo;
	pMInfo->m_mapRect.dLeft = m_dbLeft;
	pMInfo->m_mapRect.dTop = m_dbTop;
	pMInfo->m_mapRect.dRight = m_dbRight;
	pMInfo->m_mapRect.dBottom = m_dbBotton;
	pMInfo->m_strMName = m_strMName;
	pMInfo->m_strMFPath = m_strFName;
	m_pMap = m_pMapManager->CreateMap( pMInfo );
*/
}

BOOL CEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int nCol = 0;
	nCol = m_ctlFList.InsertColumn( 0, L"Name", LVCFMT_CENTER, 40 );
	nCol = m_ctlFList.InsertColumn( 1, L"Type", LVCFMT_CENTER, 40 );
	nCol = m_ctlFList.InsertColumn( 2, L"Size", LVCFMT_CENTER, 40 );
	nCol = m_ctlFList.InsertColumn( 3, L"DefValue", LVCFMT_CENTER, 50 );
	nCol = m_ctlFList.InsertColumn( 4, L"Value", LVCFMT_CENTER, 50  );
	
	m_ctlTCom.SetCurSel( 1 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditDlg::OnButtonCreateField() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	int nRow = m_ctlFList.GetItemCount();
	m_ctlFList.InsertItem( nRow, m_strFieldName );
	CString str;
	str.Format( L"%d", m_ctlTCom.GetCurSel() );
	m_ctlFList.SetItemText( nRow, 1, str );
	str.Format( L"%d", m_nSize );
	m_ctlFList.SetItemText( nRow, 2, str );
	m_ctlFList.SetItemText( nRow, 3, m_strDefValue );
	m_ctlFList.SetItemText( nRow, 4, m_strValue );
}

void CEditDlg::OnButtonCreateLayer() 
{
	// TODO: Add your control notification handler code here
	ASSERT( m_pMap );
/*	UpdateData(TRUE);
	CGIS_LayerTool* pLTool;
	pLTool = (CGIS_LayerTool*)m_pMap->GetTool(LAYER_TOOL);

	CGIS_LayerInfo *pLInfo = new CGIS_LayerInfo( 0 );
	ST_GEO_RECT rtLay;
	rtLay.dLeft = m_dbLeft;
	rtLay.dTop = m_dbTop;
	rtLay.dRight = m_dbRight;
	rtLay.dBottom = m_dbBotton;
	pLInfo->SetLayerRect( rtLay );
	pLInfo->m_strLName = m_strLName;
	ST_GEO_PEN pen;
	ST_GEO_BRUSH brush;
	pen.nColor		= 9692023;
	brush.nForeColor	= 9692023;
	pen.nWidth		= 1;
	pen.nStyle		= 0;
	brush.nStyle		= 0;
	pLInfo->SetPen( pen );
	pLInfo->SetBrush( brush );
	pLInfo->SetIconID( 64 );
	pLInfo->SetMinGRScale( 40000.0 );
	pLInfo->SetMaxGRScale( 1.7E+308 );
	pLInfo->SetMinARScale( 80000.0 ); 
	pLInfo->SetMaxARScale( 1.7E+308 );

	CGIS_LayerAttInfo *pAttInfo = new CGIS_LayerAttInfo;
	CGIS_Field *pField = NULL;
	for( int i=0;	i<m_ctlFList.GetItemCount();	i++ )
	{
		pField = new CGIS_Field;
		CString str;
		str = m_ctlFList.GetItemText( i, 0 );
		pField->SetFieldName( str.AllocSysString() );
		str = m_ctlFList.GetItemText( i, 1 );
		pField->SetFieldType( _wtoi(str) );
		str = m_ctlFList.GetItemText( i, 2 );
		pField->SetFieldSize( _wtoi(str) );
		str = m_ctlFList.GetItemText( i, 3 );
		pField->SetFieldDefVal( str.AllocSysString() );
		pAttInfo->AddFieldInfo( pField );
	}
	pLTool->CreateUserLayer( pLInfo, pAttInfo );
*/	AfxMessageBox( L"Create Success!" );
}

void CEditDlg::OnButtonDeleteLayer() 
{
	// TODO: Add your control notification handler code here
	ASSERT( m_pMap );
	CGIS_LayerTool* pLTool;
	pLTool = (CGIS_LayerTool*)m_pMap->GetTool(LAYER_TOOL);
	pLTool->DeleteUserLayer( 0 );
}
#include "GIS_FeatureEditer.h"
void CEditDlg::OnButtonCreateFeature() 
{
	// TODO: Add your control notification handler code here
/*	ASSERT( m_pMap );
	UpdateData(TRUE);
	CGIS_FeatureEditer* pFTool;
	pFTool = (CGIS_FeatureEditer*)m_pMap->GetTool(EDIT_FEATURE);
	CGIS_FeaturePoint *pPoint = new CGIS_FeaturePoint( EN_GOTYPE_POINT );
	pPoint->SetAnnotation( m_strFeatureName.AllocSysString() );
	ST_GEO_PTXY *pPt = new ST_GEO_PTXY;
	pPt->dx = m_dbX;
	pPt->dy = m_dbY;
	pPoint->SetPtList( pPt );
	pFTool->CreateFeature( pPoint );
	m_pFeature = pPoint;
*/
}

void CEditDlg::OnButtonDeleteFeature() 
{
	// TODO: Add your control notification handler code here
	ASSERT( m_pMap );
	ASSERT( m_pFeature );
	CGIS_FeatureEditer* pFTool;
	pFTool = (CGIS_FeatureEditer*)m_pMap->GetTool(EDIT_FEATURE);
	pFTool->DeleteFeature( m_pFeature );
}
