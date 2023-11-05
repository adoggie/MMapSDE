// GIS_FeatureLineV2.h: interface for the CGIS_FeatureLineV2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATURELINEV2_H__AF9BF2B6_2F4A_43DD_8E3F_2D86662CC994__INCLUDED_)
#define AFX_GIS_FEATURELINEV2_H__AF9BF2B6_2F4A_43DD_8E3F_2D86662CC994__INCLUDED_

#include "GIS_FeatureV2.h"
#include "SpaceFileStructV2.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGIS_FeatureLineV2  : public CGIS_FeatureV2
{
public:
	CGIS_FeatureLineV2();
	virtual ~CGIS_FeatureLineV2();

    StuSpaceFileStructV2::StuGisFeatureLine* GetDataAddr(){return &m_stData;};
    void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
	void MakeBuddyIndependent();
//////////////////////////////////////////////////////////////////////////
    public:
    char*   GetAnnotation()         {return m_pAnno;};
    int     GetAnnoNum()            {return m_stData.nAnnoNum;};
    int     GetPartNum()            {return m_stData.nPartCount;};
    int     GetPtNum( )     {return m_stData.nPointCount;};
    ST_GEO_PTXY*    GetPtList()     {return m_pLoction;};
    int* GetPart()                  {return m_pPart;};

	void SetRectObj(ST_GEO_RECT rtObj);
	ST_GEO_RECT GetRectObj();
//////////////////////////////////////////////////////////////////////////
public:
    StuSpaceFileStructV2::StuGisFeatureLine m_stData;
    char*           m_pAnno;
    ST_GEO_PTXY*    m_pLoction;
    int*            m_pPart;   
	ST_GEO_RECT	    m_rtObj;
    
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

inline void CGIS_FeatureLineV2::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
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

	if(m_stData.nPartCount)
	{
		m_pPart	= (int*)(pDwordAddr);
		pDwordAddr += m_stData.nPartCount<<2;
	}
}

inline void CGIS_FeatureLineV2::MakeBuddyIndependent()
{
    ASSERT(!(m_bBuddyIndependent));
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
	if(m_stData.nPartCount)
	{
        ASSERT(m_pPart);
        nSize = m_stData.nPartCount<<2;
        pSrc = m_pPart;
        pDest = malloc(nSize);
        memcpy(pDest,pSrc,nSize);
        m_pPart = (int*)pDest;
	}
    //////////////////////////////////////////////////////////////////////////
    m_bBuddyIndependent = TRUE;
}




#endif // !defined(AFX_GIS_FEATURELINEV2_H__AF9BF2B6_2F4A_43DD_8E3F_2D86662CC994__INCLUDED_)
