
//#include "stdafx.h"
#include "IGIS_Map.h"


IGIS_Map::IGIS_Map(){
}

IGIS_Map::~IGIS_Map(){
}

CGIS_MapInfo* IGIS_Map::GetMapInfo()
{
	return &m_pMapInfo;
}
