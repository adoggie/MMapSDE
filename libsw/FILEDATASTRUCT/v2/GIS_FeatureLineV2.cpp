// GIS_FeatureLineV2.cpp: implementation of the CGIS_FeatureLineV2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureLineV2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureLineV2::CGIS_FeatureLineV2()
	: CGIS_FeatureV2( EN_GOTYPE_LINE )
{
    memset(&m_stData,0,sizeof(m_stData));

}

CGIS_FeatureLineV2::~CGIS_FeatureLineV2()
{

}

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_FeatureLineV2::DumpState() const	
{  
}
void CGIS_FeatureLineV2::AssertValid() const	
{   
    ASSERT(m_stData.nAnnoNum    <= 70);
}
#endif
//////////////////////////////////////////////////////////////////////////
