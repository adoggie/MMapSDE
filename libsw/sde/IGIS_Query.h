#ifndef _IGIS_QUERY_H
#define _IGIS_QUERY_H

#include "swBase.h"

#include "IGIS_TOOL.h"
#include "GIS_Feature.h"
#include "GIS_FeatureLine.h"
#include "GIS_FeaturePoint.h"
#include "GIS_FeaturePoly.h"
#include "GIS_FeatureRut.h"
#include "Globe.h"
#include "GeoGlobe.h"
class  swEXPORT IGIS_Query : public IGIS_TOOL  
{
public:
	IGIS_Query();
	virtual ~IGIS_Query();
};

#endif 
