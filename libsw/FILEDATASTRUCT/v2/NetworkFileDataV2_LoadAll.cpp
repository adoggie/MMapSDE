// NetworkFileDataV2.cpp: implementation of the CNetworkFileDataV2_LoadAll class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetworkFileDataV2_LoadAll.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "./Zlib/zlib.h"
#include "./FileDataStruct/V2/GIS_NetworkRoadSectionV2.h"
#include "./FileDataStruct/V2/GIS_NetworkVertexV2.h"
//////////////////////////////////////////////////////////////////////////
static char s_DestBuf2[1000000];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkFileDataV2_LoadAll::CNetworkFileDataV2_LoadAll()
{

}

CNetworkFileDataV2_LoadAll::~CNetworkFileDataV2_LoadAll()
{
    //ClearOperation();
}

//////////////////////////////////////////////////////////////////////////
BOOL CNetworkFileDataV2_LoadAll::Load(LPCTSTR strFileName)
{
    ClearOperation();
    CFile aFile(strFileName,CFile::modeRead);
	CFile* pFile = &aFile;
	ReadNetwork(pFile); 
    //ExtractNetwork();
    return FALSE;
}



#ifdef _DEBUG
void CNetworkFileDataV2_LoadAll::DumpState()
{
    TRACE0("********************************************************************\n");
    TRACE0("CNetworkFileDataV2_LoadAll Infomation----------------\n");
    TRACE1("RoadSectionLoadCount            : %d.\n",GetRoadSectionCount());
    TRACE1("VertexLoadCount                 : %d.\n",GetVertexCount());
    TRACE0("********************************************************************\n");
    
}
void CNetworkFileDataV2_LoadAll::AssertValid() const
{

}
#endif
  

BOOL CNetworkFileDataV2_LoadAll::InitialOperation()
{
    return FALSE;
}
BOOL CNetworkFileDataV2_LoadAll::ClearOperation()
{
    {
        MapOfBreadthOfRoadSection::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = m_BreadthContainerOfRoadSection.begin();
        iteratorOfBreadth != m_BreadthContainerOfRoadSection.end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,MapOfRoadSection* > aPair = *iteratorOfBreadth;
            MapOfRoadSection* pFtrContainer = aPair.second;
            
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
        m_BreadthContainerOfRoadSection.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    
    {
        MapOfBreadthOfVertex::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = m_BreadthContainerOfVertex.begin();
        iteratorOfBreadth != m_BreadthContainerOfVertex.end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,MapOfVertex* > aPair = *iteratorOfBreadth;
            MapOfVertex* pFtrContainer = aPair.second;
            
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
        m_BreadthContainerOfVertex.clear();
    }

    return FALSE;
}    

unsigned long CNetworkFileDataV2_LoadAll::GetRoadSectionCount()
{
    unsigned long nCount = 0;

    {
        MapOfGeoMap* pMapContainer = m_pMapContainer;
        MapOfGeoMap::iterator iteratorOfMap;
        for(iteratorOfMap = pMapContainer->begin();
        iteratorOfMap != pMapContainer->end(); iteratorOfMap++)
        {
            pair <unsigned long ,stuMapItem* > aPair = *iteratorOfMap;
            stuMapItem* pItem = aPair.second;

            MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;          
            MapOfBreadth::iterator iteratorOfBreadth;
            for(iteratorOfBreadth = pBreadthContainer->begin();
            iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
            {
                pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
                stuBreadthItem* pItem = aPair.second;

                MapOfRoadSection*	pRoadSectionContainer   =   pItem->pRoadSectionContainer;
                MapOfVertex*		pVertexContainer        =   pItem->pVertexContainer;
                char*               pRoadSectionBuddy       =   pItem->pRoadSectionBuddy;
                char*               pVertexBuddy            =   pItem->pVertexBuddy;   
                StuNetworkFileStructV2C::StuBreadthHeaderInfo*      pHeader  =   pItem->pHeader;
                StuNetworkFileStructV2C::StuIndexInfoOfBreadth*     pIndex   =   pItem->pIndex;
                unsigned long       nID                  =       pItem->nID;


                nCount += pRoadSectionContainer->size();
            }
        }
    }

    return nCount;
}
unsigned long CNetworkFileDataV2_LoadAll::GetVertexCount()
{
    unsigned long nCount = 0;

    {
        MapOfGeoMap* pMapContainer = m_pMapContainer;
        MapOfGeoMap::iterator iteratorOfMap;
        for(iteratorOfMap = pMapContainer->begin();
        iteratorOfMap != pMapContainer->end(); iteratorOfMap++)
        {
            pair <unsigned long ,stuMapItem* > aPair = *iteratorOfMap;
            stuMapItem* pItem = aPair.second;

            MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;          
            MapOfBreadth::iterator iteratorOfBreadth;
            for(iteratorOfBreadth = pBreadthContainer->begin();
            iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
            {
                pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
                stuBreadthItem* pItem = aPair.second;

                MapOfRoadSection*	pRoadSectionContainer   =   pItem->pRoadSectionContainer;
                MapOfVertex*		pVertexContainer        =   pItem->pVertexContainer;
                char*               pRoadSectionBuddy       =   pItem->pRoadSectionBuddy;
                char*               pVertexBuddy            =   pItem->pVertexBuddy;   
                StuNetworkFileStructV2C::StuBreadthHeaderInfo*      pHeader  =   pItem->pHeader;
                StuNetworkFileStructV2C::StuIndexInfoOfBreadth*     pIndex   =   pItem->pIndex;
                unsigned long       nID                  =       pItem->nID;


                nCount += pVertexContainer->size();
            }
        }
    }
    
    return nCount;

}


BOOL CNetworkFileDataV2_LoadAll::ReadNetwork(CFile* pFile)
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

BOOL CNetworkFileDataV2_LoadAll::ReadMap(CFile* pFile,stuMapItem* pMapItem)
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
		if(pIndexInfoArray[i].nBeforeCompressSize == 0)
        {
            ASSERT(pIndexInfoArray[i].nAfterCompressSize == 0);
			continue;
        }			 
        stuBreadthItem* pItem = new stuBreadthItem;
        pItem->pIndex = new StuNetworkFileStructV2C::StuIndexInfoOfBreadth;
        pItem->pHeader = new StuNetworkFileStructV2C::StuBreadthHeaderInfo;
        pItem->pRoadSectionContainer = new MapOfRoadSection;
        pItem->pVertexContainer = new MapOfVertex;
        pItem->pRoadSectionBuddy = NULL;
        pItem->pVertexBuddy = NULL;
        pItem->nID = i;

		memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuNetworkFileStructV2C::StuIndexInfoOfBreadth));


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


BOOL CNetworkFileDataV2_LoadAll::ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem,int nLoadFlag)
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

BOOL CNetworkFileDataV2_LoadAll::ExtractNetwork()
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
BOOL CNetworkFileDataV2_LoadAll::ExtractNetwork_Vertex(CGIS_NetworkVertexV2* pVertex)
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
	unsigned short	pExtractRoadSectionTurnList[100];							//与水平方向的夹角数组
	unsigned short	pExtractRoadSectionLayerIDList[100];		//路段所在图层ID

    int nIndex = 0;
    int nRoadSectionNum = pVertex->GetRoadSectionNum();
    unsigned short* pRoadSectionList = pVertex->GetRoadSectionList();
    unsigned short* pRoadSectionTurnList = pVertex->GetRoadSectionTurnList();
    unsigned short* pRoadSectionLayerIDList = pVertex->GetRoadSectionLayerIDList();
	for(int i = 0; i < nRoadSectionNum; i++)
    {
        unsigned short nRoadSectionID = pRoadSectionList[i];
        MapOfRoadSection::iterator iteratorOfFind =  pRoadSectionContainer->find(nRoadSectionID);
        if(iteratorOfFind != pRoadSectionContainer->end())
        {
            pExtractRoadSectionList         [nIndex] = pRoadSectionList[i];
            pExtractRoadSectionTurnList     [nIndex] = pRoadSectionTurnList[i];
            pExtractRoadSectionLayerIDList  [nIndex] = pRoadSectionLayerIDList[i];
            nIndex++;
        }
    }
    int nExtractRoadSectionCount = nIndex;
    memcpy(pRoadSectionList,        pExtractRoadSectionList,        sizeof(unsigned short)*nExtractRoadSectionCount);
    memcpy(pRoadSectionTurnList,    pExtractRoadSectionTurnList,    sizeof(unsigned short)*nExtractRoadSectionCount);
    memcpy(pRoadSectionLayerIDList, pExtractRoadSectionLayerIDList, sizeof(unsigned short)*nExtractRoadSectionCount);
    pVertex->m_byRoadSectionNum = nExtractRoadSectionCount;

    return TRUE;
}
