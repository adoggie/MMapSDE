// SDETest.h : main header file for the SDETEST application
//

#if !defined(AFX_SDETEST_H__3EE081D5_F18F_4EB1_8D6C_6A86E6348B25__INCLUDED_)
#define AFX_SDETEST_H__3EE081D5_F18F_4EB1_8D6C_6A86E6348B25__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "MainControl.h"

CMainControl* AppGetMainCtrl();

CString GetProPathName(CString strPathName);
/////////////////////////////////////////////////////////////////////////////
// CSDETestApp:
// See SDETest.cpp for the implementation of this class
//

class CSDETestApp : public CWinApp
{
public:
	CSDETestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDETestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSDETestApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CMainControl* m_pMainCtrl;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDETEST_H__3EE081D5_F18F_4EB1_8D6C_6A86E6348B25__INCLUDED_)
