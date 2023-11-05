// GIS_FeatureRutV7.h: interface for the CGIS_FeatureRoadSectionV7 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATURERUTV7_H__7CFE54E6_0732_47C9_B784_0A14FECAFE55__INCLUDED_)
#define AFX_GIS_FEATURERUTV7_H__7CFE54E6_0732_47C9_B784_0A14FECAFE55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoGlobe.h"
#include "FileDataStruct/V2/GIS_FeatureV2.h"
#include "SpaceFileStructV7.h"
//////////////////////////////////////////////////////////////////////////
/**
 * @brief 这是主干路网的空间路段的数据结构
 *
 * @param 
 * @return
*/
class CGIS_FeatureRoadSectionV7 //: public CGIS_FeatureV2
{
public:
	CGIS_FeatureRoadSectionV7();
	~CGIS_FeatureRoadSectionV7();
    void* Clone();

    StuSpaceFileStructV7::StuGisFeatureRoadSection* GetDataAddr(){return m_pStData;};
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
	void MakeBuddyIndependent();
//////////////////////////////////////////////////////////////////////////
public:
	unsigned char GetAnnoNum()  {return m_pStData->nAnnoNum;};
    unsigned char GetDirection(){return m_pStData->nDirection;};
    unsigned short GetPtNum()   {return m_pStData->nPointCount;};
    unsigned long GetRutID()    {return m_pStData->nID;};
	char* GetAnnotation(){	return m_pAnno;};
    ST_GEO_PTXY* GetPtList()    {return m_pLoction;};
    

//////////////////////////////////////////////////////////////////////////
public:
    StuSpaceFileStructV7::StuGisFeatureRoadSection* m_pStData;
	char*   m_pAnno;
	ST_GEO_PTXY*  m_pLoction;
//////////////////////////////////////////////////////////////////////////
public:
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////
};

//////////////////////////////////////////////////////////////////////////

inline void CGIS_FeatureRoadSectionV7::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
{		
	if(m_pStData->nAnnoNum)
	{
		m_pAnno	= (char*)(pCharAddr);
		pCharAddr += m_pStData->nAnnoNum*sizeof(char);
	}
	
	if(m_pStData->nPointCount)
	{
		m_pLoction	= (ST_GEO_PTXY*)(pDwordAddr);
		pDwordAddr += m_pStData->nPointCount<<3;
	}
}

#endif // !defined(AFX_GIS_FEATURERUTV7_H__7CFE54E6_0732_47C9_B784_0A14FECAFE55__INCLUDED_)
