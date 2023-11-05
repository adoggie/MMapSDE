// GIS_FeatureRoadV7.cpp: implementation of the CGIS_FeatureRoadV7 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureRoadV7.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureRoadV7::CGIS_FeatureRoadV7()
{
    m_pStData = NULL;
    m_pAnnotateBlock = NULL;
    m_pSectionNumberList = NULL;
    m_pSectionAngleList = NULL;
    m_pLoctionList = NULL;
}

CGIS_FeatureRoadV7::~CGIS_FeatureRoadV7()
{

}

void* CGIS_FeatureRoadV7::Clone()
{
	CGIS_FeatureRoadV7* pFtr = new CGIS_FeatureRoadV7;

    pFtr->m_pStData = new StuSpaceFileStructV7::StuGisFeatureRoad;
    memcpy(pFtr->m_pStData,this->m_pStData,sizeof(StuSpaceFileStructV7::StuGisFeatureRoad));
    int nCount = this->GetAnnotationBlockSize();
    pFtr->m_pAnnotateBlock = new char[nCount];
    memcpy(pFtr->m_pAnnotateBlock,this->GetAnnotationBlock(),nCount);
    nCount = this->GetPointCount();
    pFtr->m_pLoctionList = new ST_GEO_PTXY[nCount];
    memcpy(pFtr->m_pLoctionList,this->GetPointList(),sizeof(ST_GEO_PTXY)*nCount);
    return pFtr;
    
	return pFtr;
}



//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_FeatureRoadV7::DumpState() const	
{  
    TRACE1("nID:%d\n",m_pStData->nID);
    TRACE1("nDirection:%d\n",m_pStData->nDirection );
    TRACE1("nPointCount:%d\n",m_pStData->nPointCount );
    TRACE1("nSectionCount:%d\n",m_pStData->nSectionCount );
    TRACE1("nAnnotateBlockSize:%d\n",m_pStData->nAnnotateBlockSize );

}

void CGIS_FeatureRoadV7::AssertValid() const	
{   
    ASSERT(this);
    ASSERT(m_pStData->nAnnotateBlockSize    <= 20000);
    ASSERT((m_pStData->nDirection&0x7f)  <= 3);

    ASSERT((m_pStData->nID>>15)    < 100000);
    ASSERT((m_pStData->nID&0X7FFF) < 20000);
    ASSERT(m_pStData->nPointCount < 9000);

    if(m_pStData->nAnnotateBlockSize)
    {
        ASSERT(m_pAnnotateBlock);
    }
    if(m_pStData->nSectionCount)
    {
        ASSERT(m_pSectionNumberList);
        ASSERT(m_pSectionAngleList);
    }


}
#endif
//////////////////////////////////////////////////////////////////////////
