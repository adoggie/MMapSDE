// TempLayerManager.cpp: implementation of the CTempLayerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TempLayerManager.h"
#include "TempMapLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempLayerManager::CTempLayerManager()
{
	m_pLayListMap = new CTempLayerMap;
}

CTempLayerManager::~CTempLayerManager()
{
	if(m_pLayListMap)
	{
		POSITION pos = m_pLayListMap->GetStartPosition();
		short nKey;
		CTempMapLayer* pLayer = NULL;
		while(pos)
		{
			m_pLayListMap->GetNextAssoc(pos,nKey,pLayer);
			ASSERT(pLayer);
			delete pLayer;
			pLayer = NULL;
		}
	}

	m_pLayListMap->RemoveAll();
	delete m_pLayListMap;
	m_pLayListMap = NULL;
}

CTempLayerMap *CTempLayerManager::GetLayerListMap()
{
	return m_pLayListMap;
}
