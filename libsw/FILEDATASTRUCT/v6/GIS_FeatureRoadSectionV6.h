// GIS_FeatureRoadSectionV6.h: interface for the CGIS_FeatureRoadSectionV6 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATUREROADSECTIONV6_H__8C2612DA_9D1B_47E7_9650_1DEB918795E4__INCLUDED_)
#define AFX_GIS_FEATUREROADSECTIONV6_H__8C2612DA_9D1B_47E7_9650_1DEB918795E4__INCLUDED_

#include "../V5/GIS_FeatureRoadSectionV5.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGIS_FeatureRoadSectionV6  : public CGIS_FeatureRoadSectionV5
{
public:
	CGIS_FeatureRoadSectionV6();
	virtual ~CGIS_FeatureRoadSectionV6();
public:
    BOOL m_bValid;
};

#endif // !defined(AFX_GIS_FEATUREROADSECTIONV6_H__8C2612DA_9D1B_47E7_9650_1DEB918795E4__INCLUDED_)
