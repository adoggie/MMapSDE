// NetworkFileDataV1.cpp: implementation of the CNetworkFileDataV1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TempMapInfo.h"
#include "TempGeoMap.h"
#include "TempMapLayer.h"
#include "TempLayerManager.h"
#include "TempLayerInfo.h"
#include "GIS_BreadthMap.h"

#include <math.h>
#include "./Zlib/zlib.h"
#include "./FileDataStruct/V1/GIS_NetworkRut.h"
#include "./FileDataStruct/V1/GIS_NetworkVtx.h"

#include "NetworkFileDataV1.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static int MAP_ORIGIN_BID = 444624;
static int MAP_ORIGIN_XNUM = 372;
static int MAP_ORIGIN_YNUM = 354;
static float MAP_ORIGIN_CENTERX = 106.563f;
static float MAP_ORIGIN_CENTERY = 29.5417f;


//////////////////////////////////////////////////////////////////////////
#define StuNetworkFileStruct StuNetworkFileStructV1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkFileDataV1::CNetworkFileDataV1()
{
	m_pMapContainer = NULL;
    m_pArrayOfBreadthMap = NULL;
    m_nBreadthCount = 0;




#ifdef ANALYSE_DATA
    m_DataAnalyse.Initial();
#endif
}

CNetworkFileDataV1::~CNetworkFileDataV1()
{
	ClearOperation();
}

//////////////////////////////////////////////////////////////////////////
BOOL CNetworkFileDataV1::Load(CStringArray& arFilePath)
{
    ASSERT(arFilePath.GetSize() == 2);
    CString strFilePath = arFilePath[0];
    CString strIn2FilePath = arFilePath[1];

    TRACE1("CNetworkFileDataV1 load(%s)\n",strFilePath);
    ClearOperation();
#ifdef  ANALYSE_DATA
    m_DataAnalyse.Initial();
#endif
	//����in2�ļ�������ͼ����ͳ���ͼ����.
	LoadIn2File(strIn2FilePath);

    
    CFile aFile(strFilePath,CFile::modeRead);
	CFile* pFile = &aFile;
	ReadNetwork(pFile);
	DUMPSTATE(this);

    Check();
	
    return FALSE;
}

BOOL CNetworkFileDataV1::ReadNetwork(CFile* pFile)
{
	ASSERT(pFile);

	if(m_pMapContainer)
	{
		ClearOperation();
	}
	m_pMapContainer = new MapOfGeoMap;

	//��ͷ
	pFile->SeekToBegin();
	char szInfo[32];
	pFile->Read( szInfo, 9 );
	//ȡ�ô��ͼ��ƫ��
	int nCount = 0;
	pFile->Read( &nCount, sizeof(int) );
	//---------------------------
	
	DWORD dwOffsetBase = pFile->GetPosition();
    for(int i = 0; i < nCount; i++)
    {
		MapOfBreadth* pBreadthContainer = new MapOfBreadth;

		StuNetworkFileStruct::StuIndexInfoOfMap* pMapIndex = 
			new StuNetworkFileStruct::StuIndexInfoOfMap;
		pFile->Seek(dwOffsetBase + 8*i,CFile::begin);
		pFile->Read( &pMapIndex->nID, sizeof(int) );
		pFile->Read( &pMapIndex->nOffset, sizeof(int) );
		

        CTempGeoMap* pMap = (CTempGeoMap*)m_ListOfMap.GetAt(m_ListOfMap.FindIndex(i));
        ASSERT(pMap);
        pMapIndex->nBreadthCount		= pMap->GetBreadthMapNum();

        if(pMapIndex->nOffset == 0)
            continue;


		pFile->Seek(pMapIndex->nOffset,CFile::begin);  //��ͼ��ƫ��������ļ���ʼλ�õ�.���Բ���ͷ�Ĵ�С.    
		ReadMap(pFile,pMapIndex,pBreadthContainer);

		stuMapItem* pItem = new stuMapItem;
		pItem->pBreadthContainer = pBreadthContainer;
		pItem->pIndex = pMapIndex;
		(*m_pMapContainer)[i] = pItem;
    }
	return TRUE;
}

BOOL CNetworkFileDataV1::ReadMap(
		CFile* pFile,
		StuNetworkFileStruct::StuIndexInfoOfMap* pIndexInfoOfMap,
		MapOfBreadth*& pBreadthContainer)
{	
	ASSERT(pFile);
	ASSERT(pIndexInfoOfMap);
	ASSERT(pBreadthContainer);

	//////////////////////////////////////////////////////////////////////////
	
	DWORD dwMapOffsetBase = pFile->GetPosition();
    //����������   
    int nCount = pIndexInfoOfMap->nBreadthCount;
    unsigned long nMapID = pIndexInfoOfMap->nID;
	StuNetworkFileStruct::StuIndexInfoOfBreadth* pArIndexOfBreadth = 
        new StuNetworkFileStruct::StuIndexInfoOfBreadth[nCount];
	pFile->Seek(dwMapOffsetBase + 13,CFile::begin);
	pFile->Read(pArIndexOfBreadth,sizeof(StuNetworkFileStruct::StuIndexInfoOfBreadth) * nCount);
    
	int i;
    for(i = 0; i < nCount; i++)
	{	
		if(!(i%10000))
		{
			static DWORD dwtickstart = GetTickCount();
			TRACE2("----ReadBreadthV1(%d):%d ms--------\n",i,GetTickCount() - dwtickstart);
			dwtickstart = GetTickCount();
		}
	
		//ƫ��Ϊ0��ʾ��ͼ����һ���յ�ͼ��
		if(pArIndexOfBreadth[i].nBreadthMapOffset == 0)
			continue;
			

		MapOfRoadSection* pRoadSectionMap = new MapOfRoadSection;
		ASSERT(pRoadSectionMap);
		MapOfVertex* pVertexMap = new MapOfVertex;
		ASSERT(pVertexMap);
		StuNetworkFileStruct::StuIndexInfoOfBreadth* pBreadthIndex = new StuNetworkFileStruct::StuIndexInfoOfBreadth;
		ASSERT(pBreadthIndex);
		memcpy(pBreadthIndex,&pArIndexOfBreadth[i],sizeof(StuNetworkFileStruct::StuIndexInfoOfBreadth));
		StuNetworkFileStruct::StuBreadthHeaderInfo* pBreadthHeader = new StuNetworkFileStruct::StuBreadthHeaderInfo;

		DWORD dwBreadthMapOffset = dwMapOffsetBase+pBreadthIndex->nBreadthMapOffset ;
		pFile->Seek(dwBreadthMapOffset,CFile::begin);
		//BOOL bReadSuccess = ReadBreadthHeadInfo(pFile,pBreadthIndex,pBreadthHeader,1);
		BOOL bReadSuccess = ReadBreadth(pFile,pBreadthIndex,pRoadSectionMap,pVertexMap,0x07);
        {
            {
                MapOfRoadSection* pFtrContainer = pRoadSectionMap;
                
                MapOfRoadSection::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair <unsigned long ,CGIS_NetworkRut* > aPair = *iteratorOfFtr;
                    CGIS_NetworkRut* pFtr = aPair.second;
                    ASSERT(pFtr);
                }
            }
            {
                MapOfVertex* pFtrContainer = pVertexMap;
                
                MapOfVertex::iterator iteratorOfFtr;
                for(iteratorOfFtr = pFtrContainer->begin();
                iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
                {
                    pair <unsigned long ,CGIS_NetworkVtx* > aPair = *iteratorOfFtr;
                    CGIS_NetworkVtx* pFtr = aPair.second;
                    ASSERT(pFtr);
                    if(pFtr->GetVtxEdge())
                    {
                        //ASSERT(pFtr->GetVAbutBID());
                        if(!pFtr->GetVAbutBID())
                        {
                            TRACE2("ep:%.6f,%.6f\n",pFtr->GetVtxPosition().dx,pFtr->GetVtxPosition().dy);
                            TRACE1("EP:link num = %d\n",pFtr->GetVRutNum());
                        }
                    }
                    
#ifdef ANALYSE_DATA
                    m_DataAnalyse.m_nVertexCount++;

                    if(pFtr->GetVtxEdge())
                    {
                        m_DataAnalyse.m_nSideVertexCount++;
                    }
                    else
                    {

//                        int nActualRutNum = 0;
//                        //�������е�·�ε���ʼ�ڵ����ֹ�ڵ�
//                        CGIS_NetworkVtx* pVertex = pFtr;
//                        int nRoadSectionNum = pVertex->GetVRutNum();
//                        unsigned short* pListOfRoadSection = pVertex->GetVRutList();
//                        for (int j = 0; j < nRoadSectionNum; j++)
//                        {
//                            int nRoadSectionID = pListOfRoadSection[j];
//
//                            CGIS_NetworkRut* pRut = (*pRoadSectionMap)[nRoadSectionID];
//                            if(pRut)
//                            {
//                                nActualRutNum++;
//                            }
//                        }
//
//                        if(nActualRutNum < 10)
//                        {                           
//                            m_DataAnalyse.m_nVertexWithSpecialEdgeCount[nActualRutNum]++;
//                        }
//                        else
//                        {
//                            m_DataAnalyse.m_nVertexWithSpecialEdgeCount[10]++;
//                        }

                        if(pFtr->GetVRutNum() < 10)
                        {                           
                            m_DataAnalyse.m_nVertexWithSpecialEdgeCount[pFtr->GetVRutNum()]++;
                        }
                        else
                        {
                            m_DataAnalyse.m_nVertexWithSpecialEdgeCount[10]++;
                        }
                    }
#endif
                }
            }
 
        }
		if(bReadSuccess)
		{
			stuBreadthItem* pItem = new stuBreadthItem;
			pItem->pRoadSectionContainer = pRoadSectionMap;
			pItem->pVertexContainer = pVertexMap;
			pItem->pIndex = pBreadthIndex;	
			pItem->pHeader = pBreadthHeader;
            pItem->nMapID = nMapID;
			(*pBreadthContainer)[i] = pItem;
		}

	}

    delete pArIndexOfBreadth;
    pArIndexOfBreadth = NULL;

    return FALSE;
}


BOOL CNetworkFileDataV1::ReadBreadth(
	CFile* pFile,
	StuNetworkFileStruct::StuIndexInfoOfBreadth* pIndexInfoOfBreadth,
	MapOfRoadSection*& pRoadSectionMap,
	MapOfVertex*& pVertexMap,
	BYTE nLoadFlag)
{
	ASSERT(pFile);
	ASSERT(pIndexInfoOfBreadth);
	
	
	DWORD dwCSize = pIndexInfoOfBreadth->nBreadthMapSize;
	char* pFBuf = NULL;
	char* pBufAfterCompress = NULL;
	TRY
	{
		unsigned long nUnCom = 5;
		pFile->Read( &nUnCom, sizeof(unsigned long) );
		unsigned long nCom = dwCSize - sizeof( unsigned long );
		ASSERT( nUnCom < nCom*8 );
		pFBuf = new char[nUnCom];
		
		{
			pBufAfterCompress = new char[nCom];
			pFile->Read( pBufAfterCompress, nCom );
			DWORD dwSize = nUnCom;
			int nRtn = uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBufAfterCompress, nCom );
			ASSERT(Z_OK == nRtn );
			ASSERT(dwSize == nUnCom);
			delete [] pBufAfterCompress;
			pBufAfterCompress = NULL;
		}
		
		
		
		DWORD dwStation = 0;
		DWORD dwSize = nUnCom;
		{
			///�ж��ļ��İ汾��Ϣ
			char szInfo [32];
			Fread( pFBuf, dwSize, dwStation, szInfo, 3, 1 );
			if( CString(szInfo).IsEmpty() )
			{
				AfxThrowUserException();
			}
			if( CString(szInfo).Find( L"RUT" ) < 0 )
			{
				AfxThrowUserException();
			}
			int nVer = 0;
			Fread( pFBuf, dwSize, dwStation, &nVer, sizeof(int), 1 );
			if( nVer != 910 )
			{
				AfxThrowUserException();
			}
		}
		
		{
			////��ȡ��������
			int nRawBreadthBID = 0;
			Fread( pFBuf, dwSize, dwStation, &nRawBreadthBID, sizeof(int), 1 );
            int nBreadthID = GetBreadthMapByID(nRawBreadthBID)->m_nBreadthID;
			unsigned short	nMaxRutID = 0;
			unsigned short	nMaxVtxID = 0;
			int				nRutNum = 0;
			int				nVtxNum = 0;
			Fread( pFBuf, dwSize, dwStation, &nMaxRutID, sizeof(unsigned short), 1 );
			Fread( pFBuf, dwSize, dwStation, &nMaxVtxID, sizeof(unsigned short), 1 );
			Fread( pFBuf, dwSize, dwStation, &nRutNum, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nVtxNum, sizeof(int), 1 );
			
			
			int nCountOfLevel1RoadSection = 0;//��һ�ȼ�·����·�κͽڵ���
			int nCountOfLevel1Vertex = 0;
			int nCountOfLevel2RoadSection = 0;	//�ڶ��ȼ�·����·�κͽڵ���
			int nCountOfLevel2Vertex = 0;
			int nOffsetOfVertexData = 0;
			Fread( pFBuf, dwSize, dwStation, &nCountOfLevel1RoadSection,    sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nCountOfLevel1Vertex,			sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nCountOfLevel2RoadSection,    sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nCountOfLevel2Vertex,         sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nOffsetOfVertexData,          sizeof(int), 1 );
			
			//////////////////////////////////////////////////////////////////////////	
			
			
			///��ȡ·����Ϣ
			unsigned short  nRutID = 0;	//·��ID
			BYTE	        byDirection = 0;	//·�ε�˫�б�־ m,
			BYTE	        byType = 0;			//·����𣨹���..��
			BYTE	        byCharge = 0;		//�շ�:0--���շ�1--˫���շ�2--�����շ�3--�����շ�
			BYTE	        bySpeed = 0;		//�ٶȼ���:���ݵ�·�����ȱʡ�ٶ�
			BYTE	        nLaneNum = 0;		//·�γ�����
			BYTE	        nRutLevel = 0;		//·�εȼ�
			BYTE	        byRutType = 0;		//·�����ͣ��ѵ�..��
			float	        fRutLength = 0.0;	//·�γ���
			int		        nVtxStartID = 0;
			int		        nVtxEndID = 0;
			BYTE	        byLimit = 0;		//�Ƿ���ڽ�ͨ����:0-û�н�ͨ����;��0- �н�ͨ����
			unsigned short  pLRList[11];	//��ֹͨ�еĵ�·id
			BYTE	        byRLimit = 0;		//�Ƿ���ڽ�ͨ����:0-û�н�ͨ����;��0- �н�ͨ����
			unsigned short  pRLRList[11];	//��ֹͨ�еĵ�·id
			BYTE	        byVirtual = 0;		//�Ƿ��������ͼ:0- û������ͼ;��0- ������ͼ
			unsigned short  pVORList[11];	//��ȥ�ĵ�·ID����
			int		        pVIDs[8];			//����ͼ�������
			
			
			int nReadNum = 0;
            if(nLoadFlag&0x01)
            {
               nReadNum += nCountOfLevel1RoadSection;
            }
            if(nLoadFlag&0x02)
            {
               nReadNum += nCountOfLevel2RoadSection;
            }
            if(nLoadFlag&0x04)
            {
               nReadNum += (nRutNum - nCountOfLevel1RoadSection - nCountOfLevel2RoadSection);
            }

			ASSERT(nReadNum < 20000);
			int  i;
			for(i=0;	i < nReadNum;	i++ )
			{
				Fread( pFBuf, dwSize, dwStation, &nRutID,       sizeof(unsigned short), 1 );
				Fread( pFBuf, dwSize, dwStation, &byDirection,  sizeof(BYTE), 1 );
				Fread( pFBuf, dwSize, dwStation, &byType,       sizeof(BYTE), 1 );
				Fread( pFBuf, dwSize, dwStation, &byCharge,     sizeof(BYTE), 1 );
				Fread( pFBuf, dwSize, dwStation, &nLaneNum,     sizeof(BYTE), 1 );
				Fread( pFBuf, dwSize, dwStation, &bySpeed,      sizeof(BYTE), 1 );
				Fread( pFBuf, dwSize, dwStation, &nRutLevel,    sizeof(BYTE), 1 );
				Fread( pFBuf, dwSize, dwStation, &byRutType,    sizeof(BYTE), 1 );
				Fread( pFBuf, dwSize, dwStation, &fRutLength,   sizeof(float), 1 );
				Fread( pFBuf, dwSize, dwStation, &nVtxStartID,  sizeof(unsigned short), 1 );
				Fread( pFBuf, dwSize, dwStation, &nVtxEndID,    sizeof(unsigned short), 1 );
				
				ASSERT( (nVtxStartID < nVtxNum+100) && (nVtxEndID < nVtxNum+100) );
				Fread( pFBuf, dwSize, dwStation, &byLimit, sizeof(BYTE), 1 );
				if( byLimit > 10 )
				{
					delete[] pFBuf;
					return FALSE;
				}
				if( byLimit > 0 )
				{
					Fread( pFBuf, dwSize, dwStation, pLRList, sizeof(unsigned short)*byLimit, 1 );
				}
				Fread( pFBuf, dwSize, dwStation, &byRLimit, sizeof(BYTE), 1 );
				if( byRLimit > 10 )
				{
					AfxThrowUserException();
				}
				if( byRLimit > 0 )
				{
					Fread( pFBuf, dwSize, dwStation, pRLRList, sizeof(unsigned short)*byRLimit, 1 );
				}
				Fread( pFBuf, dwSize, dwStation, &byVirtual, sizeof(BYTE), 1 );
				if( byVirtual > 10 )
				{
					AfxThrowUserException();
				}
				if( byVirtual > 0 )
				{
					Fread( pFBuf, dwSize, dwStation, pVORList, sizeof(unsigned short)*byVirtual, 1 );
					Fread( pFBuf, dwSize, dwStation, pVIDs, sizeof(int)*byVirtual, 1 );
				}
				
				CGIS_NetworkRut *pNRut = new CGIS_NetworkRut;
				pNRut->SetRutInfo( 
					nRutID,
					byDirection,
					byType, 
					byCharge,
					nLaneNum,
					bySpeed, 
					nRutLevel,
					byRutType, 
					fRutLength,
					nVtxStartID, 
					nVtxEndID,
					nBreadthID );
				
				pNRut->SetRutLimitInfo(     byLimit,    pLRList );
				pNRut->SetRutRLimitInfo(    byRLimit,   pRLRList );
				pNRut->SetRutVirtualInfo(   byVirtual,  pVORList, pVIDs);
				
				(*pRoadSectionMap)[nRutID] = pNRut;
			}
			
			//��ȡ�ڵ���Ϣ
			unsigned short	    nVtxID = 0;		//�ڵ�ID
			ST_GEO_PTXY		    ptPosition;		//�ڵ�����
			BYTE			    byEdge = 0;		//�Ƿ���ͼ����Ե:0-����,1-��
			int				    nABID = 0;		//�ڽ�ͼ����
			unsigned short	    nARID = 0;		//����Rut_id
			BYTE			    byRNum = 0;		//�����ӵ�·����
			unsigned short	    pVRList[11];	//�����ӵ�·��ID
			unsigned short	    pRTList[11];	//��ˮƽ����ļн�����
			BYTE			    pRLList[11];	//·������ͼ��ID
			int		            nVBID = 0;		//�ڵ�����ͼ��ID
			unsigned short nPos = 0;
			nReadNum = 0;
            if(nLoadFlag&0x01)
            {
               nReadNum += nCountOfLevel1Vertex;
            }
            if(nLoadFlag&0x02)
            {
               nReadNum += nCountOfLevel2Vertex;
            }
            if(nLoadFlag&0x04)
            {
               nReadNum += (nVtxNum - nCountOfLevel1Vertex - nCountOfLevel2Vertex);
            }

            

			ASSERT(nReadNum <= 20000);
			
			dwStation = nOffsetOfVertexData;
			for( i=0;	i<nReadNum;	i++ )
			{
				Fread( pFBuf, dwSize, dwStation, &nVtxID, sizeof(unsigned short), 1 );
				ASSERT( nVtxID <= nReadNum );
				Fread( pFBuf, dwSize, dwStation, &ptPosition, sizeof(ST_GEO_PTXY), 1 );
				Fread( pFBuf, dwSize, dwStation, &byEdge, sizeof(BYTE), 1 );
				if( byEdge == 1 )
				{
					Fread( pFBuf, dwSize, dwStation, &nABID, sizeof(int), 1 );
					Fread( pFBuf, dwSize, dwStation, &nARID, sizeof(unsigned short), 1 );
				}
				Fread( pFBuf, dwSize, dwStation, &byRNum, sizeof(BYTE), 1 );
				if( byRNum > 10 || byRNum == 0 )
				{
					AfxThrowUserException();
				}
				ASSERT( byRNum > 0 );
				Fread( pFBuf, dwSize, dwStation, pVRList, sizeof(unsigned short)*byRNum, 1 );
				Fread( pFBuf, dwSize, dwStation, pRTList, sizeof(unsigned short)*byRNum, 1 );
				Fread( pFBuf, dwSize, dwStation, pRLList, sizeof(BYTE)*byRNum, 1 );
				CGIS_NetworkVtx *pNVtx = new CGIS_NetworkVtx;
				pNVtx->SetVtxInfo( nVtxID, ptPosition, nBreadthID );
				pNVtx->SetVEInfo( byEdge, nABID, nARID );
				pNVtx->SetVRutInfo( byRNum, pVRList, pRTList, pRLList );
				
                if(pNVtx->GetVtxEdge())
                {
                    if(pNVtx->GetVAbutBID() == 0)
                    {
                        TRACE0("��Ե�ڵ���Ϣ�쳣,�������޸�!\n");
                        pNVtx->SetVEInfo();
                    }
                }
				(*pVertexMap)[pNVtx->GetVtxID()] = pNVtx;
			}		
		}	
    }
	CATCH(CMemoryException,e)
	{
		if(pBufAfterCompress)
		{
			delete [] pBufAfterCompress;
			pBufAfterCompress = NULL;
		}
		
		if(pFBuf)
		{
			delete [] pFBuf;
			pFBuf = NULL;
		}
		return FALSE;
	}
	AND_CATCH(CFileException,e)
	{
		if(pBufAfterCompress)
		{
			delete [] pBufAfterCompress;
			pBufAfterCompress = NULL;
		}
		
		if(pFBuf)
		{
			delete [] pFBuf;
			pFBuf = NULL;
		}
		return FALSE;
	}
	AND_CATCH(CUserException,e)
	{
		if(pBufAfterCompress)
		{
			delete [] pBufAfterCompress;
			pBufAfterCompress = NULL;
		}
		
		if(pFBuf)
		{
			delete [] pFBuf;
			pFBuf = NULL;
		}
		return FALSE;
	}
	END_CATCH		   
	delete[] pFBuf;




//////////////////////////////////////////////////////////////////////////
	//check code(temp)
	
#ifdef _ANALYSE_DATA
	DWORD dwTickStart = GetTickCount();	

	int nRoadSectionTotalCount = m_nRutNum[nBreadthID];
	int nJunctionTotalCount = m_nVtxNum[nBreadthID];

	if(nRoadSectionTotalCount == 1 && nJunctionTotalCount == 0)
	{
		m_DataFileAnalyse.m_nEmptyBreadthCount++;
	}
	else
	{
	//TRACE2("\t·����:%d �ڵ���:%d\n",nRoadSectionTotalCount,nJunctionTotalCount);
	{
		for(int i = 0; i < nRoadSectionTotalCount; i++)
		{
			CGIS_NetworkRut* pRoadSection = m_pRutList[nBreadthID][i];
	
			ASSERT(pRoadSection);
			ASSERT(pRoadSection->GetRutID() == i);
			if(!pRoadSection)
			{
				TRACE1("IdΪ%d��·��Ϊ��.\n ",i);
			}
			else
			{	
				int nStartVtxID = pRoadSection->GetRutStartVtxID();
				int nEndVtxID = pRoadSection->GetRutEndVtxID();
				if(nStartVtxID >= nJunctionTotalCount)
				{
					TRACE2("IdΪ%d��·�εĿ�ʼ�ڵ�id�Ƿ�[%d].\n ",i,nStartVtxID);
				}
				else
				{
					CGIS_NetworkVtx* pStartVtx = m_pVtxList[nBreadthID][nStartVtxID];
					ASSERT(pStartVtx->GetVtxID() == nStartVtxID);
					
				}
				if(nStartVtxID >= nJunctionTotalCount)
				{
					TRACE2("IdΪ%d��·�εĽ����ڵ�id�Ƿ�[%d].\n ",i,nEndVtxID);
				}
				else
				{
					CGIS_NetworkVtx* pEndVtx = m_pVtxList[nBreadthID][nEndVtxID];
					ASSERT(pEndVtx->GetVtxID() == nEndVtxID);
					
				}
			}

		}
	}
	{
		for(int i = 0; i < nJunctionTotalCount; i++)
		{
			CGIS_NetworkVtx* pVertex = m_pVtxList[nBreadthID][i];
			ASSERT(pVertex);
			ASSERT(pVertex->GetVtxID() == i);
			if(!pVertex)
			{
				TRACE1("IdΪ%d�Ľڵ�Ϊ��.\n ",i);
			}
 			else
			{

				//�������е�·�ε���ʼ�ڵ����ֹ�ڵ�
				int nRoadSectionNum = pVertex->GetVRutNum();
				unsigned short* pListOfRoadSection = pVertex->GetRoadSectionList();
				for (int j = 0; j < nRoadSectionNum; j++)
				{
					int nRoadSectionID = pListOfRoadSection[j];
					if(nRoadSectionID >= nRoadSectionTotalCount)
					{
						TRACE3("IdΪ%d�Ľڵ� ��%d��·��[idΪ%d]�ķǷ�.\n",i,j,nRoadSectionID);
					}
					else
					{
						CGIS_NetworkRut* pRoadSection = m_pRutList[nBreadthID][nRoadSectionID];
						if(pRoadSection->GetRutStartVtxID() != i && pRoadSection->GetRutEndVtxID() != i)
						{
							TRACE2("IdΪ%d�Ľڵ��·��[idΪ%d]����β�ڵ���Ҳ����˽ڵ�.\n",i,nRoadSectionID);
						}
					}
				}

			}
		}
			
	}
}

	//TRACE1("����Ĵ����ִ��ʱ��Ϊ %d ms.\n",GetTickCount() - dwTickStart);
#endif
//////////////////////////////////////////////////////////////////////////
	return TRUE;
}

BOOL CNetworkFileDataV1::ReadBreadthHeadInfo(
		CFile* pFile,
		StuNetworkFileStruct::StuIndexInfoOfBreadth* pIndexInfoOfBreadth,
		StuNetworkFileStruct::StuBreadthHeaderInfo* pBreadthHeader,
		int nLoadFlag)
{
	ASSERT(pFile);
	ASSERT(pIndexInfoOfBreadth);
	ASSERT(pBreadthHeader);
	
	int nRoadSectionTotalCount = 0;
	int nJunctionTotalCount = 0;
	
	
	DWORD dwCSize = pIndexInfoOfBreadth->nBreadthMapSize;
	char* pFBuf = NULL;
	char* pBufAfterCompress = NULL;
	TRY
	{
		
		unsigned long nUnCom = 5;
		pFile->Read( &nUnCom, sizeof(unsigned long) );
		unsigned long nCom = dwCSize - sizeof( unsigned long );
		ASSERT( nUnCom < nCom*8 );
		pFBuf = new char[nUnCom];
		
		{
			pBufAfterCompress = new char[nCom];
			pFile->Read( pBufAfterCompress, nCom );
			DWORD dwSize = nUnCom;
			int nRtn = uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBufAfterCompress, nCom );
			ASSERT(Z_OK == nRtn );

            int nWrite = 0;
            
            if(nWrite)
            {
                static int nErrCount = 0;
                CString strDataFileName;
                strDataFileName.Format(_T("f:\\PCerrUncompressData%.3d.dat"),nErrCount);
                CFile aFile(strDataFileName,CFile::modeCreate|CFile::modeReadWrite);
                aFile.Write(pBufAfterCompress,nCom);
                nErrCount++;
            }


            ASSERT(dwSize == nUnCom);
			delete [] pBufAfterCompress;
			pBufAfterCompress = NULL;

#ifdef ANALYSE_DATA
            if(nCom > m_DataAnalyse.m_nMaxAfterCompressSize)
            {
                m_DataAnalyse.m_nMaxAfterCompressSize = nCom;
            }
            if(nUnCom > m_DataAnalyse.m_nMaxBeforeCompressSize)
            {
                m_DataAnalyse.m_nMaxBeforeCompressSize = nUnCom;
            }
#endif
		}
		
		
		DWORD dwStation = 0;
		DWORD dwSize = nUnCom;
		{
			///�ж��ļ��İ汾��Ϣ
			char szInfo [32];
			Fread( pFBuf, dwSize, dwStation, szInfo, 3, 1 );
			if( CString(szInfo).IsEmpty() )
			{
				AfxThrowUserException();
			}
			if( CString(szInfo).Find( L"RUT" ) < 0 )
			{
				AfxThrowUserException();
			}
			int nVer = 0;
			Fread( pFBuf, dwSize, dwStation, &nVer, sizeof(int), 1 );
			if( nVer != 910 )
			{
				AfxThrowUserException();
			}
		}
		
		{
			////��ȡ��������
			int nRawBreadthID = 0;
			Fread( pFBuf, dwSize, dwStation, &nRawBreadthID, sizeof(int), 1 );
			unsigned short	nMaxRutID = 0;
			unsigned short	nMaxVtxID = 0;
			int				nRutNum = 0;
			int				nVtxNum = 0;
			Fread( pFBuf, dwSize, dwStation, &nMaxRutID, sizeof(unsigned short), 1 );
			Fread( pFBuf, dwSize, dwStation, &nMaxVtxID, sizeof(unsigned short), 1 );
			Fread( pFBuf, dwSize, dwStation, &nRutNum, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nVtxNum, sizeof(int), 1 );
			
			
			int nCountOfLevel1RoadSection = 0;//��һ�ȼ�·����·�κͽڵ���
			int nCountOfLevel1Vertex = 0;
			int nCountOfLevel2RoadSection = 0;	//�ڶ��ȼ�·����·�κͽڵ���
			int nCountOfLevel2Vertex = 0;
			int nOffsetOfVertexData = 0;
			Fread( pFBuf, dwSize, dwStation, &nCountOfLevel1RoadSection,    sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nCountOfLevel1Vertex,			sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nCountOfLevel2RoadSection,    sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nCountOfLevel2Vertex,         sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nOffsetOfVertexData,          sizeof(int), 1 );
			
			//////////////////////////////////////////////////////////////////////////	
			pBreadthHeader->nCountOfLevel1RoadSection = nCountOfLevel1RoadSection;
			pBreadthHeader->nCountOfLevel2RoadSection = nCountOfLevel2RoadSection;
			pBreadthHeader->nCountOfLevel3RoadSection = nRutNum - nCountOfLevel1RoadSection - nCountOfLevel2RoadSection;

			pBreadthHeader->nVertexOffset = nOffsetOfVertexData;
			pBreadthHeader->nCountOfLevel1Vertex = nCountOfLevel1Vertex;
			pBreadthHeader->nCountOfLevel2Vertex = nCountOfLevel2Vertex;
			pBreadthHeader->nCountOfLevel3Vertex = nVtxNum - nCountOfLevel1Vertex - nCountOfLevel2Vertex;							
		}	
    }
	CATCH(CMemoryException,e)
	{
		if(pBufAfterCompress)
		{
			delete [] pBufAfterCompress;
			pBufAfterCompress = NULL;
		}
		
		if(pFBuf)
		{
			delete [] pFBuf;
			pFBuf = NULL;
		}
		return FALSE;
	}
	AND_CATCH(CFileException,e)
	{
		if(pBufAfterCompress)
		{
			delete [] pBufAfterCompress;
			pBufAfterCompress = NULL;
		}
		
		if(pFBuf)
		{
			delete [] pFBuf;
			pFBuf = NULL;
		}
		return FALSE;
	}
	AND_CATCH(CUserException,e)
	{
		if(pBufAfterCompress)
		{
			delete [] pBufAfterCompress;
			pBufAfterCompress = NULL;
		}
		
		if(pFBuf)
		{
			delete [] pFBuf;
			pFBuf = NULL;
		}
		return FALSE;
	}
	END_CATCH
		   
	delete[] pFBuf;
	return TRUE;	
}
void CNetworkFileDataV1::ClearBreadth(
		MapOfRoadSection*& pRoadSectionMap,
		MapOfVertex*& pVertexMap)
{
	
	{
		MapOfRoadSection* pFtrContainer = pRoadSectionMap;	
		MapOfRoadSection::iterator iteratorOfFtr;
		for(iteratorOfFtr = pFtrContainer->begin();
		iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
		{
			pair <unsigned long ,CGIS_NetworkRut* > aPair = *iteratorOfFtr;
			CGIS_NetworkRut* pFtr = aPair.second;
			ASSERT(pFtr);
			delete pFtr; 
			pFtr = NULL;
		}
		pFtrContainer->clear();
	}	
	{
		MapOfVertex* pFtrContainer = pVertexMap;
		
		MapOfVertex::iterator iteratorOfFtr;
		for(iteratorOfFtr = pFtrContainer->begin();
		iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
		{
			pair <unsigned long ,CGIS_NetworkVtx* > aPair = *iteratorOfFtr;
			CGIS_NetworkVtx* pFtr = aPair.second;
			ASSERT(pFtr);
			delete pFtr; 
			pFtr = NULL;
		}
		pFtrContainer->clear();
	}
}


#ifdef _DEBUG
void CNetworkFileDataV1::DumpState()
{
    TRACE0("**********CNetworkFileDataV1 infomation********************\n");
	//    TRACE1("m_BreadthContainerOfRoadSection : %d.\n",m_BreadthContainerOfVertex.size());
    TRACE1("roadSection's size          : %d.\n",sizeof(CGIS_NetworkRut));
    TRACE1("roadSection     total count : %d.\n",GetRoadSectionCount(0));
    TRACE1("roadSection1    total count : %d.\n",GetRoadSectionCount(1));
    TRACE1("roadSection2    total count : %d.\n",GetRoadSectionCount(2));
    TRACE1("roadSection3    total count : %d.\n",GetRoadSectionCount(3));
	//    TRACE1("m_BreadthContainerOfVertex      : %d.\n",m_BreadthContainerOfVertex.size());
    TRACE1("vertex's size               : %d.\n",sizeof(CGIS_NetworkVtx));
    TRACE1("vertex		    total count : %d.\n",GetVertexCount(0));
    TRACE1("vertex1		    total count : %d.\n",GetVertexCount(1));
    TRACE1("vertex2         total count : %d.\n",GetVertexCount(2));
    TRACE1("vertex3         total count : %d.\n",GetVertexCount(3));

#ifdef ANALYSE_DATA
    m_DataAnalyse.DumpState();
#endif
	TRACE0("*********************************************************\n\n");
    
}
void CNetworkFileDataV1::AssertValid() const
{

}
#endif
  

BOOL CNetworkFileDataV1::InitialOperation()
{
    return FALSE;
}
BOOL CNetworkFileDataV1::ClearOperation()
{
	
	if(!m_pMapContainer)
		return TRUE;
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
            {
				MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;
					
				MapOfRoadSection::iterator iteratorOfFtr;
				for(iteratorOfFtr = pFtrContainer->begin();
				iteratorOfFtr != pFtrContainer->end(); iteratorOfFtr++)
				{
					pair <unsigned long ,CGIS_NetworkRut* > aPair = *iteratorOfFtr;
					CGIS_NetworkRut* pFtr = aPair.second;
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
					pair <unsigned long ,CGIS_NetworkVtx* > aPair = *iteratorOfFtr;
					CGIS_NetworkVtx* pFtr = aPair.second;
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
	m_pMapContainer->clear();
	delete m_pMapContainer;
	m_pMapContainer = NULL;

    //////////////////////////////////////////////////////////////////////////
   
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
unsigned long CNetworkFileDataV1::GetRoadSectionCount(int nLevel)
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
			switch(nLevel)
			{
			case 0:
				nCount += 
					pBreadthHeader->nCountOfLevel1RoadSection +
					pBreadthHeader->nCountOfLevel2RoadSection +
					pBreadthHeader->nCountOfLevel3RoadSection;
				break;
			case 1:
				nCount += pBreadthHeader->nCountOfLevel1RoadSection;
				break;
			case 2:
				nCount += pBreadthHeader->nCountOfLevel2RoadSection;
				break;
			case 3:
				nCount += pBreadthHeader->nCountOfLevel3RoadSection;
				break;
			default:
				ASSERT(FALSE);
				break;
			}
            {
				MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;				
			}
			{
				MapOfVertex* pFtrContainer = pItem->pVertexContainer;
			}

        }

    }
    //////////////////////////////////////////////////////////////////////////
    
    return nCount;
}

unsigned long CNetworkFileDataV1::GetVertexCount(int nLevel)
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
			switch(nLevel)
			{
			case 0:
				nCount += 
					pBreadthHeader->nCountOfLevel1Vertex +
					pBreadthHeader->nCountOfLevel2Vertex +
					pBreadthHeader->nCountOfLevel3Vertex;
				break;
			case 1:
				nCount += pBreadthHeader->nCountOfLevel1Vertex;
				break;
			case 2:
				nCount += pBreadthHeader->nCountOfLevel2Vertex;
				break;
			case 3:
				nCount += pBreadthHeader->nCountOfLevel3Vertex;
				break;
			default:
				ASSERT(FALSE);
				break;
			}

            {
				MapOfRoadSection* pFtrContainer = pItem->pRoadSectionContainer;
			}
			{
				MapOfVertex* pFtrContainer = pItem->pVertexContainer;
			}

        }

    }
    //////////////////////////////////////////////////////////////////////////
    
    return nCount;
}
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
 

#ifdef ANALYSE_DATA
void CNetworkFileDataV1::CDataAnalyse::Initial()
{
    m_nMaxBeforeCompressSize = 0;
    m_nMaxAfterCompressSize = 0;

    //����ͼ����
    m_nBreadthCount = 0;
    m_nEmptyBreadthCount = 0;
    
    //���ڽڵ��
    m_nVertexCount = 0;
    m_nSideVertexCount = 0;
    
    for(int i = 0; i < 11; i++)
        m_nVertexWithSpecialEdgeCount[i] = 0;
    
    m_nVertexCountOfHighLevel = 0;
    m_nVertexCountOfMidLevel = 0;
    m_nVertexCountOfLowLevel = 0;
    
    //����·�ε�
    m_nRoadSectionCount = 0;
    m_nRoadSectionCountOfHighLevel = 0;
    m_nRoadSectionCountOfMidLevel = 0;
    m_nRoadSectionCountOfLowLevel = 0;
}
void CNetworkFileDataV1::CDataAnalyse::DumpState()
{
	TRACE0("�����ļ�����-------------------------------------\n");
    TRACE1("MaxnBeforeCompressSize      :%d.\n",m_nMaxBeforeCompressSize);
    TRACE1("MaxnAfterCompressSize       :%d.\n",m_nMaxAfterCompressSize);
	TRACE1("ͼ��                        :%d.\n",m_nBreadthCount);
	TRACE1("��ͼ��                      :%d .\n",m_nEmptyBreadthCount);
	TRACE1("·��                        :%d .\n",m_nRoadSectionCount);
	TRACE1("�߼�·��                    :%d .\n",m_nRoadSectionCountOfHighLevel);
	TRACE1("�м�·��                    :%d .\n",m_nRoadSectionCountOfMidLevel);
	TRACE1("�ͼ�·��                    :%d .\n",m_nRoadSectionCountOfLowLevel);
	TRACE1("�ڵ�                        :%d .\n",m_nVertexCount);
	TRACE1("��Ե�ڵ�                    :%d .\n",m_nSideVertexCount);
	TRACE1("�߼��ڵ�                    :%d .\n",m_nVertexCountOfHighLevel);
	TRACE1("�м��ڵ�                    :%d .\n",m_nVertexCountOfMidLevel);
	TRACE1("�ͼ��ڵ�                    :%d .\n",m_nVertexCountOfLowLevel);
	TRACE1("0��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[0]);
	TRACE1("1��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[1]);
	TRACE1("2��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[2]);
	TRACE1("3��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[3]);
	TRACE1("4��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[4]);
	TRACE1("5��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[5]);
	TRACE1("6��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[6]);
	TRACE1("7��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[7]);
	TRACE1("8��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[8]);
	TRACE1("9��edge�Ľڵ�               :%d .\n",m_nVertexWithSpecialEdgeCount[9]);
	TRACE1("10��������edge�Ľڵ�        :%d .\n",m_nVertexWithSpecialEdgeCount[10]);

}

#endif


//////////////////////////////////////////////////////////////////////////
//����in2�ļ�
BOOL CNetworkFileDataV1::LoadIn2File( CString& strFName)
{
	
	HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwSize = GetFileSize( hFile, NULL );
	DWORD dwCount = 0;
	char *pFBuf = new char[dwSize];
	ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
	if( dwSize > dwCount )
		return FALSE;

	DWORD dwStation = 0;
	char szInfo[32];

    
		//���ܼ��ܹ���
		DWORD dwReturnBytes = 0;
		char MyUUID[16];
		Fread( pFBuf, dwSize, dwStation, szInfo, 32, 1 );
#ifdef _WIN32_WCE
#ifdef _ENCRYPT_CHECK_ENABLE
		BOOL bIsOK = KernelIoControl(IOCTL_HAL_GET_UUID, 0, 0, MyUUID, sizeof(UUID), &dwReturnBytes );
		if( !bIsOK )
			return 1;
		if( !EnCode( szInfo , MyUUID ,  hFile) )
		{
			if( !UnCode(szInfo , MyUUID) )
				return 1;
		}
#endif
#endif
        
        CTempGeoMap *pMap = new CTempGeoMap;
	int n0 = strFName.ReverseFind(L'\\');
	CString strPath = strFName.Left( n0+1 );
	pMap->GetMapInfo()->SetMapFilePath( strFName.Left( n0+1 ) );

	
	//////////////////////////////////////////////////////////////////////////
	//�������еĲ���Ϣ
	//
	int nNum = 0;
	int nLNum = 0;
	//��ͼ�����(ȫ��ͼΪ6,��ͼΪ25)
	Fread( pFBuf, dwSize, dwStation, &nLNum, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapLayNum(nLNum);
	BYTE byNum = 0;
	float dbNum = 0.0;
	CTempMapLayer *pLList[25];
	
	//ѭ���������в����Ϣ
	CTempLayerManager *pLManager = new CTempLayerManager();//���ͼ�������
	int i;
	for( i=0;	i< nLNum;	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		CTempLayerInfo *pLInfo = new CTempLayerInfo( byNum );
		ST_GEO_PEN pen;
		ST_GEO_BRUSH brush;
		Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pen.nStyle = nNum;
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		brush.nStyle = nNum;
		//pLInfo->SetPen( pen );
		//pLInfo->SetBrush( brush );
		///ͼ�����ʾ
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		pLInfo->SetLayerState( byNum );
		//ͼ���scale��Χ
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		dbNum = 0.1;
		//�׶���5 ֱϽ�У�4 ʡ�᣺3 �ؼ��У�2
		if( i == 2 )
			dbNum = 70;
		if( i == 4 || i == 3 || i == 5 )
			dbNum = 20;
		pLInfo->SetMinGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		dbNum = 10000000.1;
		pLInfo->SetMaxGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		dbNum = 0.1;
		//�׶���5 ֱϽ�У�4 ʡ�᣺3 �ؼ��У�2
		if( i == 2 )
			dbNum = 70;
		if( i == 4 || i == 3 || i == 5 )
			dbNum = 20;
		pLInfo->SetMinARScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		pLInfo->SetMaxARScale(dbNum);
		///
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pLInfo->SetIconID(nNum);
		
		CTempMapLayer *pLayer = new CTempMapLayer( pLInfo );
		pLList[i] = pLayer;
		pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
	}
	//////////////////////////////////////////////////////////////////////////
	
	
	//////////////////////////////////////////////////////////////////////////
	//����ͼ����һ����֤
	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
	if( nNum )
		szInfo[nNum] = '\0';
	//pMap->GetMapInfo()->SetMapName( szInfo );
	//if( CString(szInfo).Find(L"NAV_2005_SChina") >= 0 )
	//	pMap->GetMapInfo()->SetMapType( EN_MAPTYPE_NATION );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//����ͼid(ȫ��ͼΪ0)
	int nMID = 0;
	Fread( pFBuf, dwSize, dwStation, &nMID, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapID(nMID);

	//���߽��(ȫ��ͼû��)
	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapBndPtNum( nNum );
	if( nNum != 0 )
	{
		ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
		Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
		pMap->GetMapInfo()->SetMapBndList( pBndPtList );
	}
	//////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////////////
	int nBMID = 0;
	int nBMapID = 0;
	ST_GEO_RECT rtBreadth;
	Fread( pFBuf, dwSize, dwStation, &nBMID, sizeof(int), 1 );

	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );
	pMap->GetMapInfo()->SetMapRect( rtBreadth );
	//����ȫ��ͼͼ����(ֻ��28��,0-27)
	int nBMNum = 0;
	Fread( pFBuf, dwSize, dwStation, &nBMNum, sizeof(int), 1 );

	int nBMapMaxID = 0;
	CGIS_BreadthMap *pBMapList = NULL;

	pBMapList = new CGIS_BreadthMap[nBMNum];
	memset( pBMapList, 0, sizeof(CGIS_BreadthMap)*nBMNum );
	ST_GEO_RECT pRTList[128];
	for( i=0;	i<nBMNum;	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &pBMapList[i].m_nBMID, sizeof(int), 1 );

		Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &pRTList[i].dTop, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &pRTList[i].dBottom, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &pRTList[i].dLeft, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &pRTList[i].dRight, sizeof(float), 1 );

		pBMapList[i].m_nBreadthID = nBMapID;

		if( pBMapList[i].m_nBreadthID > nBMapMaxID )
			nBMapMaxID = pBMapList[i].m_nBreadthID;
	}
	pMap->SetBreadthMapNum( nBMNum );
	pMap->SetBreadthMapList( pBMapList );
	pMap->SetBreadthMapRects( pRTList, nBMNum );

	//��ͼ��������б�������ͼ��,��ÿ��ͼ���ʼ��
//	{
//		POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
//		short    nKey;
//		CTempMapLayer* pLay = NULL;
//		while( pos != NULL )
//		{
//			pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
//			if( pLay )
//			{
//				((CTempMapLayer*)pLay)->InitLBCount( nBMapMaxID );
//			}
//		}
// 	}
	//pMap->m_LayerManagerSet->AddTail( pLManager );
	pMap->m_pLayerMng = pLManager;

	//pMap->Open( "" );
	//pMapList->AddTail( pMap );

	m_ListOfMap.AddTail(pMap);

	pBMapList = NULL;
	//������ȫ��ͼ
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//�����ǳ���ͼ
	CTempLayerInfo LIList[25];
	Fread( pFBuf, dwSize, dwStation, &nLNum, sizeof(int), 1 );
	for( i=0;	i<nLNum;	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		LIList[i].SetLayerID( byNum );
		ST_GEO_PEN pen;
		ST_GEO_BRUSH brush;
		Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pen.nStyle = nNum;
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		brush.nStyle = nNum;
//		LIList[i].SetPen( pen );
//		LIList[i].SetBrush( brush );
		///ͼ�����ʾ
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		LIList[i].SetLayerState( byNum );
		//ͼ���scale��Χ
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		LIList[i].SetMinGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		LIList[i].SetMaxGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		LIList[i].SetMinARScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );

		LIList[i].SetMaxARScale(dbNum);
		///
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		if( i>10 )
			LIList[i].SetIconID(nNum);
		else
			LIList[i].SetIconID(0);
	}
	int nMNum = 0;
	Fread( pFBuf, dwSize, dwStation, &nMNum, sizeof(int), 1 );
	for( i=0;	i<nMNum;	i++ )
	{
		pMap = new CTempGeoMap;
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
		if( nNum )
			szInfo[nNum] = '\0';
		//pMap->GetMapInfo()->SetMapName( szInfo );
		//pMap->GetMapInfo()->SetMapFilePath( strFName.Left( n0+1 ) );

		Fread( pFBuf, dwSize, dwStation, &nMID, sizeof(int), 1 );
		pMap->GetMapInfo()->SetMapID(nMID);
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pMap->GetMapInfo()->SetMapBndPtNum( nNum );
		if( nNum != 0 )
		{
			ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
			Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
			pMap->GetMapInfo()->SetMapBndList( pBndPtList );
		}
		pLManager = new CTempLayerManager();
		int j;
		for( j=0;	j<nLNum;	j++ )
		{
			CTempLayerInfo *pLInfo = new CTempLayerInfo();
			memcpy( pLInfo, &LIList[j], sizeof(CTempLayerInfo) );
			CTempMapLayer *pLayer = new CTempMapLayer( pLInfo );
			pLList[j] = pLayer;
			pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
		}

		Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );

		pMap->GetMapInfo()->SetMapRect( rtBreadth );


		//������ͼ��ͼ������
		Fread( pFBuf, dwSize, dwStation, &nBMNum, sizeof(int), 1 );

		//-------------------------------------------------------------
		//��������ͼ����,����ͼ���ŵ�X��,����ͼ���ŵ�Y��,����ͼ�������ĵ�
		Fread( pFBuf, dwSize, dwStation, &MAP_ORIGIN_BID, sizeof(int), 1 );
		int nn3 = 0;
		Fread( pFBuf, dwSize, dwStation, &nn3, sizeof(int), 1 );
		MAP_ORIGIN_XNUM = nn3;
		Fread( pFBuf, dwSize, dwStation, &nn3, sizeof(int), 1 );
		MAP_ORIGIN_YNUM = nn3;
		float dValue = 0.0;
		Fread( pFBuf, dwSize, dwStation, &dValue, sizeof(float), 1 );
		MAP_ORIGIN_CENTERY = dValue;
		Fread( pFBuf, dwSize, dwStation, &dValue, sizeof(float), 1 );
		MAP_ORIGIN_CENTERX = dValue;
		//-------------------------------------------------------------


		//////////////////////////////////////////////////////////////////////////
		//�����ͼ��������BreadthMap,
		int nBMapNum = 0;
		nBMapMaxID = 0;
		pBMapList = new CGIS_BreadthMap[nBMNum];
		memset( pBMapList, 0, sizeof(CGIS_BreadthMap)*nBMNum );
		for( j=0;	j<nBMNum;	j++ )
		{

			Fread( pFBuf, dwSize, dwStation, &pBMapList[nBMapNum].m_nBMID, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
			pBMapList[nBMapNum].m_nBreadthID = nBMapID;

			if( pBMapList[nBMapNum].m_nBreadthID > nBMapMaxID )
				nBMapMaxID = pBMapList[nBMapNum].m_nBreadthID;
			nBMapNum++;
			/////init breadthmap layer object list;
		}
		//-----------------------------------------------------------------------

		
		pMap->SetBreadthMapNum( nBMapNum );
		pMap->SetBreadthMapList( pBMapList );

        m_nBreadthCount = nBMapNum;
        m_pArrayOfBreadthMap = pBMapList;
//        {
            TRACE1("nBMapNum = %d\n",nBMapNum);
//            CFile aFile(_T("e:\\CGIS_BreadthMap.dat"),CFile::modeCreate|CFile::modeReadWrite);
//            aFile.Write(pBMapList,sizeof(CGIS_BreadthMap)*nBMapNum);
//         }


//		{
//			POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
//			short    nKey;
//			CTempMapLayer* pLay = NULL;
//			
//			while( pos != NULL )
//			{
//				pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
//				if( pLay )
//				{
//					((CTempMapLayer*)pLay)->InitLBCount( nBMapMaxID );
//				}
//			}
//		}

//		pMap->m_LayerManagerSet->AddTail( pLManager );
		pMap->m_pLayerMng = pLManager;

//		pMap->Open( "" );
//		pMapList->AddTail( pMap );
		m_ListOfMap.AddTail(pMap);

		pBMapList = NULL;
	}

	delete[] pFBuf;
	CloseHandle( hFile );
	return TRUE;
}

BOOL CNetworkFileDataV1::Check()
{
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
                    pair <unsigned long ,CGIS_NetworkVtx* > aPair = *iteratorOfFtr;
                    CGIS_NetworkVtx* pFtr = (CGIS_NetworkVtx*)aPair.second;

                    if(pFtr->GetVtxEdge())
                    {
                        CheckEdgeVertex(pFtr);
                    }
                }
                
			}

        }

    }
    
    return TRUE;
}

//���߽�ڵ�
BOOL CNetworkFileDataV1::CheckEdgeVertex(CGIS_NetworkVtx* pVertex)
{
	ASSERT(pVertex);
	ASSERT(pVertex->GetVtxEdge());
		
	//ȡ������·��
    CGIS_BreadthMap* pBMap = GetBreadthMapByID(pVertex->GetVAbutBID());
    ASSERT(pBMap);

	int nBreadthID = pBMap->m_nBreadthID;

	int nRutID = pVertex->GetVAbutRID();

    stuMapItem* pMapItem = (*m_pMapContainer)[1];
    MapOfBreadth* pBreadthContainer = pMapItem->pBreadthContainer;
    stuBreadthItem* pBreadthItem = (*pBreadthContainer)[nBreadthID];
    MapOfRoadSection*   pRoadSectionContainer   = pBreadthItem->pRoadSectionContainer;
    MapOfVertex*        pVertexContainer        = pBreadthItem->pVertexContainer;
    //////////////////////////////////////////////////////////////////////////
    
	CGIS_NetworkRut* pRoadSection = (*pRoadSectionContainer)[nRutID];
	
	ASSERT(pRoadSection);
	ASSERT(pRoadSection->GetRutID() == nRutID );
	ASSERT(pRoadSection->GetRBreadthID() == nBreadthID);
	

	//////////////////////////////////////////////////////////////////////////	
	CGIS_NetworkVtx*   pSVtx = (*pVertexContainer)[pRoadSection->GetRutStartVtxID()];
	CGIS_NetworkVtx*   pEVtx = (*pVertexContainer)[pRoadSection->GetRutEndVtxID() ];
	ASSERT(pSVtx);
	ASSERT(pEVtx);
		
	//ͨ�������ж�ȷʵ��ԭʼ�ڵ��Ӱ�ӽڵ�
	if( pSVtx->GetVtxEdge() == 1 || pEVtx->GetVtxEdge() == 1 )
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
    {
        ASSERT( FALSE );
    }
		
	return TRUE;
}

CGIS_BreadthMap* CNetworkFileDataV1::GetBreadthMapByID( int nRawBreadthID )
{
	CGIS_BreadthMap *pBNet = NULL;
	// ��λ����
	int nStart = 0;
	int nEnd = m_nBreadthCount;
	int nMid = 0;
	while( nEnd > nStart )
	{
		nMid = (nStart+nEnd)/2;
		if( m_pArrayOfBreadthMap[nMid].m_nBMID == nRawBreadthID )
			return &m_pArrayOfBreadthMap[nMid];
		else if( m_pArrayOfBreadthMap[nMid].m_nBMID > nRawBreadthID )
			nEnd = nMid;
		else
			nStart = nMid;
		if( nEnd-nStart <= 1 )
        {
            if ( m_pArrayOfBreadthMap[nStart].m_nBMID == nRawBreadthID )
                return &m_pArrayOfBreadthMap[nStart];
            else if ( m_pArrayOfBreadthMap[nEnd].m_nBMID == nRawBreadthID )
                return &m_pArrayOfBreadthMap[nEnd];
            return NULL;
            

        }
	}
	return NULL;
}