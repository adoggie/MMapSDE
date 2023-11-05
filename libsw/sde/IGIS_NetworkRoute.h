#ifndef _IGIS_NETWORKROUTE_H
#define _IGIS_NETWORKROUTE_H

#include "swBase.h"
#include "GIS_NetworkPathInfo.h"
#include "GIS_NetworkPath.h"

class swEXPORT IGIS_NetworkRoute  
{
public:
	IGIS_NetworkRoute();
	virtual ~IGIS_NetworkRoute();

public:
	virtual CGIS_NetworkPathInfo* GetNetworkPathInfo( ) = 0;
	virtual void SetNetworkPathInfo( CGIS_NetworkPathInfo* pPathInfo ) = 0;
	virtual CPtrList *GetWayPointList( ) = 0;
	virtual CGIS_NetworkPath *GetNetworkPath( ) = 0;
};

#endif 
