// SimulNavInfo.cpp : implementation file
//

#include "stdafx.h"
#include "sdetest.h"
#include "SimulNavInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimulNavInfo dialog


CSimulNavInfo::CSimulNavInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CSimulNavInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimulNavInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSimulNavInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimulNavInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimulNavInfo, CDialog)
	//{{AFX_MSG_MAP(CSimulNavInfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimulNavInfo message handlers

void CSimulNavInfo::SetShowInfo(CString str)
{
	if (this->GetSafeHwnd() != NULL)
	{
		GetDlgItem(IDC_STATIC_SIMULINFO_SHOW)->SetWindowText(str);
	}
}