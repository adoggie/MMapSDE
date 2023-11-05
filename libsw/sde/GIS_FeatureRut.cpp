// GIS_FeatureRut.cpp: implementation of the CGIS_FeatureRut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//////////////////////////////////////////////////////////////////////////
#include "./MemoryMngLib/IMemoryMng.h"
#include "./MemoryMngLib/IMemoryMng2.h"
//////////////////////////////////////////////////////////////////////////
#include "GIS_FeatureRut.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureRut::CGIS_FeatureRut(EnGeoObjType enOType)
	: CGIS_Feature( enOType )
{
	m_nRutID = 0;
//	m_nPartNum = 0;
	m_nPtNum = 0;
// 	m_pPart = NULL;
	
	// 增加成员变量的初始化 20070326
	::memset(&m_rtObj, 0, sizeof(ST_GEO_RECT));
	m_byDirection = 0;

	m_bySignInfoCount = 0;
	m_byImgInfoCount = 0;
	m_byShunXingCount = 0;

	m_pImgInfo = NULL;
	m_pSignInfo = NULL;

	//shunxing
	m_pwShunXingOutRutID = NULL;
    m_dwLaneInfoOffset = 0;

}

CGIS_FeatureRut::~CGIS_FeatureRut()
{
	m_nRutID = 0;
    switch(m_nMemmoryType)
    {
    case 0:
        {
            if( m_pImgInfo )
            {
                free(m_pImgInfo);
                m_pImgInfo = NULL;
            }
            if( m_pSignInfo )
            {
                for(int i = 0; i < m_bySignInfoCount; i++)
                {
                    if(m_pSignInfo[i].pszName)
                    {
                        free(m_pSignInfo[i].pszName);
                    }
                }
                free(m_pSignInfo);
                m_pSignInfo = NULL;
            }

        }
        break;
    case 1:
        {
            if( m_pImgInfo )
            {
                ASSERT(m_pMemMngOfBuddyData);
                m_pMemMngOfBuddyData->newfree(m_pImgInfo);
                m_pImgInfo = NULL;
            }
            if( m_pSignInfo )
            {
                ASSERT(m_pMemMngOfBuddyData);
                for(int i = 0; i < m_bySignInfoCount; i++)
                {
                    if(m_pSignInfo[i].pszName)
                    {
                        m_pMemMngOfBuddyData->newfree(m_pSignInfo[i].pszName);
                    }
                }
                m_pMemMngOfBuddyData->newfree(m_pSignInfo);
                m_pSignInfo = NULL;
            }      

        }
        break;
    case 2:
        {
            if( m_pImgInfo )
            {
                ASSERT(m_pForQueryMemMngOfBuddyData);
                m_pForQueryMemMngOfBuddyData->newfree(m_pImgInfo);
                m_pImgInfo = NULL;
            }
            if( m_pSignInfo )
            {
                ASSERT(m_pForQueryMemMngOfBuddyData);
                for(int i = 0; i < m_bySignInfoCount; i++)
                {
                    if(m_pSignInfo[i].pszName)
                    {
                        m_pForQueryMemMngOfBuddyData->newfree(m_pSignInfo[i].pszName);
                    }
                }
                m_pForQueryMemMngOfBuddyData->newfree(m_pSignInfo);
                m_pSignInfo = NULL;
            }

        }
        break;
    case 3:
        {
            if( m_pImgInfo )
            {
                ASSERT(m_pForNetworkMemMngOfBuddyData);
                m_pForNetworkMemMngOfBuddyData->newfree(m_pImgInfo);
                m_pImgInfo = NULL;
            }
            if( m_pSignInfo )
            {
                ASSERT(m_pForNetworkMemMngOfBuddyData);
                for(int i = 0; i < m_bySignInfoCount; i++)
                {
                    if(m_pSignInfo[i].pszName)
                    {
                        m_pForNetworkMemMngOfBuddyData->newfree(m_pSignInfo[i].pszName);
                    }
                }
                m_pForNetworkMemMngOfBuddyData->newfree(m_pSignInfo);
                m_pSignInfo = NULL;
            }

        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    
	m_pImgInfo       = NULL;
	m_byImgInfoCount    = 0;
	m_pSignInfo      = NULL;
	m_bySignInfoCount         = 0;

    m_dwLaneInfoOffset = 0;

}
//void CGIS_FeatureRut::SetPartNum( int nPartNum )
//{
//	m_nPartNum = nPartNum;
//}
//int  CGIS_FeatureRut::GetPartNum( )
//{
//	return m_nPartNum;
//}

//void CGIS_FeatureRut::SetPart( int *pPart )
//{
//	m_pPart = pPart;
//}
//int* CGIS_FeatureRut::GetPart( )
//{
//	return m_pPart;
//}
void CGIS_FeatureRut::SetRectObj( ST_GEO_RECT rtObj )
{
	memcpy(&m_rtObj, &rtObj, sizeof(ST_GEO_RECT));
//	m_rtObj = rtObj;
}
ST_GEO_RECT CGIS_FeatureRut::GetRectObj( )
{
	return m_rtObj;
}
int CGIS_FeatureRut::GetRutID( )
{
	return m_nRutID;
}
void CGIS_FeatureRut::SetRutID( int nRutID )
{
	m_nRutID = nRutID;
}
BYTE CGIS_FeatureRut::GetRutDirection( )
{
	return m_byDirection;
}
void CGIS_FeatureRut::SetRutDirection( BYTE byDirection )
{
	m_byDirection = byDirection;
}
CGIS_Feature* CGIS_FeatureRut::Clone()
{
    ASSERT(m_pForQueryMemMngOfFeatureRut);
    CGIS_FeatureRut* pFtrAddr = (CGIS_FeatureRut*)malloc(sizeof(CGIS_FeatureRut));
    if(!pFtrAddr)
        return NULL;

	CGIS_FeatureRut* pFtr = new (pFtrAddr) CGIS_FeatureRut(m_enOType);
    pFtr->m_nMemmoryType = 0;

    TRY
    {
        pFtr->SetPtInfo(m_nPtNum,m_pPtList);
        pFtr->SetAnnotationInfo(m_nAnno,m_bstrAnno);
        pFtr->SetImgInfo(m_byImgInfoCount,m_pImgInfo);
        pFtr->SetSignInfo(m_bySignInfoCount,m_pSignInfo);
        pFtr->SetLaneInfoOffset(m_dwLaneInfoOffset);
    }
    CATCH(CUserException,e)
    {
        delete pFtr;
        return NULL;
    }
    END_CATCH

	pFtr->m_nBreadthID = m_nBreadthID;
	pFtr->SetRectObj( m_rtObj );
	pFtr->SetRutID( m_nRutID );
	pFtr->SetRutDirection( m_byDirection );

	return pFtr;
}
CGIS_Feature* CGIS_FeatureRut::CloneV2()
{
    ASSERT(m_pForQueryMemMngOfFeatureRut);
    CGIS_FeatureRut* pFtrAddr = (CGIS_FeatureRut*)m_pForQueryMemMngOfFeatureRut->newmalloc(sizeof(CGIS_FeatureRut));
	CGIS_FeatureRut* pFtr = new (pFtrAddr) CGIS_FeatureRut(m_enOType);
    pFtr->m_nMemmoryType = 2;

	pFtr->SetPtInfo(m_nPtNum,m_pPtList);
    pFtr->SetAnnotationInfo(m_nAnno,m_bstrAnno);

	pFtr->m_nBreadthID = m_nBreadthID;
	pFtr->SetRectObj( m_rtObj );
	pFtr->SetRutID( m_nRutID );
	pFtr->SetRutDirection( m_byDirection );

	return pFtr;
}

#ifdef MEMORY_SELF_MANAGE

//--------------------------------------
void* CGIS_FeatureRut::operator new(size_t size)
{	
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_Feature 的本体内存管理器尚未初始化.\n");
		THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        return NULL;
    }       
	void* pRtn = m_pMemMngOfFeatureRut->newmalloc(size);
	if(!pRtn)
	{
        THROW(new CUserException(TRUE,EN_ET_MEMOVER));
	}
    return pRtn;
}
void* CGIS_FeatureRut::operator new(size_t size,CGIS_FeatureRut* pObjAddr)
{
	if(!pObjAddr)
	{
		THROW(new CUserException(TRUE,EN_ET_MEMOVER));
	}
    return pObjAddr;
}
void* CGIS_FeatureRut::operator new(size_t size,IMemoryMng2* pMemoryMng)
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
void CGIS_FeatureRut::operator delete (void* p,size_t size)
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
            m_pMemMngOfFeatureRut->newfree(p);
        }
        break;
    case 2:
        {
            m_pForQueryMemMngOfFeatureRut->newfree(p);
        }
        break;
    case 3:
        {
            m_pForNetworkMemMngOfFeatureRut->newfree(p);
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
void CGIS_FeatureRut::SetImgInfoDirect(BYTE nLength,LPIMGINFO pArray)
{
    m_byImgInfoCount = nLength;
    m_pImgInfo = pArray;
}
void CGIS_FeatureRut::SetSignInfoDirect(BYTE nLength,LPSIGNINFO pArray)
{
    m_bySignInfoCount = nLength;
    m_pSignInfo = pArray;
}


//////////////////////////////////////////////////////////////////////////
CGIS_FeatureRut::LPIMGINFO CGIS_FeatureRut::GetImgInfoByExitRoadSectionID(unsigned short nID)
{
    int nCount = GetImgInfoCount();
    for(int i = 0; i < nCount; i++)
    {
        if(m_pImgInfo[i].wOutRutID == nID)
            return &m_pImgInfo[i];
    }
    return NULL;
}


CGIS_FeatureRut::LPSIGNINFO CGIS_FeatureRut::GetSignInfoByExitRoadSectionID(unsigned short nID)
{
    int nCount = GetSignInfoCount();
    for(int i = 0; i < nCount; i++)
    {
        if(m_pSignInfo[i].wOutRutID == nID)
            return &m_pSignInfo[i];
    }
    return NULL;
}
//////////////////////////////////////////////////////////////////////////



BOOL CGIS_FeatureRut::SetImgInfo(BYTE nLength,LPIMGINFO pArray)
{
    ASSERT(m_pMemMngOfBuddyData);
    ASSERT(m_pForQueryMemMngOfBuddyData);
    ////////////////////////////////////


    switch(m_nMemmoryType)
    {
    case 0:
        {
            SetImgInfoAt(NULL,nLength,pArray);
        }
        break;
    case 1:
        {
            SetImgInfoAt(m_pMemMngOfBuddyData,nLength,pArray);
        }
        break;
    case 2:
        {
            SetImgInfoAt(m_pForQueryMemMngOfBuddyData,nLength,pArray);
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }

    return TRUE;
}
BOOL CGIS_FeatureRut::SetImgInfoAt(IMemoryMng* pMemoryMng,int nLength,LPIMGINFO pArray)
{
    ////////////////////////////////////

    //旧的数据空间一定不存在
    ASSERT(!m_pImgInfo);
     
    //分配新的数据空间
    int nByteLength = sizeof(IMGINFO)*nLength;
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
        if(!pBuf)
		{
			THROW(new CUserException(TRUE,EN_ET_MEMOVER));			
		}
        ASSERT(pBuf);
        memcpy(pBuf,pArray,nByteLength);   
    }
        
    //////////////////////////////////////////////////////////////////////////
    m_pImgInfo = (IMGINFO*)pBuf;
    m_byImgInfoCount = nLength;
    return TRUE;
}
BOOL CGIS_FeatureRut::SetSignInfo(BYTE nLength,LPSIGNINFO pArray)
{
    ASSERT(m_pMemMngOfBuddyData);
    ASSERT(m_pForQueryMemMngOfBuddyData);
    ////////////////////////////////////


    switch(m_nMemmoryType)
    {
    case 0:
        {
            SetSignInfoAt(NULL,nLength,pArray);
        }
        break;
    case 1:
        {
            SetSignInfoAt(m_pMemMngOfBuddyData,nLength,pArray);
        }
        break;
    case 2:
        {
            SetSignInfoAt(m_pForQueryMemMngOfBuddyData,nLength,pArray);
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }

    return TRUE;
}
BOOL CGIS_FeatureRut::SetSignInfoAt(IMemoryMng* pMemoryMng,int nLength,LPSIGNINFO pArray)
{
    ////////////////////////////////////

    //旧的数据空间一定不存在
    ASSERT(!m_pSignInfo);
     
    //分配新的数据空间
    int nByteLength = sizeof(SIGNINFO)*nLength;
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
        if(!pBuf)
		{
			THROW(new CUserException(TRUE,EN_ET_MEMOVER));			
		}
        ASSERT(pBuf);
        memcpy(pBuf,pArray,nByteLength);   
    }
        
    
    //////////////////////////////////////////////////////////////////////////
    m_pSignInfo = (SIGNINFO*)pBuf;
    m_bySignInfoCount = nLength;

    memset(pBuf,0,nByteLength);   

    {
        for(int i = 0; i < nLength; i++)
        {
            m_pSignInfo[i].wOutRutID = pArray[i].wOutRutID;
            m_pSignInfo[i].byNameLen = pArray[i].byNameLen;
            if(pArray[i].byNameLen)
            {
                nByteLength = sizeof(BYTE)*pArray[i].byNameLen;
                if(!pMemoryMng)
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

				memcpy(pBuf,pArray[i].pszName,nByteLength);
                m_pSignInfo[i].pszName = pBuf;
            }
        }
    }




    return TRUE;
}

/*
*/
void CGIS_FeatureRut::ReadLaneInfo(BYTE * pBuf/*in*/, DWORD dwOffset/*in*/, LPLANEINFO pLaneInfo/*out*/)
{
    if (dwOffset <= 0)
        return ;

	pBuf = pBuf + dwOffset;

	//start lane
	memcpy(&(pLaneInfo->byStartLaneNumInfo), pBuf, sizeof(pLaneInfo->byStartLaneNumInfo));
	pBuf += sizeof(pLaneInfo->byStartLaneNumInfo);
	
	if ((pLaneInfo->byStartLaneNumInfo.bit4LaneNum) > 0)
	{
		pLaneInfo->pbyStartLaneDir = new BYTE[pLaneInfo->byStartLaneNumInfo.bit4LaneNum];
		memcpy(pLaneInfo->pbyStartLaneDir, pBuf, sizeof(pLaneInfo->pbyStartLaneDir[0]) * (pLaneInfo->byStartLaneNumInfo.bit4LaneNum));
		pBuf += sizeof(pLaneInfo->pbyStartLaneDir[0]) * (pLaneInfo->byStartLaneNumInfo.bit4LaneNum);
	}

	if (pLaneInfo->byStartLaneNumInfo.bit4OutLinkNum > 0)
	{
		pLaneInfo->lpStartLaneForOutID = new LANEFOROUTID[pLaneInfo->byStartLaneNumInfo.bit4OutLinkNum];
		int i;
		for (i = 0; i < pLaneInfo->byStartLaneNumInfo.bit4OutLinkNum; i++)
		{
			memcpy(&(pLaneInfo->lpStartLaneForOutID[i].wOutLinkID), pBuf, sizeof(pLaneInfo->lpStartLaneForOutID[i].wOutLinkID));
			pBuf += sizeof(pLaneInfo->lpStartLaneForOutID[i].wOutLinkID);
			pLaneInfo->lpStartLaneForOutID[i].pbyLaneDir = new BYTE[pLaneInfo->byStartLaneNumInfo.bit4LaneNum];//注意此处的num值不是bit4OutLinkNum
			memcpy(pLaneInfo->lpStartLaneForOutID[i].pbyLaneDir, pBuf, sizeof(pLaneInfo->lpStartLaneForOutID[i].pbyLaneDir[0]) * pLaneInfo->byStartLaneNumInfo.bit4LaneNum);
			pBuf += sizeof(pLaneInfo->lpStartLaneForOutID[i].pbyLaneDir[0]) * pLaneInfo->byStartLaneNumInfo.bit4LaneNum;
		}

		//for check data
		for (i = 0; i < pLaneInfo->byStartLaneNumInfo.bit4LaneNum; i++)
		{
			BYTE byLaneDir = 0;
			for (int j = 0; j < pLaneInfo->byStartLaneNumInfo.bit4OutLinkNum; j++)
			{
				byLaneDir = byLaneDir | pLaneInfo->lpStartLaneForOutID[j].pbyLaneDir[i];
			}
			ASSERT(byLaneDir == pLaneInfo->pbyStartLaneDir[i]);
			
			int nLaneDirCount = 0;
			nLaneDirCount = (pLaneInfo->pbyStartLaneDir[i] & LANEINFO::SlightLeft) + 
							((pLaneInfo->pbyStartLaneDir[i] & LANEINFO::Left) >> 1) + 
							((pLaneInfo->pbyStartLaneDir[i] & LANEINFO::SharpLeft) >> 2) + 
							((pLaneInfo->pbyStartLaneDir[i] & LANEINFO::UTurn) >> 3) + 
							((pLaneInfo->pbyStartLaneDir[i] & LANEINFO::SharpRight) >> 4) + 
							((pLaneInfo->pbyStartLaneDir[i] & LANEINFO::Right) >> 5) + 
							((pLaneInfo->pbyStartLaneDir[i] & LANEINFO::SlightRight) >> 6)+ 
							((pLaneInfo->pbyStartLaneDir[i] & LANEINFO::Straight) >> 7);
			if (nLaneDirCount > 2)
			{
				TRACE(_T("nLaneDirCount: %d, %d, %d\r\n"), /*pFtr->nEdgeID, */pLaneInfo->byStartLaneNumInfo.bit4LaneNum, nLaneDirCount, pLaneInfo->pbyStartLaneDir[i]);
			}
		}
	}//end if (pLaneInfo->byStartLaneNumInfo.bit4OutLinkNum > 0)

	//End lane
	memcpy(&(pLaneInfo->byEndLaneNumInfo), pBuf, sizeof(pLaneInfo->byEndLaneNumInfo));
	pBuf += sizeof(pLaneInfo->byEndLaneNumInfo);
	
	if ((pLaneInfo->byEndLaneNumInfo.bit4LaneNum) > 0)
	{
		pLaneInfo->pbyEndLaneDir = new BYTE[pLaneInfo->byEndLaneNumInfo.bit4LaneNum];
		memcpy(pLaneInfo->pbyEndLaneDir, pBuf, sizeof(pLaneInfo->pbyEndLaneDir[0]) * (pLaneInfo->byEndLaneNumInfo.bit4LaneNum));
		pBuf += sizeof(pLaneInfo->pbyEndLaneDir[0]) * (pLaneInfo->byEndLaneNumInfo.bit4LaneNum);
	}

	if (pLaneInfo->byEndLaneNumInfo.bit4OutLinkNum > 0)
	{
		pLaneInfo->lpEndLaneForOutID = new LANEFOROUTID[pLaneInfo->byEndLaneNumInfo.bit4OutLinkNum];
		int i;
		for (i = 0; i < pLaneInfo->byEndLaneNumInfo.bit4OutLinkNum; i++)
		{
			memcpy(&(pLaneInfo->lpEndLaneForOutID[i].wOutLinkID), pBuf, sizeof(pLaneInfo->lpEndLaneForOutID[i].wOutLinkID));
			pBuf += sizeof(pLaneInfo->lpEndLaneForOutID[i].wOutLinkID);
			pLaneInfo->lpEndLaneForOutID[i].pbyLaneDir = new BYTE[pLaneInfo->byEndLaneNumInfo.bit4LaneNum];//注意此处的num值不是bit4OutLinkNum
			memcpy(pLaneInfo->lpEndLaneForOutID[i].pbyLaneDir, pBuf, sizeof(pLaneInfo->lpEndLaneForOutID[i].pbyLaneDir[0]) * pLaneInfo->byEndLaneNumInfo.bit4LaneNum);
			pBuf += sizeof(pLaneInfo->lpEndLaneForOutID[i].pbyLaneDir[0]) * pLaneInfo->byEndLaneNumInfo.bit4LaneNum;
		}

		//for check data
		for (i = 0; i < pLaneInfo->byEndLaneNumInfo.bit4LaneNum; i++)
		{
			BYTE byLaneDir = 0;
			for (int j = 0; j < pLaneInfo->byEndLaneNumInfo.bit4OutLinkNum; j++)
			{
				byLaneDir = byLaneDir | pLaneInfo->lpEndLaneForOutID[j].pbyLaneDir[i];
			}
			ASSERT(byLaneDir == pLaneInfo->pbyEndLaneDir[i]);

			int nLaneDirCount = 0;
			nLaneDirCount = (pLaneInfo->pbyEndLaneDir[i] & LANEINFO::SlightLeft) + 
							((pLaneInfo->pbyEndLaneDir[i] & LANEINFO::Left) >> 1) + 
							((pLaneInfo->pbyEndLaneDir[i] & LANEINFO::SharpLeft) >> 2) + 
							((pLaneInfo->pbyEndLaneDir[i] & LANEINFO::UTurn) >> 3) + 
							((pLaneInfo->pbyEndLaneDir[i] & LANEINFO::SharpRight) >> 4) + 
							((pLaneInfo->pbyEndLaneDir[i] & LANEINFO::Right) >> 5) + 
							((pLaneInfo->pbyEndLaneDir[i] & LANEINFO::SlightRight) >> 6)+ 
							((pLaneInfo->pbyEndLaneDir[i] & LANEINFO::Straight) >> 7);
			if (nLaneDirCount > 2)
			{
				TRACE(_T("nLaneDirCount: %d, %d, %d\r\n"), /*pFtr->nEdgeID*/ pLaneInfo->byEndLaneNumInfo.bit4LaneNum, nLaneDirCount, pLaneInfo->pbyEndLaneDir[i]);
			}
		}
	}//end if (pLaneInfo->byEndLaneNumInfo.bit4OutLinkNum > 0)
}

//////////////////////////////////////////////////////////////////////////