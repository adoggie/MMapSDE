
#ifndef _GIS_LAYERMANAGER_H
#define _GIS_LAYERMANAGER_H

#include "swBase.h"
#include "IGIS_Layer.h"
#include "GIS_LayerAttInfo.h"

class CGIS_LayerManager  {
public:
	CGIS_LayerManager();
	virtual ~CGIS_LayerManager();
public:
	BOOL CreateUserLayer( CGIS_LayerInfo *pInfo, CGIS_LayerAttInfo *pLAttInfo );
	BOOL DeleteUserLayer( UINT nLayerID );
	BOOL UpdateUserLayer( UINT nLID );
	BOOL SaveUserLayer( );
	IGIS_Layer* GetLayer( UINT nLayerID );
	/*
	IGIS_Layer* GetFirstLayer( );
	IGIS_Layer* GetNextLayer( );
	IGIS_Layer* GetLastLayer( );
	*/
	//void DrawLayer( IGIS_MapView *pView, CRect rtPolt, ST_GEO_RECT rtDraw, double dbScale );
	//void DrawLayer( IGIS_LayerFile *pLFile, IGIS_MapView *pView, CRect rtPolt, ST_GEO_RECT rtDraw, double dbScale );

	//CGIS_LayerMap *GetLayerListMap();
	CGIS_LayerMap& GetLayerListMap();
private:
	//CGIS_LayerMap *m_pLayListMap;
	CGIS_LayerMap _layerList;

};

#endif 
