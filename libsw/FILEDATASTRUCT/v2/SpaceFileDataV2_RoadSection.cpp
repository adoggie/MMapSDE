// SpaceFileDataV2.cpp: implementation of the CSpaceFileDataV2_RoadSection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpaceFileDataV2_RoadSection.h"
#include "./FileDataStruct/V2/GIS_FeatureRoadSectionV2.h"
#include "./FileDataStruct/V2/GIS_FeaturePointV2.h"
#include "./FileDataStruct/V2/GIS_FeatureLineV2.h"
#include "./FileDataStruct/V2/GIS_FeaturePolyV2.h"
#include "./Zlib/zlib.h"


static unsigned long nFtrTotalCount = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpaceFileDataV2_RoadSection::CSpaceFileDataV2_RoadSection()
{
    m_pMapContainer = NULL;
}

CSpaceFileDataV2_RoadSection::~CSpaceFileDataV2_RoadSection()
{
    ClearOperation();            
}

BOOL CSpaceFileDataV2_RoadSection::Load(LPCTSTR strFilePath)
{
    TRACE1("SpaceFile:%s\n",strFilePath);
	CFile aFile(strFilePath,CFile::modeRead);
	CFile* pFile = &aFile;
    ReadSpaceData(pFile);
	return TRUE;
}
BOOL CSpaceFileDataV2_RoadSection::ReadSpaceData(CFile* pFile)
{
    ASSERT(!m_pMapContainer);
    m_pMapContainer = new MapOfGeoMap;
	//读头
    StuSpaceFileStructV2 :: StuHeaderInfo* pFileHeadInfo = new StuSpaceFileStructV2 :: StuHeaderInfo;
	pFile->Read(pFileHeadInfo,sizeof(StuSpaceFileStructV2 :: StuHeaderInfo));

    

	int nCount = pFileHeadInfo->nMapCount;
	//---------------------------
	//读入地图索引
	StuSpaceFileStructV2::StuIndexInfoOfMap* pIndexInfoArray = NULL;		//地图索引信息
	pIndexInfoArray = new StuSpaceFileStructV2::StuIndexInfoOfMap[nCount];
	memset(pIndexInfoArray,0,sizeof(StuSpaceFileStructV2::StuIndexInfoOfMap)*nCount);
	pFile->Read(pIndexInfoArray,sizeof(StuSpaceFileStructV2::StuIndexInfoOfMap) * nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////
	
	//------------------------------------------------------------------------
	//读入地图
	for(int i = 0; i < nCount; i++)
	{
        stuMapItem* pItem = new stuMapItem;
        pItem->pBreadthContainer = new MapOfBreadth;
        pItem->pIndex = new StuSpaceFileStructV2::StuIndexInfoOfMap;
        memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuSpaceFileStructV2::StuIndexInfoOfMap));
        pFile->Seek(pItem->pIndex->nOffset + dwOffsetBase,CFile::begin);
		ReadMap(pFile,pItem);
        (*m_pMapContainer)[pItem->pIndex->nID] = pItem;
	}
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	//清理工作
	delete pIndexInfoArray;
	pIndexInfoArray = NULL;

	
	return TRUE;
}
BOOL CSpaceFileDataV2_RoadSection::ReadMap(CFile* pFile,stuMapItem* pMapItem)
{
    StuSpaceFileStructV2 :: StuIndexInfoOfMap* pIndexInfoOfMap = pMapItem->pIndex;
	ASSERT(pIndexInfoOfMap);

	//读入图幅索引
	StuSpaceFileStructV2::StuIndexInfoOfBreadth* pIndexInfoArray = NULL;	//图幅索引数组

	int nCount = pIndexInfoOfMap->nBreadthCount;
	{
		pIndexInfoArray = new StuSpaceFileStructV2::StuIndexInfoOfBreadth[nCount];
		memset(pIndexInfoArray,0,sizeof(StuSpaceFileStructV2::StuIndexInfoOfBreadth)*nCount);
		pFile->Read(pIndexInfoArray,sizeof(StuSpaceFileStructV2::StuIndexInfoOfBreadth) * nCount);
	}
    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////

	//读入图幅数据
	//------------------------------------------------------------------------
	for(int i = 0; i < nCount; i++)
	{
		if(!(i%10000))
		{
			static DWORD	 dwtickstart = 0;
			TRACE2("----ReadBreadthV2(%.5d):%d ms--------\n",i,GetTickCount() - dwtickstart);
			dwtickstart = GetTickCount();
		}

        stuBreadthItem* pItem = new stuBreadthItem;
        pItem->pLayerContainer = new MapOfLayer;
        pItem->pIndex = new StuSpaceFileStructV2::StuIndexInfoOfBreadth;
        memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuSpaceFileStructV2::StuIndexInfoOfBreadth));
        pItem->pIndex->nID = i;
        
        //读入图幅
        pFile->Seek(pItem->pIndex->nOffset+dwOffsetBase,CFile::begin);
        ReadBreadth(pFile,pItem);//写入图幅
        //if(pItem->pLayerContainer->size())
        (*(pMapItem->pBreadthContainer))[pItem->pIndex->nID] = pItem;
	}
    
	//清理
	delete pIndexInfoArray;
	pIndexInfoArray = NULL;

	return TRUE;
}

BOOL CSpaceFileDataV2_RoadSection::ReadBreadth(CFile* pFile,
        stuBreadthItem* pBreadthItem)
{
	ASSERT(pBreadthItem);
	ASSERT(pBreadthItem->pIndex);
	//////////////////////////////////////////////////////////////////////////
    StuSpaceFileStructV2 :: StuIndexInfoOfBreadth* pIndexInfoOfBreadth = pBreadthItem->pIndex;

	int nCount = pIndexInfoOfBreadth->nLayerCount;
	//////////////////////////////////////////////////////////////////////////
	StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData* pIndexInfoArray = NULL;//层索引区
	//读入图层索引
	{
		pIndexInfoArray = new StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData[nCount];
		memset(pIndexInfoArray,0,sizeof(StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData)*nCount);
		pFile->Read(pIndexInfoArray,sizeof(StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData) * nCount );
	}
    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//读入图层数据
	for(int i = 0 ; i < nCount; i++)
	{
        stuLayerItem* pItem = new stuLayerItem;
        pItem->pFtrContainer = new MapOfFeature;
        pItem->pIndex = new StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData;
        memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData));
        //if(pIndexOfLayer->nID == 10 || pIndexOfLayer->nID == 11)
        {
            pFile->Seek(pItem->pIndex->nOffset + dwOffsetBase,CFile::begin);
            ReadLayer( pFile,pItem );	
            (*(pBreadthItem->pLayerContainer))[pItem->pIndex->nID] = pItem;
        }
	}
    
	delete pIndexInfoArray;
	pIndexInfoArray = NULL;

	return FALSE;
}

BOOL CSpaceFileDataV2_RoadSection::ReadLayer(CFile* pFile,stuLayerItem* pItem)
{
    ASSERT(pFile);
    ASSERT(pItem);
    ASSERT(pItem->pIndex);

//////////////////////////////////////////////////////////////////////////
    StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData* pLayerIndex = pItem->pIndex;
	ASSERT(pLayerIndex);
    ASSERTVALID(pLayerIndex);
	int nBeforeCompressSize = pLayerIndex->nBeforeCompressSize;
	int nAfterCompressSize = pLayerIndex->nAfterCompressSize;
	//分配压缩前空间
	char *pBufOfBeforeCompress = new char[nBeforeCompressSize];
	ASSERT(pBufOfBeforeCompress);
	
	//-------------------------------------------------
	//读取并解压数据
	{
		char *pBufOfAfterCompress = new char[nAfterCompressSize];//分配压缩后
		ASSERT(pBufOfAfterCompress);
		
		pFile->Read( pBufOfAfterCompress, nAfterCompressSize );
		DWORD dwSize = nBeforeCompressSize;
		int nRet = uncompress( (unsigned char*)pBufOfBeforeCompress, &dwSize, (unsigned char*)pBufOfAfterCompress, nAfterCompressSize  );
		ASSERT(nRet == Z_OK);
		delete [] pBufOfAfterCompress;
		ASSERT(nBeforeCompressSize == dwSize);
	}
	//-----------------------------------------------------
	
	{	
		char* pCharAddr		= NULL;
		char* pShortAddr	= NULL;
		char* pDwordAddr	= NULL;
		
		StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData* pHeader = (StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData*)pBufOfBeforeCompress;
		DWORD dwBuddyDataSize = pHeader->nCharDataBuddyLength+pHeader->nShortDataBuddyLength+pHeader->nDwordDataBuddyLength;
        ASSERTVALID(pHeader);
        char* pBuddyData = new char[dwBuddyDataSize];
        pItem->pBuddy = pBuddyData;

        unsigned long nFtrSize = 0;
        switch(pLayerIndex->nType)
        {
		case CGIS_FeatureV2::EN_GOTYPE_POINT:
            nFtrSize = sizeof(StuSpaceFileStructV2::StuGisFeaturePoint);
			break;
		case CGIS_FeatureV2::EN_GOTYPE_ROADSECTION:
            nFtrSize = sizeof(StuSpaceFileStructV2::StuGisFeatureRoadSection);
			break;
		case CGIS_FeatureV2::EN_GOTYPE_LINE:
            nFtrSize = sizeof(StuSpaceFileStructV2::StuGisFeatureLine);
			break;
		case CGIS_FeatureV2::EN_GOTYPE_POLY:
            nFtrSize = sizeof(StuSpaceFileStructV2::StuGisFeaturePoly);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
        unsigned long nBuddyOffset = sizeof(StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData) + pHeader->nFeatureCount*nFtrSize;

        ASSERT(pItem->pBuddy);
		memcpy(pItem->pBuddy,pBufOfBeforeCompress+nBuddyOffset,dwBuddyDataSize);

        
		if(dwBuddyDataSize > 0)
		{
			pCharAddr	= pItem->pBuddy;
			pShortAddr	= pCharAddr + pHeader->nCharDataBuddyLength;
			pDwordAddr	= pShortAddr+ pHeader->nShortDataBuddyLength;
			if(	*( (DWORD*)pCharAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0)	)
			{
				pCharAddr	+= 4;	
			}
			//if( *( (DWORD*)pShortAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0))
			{
				//pShortAddr	+= 4;
			}
			if( *( (DWORD*)pDwordAddr ) == ('y'<<24|'l'<<16|'d'<<8|' '<<0))
			{
				pDwordAddr	+= 4;
			}
		}
		
		switch(pLayerIndex->nType)
		{
		case CGIS_FeatureV2::EN_GOTYPE_POINT:			
			{
				StuSpaceFileStructV2::StuGisFeaturePoint* pArrayOfStGisFeature = 
					(StuSpaceFileStructV2::StuGisFeaturePoint*)(pBufOfBeforeCompress+sizeof(StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData));
				
				for(int i = 0; i < pHeader->nFeatureCount; i++)
				{
					CGIS_FeaturePointV2* pFtr = new CGIS_FeaturePointV2;
					ASSERT(pFtr);
					memcpy(pFtr->GetDataAddr(),&pArrayOfStGisFeature[i],sizeof(StuSpaceFileStructV2::StuGisFeaturePoint));
					pFtr->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
                    pFtr->MakeBuddyIndependent();
                    ASSERTVALID(pFtr);

                    CGIS_FeatureV2* pFtrOld = (*(pItem->pFtrContainer))[i];
                    ASSERT(!pFtrOld);
                   (*(pItem->pFtrContainer))[i] = pFtr;
				}

			}
			break;

		case CGIS_FeatureV2::EN_GOTYPE_ROADSECTION:
			{
				StuSpaceFileStructV2::StuGisFeatureRoadSection* pArrayOfStGisFeatureRoadSection = 
					(StuSpaceFileStructV2::StuGisFeatureRoadSection*)(pBufOfBeforeCompress+sizeof(StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData));

				for(int i = 0; i < pHeader->nFeatureCount; i++)
				{
					CGIS_FeatureRoadSectionV2* pFtr = new CGIS_FeatureRoadSectionV2;
					ASSERT(pFtr);
					memcpy(pFtr->GetDataAddr(),&pArrayOfStGisFeatureRoadSection[i],sizeof(StuSpaceFileStructV2::StuGisFeatureRoadSection));
                    char* pCharAddrOld = pCharAddr;
                    char* pShortAddrOld = pShortAddr;
                    char* pDwordAddrOld = pDwordAddr;
					pFtr->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
                    ASSERTVALID(pFtr);
				
                    int nRoadSectionID = pFtr->GetRutID();
                    if(nRoadSectionID == 65535)
                    {
					    (*(pItem->pFtrContainer))[pFtr->GetRutID()] = pFtr;
                    }
                    else
                    {
                        CGIS_FeatureV2* pFtrOld = (*(pItem->pFtrContainer))[pFtr->GetRutID()];
                        ASSERT(!pFtrOld);
					    (*(pItem->pFtrContainer))[pFtr->GetRutID()] = pFtr;
                    }
				}

			}
			break;
		case CGIS_FeatureV2::EN_GOTYPE_LINE:
			{
				StuSpaceFileStructV2::StuGisFeatureLine* pArrayOfStGisFeature = 
					(StuSpaceFileStructV2::StuGisFeatureLine*)(pBufOfBeforeCompress+sizeof(StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData));
				
				for(int i = 0; i < pHeader->nFeatureCount; i++)
				{
					CGIS_FeatureLineV2* pFtr = new CGIS_FeatureLineV2;
					ASSERT(pFtr);
					memcpy(pFtr->GetDataAddr(),&pArrayOfStGisFeature[i],sizeof(StuSpaceFileStructV2::StuGisFeatureLine));
					pFtr->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
                    pFtr->MakeBuddyIndependent();
                    ASSERTVALID(pFtr);

                    CGIS_FeatureV2* pFtrOld = (*(pItem->pFtrContainer))[i];
                    ASSERT(!pFtrOld);
                    (*(pItem->pFtrContainer))[i] = pFtr;
                }

			}
			break;
		case CGIS_FeatureV2::EN_GOTYPE_POLY:
            {
                StuSpaceFileStructV2::StuGisFeaturePoly* pArrayOfStGisFeature = 
					(StuSpaceFileStructV2::StuGisFeaturePoly*)(pBufOfBeforeCompress+sizeof(StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData));
				
				for(int i = 0; i < pHeader->nFeatureCount; i++)
				{
					CGIS_FeaturePolyV2* pFtr = new CGIS_FeaturePolyV2;
					ASSERT(pFtr);
					memcpy(pFtr->GetDataAddr(),&pArrayOfStGisFeature[i],sizeof(StuSpaceFileStructV2::StuGisFeaturePoly));
					pFtr->SetVarietyBaseAddr(pCharAddr,pShortAddr,pDwordAddr);
                    pFtr->MakeBuddyIndependent();
                    ASSERTVALID(pFtr);

                    CGIS_FeatureV2* pFtrOld = (*(pItem->pFtrContainer))[i];
                    ASSERT(!pFtrOld);
                    (*(pItem->pFtrContainer))[i] = pFtr;
                }

            }

			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	
	delete [] pBufOfBeforeCompress;
	pBufOfBeforeCompress = NULL;
	
	return FALSE;
}

#ifdef _DEBUG
void CSpaceFileDataV2_RoadSection::DumpState()
{
    TRACE0("********************************************************************\n");
    TRACE0("CSpaceFileDataV2_RoadSection Info:\n");
    //TRACE1("BreadthCount    : %d.\n",m_BreadthContainer.size());
    TRACE1("FtrCount        : %d.\n",GetFtrCount());
    TRACE0("********************************************************************\n");
}
void CSpaceFileDataV2_RoadSection::AssertValid() const
{
    
}
#endif



//////////////////////////////////////////////////////////////////////////
unsigned long CSpaceFileDataV2_RoadSection::GetFtrCount() const
{
    unsigned long nCount = 0;

    //遍历所有地图
    {
        MapOfGeoMap* pMapContainer = m_pMapContainer;
        MapOfGeoMap::iterator iteratorOfMap;
        for(iteratorOfMap = m_pMapContainer->begin();
        iteratorOfMap != m_pMapContainer->end(); iteratorOfMap++)
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

                MapOfLayer* pLayerContainer = pItem->pLayerContainer;
                MapOfLayer::iterator iteratorOfLayer;
                for(iteratorOfLayer = pLayerContainer->begin();
                iteratorOfLayer != pLayerContainer->end(); iteratorOfLayer++)
                {
                    pair <unsigned long ,stuLayerItem* > aPair = *iteratorOfLayer;
                    stuLayerItem* pItem = aPair.second;
                                        
                    MapOfFeature* pFtrContainer = pItem->pFtrContainer;
                    nCount += pFtrContainer->size();
                }
            }
        }
    }


    return nCount;
}

BOOL CSpaceFileDataV2_RoadSection::ClearOperation()
{
    if(!m_pMapContainer)
        return FALSE;
        
    //遍历所有地图
    {
        MapOfGeoMap* pMapContainer = m_pMapContainer;
        MapOfGeoMap::iterator iteratorOfMap;
        for(iteratorOfMap = m_pMapContainer->begin();
        iteratorOfMap != m_pMapContainer->end(); iteratorOfMap++)
        {
            pair <unsigned long ,stuMapItem* > aPair = *iteratorOfMap;
            stuMapItem* pItem = aPair.second;

            ASSERT(pItem->pIndex);
            delete pItem->pIndex;
            
            MapOfBreadth* pBreadthContainer = pItem->pBreadthContainer;          
            MapOfBreadth::iterator iteratorOfBreadth;
            for(iteratorOfBreadth = pBreadthContainer->begin();
            iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
            {
                pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
                stuBreadthItem* pItem = aPair.second;
                
                ASSERT(pItem->pIndex);
                delete pItem->pIndex;
                               
                MapOfLayer* pLayerContainer = pItem->pLayerContainer;
                MapOfLayer::iterator iteratorOfLayer;
                for(iteratorOfLayer = pLayerContainer->begin();
                iteratorOfLayer != pLayerContainer->end(); iteratorOfLayer++)
                {
                    pair <unsigned long ,stuLayerItem* > aPair = *iteratorOfLayer;
                    stuLayerItem* pItem = aPair.second;
                    
                    ASSERT(pItem->pIndex);
                    delete pItem->pIndex;
                                        
                    MapOfFeature* pFtrContainer = pItem->pFtrContainer;
                    MapOfFeature::iterator iteratorOfFtr;
                    for(iteratorOfFtr = pFtrContainer->begin();
                    iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                    {
                        pair <unsigned long ,CGIS_FeatureV2* > aPair = *iteratorOfFtr;
                        CGIS_FeatureV2* pFtr = aPair.second;
                        ASSERT(pFtr);
                        delete pFtr; 
                        pFtr = NULL;
                    }
                    delete pFtrContainer;
                }
                delete pLayerContainer;
            }
            delete pBreadthContainer;
        }
        delete pMapContainer;
    }

    m_pMapContainer = NULL;
    return TRUE;
}