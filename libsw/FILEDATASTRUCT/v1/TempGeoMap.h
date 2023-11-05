// TempGeoMap.h: interface for the CTempGeoMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPGEOMAP_H__AE52128D_440A_472D_8E8D_33F33F1873F1__INCLUDED_)
#define AFX_TEMPGEOMAP_H__AE52128D_440A_472D_8E8D_33F33F1873F1__INCLUDED_

#include "TempMapLayer.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTempMapInfo;
class CGIS_BreadthMap;
class CTempLayerManager;

#include "GeoGlobe.h"
//////////////////////////////////////////////////////////////////////////

class CTempGeoMap  
{
public:
	CGIS_BreadthMap* GetBreadthMapByID2( int nBID );
	CTempMapInfo* GetMapInfo();
	CTempGeoMap();
	virtual ~CTempGeoMap();

	void SetBreadthMapList( CGIS_BreadthMap* pBMapList );
	int GetBreadthMapNum();
	void SetBreadthMapNum( int nBMapNum );
	void SetBreadthMapRects( ST_GEO_RECT *pRTList = NULL, int nBNum = 0 );

protected:
	int m_nBMapNum;

	CTempMapInfo*		m_pMapInfo;
	ST_GEO_RECT*		m_pRTList;	
	CGIS_BreadthMap*	m_pBMapList;
public:
	CTempLayerManager* m_pLayerMng;
};

#endif // !defined(AFX_TEMPGEOMAP_H__AE52128D_440A_472D_8E8D_33F33F1873F1__INCLUDED_)
