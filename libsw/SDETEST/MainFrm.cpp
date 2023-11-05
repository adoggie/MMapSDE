// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SDETest.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Array tbSTDButton contains relevant buttons of bitmap IDB_STD_SMALL_COLOR
static TBBUTTON tbButtons[] = {
	{0, 0,				TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0,  0},  
	{0,	ID_FILE_NEW,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
	{1, ID_FILE_OPEN,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
	{2, ID_FILE_SAVE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
	{0, 0,				TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
	{3, ID_EDIT_CUT,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
	{4, ID_EDIT_COPY,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
	{5, ID_EDIT_PASTE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
	{0, 0,				TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, -1},
	{6, ID_APP_ABOUT,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
	{0, 0,				TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0,  0}
};
const int nNumButtons = sizeof(tbButtons)/sizeof(TBBUTTON);
const int nNumImages = 7;
const DWORD dwAdornmentFlags = 0; // exit button

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Add the buttons and adornments to the CommandBar.
/*	if (!InsertButtons(tbButtons, nNumButtons, IDR_MAINFRAME, nNumImages) ||
	    !AddAdornments(dwAdornmentFlags))
	{
		TRACE0("Failed to add toolbar buttons\n");
		return -1;
	}
	*/
//	if (!m_wndCommandBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
//		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED) ||
//	    !m_wndCommandBar.LoadToolBar(IDR_TOOLBAR_SDEPANE))
//	{
//		TRACE0("Failed to add toolbar buttons\n");
//		return -1;
//	}
//	m_wndCommandBar.SetWindowPos(&CWnd::wndTop, -2, 274, 242, 21, SWP_SHOWWINDOW);
	if (!m_MapBar.Create(this, IDD_MAP_BAR,
		CBRS_TOP | CBRS_LEFT | CBRS_FLYBY | CBRS_HIDE_INPLACE |CBRS_SIZE_FIXED,
		IDD_MAP_BAR                     ))
	{
		TRACE0("Failed to create dialog bar m_wndMyDialogBar\n");
		return -1;		// fail to create
	}
	m_MapBar.SetMapViewSrc(GetActiveView());
    
	if (!m_NaviTestBar.Create(this, IDD_NAVITEST_BAR,
		CBRS_TOP | CBRS_LEFT | CBRS_FLYBY | CBRS_HIDE_INPLACE |CBRS_SIZE_FIXED,
		IDD_NAVITEST_BAR                     ))
	{
		TRACE0("Failed to create dialog bar m_wndMyDialogBar\n");
		return -1;		// fail to create
	}
	m_NaviTestBar.SetMapViewSrc(GetActiveView());

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs


	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
