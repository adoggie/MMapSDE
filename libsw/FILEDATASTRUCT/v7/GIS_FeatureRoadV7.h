// GIS_FeatureRoadV7.h: interface for the CGIS_FeatureRoadV7 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATUREROADV7_H__AC7FC514_FC2D_4136_8773_06926B0EA399__INCLUDED_)
#define AFX_GIS_FEATUREROADV7_H__AC7FC514_FC2D_4136_8773_06926B0EA399__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoGlobe.h"
#include "FileDataStruct/V2/GIS_FeatureV2.h"
#include "SpaceFileStructV7.h"
//////////////////////////////////////////////////////////////////////////
/**
 * @brief 道路数据结构
 *
 * @param 
 * @return
*/
class CGIS_FeatureRoadV7  
{
public:
	CGIS_FeatureRoadV7();
	~CGIS_FeatureRoadV7();
    void* Clone();

    StuSpaceFileStructV7::StuGisFeatureRoad* GetDataAddr(){return m_pStData;};
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
//////////////////////////////////////////////////////////////////////////
public:
    unsigned long       GetID()                 {return m_pStData->nID;         };
    unsigned char       GetDirection()          {return m_pStData->nDirection;  };
    unsigned short      GetPointCount()         {return m_pStData->nPointCount; };
    unsigned short      GetSectionCount()       {return m_pStData->nSectionCount;};
	unsigned short      GetAnnotationBlockSize()    {return m_pStData->nAnnotateBlockSize;};

	char*               GetAnnotationBlock()    {return m_pAnnotateBlock;       };
    unsigned short*     GetSectionNumberList()  {return m_pSectionNumberList;   };
    unsigned short*     GetSectionAngleList()   {return m_pSectionAngleList;    };
    ST_GEO_PTXY*        GetPointList()          {return m_pLoctionList;             };
    

//////////////////////////////////////////////////////////////////////////
public:
    StuSpaceFileStructV7::StuGisFeatureRoad* m_pStData;
	char*               m_pAnnotateBlock;//指向名字串块
	unsigned short*     m_pSectionNumberList;
    unsigned short*     m_pSectionAngleList;
	ST_GEO_PTXY*        m_pLoctionList;
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

inline void CGIS_FeatureRoadV7::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
{		
	if(m_pStData->nAnnotateBlockSize)
	{
		m_pAnnotateBlock	= (char*)(pCharAddr);
		pCharAddr += m_pStData->nAnnotateBlockSize*sizeof(char);
	}
	
	if(m_pStData->nSectionCount)
	{
		m_pSectionNumberList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_pStData->nSectionCount<<1;

		m_pSectionAngleList	    = (unsigned short*)(pShortAddr);
		pShortAddr += m_pStData->nSectionCount<<1;
	}
    
	if(m_pStData->nPointCount)
	{
		m_pLoctionList	= (ST_GEO_PTXY*)(pDwordAddr);
		pDwordAddr += m_pStData->nPointCount<<3;
	}
}

#endif // !defined(AFX_GIS_FEATUREROADV7_H__AC7FC514_FC2D_4136_8773_06926B0EA399__INCLUDED_)
