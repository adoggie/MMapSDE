// GIS_FeatureRutV2.h: interface for the CGIS_FeatureRoadSectionV2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATURERUTV2_H__A887932D_CB45_4245_8859_CCA99C67DD90__INCLUDED_)
#define AFX_GIS_FEATURERUTV2_H__A887932D_CB45_4245_8859_CCA99C67DD90__INCLUDED_

#include "GIS_FeatureV2.h"
#include "SpaceFileStructV2.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


 
class CGIS_FeatureRoadSectionV2 : public CGIS_FeatureV2
{
public:
	CGIS_FeatureRoadSectionV2();
	virtual ~CGIS_FeatureRoadSectionV2();

    StuSpaceFileStructV2::StuGisFeatureRoadSection* GetDataAddr(){return &m_stData;};
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
	void MakeBuddyIndependent();
//////////////////////////////////////////////////////////////////////////
public:
	char* GetAnnotation(){	return m_pAnno;};
	int GetAnnoNum(){return m_stData.nAnnoNum;};
    int GetDirection(){return m_stData.nDirection;};
    int GetPtNum(){return m_stData.nPointCount;};
    int GetRutID(){return m_stData.nID;};
    ST_GEO_PTXY* GetPtList(){	return m_pLoction;};

//////////////////////////////////////////////////////////////////////////
public:
    StuSpaceFileStructV2::StuGisFeatureRoadSection m_stData;

	char*   m_pAnno;
	ST_GEO_PTXY*  m_pLoction;
//////////////////////////////////////////////////////////////////////////
public:
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	virtual void DumpState() const;		
	virtual void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////
};

//////////////////////////////////////////////////////////////////////////

inline void CGIS_FeatureRoadSectionV2::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
{		
	if(m_stData.nAnnoNum)
	{
		m_pAnno	= (char*)(pCharAddr);
		pCharAddr += m_stData.nAnnoNum*sizeof(char);
	}
	
	if(m_stData.nPointCount)
	{
		m_pLoction	= (ST_GEO_PTXY*)(pDwordAddr);
		pDwordAddr += m_stData.nPointCount<<3;
	}
}

inline void CGIS_FeatureRoadSectionV2::MakeBuddyIndependent()
{
    ASSERT(!m_bBuddyIndependent);
    //////////////////////////////////////////////////////////////////////////
    int nSize = 0;
    void* pDest = NULL;
    void* pSrc = NULL;
    
	if(m_stData.nAnnoNum)
	{
        ASSERT(m_pAnno);
        nSize = m_stData.nAnnoNum;
        pSrc = m_pAnno;
        pDest = malloc(nSize);
        memcpy(pDest,pSrc,nSize);
        m_pAnno = (char*)pDest;
	}
	if(m_stData.nPointCount)
	{
        ASSERT(m_pLoction);
        nSize = m_stData.nPointCount<<3;
        pSrc = m_pLoction;
        pDest = malloc(nSize);
        memcpy(pDest,pSrc,nSize);
        m_pLoction = (ST_GEO_PTXY*)pDest;
	}
    //////////////////////////////////////////////////////////////////////////
    m_bBuddyIndependent = TRUE;
}

#endif // !defined(AFX_GIS_FEATURERUTV2_H__A887932D_CB45_4245_8859_CCA99C67DD90__INCLUDED_)
