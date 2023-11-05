
#ifndef _GIS_NETWORK_ROUTE_H
#define _GIS_NETWORK_ROUTE_H

#include "swBase.h"

#include "IGIS_NetworkRoute.h"
#include "GIS_NetworkPathInfo.h"
#include "GIS_NetworkPath.h"
#include "GIS_WayPoint.h"

class swEXPORT CGIS_NetworkRoute : public IGIS_NetworkRoute  
{
public:
	CGIS_NetworkRoute();
	virtual ~CGIS_NetworkRoute();

public:
	virtual CGIS_NetworkPathInfo* GetNetworkPathInfo( );
	virtual void SetNetworkPathInfo( CGIS_NetworkPathInfo* pPathInfo );
	virtual CPtrList *GetWayPointList( );
	virtual CGIS_NetworkPath *GetNetworkPath( );

	UINT GetRouteID();
	void SetRouteID( UINT nID );
	swString GetRouteName();
	void SetRouteName( swString strRName );
	CGIS_WayPoint *GetWayPointByID( UINT nID );
	UINT GetWPMaxID( );
	void SetNWRouteState( BOOL bNWRState );
	BOOL GetNWRouteState( );
protected:
	UINT m_nRouteID;
	swString m_strRName;
	CGIS_NetworkPathInfo* m_pPathInfo;	//路径规划信息
	CPtrList *m_pWayPtList;		///途经点链表
	CGIS_NetworkPath *m_pNetworkPath;
	BOOL m_bNWRState;		//表示航线的状态（true被修改 ）
};

#endif 
