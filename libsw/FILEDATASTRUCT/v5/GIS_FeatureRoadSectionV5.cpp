// GIS_FeatureRutV5.cpp: implementation of the CGIS_FeatureRoadSectionV5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureRoadSectionV5.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureRoadSectionV5::CGIS_FeatureRoadSectionV5()
: CGIS_FeatureV2( EN_GOTYPE_ROADSECTION )
{
    memset(&m_stData,0,sizeof(m_stData));
    m_pAnno = NULL;
    m_pLoction = NULL;
}

CGIS_FeatureRoadSectionV5::~CGIS_FeatureRoadSectionV5()
{

}

void* CGIS_FeatureRoadSectionV5::Clone()
{
    ASSERT(FALSE);
    return NULL;
}



//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_FeatureRoadSectionV5::DumpState() const	
{  
}

void CGIS_FeatureRoadSectionV5::AssertValid() const	
{   
    ASSERT(this);
    ASSERT(m_stData.nAnnoNum    <= 70);
    ASSERT((m_stData.nDirection&0x7f)  <= 3);

    ASSERT((m_stData.nID>>15)    < 100000);
    ASSERT((m_stData.nID&0X7FFF) < 20000);
    ASSERT(m_stData.nPointCount < 12000);

    if(m_stData.nAnnoNum)
    {
        ASSERT(m_pAnno);
    }

}
#endif
//////////////////////////////////////////////////////////////////////////
