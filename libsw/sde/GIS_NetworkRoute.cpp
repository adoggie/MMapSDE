// GIS_NetworkRoute.cpp: implementation of the CGIS_NetworkRoute class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkRoute.h"
#include "GIS_WayPoint.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkRoute::CGIS_NetworkRoute()
{
	m_pPathInfo = NULL;
	m_pNetworkPath = new CGIS_NetworkPath;
	m_pWayPtList = new CPtrList;
	m_bNWRState = FALSE;
	
	// 增加对成员变量的初始化 20070327
	m_nRouteID = 0;
}

CGIS_NetworkRoute::~CGIS_NetworkRoute()
{
	if( m_pNetworkPath )
		delete m_pNetworkPath;
	if( m_pWayPtList )
	{
		CGIS_WayPoint *pWPoint;
		POSITION pos = m_pWayPtList->GetHeadPosition();
		while( pos )
		{
			pWPoint = (CGIS_WayPoint*)m_pWayPtList->GetNext( pos );
			if( pWPoint )
				delete pWPoint;
		}
		m_pWayPtList->RemoveAll();
		delete m_pWayPtList;
	}

	m_pNetworkPath = NULL;
	m_pWayPtList = NULL;
	m_bNWRState = FALSE;
}
CGIS_NetworkPathInfo* CGIS_NetworkRoute::GetNetworkPathInfo( )
{
	return m_pPathInfo;
}
void CGIS_NetworkRoute::SetNetworkPathInfo( CGIS_NetworkPathInfo *pPathInfo )
{
	m_pPathInfo = pPathInfo;
}
CPtrList *CGIS_NetworkRoute::GetWayPointList( )
{
	return m_pWayPtList;
}
CGIS_NetworkPath *CGIS_NetworkRoute::GetNetworkPath( )
{
	return m_pNetworkPath;
}
UINT CGIS_NetworkRoute::GetRouteID()
{
	return m_nRouteID;
}
void CGIS_NetworkRoute::SetRouteID( UINT nID )
{
	m_nRouteID = nID;
}
swString CGIS_NetworkRoute::GetRouteName()
{
	return m_strRName;
}
void CGIS_NetworkRoute::SetRouteName( swString strRName )
{
	m_strRName = strRName;
}
void CGIS_NetworkRoute::SetNWRouteState( BOOL bNWRState )
{
	m_bNWRState = bNWRState;
}
BOOL CGIS_NetworkRoute::GetNWRouteState( )
{
	return m_bNWRState;
}
CGIS_WayPoint *CGIS_NetworkRoute::GetWayPointByID( UINT nID )
{
	CGIS_WayPoint *pWPoint = NULL;
	POSITION pos = m_pWayPtList->GetHeadPosition();
	while( pos )
	{
		pWPoint = (CGIS_WayPoint*)m_pWayPtList->GetNext(pos);
		if( pWPoint )
		{
			if( pWPoint->GetWPID() == nID )
				return pWPoint;
		}
	}
	return NULL;
}
UINT CGIS_NetworkRoute::GetWPMaxID( )
{
	UINT nID = 0;
	CGIS_WayPoint *pWPoint = NULL;
	POSITION pos = m_pWayPtList->GetHeadPosition();
	while( pos )
	{
		pWPoint = (CGIS_WayPoint*)m_pWayPtList->GetNext(pos);
		if( pWPoint )
		{
			if( pWPoint->GetWPID() > nID )
				nID = pWPoint->GetWPID();
		}
	}
	return nID;
}