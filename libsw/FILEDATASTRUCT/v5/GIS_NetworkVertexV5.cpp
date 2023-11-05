// GIS_NetworkVertexV5.cpp: implementation of the CGIS_NetworkVertexV5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkVertexV5.h"

#include "./MemoryMngLib/IMemoryMng.h"
#include "./MemoryMngLib/IMemoryMng2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkVertexV5::CGIS_NetworkVertexV5()
{
	memset(this,0,sizeof(CGIS_NetworkVertexV5));
}

CGIS_NetworkVertexV5::~CGIS_NetworkVertexV5()
{
#ifdef MEMORY_SELF_MANAGE
    //如变长数据是从本体内存管理器中malloc出来,那么析构中要把它free掉
    if(m_nEdge&0x80)
    {
        if(m_pRoadSectionList)
        {
            m_pMemMngOfBuddyData->newfree( m_pRoadSectionList );
            m_pRoadSectionList = NULL;
        }
        if(m_pRoadSectionTurnList)
        {
            m_pMemMngOfBuddyData->newfree( m_pRoadSectionTurnList );
            m_pRoadSectionTurnList = NULL;
        }
        if(m_pRoadSectionList)
        {
            m_pMemMngOfBuddyData->newfree( m_pRoadSectionLayerIDLList );
            m_pRoadSectionLayerIDLList = NULL;
        }

        if(m_nEdge)
        {
            ASSERT(m_pEdgeInfo);
            m_pMemMngOfEdgeInfo->newfree( m_pEdgeInfo );
            m_pEdgeInfo = NULL;

        }
    }

    if( m_pVSInfo )
    {
        m_pMemMngOfSortInfo->newfree( m_pVSInfo );
        m_pVSInfo = NULL;
    }
#endif
}
void CGIS_NetworkVertexV5::MakeBuddyIndependent()
{
#ifdef MEMORY_SELF_MANAGE    
    ASSERT(m_nRoadSectionNum);
    ASSERT(m_pRoadSectionList);
    ASSERT(m_pRoadSectionTurnList);
    ASSERT(m_pRoadSectionLayerIDLList);

    unsigned long* pDwordData = (unsigned long*)m_pMemMngOfBuddyData->newmalloc(m_nRoadSectionNum<<2);
    memcpy(pDwordData,m_pRoadSectionList,m_nRoadSectionNum<<2);
    m_pRoadSectionList = pDwordData;
    unsigned short* pShortData = (unsigned short*)m_pMemMngOfBuddyData->newmalloc(m_nRoadSectionNum<<1);
    memcpy(pShortData,m_pRoadSectionTurnList,m_nRoadSectionNum<<1);
    m_pRoadSectionTurnList = pShortData;
                    pShortData = (unsigned short*)m_pMemMngOfBuddyData->newmalloc(m_nRoadSectionNum<<1);
    memcpy(pShortData,m_pRoadSectionLayerIDLList,m_nRoadSectionNum<<1);
    m_pRoadSectionLayerIDLList = pShortData;

    if(GetEdge())
    {
        ASSERT(m_pEdgeInfo);
		ST_GEO_VEDGE* pEdgeInfo = (ST_GEO_VEDGE*)m_pMemMngOfEdgeInfo->newmalloc();
        memcpy(pEdgeInfo,m_pEdgeInfo,sizeof(ST_GEO_VEDGE));
        m_pEdgeInfo = pEdgeInfo;
    }

    m_nEdge |=0x80;
#endif
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// this code for check self
#ifdef _DEBUG
void CGIS_NetworkVertexV5::AssertValid() const
{
   	//ASSERT( m_nVtxID < 20000 );
	ASSERT( m_stData.m_nRoadSectionNum < 11 );	

	ASSERT( m_stData.m_fLat < 180.0f && m_stData.m_fLat > -180.0f );
	ASSERT( m_stData.m_fLog < 180.0f && m_stData.m_fLog > -180.0f );	

	ASSERT( m_stData.m_nBreadthID < 100000 );					//节点所在图幅ID

	ASSERT( m_pRoadSectionList);				//相连接的路段ID
	ASSERT( m_pRoadSectionTurnList );						//与水平方向的夹角数组
	ASSERT( m_pRoadSectionLayerIDLList);		//路段所在图层ID
	
	for(int i = 0; i < m_stData.m_nRoadSectionNum; i++)
	{
		//ASSERT( m_pRoadSectionList[i] < 20000 );
		ASSERT( m_pRoadSectionTurnList[i] <= 360 &&  m_pRoadSectionTurnList[i] >= 0);
		ASSERT( m_pRoadSectionLayerIDLList[i] < 25 );
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
void CGIS_NetworkVertexV5::Dump(CDumpContext& dc) const
{

}
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE
IMemoryMng2*    CGIS_NetworkVertexV5::m_pMemMngOfSelf = NULL;
IMemoryMng*     CGIS_NetworkVertexV5::m_pMemMngOfBuddyData = NULL;
IMemoryMng2*    CGIS_NetworkVertexV5::m_pMemMngOfSortInfo = NULL;
IMemoryMng2*    CGIS_NetworkVertexV5::m_pMemMngOfEdgeInfo = NULL;
//--------------------------------------
void* CGIS_NetworkVertexV5::operator new(size_t size)
{	
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkVertexV5 的本体内存管理器尚未初始化.\n");
        return NULL;
    }

    return m_pMemMngOfSelf->newmalloc();
}

void CGIS_NetworkVertexV5::operator delete (void* p,size_t size)
{
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkVertexV5 的本体内存管理器尚未初始化.\n");
        return;
    }
    m_pMemMngOfSelf->newfree(p);
}
#endif
//end
//////////////////////////////////////////////////////////////////////////
