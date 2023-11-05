#if !defined(AFX_EXPANDMAP_H__7F4B8822_FCFA_469F_BB6F_1C436A5AB0F3__INCLUDED_)
#define AFX_EXPANDMAP_H__7F4B8822_FCFA_469F_BB6F_1C436A5AB0F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExpandMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExpandMap dialog

class CExpandMap : public CDialog
{
// Construction
public:
	CExpandMap(CWnd* pParent = NULL);   // standard constructor
private:
	POSITION m_posCurrent;
	CDC* m_pDrawDC;
// Dialog Data
	//{{AFX_DATA(CExpandMap)
	enum { IDD = IDD_DIALOG_EXPAND };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpandMap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExpandMap)
	afx_msg void OnButtonNext();
	afx_msg void OnButtonPrevious();
	afx_msg void OnPaint();
	afx_msg void OnButtonMoveto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPANDMAP_H__7F4B8822_FCFA_469F_BB6F_1C436A5AB0F3__INCLUDED_)
