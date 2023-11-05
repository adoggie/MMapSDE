
#ifndef _GIS_USERLAYER_H
#define _GIS_USERLAYER_H

#include "swBase.h"

#include "IGIS_Layer.h"
#include "GIS_LayerAttInfo.h"
#include "GIS_Feature.h"

class CGIS_Middle;

class swEXPORT CGIS_UserLayer : public IGIS_Layer  
{
public:
	CGIS_UserLayer(CGIS_LayerInfo *pInfo, CGIS_LayerAttInfo *pLAttInfo );
	CGIS_UserLayer(CGIS_LayerInfo *pInfo );
	virtual ~CGIS_UserLayer();
	
public:
	BOOL DeleteAllFeature(swString strFPath);
	DWORD GetTotalFeatrueSize( swString strPath, long & nNum );
	CGIS_LayerAttInfo* GetLayerAttInfo( );
	CGIS_Feature *CreateFeature( );
//	CGIS_Feature *CreateFeature( EnGeoObjType enGOType );
	BOOL DeleteFeature( /*CGIS_Feature *pFeature*/UINT nID, swString strFPath );
	BOOL UpdateFeature( CGIS_Feature *pFeature, swString strFPath, int nFlag = 0 );

	virtual void DrawLayer( IGIS_LayerFile *pLFile, IGIS_MapView *pView, CRect rtPolt, ST_GEO_RECT rtDraw, double dbScale );
	void DrawLayer( IGIS_MapView *pView, CRect rtPolt, ST_GEO_RECT rtDraw, double dbScale );

	void SetUserMapLayerType( EnUserMapLayerType enUMLType );
	EnUserMapLayerType GetUserMapLayerType( );

	BOOL LoadLayer( swString strFPath );
	CGIS_Feature* LoadFeatureObject( UINT nID, swString strFPath, int nFlag = 0 );
	UINT GetFeatureMaxID();
	
	// 根据完整的路径加载航线
	CGIS_Feature* LoadFeatureObject(swString strFullPath);
	// 将航线保存到完整路径
	BOOL SaveFeatureObject(CGIS_Feature *pFeature, swString strFPath);

private:

	BOOL EstimateDrawCondition( double dbScale, double dbMinGRScale, double dbMaxGRScale );
	CGIS_LayerAttInfo *m_pLAttInfo;
	EnUserMapLayerType m_enUMLType;

	CGIS_Middle * m_pMiddle;
};

#endif 























