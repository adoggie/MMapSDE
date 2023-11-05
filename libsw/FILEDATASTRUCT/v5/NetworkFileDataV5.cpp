// NetworkFileDataV5.cpp: implementation of the CNetworkFileDataV5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetworkFileDataV5.h"

#include "FileDataStruct/V5/Gis_NetworkRoadSectionV5.h"
#include "FileDataStruct/V5/GIS_NetworkVertexV5.h"

#include "Zlib/zlib.h"

#include "DebugFunctionLib/DebugFun.h"
#include <math.h>
//////////////////////////////////////////////////////////////////////////
static char s_DestBuf2[1000000];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkFileDataV5::CNetworkFileDataV5()
{
    m_pBreadthContainer = new MapOfBreadth;
}

CNetworkFileDataV5::~CNetworkFileDataV5()
{

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL CNetworkFileDataV5::Load(LPCTSTR strFilePath)
{
#ifdef ANALYSE_DATA
    m_DataAnalyse.Initial();
#endif  
    CFile aFile(strFilePath,CFile::modeRead);
	CFile* pFile = &aFile;
	ReadNetwork(pFile);

    {
        CString strDir = strFilePath;
        strDir = strDir.Left(strDir.ReverseFind(_T('\\'))+1);
        CString strFilePath2 = strDir + _T("DetailBreadthID2MainBreadthID.dat");
        
        Check(strFilePath2);
    }

	//DUMPSTATE(this);
    return TRUE;
}
//BOOL CNetworkFileDataV5::ReSort()
//{
//    MapOfBreadth* pBreadthContainer = m_pBreadthContainer;
//    MapOfBreadth::iterator iteratorOfBreadth;
//    for(iteratorOfBreadth = pBreadthContainer->begin();
//    iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
//    {
//        pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
//        stuBreadthItem* pItem = aPair.second;
//        ASSERT(pItem);
//        ASSERT(!pItem->pRoadSectionIDContainer);
//        ASSERT(!pItem->pVertexIDContainer);
//
//        pItem->pRoadSectionIDContainer = new MapOfID;
//        pItem->pVertexIDContainer = new MapOfID;
//
//        int nBreadthID = pItem->pIndex->nID;
//
//        
//        
//        StuNetworkFileStructV5::StuBreadthHeaderInfo* pBreadthHeader = pItem->pHeader;
//        
//        {
//            int nIndexN = 0;
//            MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;		
//            MapOfRoadSection::iterator iteratorOfFtr;
//            for(iteratorOfFtr = pFtrContainer->begin();
//            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
//            {
//                pair <unsigned long ,CGIS_NetworkRoadSectionV5* > aPair = *iteratorOfFtr;
//                CGIS_NetworkRoadSectionV5* pFtr = (CGIS_NetworkRoadSectionV5*)aPair.second;
//                
//                unsigned long nOldFtrID = pFtr->GetRutID();
//                unsigned long nNewFtrID = (nBreadthID<<15)|nIndexN;
//                ChangeInfo(nOldFtrID,nNewFtrID);
//
//                nIndexN++;
//            }
// 
//            
//        }
//        {
//            MapOfVertex* pFtrContainer = pItem->pVertexContainer;
//            MapOfVertex::iterator iteratorOfFtr;
//            for(iteratorOfFtr = pFtrContainer->begin();
//            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
//            {
//                pair <unsigned long ,CGIS_NetworkVertexV5* > aPair = *iteratorOfFtr;
//                CGIS_NetworkVertexV5* pFtr = (CGIS_NetworkVertexV5*)aPair.second;
//                
//                (*pItem->pVertexIDContainer)[pFtr->GetVtxID()] = pFtr->GetVtxID();
//            }
//            
//        }
//        
//        
//        
//    }
//    return TRUE;
//}

//按V5格式写入文件
BOOL CNetworkFileDataV5::Write(LPCTSTR strFileName)
{
    //////////////////////////////////////////////////////////////////////////
	//写文件
	CMemFile aMemFile;
    WriteNetwork(&aMemFile);
  
    //////////////////////////////////////////////////////////////////////////
    //写入真实文件
    CFile aFile(strFileName,CFile::modeCreate|CFile::modeReadWrite);
 	unsigned long nSrcSize = aMemFile.GetLength();
	unsigned char* pSrcData = aMemFile.Detach();
	aFile.Write(pSrcData,nSrcSize);
	delete []pSrcData;
	pSrcData = NULL;

    return FALSE;
}

#ifdef _DEBUG
void CNetworkFileDataV5::DumpState()
{

    TRACE0("********************************************************************\n");
    TRACE0("CNetworkFileDataV5 Info:\n");
    //TRACE1("FtrTotalCount    : %d.\n",GetFtrCount());
#ifdef ANALYSE_DATA
    m_DataAnalyse.DumpState();
#endif
    TRACE0("********************************************************************\n");
}
void CNetworkFileDataV5::AssertValid() const
{
    
}
#endif
//////////////////////////////////////////////////////////////////////////
static char s_BuddyBuf[10000000];
BOOL CNetworkFileDataV5::WriteNetwork(CFile* pFile)
{
	//---------------------------
	//构造头,写头	
    StuNetworkFileStructV5::StuHeaderInfo aHeadInfo;
    memset(&aHeadInfo,0,sizeof(aHeadInfo));
    strcpy(aHeadInfo.chSign,"RUT_TEST");
    aHeadInfo.nMapCount = 1;    //(只有简图)
    pFile->Write(&aHeadInfo,sizeof(aHeadInfo));
    
    //---------------------------
    //写入地图索引
    int nCount = aHeadInfo.nMapCount;
    StuNetworkFileStructV5::StuIndexInfoOfMap* pIndexInfoOfMap = 
        new StuNetworkFileStructV5::StuIndexInfoOfMap[nCount];

    //////////////////////////////////////////////////////////////////////////
    DWORD dwIndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfMap,sizeof(StuNetworkFileStructV5::StuIndexInfoOfMap) * nCount);
    //////////////////////////////////////////////////////////////////////////
    DWORD dwPreSize = pFile->GetPosition();
    for(int i = 0; i < nCount; i++)
    {
        
        stuMapItem* pItem = new stuMapItem;
        pItem->pBreadthContainer = m_pBreadthContainer;
        pItem->pIndex = new StuNetworkFileStructV5::StuIndexInfoOfMap;
        
        pItem->pIndex->nID = i;
        pItem->pIndex->nOffset = pFile->GetPosition() - dwPreSize;
        pItem->pIndex->nBreadthCount = pItem->pBreadthContainer->size();
 
        VERIFY(WriteMap(pFile,pItem));
        memcpy(&pIndexInfoOfMap[i],pItem->pIndex,sizeof(StuNetworkFileStructV5::StuIndexInfoOfMap));
    }

    //////////////////////////////////////////////////////////////////////////
    pFile->Seek(dwIndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfMap,sizeof(StuNetworkFileStructV5::StuIndexInfoOfMap) * nCount);
    //////////////////////////////////////////////////////////////////////////
    
    delete pIndexInfoOfMap;
    pIndexInfoOfMap = NULL;

    return TRUE;
}


BOOL CNetworkFileDataV5::WriteMap       (CFile* pFile,stuMapItem* pMapItem)
{
    ASSERT(pMapItem);
    ASSERT(pMapItem->pIndex);
    ASSERT(pMapItem->pBreadthContainer);
    
    StuNetworkFileStructV5 :: StuIndexInfoOfMap* pIndexInfoOfMap = pMapItem->pIndex;
        
    int nCount = pIndexInfoOfMap->nBreadthCount;
    StuNetworkFileStructV5::StuIndexInfoOfBreadth* pIndexInfoOfBreadth 
        = new StuNetworkFileStructV5::StuIndexInfoOfBreadth[nCount];
    
    //写入所有图幅的索引
    //////////////////////////////////////////////////////////////////////////
    DWORD dwIndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuNetworkFileStructV5::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    DWORD dwPreSize = pFile->GetPosition();
    //-------------------------------------

    int nIndexN = 0;
    MapOfBreadth* pBreadthContainer = pMapItem->pBreadthContainer;
    MapOfBreadth::iterator iteratorOfBreadth;
    for(iteratorOfBreadth = pBreadthContainer->begin();
    iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
    {
        pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
        unsigned long nIndex = aPair.first;
        stuBreadthItem* pItem = aPair.second;
        ASSERT(pItem);
        //ASSERT(!pItem->pIndex);

        pItem->pIndex = new StuNetworkFileStructV5::StuIndexInfoOfBreadth;
        pItem->pIndex->nID= nIndex;
        pItem->pIndex->nOffset = pFile->GetPosition() - dwPreSize;
        pItem->pIndex->nBeforeCompressSize = 0;
        pItem->pIndex->nAfterCompressSize = 0;
        
        WriteBreadth(pFile,pItem);
        
        memcpy(&pIndexInfoOfBreadth[nIndexN],pItem->pIndex,sizeof(StuNetworkFileStructV5::StuIndexInfoOfBreadth));
        nIndexN++;
    }

    
    //////////////////////////////////////////////////////////////////////////
    //真正写入所有图幅的索引的有效数据
    pFile->Seek(dwIndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuNetworkFileStructV5::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////

    delete pIndexInfoOfBreadth;
    pIndexInfoOfMap = NULL;
    return TRUE;
    
    return FALSE;
}

BOOL CNetworkFileDataV5::WriteBreadth   (CFile* pFile,stuBreadthItem* pBreadthItem)
{
    ASSERT(pFile);
    ASSERT(pBreadthItem->pIndex);
    ASSERT(pBreadthItem->pRoadSectionContainer);
    ASSERT(pBreadthItem->pVertexContainer);
  
    StuNetworkFileStructV5 :: StuIndexInfoOfBreadth* pIndexInfoOfBreadth = pBreadthItem->pIndex;
    MapOfRoadSection* pRoadSectionContainer = pBreadthItem->pRoadSectionContainer  ;  
    MapOfVertex* pVertexContainer = pBreadthItem->pVertexContainer;
    //////////////////////////////////////////////////////////////////////////
    
    StuNetworkFileStructV5::StuBreadthHeaderInfo aBreadthHeaderInfo; 
    aBreadthHeaderInfo.nCountOfLevelAllRoadSection = pRoadSectionContainer->size();
    aBreadthHeaderInfo.nCountOfLevelAllVertex = pVertexContainer->size();

	CMemFile aMemFile;
	{
		CFile* pFile = &aMemFile;

		//写入图幅头		
        DWORD dwHeadPos = pFile->GetPosition();
		pFile->Write(&aBreadthHeaderInfo,sizeof(StuNetworkFileStructV5::StuBreadthHeaderInfo));		
        //////////////////////////////////////////////////////////////////////////
		//写入所有路段
		{
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pRoadSectionContainer->begin();
            iteratorOfFtr != pRoadSectionContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_NetworkRoadSectionV5*> aPair = *iteratorOfFtr;
                CGIS_NetworkRoadSectionV5* pFtr = aPair.second;
                unsigned long nIndex = aPair.first;
                ASSERTVALID(pFtr);
                pFile->Write(pFtr->GetDataAddr(),sizeof(StuNetworkFileStructV5::stuRoadSection));

            }
		}
		
		//写入所有节点
		{
            MapOfVertex::iterator iteratorOfFtr;
            for(iteratorOfFtr = pVertexContainer->begin();
            iteratorOfFtr != pVertexContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_NetworkVertexV5*> aPair = *iteratorOfFtr;
                CGIS_NetworkVertexV5* pFtr = aPair.second;
                unsigned long nIndex = aPair.first;
                ASSERTVALID(pFtr);
                pFile->Write(pFtr->GetDataAddr(),sizeof(StuNetworkFileStructV5::stuVertex));
            }
        }
       
        //处理出buddy数据块,并写入文件

        {
            char* pBuddy = s_BuddyBuf;
            unsigned long nCharDataBuddyLength;
            unsigned long nShortDataBuddyLength;
            unsigned long nDwordDataBuddyLength;
            
            unsigned long nBuddyDataSize;
            VERIFY(GetBuddyDataOfRoadSectionSize(pRoadSectionContainer,nBuddyDataSize));
            ASSERT(nBuddyDataSize <= sizeof(s_BuddyBuf));
            
            memset(s_BuddyBuf,0,nBuddyDataSize);
            CollectBuddyDataOfRoadSection(
                pRoadSectionContainer,
                pBuddy,
                nCharDataBuddyLength,
                nShortDataBuddyLength,
                nDwordDataBuddyLength);
            
            ASSERT(nBuddyDataSize = nCharDataBuddyLength+ nShortDataBuddyLength + nDwordDataBuddyLength);
            pFile->Write(s_BuddyBuf,nBuddyDataSize);

            aBreadthHeaderInfo.nRoadSectionCharDataBuddyLength = nCharDataBuddyLength;
            aBreadthHeaderInfo.nRoadSectionShortDataBuddyLength = nShortDataBuddyLength;
            aBreadthHeaderInfo.nRoadSectionDwordDataBuddyLength = nDwordDataBuddyLength;
        }
        {
            char* pBuddy = s_BuddyBuf;
            unsigned long nCharDataBuddyLength;
            unsigned long nShortDataBuddyLength;
            unsigned long nDwordDataBuddyLength;
            
            unsigned long nBuddyDataSize;
            VERIFY(GetBuddyDataOfVertexSize(pVertexContainer,nBuddyDataSize));
            ASSERT(nBuddyDataSize <= sizeof(s_BuddyBuf));
            
            memset(s_BuddyBuf,0,nBuddyDataSize);
            CollectBuddyDataOfVertex(
                pVertexContainer,
                pBuddy,
                nCharDataBuddyLength,
                nShortDataBuddyLength,
                nDwordDataBuddyLength);
            
            ASSERT(nBuddyDataSize = nCharDataBuddyLength+ nShortDataBuddyLength + nDwordDataBuddyLength);
            pFile->Write(s_BuddyBuf,nBuddyDataSize);

            aBreadthHeaderInfo.nVertexCharDataBuddyLength = nCharDataBuddyLength;
            aBreadthHeaderInfo.nVertexShortDataBuddyLength = nShortDataBuddyLength;
            aBreadthHeaderInfo.nVertexDwordDataBuddyLength = nDwordDataBuddyLength;
        }

        {
            //写入映射关系数组.
        }
        //////////////////////////////////////////////////////////////////////////
        //写入图幅头的有效数据
        pFile->Seek(dwHeadPos,CFile::begin);
		pFile->Write(&aBreadthHeaderInfo,sizeof(StuNetworkFileStructV5::StuBreadthHeaderInfo));

	}

	//压缩数据
	unsigned long nSrcSize = aMemFile.GetLength();
	unsigned char* pSrcData = aMemFile.Detach();
	unsigned char* pDestBuf =(unsigned char*) s_DestBuf2;//new unsigned char[0x1000000];
	unsigned long nDestSize = compressBound(nSrcSize);
    ASSERT(nDestSize < 1000000);
	compress2(pDestBuf,&nDestSize,pSrcData,nSrcSize,Z_DEFAULT_COMPRESSION);
	ASSERT(nDestSize > 0);
  
    delete [] pSrcData;
    pSrcData = NULL;

    pIndexInfoOfBreadth->nBeforeCompressSize = nSrcSize;
    pIndexInfoOfBreadth->nAfterCompressSize = nDestSize;

	//写入数据到文件
	pFile->Write(pDestBuf,nDestSize);
	pFile->Flush();



	return FALSE;
}
BOOL CNetworkFileDataV5::GetBuddyDataOfRoadSectionSize(
     MapOfRoadSection* pFtrContainer,
     unsigned long& nBuddyDataSize)
{
    ASSERT(pFtrContainer);
    
    //////////////////////////////////////////////////////////////////////////
    //对路段的处理
    
    int nNum;
    int nSize;
    char* pBuddyData = NULL;
    //ASSERT(pBuddyData);
    //*( (DWORD*)pBuddyData ) = ('y'<<24|'l'<<16|'d'<<8|' '<<0);
    char* pCurrent = pBuddyData+4;
    
    MapOfRoadSection::iterator iteratorOfFtr;
    for(iteratorOfFtr = pFtrContainer->begin();
    iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
    {
        pair < unsigned long,CGIS_NetworkRoadSectionV5*> aPair = *iteratorOfFtr;
        CGIS_NetworkRoadSectionV5* pFtr = aPair.second;
        unsigned long nID = aPair.first;
        
        nNum = pFtr->GetLimitNormalNum();
        ASSERT(nNum < 50);
        if( nNum > 0 )
        {
            nSize = sizeof(unsigned long) * nNum;
            pCurrent += nSize;
        }
        
        
        nNum = pFtr->GetLimitReverseNum();
        ASSERT(nNum < 50);
        if( nNum > 0 )
        {
            nSize = sizeof(unsigned long) * nNum;
            pCurrent += nSize;
        }
        
    }
    pCurrent = (char*)((DWORD(pCurrent + 0x03))&0xfffffffc);
    int nCountOfBuddyData = pCurrent - pBuddyData;
    ASSERT(nCountOfBuddyData >= 4);
    ASSERT( nCountOfBuddyData == ((nCountOfBuddyData + 0x03)&0xfffffffc)	);//确定以4为边界对齐
    
    nBuddyDataSize = nCountOfBuddyData;   
    pBuddyData = NULL;
    
	return TRUE;
}


BOOL CNetworkFileDataV5::CollectBuddyDataOfRoadSection(
        MapOfRoadSection* pFtrContainer,
        char* pBuddy,
        unsigned long& nCharDataBuddyLength,
        unsigned long& nShortDataBuddyLength,
        unsigned long& nDwordDataBuddyLength)

{
    ASSERT(pFtrContainer);
    ASSERT(pBuddy);
    nCharDataBuddyLength = 0;
    nShortDataBuddyLength = 0;
    nDwordDataBuddyLength = 0;
    

	//////////////////////////////////////////////////////////////////////////
	//对路段的处理
	{
        int nNum;
        int nSize;
        char* pBuddyData = pBuddy;
        ASSERT(pBuddyData);
        *( (DWORD*)pBuddyData ) = ('y'<<24|'l'<<16|'d'<<8|' '<<0);
        char* pCurrent = pBuddyData+4;
        
        MapOfRoadSection::iterator iteratorOfFtr;
        for(iteratorOfFtr = pFtrContainer->begin();
        iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
        {
            pair < unsigned long,CGIS_NetworkRoadSectionV5*> aPair = *iteratorOfFtr;
            CGIS_NetworkRoadSectionV5* pFtr = aPair.second;
            unsigned long nID = aPair.first;
            
            nNum = pFtr->GetLimitNormalNum();
            ASSERT(nNum < 50);
            if( nNum > 0 )
            {
                nSize = sizeof(unsigned long) * nNum;
                memcpy(pCurrent,pFtr->GetLimitNormalList(),nSize);
                pCurrent += nSize;
            }
            
            
            nNum = pFtr->GetLimitReverseNum();
            ASSERT(nNum < 50);
            if( nNum > 0 )
            {
                nSize = sizeof(unsigned long) * nNum;
                memcpy(pCurrent,pFtr->GetLimitReverseList(),nSize);					
                pCurrent += nSize;
            }
            
        }
        pCurrent = (char*)((DWORD(pCurrent + 0x03))&0xfffffffc);
        int nCountOfBuddyData = pCurrent - pBuddyData;
        ASSERT(nCountOfBuddyData >= 4);
        ASSERT( nCountOfBuddyData == ((nCountOfBuddyData + 0x03)&0xfffffffc)	);//确定以4为边界对齐
        
        nDwordDataBuddyLength = nCountOfBuddyData;   
        pBuddyData = NULL;
    }
	return TRUE;
}

BOOL CNetworkFileDataV5::GetBuddyDataOfVertexSize(
       MapOfVertex* pFtrContainer,
       unsigned long& nBuddyDataSize)
{
    ASSERT(pFtrContainer);

    //////////////////////////////////////////////////////////////////////////
    //对节点的处理
    
    int nNum;
    int nSize;
    char* pBuddyData = NULL;
    //ASSERT(pBuddyData);
    //*( (DWORD*)pBuddyData ) = ('y'<<24|'l'<<16|'d'<<8|' '<<0);
    char* pCurrent = pBuddyData+4;


    {  
        MapOfVertex::iterator iteratorOfFtr;
        for(iteratorOfFtr = pFtrContainer->begin();
        iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
        {
            pair < unsigned long,CGIS_NetworkVertexV5*> aPair = *iteratorOfFtr;
            CGIS_NetworkVertexV5* pFtr = aPair.second;
            unsigned long nIndex = aPair.first;
            
            
            nNum = pFtr->GetRoadSectionNum();
            ASSERT(nNum < 11);
            if (nNum > 0)
            {
                nSize = sizeof(unsigned short) * nNum;
                pCurrent += nSize;
            }
            
            
            nNum = pFtr->GetRoadSectionNum();
            ASSERT(nNum < 11);
            if(nNum > 0 )
            {
                nSize = sizeof(unsigned short) * nNum;
                pCurrent += nSize;
            }
            
            
            
            
        }
    }
    
    pCurrent = (char*)((DWORD(pCurrent + 0x03))&0xfffffffc);
    int nCountOfBuddyData = pCurrent - pBuddyData;
    ASSERT(nCountOfBuddyData >= 4);
    
    {  
        MapOfVertex::iterator iteratorOfFtr;
        for(iteratorOfFtr = pFtrContainer->begin();
        iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
        {
            pair < unsigned long,CGIS_NetworkVertexV5*> aPair = *iteratorOfFtr;
            CGIS_NetworkVertexV5* pFtr = aPair.second;
            unsigned long nIndex = aPair.first;
            
            nNum = pFtr->GetRoadSectionNum();	
            ASSERT(nNum < 11);
            if (nNum > 0)
            {
                nSize = sizeof(unsigned long) * nNum;
                pCurrent += nSize;
            }
            
            if(pFtr->GetEdge())
            {
                CGIS_NetworkVertexV5::ST_GEO_VEDGE* pEdgeInfo = (CGIS_NetworkVertexV5::ST_GEO_VEDGE*)pCurrent;
                pCurrent += sizeof(CGIS_NetworkVertexV5::ST_GEO_VEDGE);
            }								
        }	
        int nCountOfBuddyData = pCurrent - pBuddyData;
        ASSERT(nCountOfBuddyData >= 4);
        ASSERT( nCountOfBuddyData == ((nCountOfBuddyData + 0x03)&0xfffffffc)	);//确定以4为边界对齐
        //写入一二三等级路网的合计dword size
        nBuddyDataSize	= nCountOfBuddyData;
    }
 
    
    pBuddyData = NULL;
	return TRUE;
}
BOOL CNetworkFileDataV5::CollectBuddyDataOfVertex(
        MapOfVertex* pFtrContainer,
        char* pBuddy,
        unsigned long& nCharDataBuddyLength,
        unsigned long& nShortDataBuddyLength,
        unsigned long& nDwordDataBuddyLength)

{
    ASSERT(pFtrContainer);
    ASSERT(pBuddy);
    nCharDataBuddyLength = 0;
    nShortDataBuddyLength = 0;
    nDwordDataBuddyLength = 0;
    

    //////////////////////////////////////////////////////////////////////////
    //对节点的处理
    
    int nNum;
    int nSize;
    char* pBuddyData = pBuddy;
    ASSERT(pBuddyData);
    *( (DWORD*)pBuddyData ) = ('y'<<24|'l'<<16|'d'<<8|' '<<0);
    char* pCurrent = pBuddyData+4;


    {  
        MapOfVertex::iterator iteratorOfFtr;
        for(iteratorOfFtr = pFtrContainer->begin();
        iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
        {
            pair < unsigned long,CGIS_NetworkVertexV5*> aPair = *iteratorOfFtr;
            CGIS_NetworkVertexV5* pFtr = aPair.second;
            unsigned long nIndex = aPair.first;
            
            
            nNum = pFtr->GetRoadSectionNum();
            ASSERT(nNum < 11);
            if (nNum > 0)
            {
                nSize = sizeof(unsigned short) * nNum;
                memcpy(pCurrent,pFtr->GetRoadSectionTurnList(),nSize);
                pCurrent += nSize;
            }
            
            
            nNum = pFtr->GetRoadSectionNum();
            ASSERT(nNum < 11);
            if(nNum > 0 )
            {
                nSize = sizeof(unsigned short) * nNum;
                memcpy(pCurrent,pFtr->GetRoadSectionLayerIDList(),nSize);
                pCurrent += nSize;
            }
            
            
            
            
        }
    }
    
    pCurrent = (char*)((DWORD(pCurrent + 0x03))&0xfffffffc);
    int nCountOfBuddyData = pCurrent - pBuddyData;
    ASSERT(nCountOfBuddyData >= 4);
    nShortDataBuddyLength = nCountOfBuddyData - nCharDataBuddyLength;
    
    {  
        MapOfVertex::iterator iteratorOfFtr;
        for(iteratorOfFtr = pFtrContainer->begin();
        iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
        {
            pair < unsigned long,CGIS_NetworkVertexV5*> aPair = *iteratorOfFtr;
            CGIS_NetworkVertexV5* pFtr = aPair.second;
            unsigned long nIndex = aPair.first;
            
            nNum = pFtr->GetRoadSectionNum();	
            ASSERT(nNum < 11);
            if (nNum > 0)
            {
                nSize = sizeof(unsigned long) * nNum;
                memcpy(pCurrent,pFtr->GetRoadSectionList() ,nSize);
                pCurrent += nSize;
            }
            
            if(pFtr->GetEdge())
            {
                CGIS_NetworkVertexV5::ST_GEO_VEDGE* pEdgeInfo = (CGIS_NetworkVertexV5::ST_GEO_VEDGE*)pCurrent;
                pEdgeInfo->nRoadSectionID = pFtr->GetEdgeRoadSectionID();
                pCurrent += sizeof(CGIS_NetworkVertexV5::ST_GEO_VEDGE);
            }								
        }	
        int nCountOfBuddyData = pCurrent - pBuddyData;
        ASSERT(nCountOfBuddyData >= 4);
        ASSERT( nCountOfBuddyData == ((nCountOfBuddyData + 0x03)&0xfffffffc)	);//确定以4为边界对齐
        //写入一二三等级路网的合计dword size
        nDwordDataBuddyLength	= nCountOfBuddyData - nCharDataBuddyLength - nShortDataBuddyLength ;
    }
 
    
    pBuddyData = NULL;
	return FALSE;
}

BOOL CNetworkFileDataV5::ReadNetwork(CFile* pFile)
{
	ASSERT(pFile);

	//读头
    StuNetworkFileStructV5 :: StuHeaderInfo* m_pFileHeadInfo = new StuNetworkFileStructV5 :: StuHeaderInfo;
	pFile->Read(m_pFileHeadInfo,sizeof(StuNetworkFileStructV5 :: StuHeaderInfo));

	int nCount = m_pFileHeadInfo->nMapCount;
	//---------------------------
	//读入地图索引
	StuNetworkFileStructV5::StuIndexInfoOfMap* pIndexInfoArray = NULL;		//地图索引信息
	pIndexInfoArray = new StuNetworkFileStructV5::StuIndexInfoOfMap[nCount];
	memset(pIndexInfoArray,0,sizeof(StuNetworkFileStructV5::StuIndexInfoOfMap)*nCount);
	pFile->Read(pIndexInfoArray,sizeof(StuNetworkFileStructV5::StuIndexInfoOfMap) * nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////

    ASSERT(nCount == 1);
    for(int i = 0; i < nCount; i++)
    {        
        stuMapItem* pItem = new stuMapItem;
        pItem->pIndex               = new StuNetworkFileStructV5::StuIndexInfoOfMap;
        ASSERT(m_pBreadthContainer);
        pItem->pBreadthContainer = m_pBreadthContainer;

        memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuNetworkFileStructV5::StuIndexInfoOfMap));
		pFile->Seek( pItem->pIndex->nOffset + dwOffsetBase,CFile::begin);  //地图的偏移相对于文件起始位置的.所以不加头的大小.    
		ReadMap(pFile,pItem);

        delete pItem->pIndex;
        pItem->pIndex = NULL;
        delete pItem;
        pItem = NULL;

    }

    delete []pIndexInfoArray;
    pIndexInfoArray = NULL;

    return TRUE;
}

BOOL CNetworkFileDataV5::ReadMap(CFile* pFile,stuMapItem* pMapItem)
{

    StuNetworkFileStructV5::StuIndexInfoOfMap* pIndexInfoOfMap = pMapItem->pIndex;
	MapOfBreadth*& pBreadthContainer = pMapItem->pBreadthContainer;

	ASSERT(pFile);
	ASSERT(pIndexInfoOfMap);
	ASSERT(pBreadthContainer);

	//////////////////////////////////////////////////////////////////////////
    //读索引数据   
    int nCount = pIndexInfoOfMap->nBreadthCount;
    int nMapID = pIndexInfoOfMap->nID;
	StuNetworkFileStructV5::StuIndexInfoOfBreadth* pIndexInfoArray = 
        new StuNetworkFileStructV5::StuIndexInfoOfBreadth[nCount];
	pFile->Read(pIndexInfoArray,sizeof(StuNetworkFileStructV5::StuIndexInfoOfBreadth) * nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////
    //读入地图数据
    for(int i = 0; i < nCount; i++)
	{	
#ifdef ANALYSE_DATA
        m_DataAnalyse.m_nBreadthCount++;
#endif
	    DBG2LOG2("BreadthID[%d]'s ID = %d",i,pIndexInfoArray[i].nID);
		//偏移为0表示此图幅是一个空的图幅
		if(pIndexInfoArray[i].nBeforeCompressSize == 0)
        {
            ASSERT(pIndexInfoArray[i].nAfterCompressSize == 0);
			continue;
        }
			 
        stuBreadthItem* pItem = new stuBreadthItem;
        pItem->pIndex = new StuNetworkFileStructV5::StuIndexInfoOfBreadth;
        pItem->pHeader = new StuNetworkFileStructV5::StuBreadthHeaderInfo;
        pItem->pRoadSectionContainer = new MapOfRoadSection;
        pItem->pVertexContainer = new MapOfVertex;
        pItem->pRoadSectionBuddy = NULL;
        pItem->pVertexBuddy = NULL;
        pItem->nID = i;

		memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuNetworkFileStructV5::StuIndexInfoOfBreadth));


		pFile->Seek(pItem->pIndex->nOffset + dwOffsetBase,CFile::begin);
		BOOL bReadSuccess = FALSE;
         bReadSuccess = ReadBreadth(pFile,pItem, 0);
        //bReadSuccess = ReadBreadthHeadInfo(pFile,pBreadthIndex,pBreadthHeader,1);
		VERIFY(bReadSuccess);
		
		(*pBreadthContainer)[i] = pItem;
		

	}

    delete []pIndexInfoArray;
    pIndexInfoArray = NULL;

    return FALSE;
}


BOOL CNetworkFileDataV5::ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem,int nLoadFlag)
{

    StuNetworkFileStructV5::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = pBreadthItem->pIndex;
	MapOfRoadSection* pRoadSectionMap = pBreadthItem->pRoadSectionContainer;
    MapOfVertex*  pVertexMap = pBreadthItem->pVertexContainer;
	char*& pRoadSectionBuddy = pBreadthItem->pRoadSectionBuddy;
    char*& pVertexBuddy = pBreadthItem->pVertexBuddy;
    

	ASSERT(pIndexInfoOfBreadth);
 
    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////
    
	int nRoadSectionTotalCount = 0;
	int nJunctionTotalCount = 0;
	
	char *pFBuf = NULL;
	{
		//两个size为0表示此图幅是一个空的图幅
		ASSERT(pIndexInfoOfBreadth->nBeforeCompressSize > 0);
		ASSERT(pIndexInfoOfBreadth->nAfterCompressSize > 0);


		int nBeforeCompressSize = pIndexInfoOfBreadth->nBeforeCompressSize;
		int nAfterCompressSize  = pIndexInfoOfBreadth->nAfterCompressSize;
		DWORD dwLayerOffset     = dwOffsetBase;
        
#ifdef ANALYSE_DATA
        if(nBeforeCompressSize > m_DataAnalyse.m_dwMaxBeforeCompressSize)
        {
            m_DataAnalyse.m_dwMaxBeforeCompressSize = nBeforeCompressSize;
        }
        if(nBeforeCompressSize < m_DataAnalyse.m_dwMinBeforeCompressSize)
        {
            m_DataAnalyse.m_dwMinBeforeCompressSize = nBeforeCompressSize;
        }
        if(nAfterCompressSize > m_DataAnalyse.m_dwMaxAfterCompressSize)
        {
            m_DataAnalyse.m_dwMaxAfterCompressSize = nAfterCompressSize;
        }
        if(nAfterCompressSize < m_DataAnalyse.m_dwMinAfterCompressSize)
        {
            m_DataAnalyse.m_dwMinAfterCompressSize = nAfterCompressSize;
        }

        m_DataAnalyse.m_nTotalBeforeCompressSize += nBeforeCompressSize;
        m_DataAnalyse.m_nTotalAfterCompressSize += nAfterCompressSize;
#endif
    
        dwLayerOffset = dwOffsetBase;
		
		//分配压缩前空间
		char *pBufOfBeforeCompress = new char[nBeforeCompressSize];
		ASSERT(pBufOfBeforeCompress);

		//-------------------------------------------------
		//读取并解压数据
		{
			pFile->Seek( dwLayerOffset,CFile::begin);		
			char *pBufOfAfterCompress = new char[nAfterCompressSize];//分配压缩后
			ASSERT(pBufOfAfterCompress);
			
			pFile->Read( pBufOfAfterCompress, nAfterCompressSize );
			DWORD dwSize = nBeforeCompressSize;
			int nRet = uncompress( (unsigned char*)pBufOfBeforeCompress, &dwSize, (unsigned char*)pBufOfAfterCompress, nAfterCompressSize  );
			ASSERT(nRet == Z_OK);
			delete [] pBufOfAfterCompress;
			//ASSERT(nBeforeCompressSize == dwSize);
		}
		//-----------------------------------------------------

		//-----------------------------------------------------
		//处理图幅头数据
		StuNetworkFileStructV5::StuBreadthHeaderInfo* pBreadthHeader = (StuNetworkFileStructV5::StuBreadthHeaderInfo*)pBufOfBeforeCompress;
		unsigned long nCountOfRoadSection   = pBreadthHeader->nCountOfLevelAllRoadSection;
		unsigned long nCountOfVertex        = pBreadthHeader->nCountOfLevelAllVertex;
#ifdef ANALYSE_DATA
        unsigned long nFtrCount = nCountOfRoadSection ;
        if(nFtrCount > m_DataAnalyse.m_nMaxFtrCountOfBreadth)
        {
            m_DataAnalyse.m_nMaxFtrCountOfBreadth = nFtrCount;
        }
        if(nFtrCount < m_DataAnalyse.m_nMinFtrCountOfBreadth)
        {
            m_DataAnalyse.m_nMinFtrCountOfBreadth = nFtrCount;
        }        
#endif


		unsigned long nSizeOfRoadSectionBuddy = 
            pBreadthHeader->nRoadSectionCharDataBuddyLength + 
            pBreadthHeader->nRoadSectionShortDataBuddyLength +
            pBreadthHeader->nRoadSectionDwordDataBuddyLength;
		unsigned long nSizeOfVertexBuddy = 
            pBreadthHeader->nVertexCharDataBuddyLength + 
            pBreadthHeader->nVertexShortDataBuddyLength +
            pBreadthHeader->nVertexDwordDataBuddyLength;

        unsigned long nOffsetOfRoadSection      = sizeof(StuNetworkFileStructV5::StuBreadthHeaderInfo);
        unsigned long nOffsetOfVertex           = nOffsetOfRoadSection + sizeof(StuNetworkFileStructV5::stuRoadSection)*nCountOfRoadSection;
        unsigned long nOffsetOfRoadSectionBuddy = nOffsetOfVertex + sizeof(StuNetworkFileStructV5::stuVertex)*nCountOfVertex;
        unsigned long nOffsetOfVertexBuddy      = nOffsetOfRoadSectionBuddy + nSizeOfRoadSectionBuddy;
		//-----------------------------------------------------

		char* pRoadSectionBuddyData = NULL;
		if(nSizeOfRoadSectionBuddy > 0)
		{
			pRoadSectionBuddyData = new char[nSizeOfRoadSectionBuddy];
			ASSERT(pRoadSectionBuddyData);
			memcpy(
                pRoadSectionBuddyData,
                &pBufOfBeforeCompress[nOffsetOfRoadSectionBuddy],
                nSizeOfRoadSectionBuddy);
			pRoadSectionBuddy = pRoadSectionBuddyData;		
		}

		char* pVertexBuddyData = NULL;
		if(nSizeOfVertexBuddy > 0)
		{
			pVertexBuddyData = new char [nSizeOfVertexBuddy];
			ASSERT(pVertexBuddyData);
			memcpy(
                pVertexBuddyData,
                &pBufOfBeforeCompress[nOffsetOfVertexBuddy],
                nSizeOfVertexBuddy);
			pVertexBuddy = pVertexBuddyData;
		}
		
		
		//-----------------------------------------------------
		{
			char* pCharAddr		= NULL;
			char* pShortAddr	= NULL;
			char* pDwordAddr	= NULL;
			
			if(nSizeOfRoadSectionBuddy > 0)
			{
				pCharAddr	= pRoadSectionBuddyData;
				pShortAddr	= pRoadSectionBuddyData+pBreadthHeader->nRoadSectionCharDataBuddyLength;
				pDwordAddr	= pRoadSectionBuddyData+pBreadthHeader->nRoadSectionShortDataBuddyLength;
				
				if((*( (DWORD*)pCharAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pCharAddr	+= 4;
				}
				if((*( (DWORD*)pShortAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pShortAddr	+= 4;
				}
				if((*( (DWORD*)pDwordAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pDwordAddr	+= 4;
				}
			}
			
			int nReadNum = -1;
			switch(nLoadFlag)
			{
			case 0:
				nReadNum = pBreadthHeader->nCountOfLevelAllRoadSection;
				break;
			case 1:
				nReadNum = pBreadthHeader->nCountOfLevelAllRoadSection;
				break;
			case 2:
				nReadNum = pBreadthHeader->nCountOfLevelAllRoadSection;
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			ASSERT(nReadNum <= 20000);
			
			StuNetworkFileStructV5::stuRoadSection* pArrayOfStRoadSection = 
                (StuNetworkFileStructV5::stuRoadSection*)(pBufOfBeforeCompress+nOffsetOfRoadSection);
			for( int i = 0;	i < nReadNum;	i++ )
			{
#ifdef ANALYSE_DATA
                m_DataAnalyse.m_nRoadSectionCount++;
#endif
                CGIS_NetworkRoadSectionV5 *pRoadSection = new CGIS_NetworkRoadSectionV5;
				memcpy(pRoadSection->GetDataAddr(), &pArrayOfStRoadSection[i],sizeof(StuNetworkFileStructV5::stuRoadSection));
				pRoadSection->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
								
				ASSERTVALID(pRoadSection);
				ASSERT(pRoadSection->GetBreadthID() == pIndexInfoOfBreadth->nID);
				ASSERT((pRoadSection->GetRutID()>>15) > 0);
				(*pRoadSectionMap)[pRoadSection->GetRutID()] = pRoadSection;
			}
		}
		//--------------------------------------------------------------
		{
			char* pCharAddr		= NULL;
			char* pShortAddr	= NULL;
			char* pDwordAddr	= NULL;
			
			if(nSizeOfVertexBuddy)
			{
				pCharAddr	= pVertexBuddyData;
				pShortAddr	= pVertexBuddyData + pBreadthHeader->nVertexCharDataBuddyLength;
				pDwordAddr	= pVertexBuddyData + pBreadthHeader->nVertexShortDataBuddyLength;
				
				if((*( (DWORD*)pCharAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pCharAddr	+= 4;
				}
				if((*( (DWORD*)pShortAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pShortAddr	+= 4;
				}
				if((*( (DWORD*)pDwordAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)))
				{
					pDwordAddr	+= 4;
				}
			}
			int nReadNum = -1;
			switch(nLoadFlag)
			{
			case 0:
				nReadNum = pBreadthHeader->nCountOfLevelAllVertex;
				break;
			case 1:
				nReadNum = pBreadthHeader->nCountOfLevelAllVertex;
				break;
			case 2:
				nReadNum = pBreadthHeader->nCountOfLevelAllVertex;
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			ASSERT(nReadNum <= 20000);
			
			
			StuNetworkFileStructV5::stuVertex* pArrayOfStVertex = (StuNetworkFileStructV5::stuVertex*)(pBufOfBeforeCompress+nOffsetOfVertex);
			for(int i = 0;	i < nReadNum;	i++ )
			{
#ifdef ANALYSE_DATA
                m_DataAnalyse.m_nVertexCount++;
#endif
				
				CGIS_NetworkVertexV5 *pVertex = new CGIS_NetworkVertexV5;
				memcpy(pVertex->GetDataAddr(), &pArrayOfStVertex[i],sizeof(StuNetworkFileStructV5::stuVertex));
				pVertex->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
				ASSERTVALID(pVertex);
				ASSERT(pVertex->GetBreadthID() == pIndexInfoOfBreadth->nID);

#ifdef ANALYSE_DATA
                if(pVertex->GetEdge())
                {
                    m_DataAnalyse.m_nEdgeVertexCount++;
                }
#endif
				ASSERT((pVertex->GetVtxID()>>15) > 0);
                (*pVertexMap)[pVertex->GetVtxID()] = pVertex;
			}

		}
		delete[] pBufOfBeforeCompress;
		pBufOfBeforeCompress = NULL;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

#ifdef ANALYSE_DATA
void CNetworkFileDataV5::CDataAnalyse::Initial()
{
    memset(this,0,sizeof(CDataAnalyse)); 
    m_nMinFtrCountOfBreadth = 0xffffffff;
    m_dwMinBeforeCompressSize   = 0xffffffff;
    m_dwMinAfterCompressSize    = 0xffffffff;
}
void CNetworkFileDataV5::CDataAnalyse::DumpState()
{
	TRACE1("m_nMapCount                     :%d.\n",m_nMapCount);
	TRACE1("图幅                            :%d.\n",m_nBreadthCount);
	TRACE1("空图幅                          :%d.\n",m_nEmptyBreadthCount);
    TRACE1("MinFtrCountOfBreadth            :%d.\n",m_nMinFtrCountOfBreadth);
    TRACE1("MaxFtrCountOfBreadth            :%d.\n",m_nMaxFtrCountOfBreadth);
    TRACE1("MinBeforeCompressSize           :%d.\n",m_dwMinBeforeCompressSize);
    TRACE1("MaxBeforeCompressSize           :%d.\n",m_dwMaxBeforeCompressSize);
    TRACE1("MinAfterCompressSize            :%d.\n",m_dwMinAfterCompressSize);
    TRACE1("MaxAfterCompressSize            :%d.\n",m_dwMaxAfterCompressSize);
	TRACE1("RoadSectionCount                :%d.\n",m_nRoadSectionCount);
	TRACE1("VertexCount                     :%d.\n",m_nVertexCount);
    TRACE1("EdgeVertexCount                 :%d.\n",m_nEdgeVertexCount);
    TRACE1("TotalBeforeCompressSize         :%d.\n",m_nTotalBeforeCompressSize);
    TRACE1("TotalAfterCompressSize          :%d.\n",m_nTotalAfterCompressSize);

//    {
//        TRACE0("nCountOfSpecialSizePackageBeforeCompress:\n");
//        for(int i = 0; i < 10; i++)
//        {
//            TRACE3("[%.6dk--%.6dk)          :%d .\n",i*10,(i+1)*10,nCountOfSpecialSizePackageBeforeCompress[i]);
//        }
//        TRACE1(">100k:          :%d .\n",nCountOfSpecialSizePackageBeforeCompress[10]);
//    }
//    {
//        
//        TRACE0("nCountOfSpecialSizePackageAfterCompress:\n");
//        for(int i = 0; i < 10; i++)
//        {
//            TRACE3("[%.6dk--%.6dk)          :%d .\n",i*10,(i+1)*10,nCountOfSpecialSizePackageAfterCompress[i]);
//        }
//        TRACE1(">100k:          :%d .\n",nCountOfSpecialSizePackageAfterCompress[10]);
//    }
}

#endif

BOOL CNetworkFileDataV5::Check(LPCTSTR strFilePath)
{
    //////////////////////////////////////////////////////////////////////////
    //加载索引ID数组
    {
        CFile aFile(strFilePath,CFile::modeRead);
        DWORD dwSize = aFile.GetLength();
        int nCount = dwSize>>1;
        m_pBreadthIDArray = new unsigned short[nCount];
        aFile.Read(m_pBreadthIDArray,sizeof(unsigned short)*nCount);
        m_nSizeOfBreadthIDArray = nCount;
    }

    
    MapOfBreadth* pBreadthContainer = m_pBreadthContainer;
    MapOfBreadth::iterator iteratorOfBreadth;
    for(iteratorOfBreadth = pBreadthContainer->begin();
    iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
    {
        pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
        stuBreadthItem* pItem = aPair.second;
        
        StuNetworkFileStructV5::StuBreadthHeaderInfo* pBreadthHeader = pItem->pHeader;
        
        {
            MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;		
            
        }
        {
            MapOfVertex* pFtrContainer = pItem->pVertexContainer;
            MapOfVertex::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair <unsigned long ,CGIS_NetworkVertexV5* > aPair = *iteratorOfFtr;
                CGIS_NetworkVertexV5* pFtr = (CGIS_NetworkVertexV5*)aPair.second;
                if(pFtr->GetEdge())
                {
                    CheckEdgeVertex(pFtr);
                }
            }
            
        }
        
        
        
    }
    
    return TRUE;
}
#define FLOAT_ZERO 0.0000082
//检查边界节点
BOOL CNetworkFileDataV5::CheckEdgeVertex(CGIS_NetworkVertexV5* pVertex)
{
	ASSERT(pVertex);
	ASSERT(pVertex->GetEdge());
		
	//取到相邻路段
	unsigned long nRutID = pVertex->GetEdgeRoadSectionID();
    
	int nDetailBreadthID = nRutID>>15;
    ASSERT(nDetailBreadthID < m_nSizeOfBreadthIDArray);
    int nBreadthID = m_pBreadthIDArray[nDetailBreadthID];

    MapOfBreadth* pBreadthContainer = m_pBreadthContainer;
    stuBreadthItem* pBreadthItem = (*pBreadthContainer)[nBreadthID];
    ASSERT(pBreadthItem);
    MapOfRoadSection*   pRoadSectionContainer   = pBreadthItem->pRoadSectionContainer;
    MapOfVertex*        pVertexContainer        = pBreadthItem->pVertexContainer;
    //////////////////////////////////////////////////////////////////////////
    
	CGIS_NetworkRoadSectionV5* pRoadSection = (*pRoadSectionContainer)[nRutID];
    
	ASSERT(pRoadSection);
	ASSERT(pRoadSection->GetRutID() == nRutID );
	ASSERT(pRoadSection->GetBreadthID() == nBreadthID);
	

	//////////////////////////////////////////////////////////////////////////	
	CGIS_NetworkVertexV5*   pSVtx = (*pVertexContainer)[pRoadSection->GetStartVtxID()];
	CGIS_NetworkVertexV5*   pEVtx = (*pVertexContainer)[pRoadSection->GetEndVtxID()];
	ASSERT(pSVtx);
	ASSERT(pEVtx);
		
	//通过距离判定确实是原始节点的影子节点
	if( pSVtx->GetEdge() == 1 || pEVtx->GetEdge() == 1 )
	{
		
		ASSERT( 
			(
			fabs(pSVtx->GetLog()-pVertex->GetLog()) < FLOAT_ZERO &&
			fabs(pSVtx->GetLat()-pVertex->GetLat()) < FLOAT_ZERO 
			) 
			||
			( 
			fabs(pEVtx->GetLog()-pVertex->GetLog()) < FLOAT_ZERO &&
			fabs(pEVtx->GetLat()-pVertex->GetLat()) < FLOAT_ZERO 
			)
			);
	}
	else
		ASSERT( FALSE );
		
	return TRUE;
}