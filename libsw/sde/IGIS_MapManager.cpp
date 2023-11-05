// IGIS_MapManager.cpp: implementation of the IGIS_MapManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IGIS_MapManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IGIS_MapManager::IGIS_MapManager()
{

}

IGIS_MapManager::~IGIS_MapManager()
{

}

//////////////////////////////////////////////////////////////////////////    
#ifdef _CVIEW
BOOL IGIS_MapManager::AttechView(CView* pView)
#else
BOOL IGIS_MapManager::AttechView(CWnd* pView)
#endif
{
    ASSERT(pView);
    ASSERT(!(m_ListOfView.Find(pView)));
    m_ListOfView.AddTail(pView);

    return TRUE; 
}
#ifdef _CVIEW
BOOL IGIS_MapManager::DetchView(CView* pView)
#else
BOOL IGIS_MapManager::DetchView(CWnd* pView)
#endif
{
    ASSERT(pView);
    ASSERT(m_ListOfView.Find(pView));
    m_ListOfView.RemoveAt(m_ListOfView.Find(pView));
    
    return TRUE;
}
//////////////////////////////////////////////////////////////////////////