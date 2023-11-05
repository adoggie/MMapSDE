// GIS_Route.cpp: implementation of the CGIS_Route class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_Route.h"
#include "GIS_WayPoint.h"
#include "GIS_PathpointMark.h"
#include "GIS_PathpointNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_Route::CGIS_Route( const CString & strRouteName/* = ""*/, const UINT & nID /*= 0*/ )
{
	m_pNWRoute		=	 NULL;
	m_strRouteName	=	 strRouteName;
	m_nRouteID		=	 nID;
}

CGIS_Route::~CGIS_Route()
{

}

void CGIS_Route::SetNetWorkRoute(CGIS_NetworkRoute *pNWRoute)
{
	m_pNWRoute = pNWRoute;
}

BOOL CGIS_Route::SaveRoute()
{
	CFile	file;
	BOOL	bOpen = file.Open( m_strRouteFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary );
	if ( !bOpen )
	{
		return FALSE;
	}

	CArchive	ar( &file, CArchive::store );
	SerializeRoute( ar );

	ar.Close();
	file.Close();

	return TRUE;
}

BOOL CGIS_Route::GetRouteInfo( const CString & strRouteFile, tgROUTEINFO * pRouteInfo )
{
	ASSERT( pRouteInfo );

	CFile	file;
	BOOL	bOpen = file.Open( strRouteFile, CFile::modeRead | CFile::typeBinary );
	if ( !bOpen )
	{
		return FALSE;
	}

	CArchive ar( &file, CArchive::load );
	pRouteInfo->Serialize( ar );

	ar.Close();
	file.Close();
	
	return TRUE;
}

//设置航线名称
void CGIS_Route::SetRouteName( const CString & strRouteName )
{
	m_strRouteName = strRouteName;
}

CString CGIS_Route::GetRouteName() const
{
	return m_strRouteName;
}

//删除一条航线
BOOL CGIS_Route::DeleteRoute( const CString & strRouteFile )
{
	if ( !DeleteFile(strRouteFile) )
	{
		return FALSE;
	}

	return TRUE;
}

void ST_GIS_ROUTEINFO::Serialize(CArchive &ar)
{
	if ( ar.IsStoring() )
	{
		ar<<enCalMethod<<enRoutePref<<enVehicleType;
		arEnWPType.Serialize( ar );
		arPtWPPos.Serialize( ar );
		arSzAnno.Serialize( ar );
		pPathPointList->Serialize( ar );
	}
	else
	{
		ar>>enCalMethod>>enRoutePref>>enVehicleType;
		arEnWPType.Serialize( ar );
		arPtWPPos.Serialize( ar );
		arSzAnno.Serialize( ar );
		pPathPointList->Serialize( ar );
	}
}

void CGIS_Route::SerializeRoute(CArchive &ar)
{
	ASSERT( m_pNWRoute );

	CGIS_NetworkPath		*	pNWPath;
	CGIS_NetworkPathInfo	*	pNWPathInfo;
	CPtrList				*	pWayPointList;
	CPtrList				*	pPathPointList;
	
	//航线相关信息
	pNWPathInfo		=	 m_pNWRoute->GetNetworkPathInfo();
	pNWPath			=	 m_pNWRoute->GetNetworkPath();
	pWayPointList	=	 m_pNWRoute->GetWayPointList();

//	pNWPathInfo->Serialize( ar );
//	pNWPath->Serialize( ar );

	CGIS_WayPoint	*	pWayPoint;
	
	POSITION	pos = pWayPointList->GetHeadPosition();
	while ( pos )
	{
		pWayPoint = (CGIS_WayPoint*)pWayPointList->GetNext( pos );
		if ( !pWayPoint )
		{
			return;
		}
		
		pWayPoint->Serialize( ar );
	}
	
	pPathPointList = pNWPath->GetPathpointList();
	
	pos = pPathPointList->GetHeadPosition();
	while ( pos )
	{
		CGIS_Pathpoint	* pPathPoint = (CGIS_Pathpoint	*)pPathPointList->GetNext( pos );
		if ( ar.IsStoring() )
		{
			ar<<pPathPoint->GetPathpointType();
		}
		else
		{
//			ar>>pPathPoint->GetPathpointType();
		}

		pPathPoint->Serialize( ar );
	}
}

void CGIS_Route::SerializeRInfo(CArchive &ar)
{

}
