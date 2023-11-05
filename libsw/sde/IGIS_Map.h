#ifndef _IGIS_MAP_H
#define _IGIS_MAP_H

#include "swBase.h"

//#include "IGIS_MapView.h"
//#include "GIS_LayerInfo.h"
#include "GIS_MapInfo.h"
//#include "IGIS_Message.h"
//#include "IGIS_TOOL.h"
//#include "GIS_FeatureRut.h"

class IGIS_TOOL;
class IGIS_MapView;
class IGIS_Message;
class CGIS_LayerInfo;
class swEXPORT IGIS_Map  
{
public:
	IGIS_Map();
	virtual ~IGIS_Map();
public:
	virtual BOOL Draw(IGIS_MapView* pMapView) = 0;
	virtual BOOL Attach(IGIS_MapView* pMapView) = 0;
	virtual BOOL Detache(IGIS_MapView* pMapView) = 0;
	virtual BOOL Notify(IGIS_Message* pMessage) = 0;
	virtual CGIS_LayerInfo* GetLayerInfo(UINT ILayerID) = 0;
	CGIS_MapInfo* GetMapInfo();
	virtual IGIS_TOOL* GetTool(EnTooType enToolType) = 0; 
	virtual BOOL ReleaseTool() = 0 ;
	virtual BOOL SaveToFile() = 0;
	virtual EnMatchRoad MatchRoad(ST_GEO_PTXY pt,ST_GEO_PTXY& Rpt,EnMathState enState,CGIS_FeatureRut** pLastRoat,int Routedis,BOOL bHighWay,ST_GEO_PTXY ptMatchLast ,int& nLeaveCount) = 0;
	
protected:
	//CGIS_MapInfo *m_pMapInfo;
	CGIS_MapInfo m_pMapInfo;

};

typedef std::list< shared_ptr<IGIS_Map> > IGisMapPtrListT;


#endif 
