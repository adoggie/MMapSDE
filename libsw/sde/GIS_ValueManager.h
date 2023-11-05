
#ifndef _GIS_VALUE_MANAGER_H
#define _GIS_VALUE_MANAGER_H

#include "swBase.h"

#include "GIS_MessageValue.h"

class swEXPORT CGIS_ValueManager  
{
public:
	CGIS_ValueManager();
	virtual ~CGIS_ValueManager();
public:
	//CPtrList* m_pValueSet;
	std::vector< CGIS_MessageValue*> m_pValueSet;
public:
	BOOL LoadAValue(CGIS_MessageValue* pMessageValue);
	BOOL UnLoadAValue(UINT ValueID);
};

#endif 
