//////////////////////////////////////////////////////////////////////////
// AutoMoveByRoute.cpp

#include "StdAfx.h"

#include "SDELib.h"
#include "SDEControler.h"
#include "GIS_FeatureRoute.h"

#include "AutoMoveByRoute.h"

#include "GIS_PathpointNode.h"


#define  DEGREE_MAP2GPS(n)   ((int)( 450 - n ) % 360)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoMoveByRoute::CAutoMoveByRoute()
{
	m_dbLat = 0.0;
	m_dbLoog = 0.0;
	m_dbAngle = 0.0;
	m_posList = NULL;
	m_pList = NULL;
	m_bIsTail = FALSE;
	m_nRoadtype = 2;
	m_pFeaRoute = NULL;
	m_dbSpeed[0] = 33.3; // 120km/h
	m_dbSpeed[1] = 16.7; // 60 km/h
	m_dbSpeed[2] = 11.1; // 40 km/h
}

CAutoMoveByRoute::~CAutoMoveByRoute()
{
}

//========================================================================
// 初始化模拟定位数据参数
//========================================================================
BOOL CAutoMoveByRoute::InitSimulNav()
{
	ASSERT(m_pFeaRoute != NULL);
	CGIS_NetworkRoute *pNetworkRoute = m_pFeaRoute->GetNetworkRoute();
	if (pNetworkRoute == NULL)
		return FALSE;
	CGIS_NetworkPath *pNetworkPath = pNetworkRoute->GetNetworkPath();
	if (pNetworkPath == NULL)
		return FALSE;
	m_pList = pNetworkPath->GetPathpointList();
	if (m_pList == NULL) 
		return FALSE;
	m_posList = m_pList->GetHeadPosition();
	CGIS_Pathpoint *pPathPoint = (CGIS_Pathpoint*)m_pList->GetAt(m_posList);
	if (pPathPoint == NULL)
		return FALSE;
	
	m_dbLoog = pPathPoint->GetPathpointPosition().dx;
	m_dbLat  = pPathPoint->GetPathpointPosition().dy;
	m_pList->GetNext(m_posList);

	return TRUE;
}

//========================================================================
// 获得数据，通过解析航线获得定位数据
//========================================================================
BOOL CAutoMoveByRoute::GetSimulInfo(double &dbLoogitude,
									double &dbLatitude,
									int &nAngle)
{
	MoveNext();
	dbLoogitude = m_dbLoog;
	dbLatitude = m_dbLat;
	nAngle = (int)DEGREE_MAP2GPS(m_dbAngle*180/PI);

	return TRUE;
}

void CAutoMoveByRoute::MoveNext()
{
	if (m_bIsTail)
		return;
	if (m_pList == NULL)
		return;

	if (m_posList == NULL)
	{
		m_bIsTail = TRUE;
		float x, y;
		x = ((CGIS_Pathpoint*)m_pList->GetTail())->GetPathpointPosition().dx;
		y = ((CGIS_Pathpoint*)m_pList->GetTail())->GetPathpointPosition().dy;
		m_dbAngle = CalcAngle(x, y, m_dbLoog, m_dbLat);
		m_dbLoog = x;
		m_dbLat = y;
		return;
	}

	CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)m_pList->GetAt(m_posList);
	if (pPPoint == NULL)
		return;

	// 判断当前行驶的道路类型
	if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
	{
		BYTE byRType = ((CGIS_PathpointNode*)pPPoint)->GetRType();
		if( byRType == 0)
			m_nRoadtype = 0;
		else if(byRType==1 || byRType == 2 || byRType == 3)
			m_nRoadtype = 1;
		else if( byRType > 3)
			m_nRoadtype = 2;
	}
	
	
	double x, y, a, b;
	CGIS_Pathpoint *pPathPoint = (CGIS_Pathpoint*)m_pList->GetAt(m_posList);
	if (pPathPoint == NULL)
		return;

	x = pPathPoint->GetPathpointPosition().dx;
	y = pPathPoint->GetPathpointPosition().dy;

	double dbLen = CalcDist(x, y, m_dbLoog, m_dbLat);
	
	// 为了保证角度的准确性，计算角度的两个点的距离应该大于10米
	POSITION posAngle = m_posList;
	double dbDis = dbLen;
	double dbx = x;
	double dby = y;
	while (posAngle!=NULL && dbDis<=10)
	{
		CGIS_Pathpoint *p = (CGIS_Pathpoint*)m_pList->GetNext(posAngle);
		if (p == NULL)
			break;
		
		dbx = p->GetPathpointPosition().dx;
		dby = p->GetPathpointPosition().dy;
		dbDis = CalcDist(dbx, dby, m_dbLoog, m_dbLat);
	}
	m_dbAngle = CalcAngle(dbx, dby, m_dbLoog, m_dbLat);


	if (dbLen > m_dbSpeed[m_nRoadtype]) // 如果到下个点的距离大于需要走的最大距离
	{
		GetPointOnArc(m_dbLoog, m_dbLat, x, y, m_dbSpeed[m_nRoadtype]/dbLen, a, b);
		m_dbLoog = a;
		m_dbLat = b;
	}
	else // 下个点距离在要走距离之内
	{
		m_dbLoog = x;
		m_dbLat = y;
		m_pList->GetNext(m_posList);
	}
}

void CAutoMoveByRoute::SetRoute(CGIS_FeatureRoute *pRoute)
{
	m_pFeaRoute = pRoute;
}
