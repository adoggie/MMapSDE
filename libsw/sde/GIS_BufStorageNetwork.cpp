// GIS_BufStorageNetwork.cpp: implementation of the CGIS_BufStorageNetwork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_BufStorageNetwork.h"
#include "GIS_BreadthMap.h"
#include "GIS_GeoMap.h"
#include "GIS_MapBreadthNetwork.h"
#include "SDEControler.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CPtrList CGIS_BufStorageNetwork::m_BMapList;
CGIS_BufStorageNetwork::CGIS_BufStorageNetwork()
{
	m_pMap = NULL;
//	m_enBSType = EN_BUFSTORTYPE_NET;
}

CGIS_BufStorageNetwork::~CGIS_BufStorageNetwork()
{
	m_pMap = NULL;
	ClearBMapBufStorage();
//	ClearBNetBufStorage();
}
BOOL CGIS_BufStorageNetwork::ClearBMapBufStorage( )
{
	if( m_pMap == NULL )
		m_pMap = (CGIS_GeoMap*)CSDEControler::GetSDEControl()->GetMapManager()->GetMapByID(2);
	POSITION pos = m_BMapList.GetHeadPosition();
	CGIS_BreadthMap *pBMap = NULL;
	while( pos )
	{
		pBMap = (CGIS_BreadthMap*)m_BMapList.GetNext( pos );
		if( pBMap )
			m_pMap->UnLoadBLRutData( pBMap );
	}
	m_BMapList.RemoveAll();
	return TRUE;
}
BOOL CGIS_BufStorageNetwork::ClearBNetBufStorage( )
{
//	POSITION pos = m_BMapList.GetHeadPosition();
//	CGIS_BreadthMap *pBMap = NULL;
//	while( pos )
//	{
//		pBMap = (CGIS_BreadthMap*)m_BMapList.GetNext( pos );
//		if( pBMap )
//		{
//			CGIS_MapBreadthNetwork *pNetwork = (CGIS_MapBreadthNetwork*)((CGIS_GeoMap*)pBMap->GetGeoMap())->GetMapNetWork();
//			pNetwork->UnLoadMapNetwork( pBMap );
//		}
//	}
//	m_BMapList.RemoveAll();
	return TRUE;
}
BOOL CGIS_BufStorageNetwork::InsertBNet( CGIS_BreadthMap *pBMap )
{
//	CGIS_BreadthMap *pBMTemp = NULL;
//	POSITION pos = m_BMapList.GetHeadPosition();
//	POSITION ps;
//	while( pos )
//	{
//		ps = pos;
//		pBMTemp = (CGIS_BreadthMap*)m_BMapList.GetNext( pos );
//		if( pBMTemp )
//		{
//			if( pBMTemp == pBMap )
//			{
//				if( pBMap != m_BMapList.GetTail() )
//				{
//					m_BMapList.RemoveAt( ps );
//					m_BMapList.AddTail( pBMap );
//					return TRUE;
//				}
//			}
//		}
//	}
//	if( m_BMapList.GetCount() >= 30 )
//	{
//		pBMTemp = (CGIS_BreadthMap*)m_BMapList.GetHead();
//		if( pBMTemp )
//		{
//			CGIS_MapBreadthNetwork *pNetwork = (CGIS_MapBreadthNetwork*)((CGIS_GeoMap*)pBMTemp->GetGeoMap())->GetMapNetWork();
//			pNetwork->UnLoadMapNetwork( pBMTemp );
//			m_BMapList.RemoveHead();
//		}
//	}
//	m_BMapList.AddTail( pBMap );
	return TRUE;
}

BOOL CGIS_BufStorageNetwork::InsertBMap( CGIS_BreadthMap *pBMap )
{
	if( m_pMap == NULL )
		m_pMap = (CGIS_GeoMap*)CSDEControler::GetSDEControl()->GetMapManager()->GetMapByID(2);
	CGIS_BreadthMap *pBMTemp = NULL;
	POSITION pos = m_BMapList.GetHeadPosition();
	POSITION ps;
	while( pos )
	{
		ps = pos;
		pBMTemp = (CGIS_BreadthMap*)m_BMapList.GetNext( pos );
		if( pBMTemp )
		{
			if( pBMTemp == pBMap )
			{
				if( pBMap != m_BMapList.GetTail() )
				{
					m_BMapList.RemoveAt( ps );
					m_BMapList.AddTail( pBMap );
					return TRUE;
				}
			}
		}
	}
	if( m_BMapList.GetCount() >= 10 )
	{
		pBMTemp = (CGIS_BreadthMap*)m_BMapList.GetHead();
		if( pBMTemp )
		{
			if( m_pMap )
				m_pMap->UnLoadBLRutData(pBMTemp);
			m_BMapList.RemoveHead();
		}
	}
	if( m_pMap )
		m_pMap->LoadBLRutData( pBMap );
	m_BMapList.AddTail( pBMap );
	return TRUE;
}