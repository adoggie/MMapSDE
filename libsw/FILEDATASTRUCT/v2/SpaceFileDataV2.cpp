// SpaceFileDataV2.cpp: implementation of the CSpaceFileDataV2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "./FileDataStruct/V2/GIS_FeaturePointV2.h"
#include "./FileDataStruct/V2/GIS_FeatureRoadSectionV2.h"
#include "./FileDataStruct/V2/GIS_FeatureLineV2.h"
#include "./FileDataStruct/V2/GIS_FeaturePolyV2.h"

#include "./Zlib/zlib.h"
#include "./DebugFunctionLib/Watch.h"

#include "SpaceFileDataV2.h"

static unsigned long nFtrTotalCount = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpaceFileDataV2::CSpaceFileDataV2()
{
    m_pMapContainer = NULL;
}

CSpaceFileDataV2::~CSpaceFileDataV2()
{
    ClearOperation();
}

BOOL CSpaceFileDataV2::Load(LPCTSTR strFilePath)
{
#ifdef ANALYSE_DATA
    m_DataAnalyse.Initial();
#endif

	CFile aFile(strFilePath,CFile::modeRead);
	CFile* pFile = &aFile;
    ReadSpaceData(pFile);
    return FALSE;
}
BOOL CSpaceFileDataV2::ReadSpaceData(CFile* pFile)
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

BOOL CSpaceFileDataV2::ReadMap(CFile* pFile,stuMapItem* pMapItem)
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
#ifdef ANALYSE_DATA
        m_DataAnalyse.m_nBreadthCount++;
#endif
		if(!(i%10000))
		{
			static DWORD dwtickstart = 0;
			TRACE2("----ReadBreadthV2(%.5d):%d ms--------\n",i,GetTickCount() - dwtickstart);
			dwtickstart = GetTickCount();
		}

        stuBreadthItem* pItem = new stuBreadthItem;
        pItem->pLayerContainer = new MapOfLayer;
        pItem->pIndex = new StuSpaceFileStructV2::StuIndexInfoOfBreadth;
        memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuSpaceFileStructV2::StuIndexInfoOfBreadth));
        pItem->pIndex->nID = i;
#ifdef ANALYSE_DATA
        if(0 == pItem->pIndex->nLayerCount)
        {
            m_DataAnalyse.m_nEmptyPackageCount++;
        }
#endif
        
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

BOOL CSpaceFileDataV2::ReadBreadth(CFile* pFile,
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

BOOL CSpaceFileDataV2::ReadLayer(CFile* pFile,stuLayerItem* pItem)
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
    
#endif
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

#ifdef ANALYSE_DATA
        {
            unsigned long nFtrCount = pHeader->nFeatureCount;
            m_DataAnalyse.m_nFtrCount += nFtrCount;
            if(nFtrCount > m_DataAnalyse.m_nMaxFtrCountOfBreadth)
            {
                m_DataAnalyse.m_nMaxFtrCountOfBreadth = nFtrCount;
            }
            if(nFtrCount < m_DataAnalyse.m_nMinFtrCountOfBreadth)
            {
                m_DataAnalyse.m_nMinFtrCountOfBreadth = nFtrCount;
            }
        }  
#endif
        
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
void CSpaceFileDataV2::DumpState()
{
    if(!m_pMapContainer)
        return;

    TRACE0("**********CSpaceFileDataV2 infomation********************\n");
    TRACE1("FtrTotalCount    : %d.\n",GetFtrCount());
#ifdef ANALYSE_DATA
    m_DataAnalyse.DumpState();
#endif
    TRACE0("*********************************************************\n\n");
}
void CSpaceFileDataV2::AssertValid() const
{
    
}
#endif

//////////////////////////////////////////////////////////////////////////
unsigned long CSpaceFileDataV2::GetFtrCount()
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

BOOL CSpaceFileDataV2::ClearOperation()
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
    return FALSE;
}

BOOL CSpaceFileDataV2::write(LPCTSTR strFileName)
{
	//写文件
	CMemFile aMemFile;
    WriteSpaceData(&aMemFile);
  
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

BOOL CSpaceFileDataV2::WriteSpaceData(CFile* pFile)
{
    ASSERT(m_pMapContainer);
    MapOfGeoMap* pMapContainer = m_pMapContainer;
    
	//---------------------------
	//构造头,写头
    StuSpaceFileStructV2 :: StuHeaderInfo* pFileHeadInfo = new StuSpaceFileStructV2 :: StuHeaderInfo;
    memset(pFileHeadInfo,0,sizeof(StuSpaceFileStructV2 :: StuHeaderInfo));
    strcpy(pFileHeadInfo->chSign,"BAN UNI_1");
    pFileHeadInfo->nMapCount = pMapContainer->size();//(全国图+详图)
    pFile->Write(pFileHeadInfo,sizeof(StuSpaceFileStructV2 :: StuHeaderInfo));
    
    //---------------------------
    int nCount = pFileHeadInfo->nMapCount;
    DWORD dwOffsetBase = pFile->GetPosition();
    //写入地图索引
    StuSpaceFileStructV2::StuIndexInfoOfMap* pIndexInfoOfMap = 
        new StuSpaceFileStructV2::StuIndexInfoOfMap[nCount];
    memset(pIndexInfoOfMap,0,sizeof(StuSpaceFileStructV2::StuIndexInfoOfMap)*nCount);
    
    DWORD IndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfMap,sizeof(StuSpaceFileStructV2::StuIndexInfoOfMap) * nCount);
    
    int nIndexN = 0;
    //////////////////////////////////////////////////////////////////////////
    MapOfGeoMap::iterator iteratorOfGeoMap;
    for(iteratorOfGeoMap = pMapContainer->begin();
    iteratorOfGeoMap != pMapContainer->end();iteratorOfGeoMap++)
    {
        pair <unsigned long ,stuMapItem* > aPair = *iteratorOfGeoMap;
        stuMapItem* pMapItem = aPair.second;

        pMapItem->pIndex->nOffset = pFile->GetPosition() - dwOffsetBase;
        WriteMap(pFile,pMapItem);
        memcpy(&pIndexInfoOfMap[nIndexN],pMapItem->pIndex,sizeof(StuSpaceFileStructV2::StuIndexInfoOfMap));
        nIndexN++;
    }

      
    pFile->Seek(IndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfMap,sizeof(StuSpaceFileStructV2::StuIndexInfoOfMap) * nCount);

    delete pIndexInfoOfMap;
    pIndexInfoOfMap = NULL;

    delete pFileHeadInfo; 
    pFileHeadInfo = NULL;
    return FALSE;
}
BOOL CSpaceFileDataV2::WriteMap(
        CFile* pFile,
        stuMapItem* pItem)
{
    ASSERT(pFile);
    ASSERT(pItem);
    StuSpaceFileStructV2 :: StuIndexInfoOfMap* pIndexInfoOfMap = pItem->pIndex;
    ASSERT(pIndexInfoOfMap);
    ASSERTVALID(pIndexInfoOfMap);
    MapOfBreadth* pContainer = pItem->pBreadthContainer;
    ASSERT(pContainer);
//////////////////////////////////////////////////////////////////////////


    DWORD dwOffsetBase = pFile->GetPosition();

    int nCount = pIndexInfoOfMap->nBreadthCount;
    StuSpaceFileStructV2::StuIndexInfoOfBreadth* pIndexInfoOfBreadth 
        = new StuSpaceFileStructV2::StuIndexInfoOfBreadth[nCount];
    memset(pIndexInfoOfBreadth,0,sizeof(StuSpaceFileStructV2::StuIndexInfoOfBreadth)*nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwIndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuSpaceFileStructV2::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////
    
    int nIndexN = 0;
    MapOfBreadth::iterator iteratorOfContainer;
    for(iteratorOfContainer = pContainer->begin();
        iteratorOfContainer != pContainer->end(); iteratorOfContainer++)
        {
            //WATCH(_T("write breadth"));
            pair < unsigned long,stuBreadthItem*> aPair = *iteratorOfContainer;
            stuBreadthItem* pBreadthItem = aPair.second;
          
            pBreadthItem->pIndex->nOffset = pFile->GetPosition() - dwOffsetBase;
            WriteBreadth(pFile,pBreadthItem);
            memcpy(&pIndexInfoOfBreadth[nIndexN],pBreadthItem->pIndex,sizeof(StuSpaceFileStructV2::StuIndexInfoOfBreadth));
            ASSERT(pBreadthItem);
           
            //pIndexInfoOfBreadth[nIndex].nID         = nID;
            //pIndexInfoOfBreadth[nIndex].nOffset     = pFile->GetPosition();
            //pIndexInfoOfBreadth[nIndex].nLayerCount = 1;

            nIndexN++;
            if(!(nIndexN&0xff))
            {
                static DWORD dwTickStart = GetTickCount();
                TRACE2("write breadth %d..%d ms\n",nIndexN,GetTickCount() - dwTickStart);
                dwTickStart = GetTickCount();
            }
        }

    //////////////////////////////////////////////////////////////////////////
    pFile->Seek(dwIndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuSpaceFileStructV2::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////
    
    delete pIndexInfoOfBreadth;
    pIndexInfoOfMap = NULL;
    return TRUE;
    
}
BOOL CSpaceFileDataV2::WriteBreadth(
    CFile* pFile,
    stuBreadthItem* pItem)
{
    ASSERT(pFile);
    ASSERT(pItem);
    ASSERT(pItem->pIndex);
    ASSERTVALID(pItem->pIndex);
    ASSERT(pItem->pLayerContainer);
//////////////////////////////////////////////////////////////////////////
    MapOfLayer* pContainer = pItem->pLayerContainer;
    ASSERT(pContainer);
    StuSpaceFileStructV2 :: StuIndexInfoOfBreadth* pIndexInfoOfBreadth = pItem->pIndex;

    DWORD dwOffsetBase = pFile->GetPosition();
  	//////////////////////////////////////////////////////////////////////////
	//写图幅中的所有层的索引
    int nCount = pIndexInfoOfBreadth->nLayerCount;
	StuSpaceFileStructV2 :: StuIndexInfoOfLayerInBreadthData* pLayerIndex = 
        new StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData[nCount];
    memset(pLayerIndex,0,sizeof(StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData)*nCount);
    //////////////////////////////////////////////////////////////////////////
	DWORD dwIndexPos = pFile->GetPosition();
	pFile->Write(pLayerIndex,sizeof(StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData) * nCount );
	//////////////////////////////////////////////////////////////////////////

    //写图幅中的所有层的数据
    int nIndexN = 0;
    MapOfLayer::iterator iteratorOfContainer;
    for(iteratorOfContainer = pContainer->begin();
    iteratorOfContainer != pContainer->end(); iteratorOfContainer++)
    {
        pair < unsigned long,stuLayerItem*> aPair = *iteratorOfContainer;
        stuLayerItem* pLayerItem = aPair.second;
              
        pLayerItem->pIndex->nOffset = pFile->GetPosition() - dwOffsetBase;
        WriteLayer( pFile, pLayerItem);		
        memcpy(&pLayerIndex[nIndexN],pLayerItem->pIndex,sizeof(StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData));

        
        nIndexN++;
    } 
    //////////////////////////////////////////////////////////////////////////
	pFile->Seek(dwIndexPos,CFile::begin);
	pFile->Write(pLayerIndex,sizeof(StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData) * nCount);
    //////////////////////////////////////////////////////////////////////////
    
	pFile->SeekToEnd();//重新定位到尾部

	delete pLayerIndex;
	pLayerIndex = NULL;

    return FALSE;

 

}
static char s_BuddyBuf [10000000];
static char s_DestBuf2 [10000000];
BOOL CSpaceFileDataV2::WriteLayer(
    CFile* pFile,
    stuLayerItem* pItem)
{
    ASSERT(pFile);
    ASSERT(pItem);
    ASSERT(pItem->pIndex);
    ASSERTVALID(pItem->pIndex);
    ASSERT(pItem->pFtrContainer);
//////////////////////////////////////////////////////////////////////////
    MapOfFeature* pFtrContainer = pItem->pFtrContainer;
    ASSERT(pFtrContainer);
    StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData* pLayerIndex = pItem->pIndex;
      
    StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData* pHeader = 
        new StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData;
    memset(pHeader,0,sizeof(StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData));
    pHeader->nFeatureCount = pFtrContainer->size();
    

    CMemFile aMemFile(1000000);
    {
        CFile* pFile = &aMemFile;        
        //注入层头
        DWORD dwHeaderPos = pFile->GetPosition();
		pFile->Write(pHeader,		sizeof(StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData));

        switch(pLayerIndex->nType)
        {
        case CGIS_FeatureV2::EN_GOTYPE_POINT:
            {
                MapOfRoadSection::iterator iteratorOfRoadSection;
                for(iteratorOfRoadSection = pFtrContainer->begin();
                iteratorOfRoadSection != pFtrContainer->end(); iteratorOfRoadSection++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfRoadSection;
                    CGIS_FeaturePointV2* pFtr = (CGIS_FeaturePointV2*)aPair.second;
                    
                    pFile->Write(pFtr->GetDataAddr(),sizeof(StuSpaceFileStructV2::StuGisFeaturePoint));
                }
            }
            break;
        case CGIS_FeatureV2::EN_GOTYPE_ROADSECTION:
            {
                MapOfRoadSection::iterator iteratorOfRoadSection;
                for(iteratorOfRoadSection = pFtrContainer->begin();
                iteratorOfRoadSection != pFtrContainer->end(); iteratorOfRoadSection++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfRoadSection;
                    CGIS_FeatureRoadSectionV2* pFtr = (CGIS_FeatureRoadSectionV2*)aPair.second;
                    
                    pFile->Write(pFtr->GetDataAddr(),sizeof(StuSpaceFileStructV2::StuGisFeatureRoadSection));
                }
            }
            break;
        case CGIS_FeatureV2::EN_GOTYPE_LINE:
            {
                MapOfRoadSection::iterator iteratorOfRoadSection;
                for(iteratorOfRoadSection = pFtrContainer->begin();
                iteratorOfRoadSection != pFtrContainer->end(); iteratorOfRoadSection++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfRoadSection;
                    CGIS_FeatureLineV2* pFtr = (CGIS_FeatureLineV2*)aPair.second;
                    
                    pFile->Write(pFtr->GetDataAddr(),sizeof(StuSpaceFileStructV2::StuGisFeatureLine));
                }
            }
            break;
        case CGIS_FeatureV2::EN_GOTYPE_POLY:
            {
                MapOfRoadSection::iterator iteratorOfRoadSection;
                for(iteratorOfRoadSection = pFtrContainer->begin();
                iteratorOfRoadSection != pFtrContainer->end(); iteratorOfRoadSection++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfRoadSection;
                    CGIS_FeaturePolyV2* pFtr = (CGIS_FeaturePolyV2*)aPair.second;
                    
                    pFile->Write(pFtr->GetDataAddr(),sizeof(StuSpaceFileStructV2::StuGisFeaturePoly));
                }
            }
            break;
        default:
            ASSERT(FALSE);
            break;
        }
        //////////////////////////////////////////////////////////////////////////
        //在这里要转换所有Ftr的Buddy数据
        {
            char* pBuddy = s_BuddyBuf;
            unsigned long nCharDataBuddyLength;
            unsigned long nShortDataBuddyLength;
            unsigned long nDwordDataBuddyLength;

            unsigned long nBuddyDataSize;
            VERIFY(GetBuddyDataSize(pFtrContainer,pLayerIndex->nType,nBuddyDataSize));
            ASSERT(nBuddyDataSize <= sizeof(s_BuddyBuf));
            
            memset(s_BuddyBuf,0,nBuddyDataSize);
            CollectBuddyData(
                pFtrContainer,
                pBuddy,
                pLayerIndex->nType,
                nCharDataBuddyLength,
                nShortDataBuddyLength,
                nDwordDataBuddyLength);

            ASSERT(nBuddyDataSize = nCharDataBuddyLength+ nShortDataBuddyLength + nDwordDataBuddyLength);
            pFile->Write(s_BuddyBuf,nBuddyDataSize);
                
            pHeader->nCharDataBuddyLength = nCharDataBuddyLength;
            pHeader->nShortDataBuddyLength = nShortDataBuddyLength;
            pHeader->nDwordDataBuddyLength = nDwordDataBuddyLength;

            pFile->Seek(dwHeaderPos,CFile::begin);
            pFile->Write(pHeader,sizeof(StuSpaceFileStructV2::StuHeaderInfoOfLayerInBreadthData));

        }
        //紧跟着写入    
    }

    ASSERTVALID(pHeader);
    //////////////////////////////////////////////////////////////////////////
    

    //////////////////////////////////////////////////////////////////////////
    //压缩数据
	unsigned long nSrcSize = aMemFile.GetLength();
	unsigned char* pSrcData = aMemFile.Detach();
	unsigned char* pDestBuf =(unsigned char*) s_DestBuf2;//new unsigned char[0x1000000];
	unsigned long nDestSize = compressBound(nSrcSize);
	compress2(pDestBuf,&nDestSize,pSrcData,nSrcSize,Z_DEFAULT_COMPRESSION);
	ASSERT(nDestSize > 0);

	pLayerIndex->nBeforeCompressSize  = nSrcSize;
	pLayerIndex->nAfterCompressSize   = nDestSize;

   	delete pHeader;
	pHeader = NULL;
    //////////////////////////////////////////////////////////////////////////
    //真正写入数据
    pFile->Write(pDestBuf,nDestSize);
    delete [] pSrcData;
    pSrcData = NULL;
    return TRUE;

}


BOOL CSpaceFileDataV2::CollectBuddyData(
     MapOfFeature* pFtrContainer,
     char* pBuddy,
     int nType,
     unsigned long& nCharDataBuddyLength,
     unsigned long& nShortDataBuddyLength,
     unsigned long& nDwordDataBuddyLength)
{
    ASSERT(pFtrContainer);
     
    ASSERT(pBuddy);
    nCharDataBuddyLength    = 0;
    nShortDataBuddyLength   = 0;
    nDwordDataBuddyLength   = 0;
        
	int nNum;
	int nSize;
	char* pBuddyData = pBuddy;
	ASSERT(pBuddyData);
	*( (DWORD*)pBuddyData ) = 'y'<<24|'l'<<16|'d'<<8|' '<<0;
	char* pCurrent = pBuddyData+4;


	int nCount = pFtrContainer->size();
	//根据类型处理
	switch(nType)
	{
	case CGIS_FeatureV2::EN_GOTYPE_POINT:
		{
            
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                CGIS_FeaturePointV2* pFtr = (CGIS_FeaturePointV2*)aPair.second;
                ASSERT(pFtr);
                				
				nNum = pFtr->GetAnnoNum();
				ASSERT(nNum <= 65);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					memcpy(pCurrent,pFtr->GetAnnotation(),nSize);
					pCurrent += nSize;
				}


				nNum = pFtr->GetStrAddressLength();
				ASSERT(nNum <= 255);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					memcpy(pCurrent,pFtr->GetAddress(),nSize);
					pCurrent += nSize;
                }


				nNum = pFtr->GetStrPhoneLength();
				ASSERT(nNum <= 255);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					memcpy(pCurrent,pFtr->GetPhoneNum(),nSize);
					pCurrent += nSize;
				}
									
			}
			//--------------------------------------

			int nCountOfBuddyData = pCurrent - pBuddyData;
			ASSERT(nCountOfBuddyData >= 4);
			nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
			pCurrent = pBuddyData + nCountOfBuddyData;			
			nCharDataBuddyLength = nCountOfBuddyData;

		}

		break;
    case CGIS_FeatureV2::EN_GOTYPE_ROADSECTION:
        {
            {
				//由于字符串单项对齐开销太大,所以整合在一起进行对齐

                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRoadSectionV2* pFtr = (CGIS_FeatureRoadSectionV2*)aPair.second;
                    ASSERT(pFtr);
                    ASSERTVALID(pFtr);
                    
                    nNum = pFtr->GetAnnoNum();
                    ASSERT(nNum <= 64);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(char) * nNum;
                        memcpy(pCurrent,pFtr->GetAnnotation(),nSize);
                        pCurrent += nSize;
                    }             
                }
           	
				int nCountOfBuddyData = pCurrent - pBuddyData;
				ASSERT(nCountOfBuddyData >= 4);
				nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
				pCurrent = pBuddyData + nCountOfBuddyData;
		
				//写长度
				nCharDataBuddyLength = nCountOfBuddyData;

			}

            //////////////////////////////////////////////////////////////////////////
            
			{				
				//把坐标点整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRoadSectionV2* pFtr = (CGIS_FeatureRoadSectionV2*)aPair.second;
                    ASSERT(pFtr);
					
					nNum = pFtr->GetPtNum();
					//ASSERT(nNum < 50);
					if( nNum > 0 )
					{
						nSize = sizeof(float) * 2 * nNum;
						memcpy(pCurrent,pFtr->GetPtList(),nSize);
						pCurrent += nSize;
					}

				}

				int nCountOfBuddyData = pCurrent - pBuddyData;
				ASSERT(nCountOfBuddyData >= 4);
				nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
				pCurrent = pBuddyData + nCountOfBuddyData;

				nDwordDataBuddyLength = nCountOfBuddyData - nCharDataBuddyLength;

			}


			//--------------------------------------
		}
		break;
	case CGIS_FeatureV2::EN_GOTYPE_LINE:
		{
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                CGIS_FeatureLineV2* pFtr = (CGIS_FeatureLineV2*)aPair.second;
                ASSERT(pFtr);
                
                nNum = pFtr->GetAnnoNum();
                ASSERT(nNum <= 64);
                if( nNum > 0 )
                {
                    nSize = sizeof(char) * nNum;
                    memcpy(pCurrent,pFtr->GetAnnotation(),nSize);
                    pCurrent += nSize;
                }
                
            }
            {
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
                //写偏移
                nCharDataBuddyLength = nCountOfBuddyData;
                
            }

			
			//////////////////////////////////////////////////////////////////////////

			{			
				//把dword数据整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                    CGIS_FeatureLineV2* pFtr = (CGIS_FeatureLineV2*)aPair.second;
                    ASSERT(pFtr);
                    
					nNum = pFtr->GetPtNum();
					//ASSERT(nNum < 11);
					if( nNum > 0 )
					{
						nSize = sizeof(float) * 2 * nNum;
						memcpy(pCurrent,pFtr->GetPtList(),nSize);
						pCurrent += nSize;
					}

					
					nNum = pFtr->GetPartNum();
					//ASSERT(nNum < 11);
					if( nNum > 0 )
					{
						nSize = sizeof(int) * nNum;
						memcpy(pCurrent,pFtr->GetPart(),nSize);
						pCurrent += nSize;
					}

				}

				int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
                nDwordDataBuddyLength = nCountOfBuddyData - nCharDataBuddyLength;
                
            }
            
            //--------------------------------------
        }
        break;
    case CGIS_FeatureV2::EN_GOTYPE_POLY:
        {
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                CGIS_FeaturePolyV2* pFtr = (CGIS_FeaturePolyV2*)aPair.second;
                ASSERT(pFtr);
                
                nNum = pFtr->GetAnnoNum();
                ASSERT(nNum <= 64);
                if( nNum > 0 )
                {
                    nSize = sizeof(char) * nNum;
                    memcpy(pCurrent,pFtr->GetAnnotation(),nSize);
                    pCurrent += nSize;
                    
                }
            }
            {
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
                //写偏移
                nCharDataBuddyLength = nCountOfBuddyData;
                
            }
            
            
            //////////////////////////////////////////////////////////////////////////
            
            {
                
                //把dword数据整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                    CGIS_FeaturePolyV2* pFtr = (CGIS_FeaturePolyV2*)aPair.second;
                    ASSERT(pFtr);
                    
                    nNum = pFtr->GetPtNum();
                    //ASSERT(nNum < 11);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(float) * 2 * nNum;
                        memcpy(pCurrent,pFtr->GetPtList(),nSize);
                        pCurrent += nSize;
                    }
                    else
                        
                        
                        nNum = pFtr->GetPartNum();
                    //ASSERT(nNum < 11);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(int) * nNum;
                        memcpy(pCurrent,pFtr->GetPart(),nSize);
                        pCurrent += nSize;
                    }
                    
                }
                
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
                nDwordDataBuddyLength = nCountOfBuddyData - nCharDataBuddyLength;
                
            }
            
            
        }
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	int nCountOfBuddyData = pCurrent - pBuddyData;
	ASSERT(nCountOfBuddyData >= 4);
	ASSERT( nCountOfBuddyData == ((nCountOfBuddyData + 0x03)&0xfffffffc)	);//以4为边界对齐
	ASSERT( nCountOfBuddyData == nCharDataBuddyLength + nShortDataBuddyLength + nDwordDataBuddyLength);

	return TRUE;
}

BOOL CSpaceFileDataV2::GetBuddyDataSize( 
                      MapOfFeature* pFtrContainer,
                      int nType,
                      unsigned long& nBuddyDataSize)
{
    ASSERT(pFtrContainer);
     
        
	int nNum;
	int nSize;
	char* pBuddyData = NULL;
	char* pCurrent = pBuddyData+4;


	int nCount = pFtrContainer->size();
	//根据类型处理
	switch(nType)
	{
	case CGIS_FeatureV2::EN_GOTYPE_POINT:
		{
            
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                CGIS_FeaturePointV2* pFtr = (CGIS_FeaturePointV2*)aPair.second;
                ASSERT(pFtr);
                				
				nNum = pFtr->GetAnnoNum();
				ASSERT(nNum <= 65);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					pCurrent += nSize;
				}


				nNum = pFtr->GetStrAddressLength();
				ASSERT(nNum <= 255);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					pCurrent += nSize;
                }


				nNum = pFtr->GetStrPhoneLength();
				ASSERT(nNum <= 255);
				if( nNum > 0 )
				{
					nSize = sizeof(char) * nNum;
					pCurrent += nSize;
				}
									
			}
			//--------------------------------------

			int nCountOfBuddyData = pCurrent - pBuddyData;
			ASSERT(nCountOfBuddyData >= 4);
			nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
			pCurrent = pBuddyData + nCountOfBuddyData;			

		}

		break;
    case CGIS_FeatureV2::EN_GOTYPE_ROADSECTION:
        {
            {
				//由于字符串单项对齐开销太大,所以整合在一起进行对齐

                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRoadSectionV2* pFtr = (CGIS_FeatureRoadSectionV2*)aPair.second;
                    ASSERT(pFtr);
                    
                    nNum = pFtr->GetAnnoNum();
                    ASSERT(nNum <= 100);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(char) * nNum;
                        pCurrent += nSize;
                    }             
                }
           	
				int nCountOfBuddyData = pCurrent - pBuddyData;
				ASSERT(nCountOfBuddyData >= 4);
				nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
				pCurrent = pBuddyData + nCountOfBuddyData;
		
				//写长度

			}

            //////////////////////////////////////////////////////////////////////////
            
			{				
				//把坐标点整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRoadSectionV2* pFtr = (CGIS_FeatureRoadSectionV2*)aPair.second;
                    ASSERT(pFtr);
					
					nNum = pFtr->GetPtNum();
					//ASSERT(nNum < 50);
					if( nNum > 0 )
					{
						nSize = sizeof(float) * 2 * nNum;
						pCurrent += nSize;
					}

				}

				int nCountOfBuddyData = pCurrent - pBuddyData;
				ASSERT(nCountOfBuddyData >= 4);
				nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
				pCurrent = pBuddyData + nCountOfBuddyData;


			}


			//--------------------------------------
		}
		break;
	case CGIS_FeatureV2::EN_GOTYPE_LINE:
		{
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                CGIS_FeatureLineV2* pFtr = (CGIS_FeatureLineV2*)aPair.second;
                ASSERT(pFtr);
                
                nNum = pFtr->GetAnnoNum();
                ASSERT(nNum <= 64);
                if( nNum > 0 )
                {
                    nSize = sizeof(char) * nNum;
                    pCurrent += nSize;
                }
                
            }

            {
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                                
            }

			
			//////////////////////////////////////////////////////////////////////////

			{			
				//把dword数据整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                    CGIS_FeatureLineV2* pFtr = (CGIS_FeatureLineV2*)aPair.second;
                    ASSERT(pFtr);
                    
					nNum = pFtr->GetPtNum();
					//ASSERT(nNum < 11);
					if( nNum > 0 )
					{
						nSize = sizeof(float) * 2 * nNum;
						pCurrent += nSize;
					}

					
					nNum = pFtr->GetPartNum();
					//ASSERT(nNum < 11);
					if( nNum > 0 )
					{
						nSize = sizeof(int) * nNum;
						pCurrent += nSize;
					}

				}

				int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                                
            }
            
            //--------------------------------------
        }
        break;
    case CGIS_FeatureV2::EN_GOTYPE_POLY:
        {
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pFtrContainer->begin();
            iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                CGIS_FeaturePolyV2* pFtr = (CGIS_FeaturePolyV2*)aPair.second;
                ASSERT(pFtr);
                
                nNum = pFtr->GetAnnoNum();
                ASSERT(nNum <= 64);
                if( nNum > 0 )
                {
                    nSize = sizeof(char) * nNum;
                    pCurrent += nSize;
                    
                }
            }
            
            {
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
            }
            
                
                //////////////////////////////////////////////////////////////////////////
            
            {
                
                //把dword数据整合在一起进行对齐
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureV2*> aPair = *iteratorOfFtr;
                    CGIS_FeaturePolyV2* pFtr = (CGIS_FeaturePolyV2*)aPair.second;
                    ASSERT(pFtr);
                    
                    nNum = pFtr->GetPtNum();
                    //ASSERT(nNum < 11);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(float) * 2 * nNum;
                        pCurrent += nSize;
                    }
                    else
                        
                        
                        nNum = pFtr->GetPartNum();
                    //ASSERT(nNum < 11);
                    if( nNum > 0 )
                    {
                        nSize = sizeof(int) * nNum;
                        pCurrent += nSize;
                    }
                    
                }
                
                int nCountOfBuddyData = pCurrent - pBuddyData;
                ASSERT(nCountOfBuddyData >= 4);
                nCountOfBuddyData = (nCountOfBuddyData + 0x03)&0xfffffffc;//以4为边界对齐
                pCurrent = pBuddyData + nCountOfBuddyData;
                
            }
                
            
        }
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	int nCountOfBuddyData = pCurrent - pBuddyData;
	ASSERT(nCountOfBuddyData >= 4);
	ASSERT( nCountOfBuddyData == ((nCountOfBuddyData + 0x03)&0xfffffffc)	);//以4为边界对齐

    nBuddyDataSize = nCountOfBuddyData;

	return TRUE;
}

#ifdef ANALYSE_DATA
void CSpaceFileDataV2::CDataAnalyse::Initial()
{
    memset(this,0,sizeof(CDataAnalyse));
    //////////////////////////////////////////////////////////////////////////
    

    m_nMinFtrCountOfBreadth     = 0xffffffff;
    m_nMinPointCountOfFeature   = 0xffffffff;
    m_dwMinBeforeCompressSize   = 0xffffffff;
    m_dwMinAfterCompressSize    = 0xffffffff;
    m_pPointCountContainer = new MapOfInt_2Dimension;
    m_pCountOfSpecialSizePackageBeforeCompressContainer = new MapOfInt_2Dimension;
    m_pCountOfSpecialSizePackageAfterCompressContainer = new MapOfInt_2Dimension;
      

}
void CSpaceFileDataV2::CDataAnalyse::DumpState()
{
	TRACE0("********************************************************************\n");
	TRACE0("CSpaceFileDataV2----\n");
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
    TRACE1("MinPointCountOfFeature          :%d.\n",m_nMinPointCountOfFeature);
    TRACE1("MaxPointCountOfFeature          :%d.\n",m_nMaxPointCountOfFeature);

//    {
//        MapOfInt_2Dimension* pContainer = m_pCountOfSpecialSizePackageBeforeCompressContainer;
//        TRACE1("ContainerSize = %d\n",pContainer->size());
//        MapOfInt_2Dimension::iterator iteratorOfItem;
//        for(iteratorOfItem = pContainer->begin(); iteratorOfItem != pContainer->end(); iteratorOfItem++)
//        {
//            pair <int,int> aPair = *iteratorOfItem;
//            TRACE3("[%.6dk--%.6dk)          :%d .\n",aPair.first*10,(aPair.first+1)*10,aPair.second);
//            TRACE_SLEEP_BY_TIMES(20);
//        }
//    }
//
//    {
//        MapOfInt_2Dimension* pContainer = m_pCountOfSpecialSizePackageAfterCompressContainer;
//        TRACE1("ContainerSize = %d\n",pContainer->size());
//        MapOfInt_2Dimension::iterator iteratorOfItem;
//        for(iteratorOfItem = pContainer->begin(); iteratorOfItem != pContainer->end(); iteratorOfItem++)
//        {
//            pair <int,int> aPair = *iteratorOfItem;
//            TRACE3("[%.6dk--%.6dk)          :%d .\n",aPair.first*10,(aPair.first+1)*10,aPair.second);
//            TRACE_SLEEP_BY_TIMES(20);
//        }
//    }
    
    {
        MapOfInt_2Dimension* pContainer = m_pPointCountContainer;
        TRACE1("ContainerSize = %d\n",pContainer->size());
        MapOfInt_2Dimension::iterator iteratorOfItem;
        for(iteratorOfItem = pContainer->begin(); iteratorOfItem != pContainer->end(); iteratorOfItem++)
        {
            pair <int,int> aPair = *iteratorOfItem;
            TRACE2("PointNum[%d]'s count = %d\n",aPair.first,aPair.second);
            TRACE_SLEEP_BY_TIMES(20);
        }
    }
    TRACE0("********************************************************************\n");

}
#endif
//////////////////////////////////////////////////////////////////////////

BOOL CSpaceFileDataV2::Analyse()
{
    AnalyseSpaceData();
    DUMPSTATE(this);
    return TRUE;
}
BOOL CSpaceFileDataV2::AnalyseSpaceData()
{
    ASSERT(m_pMapContainer);
    MapOfGeoMap* pMapContainer = m_pMapContainer;
    //////////////////////////////////////////////////////////////////////////   
#ifdef _DEBUG
    m_DataAnalyse.m_nMapCount = pMapContainer->size();
#endif
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

                       switch(pFtr->GetObjectType())
                       {
                       case CGIS_FeatureV2::EN_GOTYPE_ROADSECTION:
                           {
                               CGIS_FeatureRoadSectionV2* pRoadSection = (CGIS_FeatureRoadSectionV2*)pFtr;
                               int nCount = pRoadSection->GetPtNum();

#ifdef _DEBUG
                               CDataAnalyse::MapOfInt_2Dimension* pContainer = m_DataAnalyse.m_pPointCountContainer;
                               if(pContainer->find(nCount) == pContainer->end())
                               {
                                   (*pContainer)[nCount] = 0;
                               }
                               
                               (*m_DataAnalyse.m_pPointCountContainer)[nCount]++;

                               if( pRoadSection->GetPtNum() < m_DataAnalyse.m_nMinPointCountOfFeature )
                               {
                                   m_DataAnalyse.m_nMinPointCountOfFeature = pRoadSection->GetPtNum();
                               }
                               if( pRoadSection->GetPtNum() > m_DataAnalyse.m_nMaxPointCountOfFeature )
                               {
                                   m_DataAnalyse.m_nMaxPointCountOfFeature = pRoadSection->GetPtNum();
                               }
#endif                                
                           }
                           break;
                       }
                    }
                    
                }
                
            }
            
        }
        
    }


    return TRUE;
}