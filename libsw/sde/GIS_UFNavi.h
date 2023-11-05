
#ifndef _GIS_UFNAVI_H
#define _GIS_UFNAVI_H

#include "swBase.h"

#include "IGIS_GeoFile.h"
#include "GIS_UserMap.h"
#include "GIS_UserLayer.h"
class CGIS_UFNavi //: public IGIS_GeoFile  
{
public:
//	CGIS_UFNavi();
	virtual ~CGIS_UFNavi();

public:
	static CGIS_UFNavi* GetUFNavi( );
//	virtual BOOL SaveToFile( swString bstrFName ) = 0;

//	void SetUFNaviPath( swString strFPath );
	BOOL LoadNaviMap( CGIS_UserMap* pMap, swString strFName );
	BOOL SaveNaviMap( CGIS_UserMap* pMap, swString strFName );
	BOOL LoadNaviLayer( CGIS_UserLayer *pLayer, swString strFPath );
	BOOL SaveNaviLayer( CGIS_UserLayer *pLayer, UINT nID, swString strFPath );

	CGIS_Feature* LoadNaviFeatureData( CGIS_UserLayer *pLayer, UINT nID, swString strFPath );
protected:
	CGIS_Feature* LoadNaviFeatureRouteData( CGIS_UserLayer *pLayer, UINT nID, swString strFPath );
	CGIS_Feature* LoadNaviFeatureTrackData( CGIS_UserLayer *pLayer, UINT nID, swString strFPath );
	BOOL LoadNaviRouteLayer( CGIS_UserLayer *pLayer, swString strFPath );
	BOOL SaveNaviRouteLayer( CGIS_UserLayer *pLayer, UINT nID, swString strFPath );
	BOOL LoadNaviTrackLayer( CGIS_UserLayer *pLayer, swString strFPath );
	BOOL SaveNaviTrackLayer( CGIS_UserLayer *pLayer, UINT nID, swString strFPath );
	static CGIS_UFNavi *m_pFNavi;
};

#endif
