// TypeTranslateTool.h: interface for the CTypeTranslateTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TYPETRANSLATETOOL_H__0E2C749B_0065_4260_A7BA_5697CFB0B6FC__INCLUDED_)
#define AFX_TYPETRANSLATETOOL_H__0E2C749B_0065_4260_A7BA_5697CFB0B6FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
class CGIS_FeaturePoint;
class CGIS_FeatureRut;
class CGIS_FeatureLine;
class CGIS_FeaturePoly;
//////////////////////////////////////////////////////////////////////////
class CGIS_FeaturePointV2;
class CGIS_FeatureRoadSectionV2;
class CGIS_FeatureLineV2;
class CGIS_FeaturePolyV2;
//////////////////////////////////////////////////////////////////////////


class CTypeTranslateTool  
{
//////////////////////////////////////////////////////////////////////////
//V1->V2
public:
    static BOOL TransGisFeaturePointV1ToV2(     CGIS_FeaturePoint*  pFtrSrc,CGIS_FeaturePointV2*&       pFtrDest);
    static BOOL TransGisFeatureRoadSecionV1ToV2(CGIS_FeatureRut*    pFtrSrc,CGIS_FeatureRoadSectionV2*& pFtrDest);
    static BOOL TransGisFeatureLineV1ToV2(      CGIS_FeatureLine*   pFtrSrc,CGIS_FeatureLineV2*&        pFtrDest);
    static BOOL TransGisFeaturePolyV1ToV2(      CGIS_FeaturePoly*   pFtrSrc,CGIS_FeaturePolyV2*&        pFtrDest);
//////////////////////////////////////////////////////////////////////////
//v2->v1
public:
    static BOOL TransGisFeaturePointV2ToV1(     CGIS_FeaturePointV2*        pFtrSrc,CGIS_FeaturePoint*& pFtrDest);
    static BOOL TransGisFeatureRoadSecionV2ToV1(CGIS_FeatureRoadSectionV2*  pFtrSrc,CGIS_FeatureRut*&   pFtrDest);
    static BOOL TransGisFeatureLineV2ToV1(      CGIS_FeatureLineV2*         pFtrSrc,CGIS_FeatureLine*&  pFtrDest);
    static BOOL TransGisFeaturePolyV2ToV1(      CGIS_FeaturePolyV2*         pFtrSrc,CGIS_FeaturePoly*&  pFtrDest);
};

#endif // !defined(AFX_TYPETRANSLATETOOL_H__0E2C749B_0065_4260_A7BA_5697CFB0B6FC__INCLUDED_)
