#ifndef _IGIS_MAPNETWORK_H
#define _IGIS_MAPNETWORK_H

#include "swBase.h"
#include "GIS_GeoMap.h"
#include "GIS_WayPoint.h"
#include "GIS_NetworkRoute.h"
#include "GIS_NetworkNode.h"

class IMemoryMng;
//////////////////////////////////////////////////////////////////////////
class IGIS_MapNetwork  
{
public:
	IGIS_MapNetwork();
	virtual ~IGIS_MapNetwork();

public:
	virtual BOOL LoadMapNetwork( ) = 0;
//	virtual BOOL LoadMapNetwork( swString strFName ) = 0;
	void SetNetworkRoute( CGIS_NetworkRoute *pNetworkRoute );
	CGIS_NetworkRoute* GetNetworkRoute( );
	void SetGeoMap( CGIS_GeoMap *pMap );
	virtual int CalculateNavigatePath(CPtrList* WayPointList ,CPtrList* pPathpointlist ) = 0;

protected:
	BOOL CalculateNetworkNode( CGIS_NetworkNode *pNode, CPtrList *pOList, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead );
	//得到该节点上当前路段的权值
	inline BOOL CalculateRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead );
	//计算车辆类型为小汽车时该节点上当前路段的权值
	inline BOOL CalculateCarRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead );
	//计算车辆类型为大卡车时该节点上当前路段的权值
	inline BOOL CalculateTruckRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//计算车辆类型为公交车时该节点上当前路段的权值
	inline BOOL CalculateBusRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//计算车辆类型为救护车/消防车时该节点上当前路段的权值
	inline BOOL CalculateEmergencyRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//计算车辆类型为出租车时该节点上当前路段的权值
	inline BOOL CalculateTaxiRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//计算车辆类型为邮政车时该节点上当前路段的权值
	inline BOOL CalculateDeliveryRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//计算车辆类型为自行车时该节点上当前路段的权值
	inline BOOL CalculateBicycleRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	//计算车辆类型为步行时该节点上当前路段的权值
	BOOL CalculatePedestrianRutVal( CGIS_NetworkNode *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	///向链表中插入节点
	inline void InsertNode(CGIS_NetworkNode* pNode, CPtrList *pList, BOOL bHead = TRUE);


protected:
	CGIS_GeoMap *m_pMap;	//map
	CGIS_NetworkRoute *m_pNetworkRoute;

	double m_dbMaxDis;	//起点终点间的最大距离

	ST_GEO_PTXY m_pRBuf[128];
	int			m_nRBNum;
    
public:
    //计算路线的场景
    enum enCalculateScene
    {
        SCENE_NONE,//无效场景
        SCENE_SHORT,//很近
        SCENE_NORMAL,//通常
        SCENE_FAR,//很远
    };
protected:
	enCalculateScene m_eCalculateScene;	

public:
	static IMemoryMng* m_pMemMngOfBuddyData;

//////////////////////////////////////////////////////////////////////////
	
public:
	int CalculateStop();//停止计算
protected:
	LONG m_bCalculateStopSign;

//////////////////////////////////////////////////////////////////////////

};

#endif 
