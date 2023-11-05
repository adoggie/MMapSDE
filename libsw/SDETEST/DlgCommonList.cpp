// DlgCommonList.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "DlgCommonList.h"
#include "IGIS_Query2.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCommonList dialog


CDlgCommonList::CDlgCommonList(CWnd* pParent,CPtrList* pContentList)
	: CDialog(CDlgCommonList::IDD, pParent)
{
    m_pContentList = NULL;
	//{{AFX_DATA_INIT(CDlgCommonList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    m_pContentList = pContentList;
    m_nSelectItem = -1;
}


void CDlgCommonList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCommonList)
	DDX_Control(pDX, IDC_COMMON_LIST, mlstMain);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CDlgCommonList, CDialog)
	//{{AFX_MSG_MAP(CDlgCommonList)
	ON_NOTIFY(NM_DBLCLK, IDC_COMMON_LIST, OnDblclkCommonList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCommonList message handlers

BOOL CDlgCommonList::OnInitDialog() 
{
	CDialog::OnInitDialog();

    CRect rt;
	mlstMain.GetWindowRect(&rt);
	mlstMain.InsertColumn(0, NULL, LVCFMT_LEFT, rt.Width());
	mlstMain.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	// TODO: Add extra initialization here
    int nIndexN = 0;
    if(m_pContentList)
    {
        POSITION pos = m_pContentList->GetHeadPosition();
        while(pos)
        {
            CQueryCommonObject* pObject = (CQueryCommonObject*)m_pContentList->GetNext(pos);
            ASSERT(pObject);
            ASSERT(pObject->IsKindOf(RUNTIME_CLASS(CQueryCommonObject)));
            mlstMain.InsertItem(nIndexN,pObject->m_strName);
            
            nIndexN++;
        }
    }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCommonList::OnDblclkCommonList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // TODO: Add your control notification handler code here
    POSITION pos = mlstMain.GetFirstSelectedItemPosition();
    if (pos)
    {
        m_nSelectItem = mlstMain.GetNextSelectedItem(pos);
        EndDialog(IDOK);
    }
    
    *pResult = 0;
}
