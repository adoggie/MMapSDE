// GIS_FeaturePointV2.cpp: implementation of the CGIS_FeaturePointV2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeaturePointV2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeaturePointV2::CGIS_FeaturePointV2()
	: CGIS_FeatureV2( EN_GOTYPE_POINT )
{
    memset(&m_stData,0,sizeof(m_stData));
}

CGIS_FeaturePointV2::~CGIS_FeaturePointV2()
{

}


//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_FeaturePointV2::DumpState() const	
{  
}

void CGIS_FeaturePointV2::AssertValid() const	
{   
    ASSERT(m_stData.nAnnoNum    <= 70);
    ASSERT(m_stData.nAddressNum <= 255);
    ASSERT((m_stData.nPhoneNum&0x7f)        <= 25);
 
    ASSERT(m_stData.fLat >=-180.0f);
    ASSERT(m_stData.fLat <=180.0f);
    ASSERT(m_stData.fLog >=-180.0f);
    ASSERT(m_stData.fLog <=180.0f);
    if(m_stData.nAnnoNum)
    {
        ASSERT(m_pAnno);
    }
    if(m_stData.nAddressNum)
    {
        ASSERT(m_pAddress);
    }
    if(m_stData.nPhoneNum&0x7f)
    {
        ASSERT(m_pPhone);
    }

}
#endif
//////////////////////////////////////////////////////////////////////////