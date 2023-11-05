// GIS_FeatureRutV5.h: interface for the CGIS_FeatureRoadSectionV5 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATURERUTV5_H__7CFE54E6_0732_47C9_B784_0A14FECAFE55__INCLUDED_)
#define AFX_GIS_FEATURERUTV5_H__7CFE54E6_0732_47C9_B784_0A14FECAFE55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileDataStruct/V2/GIS_FeatureV2.h"
#include "SpaceFileStructV5.h"
//////////////////////////////////////////////////////////////////////////
/**
 * @brief 这是主干路网的空间路段的数据结构
 *
 * @param 
 * @return
*/
class CGIS_FeatureRoadSectionV5 : public CGIS_FeatureV2
{
public:
	CGIS_FeatureRoadSectionV5();
	virtual ~CGIS_FeatureRoadSectionV5();
    virtual void* Clone();

    StuSpaceFileStructV5::StuGisFeatureRoadSection* GetDataAddr(){return &m_stData;};
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
	void MakeBuddyIndependent();
//////////////////////////////////////////////////////////////////////////
public:
	unsigned char GetAnnoNum()  {return m_stData.nAnnoNum;};
    unsigned char GetDirection(){return m_stData.nDirection;};
    unsigned short GetPtNum()   {return m_stData.nPointCount;};
    unsigned long GetRutID()    {return m_stData.nID;};
	char* GetAnnotation(){	return m_pAnno;};
    ST_GEO_PTXY* GetPtList()    {return m_pLoction;};
    

//////////////////////////////////////////////////////////////////////////
public:
    StuSpaceFileStructV5::StuGisFeatureRoadSection m_stData;
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

inline void CGIS_FeatureRoadSectionV5::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
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

inline void CGIS_FeatureRoadSectionV5::MakeBuddyIndependent()
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


#endif // !defined(AFX_GIS_FEATURERUTV5_H__7CFE54E6_0732_47C9_B784_0A14FECAFE55__INCLUDED_)
