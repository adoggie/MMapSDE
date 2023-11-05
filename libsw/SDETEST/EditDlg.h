#if !defined(AFX_EDITDLG_H__C340322F_6D8B_457D_962F_DFC59EF58A41__INCLUDED_)
#define AFX_EDITDLG_H__C340322F_6D8B_457D_962F_DFC59EF58A41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditDlg.h : header file
//
#include "IGIS_MapManager.h"
#include "IGIS_Map.h"
#include "IGIS_Layer.h"
/////////////////////////////////////////////////////////////////////////////
// CEditDlg dialog

class CEditDlg : public CDialog
{
// Construction
public:
	CEditDlg(IGIS_MapManager *pMapManager, CWnd* pParent = NULL);   // standard constructor

	IGIS_Map *m_pMap;
private:
	IGIS_MapManager *m_pMapManager;
	IGIS_Layer *m_pLayer;
	CGIS_Feature *m_pFeature;
// Dialog Data
	//{{AFX_DATA(CEditDlg)
	enum { IDD = IDD_DIALOG_EDIT };
	CComboBox	m_ctlTCom;
	CListCtrl	m_ctlFList;
	CString	m_strMName;
	float	m_dbLeft;
	float	m_dbTop;
	float	m_dbRight;
	float	m_dbBotton;
	CString	m_strFName;
	CString	m_strFieldName;
	UINT	m_nSize;
	CString	m_strValue;
	CString	m_strDefValue;
	CString	m_strLName;
	CString	m_strLFName;
	CString	m_strFeatureName;
	float	m_dbX;
	float	m_dbY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditDlg)
	afx_msg void OnButtonCreateMap();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonCreateField();
	afx_msg void OnButtonCreateLayer();
	afx_msg void OnButtonDeleteLayer();
	afx_msg void OnButtonCreateFeature();
	afx_msg void OnButtonDeleteFeature();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITDLG_H__C340322F_6D8B_457D_962F_DFC59EF58A41__INCLUDED_)
