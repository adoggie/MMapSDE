// MainControl.h: interface for the CMainControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINCONTROL_H__312AA1E5_992D_4669_B90D_CCF3A411002E__INCLUDED_)
#define AFX_MAINCONTROL_H__312AA1E5_992D_4669_B90D_CCF3A411002E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./RouteMng.h"
//////////////////////////////////////////////////////////////////////////

CString GetProPathName(CString strPathName);
CString GetAppPath();

class CDataCenter;
class CSDEControler;
class CScriptAnalyzer;
class IGIS_Query2;
//////////////////////////////////////////////////////////////////////////

class CMainControl  
{
public:
	CMainControl();
	virtual ~CMainControl();
	BOOL InitialOperation();

 	IGIS_Query2* GetQueryTool(){return m_pGIS_Query2;}
 	CRouteMng* GetRouteMng(){return &m_pRouteMng;}
	CScriptAnalyzer* GetScriptAnalyzer(){return m_pScriptAnalyzer;}
 
//////////////////////////////////////////////////////////////////////////
protected:
	BOOL ClearOperation();

protected:
	CSDEControler* m_pControl;
	CRouteMng   m_pRouteMng; 
	CScriptAnalyzer* m_pScriptAnalyzer;
    IGIS_Query2* m_pGIS_Query2;
};

#endif // !defined(AFX_MAINCONTROL_H__312AA1E5_992D_4669_B90D_CCF3A411002E__INCLUDED_)
