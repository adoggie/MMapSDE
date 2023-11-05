
#ifndef _GIS_LAYERATTRINFO_H
#define _GIS_LAYERATTRINFO_H

#include "swBase.h"

#include "GIS_Field.h"

class swEXPORT CGIS_LayerAttInfo  
{
public:
	CGIS_LayerAttInfo();
	virtual ~CGIS_LayerAttInfo();

	CGIS_FieldMap* GetGISFieldMap();
	void AddFieldInfo( CGIS_Field* pField );
private:
	//CGIS_FieldMap *m_pLAInfoMap;	//字段列表 
	CGIS_FieldMap m_pLAInfoMap;	//字段列表 
};

#endif 
