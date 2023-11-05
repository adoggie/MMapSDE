
#ifndef _GIS_MESSAGE_VALUE_H
#define _GIS_MESSAGE_VALUE_H

#include "swBase.h"
class swEXPORT CGIS_MessageValue  
{
public:
	CGIS_MessageValue();
	virtual ~CGIS_MessageValue();
public:
	swString m_bszName;
	void* m_pDefVal;
	UINT m_nType;
	void* m_pValue;
};

#endif 
