// NetworkIDFileDataV7.cpp: implementation of the CNetworkIDFileDataV7 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetworkIDFileDataV7.h"

//////////////////////////////////////////////////////////////////////////
static char s_DestBuf2[1000000];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkIDFileDataV7::CNetworkIDFileDataV7()
{
    m_pBreadthContainer = new MapOfBreadth;
}

CNetworkIDFileDataV7::~CNetworkIDFileDataV7()
{

}

BOOL CNetworkIDFileDataV7::Load(LPCTSTR strFilePath)
{
#ifdef ANALYSE_DATA
    m_DataAnalyse.Initial();
#endif  
    CFile aFile(strFilePath,CFile::modeRead);
	CFile* pFile = &aFile;
	ReadNetwork(pFile);

	//DUMPSTATE(this);
    return TRUE;
}

//按V7格式写入文件
BOOL CNetworkIDFileDataV7::Write(LPCTSTR strFileName)
{
    //////////////////////////////////////////////////////////////////////////
	//写文件
	CMemFile aMemFile;
    WriteNetworkV2(&aMemFile);
  
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
BOOL CNetworkIDFileDataV7::WriteNetwork(CFile* pFile)
{
	//---------------------------
	//构造头,写头	
    StuNetworkIDFileStructV7::StuHeaderInfo aHeadInfo;
    memset(&aHeadInfo,0,sizeof(aHeadInfo));
    strcpy(aHeadInfo.chSign,"RUT_TEST");
    aHeadInfo.nMapCount = 1;    //(只有简图)
    pFile->Write(&aHeadInfo,sizeof(aHeadInfo));
    
    //---------------------------
    //写入地图索引
    int nCount = aHeadInfo.nMapCount;
    StuNetworkIDFileStructV7::StuIndexInfoOfMap* pIndexInfoOfMap = 
        new StuNetworkIDFileStructV7::StuIndexInfoOfMap[nCount];

    //////////////////////////////////////////////////////////////////////////
    DWORD dwIndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfMap,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfMap) * nCount);
    //////////////////////////////////////////////////////////////////////////
    DWORD dwPreSize = pFile->GetPosition();
    for(int i = 0; i < nCount; i++)
    {
        
        stuMapItem* pItem = new stuMapItem;
        pItem->pBreadthContainer = m_pBreadthContainer;
        pItem->pIndex = new StuNetworkIDFileStructV7::StuIndexInfoOfMap;
        
        pItem->pIndex->nID = i;
        pItem->pIndex->nOffset = pFile->GetPosition() - dwPreSize;
        pItem->pIndex->nBreadthCount = pItem->pBreadthContainer->size();
 
        VERIFY(WriteMap(pFile,pItem));
        memcpy(&pIndexInfoOfMap[i],pItem->pIndex,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfMap));
    }

    //////////////////////////////////////////////////////////////////////////
    pFile->Seek(dwIndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfMap,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfMap) * nCount);
    //////////////////////////////////////////////////////////////////////////
    
    delete pIndexInfoOfMap;
    pIndexInfoOfMap = NULL;

    return TRUE;
}


BOOL CNetworkIDFileDataV7::WriteMap       (CFile* pFile,stuMapItem* pMapItem)
{
    ASSERT(pMapItem);
    ASSERT(pMapItem->pIndex);
    ASSERT(pMapItem->pBreadthContainer);
    
    StuNetworkIDFileStructV7 :: StuIndexInfoOfMap* pIndexInfoOfMap = pMapItem->pIndex;
        
    int nCount = pIndexInfoOfMap->nBreadthCount;
    StuNetworkIDFileStructV7::StuIndexInfoOfBreadth* pIndexInfoOfBreadth 
        = new StuNetworkIDFileStructV7::StuIndexInfoOfBreadth[nCount];
    
    //写入所有图幅的索引
    //////////////////////////////////////////////////////////////////////////
    DWORD dwIndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfBreadth)*nCount);
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

        pItem->pIndex = new StuNetworkIDFileStructV7::StuIndexInfoOfBreadth;
        pItem->pIndex->nID= nIndex;
        pItem->pIndex->nOffset = pFile->GetPosition() - dwPreSize;
        pItem->pIndex->nBeforeCompressSize = 0;
        pItem->pIndex->nAfterCompressSize = 0;
        
        WriteBreadth(pFile,pItem);
        
        memcpy(&pIndexInfoOfBreadth[nIndexN],pItem->pIndex,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfBreadth));
        nIndexN++;
    }

    
    //////////////////////////////////////////////////////////////////////////
    //真正写入所有图幅的索引的有效数据
    pFile->Seek(dwIndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfBreadth,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////

    delete pIndexInfoOfBreadth;
    pIndexInfoOfMap = NULL;
    return TRUE;
    
    return FALSE;
}

BOOL CNetworkIDFileDataV7::WriteBreadth   (CFile* pFile,stuBreadthItem* pBreadthItem)
{
    ASSERT(pFile);
    ASSERT(pBreadthItem->pIndex);
    ASSERT(pBreadthItem->pRoadSectionContainer);
    ASSERT(pBreadthItem->pVertexContainer);
  
    StuNetworkIDFileStructV7 :: StuIndexInfoOfBreadth* pIndexInfoOfBreadth = pBreadthItem->pIndex;
    MapOfRoadSection* pRoadSectionContainer = pBreadthItem->pRoadSectionContainer  ;  
    MapOfVertex* pVertexContainer = pBreadthItem->pVertexContainer;
    //////////////////////////////////////////////////////////////////////////
    
    StuNetworkIDFileStructV7::StuBreadthHeaderInfo aBreadthHeaderInfo; 
    aBreadthHeaderInfo.nCountOfLevelAllRoadSection = pRoadSectionContainer->size();
    aBreadthHeaderInfo.nCountOfLevelAllVertex = pVertexContainer->size();

	CMemFile aMemFile;
	{
		CFile* pFile = &aMemFile;

		//写入图幅头		
        DWORD dwHeadPos = pFile->GetPosition();
		pFile->Write(&aBreadthHeaderInfo,sizeof(StuNetworkIDFileStructV7::StuBreadthHeaderInfo));		
        //////////////////////////////////////////////////////////////////////////
		//写入所有路段
		{
            MapOfRoadSection::iterator iteratorOfFtr;
            for(iteratorOfFtr = pRoadSectionContainer->begin();
            iteratorOfFtr != pRoadSectionContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,unsigned long> aPair = *iteratorOfFtr;
                pFile->Write(&aPair.first,   sizeof(unsigned long));
                pFile->Write(&aPair.second,  sizeof(unsigned long));

            }
		}
		
		//写入所有节点
		{
            MapOfVertex::iterator iteratorOfFtr;
            for(iteratorOfFtr = pVertexContainer->begin();
            iteratorOfFtr != pVertexContainer->end(); iteratorOfFtr++)
            {
                pair < unsigned long,unsigned long> aPair = *iteratorOfFtr;
                pFile->Write(&aPair.first,   sizeof(unsigned long));
                pFile->Write(&aPair.second,  sizeof(unsigned long));
            }
        }
       
        {
            //写入映射关系数组.
        }
        //////////////////////////////////////////////////////////////////////////
        //写入图幅头的有效数据
        pFile->Seek(dwHeadPos,CFile::begin);
		pFile->Write(&aBreadthHeaderInfo,sizeof(StuNetworkIDFileStructV7::StuBreadthHeaderInfo));

	}

	//压缩数据
	unsigned long nSrcSize = aMemFile.GetLength();
	unsigned char* pSrcData = aMemFile.Detach();
	unsigned char* pDestBuf =(unsigned char*) s_DestBuf2;//new unsigned char[0x1000000];


//	unsigned long nDestSize = compressBound(nSrcSize);
//    ASSERT(nDestSize < 1000000);
//	compress2(pDestBuf,&nDestSize,pSrcData,nSrcSize,Z_DEFAULT_COMPRESSION);
    //--------------------------------
    unsigned long nDestSize = nSrcSize;
    memcpy(pDestBuf,pSrcData,nSrcSize);


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


BOOL CNetworkIDFileDataV7::WriteNetworkV2(CFile* pFile)
{
	//---------------------------
	//构造头,写头	
    StuNetworkIDFileStructV7::StuHeaderInfo aHeadInfo;
    memset(&aHeadInfo,0,sizeof(aHeadInfo));
    strcpy(aHeadInfo.chSign,"RUT_TEST");
    aHeadInfo.nMapCount = 1;    //(只有简图)
    pFile->Write(&aHeadInfo,sizeof(aHeadInfo));
    
    //---------------------------
    //写入地图索引
    int nCount = aHeadInfo.nMapCount;
    StuNetworkIDFileStructV7::StuIndexInfoOfMap* pIndexInfoOfMap = 
        new StuNetworkIDFileStructV7::StuIndexInfoOfMap[nCount];

    //////////////////////////////////////////////////////////////////////////
    DWORD dwIndexPos = pFile->GetPosition();
    pFile->Write(pIndexInfoOfMap,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfMap) * nCount);
    //////////////////////////////////////////////////////////////////////////
    DWORD dwPreSize = pFile->GetPosition();
    for(int i = 0; i < nCount; i++)
    {
        
        stuMapItem* pItem = new stuMapItem;
        pItem->pBreadthContainer = m_pBreadthContainer;
        pItem->pIndex = new StuNetworkIDFileStructV7::StuIndexInfoOfMap;
        
        pItem->pIndex->nID = i;
        pItem->pIndex->nOffset = pFile->GetPosition() - dwPreSize;
        MapOfBreadth::reverse_iterator iteratorLast = pItem->pBreadthContainer->rbegin();
        pair <unsigned long ,stuBreadthItem*> aPair = *iteratorLast;
        pItem->pIndex->nBreadthCount = aPair.first+1;
 
        VERIFY(WriteMapV2(pFile,pItem));
        memcpy(&pIndexInfoOfMap[i],pItem->pIndex,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfMap));
    }

    //////////////////////////////////////////////////////////////////////////
    pFile->Seek(dwIndexPos,CFile::begin);
    pFile->Write(pIndexInfoOfMap,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfMap) * nCount);
    //////////////////////////////////////////////////////////////////////////
    
    delete pIndexInfoOfMap;
    pIndexInfoOfMap = NULL;

    return TRUE;
}


BOOL CNetworkIDFileDataV7::WriteMapV2(CFile* pFile,stuMapItem* pMapItem)
{
    ASSERT(pMapItem);
    ASSERT(pMapItem->pIndex);
    ASSERT(pMapItem->pBreadthContainer);
    
    StuNetworkIDFileStructV7 :: StuIndexInfoOfMap* pIndexInfoOfMap = pMapItem->pIndex;
        
    int nCount = pIndexInfoOfMap->nBreadthCount;
    StuNetworkIDFileStructV7::StuIndexInfoOfBreadth* pArrayIndexInfoOfBreadth 
        = new StuNetworkIDFileStructV7::StuIndexInfoOfBreadth[nCount];
    memset(pArrayIndexInfoOfBreadth,0,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfBreadth)*nCount);
    
    //写入所有图幅的索引
    //////////////////////////////////////////////////////////////////////////
    DWORD dwIndexPos = pFile->GetPosition();
    pFile->Write(pArrayIndexInfoOfBreadth,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfBreadth)*nCount);
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
        ASSERT(nIndex < nCount);

        pItem->pIndex = new StuNetworkIDFileStructV7::StuIndexInfoOfBreadth;
        pItem->pIndex->nID= nIndex;
        pItem->pIndex->nOffset = pFile->GetPosition() - dwPreSize;
        pItem->pIndex->nBeforeCompressSize = 0;
        pItem->pIndex->nAfterCompressSize = 0;
        
        WriteBreadth(pFile,pItem);
        memcpy(&pArrayIndexInfoOfBreadth[nIndex],pItem->pIndex,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfBreadth));
        
        nIndexN++;
    }

    
    //////////////////////////////////////////////////////////////////////////
    //真正写入所有图幅的索引的有效数据
    pFile->Seek(dwIndexPos,CFile::begin);
    pFile->Write(pArrayIndexInfoOfBreadth,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfBreadth)*nCount);
    //////////////////////////////////////////////////////////////////////////

    delete [] pArrayIndexInfoOfBreadth;
    pIndexInfoOfMap = NULL;
    
    return TRUE;
  
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL CNetworkIDFileDataV7::ReadNetwork(CFile* pFile)
{
	ASSERT(pFile);

	//读头
    StuNetworkIDFileStructV7 :: StuHeaderInfo* m_pFileHeadInfo = new StuNetworkIDFileStructV7 :: StuHeaderInfo;
	pFile->Read(m_pFileHeadInfo,sizeof(StuNetworkIDFileStructV7 :: StuHeaderInfo));

	int nCount = m_pFileHeadInfo->nMapCount;
	//---------------------------
	//读入地图索引
	StuNetworkIDFileStructV7::StuIndexInfoOfMap* pIndexInfoArray = NULL;		//地图索引信息
	pIndexInfoArray = new StuNetworkIDFileStructV7::StuIndexInfoOfMap[nCount];
	memset(pIndexInfoArray,0,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfMap)*nCount);
	pFile->Read(pIndexInfoArray,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfMap) * nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////

    ASSERT(nCount == 1);
    for(int i = 0; i < nCount; i++)
    {        
        stuMapItem* pItem = new stuMapItem;
        pItem->pIndex               = new StuNetworkIDFileStructV7::StuIndexInfoOfMap;
        ASSERT(m_pBreadthContainer);
        pItem->pBreadthContainer = m_pBreadthContainer;

        memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfMap));
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

BOOL CNetworkIDFileDataV7::ReadMap(CFile* pFile,stuMapItem* pMapItem)
{

    StuNetworkIDFileStructV7::StuIndexInfoOfMap* pIndexInfoOfMap = pMapItem->pIndex;
	MapOfBreadth*& pBreadthContainer = pMapItem->pBreadthContainer;

	ASSERT(pFile);
	ASSERT(pIndexInfoOfMap);
	ASSERT(pBreadthContainer);

	//////////////////////////////////////////////////////////////////////////
    //读索引数据   
    int nCount = pIndexInfoOfMap->nBreadthCount;
    int nMapID = pIndexInfoOfMap->nID;
	StuNetworkIDFileStructV7::StuIndexInfoOfBreadth* pIndexInfoArray = 
        new StuNetworkIDFileStructV7::StuIndexInfoOfBreadth[nCount];
	pFile->Read(pIndexInfoArray,sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfBreadth) * nCount);

    //////////////////////////////////////////////////////////////////////////
    DWORD dwOffsetBase = pFile->GetPosition();
    //////////////////////////////////////////////////////////////////////////
    //读入地图数据
    for(int i = 0; i < nCount; i++)
	{	
#ifdef ANALYSE_DATA
        m_DataAnalyse.m_nBreadthCount++;
#endif
	    StuNetworkIDFileStructV7::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = &pIndexInfoArray[i];

	    TRACE2("BreadthID[%d]'s ID = %d\n",i,pIndexInfoOfBreadth->nID);
        if(pIndexInfoOfBreadth->nID)
        {
            ASSERT(i == pIndexInfoArray[i].nID);  
        }
		//偏移为0表示此图幅是一个空的图幅
		if(pIndexInfoArray[i].nBeforeCompressSize == 0)
        {
            ASSERT(pIndexInfoArray[i].nAfterCompressSize == 0);
			continue;
        }
			 
        stuBreadthItem* pItem = new stuBreadthItem;
        pItem->pIndex = new StuNetworkIDFileStructV7::StuIndexInfoOfBreadth;
        pItem->pHeader = new StuNetworkIDFileStructV7::StuBreadthHeaderInfo;
        pItem->pRoadSectionContainer = new MapOfRoadSection;
        pItem->pVertexContainer = new MapOfVertex;
        pItem->nID = i;

		memcpy(pItem->pIndex,&pIndexInfoArray[i],sizeof(StuNetworkIDFileStructV7::StuIndexInfoOfBreadth));


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


BOOL CNetworkIDFileDataV7::ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem,int nLoadFlag)
{

    StuNetworkIDFileStructV7::StuIndexInfoOfBreadth* pIndexInfoOfBreadth = pBreadthItem->pIndex;
	MapOfRoadSection* pRoadSectionMap = pBreadthItem->pRoadSectionContainer;
    MapOfVertex*  pVertexMap = pBreadthItem->pVertexContainer;
    

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
//			int nRet = uncompress( (unsigned char*)pBufOfBeforeCompress, &dwSize, (unsigned char*)pBufOfAfterCompress, nAfterCompressSize  );
//			ASSERT(nRet == Z_OK);
            memcpy(pBufOfBeforeCompress,pBufOfAfterCompress,dwSize);
			delete [] pBufOfAfterCompress;
			//ASSERT(nBeforeCompressSize == dwSize);
		}
		//-----------------------------------------------------

		//-----------------------------------------------------
		//处理图幅头数据
		StuNetworkIDFileStructV7::StuBreadthHeaderInfo* pBreadthHeader = (StuNetworkIDFileStructV7::StuBreadthHeaderInfo*)pBufOfBeforeCompress;
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



        unsigned long nOffsetOfRoadSection      = sizeof(StuNetworkIDFileStructV7::StuBreadthHeaderInfo);
        unsigned long nOffsetOfVertex           = nOffsetOfRoadSection + sizeof(StuNetworkIDFileStructV7::stuRoadSection)*nCountOfRoadSection;
		//-----------------------------------------------------


		
		
		//-----------------------------------------------------
		{			
			
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
			
			StuNetworkIDFileStructV7::stuRoadSection* pArrayOfStRoadSection = 
                (StuNetworkIDFileStructV7::stuRoadSection*)(pBufOfBeforeCompress+nOffsetOfRoadSection);
			for( int i = 0;	i < nReadNum;	i++ )
			{
#ifdef ANALYSE_DATA
                m_DataAnalyse.m_nRoadSectionCount++;
#endif
         
                unsigned long nDetailID = pArrayOfStRoadSection[i].nDetailID;
                unsigned long nMergeID = pArrayOfStRoadSection[i].nMergeID;
				(*pRoadSectionMap)[nDetailID] = nMergeID;
			}
		}
		//--------------------------------------------------------------
		{
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
			
			
			StuNetworkIDFileStructV7::stuVertex* pArrayOfStVertex = (StuNetworkIDFileStructV7::stuVertex*)(pBufOfBeforeCompress+nOffsetOfVertex);
			for(int i = 0;	i < nReadNum;	i++ )
			{
#ifdef ANALYSE_DATA
                m_DataAnalyse.m_nVertexCount++;
#endif				
                unsigned long nDetailID = pArrayOfStVertex[i].nDetailID;
                unsigned long nMergeID = pArrayOfStVertex[i].nMergeID;
                (*pVertexMap)[nDetailID] = nMergeID;

			}

		}
		delete[] pBufOfBeforeCompress;
		pBufOfBeforeCompress = NULL;
	}
	return TRUE;
}
