// NaviTestBar.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "NaviTestBar.h"

#include "MainFrm.h"
#include "SDETestDoc.h"
#include "SDETestView.h"
#include "./TestFrame/CommandAction.h"
#include "./TestFrame/ScriptAnalyzer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNaviTestBar dialog
IMPLEMENT_DYNCREATE(CNaviTestBar, CDialogBar)

CNaviTestBar::CNaviTestBar()
{
	//{{AFX_DATA_INIT(CNaviTestBar)
	m_strCommand = _T("");
	m_strLogLat = _T("");
	//}}AFX_DATA_INIT
	m_pMapView = NULL;
}


void CNaviTestBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNaviTestBar)
	DDX_Text(pDX, IDC_EDIT_COMMAND, m_strCommand);
	DDV_MaxChars(pDX, m_strCommand, 256);
	DDX_Text(pDX, IDC_EDIT_LOGLAT, m_strLogLat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNaviTestBar, CDialogBar)
	//{{AFX_MSG_MAP(CNaviTestBar)
	ON_BN_CLICKED(IDC_BUTTON_MOVETO, OnButtonMoveTo)
	ON_BN_CLICKED(IDC_BUTTON_RUN, OnButtonRun)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_GET_LATLOG, OnButtonGetLatlog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNaviTestBar message handlers
void CNaviTestBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	bDisableIfNoHndler = FALSE;
	CDialogBar::OnUpdateCmdUI(pTarget,bDisableIfNoHndler);

}

BOOL CNaviTestBar::Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	
	BOOL bRes= CDialogBar::Create(pParentWnd,nIDTemplate,nStyle,nID );
	InitDialogBar();//在类中添加一个成员函数就可以了
	return bRes;	
}

BOOL CNaviTestBar::InitDialogBar()
{
	return FALSE;
}

void CNaviTestBar::OnButtonMoveTo() 
{
	// TODO: Add your control notification handler code here
	//取到点
	ASSERT(m_pMapView && IsWindow(m_pMapView->GetSafeHwnd()));
	double dx ,dy;
	m_pMapView->XyViewToMap( m_pMapView->m_CursorPt.x , m_pMapView->m_CursorPt.y , dx , dy );	
}
void CNaviTestBar::SetMapViewSrc(CWnd *pWnd)
{
	m_pMapView = (CSDETestView*)pWnd;
}

void CNaviTestBar::OnButtonRun() 
{
	// TODO: Add your control notification handler code here
	CScriptAnalyzer* pScriptAnalyzer = AppGetMainCtrl()->GetScriptAnalyzer();
	//////////////////////////////////////////////////////////////////////////
    UpdateData();
	pScriptAnalyzer->RunText(m_strCommand);	
    m_strCommand.Empty();
    UpdateData(FALSE);
}


void CNaviTestBar::OnButtonGetLatlog() 
{
	// TODO: Add your control notification handler code here
    SetMapViewSrc(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	ASSERT(m_pMapView && IsWindow(m_pMapView->GetSafeHwnd()));
	double dx ,dy;
	m_pMapView->XyViewToMap( m_pMapView->m_CursorPt.x , m_pMapView->m_CursorPt.y , dx , dy );	

    m_strLogLat.Format(_T("%.6f,%.6f"),dx,dy);
    UpdateData(FALSE);
	
}
