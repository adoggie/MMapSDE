
#ifndef _IGIS_TOOL_H
#define _IGIS_TOOL_H

#include "swBase.h"

#include "GIS_Feature.h"
class CGIS_GeoMap;

class  swEXPORT IGIS_TOOL  
{
public:
	IGIS_TOOL();
	virtual ~IGIS_TOOL();
public:
	void SetMap(CGIS_GeoMap* pMap);
	EnTooType GetType();
protected:
	CGIS_GeoMap* m_pMap;
	EnTooType m_Type;
};

#endif 
