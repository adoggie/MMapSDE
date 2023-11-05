
#ifndef _IGIS_SDETOOL_H
#define _IGIS_SDETOOL_H

#include "swBase.h"

class swEXPORT IGIS_SDETool  
{
public:
	IGIS_SDETool(EnSDEToolType);
	virtual ~IGIS_SDETool();
public:
	EnSDEToolType GetSDEToolType( );

protected:
	EnSDEToolType m_enSDEToolType;
};

#endif 
