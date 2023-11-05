#ifndef _GIS_FEATUREROUTE_H
#define _GIS_FEATUREROUTE_H

#include "swBase.h"

#include "GIS_Feature.h"
#include "GIS_NetworkRoute.h"

class swEXPORT CGIS_FeatureRoute : public CGIS_Feature  
{
public:
	CGIS_FeatureRoute( );
	CGIS_FeatureRoute( UINT nID );
	virtual ~CGIS_FeatureRoute();	
public:
	//克隆自身
	virtual CGIS_Feature* Clone();
	//获取大小
	virtual long GetFeatureSize();
	//把自已的对象交出去????
	CGIS_NetworkRoute * GetNetworkRoute( );

//	void SetNetworkRoute( CGIS_NetworkRoute *pNWRoute );
private:
	CGIS_NetworkRoute *m_pNWRoute;	//
};

#endif 
