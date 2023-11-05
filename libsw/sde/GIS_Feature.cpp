
#include "stdafx.h"
#include "SDELib.h"
//////////////////////////////////////////////////////////////////////////
#include "./MemoryMngLib/IMemoryMng.h"
#include "./MemoryMngLib/IMemoryMng2.h"
//////////////////////////////////////////////////////////////////////////

#include "GIS_Feature.h"

//////////////////////////////////////////////////////////////////////////
#include "GIS_FeaturePoint.h"
#include "GIS_FeatureLine.h"
#include "GIS_FeaturePoly.h"
#include "GIS_FeatureRut.h"

//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_Feature::CGIS_Feature(EnGeoObjType enOType)
{
	m_enOType =  enOType;
    
	m_pPtList = NULL;
	m_nBreadthID = 0;
	m_bstrAnno = NULL;
	m_nAnno = 0;

#ifdef MEMORY_SELF_MANAGE
	m_nMemmoryType = 1;
#endif

	// 增加对成员变量的初始化 20070326
	m_nPtNum = 0;
}

CGIS_Feature::~CGIS_Feature()
{
    ////////////////////////////////////
	ASSERT( 
        m_enOType == EN_GOTYPE_UPOINT   ||
		m_enOType == EN_GOTYPE_ULINE    || 
		m_enOType == EN_GOTYPE_UPOLY    || 
		m_enOType == EN_GOTYPE_URUT	    ||
		m_enOType == EN_GOTYPE_ROUTE    || 
		m_enOType == EN_GOTYPE_TRACK    ||
		m_enOType == EN_GOTYPE_ADDRESS  || 
		m_nBreadthID >=	 0 );
	
    //////////////////////////////////////////////////////////////////////////  

    switch(m_nMemmoryType)
    {
    case 0:
        {
            if( m_pPtList )
            {
                free(m_pPtList);
                m_pPtList = NULL;
            }
            if( m_bstrAnno )
            {
                free(m_bstrAnno);
                m_bstrAnno = NULL;
            }
        }
        break;
    case 1:
        {
            if( m_pPtList )
            {
                ASSERT(m_pMemMngOfBuddyData);
                m_pMemMngOfBuddyData->newfree(m_pPtList);
                m_pPtList = NULL;
            }
            if( m_bstrAnno )
            {
                ASSERT(m_pMemMngOfBuddyData);
                m_pMemMngOfBuddyData->newfree(m_bstrAnno);
                m_bstrAnno = NULL;
            }      
        }
        break;
    case 2:
        {
            if( m_pPtList )
            {
                ASSERT(m_pForQueryMemMngOfBuddyData);
                m_pForQueryMemMngOfBuddyData->newfree(m_pPtList);
                m_pPtList = NULL;
            }
            if( m_bstrAnno )
            {
                ASSERT(m_pForQueryMemMngOfBuddyData);
                m_pForQueryMemMngOfBuddyData->newfree(m_bstrAnno);
                m_bstrAnno = NULL;
            }
        }
        break;
    case 3:
        {
            if( m_pPtList )
            {
                ASSERT(m_pForNetworkMemMngOfBuddyData);
                m_pForNetworkMemMngOfBuddyData->newfree(m_pPtList);
                m_pPtList = NULL;
            }
            if( m_bstrAnno )
            {
                ASSERT(m_pForNetworkMemMngOfBuddyData);
                m_pForNetworkMemMngOfBuddyData->newfree(m_bstrAnno);
                m_bstrAnno = NULL;
            }
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    
	m_pPtList       = NULL;
	m_nBreadthID    = 0;
	m_bstrAnno      = NULL;
	m_nAnno         = 0;
}

BOOL CGIS_Feature::SetAnnotationInfo(int nLength,char* strText)
{
    ASSERT(m_pMemMngOfBuddyData);
    ASSERT(m_pForQueryMemMngOfBuddyData);
    ////////////////////////////////////

    switch(m_nMemmoryType)
    {
    case 0:
        {
            SetAnnotationInfoAt(0,nLength,strText);            
        }
        break;
    case 1:
        {
            SetAnnotationInfoAt(m_pMemMngOfBuddyData,nLength,strText);
        }
        break;
    case 2:
        {
            SetAnnotationInfoAt(m_pForQueryMemMngOfBuddyData,nLength,strText);
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }

    return TRUE;
}

//设置标注信息
BOOL CGIS_Feature::SetAnnotationInfoAt(IMemoryMng* pMemoryMng, int nLength,char* strText)
{
    ////////////////////////////////////

    //释放旧的数据空间
    ASSERT(!m_bstrAnno);
 

    //分配新的数据空间
    int nByteLength = sizeof(char)*nLength;
    char* pBuf = NULL;
    if(nByteLength)
    {
        ASSERT(nLength-1 == strlen(strText));
        if(!pMemoryMng) //从系统堆上分配
        {
            pBuf = (char*)malloc(nByteLength);
        }
        else
        {
            pBuf = (char*)pMemoryMng->newmalloc(nByteLength);
        }
		if(!pBuf)
		{
			THROW(new CUserException(TRUE,EN_ET_MEMOVER));			
		}
        ASSERT(pBuf);
        memcpy(pBuf,strText,nByteLength);
//        pBuf[nByteLength] = 0;
    }

   
    //////////////////////////////////////////////////////////////////////////
    m_bstrAnno = pBuf;
    m_nAnno = nLength;
    return TRUE;
}

BOOL CGIS_Feature::SetPtInfo(int nLength,ST_GEO_PTXY* pList)
{
    ASSERT(m_pMemMngOfBuddyData);
    ASSERT(m_pForQueryMemMngOfBuddyData);
    ////////////////////////////////////


    switch(m_nMemmoryType)
    {
    case 0:
        {
            SetPtInfoAt(NULL,nLength,pList);
        }
        break;
    case 1:
        {
            SetPtInfoAt(m_pMemMngOfBuddyData,nLength,pList);
        }
        break;
    case 2:
        {
            SetPtInfoAt(m_pForQueryMemMngOfBuddyData,nLength,pList);
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }

    return TRUE;
}
BOOL CGIS_Feature::SetPtInfoAt(IMemoryMng* pMemoryMng,int nLength,ST_GEO_PTXY* pList)
{
    ////////////////////////////////////

    //旧的数据空间一定不存在
    ASSERT(!m_pPtList);
     
    //分配新的数据空间
    int nByteLength = sizeof(ST_GEO_PTXY)*nLength;
    char* pBuf = NULL;
    if(nByteLength)
    {
        if(!pMemoryMng)
        {
            pBuf = (char*)malloc(nByteLength); //系统堆分配
        }
        else
        {
            pBuf = (char*)pMemoryMng->newmalloc(nByteLength);
        }
        if(!pBuf)
		{
			THROW(new CUserException(TRUE,EN_ET_MEMOVER));			
		}
        ASSERT(pBuf);
        memcpy(pBuf,pList,nByteLength);   
    }
        
    //////////////////////////////////////////////////////////////////////////
    m_pPtList = (ST_GEO_PTXY*)pBuf;
    m_nPtNum = nLength;
    return TRUE;
}


//直接赋值 
void CGIS_Feature::SetAnnotationInfoDirect(int nLength,char* strText)
{
    m_bstrAnno = strText;
    m_nAnno = nLength;
    
}
void CGIS_Feature::SetPtInfoDirect(int nLength,ST_GEO_PTXY* pList)
{
    m_pPtList = pList;
    m_nPtNum = nLength;
}



//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE
IMemoryMng* CGIS_Feature::m_pMemMngOfSelf          = NULL;
IMemoryMng* CGIS_Feature::m_pMemMngOfFeaturePoint  = NULL;
IMemoryMng* CGIS_Feature::m_pMemMngOfFeatureLine   = NULL;
IMemoryMng* CGIS_Feature::m_pMemMngOfFeaturePoly   = NULL;
IMemoryMng* CGIS_Feature::m_pMemMngOfFeatureRut    = NULL;
IMemoryMng*  CGIS_Feature::m_pMemMngOfBuddyData     = NULL;

IMemoryMng* CGIS_Feature::m_pForQueryMemMngOfSelf              = NULL;
IMemoryMng* CGIS_Feature::m_pForQueryMemMngOfFeaturePoint      = NULL;
IMemoryMng* CGIS_Feature::m_pForQueryMemMngOfFeatureLine       = NULL;
IMemoryMng* CGIS_Feature::m_pForQueryMemMngOfFeaturePoly       = NULL;
IMemoryMng* CGIS_Feature::m_pForQueryMemMngOfFeatureRut        = NULL;
IMemoryMng*  CGIS_Feature::m_pForQueryMemMngOfBuddyData         = NULL;

IMemoryMng* CGIS_Feature::m_pForNetworkMemMngOfSelf            = NULL;
IMemoryMng* CGIS_Feature::m_pForNetworkMemMngOfFeaturePoint    = NULL;
IMemoryMng* CGIS_Feature::m_pForNetworkMemMngOfFeatureLine     = NULL;
IMemoryMng* CGIS_Feature::m_pForNetworkMemMngOfFeaturePoly     = NULL;
IMemoryMng* CGIS_Feature::m_pForNetworkMemMngOfFeatureRut      = NULL;
IMemoryMng*  CGIS_Feature::m_pForNetworkMemMngOfBuddyData       = NULL;
//--------------------------------------
DWORD CGIS_Feature::m_dwOverflowTickCount = 0xffffffff;
#endif
//end
//////////////////////////////////////////////////////////////////////////