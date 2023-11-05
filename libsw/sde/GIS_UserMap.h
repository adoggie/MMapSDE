
#ifndef _GIS_USERMAP_H
#define _GIS_USERMAP_H

#include "swBase.h"
#include "IGIS_Map.h"
#include "GIS_UserLayer.h"

class swEXPORT CGIS_UserMap : public IGIS_Map  
{
public:
	CGIS_UserMap();
	virtual ~CGIS_UserMap();

public:
	virtual BOOL Draw(IGIS_MapView* pMapView);
	virtual BOOL Attach(IGIS_MapView* pMapView);
	virtual BOOL Detache(IGIS_MapView* pMapView);
	virtual BOOL Notify(IGIS_Message* pMessage);
	virtual CGIS_LayerInfo* GetLayerInfo(UINT ILayerID);
//	virtual CGIS_MapInfo* GetMapInfo();
	virtual IGIS_TOOL* GetTool(EnTooType enToolType); 
	virtual BOOL ReleaseTool();
	virtual BOOL SaveToFile();
	virtual EnMatchRoad MatchRoad(ST_GEO_PTXY pt,ST_GEO_PTXY& Rpt,EnMathState enState,CGIS_FeatureRut** pLastRoat,int Routedis,BOOL bHighWay ,ST_GEO_PTXY ptMatchLast,int& nLeaveCount);
	
	CGIS_UserLayer *GetUserLayer( EnUserMapLayerType enUMLType );
public:
	BOOL OpenUserMap( swString strFName );
	BOOL InitUserMap( );
	CPtrList *GetLayerManagerList();
	BOOL SaveUserMap( swString strFName );
private:

	CPtrList*  m_pMapViewList;
	CPtrList* m_pLayManaList;
};

#endif 
