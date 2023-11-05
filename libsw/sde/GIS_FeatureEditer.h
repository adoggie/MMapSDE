
#ifndef _GIS_FEATURE_EDITOR_H
#define _GIS_FEATURE_EDITOR_H

#include "swBase.h"
#include "IGIS_Editer.h"
#include "IGIS_Layer.h"

class swEXPORT CGIS_FeatureEditer : public IGIS_Editer{
public:
	CGIS_FeatureEditer();
	virtual ~CGIS_FeatureEditer();
	
	virtual BOOL CreateFeature( CGIS_Feature *pFeature );
	virtual BOOL UpdateFeature( CGIS_Feature *pFeature );
	virtual BOOL DeleteFeature( CGIS_Feature *pFeature );
public:
	void SetLayer( IGIS_Layer *pLayer );
private:
	IGIS_Layer *m_pLayer;
};

#endif 
