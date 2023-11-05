// MainControl.cpp: implementation of the CMainControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainControl.h"

#include "SDEControler.h"
#include "./TestFrame/CommandAction.h"
#include "./TestFrame/ScriptAnalyzer.h"
#include "Query/IGIS_Query2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
///////////////////////////////////////////////////////////////////////////////
// 得到应用程序路径
CString GetProPathName(CString strPathName)
{
	TCHAR tchDir = '\\';
	CString strTemp, strIniPathName, strIniFileName;
	GetModuleFileName(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();

	strIniPathName = strTemp.Left(strTemp.ReverseFind(tchDir) + 1);
	strIniFileName = strIniPathName + strPathName;

	return strIniFileName;
}
CString GetAppPath()
{
	TCHAR tchDir = '\\';
	CString strTemp, strIniPathName, strIniFileName;
	GetModuleFileName(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();

	strIniPathName = strTemp.Left(strTemp.ReverseFind(tchDir));
	return strIniPathName;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainControl::CMainControl()
{
    m_pScriptAnalyzer = NULL;
    m_pGIS_Query2 = NULL;
}

CMainControl::~CMainControl()
{
	VERIFY(ClearOperation());
}

BOOL CMainControl::InitialOperation()
{
	m_pControl = CSDEControler::GetSDEControl();
    ASSERT(m_pControl);
	CString temp = GetProPathName(_T("MAP"));
    CSDEControler::enErrorCode eErrorCode = CSDEControler::ERROR_NONE;
	CString strTemp = L"";
    VERIFY(m_pControl->InitSDE(temp , strTemp , &eErrorCode));


    m_pGIS_Query2 = new IGIS_Query2;
    //////////////////////////////////////////////////////////////////////////
    {
        m_pScriptAnalyzer = new CScriptAnalyzer;
        
        ASSERT(m_pScriptAnalyzer);
        CScriptAnalyzer::stuCommandData stCommandData;
        
        _tcscpy(stCommandData.szCommand,_T("航线规划"));
        stCommandData.nParaCount = 4;
        stCommandData.Proc = CCommandAction::Guihuahangxian;
        m_pScriptAnalyzer->AddCommand(stCommandData);
        
        _tcscpy(stCommandData.szCommand,_T("多点航线规划"));
        stCommandData.nParaCount = -1;
        stCommandData.Proc = CCommandAction::DuoDianGuihuahangxian;
        m_pScriptAnalyzer->AddCommand(stCommandData);

        _tcscpy(stCommandData.szCommand,_T("航线规划循环"));
        stCommandData.nParaCount = 5;
        stCommandData.Proc = CCommandAction::CircleGuihuahangxian;
        m_pScriptAnalyzer->AddCommand(stCommandData);
        
        _tcscpy(stCommandData.szCommand,_T("初始化统计结果"));
        stCommandData.nParaCount = 0;
        stCommandData.Proc = CCommandAction::ClearResult;
        m_pScriptAnalyzer->AddCommand(stCommandData);
        
        _tcscpy(stCommandData.szCommand,_T("显示统计结果"));
        stCommandData.nParaCount = 0;
        stCommandData.Proc = CCommandAction::DumpResult;
        m_pScriptAnalyzer->AddCommand(stCommandData);

        _tcscpy(stCommandData.szCommand,_T("显示中心定位"));
        stCommandData.nParaCount = 2;
        stCommandData.Proc = CCommandAction::MoveToCenter;
        m_pScriptAnalyzer->AddCommand(stCommandData);

        _tcscpy(stCommandData.szCommand,_T("ResetSpaceMemory"));
        stCommandData.nParaCount = 2;
        stCommandData.Proc = CCommandAction::ResetSpaceMemory;
        m_pScriptAnalyzer->AddCommand(stCommandData);

        _tcscpy(stCommandData.szCommand,_T("地图放大"));
        stCommandData.nParaCount = 0;
        stCommandData.Proc = CCommandAction::ZoomIn;
        m_pScriptAnalyzer->AddCommand(stCommandData);

        _tcscpy(stCommandData.szCommand,_T("地图缩小"));
        stCommandData.nParaCount = 0;
        stCommandData.Proc = CCommandAction::ZoomOut;
        m_pScriptAnalyzer->AddCommand(stCommandData);

        _tcscpy(stCommandData.szCommand,_T("循环随机移动"));
        stCommandData.nParaCount = 2;
        stCommandData.Proc = CCommandAction::CircleRandomMove;
        m_pScriptAnalyzer->AddCommand(stCommandData);

        _tcscpy(stCommandData.szCommand,_T("定位到交叉路口"));
        stCommandData.nParaCount = 3;
        stCommandData.Proc = CCommandAction::MoveToCrossRoad;
        m_pScriptAnalyzer->AddCommand(stCommandData);

        _tcscpy(stCommandData.szCommand,_T("定位到POI"));
        stCommandData.nParaCount = 3;
        stCommandData.Proc = CCommandAction::MoveToPOI;
        m_pScriptAnalyzer->AddCommand(stCommandData);

        _tcscpy(stCommandData.szCommand,_T("设置航线优先方式"));
        stCommandData.nParaCount = 1;
        stCommandData.Proc = CCommandAction::SetRoutePreference;
        m_pScriptAnalyzer->AddCommand(stCommandData);
    }
	return TRUE;
}

BOOL CMainControl::ClearOperation()
{
    if(m_pScriptAnalyzer)
    {
        delete m_pScriptAnalyzer;
        m_pScriptAnalyzer = NULL;
    }
    if(m_pGIS_Query2)
    {
        delete m_pGIS_Query2;
        m_pGIS_Query2 = NULL;
    }
	return TRUE;
}
