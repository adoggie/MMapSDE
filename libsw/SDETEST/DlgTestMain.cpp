// DlgTestMain.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "DlgTestMain.h"
#include "DlgTestNavi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTestMain dialog


CDlgTestMain::CDlgTestMain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTestMain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTestMain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgTestMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTestMain)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTestMain, CDialog)
	//{{AFX_MSG_MAP(CDlgTestMain)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTestMain message handlers

void CDlgTestMain::OnButton2() 
{
	// TODO: Add your control notification handler code here
	CDlgTestNavi aDlg;
	aDlg.DoModal();
}
