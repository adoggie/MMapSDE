// Gis_FeatureRutV3.cpp: implementation of the CGIS_FeatureRoadSectionV3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureRoadSectionV3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureRoadSectionV3::CGIS_FeatureRoadSectionV3()
{
    m_bValid = TRUE;
}


CGIS_FeatureRoadSectionV3::~CGIS_FeatureRoadSectionV3()
{

}

//////////////////////////////////////////////////////////////////////////
BOOL CGIS_FeatureRoadSectionV3::Calcu32Data(int nBreadthID)
{
	m_n32ID = (nBreadthID<<15) + m_stData.nID;
   
	return TRUE;
}
