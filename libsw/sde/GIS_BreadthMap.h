
#ifndef _GIS_BREADTHMAP_H
#define _GIS_BREADTHMAP_H

#include "IGIS_Map.h"
#include "GeoGlobe.h"

class CGIS_BreadthMap  {
public:
	CGIS_BreadthMap( );
	CGIS_BreadthMap( IGIS_Map *pMap );
	~CGIS_BreadthMap();	
	IGIS_Map *GetGeoMap( );
public:
	int m_nBMID;
	int	m_nBreadthID;			// 0 开始的索引编号
	//ST_GEO_RECT m_rtBreadth;
	//unsigned char	m_nLoadCounter;
private:
	IGIS_Map *m_pMap;
};

#endif 
