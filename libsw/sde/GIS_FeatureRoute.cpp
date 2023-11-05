// GIS_FeatureRoute.cpp: implementation of the CGIS_FeatureRoute class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureRoute.h"
#include "GIS_WayPoint.h"
#include "GIS_PathpointNode.h"
#include "GIS_PathpointMark.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGIS_FeatureRoute::CGIS_FeatureRoute( )
	:CGIS_Feature( EN_GOTYPE_ROUTE )
{
	m_pNWRoute = new CGIS_NetworkRoute;
}

CGIS_FeatureRoute::CGIS_FeatureRoute( UINT nID )
	:CGIS_Feature( EN_GOTYPE_ROUTE )
{
	m_pNWRoute = new CGIS_NetworkRoute;
	m_pNWRoute->SetRouteID( nID );
}

CGIS_FeatureRoute::~CGIS_FeatureRoute()
{
	delete m_pNWRoute;
}
/*
void CGIS_FeatureRoute::SetNetworkRoute( CGIS_NetworkRoute *pNWRoute )
{
	m_pNWRoute = pNWRoute;
}
*/
CGIS_NetworkRoute * CGIS_FeatureRoute::GetNetworkRoute( )
{
	return m_pNWRoute;
}

CGIS_Feature* CGIS_FeatureRoute::Clone()
{
	CGIS_FeatureRoute * pFRoute = new CGIS_FeatureRoute();
	CGIS_NetworkRoute * pNWRoute = pFRoute->GetNetworkRoute();

	pNWRoute->SetRouteID( this->GetNetworkRoute()->GetRouteID() );
	pNWRoute->SetRouteName( this->GetNetworkRoute()->GetRouteName() );
	pNWRoute->SetNWRouteState( this->GetNetworkRoute()->GetNWRouteState() );
	
	CPtrList * pWayPtList = pNWRoute->GetWayPointList();
	CPtrList * pList = GetNetworkRoute()->GetWayPointList();
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		CGIS_WayPoint * pTmp = (CGIS_WayPoint *)pList->GetNext( pos );
		CGIS_WayPoint * pWPoint = new CGIS_WayPoint( pTmp->GetWPID() );

		pWPoint->SetWPPosition( pTmp->GetWPPosition() );
		pWPoint->SetWPType( pTmp->GetWPType() );
		
		BYTE nAnnoNum = pTmp->GetWPAnnoNum();
		char * szAnno = NULL;
		if ( nAnnoNum > 0 )
		{
			szAnno = new char[nAnnoNum];
			memcpy( szAnno, pTmp->GetWPAnnoation(), nAnnoNum );
		}
		
		pWPoint->SetWPAnnoNum( nAnnoNum );
		pWPoint->SetWPAnnoation( szAnno );
		pWPoint->SetWPRutName( pTmp->GetWPRutName() );
		pWayPtList->AddTail( pWPoint );
	}

	return pFRoute;

/*
	CGIS_NetworkPathInfo * pNWPInfo = new CGIS_NetworkPathInfo;
	pNWPInfo->SetCalPathMethod( this->GetNetworkRoute()->GetNetworkPathInfo()->GetCalPathMethod() );
	pNWPInfo->SetRoutePreference(this->GetNetworkRoute()->GetNetworkPathInfo()->GetRoutePreference() );
	pNWPInfo->SetVehicleType( this->GetNetworkRoute()->GetNetworkPathInfo()->GetVehicleType() );

	UINT	nARNum = 0;
	int * pARRutIDs = NULL;
	int * pARMapIDs = NULL;
	int * pARMapIDsNew = NULL;
	int * pARRutIDsNew = NULL;

	GetNetworkRoute()->GetNetworkPathInfo()->GetAvoidRutInfo( nARNum, &pARMapIDs, &pARRutIDs );
	if ( nARNum > 0 )
	{
		pARMapIDsNew = new int[nARNum];
		pARRutIDsNew = new int[nARNum];
		memcpy( pARMapIDsNew, pARMapIDs, nARNum );
		memcpy( pARRutIDsNew, pARRutIDs, nARNum );		
	}

	pNWPInfo->SetAvoidRutInfo( nARNum, pARMapIDsNew, pARRutIDsNew );
	pNWRoute->SetNetworkPathInfo( pNWPInfo );

	CGIS_NetworkPath * pNWPath = pNWRoute->GetNetworkPath();
	CGIS_Pathpoint * pPPoint = NULL;
	CGIS_PathpointNode * pPPNodeTmp = NULL;
	CGIS_PathpointMark * pPPMarkTmp = NULL;
	CPtrList * pPathpointListNew = pNWPath->GetPathpointList();
	CPtrList * pPathpointList = this->GetNetworkRoute()->GetNetworkPath()->GetPathpointList();
	POSITION pos = pPathpointList->GetHeadPosition();
	while ( pos )
	{
		CGIS_Pathpoint * pTmp = (CGIS_Pathpoint *)pPathpointList->GetNext( pos );
		int nType = pTmp->GetPathpointType();
		if ( nType == EN_PATHPOINTTYPE_NODE )
		{
			pPPNodeTmp = (CGIS_PathpointNode *)pTmp;
			CGIS_PathpointNode * pPPNode = new CGIS_PathpointNode;
			pPPNode->SetNextRutID( pPPNodeTmp->GetNextRutID() );
			pPPNode->SetNodeVtxID( pPPNodeTmp->GetNodeVtxID() );
			pPPNode->SetPathpointPosition( pPPNodeTmp->GetPathpointPosition() );
			pPPNode->SetPathpointType( pPPNodeTmp->GetPathpointType() );
			
			BYTE nAnnoNum = pPPNode->GetRutAnnoNum();
			char * szRutAnno = NULL;
			if ( nAnnoNum > 0 )
			{
				szRutAnno = new char[nAnnoNum];
				char * szTmp = pPPNode->GetRutAnno();
				memcpy( szRutAnno, szTmp, nAnnoNum );
			}
			
			pPPNode->SetRutAnno( szRutAnno );
			pPPNode->SetRutAnnoNum( nAnnoNum );
			pPPNode->SetTurnAngle( pPPNodeTmp->GetTurnAngle() );
			
			ST_VOICE_RUT *pVRutTmp  = pPPNodeTmp->GetVRutInfo();
			ST_VOICE_RUT *pVRut		= pPPNode->GetVRutInfo();
			memcpy( pVRut, pVRutTmp, sizeof(ST_VOICE_RUT) );

			ST_VOICE_NODE *pVNodeTmp = pPPNodeTmp->GetVNodeInfo();
			ST_VOICE_NODE *pVNode	 = pPPNode->GetVNodeInfo();
			memcpy( pVNode, pVNodeTmp, sizeof(ST_VOICE_NODE) );

			CPtrList * pNodeRutList = pPPNode->GetNodeRutList();
			CPtrList * pNodeRutListTmp = pPPNodeTmp->GetNodeRutList();
			POSITION ps = pNodeRutListTmp->GetHeadPosition();
			while ( ps )
			{
				CGIS_NetworkNodeRut * pTmp = (CGIS_NetworkNodeRut *)pNodeRutListTmp->GetNext( ps );
				CGIS_NetworkNodeRut * pNWNRut = new CGIS_NetworkNodeRut;
				pNWNRut->SetRutID( pTmp->GetRutID() );
				pNWNRut->SetRutWidth( pTmp->GetRutWidth() );
				pNWNRut->SetRutPtNum( pTmp->GetRutPtNum() );
				int nAnnoNum = pTmp->GetRutAnnoNum();
				char * szAnno = NULL;
				if ( nAnnoNum > 0 )
				{
					szAnno = new char[nAnnoNum];
					memcpy( szAnno, pTmp->GetRutAnno(), nAnnoNum );					
				}

				pNWNRut->SetRutAnnoNum( nAnnoNum );
				int nPtNum = pTmp->GetRutPtNum();
				ST_GEO_PTXY * pPtList = NULL;
				if ( nPtNum > 0 )
				{
					pPtList = new ST_GEO_PTXY[nPtNum];
					memcpy( pPtList, pTmp->GetRutPtList(), sizeof(ST_GEO_PTXY) * nPtNum );					
				}

				pNWNRut->SetRutPtList( pPtList );
				pNWNRut->SetRutPtNum( nPtNum );
				pNodeRutList->AddTail( pNWNRut );
			}

			pPathpointListNew->AddTail( pPPNode );
		}
		else if ( nType == EN_PATHPOINTTYPE_START||
					nType == EN_PATHPOINTTYPE_INTERMEDIATE||
					nType == EN_PATHPOINTTYPE_END  ) 
		{
			pPPMarkTmp = (CGIS_PathpointMark *)pTmp;
			CGIS_PathpointMark * pPPMark = new CGIS_PathpointMark;

			ST_VOICE_RUT * pVRut = pPPMark->GetVRutInfo();
			memcpy( pVRut, pPPMarkTmp->GetVRutInfo(), sizeof(ST_VOICE_RUT) );
			ST_VOICE_NODE * pVNode = pPPMark->GetVNodeInfo();
			memcpy( pVNode, pPPMarkTmp->GetVNodeInfo(), sizeof(ST_VOICE_NODE) );

			CGIS_WayPoint * pWPointTmp = pPPMarkTmp->GetWayPoint();
			CGIS_WayPoint * pWPoint = new CGIS_WayPoint(pWPointTmp->GetWPID());
			pWPoint->SetWPPosition( pWPointTmp->GetWPPosition() );

			EnMapType enMapType = (EnMapType)0;
			pWPoint->SetWPVtxID( pWPointTmp->GetWPVtxID( enMapType ), enMapType );
			pWPoint->SetWPRutID( pWPointTmp->GetWPRutID( enMapType ), enMapType );
			pWPoint->SetWPRutNID( pWPointTmp->GetWPRutNID( enMapType ), enMapType );
			pWPoint->SetWPProjPos( pWPointTmp->GetWPProjPos( enMapType), enMapType );

			CPtrList * pList = pWPoint->GetWPRutPtList( enMapType );
			CPtrList * pListTmp = pWPointTmp->GetWPRutPtList( enMapType );
			POSITION ps = pListTmp->GetHeadPosition();
			while ( ps )
			{
				ST_GEO_PTXY * ptXY = new ST_GEO_PTXY;
				ST_GEO_PTXY * ptTmp = (ST_GEO_PTXY *)pListTmp->GetNext( ps );
				memcpy( ptXY, ptTmp, sizeof(ST_GEO_PTXY) );

				pList->AddTail( ptXY );
			}
			
			pWPoint->SetWPProjPosition( pWPointTmp->GetWPProjPosition( enMapType ), enMapType );
			pWPoint->SetWPMapID( pWPointTmp->GetWPMapID(), enMapType );
			pWPoint->SetWPType( pWPointTmp->GetWPType() );

			BYTE nAnnoNum = pWPointTmp->GetWPAnnoNum();
			char * szAnno = NULL;
			if ( nAnnoNum > 0 )
			{
				szAnno = new char[nAnnoNum];
				memcpy( szAnno, pWPointTmp->GetWPAnnoation(), nAnnoNum );
			}

			pWPoint->SetWPAnnoNum( nAnnoNum );
			pWPoint->SetWPAnnoation( szAnno );
			pPathpointListNew->AddTail( pPPMark );
		}
		else if ( nType == EN_PATHPOINTTYPE_INFLEXION ) 
		{
			memcpy( pPPoint, pTmp, sizeof(CGIS_Pathpoint) );
			pPathpointListNew->AddTail( pPPoint );
		}
		else
			return NULL;
	}
*/
}

/**
 * @brief 取得大小
 *
 * @param 
 * @return
*/
long CGIS_FeatureRoute::GetFeatureSize()
{
	long		nSize	= 0;
	POSITION	pos		= NULL;

	nSize += sizeof( UINT );//m_pNWRoute->GetRouteID();
	nSize += m_pNWRoute->GetRouteName().GetLength();


	//遍历航线上所有的点获取字节大小
	CPtrList * pWPList = m_pNWRoute->GetWayPointList();

	nSize += sizeof( int );//the number of the way point
	pos = pWPList->GetHeadPosition();
	while ( pos )
	{
		CGIS_WayPoint * pWayPoint = (CGIS_WayPoint *)pWPList->GetNext( pos );
		if ( pWayPoint )
		{
			nSize += sizeof( UINT );		//pWayPoint->GetWPID()
			nSize += sizeof( int );			//pWayPoint->GetWPType()
			nSize += sizeof( ST_GEO_PTXY );	//pWayPoint->GetWPPosition()
			swString strAnno;// = pWayPoint->GetWPAnnoation();
			CharToUnicode( pWayPoint->GetWPAnnoation() , &strAnno );
			nSize += strAnno.GetLength() + 1;
			nSize += sizeof( BYTE );		//pWayPoint->GetWPAnnoNum();
		}
	}

	CGIS_NetworkPath * pNWPath = m_pNWRoute->GetNetworkPath();
	if ( !pNWPath )
	{
		return nSize;
	}
	//遍历航线路径获取大小
	CPtrList * pPPList = pNWPath->GetPathpointList();

	nSize += sizeof( int );//The number of the path point

	pos = pPPList->GetHeadPosition();
	while ( pos )
	{
		CGIS_Pathpoint * pPathPoint = (CGIS_Pathpoint *)pPPList->GetNext( pos );
		
		if ( pPathPoint )
		{
			nSize += sizeof( EnPathpointType );	//pPathPoint->GetPathpointType()
			nSize += sizeof( ST_GEO_PTXY );		//pPathPoint->GetPathpointPosition()
		}

		int nType = pPathPoint->GetPathpointType();
		if ( nType == EN_PATHPOINTTYPE_NODE )
		{
			CGIS_PathpointNode * pPNode = (CGIS_PathpointNode *)pPathPoint;
			if( pPNode )
			{
				nSize += sizeof( int );		//pPNode->GetNextRutID()
				nSize += sizeof( int );		//pPNode->GetNodeVtxID()
				nSize += sizeof( BYTE );	//pPNode->GetRutAnnoNum()
				if ( pPNode->GetRutAnnoNum() > 0 )
				{
					swString strTmp;// = pPNode->GetRutAnno();
					CharToUnicode( pPNode->GetRutAnno() , &strTmp );
					nSize += strTmp.GetLength() + 1;
				}

				nSize += sizeof( ST_VOICE_NODE );	//pPNode->GetVNodeInfo()
				nSize += sizeof( ST_VOICE_RUT );	//pPNode->GetVRutInfo()

				nSize += sizeof( int );		//The number of the CGIS_NetworkNodeRut
				CPtrList * pNRutList = pPNode->GetNodeRutList();
				POSITION p = pNRutList->GetHeadPosition();
				while ( p )
				{
					CGIS_NetworkNodeRut * pNRut = (CGIS_NetworkNodeRut*)pNRutList->GetNext( p );
					if ( pNRut )
					{
						nSize += sizeof( int );		//pNRut->GetRutID()
						nSize += sizeof( int );		//pNRut->GetRutAnnoNum()

						int nNum = pNRut->GetRutAnnoNum();
						if ( nNum > 0 )
						{
							swString str;// = pNRut->GetRutAnno();
							CharToUnicode( pNRut->GetRutAnno() , &str );
							nSize += str.GetLength() + 1;
						}

						nNum = pNRut->GetRutPtNum();
						
						nSize += sizeof( int );		//pNRut->GetRutPtNum()
						nSize += sizeof( BYTE );	//pNRut->GetRutWidth()
						nSize += nNum * sizeof( ST_GEO_PTXY );		//pNRut->GetRutPtList()
					}//End if(pNRut)
				}//End while(pos)
			}//End if(pPNode)
		}//End if( nType == ...)
		else if ( nType == EN_PATHPOINTTYPE_START||
				nType == EN_PATHPOINTTYPE_INTERMEDIATE||
				nType == EN_PATHPOINTTYPE_END )
		{
			CGIS_PathpointMark * pPMark = (CGIS_PathpointMark *)pPathPoint;

			nSize += sizeof( UINT );			//pPMark->GetWayPoint()->GetWPID()
			nSize += sizeof( ST_VOICE_RUT );	//pPMark->GetVRutInfo()
			nSize += sizeof( ST_VOICE_NODE );	//pPMark->GetVNodeInfo()
		}
	}

	return nSize;
}