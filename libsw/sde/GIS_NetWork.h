
#ifndef _GIS_NETWORK_H
#define _GIS_NETWORK_H

#include "swBase.h"

#include "IGIS_NetWork.h"
#include "IGIS_MapNetwork.h"
#include "GIS_GeoMap.h"

class swEXPORT CGIS_NetWork : public IGIS_NetWork  
{
public:
	CGIS_NetWork();
	virtual ~CGIS_NetWork();

public:

    	virtual int CalculateOptimalRoute( BYTE byCType, CGIS_WayPoint *pWPoint = NULL );
	virtual int CalculateStop() ;
#ifndef _WIN32_WCE
public:
    virtual CGIS_NetworkRut* GetRoadSectionByBreadthIDAndRoadSectionID(int nBreadthID,int nRoadSectionID,int& nLayer);
    virtual CGIS_NetworkVtx* GetVertexByBreadthIDAndVertexID(int nBreadthID,int nRoadSectionID,int& nLayer);
    virtual CGIS_NetworkRoadSectionV7* GetTrunkRoadSectionByBreadthIDAndRoadSectionID(int nBreadthID,int nID);
    virtual CGIS_NetworkVertexV7* GetTrunkVertexByBreadthIDAndVertexID(CGIS_GeoMap* pMap,int nBreadthID,int nID);
#endif
//////////////////////////////////////////////////////////////////////////

private:
	int CalculateOptimalRouteActual( );
	int CalculateOptimalRouteV2( CGIS_WayPoint *pWPoint );
	CGIS_WayPoint *ClearNetworkRoute( );
	int CalculateOptimalRouteInMap(IGIS_Map *pMap,CPtrList* WPList,CPtrList* pDesPointList);
	///parameter: byWPType 1.表示为航线上的目的地 2.表示航点是在航线上找得一点建立的目标点
	CGIS_WayPoint *ConstructNearWaypoint( BYTE &byWPType );

	IGIS_MapNetwork* m_pMapNetwork;

//////////////////////////////////////////////////////////////////////////
protected:
    typedef CArray<double,double> DoubleArray;
    BOOL CheckCalculateCondition(CPtrList* pWapPontList);
    
    BOOL MemoryInitialByDist(CPtrList*,DWORD&,DWORD& );
    BOOL MemoryInitialBySize(DWORD ,DWORD& ,DWORD& );
    BOOL MemoryClear();
    BOOL GetMemoryLimitByDistance(DoubleArray* pArrayOfDist,DWORD& dwMinSize,DWORD& dwMaxSize);

//////////////////////////////////////////////////////////////////////////

    class CCalculateStatusLock
    {
    public:
        CCalculateStatusLock();
        ~CCalculateStatusLock();
    };

};

#endif 
