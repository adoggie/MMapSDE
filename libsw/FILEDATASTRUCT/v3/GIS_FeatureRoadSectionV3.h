// Gis_FeatureRutV3.h: interface for the CGIS_FeatureRoadSectionV3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATURERUTV3_H__0517035F_405F_4DE9_AA95_1CB8813B6772__INCLUDED_)
#define AFX_GIS_FEATURERUTV3_H__0517035F_405F_4DE9_AA95_1CB8813B6772__INCLUDED_

#include "../V2/GIS_FeatureRoadSectionV2.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGIS_FeatureRoadSectionV3  : public CGIS_FeatureRoadSectionV2
{
public:
	CGIS_FeatureRoadSectionV3();
	virtual ~CGIS_FeatureRoadSectionV3();
	BOOL Calcu32Data(int nBreadthID);

    void SetPtNum( int nPtNum)
    {
        m_stData.nPointCount = nPtNum;
    }

	void SetPtList(float* ptList)
    {
        m_pLoction = (ST_GEO_PTXY*)ptList;
    }


//////////////////////////////////////////////////////////////////////////
public:
	unsigned long m_n32ID;
    BOOL m_bValid;
};

#endif // !defined(AFX_GIS_FEATURERUTV3_H__0517035F_405F_4DE9_AA95_1CB8813B6772__INCLUDED_)
