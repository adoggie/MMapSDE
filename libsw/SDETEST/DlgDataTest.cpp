// DlgDataTest.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "DlgDataTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDataTest dialog


CDlgDataTest::CDlgDataTest(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDataTest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDataTest)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgDataTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDataTest)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDataTest, CDialog)
	//{{AFX_MSG_MAP(CDlgDataTest)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDataTest message handlers

void CDlgDataTest::OnButton1() 
{
	// TODO: Add your control notification handler code here

	
	
}
