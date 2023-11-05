// QueryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "QueryDlg.h"
#include "GIS_Message.h"
#include "GIS_QueryByShape.h"
#include "GIS_QueryByAtt.h"
#include "IGIS_TOOL.h"
#include "IGIS_Query.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryDlg dialog


CQueryDlg::CQueryDlg(IGIS_Map* pmap,CWnd* pParent /*=NULL*/)
	: CDialog(CQueryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQueryDlg)
	m_dy1 = 30.2733f;
	m_buffer = 300;
	m_type = 0;
	m_type1 = 0;
	m_dx1 = 120.1764f;
	m_FeatureName = _T("");
	m_FeatureName1 = _T("");
	//}}AFX_DATA_INIT
	pMap = pmap;
}


void CQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryDlg)
	DDX_Text(pDX, IDC_EDIT2, m_dy1);
	DDX_Text(pDX, IDC_EDIT3, m_buffer);
	DDX_Text(pDX, IDC_EDIT4, m_type);
	DDX_Text(pDX, IDC_EDIT8, m_type1);
	DDX_Text(pDX, IDC_EDIT1, m_dx1);
	DDX_Text(pDX, IDC_EDIT5, m_FeatureName);
	DDX_Text(pDX, IDC_EDIT6, m_FeatureName1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryDlg, CDialog)
	//{{AFX_MSG_MAP(CQueryDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryDlg message handlers

void CQueryDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	/*
	UpdateData(TRUE);
	CGIS_QueryByShape* pTempQuery;
	pTempQuery = (CGIS_QueryByShape*)pMap->GetTool(0);
	CPtrList RList;
	RList.RemoveAll();
	ST_GEO_PTXY TempPT;
	TempPT.dx = m_dx1;
	TempPT.dy = m_dy1;
	pTempQuery->QueryByBuffer(RList,&TempPT,m_buffer,(EnQueryObjType)m_type);
	POSITION pos = RList.GetHeadPosition();
	CString str;
	str.Format(L"%d",RList.GetCount());
	AfxMessageBox(str);
	*/
//	while(pos)
//	{
//		TempFea = (CGIS_Feature*)(RList.GetNext(pos1));
//	}
}

void CQueryDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
		UpdateData(TRUE);
/*
	CGIS_QueryByAtt* pTempQueryByAtt;
	CPtrList RList;
	RList.RemoveAll();
	pTempQueryByAtt = (CGIS_QueryByAtt*)pMap->GetTool(1);
	pTempQueryByAtt->QueryByAnnotation(RList,m_FeatureName.AllocSysString());
	CGIS_Feature* TempFea = NULL;
	POSITION pos = RList.GetHeadPosition();
	while(pos)
	{
		TempFea = (CGIS_Feature*)(RList.GetNext(pos));
	}


	CGIS_QueryByShape* pTempQuery;
	pTempQuery = (CGIS_QueryByShape*)pMap->GetTool(0);
	RList.RemoveAll();
	
	pTempQuery->QuerryByIntersect(RList,TempFea,(EnQueryObjType)m_type1);
	CString str;
	str.Format(L"%d",RList.GetCount());
	AfxMessageBox(str);
	*/
}

void CQueryDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*
	CGIS_QueryByAtt* pTempQueryByAtt;
	CPtrList RList;
	RList.RemoveAll();
	pTempQueryByAtt = (CGIS_QueryByAtt*)pMap->GetTool(1);
	pTempQueryByAtt->QueryByAnnotation(RList,m_FeatureName.AllocSysString());
	CGIS_Feature* TempFea;
	POSITION pos = RList.GetHeadPosition();
	if(pos)
	{
		TempFea = (CGIS_Feature*)(RList.GetNext(pos));
	}


	CGIS_QueryByShape* pTempQuery;
	pTempQuery = (CGIS_QueryByShape*)pMap->GetTool(0);
	RList.RemoveAll();
	
	pTempQuery->QueryByContain(RList,TempFea);
	CString str;
	str.Format(L"%d",RList.GetCount());
	AfxMessageBox(str);
	*/
}
