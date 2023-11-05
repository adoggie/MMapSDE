// GIS_FeaturePolyV2.cpp: implementation of the CGIS_FeaturePolyV2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeaturePolyV2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeaturePolyV2::CGIS_FeaturePolyV2()
	: CGIS_FeatureV2( EN_GOTYPE_POLY )
{

}

CGIS_FeaturePolyV2::~CGIS_FeaturePolyV2()
{

}

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_FeaturePolyV2::DumpState() const	
{  
}
void CGIS_FeaturePolyV2::AssertValid() const	
{   
    ASSERT(m_stData.nAnnoNum   <= 70);
    ASSERT(m_stData.nShowClass <= 9);
}
#endif
//////////////////////////////////////////////////////////////////////////
