
#ifndef _GIS_MAPNETWORK_H
#define _GIS_MAPNETWORK_H

#include "swBase.h"

#include "GIS_NetworkRoute.h"
#include "GIS_NetworkRut.h"
#include "GIS_NetworkVtx.h"
#include "IGIS_MapNetwork.h"
#include "GIS_GeoMap.h"
#include "GIS_WayPoint.h"
#include "GIS_NetworkNode.h"


class CGIS_MapNetwork : public IGIS_MapNetwork  
{
public:
	CGIS_MapNetwork();
	virtual ~CGIS_MapNetwork();

public:
	virtual BOOL LoadMapNetwork( );
	virtual BOOL UnLoadMapNetwork( );
	///cal the path 
	virtual BOOL CalculateNavigatePath( CPtrList* WayPointList,CPtrList* pPathpointlist );
	//��network�Ľڵ������в��ҽڵ�
	CGIS_NetworkVtx* GetVertex(int nID);
	//��network��·�������в���·��
	CGIS_NetworkRut* GetNetworkRut( int nID );
private:
//	//���㺽����������;����Ľڵ���·��
//	BOOL CalculateNavigatePathLeg( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
//	///������Ҫ��·��
//	BOOL CalculateOptimalPathRut( CPtrList *pOList, CPtrList *pCList, CGIS_NetworkVtx *pEndVtx, CGIS_NetworkPathInfo *pPathInfo, BYTE nRLevel );
	//���㺽����������;����Ľڵ���·��
	BOOL CalculateNavigatePathLeg( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
	///������Ҫ��·��
	BOOL CalculateOptimalPathRut( CPtrList *pOList, CPtrList *pCList, CGIS_NetworkVtx *pEndVtx, CGIS_NetworkPathInfo *pPathInfo );

	///�õ��¸��ڵ�
	CGIS_NetworkVtx* GetNextRutVtx( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut );

/////////////////////
	///���;���������
	BOOL FullWayPointData( CGIS_WayPoint *pWayPoint, CGIS_GeoMap *pMap );
	///���Ҫ����·����;����Ľڵ���Ϣ
	BOOL GetRealVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CGIS_NetworkVtx **pStartVtx, CGIS_NetworkVtx **pEndVtx );
	///�����ʼwaypoint·����Ϣ
	BOOL FullWPStartPath( CGIS_WayPoint *pWPOne, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, CGIS_GeoMap *pMap, BOOL &bRutSame, CPtrList *pPathPointList, POSITION &pos );
	///�����ֹwaypoint·����Ϣ
	BOOL FullWPEndPath( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, int nRutID, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///�����ֹwaypoint·����Ϣ
	BOOL FullWPEndPath( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///���·��pathleg����
	BOOL FullPathLegData( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CPtrList *pRutList, CPtrList *pVtxList, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
	///���·�ߵ���waypoint��ͬһ·�ε����
	BOOL FullWPOneAndWPTwoOnSameRut( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///���·�ߵ���waypoint��ͬһ�ڵ�����
	BOOL FullWPOneAndWPTwoOnSameVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );

	CGIS_NetworkRut **m_pNetRutList;
	CGIS_NetworkVtx **m_pNetVtxList;
	int m_nMRutNum;
	int m_nMVtxNum;
};

#endif 
