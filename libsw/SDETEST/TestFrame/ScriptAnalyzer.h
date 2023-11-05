// ScriptAnalyzer.h: interface for the CScriptAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTANALYZER_H__0F5C8541_5DBA_44FD_BA84_F8B1F3513587__INCLUDED_)
#define AFX_SCRIPTANALYZER_H__0F5C8541_5DBA_44FD_BA84_F8B1F3513587__INCLUDED_

#include <stack>
#include <afxtempl.h>
using namespace std;


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScriptAnalyzer  
{
//////////////////////////////////////////////////////////////////////////
public:
	typedef int (CALLBACK* CMDPROC) (CStringArray& pStrParaArray);
	struct stuCommandData
	{
		TCHAR szCommand[100];
		int nParaCount;
		CMDPROC Proc;	
	};
//////////////////////////////////////////////////////////////////////////
public:
	BOOL RunText(CString& strText);
	BOOL AddCommand(stuCommandData& stCommandData);
	CScriptAnalyzer();
	virtual ~CScriptAnalyzer();

//////////////////////////////////////////////////////////////////////////	
protected:
	stack <CString> m_ParaStack;
	CArray <stuCommandData,stuCommandData&> m_arCommandData;

    struct tagThreadData
    {
        HWND            hWnd;
        HANDLE          hExitEvent;
        CStringArray    arString;
        stuCommandData  stCommandData;
    };	 
protected:
static	UINT CallThread(void* pParam);
	BOOL OverlappedRun(int nIndex);
	BOOL SeriesRun(int nIndex);
	int MetchCommand(CString strText);
};

#endif // !defined(AFX_SCRIPTANALYZER_H__0F5C8541_5DBA_44FD_BA84_F8B1F3513587__INCLUDED_)
