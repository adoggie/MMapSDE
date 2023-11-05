// GIS_FeatureV2.cpp: implementation of the CGIS_FeatureV2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureV2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureV2::CGIS_FeatureV2( EnGeoObjType eType )
{
    m_eObjectType = eType;
    m_bBuddyIndependent = FALSE;
}

CGIS_FeatureV2::~CGIS_FeatureV2()
{

}

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_FeatureV2::DumpState() const	
{  
}
void CGIS_FeatureV2::AssertValid() const	
{   
}
#endif
//////////////////////////////////////////////////////////////////////////