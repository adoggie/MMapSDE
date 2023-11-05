
#include "GIS_FeaturePoly.h"

CGIS_FeaturePoly::CGIS_FeaturePoly(EnGeoObjType enOType)
	: CGIS_Feature( enOType ){
	m_pPart = NULL;
	m_nPartNum = 0;
	m_nPtNum = 0;
	m_nShowClass = 0;
}

CGIS_FeaturePoly::~CGIS_FeaturePoly(){
    switch(m_nMemmoryType){
    case 0:
        {
            if( m_pPart )
            {
                free(m_pPart);
                m_pPart = NULL;
            }   
        }
        break;
    case 1:
        {
            if( m_pPart )
            {
                m_pMemMngOfBuddyData->newfree(m_pPart);
                m_pPart = NULL;
            }   
        }
        break;
    case 2:
        {
            if( m_pPart )
            {
                m_pForQueryMemMngOfBuddyData->newfree(m_pPart);
                m_pPart = NULL;
            }   
        }
        break;
    case 3:
        {
            if( m_pPart )
            {
                m_pForNetworkMemMngOfBuddyData->newfree(m_pPart);
                m_pPart = NULL;
            }   
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }
}

int* CGIS_FeaturePoly::GetPart( ){
	return m_pPart;
}

int  CGIS_FeaturePoly::GetPartNum( ){
	return m_nPartNum;
}

void CGIS_FeaturePoly::SetRectObj( ST_GEO_RECT rtObj ){
	//memcpy(&m_rtObj, &rtObj, sizeof(ST_GEO_RECT));
	m_rtObj = rtObj;
}
ST_GEO_RECT CGIS_FeaturePoly::GetRectObj( )
{
	return m_rtObj;
}
void CGIS_FeaturePoly::SetCenterPt( float dx, float dy )
{
	m_CenterPt.dx = dx;
	m_CenterPt.dy = dy;
}
ST_GEO_PTXY CGIS_FeaturePoly::GetCenterPt()
{
	return m_CenterPt;
}
void CGIS_FeaturePoly::SetPolyShowClass( BYTE nShowClass )
{
	m_nShowClass = nShowClass;
}
BYTE CGIS_FeaturePoly::GetPolyShowClass( )
{
	return m_nShowClass;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BOOL CGIS_FeaturePoly::SetPartInfo(int nLength,int* pList)
{
    ASSERT(m_pMemMngOfBuddyData);
    ASSERT(m_pForQueryMemMngOfBuddyData);
    ////////////////////////////////////


    switch(m_nMemmoryType)
    {
    case 0:
        {
            SetPartInfoAt(NULL,nLength,pList);
        }
        break;
    case 1:
        {
            SetPartInfoAt(m_pMemMngOfBuddyData,nLength,pList);
        }
        break;
    case 2:
        {
            SetPartInfoAt(m_pForQueryMemMngOfBuddyData,nLength,pList);
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }

    return TRUE;
}

BOOL CGIS_FeaturePoly::SetPartInfoAt(IMemoryMng* pMemoryMng,int nLength,int* pList)
{
    ////////////////////////////////////

    //旧的数据空间一定不存在
    ASSERT(!m_pPart);
    
    //分配新的数据空间
    int nByteLength = sizeof(int)*nLength;
    char* pBuf = NULL;
    if(nByteLength)
    {
        if(!pMemoryMng)
        {
            pBuf = (char*)malloc(nByteLength);
        }
        else
        {
            pBuf = (char*)pMemoryMng->newmalloc(nByteLength);
        }
        ASSERT(pBuf);
        memcpy(pBuf,pList,nByteLength);   
    }
    //////////////////////////////////////////////////////////////////////////
    m_pPart = (int*)pBuf;
    m_nPartNum = nLength;
    return TRUE;
}

void CGIS_FeaturePoly::SetPartInfoDirect(short nLength,int* pList)
{
    m_pPart = pList;
    m_nPartNum = nLength;
}

CGIS_Feature* CGIS_FeaturePoly::Clone()
{
    ASSERT(m_pForQueryMemMngOfFeaturePoly);
    CGIS_FeaturePoly* pFtrAddr = (CGIS_FeaturePoly*)malloc(sizeof(CGIS_FeaturePoly));
    if(!pFtrAddr)
        return NULL;

	CGIS_FeaturePoly* pFtr = new (pFtrAddr) CGIS_FeaturePoly(m_enOType);
    pFtr->m_nMemmoryType = 0;

    TRY
    {
        pFtr->SetPartInfo(m_nPartNum,m_pPart);
        pFtr->SetPtInfo(m_nPtNum,m_pPtList);
        pFtr->SetAnnotationInfo(m_nAnno,m_bstrAnno);
    }
    CATCH(CUserException,e)
    {
        delete pFtr;
        return NULL;
    }
    END_CATCH

	pFtr->m_nBreadthID = m_nBreadthID;
	pFtr->SetRectObj( m_rtObj );
    pFtr->SetCenterPt( m_CenterPt.dx, m_CenterPt.dy );

	return pFtr;
}

CGIS_Feature* CGIS_FeaturePoly::CloneV2()
{
    ASSERT(m_pForQueryMemMngOfFeaturePoly);
    CGIS_FeaturePoly* pFtrAddr = (CGIS_FeaturePoly*)m_pForQueryMemMngOfFeaturePoly->newmalloc(sizeof(CGIS_FeaturePoly));
	CGIS_FeaturePoly* pFtr = new (pFtrAddr) CGIS_FeaturePoly(m_enOType);
    pFtr->m_nMemmoryType = 2;

	pFtr->SetPartInfo(m_nPartNum,m_pPart);
    pFtr->SetPtInfo(m_nPtNum,m_pPtList);
    pFtr->SetAnnotationInfo(m_nAnno,m_bstrAnno);

	pFtr->m_nBreadthID = m_nBreadthID;
	pFtr->SetRectObj( m_rtObj );
    pFtr->SetCenterPt( m_CenterPt.dx, m_CenterPt.dy );

	return pFtr;
}
void CGIS_FeaturePoly::SetPolyID( int nID )
{
	m_nID = nID;
}
int CGIS_FeaturePoly::GetPolyID()
{
	return m_nID;
}

//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE

//--------------------------------------
void* CGIS_FeaturePoly::operator new(size_t size)
{	
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_Feature 的本体内存管理器尚未初始化.\n");
		THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        return NULL;
    }   
    void* pRtn = m_pMemMngOfFeaturePoly->newmalloc(size);
	if(!pRtn)
	{
        THROW(new CUserException(TRUE,EN_ET_MEMOVER));
	}
    return pRtn;
}
void* CGIS_FeaturePoly::operator new(size_t size,CGIS_FeaturePoly* pObjAddr)
{
	if(!pObjAddr)
	{
		THROW(new CUserException(TRUE,EN_ET_MEMOVER));
	}
	
    return pObjAddr;
}
void* CGIS_FeaturePoly::operator new(size_t size,IMemoryMng2* pMemoryMng)
{
    
    void* pRtn = pMemoryMng->newmalloc();
    
	if(!pRtn)
	{
        if(CGIS_Feature::m_dwOverflowTickCount != 0xffffffff)
        {
            if((GetTickCount() - CGIS_Feature::m_dwOverflowTickCount) > 300000)
            {
                CGIS_Feature::m_dwOverflowTickCount = 0xffffffff;
		        THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }

            pRtn = new char[size];
        }
        else
        {
            CGIS_Feature::m_dwOverflowTickCount = GetTickCount();
            pRtn = new char[size];
        }
	}

    return pRtn;
}
void CGIS_FeaturePoly::operator delete (void* p,size_t size)
{
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_Feature 的本体内存管理器尚未初始化.\n");
        return;
    }

    //////////////////////////////////////////////////////////////////////////
	CGIS_Feature* pFtr = (CGIS_Feature*)p;
    switch(pFtr->m_nMemmoryType)
    {
    case 0:
        {
            free(p);
        }
        break;
    case 1:
        {
            m_pMemMngOfFeaturePoly->newfree(p);
        }
        break;
    case 2:
        {
            m_pForQueryMemMngOfFeaturePoly->newfree(p);
        }
        break;
    case 3:
        {
            m_pForNetworkMemMngOfFeaturePoly->newfree(p);
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    } 
}
#endif
//end
//////////////////////////////////////////////////////////////////////////
