// GIS_MapNetwork.cpp: implementation of the CGIS_MapNetwork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_MapNetwork.h"
#include "GIS_LayerManager.h"
#include "GIS_Pathpoint.h"
#include "GIS_PathpointMark.h"
#include "GIS_NetworkVoice.h"
#include ".\ZLib\zlib.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*const int VEHICLETYPE_NUM = 8;
const int ROUTEPREFERENCE_NUM = 4;
const int RUTLEVEL_NUM = 7;
const double RUTCOMPOSITIVE_VALUE[][][] = {
*/
CGIS_MapNetwork::CGIS_MapNetwork()
{
	m_nMRutNum = 0;
	m_nMVtxNum = 0;
}

CGIS_MapNetwork::~CGIS_MapNetwork()
{
	CGIS_NetworkRut *pRut;
	for( int i=0;	i< m_nMRutNum;	i++ )
	{
		pRut = m_pNetRutList[i];
		delete pRut;
	}
	CGIS_NetworkVtx *pVtx;
	for( i=0;	i< m_nMVtxNum;	i++ )
	{
		pVtx = m_pNetVtxList[i];
		delete pVtx;
	}
	m_nMVtxNum = m_nMRutNum = 0;
}

// 得到节点
CGIS_NetworkVtx* CGIS_MapNetwork::GetVertex(int nID)
{
	return m_pNetVtxList[nID];
//	int  nStartID[2], nEndID[2], nHalfID[2];
//	nStartID[0] = 0;
//	nStartID[1] = m_pNetVtxList[0]->GetVtxID();
//	nEndID[0] = m_nVtxNum - 1;
//	nEndID[1] = m_pNetVtxList[m_nVtxNum-1]->GetVtxID();
//
//	if ((nID >= nStartID[1]) && (nID <= nEndID[1]))
//	{
//		while (1)
//		{
//			if (nEndID[0] - nStartID[0] <=3)
//			{
//				for (int i = nStartID[0]; i<= nEndID[0]; i++)
//				{
//					if (nID == m_pNetVtxList[i]->GetVtxID() )
//					{
//						return m_pNetVtxList[i];
//					}
//				}
//				return NULL;
//			}
//			else
//			{
//				nHalfID[0] = nStartID[0] + int((nEndID[0] - nStartID[0])/2);
//				nHalfID[1] = m_pNetVtxList[nHalfID[0]]->GetVtxID();
//				if ((nID >= nStartID[1]) && (nID <= nHalfID[1]))
//				{
//					nEndID[0] = nHalfID[0];
//					nEndID[1] = nHalfID[1];
//				}
//				else if ((nID >= nHalfID[1]) && (nID <= nEndID[1]))
//				{ 
//					nStartID[0] = nHalfID[0];
//					nStartID[1] = nHalfID[1];
//				}
//			}
//		}
//	}
//	return NULL;
}
// 得到线路
CGIS_NetworkRut* CGIS_MapNetwork::GetNetworkRut( int nID )
{
	return m_pNetRutList[nID];
//	int  nStartID[2], nEndID[2], nHalfID[2];
//	nStartID[0] = 0;
//	nStartID[1] = m_pNetRutList[0]->GetRutID();
//	nEndID[0] = m_nRutNum - 1;
//	nEndID[1] = m_pNetRutList[m_nRutNum-1]->GetRutID();
//
//	if ((nID >= nStartID[1]) && (nID <= nEndID[1]))
//	{
//		while (1)
//		{
//			if (nEndID[0] - nStartID[0] <=3)
//			{
//				for (int i = nStartID[0]; i<= nEndID[0]; i++)
//				{
//					if (nID == m_pNetRutList[i]->GetRutID() )
//					{
//						return m_pNetRutList[i];
//					}
//				}
//				return NULL;
//			}
//			else
//			{
//				nHalfID[0] = nStartID[0] + int((nEndID[0] - nStartID[0])/2);
//				nHalfID[1] = m_pNetRutList[nHalfID[0]]->GetRutID();
//				if ((nID >= nStartID[1]) && (nID <= nHalfID[1]))
//				{
//					nEndID[0] = nHalfID[0];
//					nEndID[1] = nHalfID[1];
//				}
//				else if ((nID >= nHalfID[1]) && (nID <= nEndID[1]))
//				{ 
//					nStartID[0] = nHalfID[0];
//					nStartID[1] = nHalfID[1];
//				}
//			}
//		}
//	}
//	// 定位区域：
//	return NULL;
}
///得到下个节点
CGIS_NetworkVtx* CGIS_MapNetwork::GetNextRutVtx( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut )
{
	CGIS_NetworkVtx* pRtnVtx = NULL;
	// 起点到终点
	if( GetVertex(pRut->GetRutStartVtxID()) == pVtx)
		pRtnVtx = GetVertex(pRut->GetRutEndVtxID());
	else
	{
		// 终点到起点
		if( pRut->GetRutDirection() == 0 )
			pRtnVtx = GetVertex(pRut->GetRutStartVtxID());
	}
	return pRtnVtx;
}

BOOL CGIS_MapNetwork::UnLoadMapNetwork( )
{
	CGIS_NetworkRut *pRut;
	for( int i=0;	i< m_nMRutNum;	i++ )
	{
		pRut = m_pNetRutList[i];
		if( pRut )
			delete pRut;
		pRut = NULL;
	}
	CGIS_NetworkVtx *pVtx;
	for( i=0;	i< m_nMVtxNum;	i++ )
	{
		pVtx = m_pNetVtxList[i];
		if( pVtx )
			delete pVtx;
		pVtx = NULL;
	}
	m_nMVtxNum = m_nMRutNum = 0;
	return TRUE;
}
BOOL CGIS_MapNetwork::LoadMapNetwork( )
{
	DWORD dwTime = GetTickCount();
	CString strFName = m_pMap->GetMapInfo()->GetMapFilePath()+L"net.rut";
	//open file
	HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwCSize = GetFileSize( hFile, NULL );
	DWORD dwCount = 0;
	MEMORYSTATUS ms;
	::GlobalMemoryStatus( &ms );
	char *pCFBuf = new char[dwCSize];
	if( pCFBuf == 0 )
		ASSERT(FALSE);
	ReadFile( hFile, pCFBuf, dwCSize, &dwCount, NULL);
	if( dwCSize > dwCount )
	{
		delete[] pCFBuf;
		CloseHandle( hFile );
		return FALSE;
	}
	dwCount = 0;
	int nUnCom = 5;
	Fread( pCFBuf, dwCSize, dwCount, &nUnCom, sizeof(int), 1 );
	int nCom = dwCSize - sizeof( int );
	char *pBuf = new char[nCom];
	if( pBuf == 0 )
		ASSERT(FALSE);
	char *pFBuf = NULL;
	HANDLE hFMap = NULL;
	if( nUnCom > 1048576 )
	{
		hFMap = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, nUnCom, NULL );
		if( hFMap )
			pFBuf = (char*)MapViewOfFile( hFMap, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, nUnCom );
	}
	else
		pFBuf = new char[nUnCom];

//	char *pFBuf = new char[nUnCom];
	if( pFBuf == 0 )
		ASSERT(FALSE);
	Fread( pCFBuf, dwCSize, dwCount, pBuf, nCom, 1 );
	DWORD dwSize = nUnCom;
	uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nCom );
	delete [] pCFBuf;
	delete [] pBuf;

/*	DWORD dwSize = GetFileSize( hFile, NULL );
	DWORD dwCount = 0;
	char *pFBuf = new char[dwSize];
	ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
	if( dwSize > dwCount )
		return FALSE;
*/
	///判断文件的版本信息
	DWORD dwStation = 0;
	char szInfo[8];
	Fread( pFBuf, dwSize, dwStation, szInfo, 3, 1 );
	if( CString(szInfo).Find( L"RUT" ) < 0 )
	{
		delete[] pFBuf;
		CloseHandle( hFile );
		return FALSE;
	}
	int nVer = 0;
	Fread( pFBuf, dwSize, dwStation, &nVer, sizeof(int), 1 );
	if( nVer != 910 )
	{
		delete[] pFBuf;
		CloseHandle( hFile );
		return FALSE;
	}

	///读取边界信息
	int nBndNum;
	Fread( pFBuf, dwSize, dwStation, &nBndNum, sizeof(int), 1 );
	ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nBndNum];
	Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nBndNum, 1 );
	delete pBndPtList;
//	m_pMap->GetMapInfo()->SetMapBndPtNum(m_nBndNum);
//	m_pMap->GetMapInfo()->SetMapBndList(m_pBndPtList);
	////读取导航数据
	m_nMRutNum = 0;
	m_nMVtxNum = 0;
	Fread( pFBuf, dwSize, dwStation, &m_nMRutNum, sizeof(int), 1 );
	Fread( pFBuf, dwSize, dwStation, &m_nMVtxNum, sizeof(int), 1 );
	m_nMVtxNum += 1;
	m_nMRutNum += 1;
	m_pNetRutList = (CGIS_NetworkRut**)malloc( sizeof(int)*m_nMRutNum );
	m_pNetVtxList = (CGIS_NetworkVtx**)malloc( sizeof(int)*m_nMVtxNum );
	for( int j=0;	j<m_nMRutNum;	j++ )
		m_pNetRutList[j] = NULL;
	for( j=0;	j<m_nMVtxNum;	j++ )
		m_pNetVtxList[j] = NULL;
	int nRutNum = 0;
	int nVtxNum = 0;
	Fread( pFBuf, dwSize, dwStation, &nRutNum, sizeof(int), 1 );
	Fread( pFBuf, dwSize, dwStation, &nVtxNum, sizeof(int), 1 );

//	DWORD	*pVtxStartID = new DWORD[m_nRutNum];
//	DWORD	*pVtxEndID = new DWORD[m_nRutNum];
	///读取路段信息
	unsigned short		nRutID;	//路段ID
	BYTE	byDirection;	//路段单双行标志 m,
	BYTE	byType;			//路段类别（国道..）
	BYTE	nWidth;			//路段宽度
	BYTE	nLaneNum;		//路段车道数
	BYTE	nSlope;			//路段坡度
	BYTE	byMaterial;		//路段路面材料
	BYTE	nRutLevel;		//路段等级
	BYTE	byRutType;		//路段类型（匝道..）
	float	fRutLength;		//路段长度
	DWORD	nVtxStartID;
	DWORD	nVtxEndID;
	for( int i=0;	i<nRutNum;	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &nRutID, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &byDirection, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &byType, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &nWidth, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &nLaneNum, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &nSlope, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &byMaterial, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &nRutLevel, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &byRutType, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &fRutLength, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVtxStartID, sizeof(DWORD), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVtxEndID, sizeof(DWORD), 1 );
		
		m_pNetRutList[nRutID] = new CGIS_NetworkRut;
		m_pNetRutList[nRutID]->SetRutID( nRutID );
		m_pNetRutList[nRutID]->SetRutDirection( byDirection );
		m_pNetRutList[nRutID]->SetRutType( byRutType );
		m_pNetRutList[nRutID]->SetType( byType );
		m_pNetRutList[nRutID]->SetRutWidth( nWidth );
		m_pNetRutList[nRutID]->SetRutLaneNum( nLaneNum );
		m_pNetRutList[nRutID]->SetRutSlope( nSlope );
		m_pNetRutList[nRutID]->SetRutMaterial( byMaterial );
		m_pNetRutList[nRutID]->SetRutLevel( nRutLevel );
		m_pNetRutList[nRutID]->SetRutLength( fRutLength );
		m_pNetRutList[nRutID]->SetRutStartVtxID( nVtxStartID );
		m_pNetRutList[nRutID]->SetRutEndVtxID( nVtxEndID );
	}
	//读取节点信息
	unsigned short nVtxID;		//节点ID
	ST_GEO_PTXY ptPosition;		//节点坐标
	int nVRutNum;					//节点的路段数
	BYTE nRTurnNum = 0;
	for( i=0;	i<nVtxNum;	i++ )
	{
		nVtxID = 0;
		Fread( pFBuf, dwSize, dwStation, &nVtxID, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &ptPosition, sizeof(ST_GEO_PTXY), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVRutNum, sizeof(int), 1 );
		int *pRutID = NULL;
		int *pVRAngle = NULL;
		if( nVRutNum > 0 )
		{
			pRutID = new int[nVRutNum];
			Fread( pFBuf, dwSize, dwStation, pRutID, sizeof(int)*nVRutNum, 1 );
			pVRAngle = new int[nVRutNum];
			Fread( pFBuf, dwSize, dwStation, pVRAngle, sizeof(int)*nVRutNum, 1 );
		}
		Fread( pFBuf, dwSize, dwStation, &nRTurnNum, sizeof(BYTE), 1 );
		BYTE *pRTList = NULL;
		if( nRTurnNum )
		{
			pRTList = new BYTE[nRTurnNum*8];
			Fread( pFBuf, dwSize, dwStation, pRTList, sizeof(BYTE)*nRTurnNum*8, 1 );
			delete pRTList;
			pRTList = NULL;
		}

		m_pNetVtxList[nVtxID] = new CGIS_NetworkVtx;
		m_pNetVtxList[nVtxID]->SetVtxID( nVtxID );
		m_pNetVtxList[nVtxID]->SetVtxPosition( ptPosition );
		m_pNetVtxList[nVtxID]->SetVRAngleList( pVRAngle );
		m_pNetVtxList[nVtxID]->SetRutTurnList( pRTList );
		m_pNetVtxList[nVtxID]->SetNodeRuts( pRutID, nVRutNum );
	}
	if( hFMap )
	{
		UnmapViewOfFile( pFBuf );
		CloseHandle( hFMap );
	}
	else
		delete []pFBuf;
	CloseHandle( hFile );
	TRACE( L"LoadNetworkTime:%d\n", GetTickCount()-dwTime );
	return TRUE;
}
/**
 * @brief 填充途经点的数据
 *
 * @param 
 * @return
*/
BOOL CGIS_MapNetwork::FullWayPointData( CGIS_WayPoint *pWayPoint, CGIS_GeoMap *pMap )
{
	ASSERT(pWayPoint);
	ASSERT(pMap);
	
	CGIS_MapInfo* pInfo = pMap->GetMapInfo();
	ASSERT(pInfo);
	CPtrList* pList = pWayPoint->GetWPRutPtList(pInfo->GetMapType());
	ASSERT(pList);
	
	if( !pList->IsEmpty() )
		return TRUE;
	
	if( pMap->CalWayPointRut( pWayPoint ) )
	{
		//计算途经点的路段
		
		EnMapType enMapType = pMap->GetMapInfo()->GetMapType();
		CGIS_NetworkRut* pNRut = GetNetworkRut( pWayPoint->GetWPRutID(enMapType) );
		if( pNRut == NULL )
			return FALSE;

		int nVtxID = 
			CalcDist(
				pWayPoint->GetWPProjPosition(enMapType).dx,
				pWayPoint->GetWPProjPosition(enMapType).dy, 
				GetVertex(pNRut->GetRutStartVtxID())->GetVtxPosition().dx, 
				GetVertex(pNRut->GetRutStartVtxID())->GetVtxPosition().dy)
				> 
			CalcDist(
				pWayPoint->GetWPProjPosition(enMapType).dx, 
				pWayPoint->GetWPProjPosition(enMapType).dy, 
				GetVertex(pNRut->GetRutEndVtxID())->GetVtxPosition().dx, 
				GetVertex(pNRut->GetRutEndVtxID())->GetVtxPosition().dy)
				?
				pNRut->GetRutEndVtxID():pNRut->GetRutStartVtxID();
		
		pWayPoint->SetWPVtxID( nVtxID, enMapType );
		return pMap->FullWaypointRut(
			pWayPoint, 
			pWayPoint->GetWPRutID(enMapType), 
			GetVertex(pNRut->GetRutStartVtxID()), 
			GetVertex(pNRut->GetRutEndVtxID()) 
			);
	}
	return FALSE;
}
/**
 * @brief 计算导航路径
 *
 * @param WayPointList 航点列表(入口参数)
 * @param pPathpointlist 途经点列表(出口参数)

 * @return BOOL
*/
BOOL CGIS_MapNetwork::CalculateNavigatePath( CPtrList* WayPointList ,CPtrList* pPathpointlist)
{
	ASSERT( WayPointList );
	
	ASSERT( WayPointList->GetCount() >=2 );
	ASSERT( m_pNetworkRoute );

	
	CGIS_WayPoint *pWPOne = (CGIS_WayPoint*)WayPointList->GetHead();
	if( !FullWayPointData(pWPOne, m_pMap) )
		return FALSE;
	POSITION pos = WayPointList->GetHeadPosition();
	pWPOne = (CGIS_WayPoint*)WayPointList->GetNext(pos);
	CGIS_WayPoint *pWPTwo;
	while( pos )
	{
		pWPTwo = (CGIS_WayPoint*)WayPointList->GetNext(pos);
		if( !FullWayPointData(pWPTwo,m_pMap))
			return FALSE;
		BOOL bFind = CalculateNavigatePathLeg(pWPOne,pWPTwo, m_pNetworkRoute->GetNetworkPathInfo(),	m_pMap, pPathpointlist);
		pWPOne = pWPTwo;
		if(!bFind)
			return FALSE;
	}
	return FALSE;
}

///获得要计算路径的途径点的节点信息
BOOL CGIS_MapNetwork::GetRealVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CGIS_NetworkVtx **pStartVtx, CGIS_NetworkVtx **pEndVtx )
{
	EnMapType enMapType = pMap->GetMapInfo()->GetMapType();
	CGIS_NetworkRut *pRut = GetNetworkRut( pWPOne->GetWPRutID(enMapType) );
	if( pRut->GetRutDirection() != 0 )
		*pStartVtx = GetVertex(pRut->GetRutEndVtxID());
	else
		*pStartVtx = GetVertex( pWPOne->GetWPVtxID(enMapType) );
	pRut = GetNetworkRut( pWPTwo->GetWPRutID(enMapType) );
	if( pRut->GetRutDirection() != 0 )
		*pEndVtx = GetVertex(pRut->GetRutStartVtxID());
	else
		*pEndVtx = GetVertex( pWPTwo->GetWPVtxID(enMapType) );
	return TRUE;
}
///填充起始waypoint路段信息
BOOL CGIS_MapNetwork::FullWPStartPath( CGIS_WayPoint *pWPOne, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, CGIS_GeoMap *pMap, BOOL &bRutSame, CPtrList *pPathPointList, POSITION &pos )
{
	EnMapType enMapType = pMap->GetMapInfo()->GetMapType();
	if( pWPOne->GetWPType() == EN_WAYPOINTTYPE_ORIGIN )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPOne->GetWPProjPosition(enMapType) );
		pPPMark->SetWayPoint( pWPOne );
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_START );
		CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPOne->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
		CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
		pos = pPathPointList->AddTail( pPPMark );
		TRACE( L"Start Pathpoint" );
	}
	bRutSame = FALSE;
	BOOL bOrder = FALSE;
	if( pRut->GetRutID() == pWPOne->GetWPRutID(enMapType) )
	{
		bRutSame = TRUE;
		if( pVtx == GetVertex(pRut->GetRutStartVtxID()) )
			bOrder = TRUE;
		else 
			bOrder = FALSE;
	}
	else
	{
		bRutSame = FALSE;
		CGIS_NetworkRut *pNRut = GetNetworkRut( pWPOne->GetWPRutID(enMapType) );
		if( pVtx == GetVertex(pNRut->GetRutStartVtxID()) )
			bOrder = TRUE;
		else
			bOrder = FALSE;
	}
	///填充waypoint相关路径上的数据
	ST_GEO_PTXY *pPt = NULL;
	POSITION ps = pWPOne->GetWPProjPos( enMapType );
	if( bRutSame == TRUE )
	{
		if( pWPOne->GetWPRutPtList(enMapType)->GetCount() <= 2 )
			return TRUE;
		if( bOrder )
		{
			while( ps )
			{
				pPt = (ST_GEO_PTXY*)pWPOne->GetWPRutPtList(enMapType)->GetAt(ps);
				if( pPt && (ps != pWPOne->GetWPRutPtList(enMapType)->GetTailPosition()) )
				{
					CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
					pPathPt->SetPathpointPosition( *pPt );
					pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
					pos = pPathPointList->InsertAfter( pos, pPathPt );
				}
				pWPOne->GetWPRutPtList(enMapType)->GetNext(ps);
			}
		}
		else
		{
			pWPOne->GetWPRutPtList(enMapType)->GetPrev(ps);
			while( ps )
			{
				pPt = (ST_GEO_PTXY*)pWPOne->GetWPRutPtList(enMapType)->GetAt(ps);
				if( pPt && ( ps != pWPOne->GetWPRutPtList(enMapType)->GetHeadPosition()) )
				{
					CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
					pPathPt->SetPathpointPosition( *pPt );
					pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
					pos = pPathPointList->InsertAfter( pos, pPathPt );
				}
				pWPOne->GetWPRutPtList(enMapType)->GetPrev(ps);
			}
		}
	}
	else
	{
		if( pWPOne->GetWPRutPtList(enMapType)->GetCount() <= 2 )
			return TRUE;
		if( bOrder )
		{
			pWPOne->GetWPRutPtList(enMapType)->GetPrev(ps);
			while( ps )
			{
				pPt = (ST_GEO_PTXY*)pWPOne->GetWPRutPtList(enMapType)->GetAt(ps);
				if( pPt && (ps != pWPOne->GetWPRutPtList(enMapType)->GetHeadPosition()) )
				{
					CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
					pPathPt->SetPathpointPosition( *pPt );
					pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
					pos = pPathPointList->InsertAfter( pos, pPathPt );
				}
				pWPOne->GetWPRutPtList(enMapType)->GetPrev(ps);
			}
		}
		else
		{
			while( ps )
			{
				pPt = (ST_GEO_PTXY*)pWPOne->GetWPRutPtList(enMapType)->GetAt(ps);
				if( pPt && (ps != pWPOne->GetWPRutPtList(enMapType)->GetTailPosition()) )
				{
					CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
					pPathPt->SetPathpointPosition( *pPt );
					pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
					pos = pPathPointList->InsertAfter( pos, pPathPt );
				}
				pWPOne->GetWPRutPtList(enMapType)->GetNext(ps);
			}
		}
	}
	return TRUE;
}
///填充终止waypoint路段信息
BOOL CGIS_MapNetwork::FullWPEndPath( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, int nRutID, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos )
{
	////尾部waypoint的处理
	EnMapType enMapType = pMap->GetMapInfo()->GetMapType();
	BOOL bRutSame = FALSE, bOrder = FALSE;
	if( nRutID == pWPTwo->GetWPRutID(enMapType) )
	{
		bRutSame = TRUE;
		if( pVtx == GetVertex(pRut->GetRutStartVtxID()) )
			bOrder = TRUE;
		else 
			bOrder = FALSE;
	}
	else
	{
		bRutSame = FALSE;
		CGIS_NetworkRut *pNRut = GetNetworkRut( pWPTwo->GetWPRutID(enMapType) );
		if( pVtx == GetVertex(pNRut->GetRutStartVtxID()) )
			bOrder = TRUE;
		else
			bOrder = FALSE;
	}
	///填充waypoint相关路径上的数据
	ST_GEO_PTXY *pPt;
	pPt = (ST_GEO_PTXY*)(pWPTwo->GetWPRutPtList(enMapType)->GetAt(pWPTwo->GetWPProjPos(enMapType)));
	if( bRutSame )
	{
		POSITION ps = pWPTwo->GetWPProjPos(enMapType);
		int nCount = 0;
		if( pWPTwo->GetWPRutPtList(enMapType)->GetCount() <= 2 )
			nCount = 1;
		else
		{
			if( bOrder )
			{
				pWPTwo->GetWPRutPtList(enMapType)->GetPrev( ps );
				while( ps )
				{
					nCount++;
					pWPTwo->GetWPRutPtList(enMapType)->GetPrev( ps );
				}
			}
			else
			{
				while( ps )
				{
					nCount++;
					pWPTwo->GetWPRutPtList(enMapType)->GetNext( ps );
				}
			}
		}
		for( int i=0;	i< nCount;	i++ )
		{
			ps = pos;
			CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPathPointList->GetAt( pos );
			if( pPPoint )
				delete pPPoint;
			pPathPointList->GetPrev( pos );
			pPathPointList->RemoveAt( ps );
		}
	}
	else
	{
		if( pWPTwo->GetWPRutPtList(enMapType)->GetCount() <= 2 )
			return TRUE;
		POSITION ps = pWPTwo->GetWPProjPos(enMapType);
		POSITION ps1 = NULL;
		if( bOrder )
		{
			ps1 = pWPTwo->GetWPRutPtList(enMapType)->GetHeadPosition();
			pWPTwo->GetWPRutPtList(enMapType)->GetNext(ps1);
			while( ps1 )
			{
				pPt = (ST_GEO_PTXY*)pWPTwo->GetWPRutPtList(enMapType)->GetAt(ps1);
				if( ps1 == ps )
					return TRUE;
				if( pPt )
				{
					CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
					pPathPt->SetPathpointPosition( *pPt );
					pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
					pos = pPathPointList->InsertAfter( pos, pPathPt );
				}
				pWPTwo->GetWPRutPtList(enMapType)->GetNext(ps1);
			}
		}
		else
		{
			ps1 = pWPTwo->GetWPRutPtList(enMapType)->GetTailPosition();
			while( ps1 )
			{
				pPt = (ST_GEO_PTXY*)pWPTwo->GetWPRutPtList(enMapType)->GetAt(ps1);
				if( ps == ps1 )
					return TRUE;
				if( pPt )
				{
					CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
					pPathPt->SetPathpointPosition( *pPt );
					pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
					pos = pPathPointList->InsertAfter( pos, pPathPt );
				}
				pWPTwo->GetWPRutPtList(enMapType)->GetPrev(ps1);
			}
		}
	}
	return TRUE;
}
///填充终止waypoint路段信息
BOOL CGIS_MapNetwork::FullWPEndPath( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos )
{
	////尾部waypoint的处理
	EnMapType enMapType = pMap->GetMapInfo()->GetMapType();
	BOOL bOrder = FALSE;
	CGIS_NetworkRut *pNRut = GetNetworkRut( pWPTwo->GetWPRutID(enMapType) );
	if( pVtx == GetVertex(pNRut->GetRutStartVtxID()) )
		bOrder = TRUE;
	else
		bOrder = FALSE;
	///填充waypoint相关路径上的数据
	ST_GEO_PTXY *pPt;
	pPt = (ST_GEO_PTXY*)(pWPTwo->GetWPRutPtList(enMapType)->GetAt(pWPTwo->GetWPProjPos(enMapType)));
	if( pWPTwo->GetWPRutPtList(enMapType)->GetCount() <= 2 )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(enMapType) );
		pPPMark->SetWayPoint( pWPTwo );
		CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
		CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
		pos = pPathPointList->AddTail( pPPMark );
		return TRUE;
	}
	POSITION ps = pWPTwo->GetWPProjPos(enMapType);
	POSITION ps1 = NULL;
	BOOL bReturn = FALSE;
	if( bOrder )
	{
		ps1 = pWPTwo->GetWPRutPtList(enMapType)->GetHeadPosition();
		pWPTwo->GetWPRutPtList(enMapType)->GetNext(ps1);
		while( ps1 )
		{
			pPt = (ST_GEO_PTXY*)pWPTwo->GetWPRutPtList(enMapType)->GetAt(ps1);
			if( ps1 == ps )
				bReturn = TRUE;
			if( bReturn )
			{
				CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
				pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(enMapType) );
				pPPMark->SetWayPoint( pWPTwo );
				CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
				CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
				pos = pPathPointList->AddTail( pPPMark );
				return TRUE;
			}
			if( pPt )
			{
				CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
				pPathPt->SetPathpointPosition( *pPt );
				pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
				pos = pPathPointList->InsertAfter( pos, pPathPt );
			}
			pWPTwo->GetWPRutPtList(enMapType)->GetNext(ps1);
		}
	}
	else
	{
		ps1 = pWPTwo->GetWPRutPtList(enMapType)->GetTailPosition();
		while( ps1 )
		{
			pPt = (ST_GEO_PTXY*)pWPTwo->GetWPRutPtList(enMapType)->GetAt(ps1);
			if( ps == ps1 )
				bReturn = TRUE;
			if( pPt )
			{
				CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
				pPathPt->SetPathpointPosition( *pPt );
				pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
				pos = pPathPointList->InsertAfter( pos, pPathPt );
			}
			if( bReturn )
			{
				CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
				pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(enMapType) );
				pPPMark->SetWayPoint( pWPTwo );
				CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
				CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
				pos = pPathPointList->AddTail( pPPMark );
				return TRUE;
			}
			pWPTwo->GetWPRutPtList(enMapType)->GetPrev(ps1);
		}
	}
	return TRUE;
}
///填充路线当两waypoint在同一路段的情况
BOOL CGIS_MapNetwork::FullWPOneAndWPTwoOnSameRut( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos )
{
	EnMapType enMapType = pMap->GetMapInfo()->GetMapType();
	POSITION ps = pWPOne->GetWPRutPtList(enMapType)->GetHeadPosition();
	ST_GEO_PTXY *pPt;
	int nStart = 0, nEnd = 0;
	int nCount = 0;
	while( ps )
	{
		pWPOne->GetWPRutPtList(enMapType)->GetNext( ps );
		if( ps == pWPOne->GetWPProjPos(enMapType) )
		{
			nStart = nCount;
			break;
		}
		nCount++;
	}
	ps = pWPTwo->GetWPRutPtList(enMapType)->GetHeadPosition();
	nCount = 0;
	while( ps )
	{
		pWPTwo->GetWPRutPtList(enMapType)->GetNext( ps );
		if( ps == pWPTwo->GetWPProjPos(enMapType) )
		{
			nEnd = nCount;
			break;
		}
		nCount++;
	}
	if( pWPOne->GetWPType() == EN_WAYPOINTTYPE_ORIGIN )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPOne->GetWPProjPosition(enMapType) );
		pPPMark->SetWayPoint( pWPOne );
		CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPOne->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
		CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
		pos = pPathPointList->AddTail( pPPMark );
	}
	int nNum = abs(nStart-nEnd);
	ST_GEO_PTXY *pPtList = new ST_GEO_PTXY[nNum];
	if( nStart > nEnd )
	{
		nCount = 0;
		ps = pWPOne->GetWPProjPos(enMapType);
		pWPOne->GetWPRutPtList(enMapType)->GetPrev( ps );
		for( int i=0;	i<nNum;	i++ )
		{
			pPt = (ST_GEO_PTXY*)pWPOne->GetWPRutPtList(enMapType)->GetAt( ps );
			if( pPt )
				pPtList[i] = *pPt;
			pWPOne->GetWPRutPtList(enMapType)->GetPrev( ps );
		}
	}
	else if( nStart < nEnd )
	{
		ps = pWPOne->GetWPProjPos(enMapType);
		for( int i=0;	i<nNum;	i++ )
		{
			pPt = (ST_GEO_PTXY*)pWPOne->GetWPRutPtList(enMapType)->GetAt( ps );
			if( pPt )
				pPtList[i] = *pPt;
			pWPOne->GetWPRutPtList(enMapType)->GetNext( ps );
		}
	}
	for( int i=0;	i<nNum;	i++ )
	{
		CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
		pPathPt->SetPathpointPosition( pPtList[i] );
		pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
		pos = pPathPointList->InsertAfter( pos, pPathPt );
	}
	CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
	pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(enMapType) );
	pPPMark->SetWayPoint( pWPTwo );
	CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
	CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
	pos = pPathPointList->AddTail( pPPMark );
	return TRUE;
}
///填充路线当两waypoint在同一节点的情况
BOOL CGIS_MapNetwork::FullWPOneAndWPTwoOnSameVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos )
{
	EnMapType enMapType = pMap->GetMapInfo()->GetMapType();
	if( pWPOne->GetWPType() == EN_WAYPOINTTYPE_ORIGIN )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPOne->GetWPProjPosition(enMapType) );
		pPPMark->SetWayPoint( pWPOne );
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_START );
		CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPOne->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
		CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
		pos = pPathPointList->AddTail( pPPMark );
		TRACE( L"Start Pathpoint" );
	}
	BOOL bOrder = FALSE;
	if( GetNetworkRut(pWPOne->GetWPRutID(enMapType))->GetRutStartVtxID() == pWPOne->GetWPVtxID(enMapType) )
		bOrder = FALSE;
	else
		bOrder = TRUE;
	///填充waypoint相关路径上的数据
	ST_GEO_PTXY *pPt = NULL;
	POSITION ps = pWPOne->GetWPProjPos( enMapType );
	if( pWPOne->GetWPRutPtList(enMapType)->GetCount() > 2 )
	{
		if( bOrder )
		{
			while( ps )
			{
				pPt = (ST_GEO_PTXY*)pWPOne->GetWPRutPtList(enMapType)->GetAt(ps);
				if( pPt && (ps != pWPOne->GetWPRutPtList(enMapType)->GetTailPosition()) )
				{
					CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
					pPathPt->SetPathpointPosition( *pPt );
					pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
					pos = pPathPointList->InsertAfter( pos, pPathPt );
				}
				pWPOne->GetWPRutPtList(enMapType)->GetNext(ps);
			}
		}
		else
		{
			pWPOne->GetWPRutPtList(enMapType)->GetPrev(ps);
			while( ps )
			{
				pPt = (ST_GEO_PTXY*)pWPOne->GetWPRutPtList(enMapType)->GetAt(ps);
				if( pPt && ( ps != pWPOne->GetWPRutPtList(enMapType)->GetHeadPosition()) )
				{
					CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
					pPathPt->SetPathpointPosition( *pPt );
					pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
					pos = pPathPointList->InsertAfter( pos, pPathPt );
				}
				pWPOne->GetWPRutPtList(enMapType)->GetPrev(ps);
			}
		}
	}
	pMap->GetRutData( GetVertex(pWPOne->GetWPVtxID(enMapType)), pWPOne->GetWPRutID(enMapType), pWPTwo->GetWPRutID(enMapType),
				pWPTwo->GetWPRutID(enMapType), NULL, pPathPointList, pos );
	////尾部waypoint的处理
	if( GetNetworkRut(pWPTwo->GetWPRutID(enMapType))->GetRutStartVtxID() == pWPTwo->GetWPVtxID(enMapType) )
		bOrder = TRUE;
	else
		bOrder = FALSE;
	///填充waypoint相关路径上的数据
	pPt = (ST_GEO_PTXY*)(pWPTwo->GetWPRutPtList(enMapType)->GetAt(pWPTwo->GetWPProjPos(enMapType)));
	if( pWPTwo->GetWPRutPtList(enMapType)->GetCount() <= 2 )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(enMapType) );
		pPPMark->SetWayPoint( pWPTwo );
		CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
		CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
		pos = pPathPointList->AddTail( pPPMark );
		return TRUE;
	}
	ps = pWPTwo->GetWPProjPos(enMapType);
	POSITION ps1 = NULL;
	BOOL bReturn = FALSE;
	if( bOrder )
	{
		ps1 = pWPTwo->GetWPRutPtList(enMapType)->GetHeadPosition();
		pWPTwo->GetWPRutPtList(enMapType)->GetNext(ps1);
		while( ps1 )
		{
			pPt = (ST_GEO_PTXY*)pWPTwo->GetWPRutPtList(enMapType)->GetAt(ps1);
			if( ps1 == ps )
				bReturn = TRUE;
			if( bReturn )
			{
				CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
				pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(enMapType) );
				pPPMark->SetWayPoint( pWPTwo );
				CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
				CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
				pos = pPathPointList->AddTail( pPPMark );
				return TRUE;
			}
			if( pPt )
			{
				CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
				pPathPt->SetPathpointPosition( *pPt );
				pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
				pos = pPathPointList->InsertAfter( pos, pPathPt );
			}
			pWPTwo->GetWPRutPtList(enMapType)->GetNext(ps1);
		}
	}
	else
	{
		ps1 = pWPTwo->GetWPRutPtList(enMapType)->GetTailPosition();
		while( ps1 )
		{
			pPt = (ST_GEO_PTXY*)pWPTwo->GetWPRutPtList(enMapType)->GetAt(ps1);
			if( ps == ps1 )
				bReturn = TRUE;
			if( pPt )
			{
				CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
				pPathPt->SetPathpointPosition( *pPt );
				pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
				pos = pPathPointList->InsertAfter( pos, pPathPt );
			}
			if( bReturn )
			{
				CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
				pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(enMapType) );
				pPPMark->SetWayPoint( pWPTwo );
				CGIS_NetworkVoice::FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutID(enMapType) ), pPPMark->GetVRutInfo() );
				CGIS_NetworkVoice::FullVNodeInfo( pPPMark->GetVNodeInfo() );
				pos = pPathPointList->AddTail( pPPMark );
				return TRUE;
			}
			pWPTwo->GetWPRutPtList(enMapType)->GetPrev(ps1);
		}
	}
	return TRUE;
}

///填充路线pathleg数据
BOOL CGIS_MapNetwork::FullPathLegData( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CPtrList *pRutList, CPtrList *pVtxList, CGIS_GeoMap *pMap, CPtrList *pPathPointList )
{
	///两目标点在相同的路段上
	EnMapType enMapType = pMap->GetMapInfo()->GetMapType();
	POSITION pos = pPathPointList->GetTailPosition();
	if( pWPOne->GetWPRutID(enMapType) == pWPTwo->GetWPRutID(enMapType) )
		return FullWPOneAndWPTwoOnSameRut( pWPOne, pWPTwo, pMap, pPathPointList, pos );

	BOOL bRut = FALSE;
	CGIS_NetworkVtx *pVtx = NULL;
	CGIS_NetworkRut *pRut = NULL;
	if( pRutList->GetCount() == 0 )
		return FullWPOneAndWPTwoOnSameVtx( pWPOne, pWPTwo, pMap, pPathPointList, pos );

	ASSERT( pRutList->GetCount() >= 1 );
	////判断起始目标点
	pVtx = (CGIS_NetworkVtx*)pVtxList->GetHead( );
	pRut = (CGIS_NetworkRut*)pRutList->GetHead( );

	FullWPStartPath( pWPOne, pVtx, pRut, pMap, bRut, pPathPointList, pos );
	int nPreRutID = pWPOne->GetWPRutID(enMapType);
	if( bRut )
	{
		pVtxList->RemoveHead();
		pRutList->RemoveHead();
	}
	CGIS_BreadthMap *pBMap = NULL;
	if( pRutList->IsEmpty() )
	{
		pVtx = (CGIS_NetworkVtx*)pVtxList->GetHead( );
		pMap->GetRutData( pVtx, pRut->GetRutID(), pWPTwo->GetWPRutID(enMapType), pWPTwo->GetWPRutID(enMapType), pBMap, pPathPointList, pos );
		if( pBMap )
			pMap->UnLoadBLRutData( pBMap );
//			pMap->UnLoadBreadthMap( pBMap );
		return FullWPEndPath( pWPTwo, pVtx, pMap, pPathPointList, pos );
	}
/*	if( pRutList->IsEmpty() )
	{
		pVtx = (CGIS_NetworkVtx*)pVtxList->GetHead( );
		pMap->GetRutData( pVtx, pRut->GetRutID(), pWPTwo->GetWPRutID(), pWPTwo->GetWPRutID(), pPathPointList, pos );
		return FullWPEndPath( pWPTwo, pVtx, pPathPointList, pos );
	}
*/	pRut = (CGIS_NetworkRut*)pRutList->GetTail();
	if( pRut )
	{
		if( pRut->GetRutID() == pWPTwo->GetWPRutID(enMapType) )
		{
			pVtxList->RemoveTail();
			pRutList->RemoveTail();
		}
	}
	POSITION psRut = pRutList->GetHeadPosition();
	POSITION psVtx = pVtxList->GetHeadPosition();
	pBMap = NULL;
	while( psRut )
	{
		pRut = (CGIS_NetworkRut*)pRutList->GetNext( psRut );
		pVtx = (CGIS_NetworkVtx*)pVtxList->GetNext( psVtx );
		pMap->GetRutData( pVtx, nPreRutID, pRut->GetRutID(), pWPTwo->GetWPRutID(enMapType), pBMap, pPathPointList, pos );
		nPreRutID = pRut->GetRutID();
	}
	pVtx = (CGIS_NetworkVtx*)pVtxList->GetTail();
	pMap->GetRutData( pVtx, nPreRutID, pWPTwo->GetWPRutID(enMapType), pWPTwo->GetWPRutID(enMapType), pBMap, pPathPointList, pos );
	if( pBMap )
//		pMap->UnLoadBreadthMap( pBMap );
		pMap->UnLoadBLRutData( pBMap );
/*	while( psRut )
	{
		pRut = (CGIS_NetworkRut*)pRutList->GetNext( psRut );
		pVtx = (CGIS_NetworkVtx*)pVtxList->GetNext( psVtx );
		pMap->GetRutData( pVtx, nPreRutID, pRut->GetRutID(), pWPTwo->GetWPRutID(), pPathPointList, pos );
		nPreRutID = pRut->GetRutID();
	}
	pVtx = (CGIS_NetworkVtx*)pVtxList->GetTail();
	pMap->GetRutData( pVtx, nPreRutID, pWPTwo->GetWPRutID(), pWPTwo->GetWPRutID(), pPathPointList, pos );
*/	
	FullWPEndPath( pWPTwo, pVtx, pMap, pPathPointList, pos );

	return TRUE;
}
//BOOL CGIS_MapNetwork::CalculateNavigatePathLeg( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList )
//{
//	CPtrList SOList, SCList;	//起始端
//	CPtrList EOList, ECList;	//终点端
//	CGIS_NetworkVtx *pStartVtx = NULL;
//	CGIS_NetworkVtx *pEndVtx = NULL;
//	GetRealVtx( pWPOne, pWPTwo, pMap, &pStartVtx, &pEndVtx );
//	CGIS_NetworkNode *pSNode = new CGIS_NetworkNode;
//	pSNode->m_pParent = NULL;
//	pSNode->m_pRut = GetNetworkRut( pWPOne->GetWPRutID(pMap->GetMapInfo()->GetMapType()) );
//	pSNode->m_pVtx = pStartVtx;
//	SOList.AddTail( pSNode );
//	CGIS_NetworkNode *pENode = new CGIS_NetworkNode;
//	pENode->m_pParent = NULL;
//	pENode->m_pRut = GetNetworkRut( pWPTwo->GetWPRutID(pMap->GetMapInfo()->GetMapType()) );
//	pENode->m_pVtx = pEndVtx;
//	EOList.AddTail( pENode );
//	
//	DWORD dwTime = GetTickCount();
//	BOOL bFind = FALSE;
//	BOOL bStart = FALSE;
//	BYTE nRLevel = min(pSNode->m_pRut->GetRutLevel(), pENode->m_pRut->GetRutLevel());
//	CGIS_NetworkVtx *pMVtx = NULL;
//	while( !SOList.IsEmpty() && !EOList.IsEmpty() )
//	{
//		pMVtx = (CGIS_NetworkVtx*)((CGIS_NetworkNode*)EOList.GetHead())->m_pVtx;
//		if( CalculateOptimalPathRut( &SOList, &SCList, pMVtx, pPathInfo, nRLevel ) )
//		{
//			bStart = TRUE;
//			bFind = TRUE;
//			break;
//		}
//		pMVtx = (CGIS_NetworkVtx*)((CGIS_NetworkNode*)SOList.GetHead())->m_pVtx;
//		if( CalculateOptimalPathRut( &EOList, &ECList, pMVtx, pPathInfo, nRLevel ) )
//		{
//			bStart = FALSE;
//			bFind = TRUE;
//			break;
//		}
////		nRLevel = min(((CGIS_NetworkNode*)EOList.GetHead())->m_pRut->GetRutLevel(), 
////						((CGIS_NetworkNode*)SOList.GetHead())->m_pRut->GetRutLevel());
//	}
//	///get the leg path data
//	CGIS_NetworkNode* pNode = NULL;
//	if( bFind )
//	{
//		dwTime = GetTickCount();
//		CPtrList RutList;
//		CPtrList VtxList;
//		pNode = (CGIS_NetworkNode*)SOList.GetHead();
//		while( pNode )
//		{
//			RutList.AddHead( pNode->m_pRut );
//			VtxList.AddHead( pNode->m_pVtx );
//			pNode = pNode->m_pParent;
//		}
//		pNode = (CGIS_NetworkNode*)EOList.GetHead();
//		while( pNode )
//		{
//			RutList.AddTail( pNode->m_pRut );
//			VtxList.AddTail( pNode->m_pVtx );
//			pNode = pNode->m_pParent;
//		}
//
//		RutList.RemoveHead( );
//		/////填充路径的数据
//		FullPathLegData( pWPOne, pWPTwo, &RutList, &VtxList, pMap, pPathPointList );
//	}
//	if( bStart )
//		EOList.RemoveHead();
//	else 
//		SOList.RemoveHead();
//	POSITION ps = SOList.GetHeadPosition();
//	while( ps )
//	{
//		pNode = (CGIS_NetworkNode*)SOList.GetNext( ps );
//		if( pNode )
//		{
//			pNode->m_pVtx->m_byStatus = 0;
//			delete pNode;
//		}
//	}
//	ps = SCList.GetHeadPosition();
//	while( ps )
//	{
//		pNode = (CGIS_NetworkNode*)SCList.GetNext( ps );
//		if( pNode )
//		{
//			pNode->m_pVtx->m_byStatus = 0;
//			delete pNode;
//		}
//	}
//	ps = EOList.GetHeadPosition();
//	while( ps )
//	{
//		pNode = (CGIS_NetworkNode*)EOList.GetNext( ps );
//		if( pNode )
//		{
//			pNode->m_pVtx->m_byStatus = 0;
//			delete pNode;
//		}
//	}
//	ps = ECList.GetHeadPosition();
//	while( ps )
//	{
//		pNode = (CGIS_NetworkNode*)ECList.GetNext( ps );
//		if( pNode )
//		{
//			pNode->m_pVtx->m_byStatus = 0;
//			delete pNode;
//		}
//	}
//	SOList.RemoveAll();
//	SCList.RemoveAll();
//	EOList.RemoveAll();
//	ECList.RemoveAll();
//
//	return TRUE;
//}
//
/////计算最优路段
//BOOL CGIS_MapNetwork::CalculateOptimalPathRut( CPtrList *pOList, CPtrList *pCList, CGIS_NetworkVtx *pEndVtx, CGIS_NetworkPathInfo *pPathInfo, BYTE nRLevel )
//{
//	ASSERT( pOList->GetCount() > 0 );
//	CGIS_NetworkNode *pParentNode = (CGIS_NetworkNode*)pOList->GetHead();
//	if( pParentNode->m_pVtx == pEndVtx )
//	{
//		return TRUE;
//	}
//	pOList->RemoveHead( );
//	pParentNode->m_pVtx->m_posOpen = NULL;
//	pParentNode->m_pVtx->m_byStatus = 2;
//	pCList->AddTail( pParentNode );
//
//	CGIS_NetworkRut *pRut;
//	CGIS_NetworkVtx *pVtx;
//	CGIS_NetworkNode *pChildNode, *pOldNode;
//
//	POSITION pos = NULL;
//	///得到前一路段的编号
//	BYTE *pRTList = pParentNode->m_pVtx->GetRutTurnList();
//	int nParent = 0;
//	if( pRTList )
//	{
//		for( int i=0;	i<pParentNode->m_pVtx->GetNodeRutNum();	i++ )
//		{
//			if( pParentNode->m_pVtx->GetNodeRuts()[i] == pParentNode->m_pRut->GetRutID() )
//			{
//				nParent++;
//				break;
//			}
//			nParent++;
//		}
//	}
//	////计算其余路段的优先次序
//	int nChild = 0;
//	for( int i=0;	i<pParentNode->m_pVtx->GetNodeRutNum();	i++ )
//	{
//		pRut = GetNetworkRut( pParentNode->m_pVtx->GetNodeRuts()[i] );
////		if( pRut->GetRutLevel()>nRLevel )
////			continue;
//		if( pRut == NULL )
//			ASSERT( FALSE );
//		nChild++;
////		if( pRut == pParentNode->m_pRut )
////			continue;
//		pVtx = GetNextRutVtx( pParentNode->m_pVtx, pRut );
//		if( pVtx == NULL )
//			continue;
//		if( pRTList )
//		{
//			if( pRTList[(nParent-1)*8+nChild-1] == '1' )
//				continue;
//		}
//		pChildNode = new CGIS_NetworkNode;
//		pChildNode->m_pParent = pParentNode;
//		pChildNode->m_pRut = pRut;
//		pChildNode->m_pVtx = pVtx;
//
//		if( !CalculateRutVal( pChildNode, pPathInfo, pEndVtx->GetVtxPosition() ) )
//		{
//			delete pChildNode;
//			continue;
//		}
//
//		if(pChildNode->m_pVtx->m_byStatus == 1)
//		{
//			//插入到OPEN中
//			pOldNode = (CGIS_NetworkNode*)pOList->GetAt(pChildNode->m_pVtx->m_posOpen);
//			if(pChildNode->m_g < pOldNode->m_g)
//			{
//				pOList->RemoveAt(pOldNode->m_pVtx->m_posOpen);
//				delete pOldNode;
//				InsertNode(pChildNode,pOList);
//			}else
//				delete pChildNode;
//
//		}
//		else	if(pChildNode->m_pVtx->m_byStatus == 0)
//		{
//			//插入到OPEN中
//			InsertNode(pChildNode,pOList);
//			pChildNode->m_pVtx->m_byStatus = 1;
//
//		}else
//			delete pChildNode;
//	}
//	return FALSE;
//}
BOOL CGIS_MapNetwork::CalculateNavigatePathLeg( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList )
{
	CPtrList *pOList = new CPtrList;
	CPtrList *pCList = new CPtrList;

	CGIS_NetworkVtx *pStartVtx = NULL;
	CGIS_NetworkVtx *pEndVtx = NULL;
	GetRealVtx( pWPOne, pWPTwo, pMap, &pStartVtx, &pEndVtx );
	CGIS_NetworkNode *pNode = new CGIS_NetworkNode;
	pNode->m_pParent = NULL;
	pNode->m_pRut = GetNetworkRut( pWPOne->GetWPRutID(pMap->GetMapInfo()->GetMapType()) );
	pNode->m_pVtx = pStartVtx;
	pOList->AddTail( pNode );
	
	BOOL bFind = FALSE;
	DWORD dwTime = GetTickCount();
	while( !pOList->IsEmpty() )
	{
		if( CalculateOptimalPathRut( pOList, pCList, pEndVtx, pPathInfo ) )
		{
			bFind = TRUE;
			break;
		}
	}
	TRACE( L"CalOptRut:%d\n", GetTickCount()-dwTime );

	///get the leg path data
	dwTime = GetTickCount();
	if( bFind )
	{
		dwTime = GetTickCount();
		CPtrList RutList;
		CPtrList VtxList;
		pNode = (CGIS_NetworkNode*)pOList->GetHead();
		while( pNode )
		{
			RutList.AddHead( pNode->m_pRut );
			VtxList.AddHead( pNode->m_pVtx );
			pNode = pNode->m_pParent;
		}

		RutList.RemoveHead( );
		/////填充路径的数据
		FullPathLegData( pWPOne, pWPTwo, &RutList, &VtxList, pMap, pPathPointList );
	}
	TRACE( L"FullOptRut:%d\n", GetTickCount()-dwTime );
	
	POSITION ps = pOList->GetHeadPosition();
	while( ps )
	{
		pNode = (CGIS_NetworkNode*)pOList->GetNext( ps );
		if( pNode )
		{
			pNode->m_pVtx->m_bySHead = pNode->m_pVtx->m_bySEnd = 0;
			delete pNode;
		}
	}
	ps = pCList->GetHeadPosition();
	while( ps )
	{
		pNode = (CGIS_NetworkNode*)pCList->GetNext( ps );
		if( pNode )
		{
			pNode->m_pVtx->m_bySHead = pNode->m_pVtx->m_bySEnd = 0;
			delete pNode;
		}
	}
	pOList->RemoveAll();
	pCList->RemoveAll();
	delete pOList;
	delete pCList;

	return TRUE;
}

///计算最优路段
BOOL CGIS_MapNetwork::CalculateOptimalPathRut( CPtrList *pOList, CPtrList *pCList, CGIS_NetworkVtx *pEndVtx, CGIS_NetworkPathInfo *pPathInfo )
{
	ASSERT( pOList->GetCount() > 0 );
	CGIS_NetworkNode *pParentNode = (CGIS_NetworkNode*)pOList->GetHead();
	if( pParentNode->m_pVtx == pEndVtx )
	{
		return TRUE;
	}
	pOList->RemoveHead( );
	pParentNode->m_pVtx->m_posOHead = pParentNode->m_pVtx->m_posOEnd = NULL;
	pParentNode->m_pVtx->m_bySHead = pParentNode->m_pVtx->m_bySEnd = 2;
	pCList->AddTail( pParentNode );

	CGIS_NetworkRut *pRut;
	CGIS_NetworkVtx *pVtx;
	CGIS_NetworkNode *pChildNode, *pOldNode;

	POSITION pos = NULL;
	///得到前一路段的编号
	BYTE *pRTList = pParentNode->m_pVtx->GetRutTurnList();
	int nParent = 0;
	if( pRTList )
	{
		for( int i=0;	i<pParentNode->m_pVtx->GetNodeRutNum();	i++ )
		{
			if( pParentNode->m_pVtx->GetNodeRuts()[i] == pParentNode->m_pRut->GetRutID() )
			{
				nParent++;
				break;
			}
			nParent++;
		}
	}
	////计算其余路段的优先次序
	int nChild = 0;
	for( int i=0;	i<pParentNode->m_pVtx->GetNodeRutNum();	i++ )
	{
		pRut = GetNetworkRut( pParentNode->m_pVtx->GetNodeRuts()[i] );
		if( pRut == NULL )
			ASSERT( FALSE );
		nChild++;
//		if( pRut == pParentNode->m_pRut )
//			continue;
		pVtx = GetNextRutVtx( pParentNode->m_pVtx, pRut );
		if( pVtx == NULL )
			continue;
		if( pRTList )
		{
			if( pRTList[(nParent-1)*8+nChild-1] == '1' )
				continue;
		}
		pChildNode = new CGIS_NetworkNode;
		pChildNode->m_pParent = pParentNode;
		pChildNode->m_pRut = pRut;
		pChildNode->m_pVtx = pVtx;

		if( !CalculateRutVal( pChildNode, pPathInfo, pEndVtx->GetVtxPosition(), 1, TRUE ) )
		{
			delete pChildNode;
			continue;
		}

		if(pChildNode->m_pVtx->m_bySHead == 1)
		{
			//插入到OPEN中
			pOldNode = (CGIS_NetworkNode*)pOList->GetAt(pChildNode->m_pVtx->m_posOHead);
			if(pChildNode->m_g < pOldNode->m_g)
			{
				pOList->RemoveAt(pOldNode->m_pVtx->m_posOHead);
				delete pOldNode;
				InsertNode(pChildNode,pOList);
			}else
				delete pChildNode;

		}
		else	if(pChildNode->m_pVtx->m_bySHead == 0)
		{
			//插入到OPEN中
			InsertNode(pChildNode,pOList);
			pChildNode->m_pVtx->m_bySHead = 1;

		}else
			delete pChildNode;
	}
	return FALSE;
}
