
//#include "stdafx.h"
#include "GIS_BreadthMap.h"

CGIS_BreadthMap::CGIS_BreadthMap( ){
	m_nBreadthID = 0;
	m_nBMID = 0;
	//m_nLoadCounter = 0;
	m_pMap = NULL;
}

CGIS_BreadthMap::CGIS_BreadthMap( IGIS_Map *pMap ){
	m_nBreadthID = 0;
	m_nBMID = 0;
	//m_nLoadCounter = 0;
	m_pMap = pMap;
}

CGIS_BreadthMap::~CGIS_BreadthMap(){
}

IGIS_Map *CGIS_BreadthMap::GetGeoMap( ){
	return m_pMap;
}

