// TempGeoMap.cpp: implementation of the CTempGeoMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include "TempGeoMap.h"
#include "GIS_BreadthMap.h"
#include "TempMapInfo.h"
#include "TempLayerManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempGeoMap::CTempGeoMap()
{
	m_pMapInfo = new CTempMapInfo;
	m_pRTList	= NULL;
	m_pBMapList = NULL;
	m_pLayerMng = NULL;
}

CTempGeoMap::~CTempGeoMap()
{
	ASSERT(m_pMapInfo);
	delete m_pMapInfo;
	m_pMapInfo = NULL;

	if(m_pBMapList)
	{
		delete []m_pBMapList;
		m_pBMapList = NULL;
	}

	if( m_pRTList )
	{
		delete[] m_pRTList;
		m_pRTList = NULL;

	}

	if(m_pLayerMng)
	{
		delete m_pLayerMng;
		m_pLayerMng = NULL;
	}

}

CTempMapInfo* CTempGeoMap::GetMapInfo()
{
	return m_pMapInfo;
}

void CTempGeoMap::SetBreadthMapList( CGIS_BreadthMap* pBMapList )
{
	m_pBMapList = pBMapList;
}
int CTempGeoMap::GetBreadthMapNum()
{
	return m_nBMapNum;
}
void CTempGeoMap::SetBreadthMapNum( int nBMapNum )
{
	m_nBMapNum = nBMapNum;
}
void CTempGeoMap::SetBreadthMapRects( ST_GEO_RECT *pRTList, int nBNum )
{
	if( nBNum > 0 && pRTList )
	{
		if( m_pRTList )
			delete[] m_pRTList;
		m_pRTList = new ST_GEO_RECT[nBNum];
		memcpy( m_pRTList, pRTList, sizeof(ST_GEO_RECT)*nBNum);
	}
}

CGIS_BreadthMap* CTempGeoMap::GetBreadthMapByID2(int nBID)
{
	ASSERT( nBID < m_nBMapNum );
	return &m_pBMapList[nBID];
}
