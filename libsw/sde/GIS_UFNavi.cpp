
#include "stdafx.h"
#include "GIS_UFNavi.h"
#include "GIS_LayerManager.h"
#include "GIS_UserLayer.h"
#include "GIS_FeatureRoute.h"
#include "GIS_WayPoint.h"
#include "GIS_PathPoint.h"
#include "GIS_PathpointMark.h"
#include "GIS_PathpointNode.h"
#include "GIS_NetworkNodeRut.h"
#include "GIS_FeatureTrack.h"
#include "GIS_TrackPoint.h"

CGIS_UFNavi* CGIS_UFNavi::m_pFNavi = NULL;
//CGIS_UFNavi::CGIS_UFNavi()
//{
//
//}

CGIS_UFNavi::~CGIS_UFNavi()
{
//	if( m_pFNavi )
//		delete m_pFNavi;
	m_pFNavi = NULL;
}
CGIS_UFNavi* CGIS_UFNavi::GetUFNavi( )
{
	if( m_pFNavi == NULL )
		m_pFNavi = new CGIS_UFNavi;
	return m_pFNavi;
}
CGIS_Feature* CGIS_UFNavi::LoadNaviFeatureRouteData( CGIS_UserLayer *pLayer, UINT nID, swString strFPath )
{
	CPtrList *pOList;
	pLayer->GetObjectListMap()->Lookup( 0, pOList );
	POSITION pos = pOList->GetHeadPosition();
	CGIS_FeatureRoute *pFRoute = NULL;
	CGIS_FeatureRoute *pFeature = NULL;
	while( pos )
	{
		pFRoute = (CGIS_FeatureRoute*)pOList->GetNext( pos );
		if( pFRoute )
		{
			CPtrList *pPPList = pFRoute->GetNetworkRoute()->GetNetworkPath()->GetPathpointList();
			if( pFRoute->GetNetworkRoute()->GetRouteID() == nID )
			{
				if( pPPList->GetCount() > 0 )
					pFeature = pFRoute;
				else
				{
					swString strFName;
					strFName.Format( L"%sRoute\\R%d.dat", strFPath, nID );
					HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_ARCHIVE, NULL);
					if(INVALID_HANDLE_VALUE == hFile)
						return FALSE;
					DWORD dwSize = GetFileSize( hFile, NULL );
					DWORD dwCount = 0;
					char *pFBuf = new char[dwSize];
					ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
					if( dwSize > dwCount )
					{
						delete[] pFBuf;
						CloseHandle( hFile );
						return NULL;
					}
					DWORD dwStation = 0;
					int nPPCount = 0, nNum = 0;
					BYTE byNum = 0;
					Fread( pFBuf, dwSize, dwStation, &nPPCount, sizeof(int), 1 );
					EnPathpointType enPPType;
					CGIS_Pathpoint *pPPoint = NULL;
					ST_GEO_PTXY ptPos;
					for( int i=0;	i<nPPCount;	i++ )
					{
						pPPoint = NULL;
						Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
						enPPType = (EnPathpointType)nNum;
						switch( enPPType )
						{
						case EN_PATHPOINTTYPE_START:
						case EN_PATHPOINTTYPE_END:
						case EN_PATHPOINTTYPE_INTERMEDIATE:
							{
								CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
								pPPMark->SetPathpointType( enPPType );
								Fread( pFBuf, dwSize, dwStation, &ptPos, sizeof(ST_GEO_PTXY), 1 );
								pPPMark->SetPathpointPosition( ptPos );
								Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
								CGIS_WayPoint *pWPoint = pFRoute->GetNetworkRoute()->GetWayPointByID( nNum );
								if( pWPoint )
									pPPMark->SetWayPoint( pWPoint );
								Fread( pFBuf, dwSize, dwStation, pPPMark->GetVNodeInfo(), sizeof(ST_VOICE_NODE), 1 );
								Fread( pFBuf, dwSize, dwStation, pPPMark->GetVRutInfo(), sizeof(ST_VOICE_RUT), 1 );
								pPPoint = pPPMark;
							}
							break;
						case EN_PATHPOINTTYPE_NODE:
							{
								CGIS_PathpointNode *pPPNode = new CGIS_PathpointNode;
								pPPNode->SetPathpointType( enPPType );
								Fread( pFBuf, dwSize, dwStation, &ptPos, sizeof(ST_GEO_PTXY), 1 );
								pPPNode->SetPathpointPosition( ptPos );
								Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
								pPPNode->SetNextRutID( nNum );
								Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
								pPPNode->SetNodeVtxID( nNum );
								Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
								if( byNum > 0 )
								{
									char *szAnno = new char[byNum];
									Fread( pFBuf, dwSize, dwStation, szAnno, byNum, 1 );
									pPPNode->SetRutAnno( szAnno );
								}
								pPPNode->SetRutAnnoNum( byNum );
								Fread( pFBuf, dwSize, dwStation, pPPNode->GetVNodeInfo(), sizeof(ST_VOICE_NODE), 1 );
								Fread( pFBuf, dwSize, dwStation, pPPNode->GetVRutInfo(), sizeof(ST_VOICE_RUT), 1 );
								Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
								pPPNode->SetTurnAngle( nNum );
								int nNRNum = 0;
								Fread( pFBuf, dwSize, dwStation, &nNRNum, sizeof(int), 1 );
								CGIS_NetworkNodeRut *pNRut = NULL;
								for( int j=0;	j<nNRNum;	j++ )
								{
									pNRut = new CGIS_NetworkNodeRut;
									Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
									pNRut->SetRutID( nNum );
									Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
									if( nNum > 0 )
									{
										char *szAnno = new char[nNum];
										Fread( pFBuf, dwSize, dwStation, szAnno, nNum, 1 );
										pNRut->SetRutAnno( szAnno );
									}
									pNRut->SetRutAnnoNum( nNum );
//									Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
//									pNRut->SetRutWidth( byNum );
									Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
									ST_GEO_PTXY *pPt = new ST_GEO_PTXY[nNum];
									pNRut->SetRutPtNum( nNum );
									Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nNum, 1 );
									pNRut->SetRutPtList( pPt );
									pPPNode->GetNodeRutList()->AddTail( pNRut );
								}
								pPPoint = pPPNode;
							}
							break;
						case EN_PATHPOINTTYPE_INFLEXION:
							{
								pPPoint = new CGIS_Pathpoint;
								pPPoint->SetPathpointType( enPPType );
								Fread( pFBuf, dwSize, dwStation, &ptPos, sizeof(ST_GEO_PTXY), 1 );
								pPPoint->SetPathpointPosition( ptPos );
							}
							break;
						default:
							ASSERT( FALSE );
							break;
						}
						if( pPPoint )
							pFRoute->GetNetworkRoute()->GetNetworkPath()->GetPathpointList()->AddTail( pPPoint );
					}
					pFeature = pFRoute;
					delete[] pFBuf;
					CloseHandle( hFile );
				}
			}
			else if( pFRoute->GetNetworkRoute()->GetNetworkPath()->GetPathpointList()->GetCount() > 0 )
			{
				POSITION pos = pPPList->GetHeadPosition();
				CGIS_Pathpoint *pPPoint = NULL;
				while( pos )
				{
					pPPoint = (CGIS_Pathpoint*)pPPList->GetNext(pos);
					if( pPPoint )
						delete pPPoint;
				}
				pPPList->RemoveAll();
			}
		}
	}
	return pFeature;
}
CGIS_Feature* CGIS_UFNavi::LoadNaviFeatureTrackData( CGIS_UserLayer *pLayer, UINT nID, swString strFPath )
{
	CPtrList *pOList;
	pLayer->GetObjectListMap()->Lookup( 0, pOList );
	POSITION pos = pOList->GetHeadPosition();
	CGIS_FeatureTrack *pFTrack = NULL;
	CGIS_FeatureTrack *pFeature = NULL;
	while( pos )
	{
		pFTrack = (CGIS_FeatureTrack*)pOList->GetNext( pos );
		if( pFTrack )
		{
			CPtrList *pTPList = pFTrack->GetTrackPointList();
			if( pFTrack->GetTrackFeatureID() == nID )
			{
				if( pTPList->GetCount() > 0 )
					pFeature = pFTrack;
				else
				{
					swString strFName;
					strFName.Format( L"%Track\\T%d.dat", strFPath, nID );
					HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_ARCHIVE, NULL);
					if(INVALID_HANDLE_VALUE == hFile)
						return FALSE;
					DWORD dwSize = GetFileSize( hFile, NULL );
					DWORD dwCount = 0;
					char *pFBuf = new char[dwSize];
					ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
					if( dwSize > dwCount )
					{
						delete pFBuf;
						CloseHandle( hFile );
						return NULL;
					}
					DWORD dwStation = 0;
					int nTPCount = 0;
					short nNum = 0;
					double dbNum = 0;
					Fread( pFBuf, dwSize, dwStation, &nTPCount, sizeof(int), 1 );
					CGIS_TrackPoint *pTPoint = NULL;
					ST_GEO_PTXY ptPos;
					for( int i=0;	i<nTPCount;	i++ )
					{
						pTPoint = new CGIS_TrackPoint;
						Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(double), 1 );
						pTPoint->SetCollectionTime( dbNum );
						Fread( pFBuf, dwSize, dwStation, &ptPos, sizeof(ST_GEO_PTXY), 1 );
						pTPoint->SetLocation( ptPos );
						Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(short), 1 );
						pTPoint->SetOrientation( nNum );
						pTPList->AddTail( pTPoint );
					}
					pFeature = pFTrack;
					delete[] pFBuf;
					CloseHandle( hFile );
				}
			}
			else if( pTPList->GetCount() > 0 )
			{
				POSITION pos =pTPList ->GetHeadPosition();
				CGIS_TrackPoint *pTPoint = NULL;
				while( pos )
				{
					pTPoint = (CGIS_TrackPoint*)pTPList->GetNext(pos);
					if( pTPoint )
						delete pTPoint;
				}
				pTPList->RemoveAll();
			}
		}
	}
	return pFeature;
}
CGIS_Feature* CGIS_UFNavi::LoadNaviFeatureData( CGIS_UserLayer *pLayer, UINT nID, swString strFPath )
{
	if( pLayer->GetUserMapLayerType() == EN_UMLAYERTYPE_ROUTE )
		return LoadNaviFeatureRouteData( pLayer, nID, strFPath );
	return NULL;
}
//打开automap.map 这是个1k的小文件
BOOL CGIS_UFNavi::LoadNaviMap( CGIS_UserMap*pMap, swString strFName )
{
	HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwSize = GetFileSize( hFile, NULL );
	DWORD dwCount = 0;
	char *pFBuf = new char[dwSize];
	ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
	if( dwSize > dwCount )
	{
		delete[] pFBuf;
		CloseHandle( hFile );
		return FALSE;
	}

	DWORD dwStation = 0;
	char szInfo[32];
	Fread( pFBuf, dwSize, dwStation, szInfo, 6, 1 );
	szInfo[6] = '\0';
	swString strInfo;
	CharToUnicode( szInfo , &strInfo );
	if( strInfo.Find( L"INX V1" ) < 0 )
	{
		delete[] pFBuf;
		CloseHandle( hFile );
		return FALSE;
	}
	int n0 = strFName.ReverseFind(L'\\');
	swString str = strFName.Left( n0+1 );
	pMap->GetMapInfo()->SetMapFilePath( str );//反向的设置地图路径

	int nNum = 0;
	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
	pMap->GetMapInfo()->SetMapName( szInfo );
	
	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapID(nNum);

	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapLayNum(nNum);  // offset 0x17~0x1a,  3个地图层
	
	BYTE byNum = 0;
	float dbNum = 0.0;
	CGIS_LayerManager *pLManager = new CGIS_LayerManager();//层管理器来了
	for( int i=0;	i< pMap->GetMapInfo()->GetMapLayNum();	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(UINT), 1 );
		CGIS_LayerInfo *pLInfo = new CGIS_LayerInfo( nNum );//层编号
		ST_GEO_PEN pen;
		ST_GEO_BRUSH brush;
		//以下都是读取图层绘制信息
		Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 ); // 0x23
		Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pen.nStyle = nNum;
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		brush.nStyle = nNum;
		pLInfo->SetPen( pen );
		pLInfo->SetBrush( brush );
		///图层的显示
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		pLInfo->SetLayerState( byNum );
		//图层的scale范围
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
		pLInfo->SetMinGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
		pLInfo->SetMaxGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
		pLInfo->SetMinARScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
		pLInfo->SetMaxARScale(dbNum);
		///
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pLInfo->SetIconID(nNum);
		
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		//以上都是读取层信息
		EnUserMapLayerType enUMLType = (EnUserMapLayerType)nNum;
		CGIS_UserLayer *pLayer = new CGIS_UserLayer( pLInfo ); //创建层了
		pLayer->SetUserMapLayerType( enUMLType );
		pLayer->InitObjectListMap( 0 ); //初始化层内的图块数组
		pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer ); //加入到层管理器
	}
	pMap->GetLayerManagerList()->AddTail( pLManager );
	BYTE nBMapID = 0;
	ST_GEO_RECT rtBreadth;
	Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(BYTE), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );
	pMap->GetMapInfo()->SetMapRect( rtBreadth );
	delete[] pFBuf;
	CloseHandle( hFile );
	return TRUE;
}
BOOL CGIS_UFNavi::SaveNaviMap( CGIS_UserMap* pMap, swString strFName )
{
	swString strFolder = pMap->GetMapInfo()->GetMapFilePath() + L"Route";
	CreateDirectory(strFolder, NULL);
	strFolder = pMap->GetMapInfo()->GetMapFilePath() + L"Track";
	CreateDirectory(strFolder, NULL);
	strFolder = pMap->GetMapInfo()->GetMapFilePath() + L"Address";
	CreateDirectory(strFolder, NULL);

	HANDLE hFile = CreateFile(strFName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;
	DWORD dwCount = 0;
	char szInfo[32];
	memcpy( szInfo, "INX V1", 6 );
	WriteFile( hFile, szInfo, 6, &dwCount, NULL );

//	pMap->GetMapInfo()->GetMapName().GetLength()*2;
//	memcpy( szInfo, pMap->GetMapInfo()->GetMapName(), nNum );
	int nNum = UnicodeToChar( pMap->GetMapInfo()->GetMapName(), szInfo, 32 );
	WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
	WriteFile( hFile, szInfo, nNum, &dwCount, NULL );
	nNum = pMap->GetMapInfo()->GetMapID();
	WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
	nNum = pMap->GetMapInfo()->GetMapLayNum();
	WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );

	BYTE byNum = 0;
	float dbNum = 0.0;
	CGIS_LayerManager *pLManager = NULL;
	POSITION pos = pMap->GetLayerManagerList()->GetHeadPosition();
	CGIS_LayerInfo *pLInfo = NULL;
	IGIS_Layer *pLayer = NULL;
	while( pos )
	{
		pLManager = (CGIS_LayerManager*)pMap->GetLayerManagerList()->GetNext(pos);
		if( pLManager )
		{
			for( int i=0;	i<pLManager->GetLayerListMap()->GetCount();	i++ )
			{
				pLManager->GetLayerListMap()->Lookup( i, pLayer );
				if( pLayer )
				{
//					CGIS_UserLayer *pLayer = (CGIS_UserLayer*)pLay;
					pLInfo = pLayer->GetLayerInfo();
					nNum = pLInfo->GetLayerID();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
					ST_GEO_PEN pen = pLInfo->GetPen();
					ST_GEO_BRUSH brush = pLInfo->GetBrush();
					WriteFile( hFile, &pen.nColor, sizeof(int), &dwCount, NULL );
					WriteFile( hFile, &pen.nWidth, sizeof(BYTE), &dwCount, NULL );
					WriteFile( hFile, &brush.nForeColor, sizeof(int), &dwCount, NULL );
					WriteFile( hFile, &pen.nStyle, sizeof(int), &dwCount, NULL );
					WriteFile( hFile, &brush.nStyle, sizeof(int), &dwCount, NULL );
					byNum = pLInfo->GetLayerState();
					WriteFile( hFile, &byNum, sizeof(BYTE), &dwCount, NULL );
					dbNum = (float)pLInfo->GetMinGRScale();
					WriteFile( hFile, &dbNum, sizeof(float), &dwCount, NULL );
					dbNum = (float)pLInfo->GetMaxGRScale();
					WriteFile( hFile, &dbNum, sizeof(float), &dwCount, NULL );
					dbNum = (float)pLInfo->GetMinARScale();
					WriteFile( hFile, &dbNum, sizeof(float), &dwCount, NULL );
					dbNum = (float)pLInfo->GetMaxARScale();
					WriteFile( hFile, &dbNum, sizeof(float), &dwCount, NULL );
					nNum = pLInfo->GetLayerID();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );

					nNum = ((CGIS_UserLayer*)pLayer)->GetUserMapLayerType();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
				}
			}
		}
	}
	CloseHandle( hFile );
	return TRUE;
}
BOOL CGIS_UFNavi::LoadNaviLayer( CGIS_UserLayer *pLayer, swString strFPath )
{
	switch( pLayer->GetLayerType() )
	{
	case EN_UMLAYERTYPE_ROUTE:
		LoadNaviRouteLayer( pLayer, strFPath );
		break;
	case EN_UMLAYERTYPE_TRACK:
		break;
	case EN_UMLAYERTYPE_ADDRESSBOOK:
		break;
	default:
		break;
	}
	return TRUE;
}
BOOL CGIS_UFNavi::SaveNaviLayer( CGIS_UserLayer *pLayer, UINT nID, swString strFPath )
{
	if( pLayer->GetLayerType() == EN_UMLAYERTYPE_ROUTE )
		return SaveNaviRouteLayer( pLayer, nID, strFPath );
	else if( pLayer->GetLayerType() == EN_UMLAYERTYPE_TRACK )
		return SaveNaviTrackLayer( pLayer, nID, strFPath );
	return FALSE;
}
BOOL CGIS_UFNavi::LoadNaviRouteLayer( CGIS_UserLayer *pLayer, swString strFPath )
{
	swString strFName = strFPath + L"Route\\Route.dat";
	HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwSize = GetFileSize( hFile, NULL );
	DWORD dwCount = 0;
	char *pFBuf = new char[dwSize];
	ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
	if( dwSize > dwCount )
	{
		delete[] pFBuf;
		CloseHandle( hFile );
		return FALSE;
	}
	DWORD dwStation = 0;
	char szInfo[32];
	CPtrList *pOList = NULL;
	pLayer->GetObjectListMap()->Lookup( 0, pOList );
	int nCount = 0;
	Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
	CGIS_FeatureRoute *pFRoute = NULL;
	int nNum = 0;
	for( int i=0;	i<nCount;	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pFRoute = new CGIS_FeatureRoute( nNum );
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		if( nNum > 0 )
		{
			Fread( pFBuf, dwSize, dwStation, szInfo, nNum, 1 );
			pFRoute->GetNetworkRoute()->SetRouteName( szInfo );
		}

		int nWPCount = 0;
		Fread( pFBuf, dwSize, dwStation, &nWPCount, sizeof(int), 1 );
		CGIS_WayPoint *pWPoint = NULL;
		BYTE byNum = 0;
		EnWaypointType enWPType;
		for( int j=0;	j<nWPCount;	j++ )
		{
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			pWPoint = new CGIS_WayPoint( nNum );
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			enWPType = (EnWaypointType)nNum;
			pWPoint->SetWPType( enWPType );
			Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
			if( byNum > 0 )
			{
				char *szAnno = new char[byNum+1];
				Fread( pFBuf, dwSize, dwStation, szAnno, byNum, 1 );
				szAnno[byNum] = '\0';
				pWPoint->SetWPAnnoation( szAnno );
			}
			pWPoint->SetWPAnnoNum( byNum );
			ST_GEO_PTXY ptPos;
			Fread( pFBuf, dwSize, dwStation, &ptPos, sizeof(ST_GEO_PTXY), 1 );
			pWPoint->SetWPPosition( ptPos );
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &ptPos, sizeof(ST_GEO_PTXY), 1 );
			if( nNum == 1 )
			{
				pWPoint->SetWPMapID( nNum, EN_MAPTYPE_NATION );
				pWPoint->SetWPProjPosition( ptPos, EN_MAPTYPE_NATION );
			}
			else
			{
				pWPoint->SetWPMapID( nNum, EN_MAPTYPE_CITY );
				pWPoint->SetWPProjPosition( ptPos, EN_MAPTYPE_CITY );
			}
			pFRoute->GetNetworkRoute()->GetWayPointList()->AddTail( pWPoint );
		}
		pOList->AddTail( pFRoute );
	}
	delete[] pFBuf;
	CloseHandle( hFile );
	return TRUE;
}
BOOL CGIS_UFNavi::LoadNaviTrackLayer( CGIS_UserLayer *pLayer, swString strFPath )
{
	swString strFName = strFPath + L"Track\\Track.dat";
	HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwSize = GetFileSize( hFile, NULL );
	DWORD dwCount = 0;
	char *pFBuf = new char[dwSize];
	ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
	if( dwSize > dwCount )
	{
		delete[] pFBuf;
		CloseHandle( hFile );
		return FALSE;
	}
	DWORD dwStation = 0;
	char szInfo[32];
	CPtrList *pOList = NULL;
	pLayer->GetObjectListMap()->Lookup( 0, pOList );
	int nCount = 0;
	Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
	CGIS_FeatureTrack *pFTrack = NULL;
	int nNum = 0;
	for( int i=0;	i<nCount;	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pFTrack = new CGIS_FeatureTrack( nNum );
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		if( nNum > 0 )
		{
			Fread( pFBuf, dwSize, dwStation, szInfo, nNum, 1 );
			pFTrack->SetTrackFeatureName( szInfo );
		}
	}
	delete[] pFBuf;
	CloseHandle( hFile );
	return TRUE;
}
BOOL CGIS_UFNavi::SaveNaviRouteLayer( CGIS_UserLayer *pLayer, UINT nID, swString strFPath )
{
	swString strFName = strFPath + L"Route\\Route.dat";
	HANDLE hFile = CreateFile(strFName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwCount = 0;
	CPtrList *pOList = NULL;
	pLayer->GetObjectListMap()->Lookup( 0, pOList );
	int nNum = pOList->GetCount();
	WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
	char szInfo[32];
	POSITION pos = pOList->GetHeadPosition();
	CGIS_FeatureRoute *pFRoute = NULL;
	CGIS_FeatureRoute *pFeature = NULL;
	BOOL bFind = FALSE;
	while( pos )
	{
		pFRoute = (CGIS_FeatureRoute*)pOList->GetNext(pos);
		if( pFRoute )
		{
			if( pFRoute->GetNetworkRoute()->GetRouteID() == nID )
			{
				bFind = TRUE;
				pFeature = pFRoute;
			}
			nNum = pFRoute->GetNetworkRoute()->GetRouteID();
			WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
			nNum = UnicodeToChar( pFRoute->GetNetworkRoute()->GetRouteName(), szInfo, 32 );
//			nNum = pFRoute->GetNetworkRoute()->GetRouteName().GetLength()*2;
			WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
			if( nNum > 0 )
				WriteFile( hFile, szInfo, nNum, &dwCount, NULL );
			
			CPtrList *pWPList = pFRoute->GetNetworkRoute()->GetWayPointList();
			nNum = pWPList->GetCount();
			WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
			POSITION ps = pWPList->GetHeadPosition();
			CGIS_WayPoint *pWPoint = NULL;
			BYTE byNum = 0;
			while( ps )
			{
				pWPoint = (CGIS_WayPoint*)pWPList->GetNext(ps);
				if( pWPoint )
				{
					nNum = pWPoint->GetWPID();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
					nNum = pWPoint->GetWPType();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
					byNum = pWPoint->GetWPAnnoNum();
					WriteFile( hFile, &byNum, sizeof(BYTE), &dwCount, NULL );
					if( byNum > 0 )
						WriteFile( hFile, pWPoint->GetWPAnnoation(), byNum, &dwCount, NULL );
					WriteFile( hFile, &pWPoint->GetWPPosition(), sizeof(ST_GEO_PTXY), &dwCount, NULL );
					if( nNum = pWPoint->GetWPMapID() == 1 )
					{
						WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
						WriteFile( hFile, &pWPoint->GetWPProjPosition(EN_MAPTYPE_NATION), sizeof(ST_GEO_PTXY), &dwCount, NULL );
					}
					else
					{
						WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
						WriteFile( hFile, &pWPoint->GetWPProjPosition(EN_MAPTYPE_CITY), sizeof(ST_GEO_PTXY), &dwCount, NULL );
					}
				}
			}			
		}
	}
	CloseHandle( hFile );

	strFName.Format( L"%sRoute\\R%d.dat", strFPath, nID );
	///save the change
	if( bFind )
	{
		hFile = CreateFile(strFName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == hFile)
			return FALSE;
		CPtrList *pPPList = pFeature->GetNetworkRoute()->GetNetworkPath()->GetPathpointList();
		nNum = pPPList->GetCount();
		BYTE byNum = 0;
		WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
		CGIS_Pathpoint *pPPoint = NULL;
		pos = pPPList->GetHeadPosition();
		while( pos )
		{
			pPPoint = (CGIS_Pathpoint*)pPPList->GetNext(pos);
			if( pPPoint )
			{
				nNum = pPPoint->GetPathpointType();
				WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
				WriteFile( hFile, &pPPoint->GetPathpointPosition(), sizeof(ST_GEO_PTXY), &dwCount, NULL );
				if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END || 
					pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE ||
					pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_START )
				{
					CGIS_PathpointMark *pPPMark = (CGIS_PathpointMark*)pPPoint;
					nNum = pPPMark->GetWayPoint()->GetWPID();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
					WriteFile( hFile, pPPMark->GetVNodeInfo(), sizeof(ST_VOICE_NODE), &dwCount, NULL );
					WriteFile( hFile, pPPMark->GetVRutInfo(), sizeof(ST_VOICE_RUT), &dwCount, NULL );
				}
				else if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
				{
					CGIS_PathpointNode *pPPNode = (CGIS_PathpointNode*)pPPoint;
					nNum = pPPNode->GetNextRutID();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
					nNum = pPPNode->GetNodeVtxID();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
					byNum = pPPNode->GetRutAnnoNum();
					WriteFile( hFile, &byNum, sizeof(BYTE), &dwCount, NULL );
					if( byNum > 0 )
						WriteFile( hFile, pPPNode->GetRutAnno(), byNum, &dwCount, NULL );
					WriteFile( hFile, pPPNode->GetVNodeInfo(), sizeof(ST_VOICE_NODE), &dwCount, NULL );
					WriteFile( hFile, pPPNode->GetVRutInfo(), sizeof(ST_VOICE_RUT), &dwCount, NULL );
					nNum = pPPNode->GetTurnAngle();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );

					nNum = pPPNode->GetNodeRutList()->GetCount();
					WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
					POSITION ps = pPPNode->GetNodeRutList()->GetHeadPosition();
					CGIS_NetworkNodeRut *pNRut = NULL;
					while( ps )
					{
						pNRut = (CGIS_NetworkNodeRut*)pPPNode->GetNodeRutList()->GetNext(ps);
						if( pNRut )
						{
							nNum = pNRut->GetRutID();
							WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
							nNum = pNRut->GetRutAnnoNum();
							WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
							if( nNum )
								WriteFile( hFile, pNRut->GetRutAnno(), nNum, &dwCount, NULL );
//							BYTE byNum = pNRut->GetRutWidth();
//							WriteFile( hFile, &byNum, sizeof(BYTE), &dwCount, NULL );
							nNum = pNRut->GetRutPtNum();
							WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
							WriteFile( hFile, pNRut->GetRutPtList(), sizeof(ST_GEO_PTXY)*nNum, &dwCount, NULL );
						}
					}
				}
			}
		}
	}
	///delete the feature
	else
	{
		DeleteFile( strFName );
	}
	return TRUE;
}
BOOL CGIS_UFNavi::SaveNaviTrackLayer( CGIS_UserLayer *pLayer, UINT nID, swString strFPath )
{
	swString strFName = strFPath + L"Track\\Track.dat";
	HANDLE hFile = CreateFile(strFName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwCount = 0;
	char szInfo[32];
	CPtrList *pOList = NULL;
	pLayer->GetObjectListMap()->Lookup( 0, pOList );
	int nNum = pOList->GetCount();
	WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
	POSITION pos = pOList->GetHeadPosition();
	CGIS_FeatureTrack *pFTrack = NULL;
	CGIS_FeatureTrack *pFeature = NULL;
	BOOL bFind = FALSE;
	while( pos )
	{
		pFTrack = (CGIS_FeatureTrack*)pOList->GetNext(pos);
		if( pFTrack )
		{
			if( pFTrack->GetTrackFeatureID() == nID )
			{
				bFind = TRUE;
				pFeature = pFTrack;
			}
			nNum = pFTrack->GetTrackFeatureID();
			WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
			nNum = UnicodeToChar( pFTrack->GetTrackFeatureName(), szInfo, 32 );
//			nNum = pFTrack->GetTrackFeatureName().GetLength()*2;
			WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
			if( nNum > 0 )
				WriteFile( hFile, szInfo, nNum, &dwCount, NULL );
		}
	}
	CloseHandle( hFile );

	strFName.Format( L"%sTrack\\T%d.dat", strFPath, nID );
	///save the change
	if( bFind )
	{
		hFile = CreateFile(strFName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == hFile)
			return FALSE;
		CPtrList *pTPList = pFeature->GetTrackPointList();
		nNum = pTPList->GetCount();
		WriteFile( hFile, &nNum, sizeof(int), &dwCount, NULL );
		double dbNum = 0;
		short nOrient = 0;
		ST_GEO_PTXY ptPos;
		CGIS_TrackPoint *pTPoint = NULL;
		pos = pTPList->GetHeadPosition();
		while( pos )
		{
			pTPoint = (CGIS_TrackPoint*)pTPList->GetNext(pos);
			if( pTPoint )
			{
				dbNum = pTPoint->GetCollectionTime();
				WriteFile( hFile, &dbNum, sizeof(double), &dwCount, NULL );
				ptPos = pTPoint->GetLocation();
				WriteFile( hFile, &ptPos, sizeof(ST_GEO_PTXY), &dwCount, NULL );
				nOrient = pTPoint->GetOrientation();
				WriteFile( hFile, &nOrient, sizeof(short), &dwCount, NULL );
			}
		}
	}
	///delete the feature
	else
	{
		DeleteFile( strFName );
	}
	return TRUE;
}
