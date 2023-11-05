
//#include "stdafx.h"
#include "IGIS_TOOL.h"
#include "GIS_GeoMap.h"


IGIS_TOOL::IGIS_TOOL()
{
	m_pMap = NULL;
}

IGIS_TOOL::~IGIS_TOOL()
{

}
void IGIS_TOOL::SetMap(CGIS_GeoMap* pMap)
{
	if(pMap != NULL)
	{
		m_pMap = pMap;
	}
}
EnTooType IGIS_TOOL::GetType()
{
	return m_Type;
}
