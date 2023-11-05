// DlgTestFrame.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "./TestFrame/CommandAction.h"
#include "./TestFrame/ScriptAnalyzer.h"
#include "DlgTestFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTestFrame dialog


CDlgTestFrame::CDlgTestFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTestFrame::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTestFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgTestFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTestFrame)
	DDX_Control(pDX, IDC_LISTBOX_SCRIPT, m_ListBoxScript);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTestFrame, CDialog)
	//{{AFX_MSG_MAP(CDlgTestFrame)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE, OnButtonSingle)
	ON_BN_CLICKED(IDC_BUTTON_RUN, OnButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TO, OnButtonMoveTo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTestFrame message handlers

void CDlgTestFrame::OnButtonSingle() 
{
	// TODO: Add your control notification handler code here
	CScriptAnalyzer* pScriptAnalyzer = AppGetMainCtrl()->GetScriptAnalyzer();
	//////////////////////////////////////////////////////////////////////////
    if(m_ListBoxScript.GetCount() == 0)
        return;

	int nSel = m_ListBoxScript.GetCurSel();
	if(LB_ERR  == nSel)
    {
        nSel = 0;
    }
		
	
	BOOL bRun = FALSE;
	CString strText;

	int nCur = nSel;
	do
	{
		if(nCur >= m_ListBoxScript.GetCount())
			break;

		m_ListBoxScript.SetCurSel(nCur);
		m_ListBoxScript.RedrawWindow();
		Sleep(68);
		
		strText.Empty();
		m_ListBoxScript.GetText(nCur,strText);
		bRun = pScriptAnalyzer->RunText(strText);
		nCur++;	
	}
	while(!bRun);

    m_ListBoxScript.SetCurSel(nCur);
}

void CDlgTestFrame::OnButtonRun() 
{
	// TODO: Add your control notification handler code here
	CScriptAnalyzer* pScriptAnalyzer = AppGetMainCtrl()->GetScriptAnalyzer();
	//////////////////////////////////////////////////////////////////////////
    if(m_ListBoxScript.GetCount() == 0)
        return;

	int nSel = m_ListBoxScript.GetCurSel();
	if(LB_ERR  == nSel)
    {
        nSel = 0;
    }

    
    int nCount = m_ListBoxScript.GetCount();
	CString strText;
	for(int i = nSel; i < nCount; i++)
	{
		m_ListBoxScript.SetCurSel(i);
		m_ListBoxScript.RedrawWindow();
		Sleep(68);

		strText.Empty();
		m_ListBoxScript.GetText(i,strText);
		pScriptAnalyzer->RunText(strText);
	}		
}
#include "./TestFrame/CIniFile.h"
BOOL CDlgTestFrame::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ListBoxScript.ResetContent();

    CFileDialog aDlg(TRUE,_T(".txt"));
    if(IDCANCEL == aDlg.DoModal())
    {
        EndDialog(IDOK);
        return FALSE;
    }
    
    CString strFileName;
	CIniFile aFile;


    strFileName.Empty();
    strFileName = aDlg.GetPathName( );
#ifdef _WIN32_WCE
    //strFileName = _T("\\Storage Card\\TestScript.txt");
	VERIFY(aFile.Load(strFileName));
#else
    //strFileName = GetProPathName(_T("TestScript.txt"));
	VERIFY(aFile.Load(strFileName));
#endif
	
	int nCount = aFile.m_FileContainer.GetSize();
	for(int i = 0; i < nCount; i++)
	{
		m_ListBoxScript.AddString(aFile.m_FileContainer.GetAt(i));
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE	

}

void CDlgTestFrame::OnButtonMoveTo() 
{
	// TODO: Add your control notification handler code here
	CScriptAnalyzer* pScriptAnalyzer = AppGetMainCtrl()->GetScriptAnalyzer();
	//////////////////////////////////////////////////////////////////////////
    if(m_ListBoxScript.GetCount() == 0)
        return;

	int nSel = m_ListBoxScript.GetCurSel();
	if(LB_ERR  == nSel)
    {
        nSel = 0;
    }
		
	
	BOOL bRun = FALSE;
	CString strText;

	int nCur = nSel;
	do
	{
        if( (nCur - nSel) >= 2)
            break;
		if(nCur >= m_ListBoxScript.GetCount())
			break;

		m_ListBoxScript.SetCurSel(nCur);
		m_ListBoxScript.RedrawWindow();
		Sleep(68);
		
		strText.Empty();
		m_ListBoxScript.GetText(nCur,strText);
		bRun = pScriptAnalyzer->RunText(strText);
		nCur++;	


	}
	while(!bRun);

    strText = _T("同步调用:显示中心定位");
    pScriptAnalyzer->RunText(strText);

    m_ListBoxScript.SetCurSel(nCur);
	
}
