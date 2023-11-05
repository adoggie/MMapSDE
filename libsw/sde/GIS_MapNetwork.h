
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
	//从network的节点链表中查找节点
	CGIS_NetworkVtx* GetVertex(int nID);
	//从network的路段链表中查找路段
	CGIS_NetworkRut* GetNetworkRut( int nID );
private:
//	//计算航线中任意两途经点的节点间的路线
//	BOOL CalculateNavigatePathLeg( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
//	///计算需要的路段
//	BOOL CalculateOptimalPathRut( CPtrList *pOList, CPtrList *pCList, CGIS_NetworkVtx *pEndVtx, CGIS_NetworkPathInfo *pPathInfo, BYTE nRLevel );
	//计算航线中任意两途经点的节点间的路线
	BOOL CalculateNavigatePathLeg( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
	///计算需要的路段
	BOOL CalculateOptimalPathRut( CPtrList *pOList, CPtrList *pCList, CGIS_NetworkVtx *pEndVtx, CGIS_NetworkPathInfo *pPathInfo );

	///得到下个节点
	CGIS_NetworkVtx* GetNextRutVtx( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut );

/////////////////////
	///填充途经点的数据
	BOOL FullWayPointData( CGIS_WayPoint *pWayPoint, CGIS_GeoMap *pMap );
	///获得要计算路径的途径点的节点信息
	BOOL GetRealVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CGIS_NetworkVtx **pStartVtx, CGIS_NetworkVtx **pEndVtx );
	///填充起始waypoint路段信息
	BOOL FullWPStartPath( CGIS_WayPoint *pWPOne, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, CGIS_GeoMap *pMap, BOOL &bRutSame, CPtrList *pPathPointList, POSITION &pos );
	///填充终止waypoint路段信息
	BOOL FullWPEndPath( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, int nRutID, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///填充终止waypoint路段信息
	BOOL FullWPEndPath( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///填充路线pathleg数据
	BOOL FullPathLegData( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CPtrList *pRutList, CPtrList *pVtxList, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
	///填充路线当两waypoint在同一路段的情况
	BOOL FullWPOneAndWPTwoOnSameRut( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///填充路线当两waypoint在同一节点的情况
	BOOL FullWPOneAndWPTwoOnSameVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );

	CGIS_NetworkRut **m_pNetRutList;
	CGIS_NetworkVtx **m_pNetVtxList;
	int m_nMRutNum;
	int m_nMVtxNum;
};

#endif 
