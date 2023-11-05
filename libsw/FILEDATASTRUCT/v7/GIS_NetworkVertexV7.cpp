// GIS_NetworkVertexV7.cpp: implementation of the CGIS_NetworkVertexV7 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkVertexV7.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkVertexV7::CGIS_NetworkVertexV7()
{
	memset(this,0,sizeof(CGIS_NetworkVertexV7));
}

CGIS_NetworkVertexV7::~CGIS_NetworkVertexV7()
{
    if( m_pVSInfo )
    {
        free( m_pVSInfo );
        m_pVSInfo = NULL;
    }
}

CGIS_NetworkVertexV7::ST_GEO_VSORT* CGIS_NetworkVertexV7::GetVtxSInfo()
{
    if(!m_pVSInfo)
    {
        m_pVSInfo = (ST_GEO_VSORT*)malloc(sizeof(ST_GEO_VSORT));
        memset( m_pVSInfo, 0, sizeof(ST_GEO_VSORT) );
    }
    return m_pVSInfo;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// this code for check self
#ifdef _DEBUG
void CGIS_NetworkVertexV7::AssertValid() const
{
   	//ASSERT( m_nVtxID < 20000 );
	ASSERT( m_stData.m_nRoadSectionNum < 11 );	
	ASSERT( m_stData.m_nLowLayerRoadSectionNum < 11 );	

	ASSERT( m_stData.m_fLat < 180.0f && m_stData.m_fLat > -180.0f );
	ASSERT( m_stData.m_fLog < 180.0f && m_stData.m_fLog > -180.0f );	

	ASSERT( m_stData.m_nBreadthID < 10000 );					//节点所在图幅ID

	ASSERT( m_pRoadSectionList);				//相连接的路段ID
	ASSERT( m_pRoadSectionTurnList );						//与水平方向的夹角数组
	
	for(int i = 0; i < m_stData.m_nRoadSectionNum; i++)
	{
		ASSERT( m_pRoadSectionList[i] < 10000 );
		ASSERT( m_pRoadSectionTurnList[i] <= 360 &&  m_pRoadSectionTurnList[i] >= 0);
	}
	if(m_stData.m_nEdge)
	{
		ASSERT(m_pEdgeInfo);
	} 
    else
    {
        ASSERT(!m_pEdgeInfo);
    }
}
void CGIS_NetworkVertexV7::Dump(CDumpContext& dc) const
{

}
#endif
//////////////////////////////////////////////////////////////////////////
CGIS_NetworkVertexV7* CGIS_NetworkVertexV7::CreateCloneObject()
{
    CGIS_NetworkVertexV7* pFtr = (CGIS_NetworkVertexV7*)malloc(sizeof(CGIS_NetworkVertexV7));
    memset(pFtr,0,sizeof(CGIS_NetworkVertexV7));	
    pFtr->m_stData = m_stData;

    if(m_stData.m_nRoadSectionNum)
    {
        pFtr->m_pRoadSectionList = new unsigned short[m_stData.m_nRoadSectionNum];
        pFtr->m_pRoadSectionTurnList = new unsigned short[m_stData.m_nRoadSectionNum];
        memcpy(pFtr->m_pRoadSectionList,m_pRoadSectionList,sizeof(unsigned short)*m_stData.m_nRoadSectionNum);
        memcpy(pFtr->m_pRoadSectionTurnList,m_pRoadSectionTurnList,sizeof(unsigned short)*m_stData.m_nRoadSectionNum);
    }
    if(m_stData.m_nLowLayerRoadSectionNum)
    {
        pFtr->m_pLowLayerRoadSectionTurnList = new unsigned short[m_stData.m_nLowLayerRoadSectionNum];
        memcpy(pFtr->m_pLowLayerRoadSectionTurnList,m_pLowLayerRoadSectionTurnList,sizeof(unsigned short)*m_stData.m_nLowLayerRoadSectionNum);
    }
 	return pFtr;    
}
void CGIS_NetworkVertexV7::DestoryCloneObject()
{

    if(m_pLowLayerRoadSectionTurnList)
    {
        free(m_pLowLayerRoadSectionTurnList);
    }
    if(m_pRoadSectionTurnList)
    {
        free(m_pRoadSectionTurnList);
    }
    if(m_pRoadSectionList)
    {
        free(m_pRoadSectionList);
    }
    memset(this,0,sizeof(CGIS_NetworkVertexV7));


    free(this);

}
//////////////////////////////////////////////////////////////////////////