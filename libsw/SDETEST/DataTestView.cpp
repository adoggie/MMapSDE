// DataTestView.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "DataTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataTestView

IMPLEMENT_DYNCREATE(CDataTestView, CFormView)

CDataTestView::CDataTestView()
	: CFormView(CDataTestView::IDD)
{
	//{{AFX_DATA_INIT(CDataTestView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDataTestView::~CDataTestView()
{
}

void CDataTestView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataTestView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataTestView, CFormView)
	//{{AFX_MSG_MAP(CDataTestView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataTestView diagnostics

#ifdef _DEBUG
void CDataTestView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDataTestView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDataTestView message handlers
