#if !defined(AFX_SIMULNAVINFO_H__11DAC74B_E3FC_4C6A_BC9B_BFDFE84F8877__INCLUDED_)
#define AFX_SIMULNAVINFO_H__11DAC74B_E3FC_4C6A_BC9B_BFDFE84F8877__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimulNavInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimulNavInfo dialog

class CSimulNavInfo : public CDialog
{
// Construction
public:
	CSimulNavInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSimulNavInfo)
	enum { IDD = IDD_DIALOG_SIMULNAV_SHOWINFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimulNavInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimulNavInfo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetShowInfo(CString str);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMULNAVINFO_H__11DAC74B_E3FC_4C6A_BC9B_BFDFE84F8877__INCLUDED_)
