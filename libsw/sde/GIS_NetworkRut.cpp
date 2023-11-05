// GIS_NetworkRut.cpp: implementation of the CGIS_NetworkRut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkRut.h"

#include "./MemoryMngLib/IMemoryMng.h"
#include "./MemoryMngLib/IMemoryMng2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkRut::CGIS_NetworkRut()
{
	m_nRutID = 0;		
	m_byDirection = 0;	
	m_byType = 0;		
	m_byCharge = 0;		
//	m_byLaneNum = 0;		
//	m_bySpeed = 0;		
	m_byRutLevel = 0;	
	m_byRutType = 0;	
	m_dRLength = 0.0;	
	m_nVtxStartID = 0;	
	m_nVtxEndID = 0;	
//	m_byLimit = 0;		
//	m_byRLimit = 0;		
//	m_pLRList = NULL;		
//	m_pRLRList = NULL;	

//	m_pRVInfo = NULL;
//	m_byVirtual = 0;	
//	m_pVORList = NULL;		
//	m_pVIDs = NULL;	
	m_nRBID = 0;
    m_nUsedStatus = 0;

    m_pRoadSectionLimitInfo = NULL;
}
void CGIS_NetworkRut::Clear()
{
ASSERT(FALSE);
}

CGIS_NetworkRut::~CGIS_NetworkRut()
{
    ASSERT(m_pMemMngOfBuddyData);

    if( m_pRoadSectionLimitInfo )
    {
        if( m_pRoadSectionLimitInfo->sGroup.pTurnInfoArray )
        {
            for(int i = 0; i < m_pRoadSectionLimitInfo->sGroup.byTurnCount; i++)
            {
                CGIS_NetworkRut::TURNINFO* pTurnInfo = &m_pRoadSectionLimitInfo->sGroup.pTurnInfoArray[i];
                if(pTurnInfo->byToRouteIDCount)
                {
                    m_pMemMngOfBuddyData->newfree(pTurnInfo->pwToRutID);
                }
            }
            
            m_pMemMngOfBuddyData->newfree( m_pRoadSectionLimitInfo->sGroup.pTurnInfoArray );
        }
        if( m_pRoadSectionLimitInfo->eGroup.pTurnInfoArray )
        {
            for(int i = 0; i < m_pRoadSectionLimitInfo->eGroup.byTurnCount; i++)
            {
                CGIS_NetworkRut::TURNINFO* pTurnInfo = &m_pRoadSectionLimitInfo->eGroup.pTurnInfoArray[i];
                if(pTurnInfo->byToRouteIDCount)
                {
                    m_pMemMngOfBuddyData->newfree(pTurnInfo->pwToRutID);
                }
            }          
            m_pMemMngOfBuddyData->newfree(m_pRoadSectionLimitInfo->eGroup.pTurnInfoArray);
        }
        
        m_pMemMngOfBuddyData->newfree( m_pRoadSectionLimitInfo );
	}
	m_pRoadSectionLimitInfo = NULL;
}

void CGIS_NetworkRut::SetRutInfo( unsigned short nRID, BYTE byDirtect, BYTE byType, BYTE byCharge,
		BYTE byLNum,	BYTE bySpeed, BYTE byRLevel, BYTE byRType,
		float dRLength, unsigned short nVStart, unsigned short nVEnd, int nRBID )
{
	m_nRutID = nRID;		
	m_byDirection = byDirtect;	
	m_byType = byType;		
	m_byCharge = byCharge;		
//	m_byLaneNum = byLNum;		
//	m_bySpeed = bySpeed;		
	m_byRutLevel = byRLevel;	
	m_byRutType = byRType;	
	m_dRLength = dRLength;	
	m_nVtxStartID = nVStart;	
	m_nVtxEndID = nVEnd;
	m_nRBID = nRBID;
}
//void CGIS_NetworkRut::SetRutLimitInfo( BYTE byLimit, unsigned short *pLRList )
//{
//	if( byLimit > 0 )
//	{
//		if( m_pRLInfo == NULL )
//		{
//			m_pRLInfo = (ST_GEO_RLIMIT*)m_pMemMngOfBuddyData->newmalloc(sizeof(ST_GEO_RLIMIT));
//            if(!m_pRLInfo)
//            {
//                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
//            }
//			memset( m_pRLInfo, 0, sizeof(ST_GEO_RLIMIT) );
//		}
//		m_pRLInfo->byLimit = byLimit;
//		m_pRLInfo->pLRList = (unsigned short*)m_pMemMngOfBuddyData->newmalloc(sizeof(unsigned short)*byLimit);
//        if(!m_pRLInfo->pLRList)
//        {
//            THROW(new CUserException(TRUE,EN_ET_MEMOVER));
//        }
//		memcpy( m_pRLInfo->pLRList, pLRList, sizeof(unsigned short)*byLimit );
//	}
//}
//void CGIS_NetworkRut::SetRutRLimitInfo( BYTE byLimit, unsigned short *pLRList )
//{
//	if( byLimit > 0 )
//	{
//		if( m_pRLInfo == NULL )
//		{
//			m_pRLInfo = (ST_GEO_RLIMIT*)m_pMemMngOfBuddyData->newmalloc(sizeof(ST_GEO_RLIMIT));
//            if(!m_pRLInfo)
//            {
//                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
//            }
//			memset( m_pRLInfo, 0, sizeof(ST_GEO_RLIMIT) );
//		}
//		m_pRLInfo->byRLimit = byLimit;
//		m_pRLInfo->pRLRList = (unsigned short*)m_pMemMngOfBuddyData->newmalloc(sizeof(unsigned short)*byLimit);
//        if(!m_pRLInfo->pRLRList)
//        {
//            THROW(new CUserException(TRUE,EN_ET_MEMOVER));
//        }
//		memcpy( m_pRLInfo->pRLRList, pLRList, sizeof(unsigned short)*byLimit );
//	}
//}
void CGIS_NetworkRut::SetRutVirtualInfo( BYTE byVirtual, unsigned short *pVORList, int *pVIDs )
{
//	if( m_pRVInfo )
//	{
//		if( m_pRVInfo->pVORList )
//			delete[] m_pRVInfo->pVORList;
//		if( m_pRVInfo->pVIDs )
//			delete[] m_pRVInfo->pVIDs;
//		delete m_pRVInfo;
//	}
//	m_pRVInfo = NULL;
//	if( byVirtual > 0 )
//	{
//		m_pRVInfo = new ST_GEO_RVDRAFT;
//		m_pRVInfo->byVirtual = byVirtual;
//		m_pRVInfo->pVORList = new unsigned short[byVirtual];
//		memcpy( m_pRVInfo->pVORList, pVORList, sizeof(unsigned short)*byVirtual );
//		m_pRVInfo->pVIDs = new int[byVirtual];
//		memcpy( m_pRVInfo->pVIDs, pVIDs, sizeof(int)*byVirtual );
//	}
	
//	m_byVirtual = byVirtual;	
//	if( m_pVORList )
//		delete m_pVORList;
//	m_pVORList = NULL;		
//	if( m_pVIDs )
//		delete m_pVIDs;
//	m_pVIDs = NULL;
//	if( m_byVirtual > 0 )
//	{
//		m_pVORList = new unsigned short[m_byVirtual];
//		memcpy( m_pVORList, pVORList, sizeof(unsigned short)*m_byVirtual );
//		m_pVIDs = new int[m_byVirtual];
//		memcpy( m_pVIDs, pVIDs, sizeof(int)*m_byVirtual );
//	}
}

int CGIS_NetworkRut::GetRutID( )
{
	return m_nRutID;
}
BYTE CGIS_NetworkRut::GetRutDirection( )
{
	return m_byDirection;
}
BYTE CGIS_NetworkRut::GetRutType( )
{
	return m_byRutType;
}
BYTE CGIS_NetworkRut::GetType( )
{
	return m_byType;
}
BYTE CGIS_NetworkRut::GetRutLaneNum()
{
	return 0;
//	return m_byLaneNum;
}
BYTE CGIS_NetworkRut::GetRutCharge( )
{
	return m_byCharge;
}
BYTE CGIS_NetworkRut::GetRutSpeed( )
{
	return 0;
//	return m_bySpeed;
}
BYTE CGIS_NetworkRut::GetRutLevel( )
{
	return m_byRutLevel;
}
float CGIS_NetworkRut::GetRutLength( )
{
	return m_dRLength;
}
unsigned short CGIS_NetworkRut::GetRutStartVtxID( )
{
	return m_nVtxStartID;
}
unsigned short CGIS_NetworkRut::GetRutEndVtxID( )
{
	return m_nVtxEndID;
}
int CGIS_NetworkRut::GetRBreadthID( )
{
	return m_nRBID;
}



//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE
IMemoryMng* CGIS_NetworkRut::m_pMemMngOfSelf = NULL;
IMemoryMng* CGIS_NetworkRut::m_pMemMngOfBuddyData = NULL;
//--------------------------------------
void* CGIS_NetworkRut::operator new(size_t size)
{	
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkRut 的本体内存管理器尚未初始化.\n");
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

void CGIS_NetworkRut::operator delete (void* p,size_t size)
{
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkRut 的本体内存管理器尚未初始化.\n");
        return;
    }
    m_pMemMngOfSelf->newfree(p);
}
#endif
//end
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CGIS_NetworkRut::DumpState() const	
{  
    TRACE2("ID[%d],BreadthID[%d]\n",this->m_nRutID,this->m_nRBID);
}

void CGIS_NetworkRut::AssertValid() const	
{   
    if(m_pRoadSectionLimitInfo)
    {
        ASSERTVALID(m_pRoadSectionLimitInfo);
    }

}
#endif
//////////////////////////////////////////////////////////////////////////

CGIS_NetworkRut* CGIS_NetworkRut::CreateCloneObject()
{
    CGIS_NetworkRut* pFtr = (CGIS_NetworkRut*)malloc(sizeof(CGIS_NetworkRut));
    memset(pFtr,0,sizeof(CGIS_NetworkRut));
	

    pFtr->SetRutInfo(
        m_nRutID,
        m_byDirection,
        m_byType,
        m_byCharge,
        0,
        0,
        m_byRutLevel,
        m_byRutType,
        m_dRLength,
        m_nVtxStartID,
        m_nVtxEndID,
        m_nRBID);
    

    if(m_pRoadSectionLimitInfo)
    {
        stuRoadSectionLimitInfo* pLimitInfo = new stuRoadSectionLimitInfo;
        memset(pLimitInfo,0,sizeof(stuRoadSectionLimitInfo));

        if( m_pRoadSectionLimitInfo->sGroup.byTurnCount > 0 )
        {
            pLimitInfo->sGroup.byTurnCount = m_pRoadSectionLimitInfo->sGroup.byTurnCount;
            pLimitInfo->sGroup.pTurnInfoArray = new TURNINFO[m_pRoadSectionLimitInfo->sGroup.byTurnCount];
            
            for(int i = 0; i < pLimitInfo->sGroup.byTurnCount; i++)
            {
                CGIS_NetworkRut::TURNINFO* pTurnInfoSrc = &m_pRoadSectionLimitInfo->sGroup.pTurnInfoArray[i];
                CGIS_NetworkRut::TURNINFO* pTurnInfoDest = &pLimitInfo->sGroup.pTurnInfoArray[i];
                
                pTurnInfoDest->byToRouteIDCount = pTurnInfoSrc->byToRouteIDCount;
                pTurnInfoDest->pwToRutID = (WORD*)malloc(sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
                memcpy(pTurnInfoDest->pwToRutID,pTurnInfoSrc->pwToRutID,sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
                pTurnInfoDest->byStartTime = pTurnInfoSrc->byStartTime;
                pTurnInfoDest->byEndTime = pTurnInfoSrc->byEndTime;
                
            }
        }
        if( m_pRoadSectionLimitInfo->eGroup.byTurnCount > 0 )
        {
            pLimitInfo->eGroup.byTurnCount = m_pRoadSectionLimitInfo->eGroup.byTurnCount;
            pLimitInfo->eGroup.pTurnInfoArray = new TURNINFO[m_pRoadSectionLimitInfo->eGroup.byTurnCount];
            for(int i = 0; i < pLimitInfo->eGroup.byTurnCount; i++)
            {
                CGIS_NetworkRut::TURNINFO* pTurnInfoSrc = &m_pRoadSectionLimitInfo->eGroup.pTurnInfoArray[i];
                CGIS_NetworkRut::TURNINFO* pTurnInfoDest = &pLimitInfo->eGroup.pTurnInfoArray[i];
                
                pTurnInfoDest->byToRouteIDCount = pTurnInfoSrc->byToRouteIDCount;
                pTurnInfoDest->pwToRutID = (WORD*)malloc(sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
                memcpy(pTurnInfoDest->pwToRutID,pTurnInfoSrc->pwToRutID,sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
                pTurnInfoDest->byStartTime = pTurnInfoSrc->byStartTime;
                pTurnInfoDest->byEndTime = pTurnInfoSrc->byEndTime;
                
            }
        }
        pFtr->SetLimitInfoDirect(pLimitInfo);
    }
	return pFtr;    
}
void CGIS_NetworkRut::DestoryCloneObject()
{
    if(m_pRoadSectionLimitInfo)
    {
        if( m_pRoadSectionLimitInfo->sGroup.byTurnCount > 0 )
        {
            for(int i = 0; i < m_pRoadSectionLimitInfo->sGroup.byTurnCount; i++)
            {
                free(m_pRoadSectionLimitInfo->sGroup.pTurnInfoArray[i].pwToRutID);
            }

            delete [] (m_pRoadSectionLimitInfo->sGroup.pTurnInfoArray);
        }
        if( m_pRoadSectionLimitInfo->eGroup.byTurnCount > 0)
        {
            for(int i = 0; i < m_pRoadSectionLimitInfo->eGroup.byTurnCount; i++)
            {
                free(m_pRoadSectionLimitInfo->eGroup.pTurnInfoArray[i].pwToRutID);
            }
            
            delete [] (m_pRoadSectionLimitInfo->eGroup.pTurnInfoArray);
        }

        delete (m_pRoadSectionLimitInfo);
    }

    memset(this,0,sizeof(CGIS_NetworkRut));
    free(this);

}
void CGIS_NetworkRut::SetLimitInfo(stuRoadSectionLimitInfo* pInfo)
{
    if(!pInfo)
        return;
    if(pInfo->sGroup.byTurnCount == 0 && pInfo->eGroup.byTurnCount == 0)
        return;
    
    ASSERT(pInfo->sGroup.byTurnCount || pInfo->eGroup.byTurnCount);

    if( m_pRoadSectionLimitInfo == NULL )
    {
        m_pRoadSectionLimitInfo = (stuRoadSectionLimitInfo*)m_pMemMngOfBuddyData->newmalloc(sizeof(stuRoadSectionLimitInfo));
        if(!m_pRoadSectionLimitInfo)
        {
            THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        }
        memset( m_pRoadSectionLimitInfo, 0, sizeof(stuRoadSectionLimitInfo) );
    }

    if ( pInfo->sGroup.byTurnCount > 0)
    {
        m_pRoadSectionLimitInfo->sGroup.byTurnCount = pInfo->sGroup.byTurnCount;
        m_pRoadSectionLimitInfo->sGroup.pTurnInfoArray = (CGIS_NetworkRut::TURNINFO*)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_NetworkRut::TURNINFO)*pInfo->sGroup.byTurnCount);
        for(int i = 0; i < pInfo->sGroup.byTurnCount; i++)
        {
            CGIS_NetworkRut::TURNINFO* pTurnInfoSrc = &pInfo->sGroup.pTurnInfoArray[i];
            CGIS_NetworkRut::TURNINFO* pTurnInfoDest = &m_pRoadSectionLimitInfo->sGroup.pTurnInfoArray[i];

            pTurnInfoDest->byToRouteIDCount = pTurnInfoSrc->byToRouteIDCount;
            pTurnInfoDest->pwToRutID = (WORD*)m_pMemMngOfBuddyData->newmalloc(sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
            memcpy(pTurnInfoDest->pwToRutID,pTurnInfoSrc->pwToRutID,sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
            pTurnInfoDest->byStartTime = pTurnInfoSrc->byStartTime;
            pTurnInfoDest->byEndTime = pTurnInfoSrc->byEndTime;

        }
    }
    if ( pInfo->eGroup.byTurnCount > 0)
    {
        m_pRoadSectionLimitInfo->eGroup.byTurnCount = pInfo->eGroup.byTurnCount;
        m_pRoadSectionLimitInfo->eGroup.pTurnInfoArray = (CGIS_NetworkRut::TURNINFO*)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_NetworkRut::TURNINFO)*pInfo->eGroup.byTurnCount);
        for(int i = 0; i < pInfo->eGroup.byTurnCount; i++)
        {
            CGIS_NetworkRut::TURNINFO* pTurnInfoSrc = &pInfo->eGroup.pTurnInfoArray[i];
            CGIS_NetworkRut::TURNINFO* pTurnInfoDest = &m_pRoadSectionLimitInfo->eGroup.pTurnInfoArray[i];

            pTurnInfoDest->byToRouteIDCount = pTurnInfoSrc->byToRouteIDCount;
            pTurnInfoDest->pwToRutID = (WORD*)m_pMemMngOfBuddyData->newmalloc(sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
            memcpy(pTurnInfoDest->pwToRutID,pTurnInfoSrc->pwToRutID,sizeof(pTurnInfoDest->pwToRutID[0])*pTurnInfoSrc->byToRouteIDCount);
            pTurnInfoDest->byStartTime = pTurnInfoSrc->byStartTime;
            pTurnInfoDest->byEndTime = pTurnInfoSrc->byEndTime;

        }
    }    
}


void CGIS_NetworkRut::stuRoadSectionLimitInfo::Format(BOOL bHead,int nTimeZone,swString& strText)
{

        if(bHead)
        {   
            sGroup.Format(nTimeZone,strText);
        }
        else
        {
            eGroup.Format(nTimeZone,strText);
        }
}

void CGIS_NetworkRut::stuRoadSectionLimitInfoGroup::Format(int nTimeZone,swString& strText)
{  
    swString strTemp;
    for(int i = 0; i < byTurnCount; i++)
    {
        if(pTurnInfoArray[i].byStartTime == 0xff)
        {
            ASSERT(pTurnInfoArray[i].byEndTime = 0xff);
            swString strIDArray;
            pTurnInfoArray[i].Format(strIDArray);
            strTemp.Format(_T("\n     %s(------------)"),strIDArray);
        }
        else
        {
            ASSERT(pTurnInfoArray[i].byStartTime < 144);
            swString strIDArray;
            pTurnInfoArray[i].Format(strIDArray);
           
            strTemp.Format(_T("\n     %s (%.2d:%.2d--%.2d:%.2d)"),
                strIDArray,
                (pTurnInfoArray[i].byStartTime/6+nTimeZone)%24,
                pTurnInfoArray[i].byStartTime*10%60,
                (pTurnInfoArray[i].byEndTime/6+nTimeZone)%24,
                pTurnInfoArray[i].byEndTime*10%60);
            
        }
        strText += strTemp;
    }
}


//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG

void CGIS_NetworkRut::stuRoadSectionLimitInfo::AssertValid() 
{
    ASSERT(this);
    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup = GetGroup(TRUE);
    if(pLimitInfoGroup)
        ASSERTVALID(pLimitInfoGroup);
    
    pLimitInfoGroup = GetGroup(FALSE);
    if(pLimitInfoGroup)
        ASSERTVALID(pLimitInfoGroup);
}
#endif
//////////////////////////////////////////////////////////////////////////	