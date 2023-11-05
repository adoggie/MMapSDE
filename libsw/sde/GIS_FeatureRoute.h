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
	//��¡����
	virtual CGIS_Feature* Clone();
	//��ȡ��С
	virtual long GetFeatureSize();
	//�����ѵĶ��󽻳�ȥ????
	CGIS_NetworkRoute * GetNetworkRoute( );

//	void SetNetworkRoute( CGIS_NetworkRoute *pNWRoute );
private:
	CGIS_NetworkRoute *m_pNWRoute;	//
};

#endif 
