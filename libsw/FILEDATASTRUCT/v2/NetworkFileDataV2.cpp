// NetworkFileDataV2.cpp: implementation of the CNetworkFileDataV2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetworkFileDataV2.h"
#include "DebugFunctionLib/DebugFun.h"
#include <math.h>
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "./Zlib/zlib.h"
#include "./FileDataStruct/V2/GIS_NetworkRoadSectionV2.h"
#include "./FileDataStruct/V2/GIS_NetworkVertexV2.h"

#include "./filedatastruct/v2/GIS_FeatureRoadSectionV2.h"
#include "./filedatastruct/v2/SpaceFileDataV2_RoadSection.h"
//////////////////////////////////////////////////////////////////////////
static char s_DestBuf2[1000000];

#define StuNetworkFileStruct StuNetworkFileStructV2C
#define TRACE_SLEEP_BY_TIMES(n)   static unsigned char nTraceCount = 0;\
                if((nTraceCount++) == (n))\
                {\
                    nTraceCount = 0;\
                    Sleep(100);\
                }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//声明一些功能函数
unsigned char GetVertexSignOfTwoRoadSection(
    CGIS_NetworkVertexV2* pVertex,
    CGIS_NetworkRoadSectionV2* pRoadSection0,
    CGIS_NetworkRoadSectionV2* pRoadSection1
    );
unsigned char GetDirectionSignOfTwoRoadSection(
    CGIS_NetworkVertexV2* pVertex,
    CGIS_NetworkRoadSectionV2* pRoadSection0,
    CGIS_NetworkRoadSectionV2* pRoadSection1
    );
//////////////////////////////////////////////////////////////////////////
CNetworkFileDataV2::CNetworkFileDataV2()
{
    m_nLoadLayerSign = 0;
    m_pMapContainer = NULL;
    m_DataAnalyse.Initial();

    m_pSpaceFile = NULL;
}

CNetworkFileDataV2::~CNetworkFileDataV2()
{
    ClearOperation();
}

//////////////////////////////////////////////////////////////////////////
BOOL CNetworkFileDataV2::Load(LPCTSTR strFileName)
{
#ifdef ANALYSE_RUNTIME
    m_RunTimeAnalyse.Initial();
#endif  
    CFile aFile(strFileName,CFile::modeRead);
	CFile* pFile = &aFile;
	ReadNetwork(pFile);
    Check();    
	//DUMPSTATE(this);
    return FALSE;
}
//按V5格式写入文件
BOOL CNetworkFileDataV2::Write(LPCTSTR strFileName)
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

BOOL CNetworkFileDataV2::AddMapContainer(MapOfGeoMap* pMapContainer)
{ 
    if(!m_pMapContainer)
    {
        m_pMapContainer = new MapOfGeoMap;
    }

	MapOfGeoMap::iterator iteratorOfMap;
	for(iteratorOfMap = pMapContainer->begin();
	iteratorOfMap != pMapContainer->end(); iteratorOfMap++)
	{
		pair <unsigned long,stuMapItem* > aPair = *iteratorOfMap;
		stuMapItem* pItem = aPair.second;
        unsigned long nMapID = aPair.first;

        if(!(*m_pMapContainer)[nMapID])
        {
            (*m_pMapContainer)[nMapID] = pItem;
            (*pMapContainer)[nMapID] = NULL;
        }
        else
        {
            //如果此id已存在,那么把对象一个一个挂上去
            stuMapItem* pItemOld = (*m_pMapContainer)[nMapID];
            MapOfBreadth* pBreadthContainerOld = pItemOld->pBreadthContainer;

            MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;
            MapOfBreadth::iterator iteratorOfBreadth;
            for(iteratorOfBreadth = pBreadthContainer->begin();
            iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
            {
                pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
                stuBreadthItem* pItem = aPair.second;
                unsigned long nBreadthID = aPair.first;

                ASSERT(!(*pBreadthContainerOld)[nBreadthID]);
                (*pBreadthContainerOld)[nBreadthID] = pItem;
                (*pBreadthContainer)[nBreadthID] = NULL;

            }
         }
	
    }
    return TRUE;
}

BOOL CNetworkFileDataV2::AddBreadthItem(stuBreadthItem* pBreadthItem)
{
    ASSERT(pBreadthItem);
    ASSERT(m_pMapContainer);
    MapOfGeoMap* pMapContainer = m_pMapContainer;

    stuMapItem* pMapItem = (*pMapContainer)[pBreadthItem->nID];
    ASSERT(pMapItem);

    ASSERT(!(*pMapItem->pBreadthContainer)[pBreadthItem->pIndex->nBreadthMapID]);
    (*pMapItem->pBreadthContainer)[pBreadthItem->pIndex->nBreadthMapID] = pBreadthItem;

    return TRUE; 
}

BOOL CNetworkFileDataV2::ReadNetwork(CFile* pFile)
{
	ASSERT(pFile);

	if(m_pMapContainer)
	{
		ClearOperation();
	}
	m_pMapContainer = new MapOfGeoMap;

	//读头
    StuNetworkFileStructV2C :: StuHeaderInfo* m_pFileHeadInfo = new StuNetworkFileStructV2C :: StuHeaderInfo;
	pFile->Read(m_pFileHeadInfo,sizeof(StuNetworkFileStructV2C :: StuHeaderInfo));

	int nCount = m_pFileHeadInfo->nMapCount;
	//---------------------------
	//读入地图索引
	StuNetworkFileStructV2C::StuIndexInfoOfMap* pIndexInfoArray = NULL;		//地图索引信息
	pIndexInfoArray = new StuNetworkFileStructV2C::StuIndexInfoOfMap[nCount];
	memset(pIndexInfoArray,0,sizeof(StuNetworkFileStructV2C::StuIndexInfoOfMap)*nCount);
	pFile->Read(pIndexInfoArray,sizeof(StuNetworkFileStructV2C::StuIndexInfoOfMap) * nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////

    for(int i = 0; i < nCount; i++)
    {        
        stuMapItem* pItem = new stuMapItem;
        pItem->pIndex = new StuNetworkFileStructV2C::StuIndexInfoOfMap;
        pItem->pBreadthContainer = new MapOfBreadth;
        memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuNetworkFileStructV2C::StuIndexInfoOfMap));

		pFile->Seek( pItem->pIndex->nOffset + dwOffsetBase,CFile::begin);  //地图的偏移相对于文件起始位置的.所以不加头的大小.    
		ReadMap(pFile,pItem);

		(*m_pMapContainer)[i] = pItem;
    }

    delete []pIndexInfoArray;
    pIndexInfoArray = NULL;

    return TRUE;
}

BOOL CNetworkFileDataV2::ReadMap(CFile* pFile,stuMapItem* pMapItem)
{

    StuNetworkFileStructV2C::StuIndexInfoOfMap* pIndexInfoOfMap = pMapItem->pIndex;
	MapOfBreadth*& pBreadthContainer = pMapItem->pBreadthContainer;

	ASSERT(pFile);
	ASSERT(pIndexInfoOfMap);
	ASSERT(pBreadthContainer);

	//////////////////////////////////////////////////////////////////////////
    //读索引数据   
    int nCount = pIndexInfoOfMap->nBreadthCount;
    int nMapID = pIndexInfoOfMap->nID;
	StuNetworkFileStructV2C::StuIndexInfoOfBreadth* pIndexInfoArray = 
        new StuNetworkFileStructV2C::StuIndexInfoOfBreadth[nCount];
	pFile->Read(pIndexInfoArray,sizeof(StuNetworkFileStructV2C::StuIndexInfoOfBreadth) * nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////
    //读入地图数据
    for(int i = 0; i < nCount; i++)
	{	
		if(!(i%10000))
		{
			static DWORD dwtickstart = GetTickCount();
			TRACE2("----ReadBreadthV2(%.5d):%d ms--------\n",i,GetTickCount() - dwtickstart);
			dwtickstart = GetTickCount();
		}
	
		//偏移为0表示此图幅是一个空的图幅
//		if(pIndexInfoArray[i].nBeforeCompressSize == 0)
//        {
//            ASSERT(pIndexInfoArray[i].nAfterCompressSize == 0);
//			continue;
//         }
			 
        stuBreadthItem* pItem = new stuBreadthItem;
        pItem->pIndex = new StuNetworkFileStructV2C::StuIndexInfoOfBreadth;
        pItem->pHeader = new StuNetworkFileStructV2C::StuBreadthHeaderInfo;
        pItem->pRoadSectionContainer = new MapOfRoadSection;
        pItem->pVertexContainer = new MapOfVertex;
        pItem->pRoadSectionBuddy = NULL;
        pItem->pVertexBuddy = NULL;
        pItem->nID = i;

		memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuNetworkFileStructV2C::StuIndexInfoOfBreadth));

        if(pIndexInfoArray[i].nBeforeCompressSize > 0)
        {           
            pFile->Seek(pItem->pIndex->nOffset + dwOffsetBase,CFile::begin);
            BOOL bReadSuccess = FALSE;
            bReadSuccess = ReadBreadth(pFile,pItem, m_nLoadLayerSign);
            //bReadSuccess = ReadBreadthHeadInfo(pFile,pBreadthIndex,pBreadthHeader,1);
            VERIFY(bReadSuccess);
        }
       		
		(*pBreadthContainer)[i] = pItem;
		

	}

    delete []pIndexInfoArray;
    pIndexInfoArray = NULL;

    return FALSE;
}


BOOL CNetworkFileDataV2::ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem,int nLoadFlag)
{

    StuNetworkFileStructV2C::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = pBreadthItem->pIndex;
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
        
#ifdef ANALYSE_RUNTIME
        if(nBeforeCompressSize > m_RunTimeAnalyse.m_dwMaxBeforeCompressSize)
        {
            m_RunTimeAnalyse.m_dwMaxBeforeCompressSize = nBeforeCompressSize;
        }
        if(nBeforeCompressSize < m_RunTimeAnalyse.m_dwMinBeforeCompressSize)
        {
            m_RunTimeAnalyse.m_dwMinBeforeCompressSize = nBeforeCompressSize;
        }
        if(nAfterCompressSize > m_RunTimeAnalyse.m_dwMaxAfterCompressSize)
        {
            m_RunTimeAnalyse.m_dwMaxAfterCompressSize = nAfterCompressSize;
        }
        if(nAfterCompressSize < m_RunTimeAnalyse.m_dwMinAfterCompressSize)
        {
            m_RunTimeAnalyse.m_dwMinAfterCompressSize = nAfterCompressSize;
        }
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
		StuNetworkFileStructV2C::StuBreadthHeaderInfo* pBreadthHeader = (StuNetworkFileStructV2C::StuBreadthHeaderInfo*)pBufOfBeforeCompress;
		unsigned long nCountOfRoadSection = 
            pBreadthHeader->nCountOfLevel1RoadSection + 
            pBreadthHeader->nCountOfLevel2RoadSection +
            pBreadthHeader->nCountOfLevel3RoadSection;
		unsigned long nCountOfVertex = 
            pBreadthHeader->nCountOfLevel1Vertex + 
            pBreadthHeader->nCountOfLevel2Vertex +
            pBreadthHeader->nCountOfLevel3Vertex;


		unsigned long nSizeOfRoadSectionBuddy = 
            pBreadthHeader->nRoadSectionCharDataBuddyLength + 
            pBreadthHeader->nRoadSectionShortDataBuddyLength +
            pBreadthHeader->nRoadSectionDwordDataBuddyLength;
		unsigned long nSizeOfVertexBuddy = 
            pBreadthHeader->nVertexCharDataBuddyLength + 
            pBreadthHeader->nVertexShortDataBuddyLength +
            pBreadthHeader->nVertexDwordDataBuddyLength;

        unsigned long nOffsetOfRoadSection      = sizeof(StuNetworkFileStructV2C::StuBreadthHeaderInfo);
        unsigned long nOffsetOfVertex           = nOffsetOfRoadSection + sizeof(StuNetworkFileStructV2C::stuRoadSection)*nCountOfRoadSection;
        unsigned long nOffsetOfRoadSectionBuddy = nOffsetOfVertex + sizeof(StuNetworkFileStructV2C::stuVertex)*nCountOfVertex;
        unsigned long nOffsetOfVertexBuddy      = nOffsetOfRoadSectionBuddy + nSizeOfRoadSectionBuddy;

        memcpy(pBreadthItem->pHeader,pBreadthHeader,sizeof(StuNetworkFileStructV2C::StuBreadthHeaderInfo));
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
				nReadNum = pBreadthHeader->nCountOfLevel1RoadSection;
				break;
			case 1:
				nReadNum = 
                    pBreadthHeader->nCountOfLevel1RoadSection + 
                    pBreadthHeader->nCountOfLevel2RoadSection;
				break;
			case 2:
				nReadNum = 
                    pBreadthHeader->nCountOfLevel1RoadSection + 
                    pBreadthHeader->nCountOfLevel2RoadSection +
                    pBreadthHeader->nCountOfLevel3RoadSection;
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			ASSERT(nReadNum <= 20000);
			
			StuNetworkFileStructV2C::stuRoadSection* pArrayOfStRoadSection = 
                (StuNetworkFileStructV2C::stuRoadSection*)(pBufOfBeforeCompress+nOffsetOfRoadSection);
			for( int i = 0;	i < nReadNum;	i++ )
			{
				CGIS_NetworkRoadSectionV2 *pRoadSection = new CGIS_NetworkRoadSectionV2;
				memcpy(pRoadSection, &pArrayOfStRoadSection[i],sizeof(StuNetworkFileStructV2C::stuRoadSection));
				pRoadSection->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
								
				ASSERTVALID(pRoadSection);
				ASSERT(pRoadSection->GetRutID() == i);
				(*pRoadSectionMap)[i] = pRoadSection;
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
				nReadNum = pBreadthHeader->nCountOfLevel1Vertex;
				break;
			case 1:
				nReadNum = pBreadthHeader->nCountOfLevel1Vertex + pBreadthHeader->nCountOfLevel2Vertex;
				break;
			case 2:
				nReadNum = 
                    pBreadthHeader->nCountOfLevel1Vertex + 
                    pBreadthHeader->nCountOfLevel2Vertex + 
                    pBreadthHeader->nCountOfLevel3Vertex;
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			ASSERT(nReadNum <= 20000);
			
			
			StuNetworkFileStructV2C::stuVertex* pArrayOfStVertex = (StuNetworkFileStructV2C::stuVertex*)(pBufOfBeforeCompress+nOffsetOfVertex);
			for(int i = 0;	i < nReadNum;	i++ )
			{
				
				CGIS_NetworkVertexV2 *pVertex = new CGIS_NetworkVertexV2;
				memcpy(pVertex, &pArrayOfStVertex[i],sizeof(StuNetworkFileStructV2C::stuVertex));
				pVertex->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
				ASSERTVALID(pVertex);

                ASSERT(pVertex->GetVtxID() == i);
                (*pVertexMap)[i] = pVertex;
			}

		}
		delete[] pBufOfBeforeCompress;
		pBufOfBeforeCompress = NULL;
	}
	
	return TRUE;
}
BOOL CNetworkFileDataV2::ReadBreadthHeadInfo(
		CFile* pFile,
		StuNetworkFileStruct::StuIndexInfoOfBreadth* pIndexInfoOfBreadth,
		StuNetworkFileStruct::StuBreadthHeaderInfo* pBreadthHeader,
		int nLoadFlag)
{
	ASSERT(pFile);
	ASSERT(pIndexInfoOfBreadth);
	ASSERT(pBreadthHeader);
    		
    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////

    char *pFBuf = NULL;
	
	{
		//偏移为0表示此图幅是一个空的图幅
		if(pIndexInfoOfBreadth->nOffset == 0)
			return FALSE;

		int nReadNum = -1;
		int nBeforeCompressSize = -1;
		int nAfterCompressSize = -1;
		DWORD dwLayerOffset = 0;
		switch(nLoadFlag)
		{
		case 2:
			nBeforeCompressSize = pIndexInfoOfBreadth->nBeforeCompressSize;
			nAfterCompressSize = pIndexInfoOfBreadth->nAfterCompressSize;
			dwLayerOffset = pIndexInfoOfBreadth->nOffset;

			break;
		case 1:
			nBeforeCompressSize = pIndexInfoOfBreadth->nBeforeCompressSize;
			nAfterCompressSize = pIndexInfoOfBreadth->nAfterCompressSize;
			dwLayerOffset = pIndexInfoOfBreadth->nOffset;
			break;
		case 0:
			nBeforeCompressSize = pIndexInfoOfBreadth->nBeforeCompressSize;
			nAfterCompressSize = pIndexInfoOfBreadth->nAfterCompressSize;
			dwLayerOffset = pIndexInfoOfBreadth->nOffset;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		
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
            pBufOfAfterCompress = NULL;
			//ASSERT(nBeforeCompressSize == dwSize);
		}
		//-----------------------------------------------------

		//-----------------------------------------------------
		//处理图幅头数据
        memcpy(pBreadthHeader,pBufOfBeforeCompress,sizeof(StuNetworkFileStruct::StuBreadthHeaderInfo));
	
		delete[] pBufOfBeforeCompress;
		pBufOfBeforeCompress = NULL;
	}
	
	return TRUE;	
}

//////////////////////////////////////////////////////////////////////////
BOOL CNetworkFileDataV2::WriteNetwork   (CFile* pFile)
{
    ASSERT(m_pMapContainer);
    

    //////////////////////////////////////////////////////////////////////////
    MapOfGeoMap* pMapContainer =m_pMapContainer;
	//---------------------------
	//构造头,写头	
    StuNetworkFileStruct::StuHeaderInfo aHeadInfo;
    memset(&aHeadInfo,0,sizeof(aHeadInfo));
    strcpy(aHeadInfo.chSign,"RUT_V2");
    aHeadInfo.nMapCount = pMapContainer->size(); 
    pFile->Write(&aHeadInfo,sizeof(aHeadInfo));
    
      
    //////////////////////////////////////////////////////////////////////////
    CMemFile aIndexFile;
    CMemFile aDataFile;

	MapOfGeoMap::iterator iteratorOfMap;
	for(iteratorOfMap = pMapContainer->begin();
	iteratorOfMap != pMapContainer->end(); iteratorOfMap++)
	{
		pair <unsigned long,stuMapItem* > aPair = *iteratorOfMap;
		stuMapItem* pItem = aPair.second;
        ASSERT(!pItem->pIndex);
        pItem->pIndex = new StuNetworkFileStruct::StuIndexInfoOfMap;
        pItem->pIndex->nID = aPair.first;
        pItem->pIndex->nBreadthCount = pItem->pBreadthContainer->size();

        WriteMap(&aDataFile,pItem->pIndex,pItem->pBreadthContainer);	
        aIndexFile.Write(pItem->pIndex,sizeof(StuNetworkFileStruct::StuIndexInfoOfMap));
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //写入索引文件
    {
        unsigned long nSrcSize = aIndexFile.GetLength();
        unsigned char* pSrcData = aIndexFile.Detach();
        pFile->Write(pSrcData,nSrcSize);
        delete []pSrcData;
        pSrcData = NULL;
    }
    //写入数据文件
    {
        unsigned long nSrcSize = aDataFile.GetLength();
        unsigned char* pSrcData = aDataFile.Detach();
        pFile->Write(pSrcData,nSrcSize);
        delete []pSrcData;
        pSrcData = NULL;
    }
    
 
    return TRUE;
}

BOOL CNetworkFileDataV2::WriteMap       
(
 CFile* pFile,
 StuNetworkFileStructV2C::StuIndexInfoOfMap* pIndexInfoOfMap,
 MapOfBreadth* pBreadthContainer
 )
{
    CMemFile aIndexFile;
    CMemFile aDataFile;

    //////////////////////////////////////////////////////////////////////////
    //在索引文件和数据文件中分别写入.
    MapOfBreadth::iterator iteratorOfBreadth;
    for(iteratorOfBreadth = pBreadthContainer->begin();
    iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
    {
        pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
        stuBreadthItem* pItem = aPair.second;
        unsigned long nBreadthMapID = aPair.first;

        if(pItem->pIndex)
        {
            delete pItem->pIndex;
            pItem->pIndex = NULL;
        }
        pItem->pIndex = new StuNetworkFileStruct :: StuIndexInfoOfBreadth;
        pItem->pIndex->nBreadthMapID = nBreadthMapID;

        WriteBreadth(&aDataFile,pItem);
        aIndexFile.Write(pItem->pIndex,sizeof(StuNetworkFileStruct :: StuIndexInfoOfBreadth));
    }
          
    //////////////////////////////////////////////////////////////////////////
    //写入索引文件
    {
        unsigned long nSrcSize = aIndexFile.GetLength();
        unsigned char* pSrcData = aIndexFile.Detach();
        pFile->Write(pSrcData,nSrcSize);
        delete []pSrcData;
        pSrcData = NULL;
    }
    //写入数据文件
    {
        unsigned long nSrcSize = aDataFile.GetLength();
        unsigned char* pSrcData = aDataFile.Detach();
        pFile->Write(pSrcData,nSrcSize);
        delete []pSrcData;
        pSrcData = NULL;
    }

    return TRUE; 
}

static char s_BuddyBuf[10000000];
BOOL CNetworkFileDataV2::WriteBreadth   (
        CFile* pFile,
        stuBreadthItem* pItem)
{
    StuNetworkFileStruct::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = pItem->pIndex;
	MapOfRoadSection* pRoadSectionContainer = pItem->pRoadSectionContainer;
    MapOfVertex* pVertexContainer           = pItem->pVertexContainer;
	StuNetworkFileStruct::StuBreadthHeaderInfo* pHeader = pItem->pHeader;

        
    ASSERT(pFile);
    ASSERT(pIndexInfoOfBreadth);
    ASSERT(pRoadSectionContainer);
    ASSERT(pVertexContainer);
    ASSERT(pHeader);

	CMemFile aMemFile;
	{
		CFile* pFile = &aMemFile;

        DWORD dwHeadPos = pFile->GetPosition();
		//写入图幅头
		{
			pFile->Write(pHeader,sizeof(StuNetworkFileStruct::StuBreadthHeaderInfo));
		}
        //////////////////////////////////////////////////////////////////////////
		//写入所有路段
		{
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pRoadSectionContainer->begin();
            iteratorOfFtr != pRoadSectionContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_NetworkRoadSectionV2*> aPair = *iteratorOfFtr;
                CGIS_NetworkRoadSectionV2* pFtr = aPair.second;
                unsigned long nIndex = aPair.first;
                
                pFile->Write(pFtr,sizeof(CGIS_NetworkRoadSectionV2));
            }

		}
		
		//写入所有节点
		{
            MapOfVertex::iterator iteratorOfFtr;
            for(iteratorOfFtr = pVertexContainer->begin();
            iteratorOfFtr != pVertexContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_NetworkVertexV2*> aPair = *iteratorOfFtr;
                CGIS_NetworkVertexV2* pFtr = aPair.second;
                unsigned long nIndex = aPair.first;
                
                pFile->Write(pFtr,sizeof(CGIS_NetworkVertexV2));
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

            pHeader->nRoadSectionCharDataBuddyLength = nCharDataBuddyLength;
            pHeader->nRoadSectionShortDataBuddyLength = nShortDataBuddyLength;
            pHeader->nRoadSectionDwordDataBuddyLength = nDwordDataBuddyLength;
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

            pHeader->nVertexCharDataBuddyLength = nCharDataBuddyLength;
            pHeader->nVertexShortDataBuddyLength = nShortDataBuddyLength;
            pHeader->nVertexDwordDataBuddyLength = nDwordDataBuddyLength;
        }

        //////////////////////////////////////////////////////////////////////////
        //写入图幅头的有效数据
        pFile->Seek(dwHeadPos,CFile::begin);
		pFile->Write(pHeader,sizeof(StuNetworkFileStruct::StuBreadthHeaderInfo));

	}

	//压缩数据
	unsigned long nSrcSize = aMemFile.GetLength();
	unsigned char* pSrcData = aMemFile.Detach();
	unsigned char* pDestBuf =(unsigned char*) s_DestBuf2;//new unsigned char[0x1000000];
	unsigned long nDestSize = compressBound(nSrcSize);
    ASSERT(nDestSize < 1000000);
	compress2(pDestBuf,&nDestSize,pSrcData,nSrcSize,Z_DEFAULT_COMPRESSION);
	ASSERT(nDestSize > 0);

    pIndexInfoOfBreadth->nBeforeCompressSize = nSrcSize;
    pIndexInfoOfBreadth->nAfterCompressSize = nDestSize;

	//写入数据到文件
	pFile->Write(pDestBuf,nDestSize);
	pFile->Flush();

	return FALSE;
}

BOOL CNetworkFileDataV2::GetBuddyDataOfRoadSectionSize(
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
        pair < unsigned long,CGIS_NetworkRoadSectionV2*> aPair = *iteratorOfFtr;
        CGIS_NetworkRoadSectionV2* pFtr = aPair.second;
        unsigned long nID = aPair.first;
        
        nNum = pFtr->GetLimitNormalNum();
        ASSERT(nNum < 11);
        if( nNum > 0 )
        {
            nSize = sizeof(unsigned long) * nNum;
            pCurrent += nSize;
        }
        
        
        nNum = pFtr->GetLimitReverseNum();
        ASSERT(nNum < 11);
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


BOOL CNetworkFileDataV2::CollectBuddyDataOfRoadSection(
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
            pair < unsigned long,CGIS_NetworkRoadSectionV2*> aPair = *iteratorOfFtr;
            CGIS_NetworkRoadSectionV2* pFtr = aPair.second;
            unsigned long nID = aPair.first;
            
            nNum = pFtr->GetLimitNormalNum();
            ASSERT(nNum < 11);
            if( nNum > 0 )
            {
                nSize = sizeof(unsigned long) * nNum;
                memcpy(pCurrent,pFtr->GetLimitNormalList(),nSize);
                pCurrent += nSize;
            }
            
            
            nNum = pFtr->GetLimitReverseNum();
            ASSERT(nNum < 11);
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

BOOL CNetworkFileDataV2::GetBuddyDataOfVertexSize(
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
            pair < unsigned long,CGIS_NetworkVertexV2*> aPair = *iteratorOfFtr;
            CGIS_NetworkVertexV2* pFtr = aPair.second;
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
            pair < unsigned long,CGIS_NetworkVertexV2*> aPair = *iteratorOfFtr;
            CGIS_NetworkVertexV2* pFtr = aPair.second;
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
                CGIS_NetworkVertexV2::ST_GEO_VEDGE* pEdgeInfo = (CGIS_NetworkVertexV2::ST_GEO_VEDGE*)pCurrent;
                pCurrent += sizeof(CGIS_NetworkVertexV2::ST_GEO_VEDGE);
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
BOOL CNetworkFileDataV2::CollectBuddyDataOfVertex(
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
            pair < unsigned long,CGIS_NetworkVertexV2*> aPair = *iteratorOfFtr;
            CGIS_NetworkVertexV2* pFtr = aPair.second;
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
            pair < unsigned long,CGIS_NetworkVertexV2*> aPair = *iteratorOfFtr;
            CGIS_NetworkVertexV2* pFtr = aPair.second;
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
                CGIS_NetworkVertexV2::ST_GEO_VEDGE* pEdgeInfo = (CGIS_NetworkVertexV2::ST_GEO_VEDGE*)pCurrent;
                pEdgeInfo->nRoadSectionID = pFtr->GetEdgeRoadSectionID();
                pCurrent += sizeof(CGIS_NetworkVertexV2::ST_GEO_VEDGE);
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

#ifdef _DEBUG
void CNetworkFileDataV2::DumpState()
{
    TRACE0("********************************************************************\n");
    TRACE0("CNetworkFileDataV2 Info:\n");
	//    TRACE1("m_BreadthContainerOfRoadSection : %d.\n",m_BreadthContainerOfVertex.size());
    TRACE1("RoadSectionSize        : %d.\n",sizeof(CGIS_NetworkRoadSectionV2));
    TRACE1("RoadSectionCount       : %d.\n",GetRoadSectionCount());
    TRACE1("VertexSize             : %d.\n",sizeof(CGIS_NetworkVertexV2));
    TRACE1("VertexCount            : %d.\n",GetVertexCount());
    TRACE0("********************************************************************\n");
    
#ifdef ANALYSE_RUNTIME
    m_RunTimeAnalyse.DumpState();
#endif
    m_DataAnalyse.DumpState();
}
void CNetworkFileDataV2::AssertValid() const
{
    
}
#endif
  

BOOL CNetworkFileDataV2::InitialOperation()
{
    return FALSE;
}

BOOL CNetworkFileDataV2::ClearOperation()
{
	if(!m_pMapContainer)
		return FALSE;

    MapOfGeoMap* pMapContainer = m_pMapContainer;
	MapOfGeoMap::iterator iteratorOfMap;
	for(iteratorOfMap = pMapContainer->begin();
	iteratorOfMap != pMapContainer->end(); iteratorOfMap++)
	{
		pair <unsigned long,stuMapItem* > aPair = *iteratorOfMap;
		stuMapItem* pItem = aPair.second;

		MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;
        MapOfBreadth::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = pBreadthContainer->begin();
        iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
			stuBreadthItem* pItem = aPair.second;
            {
				MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;
					
				MapOfRoadSection::iterator iteratorOfFtr;
				for(iteratorOfFtr = pFtrContainer->begin();
				iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
				{
					pair <unsigned long ,CGIS_NetworkRoadSectionV2* > aPair = *iteratorOfFtr;
					CGIS_NetworkRoadSectionV2* pFtr = aPair.second;
					ASSERT(pFtr);
					delete pFtr; 
					pFtr = NULL;
				}
				pFtrContainer->clear();
				delete pFtrContainer;
			}
			pItem->pRoadSectionContainer = NULL;
			{
				MapOfVertex* pFtrContainer = pItem->pVertexContainer;
				
				MapOfVertex::iterator iteratorOfFtr;
				for(iteratorOfFtr = pFtrContainer->begin();
				iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
				{
					pair <unsigned long ,CGIS_NetworkVertexV2* > aPair = *iteratorOfFtr;
					CGIS_NetworkVertexV2* pFtr = aPair.second;
					ASSERT(pFtr);
					delete pFtr; 
					pFtr = NULL;
				}
				pFtrContainer->clear();
				delete pFtrContainer;
			}
			pItem->pVertexContainer = NULL;

			if(pItem->pIndex)
			{
				delete pItem->pIndex;
				pItem->pIndex = NULL;
			}
			if(pItem->pHeader)
			{
				delete pItem->pHeader;
				pItem->pHeader = NULL;
			}

			delete pItem;
			pItem = NULL;
        }
		pBreadthContainer->clear();
		delete pBreadthContainer;
		if(pItem->pIndex)
		{
			delete pItem->pIndex;
			pItem->pIndex = NULL;
		}
		delete pItem;
		pItem = NULL;
    }
    
    pMapContainer->clear();
    delete pMapContainer;
    pMapContainer = NULL;
    
    //////////////////////////////////////////////////////////////////////////
    m_pMapContainer = NULL;
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
unsigned long CNetworkFileDataV2::GetRoadSectionCount()
{
    unsigned long nCount = 0;

    MapOfGeoMap* pMapContainer = m_pMapContainer;
	MapOfGeoMap::iterator iteratorOfMap;
	for(iteratorOfMap = pMapContainer->begin();
	iteratorOfMap != pMapContainer->end(); iteratorOfMap++)
	{
		pair <unsigned long,stuMapItem* > aPair = *iteratorOfMap;
		stuMapItem* pItem = aPair.second;
		


		MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;
        MapOfBreadth::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = pBreadthContainer->begin();
        iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
			stuBreadthItem* pItem = aPair.second;

			StuNetworkFileStruct::StuBreadthHeaderInfo* pBreadthHeader = pItem->pHeader;

            {
				MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;		
                nCount += pFtrContainer->size();
			}
			{
				MapOfVertex* pFtrContainer = pItem->pVertexContainer;
                
			}

        }

    }
    //////////////////////////////////////////////////////////////////////////
    
    return nCount;
}

unsigned long CNetworkFileDataV2::GetVertexCount()
{
    unsigned long nCount = 0;

	MapOfGeoMap::iterator iteratorOfMap;
	for(iteratorOfMap = m_pMapContainer->begin();
	iteratorOfMap != m_pMapContainer->end(); iteratorOfMap++)
	{
		pair <unsigned long,stuMapItem* > aPair = *iteratorOfMap;
		stuMapItem* pItem = aPair.second;

		MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;
        MapOfBreadth::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = pBreadthContainer->begin();
        iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
			stuBreadthItem* pItem = aPair.second;
			StuNetworkFileStruct::StuBreadthHeaderInfo* pBreadthHeader = pItem->pHeader;

            {
				MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;
			}
			{
				MapOfVertex* pFtrContainer = pItem->pVertexContainer;
                nCount += pFtrContainer->size();
			}

        }

    }
    //////////////////////////////////////////////////////////////////////////
    
    return nCount;
}
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef ANALYSE_RUNTIME
void CNetworkFileDataV2::CRunTimeAnalyse::Initial()
{
    memset(this,0,sizeof(CRunTimeAnalyse));

    m_nMinFtrCountOfBreadth = 0xffffffff;
    m_dwMinBeforeCompressSize   = 0xffffffff;
    m_dwMinAfterCompressSize    = 0xffffffff;
    
}
void CNetworkFileDataV2::CRunTimeAnalyse::DumpState()
{
    TRACE0("********************************************************************\n");
	TRACE1("m_nMapCount                     :%d.\n",m_nMapCount);
	TRACE1("m_nFtrCount                     :%d.\n",m_nFtrCount);
	TRACE1("图幅                            :%d.\n",m_nBreadthCount);
	TRACE1("空图幅                          :%d.\n",m_nEmptyBreadthCount);
    TRACE1("MinFtrCountOfBreadth            :%d.\n",m_nMinFtrCountOfBreadth);
    TRACE1("MaxFtrCountOfBreadth            :%d.\n",m_nMaxFtrCountOfBreadth);
    TRACE1("MinBeforeCompressSize           :%d.\n",m_dwMinBeforeCompressSize);
    TRACE1("MaxBeforeCompressSize           :%d.\n",m_dwMaxBeforeCompressSize);
    TRACE1("MinAfterCompressSize            :%d.\n",m_dwMinAfterCompressSize);
    TRACE1("MaxAfterCompressSize            :%d.\n",m_dwMaxAfterCompressSize);
    TRACE1("EdgeExtractCount                :%d.\n",m_nEdgeExtractCount);
    TRACE1("CountOfExtractVertexWhichHave3Edge                :%d.\n",m_nCountOfExtractVertexWhichHave3Edge);
    TRACE0("********************************************************************\n");


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
//     }
}

#endif

void CNetworkFileDataV2::CDataAnalyse::Initial()
{
    memset(this,0,sizeof(CDataAnalyse));

    m_nMinFtrCountOfBreadth = 0xffffffff;
    m_pRoadSectionData = new CRoadSectionData;
    m_pVertexData = new CVertexData;
    
}
void CNetworkFileDataV2::CDataAnalyse::DumpState()
{

    TRACE0("********************************************************************\n");
    TRACE0("CNetworkFileDataV2CDataAnalyse Info:\n");  
	TRACE1("MapCount                     :%d.\n",m_nMapCount);
	TRACE1("BreadthCount                    :%d.\n",m_nBreadthCount);
	TRACE1("m_nFtrTotalCount                     :%d.\n",m_nFtrCount);
	TRACE1("EmptyBreadthCount               :%d.\n",m_nEmptyBreadthCount);
    TRACE1("MinFtrCountOfBreadth            :%d.\n",m_nMinFtrCountOfBreadth);
    TRACE1("MaxFtrCountOfBreadth            :%d.\n",m_nMaxFtrCountOfBreadth);
    TRACE1("EdgeVertexCount                 :%d.\n",m_nEdgeVertexCount);

    TRACE1("Vertex have 0 edge:             :%d .\n",m_nVertexWithSpecialEdgeCount[0]);
	TRACE1("Vertex have 1 edge              :%d .\n",m_nVertexWithSpecialEdgeCount[1]);
	TRACE1("Vertex have 2 edge              :%d .\n",m_nVertexWithSpecialEdgeCount[2]);
	TRACE1("Vertex have 3 edge              :%d .\n",m_nVertexWithSpecialEdgeCount[3]);
	TRACE1("Vertex have 4 edge              :%d .\n",m_nVertexWithSpecialEdgeCount[4]);
	TRACE1("Vertex have 5 edge              :%d .\n",m_nVertexWithSpecialEdgeCount[5]);
	TRACE1("Vertex have 6 edge              :%d .\n",m_nVertexWithSpecialEdgeCount[6]);
	TRACE1("Vertex have 7 edge              :%d .\n",m_nVertexWithSpecialEdgeCount[7]);
	TRACE1("Vertex have 8 edge              :%d .\n",m_nVertexWithSpecialEdgeCount[8]);
	TRACE1("Vertex have 9 edge              :%d .\n",m_nVertexWithSpecialEdgeCount[9]);
	TRACE1("Vertex have more than 10 edge   :%d .\n",m_nVertexWithSpecialEdgeCount[10]);
    TRACE0("********************************************************************\n");


    TRACE1("m_nTempCount = %d\n",m_nTempCount);

}

BOOL CNetworkFileDataV2::Extract()
{
    ExtractNetwork();
    Check();
    DUMPSTATE(this);
    return TRUE;
}
BOOL CNetworkFileDataV2::Analyse()
{
    AnalyseNetwork();
    DUMPSTATE(this);
    return TRUE;
}

BOOL CNetworkFileDataV2::AnalyseNetwork()
{
    ASSERT(m_pMapContainer);
    MapOfGeoMap* pMapContainer = m_pMapContainer;
    //////////////////////////////////////////////////////////////////////////


    MapOfGeoMap::iterator iteratorOfMap;
    for(iteratorOfMap = pMapContainer->begin();
    iteratorOfMap != pMapContainer->end(); iteratorOfMap++)
    {
        pair <unsigned long,stuMapItem* > aPair = *iteratorOfMap;
        stuMapItem* pItem = aPair.second;
        
        MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;
        MapOfBreadth::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = pBreadthContainer->begin();
        iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
            stuBreadthItem* pItem = aPair.second;
            
            StuNetworkFileStruct::StuBreadthHeaderInfo* pBreadthHeader = pItem->pHeader;
            
            {
                MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;	
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair <unsigned long ,CGIS_NetworkRoadSectionV2* > aPair = *iteratorOfFtr;
                    CGIS_NetworkRoadSectionV2* pFtr = (CGIS_NetworkRoadSectionV2*)aPair.second;

                    //////////////////////////////////////////////////////////////////////////
                    //对类型的统计                   
                    {
                        int nRutType = pFtr->GetRutType();
                        int nRutLevel = pFtr->GetType();
                        int nRutLayer = 0;
                        if(pFtr->GetRutID() < pBreadthHeader->nCountOfLevel1RoadSection)
                        {
                            nRutLayer = 0;
                        }
                        else if(pFtr->GetRutID() < (pBreadthHeader->nCountOfLevel2RoadSection+pBreadthHeader->nCountOfLevel1RoadSection))
                        {
                            nRutLayer = 1;
                        }
                        else
                        {
                            nRutLayer = 2;
                        }


                        {
                            CDataAnalyse::MapOfInt_4Dimension* pContainer = &m_DataAnalyse.m_pRoadSectionData->TypeCount;
                            CDataAnalyse::MapOfInt_4Dimension::iterator iteratorOfFind =  pContainer->find(nRutType);
                            if(iteratorOfFind == pContainer->end())
                            {
                                (*pContainer)[nRutType] = new CDataAnalyse::MapOfInt_3Dimension;
                            }
                        }

                        {
                            CDataAnalyse::MapOfInt_3Dimension* pContainer = m_DataAnalyse.m_pRoadSectionData->TypeCount[nRutType];
                            CDataAnalyse::MapOfInt_3Dimension::iterator iteratorOfFind = pContainer->find(nRutLevel);
                            if(iteratorOfFind == pContainer->end())
                            {
                                (*pContainer)[nRutLevel]= new CDataAnalyse::MapOfInt_2Dimension;
                            }
                        }

                        {
                            CDataAnalyse::MapOfInt_2Dimension* pContainer = (*m_DataAnalyse.m_pRoadSectionData->TypeCount[nRutType])[nRutLevel];
                            CDataAnalyse::MapOfInt_2Dimension::iterator iteratorOfFind = pContainer->find(nRutLayer);
                            if(iteratorOfFind == pContainer->end())
                            {
                                (*pContainer)[nRutLayer] = 0;
                            }
                        }
                        
                            

                        (*(*m_DataAnalyse.m_pRoadSectionData->TypeCount[nRutType])[nRutLevel])[nRutLayer]++;     
                    }
                    //////////////////////////////////////////////////////////////////////////
                    //对等级的统计
                    {
                        int nRutType = pFtr->GetRutType();
                        int nRutLevel = pFtr->GetType();
                        int nRutLayer = 0;
                        if(pFtr->GetRutID() < pBreadthHeader->nCountOfLevel1RoadSection)
                        {
                            nRutLayer = 0;
                        }
                        else if(pFtr->GetRutID() < (pBreadthHeader->nCountOfLevel2RoadSection+pBreadthHeader->nCountOfLevel1RoadSection))
                        {
                            nRutLayer = 1;
                        }
                        else
                        {
                            nRutLayer = 2;
                        }


                        {
                            CDataAnalyse::MapOfInt_4Dimension* pContainer = &m_DataAnalyse.m_pRoadSectionData->LevelCount;
                            CDataAnalyse::MapOfInt_4Dimension::iterator iteratorOfFind = pContainer->find(nRutLevel);
                            if(iteratorOfFind == pContainer->end())
                            {
                                (*pContainer)[nRutLevel] = new CDataAnalyse::MapOfInt_3Dimension;
                            }
                        }

                        {
                            CDataAnalyse::MapOfInt_3Dimension* pContainer = m_DataAnalyse.m_pRoadSectionData->LevelCount[nRutLevel];
                            CDataAnalyse::MapOfInt_3Dimension::iterator iteratorOfFind = pContainer->find(nRutType);
                            if(iteratorOfFind == pContainer->end())
                            {
                                (*pContainer)[nRutType]= new CDataAnalyse::MapOfInt_2Dimension;
                            }
                        }

                        {
                            CDataAnalyse::MapOfInt_2Dimension* pContainer = (*m_DataAnalyse.m_pRoadSectionData->LevelCount[nRutLevel])[nRutType];
                            CDataAnalyse::MapOfInt_2Dimension::iterator iteratorOfFind = pContainer->find(nRutLayer);
                            if(iteratorOfFind == pContainer->end())
                            {
                                (*pContainer)[nRutLayer] = 0;
                            }

                        }
                        
                            

                        (*(*m_DataAnalyse.m_pRoadSectionData->LevelCount[nRutLevel])[nRutType])[nRutLayer]++;     
                    }
                    //////////////////////////////////////////////////////////////////////////
                    //////////////////////////////////////////////////////////////////////////
                    //对层的统计                   
                    {
                        int nRutType = pFtr->GetRutType();
                        int nRutLevel = pFtr->GetType();
                        int nRutLayer = 0;
                        if(pFtr->GetRutID() < pBreadthHeader->nCountOfLevel1RoadSection)
                        {
                            nRutLayer = 0;
                        }
                        else if(pFtr->GetRutID() < (pBreadthHeader->nCountOfLevel2RoadSection+pBreadthHeader->nCountOfLevel1RoadSection))
                        {
                            nRutLayer = 1;
                        }
                        else
                        {
                            nRutLayer = 2;
                        }
                        
                        
                        {
                            CDataAnalyse::MapOfInt_4Dimension* pContainer = &m_DataAnalyse.m_pRoadSectionData->LayerCount;
                            CDataAnalyse::MapOfInt_4Dimension::iterator iteratorOfFind =  pContainer->find(nRutLayer);
                            if(iteratorOfFind == pContainer->end())
                            {
                                (*pContainer)[nRutLayer] = new CDataAnalyse::MapOfInt_3Dimension;
                            }
                        }
                        
                        {
                            CDataAnalyse::MapOfInt_3Dimension* pContainer = m_DataAnalyse.m_pRoadSectionData->LayerCount[nRutLayer];
                            CDataAnalyse::MapOfInt_3Dimension::iterator iteratorOfFind = pContainer->find(nRutLevel);
                            if(iteratorOfFind == pContainer->end())
                            {
                                (*pContainer)[nRutLevel]= new CDataAnalyse::MapOfInt_2Dimension;
                            }
                        }
                        
                        {
                            CDataAnalyse::MapOfInt_2Dimension* pContainer = (*m_DataAnalyse.m_pRoadSectionData->LayerCount[nRutLayer])[nRutLevel];
                            CDataAnalyse::MapOfInt_2Dimension::iterator iteratorOfFind = pContainer->find(nRutType);
                            if(iteratorOfFind == pContainer->end())
                            {
                                (*pContainer)[nRutType] = 0;
                            }
                            
                        }
                        
                        
                        
                        (*(*m_DataAnalyse.m_pRoadSectionData->LayerCount[nRutLayer])[nRutLevel])[nRutType]++;     
                    }
                    
                    
                    
                }
            }
            //////////////////////////////////////////////////////////////////////////
            //对节点进行分析.
            {
                MapOfVertex* pFtrContainer = pItem->pVertexContainer;
                MapOfVertex::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair <unsigned long ,CGIS_NetworkVertexV2* > aPair = *iteratorOfFtr;
                    CGIS_NetworkVertexV2* pFtr = (CGIS_NetworkVertexV2*)aPair.second;
                    
                    if(pFtr->GetEdge())
                    {
                        m_DataAnalyse.m_nEdgeVertexCount++;
                    }
                    else
                    {
                        if(pFtr->GetRoadSectionNum() < 10)
                        {                           
                            m_DataAnalyse.m_nVertexWithSpecialEdgeCount[pFtr->GetRoadSectionNum()]++;
                        }
                        else
                        {
                            m_DataAnalyse.m_nVertexWithSpecialEdgeCount[10]++;
                        }
                    }

                    if(pFtr->GetVtxID() < pBreadthHeader->nCountOfLevel1Vertex)
                    {
                        AnalyseVertex(pItem,pFtr);
                    }
                    
                }
            }
                    
        }
        
    }
    

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    {

        TRACE0("====Level Count====\n");
        CDataAnalyse::MapOfInt_4Dimension* pContainer0 = &m_DataAnalyse.m_pRoadSectionData->LevelCount;
        CDataAnalyse::MapOfInt_4Dimension::iterator iterator0;
        for(iterator0 = pContainer0->begin();
        iterator0 != pContainer0->end(); iterator0++)
        {
            pair <int,CDataAnalyse::MapOfInt_3Dimension*> aPair = *iterator0;
            CDataAnalyse::MapOfInt_3Dimension* pContainer1 = (CDataAnalyse::MapOfInt_3Dimension*)aPair.second;
            TRACE2("[%d] = %d\n",aPair.first,pContainer1->size());

            CDataAnalyse::MapOfInt_3Dimension::iterator iterator1;
            for(iterator1 = pContainer1->begin();
            iterator1 != pContainer1->end(); iterator1++)
            {
                pair <int,CDataAnalyse::MapOfInt_2Dimension*> aPair = *iterator1;             
                CDataAnalyse::MapOfInt_2Dimension* pContainer2 = (CDataAnalyse::MapOfInt_2Dimension*)aPair.second;
                TRACE2("\t[%d] = %d\n",aPair.first,pContainer2->size());
                
                CDataAnalyse::MapOfInt_2Dimension::iterator iterator2;
                for(iterator2 = pContainer2->begin();
                iterator2 != pContainer2->end(); iterator2++)
                {
                    pair <int,int> aPair = *iterator2;
                    int nCount = aPair.second;
                    
                    TRACE2("\t\t[%d] = %d\n",aPair.first,nCount);
                }
            }
        }
        
    }
    //////////////////////////////////////////////////////////////////////////
    {

        TRACE0("====Type Count====\n");
        CDataAnalyse::MapOfInt_4Dimension* pContainer0 = &m_DataAnalyse.m_pRoadSectionData->TypeCount;
        CDataAnalyse::MapOfInt_4Dimension::iterator iterator0;
        for(iterator0 = pContainer0->begin();
        iterator0 != pContainer0->end(); iterator0++)
        {
            pair <int,CDataAnalyse::MapOfInt_3Dimension*> aPair = *iterator0;
            CDataAnalyse::MapOfInt_3Dimension* pContainer1 = (CDataAnalyse::MapOfInt_3Dimension*)aPair.second;
            TRACE2("[%d] = %d\n",aPair.first,pContainer1->size());

            CDataAnalyse::MapOfInt_3Dimension::iterator iterator1;
            for(iterator1 = pContainer1->begin();
            iterator1 != pContainer1->end(); iterator1++)
            {
                pair <int,CDataAnalyse::MapOfInt_2Dimension*> aPair = *iterator1;             
                CDataAnalyse::MapOfInt_2Dimension* pContainer2 = (CDataAnalyse::MapOfInt_2Dimension*)aPair.second;
                TRACE2("\t[%d] = %d\n",aPair.first,pContainer2->size());
                
                CDataAnalyse::MapOfInt_2Dimension::iterator iterator2;
                for(iterator2 = pContainer2->begin();
                iterator2 != pContainer2->end(); iterator2++)
                {
                    pair <int,int> aPair = *iterator2;
                    int nCount = aPair.second;
                    
                    TRACE2("\t\t[%d] = %d\n",aPair.first,nCount);
                }
            }
        }
        
    }
    //////////////////////////////////////////////////////////////////////////
    {

        TRACE0("====Layer Count====\n");
        CDataAnalyse::MapOfInt_4Dimension* pContainer0 = &m_DataAnalyse.m_pRoadSectionData->LayerCount;
        CDataAnalyse::MapOfInt_4Dimension::iterator iterator0;
        for(iterator0 = pContainer0->begin();
        iterator0 != pContainer0->end(); iterator0++)
        {
            pair <int,CDataAnalyse::MapOfInt_3Dimension*> aPair = *iterator0;
            CDataAnalyse::MapOfInt_3Dimension* pContainer1 = (CDataAnalyse::MapOfInt_3Dimension*)aPair.second;
            TRACE2("[%d] = %d\n",aPair.first,pContainer1->size());

            CDataAnalyse::MapOfInt_3Dimension::iterator iterator1;
            for(iterator1 = pContainer1->begin();
            iterator1 != pContainer1->end(); iterator1++)
            {
                pair <int,CDataAnalyse::MapOfInt_2Dimension*> aPair = *iterator1;             
                CDataAnalyse::MapOfInt_2Dimension* pContainer2 = (CDataAnalyse::MapOfInt_2Dimension*)aPair.second;
                TRACE2("\t[%d] = %d\n",aPair.first,pContainer2->size());
                
                CDataAnalyse::MapOfInt_2Dimension::iterator iterator2;
                for(iterator2 = pContainer2->begin();
                iterator2 != pContainer2->end(); iterator2++)
                {
                    pair <int,int> aPair = *iterator2;
                    int nCount = aPair.second;
                    
                    TRACE2("\t\t[%d] = %d\n",aPair.first,nCount);
                }
            }
        }
        
    }

    //////////////////////////////////////////////////////////////////////////
    //str Pattern
    {
        CDataAnalyse::MapOfString_3Dimension* pContainer1 = &m_DataAnalyse.m_pVertexData->m_PatternContainer1;
        unsigned long nTotalCount = 0;
        TRACE1("strPattern1Count = %d\n",pContainer1->size());
        Sleep(168);
        CDataAnalyse::MapOfString_3Dimension::iterator iterator1;
        for(iterator1 = pContainer1->begin();
        iterator1 != pContainer1->end(); iterator1++)
        {
            pair <CString,CDataAnalyse::MapOfString_2Dimension*> aPair = *iterator1;             
            CDataAnalyse::MapOfString_2Dimension* pContainer2 = (CDataAnalyse::MapOfString_2Dimension*)aPair.second;
            TRACE2("\t[%s] = %d\n",aPair.first,pContainer2->size());
            
            CDataAnalyse::MapOfString_2Dimension::iterator iterator2;
            for(iterator2 = pContainer2->begin();
            iterator2 != pContainer2->end(); iterator2++)
            {
                pair <CString,int> aPair = *iterator2;
                int nCount = aPair.second;
                nTotalCount += nCount;
                TRACE2("\t\t[%s] = %d\n",aPair.first,nCount);
            }
        }
        TRACE1("TotalDetailPatternCount = %d\n",nTotalCount);
    }  
    
    {
        CDataAnalyse::MapOfString_3Dimension* pContainer1 = &m_DataAnalyse.m_pVertexData->m_PatternContainer2;
        unsigned long nTotalCount = 0;
        TRACE1("strPattern2Count = %d\n",pContainer1->size());
        Sleep(168);
        CDataAnalyse::MapOfString_3Dimension::iterator iterator1;
        for(iterator1 = pContainer1->begin();
        iterator1 != pContainer1->end(); iterator1++)
        {
            pair <CString,CDataAnalyse::MapOfString_2Dimension*> aPair = *iterator1;             
            CDataAnalyse::MapOfString_2Dimension* pContainer2 = (CDataAnalyse::MapOfString_2Dimension*)aPair.second;
            TRACE2("\t[%s] = %d\n",aPair.first,pContainer2->size());
            
            CDataAnalyse::MapOfString_2Dimension::iterator iterator2;
            for(iterator2 = pContainer2->begin();
            iterator2 != pContainer2->end(); iterator2++)
            {
                pair <CString,int> aPair = *iterator2;
                int nCount = aPair.second;
                nTotalCount += nCount;
                TRACE2("\t\t[%s] = %d\n",aPair.first,nCount);

                TRACE_SLEEP_BY_TIMES(20);
            }
        }
        TRACE1("TotalDetailPatternCount = %d\n",nTotalCount);
    }
    
    {
        CDataAnalyse::MapOfString_3Dimension* pContainer1 = &m_DataAnalyse.m_pVertexData->m_PatternContainer3;
        unsigned long nTotalCount = 0;
        TRACE1("strPattern3Count = %d\n",pContainer1->size());
        Sleep(168);
        CDataAnalyse::MapOfString_3Dimension::iterator iterator1;
        for(iterator1 = pContainer1->begin();
        iterator1 != pContainer1->end(); iterator1++)
        {
            pair <CString,CDataAnalyse::MapOfString_2Dimension*> aPair = *iterator1;             
            CDataAnalyse::MapOfString_2Dimension* pContainer2 = (CDataAnalyse::MapOfString_2Dimension*)aPair.second;
            TRACE2("\t[%s] = %d\n",aPair.first,pContainer2->size());
            Sleep(50);
            
            CDataAnalyse::MapOfString_2Dimension::iterator iterator2;
            for(iterator2 = pContainer2->begin();
            iterator2 != pContainer2->end(); iterator2++)
            {
                pair <CString,int> aPair = *iterator2;
                int nCount = aPair.second;
                nTotalCount += nCount;
                TRACE2("\t\t[%s] = %d\n",aPair.first,nCount);


                TRACE_SLEEP_BY_TIMES(10);
            }
        }
        TRACE1("TotalDetailPatternCount = %d\n",nTotalCount);
    }
    
    {
        CDataAnalyse::MapOfString_3Dimension* pContainer1 = &m_DataAnalyse.m_pVertexData->m_PatternContainer3Match;
        unsigned long nTotalCount = 0;
        TRACE1("strPattern3MatchCount = %d\n",pContainer1->size());
        Sleep(168);
        CDataAnalyse::MapOfString_3Dimension::iterator iterator1;
        for(iterator1 = pContainer1->begin();
        iterator1 != pContainer1->end(); iterator1++)
        {
            pair <CString,CDataAnalyse::MapOfString_2Dimension*> aPair = *iterator1;             
            CDataAnalyse::MapOfString_2Dimension* pContainer2 = (CDataAnalyse::MapOfString_2Dimension*)aPair.second;
            TRACE2("\t[%s] = %d\n",aPair.first,pContainer2->size());
            
            CDataAnalyse::MapOfString_2Dimension::iterator iterator2;
            for(iterator2 = pContainer2->begin();
            iterator2 != pContainer2->end(); iterator2++)
            {
                pair <CString,int> aPair = *iterator2;
                int nCount = aPair.second;
                nTotalCount += nCount;
                TRACE2("\t\t[%s] = %d\n",aPair.first,nCount);
 
                TRACE_SLEEP_BY_TIMES(10);
           }
        }
        TRACE1("TotalDetailPatternCount = %d\n",nTotalCount);
    }


    return FALSE;
}
/**
 * @brief 
 *
 * @param 
 * @return
*/

BOOL CNetworkFileDataV2::AnalyseVertex(stuBreadthItem* pItem,CGIS_NetworkVertexV2* pFtr)
{
    MapOfRoadSection* pRoadSectionContainer = pItem->pRoadSectionContainer;
    StuNetworkFileStructV2C::StuBreadthHeaderInfo*     pBreadthHeader = pItem->pHeader;
    //////////////////////////////////////////////////////////////////////////
        
    vector <CGIS_NetworkRoadSectionV2*> arSpecialLayerRoadSection[3];
    int nRoadSectionNum = pFtr->GetRoadSectionNum();  
    for(int i = 0; i < nRoadSectionNum; i++)
    {
        int nRoadSectionID = pFtr->GetRoadSectionList()[i];
        CGIS_NetworkRoadSectionV2* pRoadSection =(*pRoadSectionContainer)[nRoadSectionID];
        ASSERT(pRoadSection);
        int nLayer = 0;
        if(nRoadSectionID < pBreadthHeader->nCountOfLevel1RoadSection)
        {
            nLayer = 0;
        }
        else if(nRoadSectionID < (pBreadthHeader->nCountOfLevel2RoadSection+pBreadthHeader->nCountOfLevel1RoadSection))
        {
            nLayer = 1;
        }
        else
        {
            nLayer = 2;
        }
        
        arSpecialLayerRoadSection[nLayer].push_back(pRoadSection);
    }
  
    //////////////////////////////////////////////////////////////////////////
    //写模式1(对层的统计)
    CString strRoadSectionNum;
    strRoadSectionNum.Format(_T("%d"),nRoadSectionNum);
    {
        CDataAnalyse::MapOfString_3Dimension* pContainer = &m_DataAnalyse.m_pVertexData->m_PatternContainer1;
        if(pContainer->find(strRoadSectionNum) == pContainer->end())
        {
            (*pContainer)[strRoadSectionNum] = new CDataAnalyse::MapOfString_2Dimension;
        }
    }
    CString strPattern1;
    strPattern1.Format(_T("%dLayer-[L0:%d L1:%d L2:%d]"),
        (arSpecialLayerRoadSection[0].size()?1:0)+(arSpecialLayerRoadSection[1].size()?1:0)+(arSpecialLayerRoadSection[2].size()?1:0),
        arSpecialLayerRoadSection[0].size(),arSpecialLayerRoadSection[1].size(),arSpecialLayerRoadSection[2].size());
    {
        CDataAnalyse::MapOfString_2Dimension* pContainer = m_DataAnalyse.m_pVertexData->m_PatternContainer1[strRoadSectionNum];
        if(pContainer->find(strPattern1) == pContainer->end())
        {
            (*pContainer)[strPattern1] = 0;
        }
    }
    
    (*m_DataAnalyse.m_pVertexData->m_PatternContainer1[strRoadSectionNum])[strPattern1]++;
    
    //////////////////////////////////////////////////////////////////////////
    //模式2统计
    CString strPattern2;
    if(arSpecialLayerRoadSection[0].size() == 2  && (nRoadSectionNum == 3 || nRoadSectionNum == 4) )
    {

        vector <BYTE> aLayerVector[3];
		int nLayer;
        for(nLayer = 0; nLayer < 3; nLayer++)
        {
            for(int i = 0; i < arSpecialLayerRoadSection[nLayer].size(); i++)
            {
                aLayerVector[nLayer].push_back(arSpecialLayerRoadSection[nLayer][i]->GetType());
            }
            sort(aLayerVector[nLayer].begin(),aLayerVector[nLayer].end());
        }
        
        CString strLayerType[3]; 
        CString strTemp;                
        for( nLayer = 0; nLayer < 3; nLayer++)
        {
            
            for(int i = 0; i < aLayerVector[nLayer].size(); i++)
            {
                strTemp.Format(_T("%d"),aLayerVector[nLayer][i]);
                strLayerType[nLayer] +=(strTemp+_T(","));
            }   
        }
        //////////////////////////////////////////////////////////////////////////
        //模式2开始
        CString strRoadSectionNum;
        strRoadSectionNum.Format(_T("%d->%d"),nRoadSectionNum,arSpecialLayerRoadSection[0].size());
        {
            CDataAnalyse::MapOfString_3Dimension* pContainer = &m_DataAnalyse.m_pVertexData->m_PatternContainer2;
            if(pContainer->find(strRoadSectionNum) == pContainer->end())
            {
                (*pContainer)[strRoadSectionNum] = new CDataAnalyse::MapOfString_2Dimension;
            }
        }
        
        strPattern2.Format(_T("%s->[%s\t]+[%s\t]"),strLayerType[0],strLayerType[1],strLayerType[2]);
        {
            CDataAnalyse::MapOfString_2Dimension* pContainer = m_DataAnalyse.m_pVertexData->m_PatternContainer2[strRoadSectionNum];
            if(pContainer->find(strPattern2) == pContainer->end())
            {
                (*pContainer)[strPattern2] = 0;
            }
        }
        
        (*m_DataAnalyse.m_pVertexData->m_PatternContainer2[strRoadSectionNum])[strPattern2]++;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //模式3统计
    if(arSpecialLayerRoadSection[0].size() == 2 && (nRoadSectionNum == 3 || nRoadSectionNum == 4) )
    {
        CGIS_NetworkRoadSectionV2* pRoadSectionHign0 = NULL;
        CGIS_NetworkRoadSectionV2* pRoadSectionHign1 = NULL;
        BOOL bMatch = TRUE;
        
        //if(pRoadSectionDel->GetRutDirection() == 1 || pRoadSectionDel->GetRutDirection() == 2)
        {
            CString strLayerType[3]; 
            CString strLayerDirection[3]; 
            CString strLayerCharge[3]; 
            CString strLayerRutType[3]; 
            CString strTemp;
            for(int nLayer = 0; nLayer < 3; nLayer++)
            {
                strLayerType[nLayer].Format(_T("L%d:"),nLayer);
                strLayerDirection[nLayer].Format(_T("[L%d:"),nLayer);
                strLayerCharge[nLayer].Format(_T("[L%d:"),nLayer);
                strLayerRutType[nLayer].Format(_T("[L%d:"),nLayer);
                for(int i = 0; i < arSpecialLayerRoadSection[nLayer].size(); i++)
                {
                    CGIS_NetworkRoadSectionV2* pRoadSection = arSpecialLayerRoadSection[nLayer][i];
                    ASSERT(pRoadSection);
                    pRoadSection->GetType();
                    
                    strTemp.Format(_T("%d"),pRoadSection->GetType());
                    strLayerType[nLayer] +=(strTemp+_T(","));
                    strTemp.Format(_T("%d"),pRoadSection->GetRutDirection());
                    strLayerDirection[nLayer] +=(strTemp+_T(","));
                    strTemp.Format(_T("%d"),pRoadSection->GetRutCharge());
                    strLayerCharge[nLayer] +=(strTemp+_T(","));
                    strTemp.Format(_T("%d"),pRoadSection->GetRutType());
                    strLayerRutType[nLayer] +=(strTemp+_T(","));
                }                
            }
 
            
            CString strPattern = 
                //_T("[Lel-")+strLayerType[0]+_T("|")+strLayerType[1]+_T("|")+strLayerType[2]+_T("] ")+
                //_T("[Dir-")+strLayerDirection[0]+_T("|")+strLayerDirection[1]+_T("|")+strLayerDirection[2]+_T("] ")+
                //_T("[Chg-")+strLayerCharge[0]+_T("|")+strLayerCharge[1]+_T("|")+strLayerCharge[2]+_T("]")+
                _T("[Typ-")+strLayerRutType[0]+_T("|")+strLayerRutType[1]+_T("|")+strLayerRutType[2]+_T("]");

            //////////////////////////////////////////////////////////////////////////
            {
                CDataAnalyse::MapOfString_3Dimension* pContainer = &m_DataAnalyse.m_pVertexData->m_PatternContainer3;
                if(pContainer->find(strRoadSectionNum) == pContainer->end())
                {
                    (*pContainer)[strRoadSectionNum] = new CDataAnalyse::MapOfString_2Dimension;
                }
            }
            CString strPattern3 = strPattern;
            {
                CDataAnalyse::MapOfString_2Dimension* pContainer = m_DataAnalyse.m_pVertexData->m_PatternContainer3[strRoadSectionNum];
                if(pContainer->find(strPattern3) == pContainer->end())
                {
                    (*pContainer)[strPattern3] = 0;
                }
            }
            (*m_DataAnalyse.m_pVertexData->m_PatternContainer3[strRoadSectionNum])[strPattern3]++;      
            

            //////////////////////////////////////////////////////////////////////////
            ASSERT(arSpecialLayerRoadSection[0].size() == 2);
            pRoadSectionHign0 = arSpecialLayerRoadSection[0][0];
            pRoadSectionHign1 = arSpecialLayerRoadSection[0][1];
            {
                CGIS_FeatureRoadSectionV2* pFtrRoadSection0 = GetFtrRoadSection(pRoadSectionHign0->GetRBreadthID(),pRoadSectionHign0->GetRutID());
                CGIS_FeatureRoadSectionV2* pFtrRoadSection1 = GetFtrRoadSection(pRoadSectionHign1->GetRBreadthID(),pRoadSectionHign1->GetRutID());
                ASSERT(pFtrRoadSection0);
                ASSERT(pFtrRoadSection1);

                if(pFtrRoadSection0->GetAnnotation() && pFtrRoadSection1->GetAnnotation())
                {
                    if(strcmp(pFtrRoadSection0->GetAnnotation(),pFtrRoadSection1->GetAnnotation()))
                    {
                        bMatch = FALSE;
                    }
                }

            }
            unsigned char chVertexSign = GetVertexSignOfTwoRoadSection(pFtr,pRoadSectionHign0,pRoadSectionHign1);
            unsigned char chDirectionSign = GetDirectionSignOfTwoRoadSection(pFtr,pRoadSectionHign0,pRoadSectionHign1);
            switch(chVertexSign)
            {
            case 0x00:
            case 0x11:
                if(chDirectionSign == 0x12 ||chDirectionSign == 0x21 || chDirectionSign == 0x00)
                {
                    ;
                }
                else
                {
                    bMatch = FALSE;
                }
                break;
            case 0x01:
            case 0x10:                       
                if(chDirectionSign == 0x22 ||chDirectionSign == 0x11 || chDirectionSign == 0x00)
                {
                    ;
                }
                else
                {
                    bMatch = FALSE;
                }
                break;
            default:
                ASSERT(FALSE);
                break;
            }
            

            if(bMatch)
            {
                //////////////////////////////////////////////////////////////////////////
                //写3match
                {
                    CDataAnalyse::MapOfString_3Dimension* pContainer = &m_DataAnalyse.m_pVertexData->m_PatternContainer3Match;
                    if(pContainer->find(strRoadSectionNum) == pContainer->end())
                    {
                        (*pContainer)[strRoadSectionNum] = new CDataAnalyse::MapOfString_2Dimension;
                    }
                }
                
                CString strLimitInfo;
                strLimitInfo.Format(_T("(%d,%d,%d,%d)"),
                    pRoadSectionHign0->GetLimitNormalNum(),pRoadSectionHign0->GetLimitReverseNum(),
                    pRoadSectionHign1->GetLimitNormalNum(),pRoadSectionHign1->GetLimitReverseNum());
                CString strPattern4 = strPattern + strLimitInfo;
                
                {
                    CDataAnalyse::MapOfString_2Dimension* pContainer = m_DataAnalyse.m_pVertexData->m_PatternContainer3Match[strRoadSectionNum];
                    if(pContainer->find(strPattern4) == pContainer->end())
                    {
                        (*pContainer)[strPattern4] = 0;
                    }
                }

                (*m_DataAnalyse.m_pVertexData->m_PatternContainer3Match[strRoadSectionNum])[strPattern4]++;
            }

            
        }
    }
    //////////////////////////////////////////////////////////////////////////

    return FALSE;
}

BOOL CNetworkFileDataV2::Check()
{
    if(!m_pMapContainer)
        return FALSE;

   MapOfGeoMap* pMapContainer = m_pMapContainer;
	MapOfGeoMap::iterator iteratorOfMap;
	for(iteratorOfMap = pMapContainer->begin();
	iteratorOfMap != pMapContainer->end(); iteratorOfMap++)
	{
		pair <unsigned long,stuMapItem* > aPair = *iteratorOfMap;
		stuMapItem* pItem = aPair.second;
		
		MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;
        MapOfBreadth::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = pBreadthContainer->begin();
        iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
			stuBreadthItem* pItem = aPair.second;

			StuNetworkFileStruct::StuBreadthHeaderInfo* pBreadthHeader = pItem->pHeader;

            {
				MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;		
                
            }
            {
                MapOfVertex* pFtrContainer = pItem->pVertexContainer;
                MapOfVertex::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair <unsigned long ,CGIS_NetworkVertexV2* > aPair = *iteratorOfFtr;
                    CGIS_NetworkVertexV2* pFtr = (CGIS_NetworkVertexV2*)aPair.second;
                    if(pFtr->GetEdge())
                    {
                        CheckEdgeVertex(pFtr);
                    }
                }
                
			}

        }

    }
    
    return TRUE;
}
BOOL CNetworkFileDataV2::CheckNormalVertex(CGIS_NetworkVertexV2* pVertex)
{
	ASSERT(pVertex);
	ASSERT(!pVertex->GetEdge());
    ASSERT(pVertex->GetRoadSectionNum() != 1);
		
	//取到相邻路段
	int nBreadthID = pVertex->GetEdgeBreadthID();
	int nRutID = pVertex->GetEdgeRoadSectionID();

    stuMapItem* pMapItem = (*m_pMapContainer)[1];
    MapOfBreadth* pBreadthContainer = pMapItem->pBreadthContainer;
    stuBreadthItem* pBreadthItem = (*pBreadthContainer)[nBreadthID];
    MapOfRoadSection*   pRoadSectionContainer   = pBreadthItem->pRoadSectionContainer;
    MapOfVertex*        pVertexContainer        = pBreadthItem->pVertexContainer;
    //////////////////////////////////////////////////////////////////////////
    
	CGIS_NetworkRoadSectionV2* pRoadSection = (*pRoadSectionContainer)[nRutID];
	
	ASSERT(pRoadSection);
	ASSERT(pRoadSection->GetRutID() == nRutID );
	ASSERT(pRoadSection->GetRBreadthID() == nBreadthID);
	

	//////////////////////////////////////////////////////////////////////////	
	CGIS_NetworkVertexV2*   pSVtx = (*pVertexContainer)[pRoadSection->GetRutStartVtxID()];
	CGIS_NetworkVertexV2*   pEVtx = (*pVertexContainer)[pRoadSection->GetRutEndVtxID() ];
	ASSERT(pSVtx);
	ASSERT(pEVtx);

    int nRoadSectionNum = pVertex->GetRoadSectionNum();
    int nRoundaboutRoadSectionCount = 0;
    for(int i = 0; i < nRoadSectionNum; i++)
    {
        pRoadSection = (*pRoadSectionContainer)[nRutID];
        ASSERT(pRoadSection);
        if(pRoadSection->GetRutType() == 0)
            nRoundaboutRoadSectionCount++;
    }

    if(nRoundaboutRoadSectionCount != 0)
		
	//通过距离判定确实是原始节点的影子节点
	if( pSVtx->GetEdge() == 1 || pEVtx->GetEdge() == 1 )
	{
		
		ASSERT( 
			(
			fabs(pSVtx->GetVtxPosition().dx-pVertex->GetVtxPosition().dx) < FLOAT_ZERO &&
			fabs(pSVtx->GetVtxPosition().dy-pVertex->GetVtxPosition().dy) < FLOAT_ZERO 
			) 
			||
			( 
			fabs(pEVtx->GetVtxPosition().dx-pVertex->GetVtxPosition().dx) < FLOAT_ZERO &&
			fabs(pEVtx->GetVtxPosition().dy-pVertex->GetVtxPosition().dy) < FLOAT_ZERO 
			)
			);
	}
	else
		ASSERT( FALSE );
		
	return TRUE;
}

//检查边界节点
BOOL CNetworkFileDataV2::CheckEdgeVertex(CGIS_NetworkVertexV2* pVertex)
{
	ASSERT(pVertex);
	ASSERT(pVertex->GetEdge());
    ASSERT(pVertex->GetRoadSectionNum() == 1);
		
	//取到相邻路段
	int nBreadthID = pVertex->GetEdgeBreadthID();
	int nRutID = pVertex->GetEdgeRoadSectionID();

    stuMapItem* pMapItem = (*m_pMapContainer)[1];
    MapOfBreadth* pBreadthContainer = pMapItem->pBreadthContainer;
    stuBreadthItem* pBreadthItem = (*pBreadthContainer)[nBreadthID];
    MapOfRoadSection*   pRoadSectionContainer   = pBreadthItem->pRoadSectionContainer;
    MapOfVertex*        pVertexContainer        = pBreadthItem->pVertexContainer;
    //////////////////////////////////////////////////////////////////////////
    
	CGIS_NetworkRoadSectionV2* pRoadSection = (*pRoadSectionContainer)[nRutID];
	
	ASSERT(pRoadSection);
	ASSERT(pRoadSection->GetRutID() == nRutID );
	ASSERT(pRoadSection->GetRBreadthID() == nBreadthID);
	

	//////////////////////////////////////////////////////////////////////////	
	CGIS_NetworkVertexV2*   pSVtx = (*pVertexContainer)[pRoadSection->GetRutStartVtxID()];
	CGIS_NetworkVertexV2*   pEVtx = (*pVertexContainer)[pRoadSection->GetRutEndVtxID() ];
	ASSERT(pSVtx);
	ASSERT(pEVtx);
		
	//通过距离判定确实是原始节点的影子节点
	if( pSVtx->GetEdge() == 1 || pEVtx->GetEdge() == 1 )
	{
		
		ASSERT( 
			(
			fabs(pSVtx->GetVtxPosition().dx-pVertex->GetVtxPosition().dx) < FLOAT_ZERO &&
			fabs(pSVtx->GetVtxPosition().dy-pVertex->GetVtxPosition().dy) < FLOAT_ZERO 
			) 
			||
			( 
			fabs(pEVtx->GetVtxPosition().dx-pVertex->GetVtxPosition().dx) < FLOAT_ZERO &&
			fabs(pEVtx->GetVtxPosition().dy-pVertex->GetVtxPosition().dy) < FLOAT_ZERO 
			)
			);
	}
	else
		ASSERT( FALSE );
		
	return TRUE;
}
BOOL CNetworkFileDataV2::WriteBreadthFtrCount()
{
    CMemFile aMemFile;
    CFile* pFile = &aMemFile;
    int nIndex = 0;
    {
        stuMapItem* pMapItem = (*m_pMapContainer)[1];
        MapOfBreadth* pBreadthContainer = pMapItem->pBreadthContainer;
        MapOfBreadth::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = pBreadthContainer->begin();
        iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
            stuBreadthItem* pItem = aPair.second;

            //ASSERT(pItem->pIndex->nBreadthMapID == nIndex);
            DWORD dwFtrCount = pItem->pRoadSectionContainer->size()+ 
                pItem->pVertexContainer->size();
            pFile->Write(&dwFtrCount,sizeof(DWORD));
            DBG2LOG2("Breadth %d Ftr:%d",nIndex,dwFtrCount);
            nIndex++;
        }
    }

    CFile aFile(_T("E:\\BreadthFtrCount.dat"),CFile::modeCreate|CFile::modeReadWrite);
    unsigned long nSrcSize = aMemFile.GetLength();
	unsigned char* pSrcData = aMemFile.Detach();
	aFile.Write(pSrcData,nSrcSize);
	delete []pSrcData;
	pSrcData = NULL;

    return FALSE;

}

BOOL CNetworkFileDataV2::ExtractNetwork()
{
    {
        stuMapItem* pMapItem = (*m_pMapContainer)[1];
        MapOfBreadth* pBreadthContainer = pMapItem->pBreadthContainer;
        MapOfBreadth::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = pBreadthContainer->begin();
        iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
            stuBreadthItem* pItem = aPair.second;

            MapOfVertex* pFtrContainer = pItem->pVertexContainer;
            MapOfVertex::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair <unsigned long ,CGIS_NetworkVertexV2* > aPair = *iteratorOfFtr;
                CGIS_NetworkVertexV2* pFtr = aPair.second;
                ASSERT(pFtr);
                ExtractNetwork_Vertex(pFtr);
                ASSERTVALID(pFtr);
            }
        }
    }
   
   return FALSE;
}
BOOL CNetworkFileDataV2::ExtractNetwork_Vertex(CGIS_NetworkVertexV2* pVertex)
{
	ASSERT(pVertex);
    if(pVertex->GetEdge())
    {
        ASSERT(pVertex->GetRoadSectionNum() == 1);
        return TRUE;
    }

    int nBreadthID = pVertex->GetVBreadthID();
    stuMapItem* pMapItem = (*m_pMapContainer)[1];
    MapOfBreadth* pBreadthContainer = pMapItem->pBreadthContainer;
    stuBreadthItem* pBreadthItem = (*pBreadthContainer)[nBreadthID];
    MapOfRoadSection*   pRoadSectionContainer   = pBreadthItem->pRoadSectionContainer;
    MapOfVertex*        pVertexContainer        = pBreadthItem->pVertexContainer;
    //////////////////////////////////////////////////////////////////////////

    ASSERT(pVertex->GetRoadSectionNum() > 0 );
    ASSERT(pVertex->GetRoadSectionNum() < 100 );
    unsigned short	pExtractRoadSectionList[100];				//相连接的路段ID
	unsigned short	pExtractRoadSectionTurnList[100];			//与水平方向的夹角数组
	unsigned short	pExtractRoadSectionLayerIDList[100];		//路段所在图层ID

    int nIndex = 0;
    int nRoadSectionNum = pVertex->GetRoadSectionNum();
    unsigned short* pRoadSectionList        = pVertex->GetRoadSectionList();
    unsigned short* pRoadSectionTurnList    = pVertex->GetRoadSectionTurnList();
    unsigned short* pRoadSectionLayerIDList = pVertex->GetRoadSectionLayerIDList();

	for(int i = 0; i < nRoadSectionNum; i++)
    {
        unsigned short nRoadSectionID = pRoadSectionList[i];
        MapOfRoadSection::iterator iteratorOfFind =  pRoadSectionContainer->find(nRoadSectionID);

        if(iteratorOfFind == pRoadSectionContainer->end())
        {
            //此路段不在第一等级中,且此路段从本节点相边的本等级的任何一条路都不可到达则可剔除
            int n = 1;
            //m_RunTimeAnalyse.m_nEdgeExtractCount++;
            //
        }
        if(iteratorOfFind != pRoadSectionContainer->end())
        {
            pExtractRoadSectionList         [nIndex] = pRoadSectionList[i];
            pExtractRoadSectionTurnList     [nIndex] = pRoadSectionTurnList[i];
            pExtractRoadSectionLayerIDList  [nIndex] = pRoadSectionLayerIDList[i];
            nIndex++;
        }

    }
    int nExtractRoadSectionCount = nIndex;
#ifdef ANALYSE_RUNTIME
    if(nRoadSectionNum != nExtractRoadSectionCount)
    {
        m_RunTimeAnalyse.m_nEdgeExtractCount++;
    }

    if(nRoadSectionNum == 3 && nRoadSectionNum != nExtractRoadSectionCount)
    {
        m_RunTimeAnalyse.m_nCountOfExtractVertexWhichHave3Edge++;
    }
#endif

    memcpy(pRoadSectionList,        pExtractRoadSectionList,        sizeof(unsigned short)*nExtractRoadSectionCount);
    memcpy(pRoadSectionTurnList,    pExtractRoadSectionTurnList,    sizeof(unsigned short)*nExtractRoadSectionCount);
    memcpy(pRoadSectionLayerIDList, pExtractRoadSectionLayerIDList, sizeof(unsigned short)*nExtractRoadSectionCount);
    pVertex->m_byRoadSectionNum = nExtractRoadSectionCount;

    return TRUE;
}

BOOL CNetworkFileDataV2::AnalyseWithSpaceData(CSpaceFileDataV2_RoadSection* pSpaceFile)
{
    m_pSpaceFile = pSpaceFile;
    AnalyseNetwork();
    DUMPSTATE(this);
    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//

unsigned char GetVertexSignOfTwoRoadSection(
    CGIS_NetworkVertexV2* pVertex,
    CGIS_NetworkRoadSectionV2* pRoadSection0,
    CGIS_NetworkRoadSectionV2* pRoadSection1
    )
{
  
    if(pVertex->GetVtxID() == pRoadSection0->GetRutStartVtxID() && pVertex->GetVtxID() == pRoadSection1->GetRutStartVtxID())
        return 0x00;
    if(pVertex->GetVtxID() == pRoadSection0->GetRutStartVtxID() && pVertex->GetVtxID() == pRoadSection1->GetRutEndVtxID())
        return 0x01;
    if(pVertex->GetVtxID() == pRoadSection0->GetRutEndVtxID() && pVertex->GetVtxID() == pRoadSection1->GetRutStartVtxID())
        return 0x10;
    if(pVertex->GetVtxID() == pRoadSection0->GetRutEndVtxID() && pVertex->GetVtxID() == pRoadSection1->GetRutEndVtxID())
        return 0x11;

    ASSERT(FALSE);
    return 0xff;
}
unsigned char GetDirectionSignOfTwoRoadSection(
    CGIS_NetworkVertexV2* pVertex,
    CGIS_NetworkRoadSectionV2* pRoadSection0,
    CGIS_NetworkRoadSectionV2* pRoadSection1
    )
{  
    return (pRoadSection0->GetRutDirection()<<4)+pRoadSection1->GetRutDirection();
}

CGIS_FeatureRoadSectionV2* CNetworkFileDataV2::GetFtrRoadSection(int nBreadthID,int nRoadSectionID)
{
    
    CGIS_FeatureRoadSectionV2* pRtnRoadSection = NULL;
    
    
    CSpaceFileDataV2_RoadSection::MapOfGeoMap* pMapContainer = m_pSpaceFile->GetMapContainer();
    CSpaceFileDataV2_RoadSection::MapOfGeoMap::iterator iteratorOfMap;
    for(iteratorOfMap = pMapContainer->begin();
    iteratorOfMap != pMapContainer->end(); iteratorOfMap++)
    {
        pair <unsigned long ,CSpaceFileDataV2_RoadSection::stuMapItem* > aPair = *iteratorOfMap;
        CSpaceFileDataV2_RoadSection::stuMapItem* pItem = aPair.second;
        if(pItem->pIndex->nID != 2)
            continue;
        
        CSpaceFileDataV2_RoadSection::MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;          
        CSpaceFileDataV2_RoadSection::MapOfBreadth::iterator iteratorOfBreadth = 
            pBreadthContainer->find(nBreadthID);
        ASSERT(iteratorOfBreadth != pBreadthContainer->end());
        {
            pair <unsigned long ,CSpaceFileDataV2_RoadSection::stuBreadthItem* > aPair = *iteratorOfBreadth;
            CSpaceFileDataV2_RoadSection::stuBreadthItem* pItem = aPair.second;
            
            unsigned long nBreadthID = pItem->pIndex->nID;
            MapOfRoadSection* pLocalRoadSectionContainer = new MapOfRoadSection;
            
            CSpaceFileDataV2_RoadSection::MapOfLayer* pLayerContainer = pItem->pLayerContainer;
            CSpaceFileDataV2_RoadSection::MapOfLayer::iterator iteratorOfLayer;
            for(iteratorOfLayer = pLayerContainer->begin();
            iteratorOfLayer != pLayerContainer->end();iteratorOfLayer++)
            {
                pair <unsigned long ,CSpaceFileDataV2_RoadSection::stuLayerItem* > aPair = *iteratorOfLayer;
                CSpaceFileDataV2_RoadSection::stuLayerItem* pLayerItem = aPair.second;
                
                CSpaceFileDataV2_RoadSection::MapOfFeature* pFtrContainer = pLayerItem->pFtrContainer;
                CSpaceFileDataV2_RoadSection::MapOfFeature::iterator iteratorOfFtr;               
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {                   
                    pair < unsigned long ,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRoadSectionV2* pFtr = (CGIS_FeatureRoadSectionV2*)aPair.second;
                    ASSERT(pFtr);
                    ASSERTVALID(pFtr);
                    if(pFtr->GetRutID() == nRoadSectionID)
                    {
                        pRtnRoadSection = pFtr;
                        break;
                    }
                }
                
            }
        }
    }
    ASSERT(pRtnRoadSection);
    return pRtnRoadSection;
}