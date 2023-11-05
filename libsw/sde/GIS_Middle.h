#ifndef _GIS_MIDDLE_H
#define _GIS_MIDDLE_H

#include "swBase.h"
#include "GIS_UserLayer.h"
#include "IArchive.h"

class CGIS_Middle  
{
public:
	CGIS_Middle();
	virtual ~CGIS_Middle();

public:
	BOOL			DeleteAllFeature(CGIS_UserLayer * pLayer, swString strFPath);
	
	DWORD		  GetFeatureSize( swString strFPath,  UINT nID );
	CGIS_Feature* LoadCurrentRoute( swString strFPath );
	BOOL		   SaveCurrentRoute( CGIS_Feature* pCurrentRoute,swString strFPath);
	BOOL		   DeleteFeature(CGIS_UserLayer * pLayer, swString strFPath, UINT nID);
	BOOL		   SaveFeature(CGIS_UserLayer * pUserLayer,CGIS_Feature * pFeature, swString strFPath, UINT nID);
	CGIS_Feature * LoadFeature(CGIS_UserLayer *pLayer, swString strFPath, UINT nID );
	
	// 根据完整路名加载航线
	CGIS_Feature* LoadRoute(swString strFullPath);
	// 保存整条航线到完整路径
	BOOL SaveRoute(CGIS_Feature *pRoute, swString strFPath);

protected:
	BOOL		   DeleteAllFeatureAddress(CGIS_UserLayer * pLayer, swString strFPath);
	BOOL		   DeleteAllFeatureTrack(CGIS_UserLayer * pLayer, swString strFPath);
	BOOL		   DeleteAllFeatureRoute(CGIS_UserLayer * pLayer, swString strFPath);
	BOOL		   DeleteFeatureTrack(CGIS_UserLayer * pLayer, swString strFPath, UINT nID);
	BOOL		   DeleteFeatureAddress(CGIS_UserLayer * pLayer, swString strFPath, UINT nID);
	BOOL		   DeleteFeatureRoute(CGIS_UserLayer * pLayer, swString strFPath, UINT nID);
	CGIS_Feature * LoadFeatureAddress( CGIS_UserLayer * pLayer, swString strFPath, UINT nID );
	CGIS_Feature * LoadFeatureRoute( CGIS_UserLayer * pLayer, swString strFPath, UINT nID );
	CGIS_Feature * LoadFeatureTrack(CGIS_UserLayer *pLayer, swString strFPath, UINT nID);
	BOOL		   SaveFeatureAddress(CGIS_UserLayer * pUserLayer, swString strFPath, UINT nID);
	BOOL		   SaveFeatureRoute(CGIS_UserLayer * pUserLayer,CGIS_Feature * pFeature, swString strFPath, UINT nID);
	BOOL		   SaveFeatureTrack(CGIS_UserLayer * pUserLayer,CGIS_Feature * pFeature, swString strFPath, UINT nID);
private:
	IArchive	*  m_pArchive;

	BOOL	m_bIsFeatureExistance;
};

#endif 
