// GIS_FeatureRutV7.cpp: implementation of the CGIS_FeatureRoadSectionV7 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureRoadSectionV7.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureRoadSectionV7::CGIS_FeatureRoadSectionV7()
{
    m_pStData = NULL;
    m_pAnno = NULL;
    m_pLoction = NULL;
}

CGIS_FeatureRoadSectionV7::~CGIS_FeatureRoadSectionV7()
{

}

void* CGIS_FeatureRoadSectionV7::Clone()
{
	CGIS_FeatureRoadSectionV7* pFtr = new CGIS_FeatureRoadSectionV7;

    
    pFtr->m_pStData = new StuSpaceFileStructV7::StuGisFeatureRoadSection;
    memcpy(pFtr->m_pStData,this->m_pStData,sizeof(StuSpaceFileStructV7::StuGisFeatureRoadSection));
    int nCount = this->GetAnnoNum();
    pFtr->m_pAnno = new char[nCount];
    memcpy(pFtr->m_pAnno,this->GetAnnotation(),nCount);
    nCount = this->GetPtNum();
    pFtr->m_pLoction = new ST_GEO_PTXY[nCount];
    memcpy(pFtr->m_pLoction,this->GetPtList(),sizeof(ST_GEO_PTXY)*nCount);
    return pFtr;
    

//	ST_GEO_PTXY *pPt = new ST_GEO_PTXY[m_nPtNum];
//	for( int i=0;	i<m_nPtNum;	i++ )
//	{
//		pPt[i].dx = m_pPtList[i].dx;
//		pPt[i].dy = m_pPtList[i].dy;
//	}
//	pFtr->SetPtList(pPt);
//	pFtr->SetPtNum(m_nPtNum);
//	char *szAnno = NULL;
//	if( m_nAnno>0 )
//	{
//		szAnno = new char[m_nAnno];
//		memcpy( szAnno, m_bstrAnno, m_nAnno );
//		pFtr->SetAnnotation( szAnno );
//	}
//	pFtr->SetAnnoNum( m_nAnno );
//	pFtr->m_nBreadthID = m_nBreadthID;
//	pFtr->SetRectObj( m_rtObj );
//	pFtr->SetRutID( m_nRutID );
//	pFtr->SetRutDirection( m_byDirection );
////	if( m_pAtt )
////	{
///*		CGIS_Attribute *pAtt = new CGIS_Attribute;
//		void *pV = m_pAtt->GetValue();
//		char *pVal = new char[sizeof(pV)];
//		memcpy( pVal, m_pAtt->GetValue(), sizeof(pV) );
//		pAtt->SetValue( pVal );
//		pFtr->SetAttribute( pAtt );
//*/// 	}
	return pFtr;
}



//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_FeatureRoadSectionV7::DumpState() const	
{  
}

void CGIS_FeatureRoadSectionV7::AssertValid() const	
{   
    ASSERT(this);
    ASSERT(m_pStData->nAnnoNum    <= 70);
    ASSERT((m_pStData->nDirection&0x7f)  <= 3);

    ASSERT((m_pStData->nID>>15)    < 100000);
    ASSERT((m_pStData->nID&0X7FFF) < 20000);
    ASSERT(m_pStData->nPointCount < 20000);

    if(m_pStData->nAnnoNum)
    {
        ASSERT(m_pAnno);
    }

}
#endif
//////////////////////////////////////////////////////////////////////////
