// GIS_FeatureRutV2.cpp: implementation of the CGIS_FeatureRutV2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureRoadSectionV2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureRoadSectionV2::CGIS_FeatureRoadSectionV2()
: CGIS_FeatureV2( EN_GOTYPE_ROADSECTION )
{
    memset(&m_stData,0,sizeof(m_stData));
    m_pAnno = NULL;
    m_pLoction = NULL;
}

CGIS_FeatureRoadSectionV2::~CGIS_FeatureRoadSectionV2()
{

}
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_FeatureRoadSectionV2::DumpState() const	
{  
}

void CGIS_FeatureRoadSectionV2::AssertValid() const	
{   
    ASSERT(this);
    ASSERT(m_stData.nAnnoNum    <= 70);
    ASSERT((m_stData.nDirection&0x7f)  <= 3);
    ASSERT(m_stData.nID         < 20000 || m_stData.nID == 65535);
    ASSERT(m_stData.nPointCount < 1000);

    if(m_stData.nAnnoNum)
    {
        ASSERT(m_pAnno);
    }

}
#endif
//////////////////////////////////////////////////////////////////////////
