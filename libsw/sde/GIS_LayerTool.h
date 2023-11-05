
#ifndef _GIS_LAYERTOOL_H
#define _GIS_LAYERTOOL_H

#include "swBase.h"
#include "IGIS_LayerTool.h"
#include "GIS_LayerAttInfo.h"
#include "GIS_LayerInfo.h"

class swEXPORT CGIS_LayerTool : public IGIS_LayerTool  {
public:
	CGIS_LayerTool();
	virtual ~CGIS_LayerTool();
public:
	virtual BOOL CreateUserLayer( CGIS_LayerInfo *pLInfo, CGIS_LayerAttInfo *pAttInfo );
	virtual BOOL DeleteUserLayer( UINT nLID );
	virtual BOOL UpdateUserLayer( UINT nLID );
};

#endif 
