// TypeTranslateTool.cpp: implementation of the CTypeTranslateTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TypeTranslateTool.h"

#include "./FileDataStruct/V1/GIS_FeaturePoint.h"
#include "./FileDataStruct/V1/GIS_FeatureLine.h"
#include "./FileDataStruct/V1/GIS_FeatureRut.h"
#include "./FileDataStruct/V1/GIS_FeaturePoly.h"

#include "./FileDataStruct/V2/GIS_FeaturePointV2.h"
#include "./FileDataStruct/V2/GIS_FeatureRoadSectionV2.h"
#include "./FileDataStruct/V2/GIS_FeatureLineV2.h"
#include "./FileDataStruct/V2/GIS_FeaturePolyV2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL CTypeTranslateTool::TransGisFeaturePointV1ToV2(CGIS_FeaturePoint* pFtrSrc,CGIS_FeaturePointV2*& pFtrDest)
{

    pFtrDest->m_stData.fLat = pFtrSrc->GetPtList()->dy;
    pFtrDest->m_stData.fLog = pFtrSrc->GetPtList()->dx;
    pFtrDest->m_stData.nAddressNum = pFtrSrc->GetStrAddressLength();
    pFtrDest->m_stData.nAnnoNum = pFtrSrc->GetAnnoNum();
    pFtrDest->m_stData.nPhoneNum = pFtrSrc->GetStrPhoneLength();
    pFtrDest->m_stData.nType = pFtrSrc->GetObjType();

    pFtrDest->m_pAnno       = pFtrSrc->GetAnnotation();
    pFtrDest->m_pAddress    = pFtrSrc->GetAddress();
    pFtrDest->m_pPhone      = pFtrSrc->GetPhoneNum();
    
    pFtrDest->MakeBuddyIndependent();

    ASSERTVALID(pFtrDest);
    return TRUE;
}
BOOL CTypeTranslateTool::TransGisFeatureRoadSecionV1ToV2(CGIS_FeatureRut*    pFtrSrc,CGIS_FeatureRoadSectionV2*& pFtrDest)
{

    pFtrDest->m_stData.nAnnoNum         = pFtrSrc->GetAnnoNum();
    pFtrDest->m_stData.nDirection       = pFtrSrc->GetRutDirection();
    pFtrDest->m_stData.nID              = pFtrSrc->GetRutID();
    pFtrDest->m_stData.nPointCount      = pFtrSrc->GetPtNum();

    pFtrDest->m_pAnno = pFtrSrc->GetAnnotation();
    pFtrDest->m_pLoction = (CGIS_FeatureV2::ST_GEO_PTXY*)pFtrSrc->GetPtList();
    
    pFtrDest->MakeBuddyIndependent();

    ASSERTVALID(pFtrDest);
    return FALSE;
}

BOOL CTypeTranslateTool::TransGisFeatureLineV1ToV2(      CGIS_FeatureLine*   pFtrSrc,CGIS_FeatureLineV2*&        pFtrDest)
{
    pFtrDest->m_stData.nAnnoNum = pFtrSrc->GetAnnoNum();
    pFtrDest->m_stData.nPointCount = pFtrSrc->GetPtNum();
    pFtrDest->m_stData.nPartCount = pFtrSrc->GetPartNum();

    pFtrDest->m_pAnno = pFtrSrc->GetAnnotation();
    pFtrDest->m_pLoction = (CGIS_FeatureV2::ST_GEO_PTXY*)pFtrSrc->GetPtList();
    pFtrDest->m_pPart = pFtrSrc->GetPart();

    pFtrDest->MakeBuddyIndependent();

    ASSERTVALID(pFtrDest);
    return TRUE;
}
BOOL CTypeTranslateTool::TransGisFeaturePolyV1ToV2(      CGIS_FeaturePoly*   pFtrSrc,CGIS_FeaturePolyV2*&        pFtrDest)
{
    pFtrDest->m_stData.nAnnoNum = pFtrSrc->GetAnnoNum();
    pFtrDest->m_stData.nPointCount = pFtrSrc->GetPtNum();
    pFtrDest->m_stData.nPartCount = pFtrSrc->GetPartNum();

    pFtrDest->m_pAnno = pFtrSrc->GetAnnotation();
    pFtrDest->m_pLoction = (CGIS_FeatureV2::ST_GEO_PTXY*)pFtrSrc->GetPtList();
    pFtrDest->m_pPart = pFtrSrc->GetPart();

    pFtrDest->MakeBuddyIndependent();

    ASSERTVALID(pFtrDest);
    return TRUE;
}
