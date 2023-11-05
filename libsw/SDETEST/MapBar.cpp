// MapBar.cpp : implementation file
//

#include "stdafx.h"

#include "SDETest.h"
#include "AutoMoveByRoute.h"
#include "SDETestDoc.h"
#include "SDETestView.h"
#include "DlgTestFrame.h"
#include "MainFrm.h"
#include "SimulNavInfo.h"

#include "MapBar.h"

#include "RichPOI.h"
#include "ExpandMap.h"
#include "GeoGlobe.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FREQ_SEND_DATA	500	// ģ�ⶨλƵ��1000����
#define EVENT_ID	123 // ��ʱ����ʶ
/////////////////////////////////////////////////////////////////////////////
// CMapBar dialog
IMPLEMENT_DYNCREATE(CMapBar, CDialogBar)

CMapBar::CMapBar()
{
	//{{AFX_DATA_INIT(CMapBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pMapView = NULL;
	m_pAutoMoveByRoute = new CAutoMoveByRoute;
	m_pSimulNavInfo = new CSimulNavInfo;
	m_bSimulNav = FALSE;
}
CMapBar::~CMapBar()
{
	if (m_pAutoMoveByRoute != NULL)
		delete m_pAutoMoveByRoute;
	m_pAutoMoveByRoute = NULL;
	if (m_pSimulNavInfo != NULL)
	{
		if (m_pSimulNavInfo->m_hWnd != NULL)
			m_pSimulNavInfo->DestroyWindow();
		delete m_pSimulNavInfo;
	}
	m_pSimulNavInfo = NULL;
}

void CMapBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapBar, CDialogBar)
	//{{AFX_MSG_MAP(CMapBar)
	ON_BN_CLICKED(IDC_BTN_ZOOMIN, OnBtnZoomin)
	ON_BN_CLICKED(IDC_BTN_ZOOMOUT, OnBtnZoomout)
	ON_BN_CLICKED(IDC_BTN_VIEWMAP, OnBtnViewmap)
	ON_BN_CLICKED(IDC_BTN_ORIGIN, OnBtnOrigin)
	ON_BN_CLICKED(IDC_BTN_NAVI, OnBtnNavi)
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBtnExit)
	ON_BN_CLICKED(IDC_BTN_CHECK, OnBtnCheck)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	ON_BN_CLICKED(IDC_BTN_CALCULATE_DIST, OnBtnCalculateDist)
	ON_BN_CLICKED(IDC_BUTTON_RICHPOI, OnButtonRichpoi)
	ON_BN_CLICKED(IDC_BUTTON_EXPANDMAP, OnButtonExpandmap)
	ON_BN_CLICKED(IDC_BUTTON_MAPBAR_NAV, OnButtonMapbarNav)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapBar message handlers
void CMapBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	bDisableIfNoHndler = FALSE;
	CDialogBar::OnUpdateCmdUI(pTarget,bDisableIfNoHndler);

}

BOOL CMapBar::Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID) 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bRes= CDialogBar::Create(pParentWnd,nIDTemplate,nStyle,nID );
	InitDialogBar();//���������һ����Ա�����Ϳ�����
	return bRes;	
}

BOOL CMapBar::InitDialogBar()
{

	return FALSE;
}

void CMapBar::OnBtnZoomin() 
{
	// TODO: Add your control notification handler code here
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ZOOMIN,0);
	
}

void CMapBar::OnBtnZoomout() 
{
	// TODO: Add your control notification handler code here
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ZOOMOUT,0);	
}

void CMapBar::OnBtnViewmap() 
{
	// TODO: Add your control notification handler code here
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_VIEWMAP,0);
	SetMapViewSrc(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
    GetDlgItem(IDC_BTN_ZOOMIN)          ->EnableWindow();
    GetDlgItem(IDC_BTN_ZOOMOUT)         ->EnableWindow();
    GetDlgItem(IDC_BTN_ORIGIN)          ->EnableWindow();
    GetDlgItem(IDC_BTN_NAVI)            ->EnableWindow();
    GetDlgItem(IDC_BTN_CALCULATE_DIST)  ->EnableWindow();
    GetDlgItem(IDC_BTN_CHECK)           ->EnableWindow();
    GetDlgItem(IDC_BUTTO_TEST)          ->EnableWindow();
	
}

void CMapBar::OnBtnOrigin() 
{
	// TODO: Add your control notification handler code here
	ASSERT(m_pMapView && IsWindow(m_pMapView->GetSafeHwnd()));
	//�趨ԭ��
//	m_ptOrigin = m_pMapView->GetCurPt();
	double dx ,dy;
	m_pMapView->XyViewToMap( m_pMapView->m_CursorPt.x , m_pMapView->m_CursorPt.y , dx , dy );
    TRACE2("\t�趨���:[ %f,%f ]\n",dx,dy);
	m_pMapView->m_PtNaviStart.dx = dx;
	m_pMapView->m_PtNaviStart.dy = dy;
	m_pMapView->Invalidate();
}

void CMapBar::OnBtnNavi() 
{
	// TODO: Add your control notification handler code here
    CMainControl* pMainControl = AppGetMainCtrl();
    ASSERT(pMainControl);
    CRouteMng* pRouteMng = pMainControl->GetRouteMng();
    ASSERT(pRouteMng);
//////////////////////////////////////////////////////////////////////////
    
    ASSERT(m_pMapView && IsWindow(m_pMapView->GetSafeHwnd()));
	double dx ,dy;
	m_pMapView->XyViewToMap( m_pMapView->m_CursorPt.x , m_pMapView->m_CursorPt.y , dx , dy );
	
	m_pMapView->m_ptNaviTo.dx = dx;
	m_pMapView->m_ptNaviTo.dy = dy;
    TRACE2("\t�趨�յ�:[ %f,%f ]\n",dx,dy);


    ST_GEO_PTXY aPrArray[2];
	aPrArray[0] = m_pMapView->m_PtNaviStart;
	aPrArray[1] = m_pMapView->m_ptNaviTo;
    
	pRouteMng->CalculateRoute(aPrArray,2);	

    CGIS_FeatureRoute* pRoute = NULL;
    VERIFY(pRouteMng->GetRoute(pRoute));
    ASSERT(pRoute);
    m_pMapView->SetNetworkRoute(pRoute->GetNetworkRoute());
}

void CMapBar::SetMapViewSrc(CWnd *pWnd)
{
	m_pMapView = (CSDETestView*)pWnd;
}

void CMapBar::OnBtnExit() 
{
	// TODO: Add your control notification handler code here
	SetMapViewSrc(NULL);
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}

void CMapBar::OnBtnCheck() 
{
	// TODO: Add your control notification handler code here
	CSDEControler* pControl = CSDEControler::GetSDEControl();
	ASSERT(pControl);
	IGIS_NetWork *pNetwork = (IGIS_NetWork*)pControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );
	ASSERT(pNetwork);
	//if(!pNetwork->CheckNetworkData())
	{
		MessageBox(_T("·�����ݲ�����!"));
	}

    //����һ���߳��������ϵ�Reset�ռ�������.

}

void CMapBar::OnButtonTest() 
{
    ASSERT(m_pMapView);
    m_pMapView->EnableShowTip(FALSE);
	CDlgTestFrame aDlg;
	aDlg.DoModal();
    m_pMapView->EnableShowTip(TRUE);
}
//void CMapBar::OnButtonTest() 
//{
//
//	// TODO: Add your control notification handler code here
//    CMainControl* pMainControl = AppGetMainCtrl();
//    ASSERT(pMainControl);
//    CRouteMng* pRouteMng = pMainControl->GetRouteMng();
//    ASSERT(pRouteMng);
////////////////////////////////////////////////////////////////////////////
//
//    ST_GEO_PTXY aPrArray[6];
////    //�Ϻ�-����-����-����
//	aPrArray[0].dx = 120.462135f;
// 	aPrArray[0].dy = 32.763924f;	
//	aPrArray[1].dx = 115.676506f;
// 	aPrArray[1].dy = 23.698793f;	
//	aPrArray[2].dx = 91.067055f;
//	aPrArray[2].dy = 29.643335f;
//	aPrArray[3].dx = 126.643364f;
//	aPrArray[3].dy = 43.480804f;	
//
////    //�Ϻ�-����-����-�Ϻ�-����-����
////	aPrArray[0].dx = 120.462135f;
//// 	aPrArray[0].dy = 32.763924f;	
////	aPrArray[1].dx = 115.676506f;
//// 	aPrArray[1].dy = 23.698793f;	
////	aPrArray[2].dx = 91.067055f;
////	aPrArray[2].dy = 29.643335f;
////	aPrArray[3].dx = 120.462135f;
////	aPrArray[3].dy = 32.763924f;	
////	aPrArray[4].dx = 91.067055f;
////	aPrArray[4].dy = 29.643335f;
////	aPrArray[5].dx = 115.676506f;
////  aPrArray[5].dy = 23.698793f;	
//	
//   
//    for(int i = 0; i < 1; i++)
//    {
//        TRACE1("=============��%d�ι滮======================================\n",i);
//        pRouteMng->CalculateRoute(&aPrArray[0],4);
//    }
//    
////    //�������и��������
////	aPrArray[0].dx = 128.385620f;
//// 	aPrArray[0].dy = 47.222713f;
////	aPrArray[1].dx = 91.128601f;
//// 	aPrArray[1].dy = 29.584124f;	
////	aPrArray[2].dx = 128.385620f;
//// 	aPrArray[2].dy = 47.222713f;
////   pRouteMng->CalculateRoute(aPrArray,3);
//    //�������и��������
////	aPrArray[0].dx = 128.385620f;
//// 	aPrArray[0].dy = 47.222713f;
////	aPrArray[1].dx = 91.128601f;
//// 	aPrArray[1].dy = 29.584124f;	
////	aPrArray[2].dx = 128.385620f;
//// 	aPrArray[2].dy = 47.222713f;
////     pRouteMng->CalculateRoute(&aPrArray[1],2);
//
////	aPrArray[0].dx = 121.377090f;
//// 	aPrArray[0].dy = 31.157909f;
////	aPrArray[1].dx = 119.943085f;
//// 	aPrArray[1].dy = 31.784048f;	
////     pRouteMng->CalculateRoute(aPrArray,2);
//
//    
//    CGIS_FeatureRoute* pRoute = NULL;
//    VERIFY(pRouteMng->GetRoute(pRoute));
//    ASSERT(pRoute);
//    m_pMapView->SetNetworkRoute(pRoute->GetNetworkRoute());
//}

void CMapBar::OnBtnCalculateDist() 
{
	// TODO: Add your control notification handler code here
    CMainControl* pMainControl = AppGetMainCtrl();
    ASSERT(pMainControl);
    CRouteMng* pRouteMng = pMainControl->GetRouteMng();
    ASSERT(pRouteMng);
//////////////////////////////////////////////////////////////////////////
    
    ASSERT(m_pMapView && IsWindow(m_pMapView->GetSafeHwnd()));
	double dx ,dy;
	m_pMapView->XyViewToMap( m_pMapView->m_CursorPt.x , m_pMapView->m_CursorPt.y , dx , dy );
	
    double dbLen = CalcDist(m_pMapView->m_PtNaviStart.dx,m_pMapView->m_PtNaviStart.dy, dx,dy);
    CString strText;
    strText.Format(_T("����Ϊ%.2f m!"),dbLen);
    AfxMessageBox(strText);

}


void CMapBar::OnButtonRichpoi() 
{
	// TODO: Add your control notification handler code here
	CRichPOI dlgRichPOi;
	dlgRichPOi.DoModal();
//	CExpandMap dlgExpandMap;
//	dlgExpandMap.DoModal();

}

void CMapBar::OnButtonExpandmap() 
{
	// TODO: Add your control notification handler code here
	CExpandMap dlgExpandMap;
	dlgExpandMap.DoModal();
	
}
void CMapBar::OnButtonMapbarNav() 
{
	if (m_bSimulNav)
	{
		m_bSimulNav = FALSE;
		KillTimer(EVENT_ID);
		GetDlgItem(IDC_BUTTON_MAPBAR_NAV)->SetWindowText(_T("��ʼ����"));
		GetDlgItem(IDC_BTN_NAVI)->EnableWindow(TRUE);
	}
	else
	{
		CMainControl *pMainControl = AppGetMainCtrl();
		ASSERT(pMainControl != NULL);
		CRouteMng *pRouteMng = pMainControl->GetRouteMng();
		ASSERT(pRouteMng != NULL);
		CGIS_FeatureRoute *pFeatureRoute = pRouteMng->GetCurRoute();
		if (NULL == pFeatureRoute)
		{
			MessageBox(_T("���ȼ���һ������"));
			return;
		}
		m_pAutoMoveByRoute->SetRoute(pFeatureRoute);
		m_pAutoMoveByRoute->InitSimulNav();
		pFeatureRoute->GetNetworkRoute()->GetNetworkPath()->InitNavigate();

		GetDlgItem(IDC_BUTTON_MAPBAR_NAV)->SetWindowText(_T("ֹͣ����"));
		GetDlgItem(IDC_BTN_NAVI)->EnableWindow(FALSE);

		SetTimer(EVENT_ID, FREQ_SEND_DATA, NULL);
		m_pSimulNavInfo->ShowWindow(TRUE);
		m_bSimulNav = TRUE;
	}
}

void CMapBar::OnDestroy() 
{
	CDialogBar::OnDestroy();
	
	m_pSimulNavInfo->DestroyWindow();
}

int CMapBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pSimulNavInfo->Create(CSimulNavInfo::IDD, this);
	
	return 0;
}

void CMapBar::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == EVENT_ID)
	{
		double dbLoogitude;
		double dbLatitude;
		int nAngle;
		m_pAutoMoveByRoute->GetSimulInfo(dbLoogitude, dbLatitude, nAngle);
		ST_GEO_PTXY pt;
		pt.dx = dbLoogitude;
		pt.dy = dbLatitude;
		m_pMapView->SetCurPt(pt);
		m_pMapView->SetCarPar(nAngle, TRUE, 0, 1);
		m_pMapView->Refresh();

		CMainControl *pMainControl = AppGetMainCtrl();
		ASSERT(pMainControl != NULL);
		CRouteMng *pRouteMng = pMainControl->GetRouteMng();
		ASSERT(pRouteMng != NULL);
		CGIS_FeatureRoute *pRoute = pRouteMng->GetCurRoute();
		ASSERT(pRoute != NULL);
		CGIS_NetworkPath *pNetworkPath = NULL;
		pNetworkPath = pRoute->GetNetworkRoute()->GetNetworkPath();
		ASSERT(pNetworkPath != NULL);
		static bool bShow = false;
		if (!pNetworkPath->GetNavigateVoice(pt))
		{
			if (pNetworkPath->GetbTipReCal() && !bShow)
			{
				bShow = true;
				int n = MessageBox(_T("ǰ���н���·�Σ��Ƿ����¼���"), NULL, MB_OKCANCEL);
				if (n == IDOK)
				{
					KillTimer(EVENT_ID);
					m_bSimulNav = FALSE;
					GetDlgItem(IDC_BUTTON_MAPBAR_NAV)->SetWindowText(_T("��ʼ����"));
					GetDlgItem(IDC_BTN_NAVI)->EnableWindow(TRUE);
					CRouteMng *pRouteMng = AppGetMainCtrl()->GetRouteMng();
					ASSERT(pRouteMng != NULL);
					pRouteMng->ReCaculRoute(pt);
				}
				bShow = false;
				return;
			}
			
		}
		BYTE byType; // ��������
		CString strCurName; // ��ǰ·��
		CString strNextName; // ��һ·��
		double dbDisNav; // ����������
		double dbAngleNav; // ���������
		double dbDisDest; // ���յ����
		double dbAngelDest; // ���յ㷽��
		CString strLVoice; // Զ��������
		CString strMVoice; // �о�������
		CString strNVoice; // ����������
		CString strCName; // ��ǰ·��
		BYTE bymNum; // ����·�ڸ���
		pNetworkPath->GetNavigateVoiceInfo(byType, strCurName, strNextName, dbDisNav,
			dbAngleNav, dbDisDest, dbAngelDest, strLVoice, strMVoice, strNVoice,
			strCName, bymNum);
		if (dbDisDest < 10)
		{
			KillTimer(EVENT_ID);
			m_bSimulNav = FALSE;
			GetDlgItem(IDC_BUTTON_MAPBAR_NAV)->SetWindowText(_T("��ʼ����"));
			GetDlgItem(IDC_BTN_NAVI)->EnableWindow(TRUE);
		}
		CString strShow;
		strShow = _T("����1��");
		strShow += strLVoice;
		strShow += _T("\r\n");
		strShow += _T("����2��");
		strShow += strMVoice;
		strShow += _T("\r\n");
		strShow += _T("����3��");
		strShow += strNVoice;
		strShow += _T("\r\n");
		strShow += _T("��ǰ·����");
		strShow += strCurName;
		strShow += _T("����һ·����");
		strShow += strNextName;
		strShow += _T("\r\n");
		CString strTemp;
		strTemp.Format(_T("����ǰ�������룻%.0fm����Ŀ�ĵؾ��룺%.0fm\r\n"), dbDisNav, dbDisDest);
		strShow += strTemp;
		if (byType==2 || byType==3)
		{
			strTemp.Format(_T("�������ڣ�%d"), bymNum);
			strTemp += strTemp;
		}
		m_pSimulNavInfo->SetShowInfo(strShow);
		return;
	}

	CDialogBar::OnTimer(nIDEvent);
}
