// GIS_NetworkVtx.cpp: implementation of the CGIS_NetworkVtx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkVtx.h"
#include "GIS_NetworkRut.h"

#include "./MemoryMngLib/IMemoryMng.h"
#include "./MemoryMngLib/IMemoryMng2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkVtx::CGIS_NetworkVtx( )
{
	m_nVtxID = 0;
//	m_byEdge = 0;	
//	m_nABID = 0;	
//	m_nARID = 0;
	m_pVEInfo = NULL;
	m_byRNum = 0;	
	m_pVSInfo = NULL;
//	m_posOHead = m_posOEnd = NULL;
//	m_bySHead = m_bySEnd = 0;
	m_pVRList = NULL;	
	m_pRTList = NULL;
	m_pRLList = NULL;
	m_nVBID	= 0;
}

CGIS_NetworkVtx::~CGIS_NetworkVtx()
{

    if( m_pVEInfo )
		m_pMemMngOfEdgeInfo->newfree( m_pVEInfo );
	m_pVEInfo = NULL;
	m_byRNum = 0;	
	m_nVBID	= 0;
	if( m_pVSInfo )
		m_pMemMngOfSortInfo->newfree( m_pVSInfo );
	m_pVSInfo = NULL;
//	m_posOHead = m_posOEnd = NULL;
//	m_bySHead = m_bySEnd = 0;
	if( m_pVRList )
		m_pMemMngOfBuddyData->newfree( m_pVRList );
	m_pVRList = NULL;	
	if( m_pRTList )
		m_pMemMngOfBuddyData->newfree(m_pRTList);
	m_pRTList = NULL;	
	if( m_pRLList )
		m_pMemMngOfBuddyData->newfree( m_pRLList );
	m_pRLList = NULL;	



	m_nVtxID = 0;	
//	m_byEdge = 0;	
//	m_nABID = 0;	
//	m_nARID = 0;
	m_byRNum = 0;	
	m_nVBID	= 0;
//	if( m_pVEInfo )
//		delete m_pVEInfo;
//	m_pVEInfo = NULL;
//	if( m_pVSInfo )
//		delete m_pVSInfo;
//	m_pVSInfo = NULL;
////	m_posOHead = m_posOEnd = NULL;
////	m_bySHead = m_bySEnd = 0;
//	if( m_pVRList )
//		delete[] m_pVRList;
//	m_pVRList = NULL;	
//	if( m_pRTList )
//		delete[] m_pRTList;
//	m_pRTList = NULL;	
//	if( m_pRLList )
//		delete[] m_pRLList;
//	m_pRLList = NULL;	
}
void CGIS_NetworkVtx::SetVtxInfo( unsigned short nVID, ST_GEO_PTXY ptPostion, int nVBID )
{
	m_nVtxID = nVID;
	m_ptPosition = ptPostion;
	m_nVBID = nVBID;
}
void CGIS_NetworkVtx::SetVRutInfo( BYTE byRNum, unsigned short *pVRList, unsigned short *pRTList, BYTE *pRLList )
{
	m_byRNum = byRNum;
	if( m_pVRList )
		delete[] m_pVRList;
	m_pVRList = NULL;	
	if( m_pRTList )
		delete[] m_pRTList;
	m_pRTList = NULL;	
	if( m_pRLList )
		delete[] m_pRLList;
	m_pRLList = NULL;	
	if( m_byRNum > 0 )
	{
//		m_pVRList = new unsigned short[byRNum];
		m_pVRList = (unsigned short*)m_pMemMngOfBuddyData->newmalloc(sizeof(unsigned short)*byRNum);
        if(!m_pVRList)
        {
            THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        }
		memcpy( m_pVRList, pVRList, sizeof(unsigned short)*byRNum );
//		m_pRTList = new unsigned short[byRNum];
		m_pRTList = (unsigned short*)m_pMemMngOfBuddyData->newmalloc(sizeof(unsigned short)*byRNum);
        if(!m_pRTList)
        {
            THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        }
		memcpy( m_pRTList, pRTList, sizeof(unsigned short)*byRNum );
//		m_pRLList = new BYTE[byRNum];
		m_pRLList = (BYTE*)m_pMemMngOfBuddyData->newmalloc(sizeof(BYTE)*byRNum);
        if(!m_pRLList)
        {
            THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        }
		memcpy( m_pRLList, pRLList, sizeof(BYTE)*byRNum );
	}
}
void CGIS_NetworkVtx::SetVEInfo( BYTE byEdge, int nABID, unsigned short nARID )
{
	if( m_pVEInfo )
		delete m_pVEInfo;
	m_pVEInfo = NULL;
	if( byEdge == 1 )
	{
//		m_pVEInfo = new ST_GEO_VEDGE;
		m_pVEInfo = (ST_GEO_VEDGE*)m_pMemMngOfEdgeInfo->newmalloc(sizeof(ST_GEO_VEDGE));
        if(!m_pVEInfo)
        {
            THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        }
		m_pVEInfo->byEdge = byEdge;
		m_pVEInfo->nABID = nABID;
		m_pVEInfo->nARID = nARID;
	}
//	m_byEdge = byEdge;
//	if( m_byEdge == 1 )
//	{
//		m_nABID = nABID;
//		m_nARID = nARID;
//	}
}

unsigned short CGIS_NetworkVtx::GetVtxID( )
{
	return m_nVtxID;
}
ST_GEO_PTXY CGIS_NetworkVtx::GetVtxPosition( )
{
	return m_ptPosition;
}
BYTE CGIS_NetworkVtx::GetVRutNum( )
{
	return m_byRNum;
}
unsigned short* CGIS_NetworkVtx::GetVRutList( )
{
	return m_pVRList;
}
unsigned short* CGIS_NetworkVtx::GetVRTurnList( )
{
	return m_pRTList;
}
BYTE* CGIS_NetworkVtx::GetVRLList( )
{
	return m_pRLList;
}
BYTE CGIS_NetworkVtx::GetVtxEdge( )
{
	if( m_pVEInfo )
		return m_pVEInfo->byEdge;
	return 0;
}
int CGIS_NetworkVtx::GetVAbutBID( )
{
	if( m_pVEInfo )
		return m_pVEInfo->nABID;
	return -1;
}
unsigned short CGIS_NetworkVtx::GetVAbutRID( )
{
	if( m_pVEInfo )
		return m_pVEInfo->nARID;
	return -1;
}
int CGIS_NetworkVtx::GetVBreadthID()
{
	return m_nVBID;
}
ST_GEO_VSORT* CGIS_NetworkVtx::GetVtxSInfo( )
{
	if( m_pVSInfo == NULL )
	{
//		m_pVSInfo = new ST_GEO_VSORT;
		m_pVSInfo = (ST_GEO_VSORT*)m_pMemMngOfSortInfo->newmalloc(sizeof(ST_GEO_VSORT));
        if(!m_pVSInfo)
        {
            THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        }
		memset( m_pVSInfo, 0, sizeof(ST_GEO_VSORT) );
	}
	return m_pVSInfo;
}

//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE
IMemoryMng* CGIS_NetworkVtx::m_pMemMngOfSelf = NULL;
IMemoryMng* CGIS_NetworkVtx::m_pMemMngOfBuddyData = NULL;
IMemoryMng* CGIS_NetworkVtx::m_pMemMngOfSortInfo = NULL;
IMemoryMng* CGIS_NetworkVtx::m_pMemMngOfEdgeInfo = NULL;
//--------------------------------------
void* CGIS_NetworkVtx::operator new(size_t size)
{	
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkVtx 的本体内存管理器尚未初始化.\n");
		THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        return NULL;
    }

	void* pRtn = m_pMemMngOfSelf->newmalloc(size);
	if(!pRtn)
	{
        THROW(new CUserException(TRUE,EN_ET_MEMOVER));
	}
    return pRtn;
}

void CGIS_NetworkVtx::operator delete (void* p,size_t size)
{
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkVtx 的本体内存管理器尚未初始化.\n");
        return;
    }
    m_pMemMngOfSelf->newfree(p);
}
#endif
//end
//////////////////////////////////////////////////////////////////////////
CGIS_NetworkVtx* CGIS_NetworkVtx::CreateCloneObject()
{
    CGIS_NetworkVtx* pFtr = (CGIS_NetworkVtx*)malloc(sizeof(CGIS_NetworkVtx));
    memset(pFtr,0,sizeof(CGIS_NetworkVtx));
	
	pFtr->m_nVtxID = m_nVtxID;		
	pFtr->m_ptPosition = m_ptPosition;	
	pFtr->m_nVBID = m_nVBID;		
	pFtr->m_byRNum = m_byRNum;	

    if(m_pVEInfo)
    {
        pFtr->m_pVEInfo = new ST_GEO_VEDGE;
        memcpy(pFtr->m_pVEInfo,m_pVEInfo,sizeof(ST_GEO_VEDGE));
    }
    if(pFtr->m_byRNum)
    {
        pFtr->m_pVRList = new unsigned short[pFtr->m_byRNum];
        pFtr->m_pRTList = new unsigned short[pFtr->m_byRNum];
        pFtr->m_pRLList = new unsigned char[pFtr->m_byRNum];
        memcpy(pFtr->m_pVRList,m_pVRList,sizeof(unsigned short)*pFtr->m_byRNum);
        memcpy(pFtr->m_pRTList,m_pRTList,sizeof(unsigned short)*pFtr->m_byRNum);
        memcpy(pFtr->m_pRLList,m_pRLList,sizeof(unsigned char)*pFtr->m_byRNum);
    }
	return pFtr;    
}
void CGIS_NetworkVtx::DestoryCloneObject()
{
    if(m_byRNum)
    {
        delete m_pVRList;
        delete m_pRTList;
        delete m_pRLList;
    }

    if(m_pVEInfo)
        delete m_pVEInfo;

    memset(this,0,sizeof(CGIS_NetworkVtx));
    free(this);
}
