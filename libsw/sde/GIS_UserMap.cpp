
#include "stdafx.h"
#include "GIS_UserMap.h"
#include "GIS_LayerManager.h"
#include "GIS_UserLayer.h"
#include "GIS_UFNavi.h"


CGIS_UserMap::CGIS_UserMap()
{
	m_pMapViewList = new CPtrList;
	m_pLayManaList = new CPtrList;
	//m_pMapInfo = new CGIS_MapInfo;
}

CGIS_UserMap::~CGIS_UserMap()
{
	POSITION pos = NULL;
	if( m_pLayManaList )
	{
		CGIS_LayerManager *pLManager = NULL;
		pos = m_pLayManaList->GetHeadPosition();
		while( pos )
		{
			pLManager = (CGIS_LayerManager*)m_pLayManaList->GetNext(pos);
			if( pLManager )
				delete pLManager;
		}
		m_pLayManaList->RemoveAll();
		delete m_pLayManaList;
	}
	if( m_pMapViewList )
	{
		m_pMapViewList->RemoveAll();
		delete m_pMapViewList;
	}
	/*
	if( m_pMapInfo )
		delete m_pMapInfo;
	m_pMapInfo = NULL;
	*/
	m_pLayManaList = NULL;
	m_pMapViewList = NULL;
	CGIS_UFNavi *pUFNavi = CGIS_UFNavi::GetUFNavi();
	if( pUFNavi )
		delete pUFNavi;
	pUFNavi = NULL;
}
BOOL CGIS_UserMap::Draw(IGIS_MapView* pMapView)
{
	return FALSE;
}
BOOL CGIS_UserMap::Attach(IGIS_MapView* pMapView)
{
	return FALSE;
}
BOOL CGIS_UserMap::Detache(IGIS_MapView* pMapView)
{
	return FALSE;
}
BOOL CGIS_UserMap::Notify(IGIS_Message* pMessage)
{
	return FALSE;
}
CGIS_LayerInfo* CGIS_UserMap::GetLayerInfo(UINT ILayerID)
{
	return NULL;
}

IGIS_TOOL* CGIS_UserMap::GetTool(EnTooType enToolType)
{
	return NULL;
}
BOOL CGIS_UserMap::ReleaseTool()
{
	return FALSE;
}
BOOL CGIS_UserMap::SaveToFile()
{
	return FALSE;
}
CPtrList *CGIS_UserMap::GetLayerManagerList()
{
	return m_pLayManaList;
}
EnMatchRoad CGIS_UserMap::MatchRoad(ST_GEO_PTXY pt,ST_GEO_PTXY& Rpt,EnMathState enState,CGIS_FeatureRut** pLastRoat,int Routedis,BOOL bHighWay ,ST_GEO_PTXY ptMatchLast,int& nLeaveCount)
{
	return EN_NAV_OUT;
}
BOOL CGIS_UserMap::OpenUserMap( swString strFName )
{
	CGIS_UFNavi *pUFNavi = CGIS_UFNavi::GetUFNavi();
	if( pUFNavi )
		return pUFNavi->LoadNaviMap( this, strFName );//准备打开automap.map
	return FALSE;
}
CGIS_UserLayer *CGIS_UserMap::GetUserLayer( EnUserMapLayerType enUMLType )
{
	POSITION pos = m_pLayManaList->GetHeadPosition();
	CGIS_UserLayer *pULayer = NULL;
	CGIS_LayerManager *pLManager = NULL;
	while( pos )
	{
		pLManager = (CGIS_LayerManager*)m_pLayManaList->GetNext(pos);
		if( pLManager )
		{
			IGIS_Layer *pLayer;
			for( int i=0; i<pLManager->GetLayerListMap()->GetCount();	i++ )
			{
				pLManager->GetLayerListMap()->Lookup( i, pLayer );
				if( pLayer )
				{
					pULayer = (CGIS_UserLayer*)pLayer;
					if( pULayer->GetUserMapLayerType() == enUMLType )
						return pULayer;
				}
			}
		}
	}
	return NULL;
}

BOOL CGIS_UserMap::InitUserMap( ){
	m_pMapInfo.SetMapBndList( NULL );
	m_pMapInfo.SetMapBndPtNum( 0 );
	m_pMapInfo.SetMapFilePath( L"Map\\" );
	m_pMapInfo.SetMapID( MAPID_UM_NAVI );
	m_pMapInfo.SetMapLayNum( 3 );
	m_pMapInfo.SetMapName( L"导航数据" );
	ST_GEO_RECT rt;
	rt.dBottom = 60;
	rt.dLeft = 70;
	rt.dRight = 135;
	rt.dTop = 3;
	m_pMapInfo.SetMapRect( rt );
	m_pMapInfo.SetMapType( EN_MAPTYPE_CITY );
	
	CGIS_LayerManager *pLMana = new CGIS_LayerManager;
	for( int i=0;	i< 3;	i++ )
	{
		CGIS_LayerInfo *pLInfo = new CGIS_LayerInfo( i );
		ST_GEO_BRUSH brush;
		brush.nForeColor = i*255;
		brush.nWidth = i+1;
		brush.nStyle = i;
		pLInfo->SetBrush( brush );
		pLInfo->SetIconID( i );
		ST_GEO_RECT rt;
		rt.dBottom = 60;
		rt.dLeft = 70;
		rt.dRight = 135;
		rt.dTop = 3;
		pLInfo->SetLayerRect( rt );
		pLInfo->SetLayerState( TRUE );
		pLInfo->SetLayerType( 5 );
		pLInfo->SetMaxARScale( (i+1)*1000 );
		pLInfo->SetMaxGRScale( (i+1)*1001 );
		pLInfo->SetMinARScale( i );
		pLInfo->SetMinGRScale( i+1 );
		ST_GEO_PEN pen;
		pen.nColor = i*200;
		pen.nStyle = i;
		pen.nWidth = i+1;
		pLInfo->SetPen( pen );
		CGIS_UserLayer *pULay = new CGIS_UserLayer( pLInfo );
		if( i == 0 )
			pULay->SetUserMapLayerType( EN_UMLAYERTYPE_ROUTE );
		else if( i == 1 )
			pULay->SetUserMapLayerType( EN_UMLAYERTYPE_TRACK );
		else if( i == 2 )
			pULay->SetUserMapLayerType( EN_UMLAYERTYPE_ADDRESSBOOK );
		pLMana->GetLayerListMap()->SetAt( i, pULay );
	}
	m_pLayManaList->AddTail( pLMana );
	return TRUE;
}
BOOL CGIS_UserMap::SaveUserMap( swString strFName )
{
	CGIS_UFNavi *pUFNavi = CGIS_UFNavi::GetUFNavi();
	if( pUFNavi )
		return pUFNavi->SaveNaviMap( this, strFName );
	return FALSE;
}
