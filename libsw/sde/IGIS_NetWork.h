
#ifndef _IGIS_NETWORK_H
#define _IGIS_NETWORK_H

#include "swBase.h"

#include "IGIS_SDETool.h"
#include "GIS_NetworkRoute.h"
#include "IGIS_MapManager.h"


#ifdef _DEBUG
#include "GeoView.h"
#endif

#ifdef _DEBUG
#ifdef ANALYSE_DATA
#undef ANALYSE_DATA
#endif
#define ANALYSE_DATA
#endif

class IMemoryMng;
class CGIS_NetworkRut;
class CGIS_NetworkVtx;
class CGIS_NetworkRoadSectionV7;
class CGIS_NetworkVertexV7;


class swEXPORT IGIS_NetWork  : public IGIS_SDETool
{
public:
	IGIS_NetWork();
	virtual ~IGIS_NetWork();

public:
    /**
     * @brief 路线规划
     *
     * @param byCType-(0.正常的路线规划 1.偏航时的路线规划 2.绕路时路线规划),
     *       对于byCType为1.2两种情况需要传入车辆的起点
     *              
     * @return  1.计算成功
     *          0.路网数据已卸载     
     *         -1.附近没有匹配的路段         
     *         -2.路网不通     
     *         -3.文件I/O异常     
     *         -4.内存不足
     *         -5.航线过长,放弃计算.
     *         -6.计算条件不足
     *         -7.计算被外部终止
    */	
	virtual int CalculateOptimalRoute( BYTE byCType, CGIS_WayPoint *pWPoint = NULL ) = 0;
	virtual int CalculateStop() = 0;
//////////////////////////////////////////////////////////////////////////
    //2007-04-14 根据鹿培信的建议.加入此接口用于pc端的测试.
#ifndef _WIN32_WCE
public:
    virtual CGIS_NetworkRut* GetRoadSectionByBreadthIDAndRoadSectionID(int nBreadthID,int nRoadSectionID,int& nLayer) = 0;
    virtual CGIS_NetworkVtx* GetVertexByBreadthIDAndVertexID(int nBreadthID,int nRoadSectionID,int& nLayer) = 0;
    virtual CGIS_NetworkRoadSectionV7* GetTrunkRoadSectionByBreadthIDAndRoadSectionID(int nBreadthID,int nID) = 0;
    virtual CGIS_NetworkVertexV7* GetTrunkVertexByBreadthIDAndVertexID(CGIS_GeoMap* pMap,int nBreadthID,int nID) = 0;
#endif
public:
	void SetNetworkRoute( CGIS_NetworkRoute *pRoute );
	CGIS_NetworkRoute* GetNetworkRoute( );
	void SetMapManager( IGIS_MapManager *pMapManager );
protected:
	CGIS_NetworkRoute *m_pNetworkRoute;
	IGIS_MapManager *m_pMapManager;
public:
	static IMemoryMng* m_pMemMngOfBuddyData;
#ifdef _DEBUG
public:
	CGeoView* m_pMapView;
	//由于模块状态的关系,应用不可以调用此接口,只能由sde内部使用
	void SetMapViewSrc(CGeoView* pView)
	{
		m_pMapView = pView;
	}
	CGeoView* GetMapView()
	{
		 return m_pMapView;
	}
	
#ifdef ANALYSE_DATA
    DWORD m_dwMaxHeapUsedSize;
    DWORD GetMaxHeapUsedSize(){return m_dwMaxHeapUsedSize;};
#endif
    
#endif  
//////////////////////////////////////////////////////////////////////////

};

#endif 
