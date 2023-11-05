// CommandAction.h: interface for the CCommandAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDACTION_H__7CDB2F13_E2A7_4884_8ADF_50C9B99DCD7A__INCLUDED_)
#define AFX_COMMANDACTION_H__7CDB2F13_E2A7_4884_8ADF_50C9B99DCD7A__INCLUDED_

#include <afxtempl.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCommandAction  
{
public:
	CCommandAction();
	virtual ~CCommandAction();

static int CALLBACK GuihuahangxianPOIToPOI(CStringArray& arString);
static int CALLBACK MoveToPOI(CStringArray& arString);
static int CALLBACK MoveToCrossRoad(CStringArray& arString);
static int CALLBACK DuoDianGuihuahangxian(CStringArray& arString);
static int CALLBACK Guihuahangxian(CStringArray& arString);
static int CALLBACK SetRoutePreference(CStringArray& arString);
static int CALLBACK CircleGuihuahangxian(CStringArray& arString);
static int CALLBACK ClearResult(CStringArray& arString);
static int CALLBACK DumpResult(CStringArray& arString);
static int CALLBACK MoveToCenter(CStringArray& arString);
static int CALLBACK ResetSpaceMemory(CStringArray& arString);
static int CALLBACK ZoomIn(CStringArray& arString);
static int CALLBACK ZoomOut(CStringArray& arString);
static int CALLBACK CircleRandomMove(CStringArray& arString);
static int CALLBACK CircleMatchRoad(CStringArray& arString);
static int CALLBACK CircleLeeway(CStringArray& arString);
};

#endif // !defined(AFX_COMMANDACTION_H__7CDB2F13_E2A7_4884_8ADF_50C9B99DCD7A__INCLUDED_)
