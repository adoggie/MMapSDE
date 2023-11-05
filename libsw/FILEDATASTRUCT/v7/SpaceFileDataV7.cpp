// SpaceFileDataV7.cpp: implementation of the CSpaceFileDataV7 class.
//
//////////////////////////////////////////////////////////////////////

#include "./stdafx.h"
#include "SpaceFileDataV7.h"

#include "./FileDataStruct/V7/GIS_FeatureRoadSectionV7.h"
#include "./Zlib/zlib.h"

//////////////////////////////////////////////////////////////////////////
static char s_DestBuf2[10000000];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpaceFileDataV7::CSpaceFileDataV7()
{
    m_pBreadthContainer = new MapOfBreadth;
}

CSpaceFileDataV7::~CSpaceFileDataV7()
{

}

BOOL CSpaceFileDataV7::Load(LPCTSTR strFilePath)
{
#ifdef ANALYSE_DATA
    m_DataAnalyse.Initial();
#endif
    TRACE1("SpaceFile:%s\n",strFilePath);
	CFile aFile(strFilePath,CFile::modeRead);
	CFile* pFile = &aFile;
    ReadSpaceData(pFile);

    //DUMPSTATE(this);
	return TRUE;    
}
BOOL CSpaceFileDataV7::Write(LPCTSTR strFileName)
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
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CSpaceFileDataV7::DumpState()
{
    TRACE0("********************************************************************\n");
	TRACE0("CSpaceFileDataV7 Info:\n");
    TRACE1("FtrTotalCount                   :%d.\n",GetFtrCount());
#ifdef ANALYSE_DATA
    m_DataAnalyse.DumpState();
#endif
    TRACE0("********************************************************************\n");
}
void CSpaceFileDataV7::AssertValid() const
{
    
}
#endif
//////////////////////////////////////////////////////////////////////////

BOOL CSpaceFileDataV7::WriteSpaceData(CFile* pFile)
{
	//---------------------------
	//构造头,写头

    StuSpaceFileStructV7::StuHeaderInfo aFileHeadInfo;
    {	
        memset(&aFileHeadInfo,0,sizeof(aFileHeadInfo));
        strcpy(aFileHeadInfo.chSign,"BAN UNI_1");
        aFileHeadInfo.nMapCount = 1;//(全国图+详图)
        pFile->Write(&aFileHeadInfo,sizeof(aFileHeadInfo));
    }
    //---------------------------
    int nCount = aFileHeadInfo.nMapCount;
    //写入地图索引
    StuSpaceFileStructV7::StuIndexInfoOfMap* pIndexInfoOfMap = 
        new StuSpaceFileStructV7::StuIndexInfoOfMap[nCount];
    memset(pIndexInfoOfMap,0,sizeof(StuSpaceFileStructV7::StuIndexInfoOfMap)*nCount);
    
    DWORD IndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfMap,sizeof(StuSpaceFileStructV7::StuIndexInfoOfMap) * nCount);
    //////////////////////////////////////////////////////////////////////////
    DWORD dwPreSize = pFile->GetPosition();
    for(int i = 0; i < nCount; i++)
    {
        pIndexInfoOfMap[i].nID = i;
        pIndexInfoOfMap[i].nOffset = pFile->GetPosition() - dwPreSize;
        pIndexInfoOfMap[i].nBreadthCount = m_pBreadthContainer->size();
        WriteMap(pFile,&pIndexInfoOfMap[i]);
    }
    pFile->Seek(IndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfMap,sizeof(StuSpaceFileStructV7::StuIndexInfoOfMap) * nCount);

    delete pIndexInfoOfMap;
    pIndexInfoOfMap = NULL;
    return FALSE;
}

BOOL CSpaceFileDataV7::WriteMap       (
    CFile* pFile,
    StuSpaceFileStructV7::StuIndexInfoOfMap* pIndexInfoOfMap)
{
    ASSERT(pIndexInfoOfMap);

    int nCount = pIndexInfoOfMap->nBreadthCount;
    StuSpaceFileStructV7::StuIndexInfoOfBreadth* pIndexInfoOfBreadth 
        = new StuSpaceFileStructV7::StuIndexInfoOfBreadth[nCount];
    memset(pIndexInfoOfBreadth,0,sizeof(StuSpaceFileStructV7::StuIndexInfoOfBreadth)*nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwIndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuSpaceFileStructV7::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////////
    DWORD dwPreSize = pFile->GetPosition();
    //-------------------------------------
    MapOfBreadth::iterator iteratorOfBreadth;
    MapOfBreadth* pBreadthContainer = GetBreadthContainer();
    ASSERT(pBreadthContainer);
    

    int nIndex = 0;
    for(iteratorOfBreadth = pBreadthContainer->begin();
        iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
        {
            pair < unsigned long,stuBreadthItem*> aPair = *iteratorOfBreadth;
            stuBreadthItem* pBreadthItem = aPair.second;
            ASSERT(pBreadthItem);
            MapOfFeature* pRoadSectionContainer = pBreadthItem->pFtrContainer;
            unsigned long nID = aPair.first;
           
            pIndexInfoOfBreadth[nIndex].nID         = nID;
            pIndexInfoOfBreadth[nIndex].nOffset     = pFile->GetPosition() - dwPreSize;
            WriteBreadth(pFile,
                &pIndexInfoOfBreadth[nIndex],
                pRoadSectionContainer);

            nIndex++;
        }

    //////////////////////////////////////////////////////////////////////////
    pFile->Seek(dwIndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuSpaceFileStructV7::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////
    
    delete pIndexInfoOfBreadth;
    pIndexInfoOfMap = NULL;
    return TRUE;
    
}


/**
 * @brief 根据本地数据写入
 *
 * @param 
 * @return
*/
static char s_BuddyBuf[10000000];

BOOL CSpaceFileDataV7::WriteBreadth   (
    CFile* pFile,
    StuSpaceFileStructV7::StuIndexInfoOfBreadth* pBreadthIndex,
    MapOfFeature* pRoadSectionContainer)
{	
    ASSERT(pBreadthIndex);
    StuSpaceFileStructV7::StuBreadthHeaderInfo* pHeader = new StuSpaceFileStructV7::StuBreadthHeaderInfo;
    memset(pHeader,0,sizeof(StuSpaceFileStructV7::StuBreadthHeaderInfo));
    pHeader->nFeatureCount = pRoadSectionContainer->size();
    

    CMemFile aMemFile;
    {
        CFile* pFile = &aMemFile;  
        
        //////////////////////////////////////////////////////////////////////////       
        //注入头
        DWORD dwHeadPos = pFile->GetPosition();
		pFile->Write(pHeader,		sizeof(StuSpaceFileStructV7::StuBreadthHeaderInfo));

        //注入Ftr
        MapOfFeature::iterator iteratorOfRoadSection;
        for(iteratorOfRoadSection = pRoadSectionContainer->begin();
        iteratorOfRoadSection != pRoadSectionContainer->end(); iteratorOfRoadSection++)
        {
            pair < unsigned long,CGIS_FeatureRoadSectionV7*> aPair = *iteratorOfRoadSection;
            CGIS_FeatureRoadSectionV7* pFtr = aPair.second;
       
            pFile->Write(pFtr->GetDataAddr(),sizeof(StuSpaceFileStructV7::StuGisFeatureRoadSection));
        }
        //////////////////////////////////////////////////////////////////////////
        //在这里要转换所有Ftr的Buddy数据
        {
            char* pBuddy = s_BuddyBuf;
            unsigned long nCharDataBuddyLength;
            unsigned long nShortDataBuddyLength;
            unsigned long nDwordDataBuddyLength;

            unsigned long nBuddyDataSize;
            VERIFY(GetBuddyDataSize(pRoadSectionContainer,StuSpaceFileStructV7::EN_GOTYPE_RUT,nBuddyDataSize));
            ASSERT(nBuddyDataSize <= sizeof(s_BuddyBuf));
            
            memset(s_BuddyBuf,0,nBuddyDataSize);
            CollectBuddyData(
                pRoadSectionContainer,
                pBuddy,
                StuSpaceFileStructV7::EN_GOTYPE_RUT,
                nCharDataBuddyLength,
                nShortDataBuddyLength,
                nDwordDataBuddyLength);

            ASSERT(nBuddyDataSize = nCharDataBuddyLength+ nShortDataBuddyLength + nDwordDataBuddyLength);
            pFile->Write(s_BuddyBuf,nBuddyDataSize);
                
            pHeader->nCharDataBuddyLength   = nCharDataBuddyLength;
            pHeader->nShortDataBuddyLength  = nShortDataBuddyLength;
            pHeader->nDwordDataBuddyLength  = nDwordDataBuddyLength;
        }
        //////////////////////////////////////////////////////////////////////////
        //写入图幅头的有效数据
        pFile->Seek(dwHeadPos,CFile::begin);
		pFile->Write(pHeader,sizeof(StuSpaceFileStructV7::StuBreadthHeaderInfo));
 
    }


    //////////////////////////////////////////////////////////////////////////
    

    //////////////////////////////////////////////////////////////////////////
    //压缩数据
	unsigned long nSrcSize = aMemFile.GetLength();
	unsigned char* pSrcData = aMemFile.Detach();
	unsigned char* pDestBuf =(unsigned char*) s_DestBuf2;//new unsigned char[0x1000000];
	unsigned long nDestSize = compressBound(nSrcSize);
	compress2(pDestBuf,&nDestSize,pSrcData,nSrcSize,Z_DEFAULT_COMPRESSION);
	ASSERT(nDestSize > 0);

	pBreadthIndex->nBeforeCompressSize  = nSrcSize;
	pBreadthIndex->nAfterCompressSize   = nDestSize;

   	delete pHeader;
	pHeader = NULL;
    //////////////////////////////////////////////////////////////////////////
    //真正写入数据
    pFile->Write(pDestBuf,nDestSize);
    delete [] pSrcData;
    pSrcData = NULL;
    return TRUE;
 	
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//math
/*
*	function:	Dichotomy
*		find one data from one sorted array
*	return value:	index
*/
long CSpaceFileDataV7::Dichotomy(unsigned long* pnArr, int nNumElement, unsigned long nValue)
{
	if (pnArr == NULL || nNumElement == 0)
		return -1;

	int nStart = 0;
	int nEnd = nNumElement;
	int nMid = 0;
	while( nEnd > nStart )
	{
		nMid = (nStart+nEnd)/2;
		if( pnArr[nMid] == nValue )
			return nMid;
		else if( pnArr[nMid] > nValue )
			nEnd = nMid;
		else
			nStart = nMid;
		if( nEnd-nStart <= 1 )
		{
			if (pnArr[nStart] == nValue)
				return nStart;
			else
				return -1;
		}
	}
	return -1;
}



long CSpaceFileDataV7::GetIndexByID(unsigned long nID)
{
    return Dichotomy(&m_SortIDArray[0],m_SortIDArray.size(),nID);

}
BOOL CSpaceFileDataV7::GetBuddyDataSize(
     MapOfFeature* pRoadSectionContainer,
     int nType,
     unsigned long& nBuddyDataSize)
{
	ASSERT(pRoadSectionContainer);
    MapOfFeature* pFtrContainer = pRoadSectionContainer;

	int nNum;
	int nSize;
	char* pBuddyData = NULL;
	char* pCurrent = pBuddyData+4;


	int nCount = pRoadSectionContainer->size();
	//根据类型处理
	switch(nType)
	{
	case StuSpaceFileStructV7::EN_GOTYPE_POINT:
        {
            ASSERT(FALSE);
        }
		break;
	case StuSpaceFileStructV7::EN_GOTYPE_RUT:
        {
            {
				//由于字符串单项对齐开销太大,所以整合在一起进行对齐

                MapOfFeature::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureRoadSectionV7*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRoadSectionV7* pFtr = aPair.second;
                    ASSERT(pFtr);
                    
                    nNum = pFtr->GetAnnoNum();
                    ASSERT(nNum <= 64);
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
			}

            //////////////////////////////////////////////////////////////////////////
            
			{				
				//把坐标点整合在一起进行对齐
                MapOfFeature::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureRoadSectionV7*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRoadSectionV7* pFtr = aPair.second;
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
				
                nBuddyDataSize = nCountOfBuddyData;
			}


			//--------------------------------------
		}
		break;
	case StuSpaceFileStructV7::EN_GOTYPE_LINE:
        {
            ASSERT(FALSE);
        }
		break;
	case StuSpaceFileStructV7::EN_GOTYPE_POLY:
        {
            ASSERT(FALSE);
        }
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	int nCountOfBuddyData = pCurrent - pBuddyData;
	ASSERT(nCountOfBuddyData >= 4);
	ASSERT( nCountOfBuddyData == ((nCountOfBuddyData + 0x03)&0xfffffffc)	);//以4为边界对齐
	ASSERT( nCountOfBuddyData == nBuddyDataSize);

	return TRUE;
}
BOOL CSpaceFileDataV7::CollectBuddyData(
     MapOfFeature* pRoadSectionContainer,
     char* pBuddy,
     int nType,
     unsigned long& nCharDataBuddyLength,
     unsigned long& nShortDataBuddyLength,
     unsigned long& nDwordDataBuddyLength)
{
	ASSERT(pRoadSectionContainer);
    ASSERT(pBuddy);
    nCharDataBuddyLength    = 0;
    nShortDataBuddyLength   = 0;
    nDwordDataBuddyLength   = 0;
        

    MapOfFeature* pFtrContainer = pRoadSectionContainer;

	int nNum;
	int nSize;
	char* pBuddyData = pBuddy;
	ASSERT(pBuddyData);
	*( (DWORD*)pBuddyData ) = 'y'<<24|'l'<<16|'d'<<8|' '<<0;
	char* pCurrent = pBuddyData+4;


	int nCount = pRoadSectionContainer->size();
	//根据类型处理
	switch(nType)
	{
	case StuSpaceFileStructV7::EN_GOTYPE_POINT:
        {
            ASSERT(FALSE);
        }
		break;
	case StuSpaceFileStructV7::EN_GOTYPE_RUT:
        {
            {
				//由于字符串单项对齐开销太大,所以整合在一起进行对齐

                MapOfFeature::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureRoadSectionV7*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRoadSectionV7* pFtr = aPair.second;
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
                MapOfFeature::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair < unsigned long,CGIS_FeatureRoadSectionV7*> aPair = *iteratorOfFtr;
                    CGIS_FeatureRoadSectionV7* pFtr = aPair.second;
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
	case StuSpaceFileStructV7::EN_GOTYPE_LINE:
        {
            ASSERT(FALSE);
        }
		break;
	case StuSpaceFileStructV7::EN_GOTYPE_POLY:
        {
            ASSERT(FALSE);
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

BOOL CSpaceFileDataV7::ReadSpaceData(CFile* pFile)
{

	//读头
    StuSpaceFileStructV7 :: StuHeaderInfo* pFileHeadInfo = new StuSpaceFileStructV7 :: StuHeaderInfo;
	pFile->Read(pFileHeadInfo,sizeof(StuSpaceFileStructV7 :: StuHeaderInfo));

    

	int nCount = pFileHeadInfo->nMapCount;
	//---------------------------
	//读入地图索引
	StuSpaceFileStructV7::StuIndexInfoOfMap* pIndexInfoArray = NULL;		//地图索引信息
	pIndexInfoArray = new StuSpaceFileStructV7::StuIndexInfoOfMap[nCount];
	memset(pIndexInfoArray,0,sizeof(StuSpaceFileStructV7::StuIndexInfoOfMap)*nCount);
	pFile->Read(pIndexInfoArray,sizeof(StuSpaceFileStructV7::StuIndexInfoOfMap) * nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////
	
	//------------------------------------------------------------------------
	//读入地图
    ASSERT(nCount == 1);
	for(int i = 0; i < nCount; i++)
	{
        stuMapItem* pItem = new stuMapItem;
        pItem->pBreadthContainer = new MapOfBreadth;
        pItem->pIndex = new StuSpaceFileStructV7::StuIndexInfoOfMap;
        memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuSpaceFileStructV7::StuIndexInfoOfMap));
        pFile->Seek(pItem->pIndex->nOffset + dwOffsetBase,CFile::begin);
		ReadMap(pFile,pItem);
        
        m_pBreadthContainer = pItem->pBreadthContainer;
        pItem->pBreadthContainer = NULL;
        delete pItem->pIndex;
        pItem->pIndex = NULL;
        delete pItem;
        pItem = NULL;

    }
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	//清理工作
	delete pIndexInfoArray;
	pIndexInfoArray = NULL;

	
	return TRUE;
}
BOOL CSpaceFileDataV7::ReadMap(CFile* pFile,stuMapItem* pMapItem)
{
    StuSpaceFileStructV7 :: StuIndexInfoOfMap* pIndexInfoOfMap = pMapItem->pIndex;
	ASSERT(pIndexInfoOfMap);

	//读入图幅索引
	StuSpaceFileStructV7::StuIndexInfoOfBreadth* pIndexInfoArray = NULL;	//图幅索引数组

	int nCount = pIndexInfoOfMap->nBreadthCount;
	{
		pIndexInfoArray = new StuSpaceFileStructV7::StuIndexInfoOfBreadth[nCount];
		memset(pIndexInfoArray,0,sizeof(StuSpaceFileStructV7::StuIndexInfoOfBreadth)*nCount);
		pFile->Read(pIndexInfoArray,sizeof(StuSpaceFileStructV7::StuIndexInfoOfBreadth) * nCount);
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
        stuBreadthItem* pItem = new stuBreadthItem;
        pItem->pFtrContainer = new MapOfFeature;
        pItem->pIndex = new StuSpaceFileStructV7::StuIndexInfoOfBreadth;
        memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuSpaceFileStructV7::StuIndexInfoOfBreadth));
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

BOOL CSpaceFileDataV7::ReadBreadth(CFile* pFile,stuBreadthItem* pItem)
{
    
    ASSERT(pFile);
    ASSERT(pItem);
    ASSERT(pItem->pIndex);

//////////////////////////////////////////////////////////////////////////
    StuSpaceFileStructV7 :: StuIndexInfoOfBreadth* pIndex = pItem->pIndex;
	ASSERT(pIndex);
    ASSERTVALID(pIndex);
	int nBeforeCompressSize = pIndex->nBeforeCompressSize;
	int nAfterCompressSize = pIndex->nAfterCompressSize;
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
		
		StuSpaceFileStructV7::StuBreadthHeaderInfo* pHeader = (StuSpaceFileStructV7::StuBreadthHeaderInfo*)pBufOfBeforeCompress;
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
        
        DWORD dwBuddyDataSize = pHeader->nCharDataBuddyLength+pHeader->nShortDataBuddyLength+pHeader->nDwordDataBuddyLength;
        ASSERTVALID(pHeader);
        char* pBuddyData = new char[dwBuddyDataSize];
        pItem->pBuddy = pBuddyData;

        unsigned long nFtrSize = sizeof(StuSpaceFileStructV7::StuGisFeatureRoadSection);
        unsigned long nBuddyOffset = sizeof(StuSpaceFileStructV7::StuBreadthHeaderInfo) + pHeader->nFeatureCount*nFtrSize;

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
		
        StuSpaceFileStructV7::StuGisFeatureRoadSection* pArrayOfStGisFeatureRoadSection = 
            (StuSpaceFileStructV7::StuGisFeatureRoadSection*)(pBufOfBeforeCompress+sizeof(StuSpaceFileStructV7::StuBreadthHeaderInfo));
        
        for(int i = 0; i < pHeader->nFeatureCount; i++)
        {
            CGIS_FeatureRoadSectionV7* pFtr = new CGIS_FeatureRoadSectionV7;
            ASSERT(pFtr);
            pFtr->m_pStData = new StuSpaceFileStructV7::StuGisFeatureRoadSection;
            memcpy(pFtr->m_pStData,&pArrayOfStGisFeatureRoadSection[i],sizeof(StuSpaceFileStructV7::StuGisFeatureRoadSection));
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
                CGIS_FeatureRoadSectionV7* pFtrOld = (*(pItem->pFtrContainer))[pFtr->GetRutID()];
                ASSERT(!pFtrOld);
                (*(pItem->pFtrContainer))[pFtr->GetRutID()] = pFtr;
            }
        }
    }
	
	delete [] pBufOfBeforeCompress;
	pBufOfBeforeCompress = NULL;
	
	return FALSE;
}

#ifdef ANALYSE_DATA
void CSpaceFileDataV7::CDataAnalyse::Initial()
{
    m_nFtrCount = 0;
    m_nMapCount = 0;
    //关于图幅的
    m_nBreadthCount = 0;
    m_nEmptyBreadthCount = 0;
    m_nPackageCount = 0;
    m_nEmptyPackageCount = 0;
    
    m_nMinFtrCountOfBreadth = 0xffffffff;
    m_nMaxFtrCountOfBreadth = 0;

    m_dwMinBeforeCompressSize   = 0xffffffff;
    m_dwMaxBeforeCompressSize   = 0;
    m_dwMinAfterCompressSize    = 0xffffffff;
    m_dwMaxAfterCompressSize    = 0;
      
    memset(nCountOfSpecialSizePackageBeforeCompress,0,sizeof(nCountOfSpecialSizePackageBeforeCompress));
    memset(nCountOfSpecialSizePackageAfterCompress, 0,sizeof(nCountOfSpecialSizePackageAfterCompress));

}
void CSpaceFileDataV7::CDataAnalyse::DumpState()
{
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
//////////////////////////////////////////////////////////////////////////

unsigned long CSpaceFileDataV7::GetFtrCount()
{
    unsigned long nCount = 0;

    {
        MapOfBreadth* pBreadthContainer = GetBreadthContainer();          
        MapOfBreadth::iterator iteratorOfBreadth;
        for(iteratorOfBreadth = pBreadthContainer->begin();
        iteratorOfBreadth != pBreadthContainer->end(); iteratorOfBreadth++)
        {
            pair <unsigned long ,stuBreadthItem* > aPair = *iteratorOfBreadth;
            stuBreadthItem* pItem = aPair.second;
            
            MapOfFeature* pFtrContainer = pItem->pFtrContainer;
            nCount += pFtrContainer->size();           
        }
        
    }

    return nCount;
}

