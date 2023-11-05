
#include "stdafx.h"
#include "GIS_LFBand.h"
#include "GIS_FeaturePoint.h"
#include "GIS_FeatureLine.h"
#include "GIS_FeaturePoly.h"
#include "GIS_FeatureRut.h"
#include "GIS_LayerManager.h"
#include "SDEControler.h"
#include "GIS_MapDataManager.h"
//////////////////////////////////////////////////////////////////////////
#include "./MemoryMngLib/IMemoryMng.h"
#include "./MemoryMngLib/IMemoryMng2.h"
//////////////////////////////////////////////////////////////////////////
#include "./ZLib/zlib.h"
//////////////////////////////////////////////////////////////////////
#include <memory>
using namespace std;

extern void PureSDDriverBuffer();
#define PURE_SDDRIVER_BUFFER  //PureSDDriverBuffer();


CGIS_LFBand::CGIS_LFBand()
{
	m_bNation = FALSE;
}
CGIS_LFBand::CGIS_LFBand( swString bstrFName )
	: IGIS_LayerFile( bstrFName )
{
	m_bNation = FALSE;
}

CGIS_LFBand::~CGIS_LFBand()
{
}
/*
BOOL CGIS_LFBand::InitLFBand( swString bstrFName, short nBreadthID, CPtrList *pLMList )
{
	swString strFName;
	strFName.Format( L"%s%d.ban", bstrFName, nBreadthID );
	m_bstrLFName = strFName; 
	m_nBreadthID = nBreadthID;
	m_pLMList = pLMList;
	return TRUE;
}
*/
BOOL CGIS_LFBand::OpenLayer()
{
	return TRUE;
}
BOOL CGIS_LFBand::SaveToFile( swString bstrFName )
{
	return TRUE;
}
CPtrList* CGIS_LFBand::GetCurrentObjList( CPtrList *pLMList, UINT nLID, int nBreadthID, short &sIconID )
{
	IGIS_Layer* pLayer = NULL;
	POSITION pos = pLMList->GetHeadPosition();
	CGIS_LayerManager *pLManager = NULL;
	while( pos )
	{
		pLManager = (CGIS_LayerManager*)pLMList->GetNext(pos);
		if( pLManager )
		{
			pLManager->GetLayerListMap()->Lookup( nLID, pLayer );
			if( pLayer )
			{
				sIconID = pLayer->GetLayerInfo()->GetIconID();
				CPtrList *pOList = NULL;
				pLayer->GetObjectListMap()->Lookup( nBreadthID, pOList );
				if( pOList )
					return pOList;
			}
		}
	}
	return NULL;
}
/*
// 读取文件数据
size_t CGIS_LFBand::Fread(char *pFileBuf,DWORD dwSize,DWORD &dwStation,void *buffer, size_t size, size_t count)
{
	if(pFileBuf == NULL)
	{
		return 0;
	}

	int nNum = size * count;
	int nRtn;
	if(dwStation + nNum > dwSize)
	{
		nRtn = dwSize - dwStation;
	}
	else
	{
		nRtn = nNum;
	}
//	strncpy((char*)buffer, pFileBuf + dwStation, nRtn );
	memcpy(buffer, pFileBuf + dwStation, nRtn);
	dwStation += nRtn;

	return nRtn / size;
}*/

// ST_GEO_PTXY BMAP_ORIGN_POS;
static HANDLE gHLFile = INVALID_HANDLE_VALUE;
/*
BOOL CGIS_LFBand::LoadLayerdel( CGIS_GeoMap *pMap, CGIS_BreadthMap *pBMap, CGIS_MapLayer **pLList, int nLNum )
{
	if( gHLFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.img", pMap->GetMapInfo()->GetMapFilePath() );
		gHLFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHLFile)
			return FALSE;
	}
 	char *pFBuf = NULL;
	TRY
	{
		CFile file(gHLFile);
		file.SeekToBegin();
		char szInfo[32];
		file.Read( szInfo, 9 );
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		file.Read( &nMNum, sizeof(int) );
		int i;
		for( i=0;	i<nMNum;	i++ )
		{
			file.Read( &nMID, sizeof(int) );
			file.Read( &nMOff, sizeof(int) );
			if( nMID == pMap->GetMapInfo()->GetMapID() )
				break;
	//		if( nMID==1 && pMap->GetMapInfo()->GetMapID()==1 )
	//			break;
	//		if( nMID==2 && pMap->GetMapInfo()->GetMapID()== 336 )
	//			break;
	//		if( nMID==3 && pMap->GetMapInfo()->GetMapID()== 337 )
	//			break;
		}
		if( pMap->GetMapInfo()->GetMapID() == MAPID_MAP_NATION )
			m_bNation = TRUE;
		else
			m_bNation = FALSE;
		int nBID = 0;
		int nBOff = nMOff + 13 + (pBMap->m_nBreadthID)*8;
	//	int nBOff = nMOff + 13 + (pBMap->m_nBreadthID-1)*8;
		file.Seek( nBOff, CFile::begin );
		file.Read( &nBID, sizeof(int) );
		file.Read( &nBOff, sizeof(int) );
		file.Seek( nMOff+nBOff, CFile::begin );
		
		CGIS_MapLayer *pLayer = NULL;
		DWORD dwStation = 0;
	//	m_nBreadthID = nBID;
		m_nBreadthID = pBMap->m_nBreadthID;
//		ST_GEO_RECT rt = pMap->GetBreadthMapRect( pBMap );
////		if( pMap->GetMapInfo()->GetMapType() == EN_MAPTYPE_NATION )
////			rt = pBMap->m_rtBreadth;
////		else
//			rt = CalculateBMapRect( pBMap->m_nBMID );
//		BMAP_ORIGN_POS.dx = rt.dLeft;
//		BMAP_ORIGN_POS.dy = rt.dTop;
		for( i=0;	i<nLNum;	i++ )
		{
			pLayer = pLList[i];
	//		ReadBreadthLayer( file, nMOff+nBOff, nBID,  pLayer );
			ASSERT( pLayer );
	//		if( pLayer->GetBLCount(nBID) > 0 )
			pLayer->InitLBOList( pBMap->m_nBreadthID );
			if( pLayer->GetBLCount(pBMap->m_nBreadthID) > 0 )
			{
 				pLayer->IncreaseLCount( pBMap->m_nBreadthID );
				continue;
			}
 			pLayer->IncreaseLCount( pBMap->m_nBreadthID );
			dwStation = nMOff+nBOff + pLayer->GetLayerInfo()->GetLayerID()*16;
			int nLInfo[4];
			file.Seek( dwStation, CFile::begin );
			file.Read( nLInfo, sizeof(int)*4 );
			if( !( nLInfo[0] == pLayer->GetLayerInfo()->GetLayerID() ) )
				return FALSE;
			if( (nLInfo[2] == 0 || nLInfo[3] == 0) ||
					(nLInfo[2] == 8 && nLInfo[3] == 8 ) )
				continue;
 			pFBuf = new char[nLInfo[2]];
			if( pFBuf == 0 )
				return FALSE;
			char *pBuf = new char[nLInfo[3]];
			if( pBuf == 0 )
			{
				delete[] pFBuf;
				return FALSE;
			}
			dwStation = nMOff+nBOff + nLInfo[1];
			file.Seek( dwStation, CFile::begin );
			file.Read( pBuf, nLInfo[3] );
			DWORD dwSize = nLInfo[2];
			int nUnComRtn = uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nLInfo[3] );
#ifdef _DEBUG
    if(Z_OK != nUnComRtn)
    {
        TRACE1("uncompress failed when loadlayer! uncompress code = %d\n",nUnComRtn);
    }
			delete[] pBuf;

//#define Z_OK            0
//#define Z_STREAM_END    1
//#define Z_NEED_DICT     2
//#define Z_ERRNO        (-1)
//#define Z_STREAM_ERROR (-2)
//#define Z_DATA_ERROR   (-3)
//#define Z_MEM_ERROR    (-4)
//#define Z_BUF_ERROR    (-5)
//#define Z_VERSION_ERROR (-6)

#endif			
			dwStation = 0;
			BOOL bRutType = FALSE;
			short sIconID = pLayer->GetLayerInfo()->GetIconID();
			CPtrList *pOList = NULL;
			pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
			while( dwSize > dwStation )
			{
				int nLID = 0;
				Fread( pFBuf, dwSize, dwStation, &nLID, sizeof(int), 1 );
				if( nLID > 4 && nLID < 12 )
					bRutType = TRUE;
				else
					bRutType = FALSE;
                
				if( pOList == NULL )
					continue;
				int nCount;
				Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
				BYTE bType = 0;
				for( int i=0;	i< nCount;	i++ )
				{
					Fread( pFBuf, dwSize, dwStation, &bType, sizeof(BYTE), 1 );
					switch( bType )
					{
					case 1:
						ReadPointFeature( pFBuf, dwSize, dwStation, nBID, sIconID, pOList );
						break;
					case 2:
						{
							if( bRutType )
								ReadRutFeature( pFBuf, dwSize, dwStation, nBID, pOList );
							else
								ReadLineFeature( pFBuf, dwSize, dwStation, nBID, pOList );
						}
						break;
					case 3:
						ReadPolyFeature( pFBuf, dwSize, dwStation, nBID, pOList );
						break;
					default:
						{
							return FALSE;
						}
						break;
					}
				}
			}
			delete[] pFBuf;
			pFBuf = NULL;
		}
	}
	CATCH( CMemoryException, e )
	{
		if(pFBuf)
			delete[] pFBuf;
		pFBuf = NULL;
		return FALSE;
	}
	AND_CATCH(CFileException, e )
	{
 		if(pFBuf)
			delete[] pFBuf;
		pFBuf = NULL;
       TRACE0("catch the CFileException when load spacedata.\n");
		if(INVALID_HANDLE_VALUE != gHLFile)
			CloseHandle( gHLFile );
		gHLFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	END_CATCH

        
	return TRUE;
}
*/
static char g_pFBufV2[500000];
static char g_pBufV2[300000];
//加载车道层数据.
/*
BOOL CGIS_LFBand::LoadLaneLayerV2(
    CGIS_GeoMap *pMap,
    CGIS_BreadthMap *pBMap,
    char*& pLaneBlockAddr//存放车道信息块的指针数组
    )
{
    if( gHLFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.img", pMap->GetMapInfo()->GetMapFilePath() );
		gHLFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHLFile)
			return FALSE;
	}
	TRY
	{
		CFile file(gHLFile);
		file.SeekToBegin();
		char szInfo[32];
		file.Read( szInfo, 9 );
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		file.Read( &nMNum, sizeof(int) );
		for( int i=0;	i<nMNum;	i++ )
		{
			file.Read( &nMID, sizeof(int) );
			file.Read( &nMOff, sizeof(int) );
			if( nMID == pMap->GetMapInfo()->GetMapID() )
				break;
	//		if( nMID==1 && pMap->GetMapInfo()->GetMapID()==1 )
	//			break;
	//		if( nMID==2 && pMap->GetMapInfo()->GetMapID()== 336 )
	//			break;
	//		if( nMID==3 && pMap->GetMapInfo()->GetMapID()== 337 )
	//			break;
		}
		if( pMap->GetMapInfo()->GetMapID() == MAPID_MAP_NATION )
			m_bNation = TRUE;
		else
			m_bNation = FALSE;
		int nBID = 0;
		int nBOff = nMOff + 13 + (pBMap->m_nBreadthID)*8;
		file.Seek( nBOff, CFile::begin );
		file.Read( &nBID, sizeof(int) );
		file.Read( &nBOff, sizeof(int) );
		file.Seek( nMOff+nBOff, CFile::begin );
		
		CGIS_MapLayer *pLayer = NULL;
		DWORD dwStation = 0;
		m_nBreadthID = pBMap->m_nBreadthID;

		{
			dwStation = nMOff+nBOff + (27)*16;
			int nLInfo[4];
			file.Seek( dwStation, CFile::begin );
			file.Read( nLInfo, sizeof(int)*4 );
			if( !( nLInfo[0] == 27 ) )
				return FALSE;
			if( (nLInfo[2] == 0 || nLInfo[3] == 0) ||
					(nLInfo[2] == 8 && nLInfo[3] == 8 ) )
				return FALSE;

//            auto_ptr <char> pFBuf   (new char[nLInfo[2]]);
//            auto_ptr <char> pBuf    (new char[nLInfo[3]]);
			char* pFBuf = g_pFBufV2;
			char* pBuf = g_pBufV2;
    
            //////////////////////////////////////////////////////////////////////////
            
            DWORD dwSize = 0;
            DWORD dwDataPos = file.GetPosition();
            int nReadCountLimit = 2;//最多读两次
            dwStation = nMOff+nBOff + nLInfo[1];
            for(int nReadCount = 0; nReadCount < nReadCountLimit; nReadCount++)
            {
                file.Seek(dwDataPos,CFile::begin);    
                
                file.Seek( dwStation, CFile::begin );
                file.Read( pBuf, nLInfo[3] );
                
                dwSize = nLInfo[2];
                int nUnComRtn = uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nLInfo[3] );
                if(Z_OK == nUnComRtn)
                {
                    if(nReadCount != 0)
                    {
                        TRACE1("loadlayer reload[%d] sucess!\n",nReadCount);
                    }
                    break;
                }
                
#ifdef _DEBUG
                if(Z_OK != nUnComRtn)
                {
                    TRACE1("uncompress failed when loadlayer! uncompress code = %d\n",nUnComRtn);
                }
            
                 
#endif			
                
                if(nReadCountLimit-1 == nReadCount)
                {
                    return FALSE;
                }

                PURE_SDDRIVER_BUFFER;
                
            }

            pLaneBlockAddr = new char [nLInfo[2]];
            memcpy(pLaneBlockAddr,pFBuf,nLInfo[2]);

		}
	}
	CATCH( CMemoryException, e )
	{
		return FALSE;
	}
	AND_CATCH(CFileException, e )
	{
       TRACE0("catch the CFileException when load spacedata.\n");
		if(INVALID_HANDLE_VALUE != gHLFile)
			CloseHandle( gHLFile );
		gHLFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	END_CATCH

        
	return TRUE;
}
*/

static char g_pFBuf[500000];//解压缩之后的数据大小
static char g_pBuf[300000];//压缩数据
//根据地图区块返回层列表
BOOL CGIS_LFBand::LoadLayerV2( CGIS_GeoMap *pMap, CGIS_BreadthMap *pBMap, CGIS_MapLayer **pLList, int nLNum )
{
    IMemoryMng* pMemMngOfFeaturePoint = NULL;
    IMemoryMng* pMemMngOfFeatureLine = NULL;
    IMemoryMng* pMemMngOfFeaturePoly = NULL;
    IMemoryMng* pMemMngOfFeatureRut = NULL;
    IMemoryMng* pMemMngOfBuddyData = NULL;

    switch(m_nMemoryDest)
    {
    case 1://空间堆
        {
            pMemMngOfFeaturePoint   = CGIS_Feature::m_pMemMngOfFeaturePoint;
            pMemMngOfFeatureLine    = CGIS_Feature::m_pMemMngOfFeatureLine;
            pMemMngOfFeaturePoly    = CGIS_Feature::m_pMemMngOfFeaturePoly;
            pMemMngOfFeatureRut     = CGIS_Feature::m_pMemMngOfFeatureRut;
            pMemMngOfBuddyData      = CGIS_Feature::m_pMemMngOfBuddyData;
        }
        break;
    case 3://路网堆
        {
            pMemMngOfFeaturePoint = CGIS_Feature::m_pForNetworkMemMngOfFeaturePoint;
            pMemMngOfFeatureLine = CGIS_Feature::m_pForNetworkMemMngOfFeatureLine;
            pMemMngOfFeaturePoly = CGIS_Feature::m_pForNetworkMemMngOfFeaturePoly;
            pMemMngOfFeatureRut = CGIS_Feature::m_pForNetworkMemMngOfFeatureRut;
            pMemMngOfBuddyData = CGIS_Feature::m_pForNetworkMemMngOfBuddyData;
        }      
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    ASSERT(pMemMngOfFeaturePoint);
    ASSERT(pMemMngOfFeatureLine);
    ASSERT(pMemMngOfFeaturePoly);
    ASSERT(pMemMngOfFeatureRut);
    ASSERT(pMemMngOfBuddyData);

	static swFile file;
	if( !file.IsOpened()){
		if( !file.Open(swString::Format(wxT("%s/Automap.img"),pMap->GetMapInfo()->GetMapFilePath())) ){
			return FALSE;
		}
	}
	/*
		if( gHLFile == INVALID_HANDLE_VALUE ){
		swString strFName;
		strFName.Format( L"%sAutomap.img", pMap->GetMapInfo()->GetMapFilePath() );
		gHLFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHLFile)
			return FALSE;
	}*/
	TRY //每次都要读取一次地图头部数据进行图块的定位
	{
		//CFile file(gHLFile);
		//file.SeekToBegin();
		file.Seek(0);
		char szInfo[32];
		file.Read( szInfo, 9 );
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		file.Read( &nMNum, sizeof(int) );
		int i;
		for( i=0;	i<nMNum;	i++ ){
			file.Read( &nMID, sizeof(int) );
			file.Read( &nMOff, sizeof(int) );
			if( nMID == pMap->GetMapInfo()->GetMapID() ) //定位地图区域
				break;
		}
		if( pMap->GetMapInfo()->GetMapID() == MAPID_MAP_NATION )
			m_bNation = TRUE;
		else
			m_bNation = FALSE;
		int nBID = 0;
		int nBOff = nMOff + 13 + (pBMap->m_nBreadthID)*8; //8是描述一个图副信息,m_nBreadthID 以0开始的索引编号
		//file.Seek( nBOff, CFile::begin );
		file.Seek(nBOff);
		file.Read( &nBID, sizeof(int) );
		file.Read( &nBOff, sizeof(int) ); //图幅数据偏移(此偏移为地图数据内偏移,从地图头开始算)
		//file.Seek( nMOff+nBOff, CFile::begin );//定位到了图副的开始位置
		file.Seek( nMOff+nBOff );//定位到了图副的开始位置
		//已经定位breadth数据位置
		CGIS_MapLayer *pLayer = NULL;
		DWORD dwStation = 0;
	
		m_nBreadthID = pBMap->m_nBreadthID;
		for( i=0;	i<nLNum;	i++ ){
			pLayer = pLList[i];
			ASSERT( pLayer );
			pLayer->InitLBOList( pBMap->m_nBreadthID ); //初始化listmap结构
			if( pLayer->GetBLCount(pBMap->m_nBreadthID) > 0 ){
 				pLayer->IncreaseLCount( pBMap->m_nBreadthID );
				continue;
			}
 			pLayer->IncreaseLCount( pBMap->m_nBreadthID );
			dwStation = nMOff+nBOff + pLayer->GetLayerInfo()->GetLayerID()*16; //16是描述层的信息占用的字节数
			int nLInfo[4];
			//file.Seek( dwStation, CFile::begin );
			file.Seek( dwStation);
			file.Read( nLInfo, sizeof(int)*4 ); //读出层信息
			if( !( nLInfo[0] == pLayer->GetLayerInfo()->GetLayerID() ) ) //判别是否是请求的可见层编号
				return FALSE;
			if( (nLInfo[2] == 0 || nLInfo[3] == 0) ||
					(nLInfo[2] == 8 && nLInfo[3] == 8 ) )
				continue; //图层数据非法

			char* pFBuf = g_pFBuf;
			char* pBuf = g_pBuf;
    
     
            DWORD dwSize = 0;
            DWORD dwDataPos = file.GetPosition();
            int nReadCountLimit = 2;//最多读两次
            dwStation = nMOff+nBOff + nLInfo[1]; //层数据开始位置
            for(int nReadCount = 0; nReadCount < nReadCountLimit; nReadCount++){
                //file.Seek(dwDataPos,CFile::begin);                    
                //file.Seek( dwStation, CFile::begin );
								file.Seek( dwStation);
                file.Read( pBuf, nLInfo[3] ); //此时的数据是压缩之后的数据大小,输入过大会导致崩溃                
                dwSize = nLInfo[2];//原始数据大小
                int nUnComRtn = uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nLInfo[3] );
                if(Z_OK == nUnComRtn){ //解压缩数据
                    if(nReadCount != 0){
                        TRACE1("loadlayer reload[%d] sucess!\n",nReadCount);
                    }
                    break;
                }
                
#ifdef _DEBUG
                if(Z_OK != nUnComRtn)
                {
                    TRACE1("uncompress failed when loadlayer! uncompress code = %d\n",nUnComRtn);
                }
            
#endif			
                
                if(nReadCountLimit-1 == nReadCount)
                {
                    return FALSE;
                }

//                PURE_SDDRIVER_BUFFER;
                
            }
            

            		//比对原始数据大小和解压缩之后的大小
			dwStation = 0;
			BOOL bRutType = FALSE;
			short sIconID = pLayer->GetLayerInfo()->GetIconID();
			CPtrList *pOList = NULL;
			pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
			while( dwSize > dwStation )  //这里开始要读取layer内的数据了
			{
				int nLID = 0;
				Fread( pFBuf, dwSize, dwStation, &nLID, sizeof(int), 1 ); //id of layer
				if( nLID > 4 && nLID < 12 )  //道路层范围区间
					bRutType = TRUE;
				else
					bRutType = FALSE;
                
				if( pOList == NULL )
					continue;
				int nCount;
				Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 ); //feature 个数
				BYTE bType = 0;
				for( int i=0;	i< nCount;	i++ )
				{
					Fread( pFBuf, dwSize, dwStation, &bType, sizeof(BYTE), 1 );
					switch( bType )
					{
					case 1:
						ReadPointFeatureInSpecialSpace( pFBuf, dwSize, dwStation, nBID, sIconID, pOList ,pMemMngOfFeaturePoint,pMemMngOfBuddyData);
						break;
					case 2:
						{
							if( bRutType )
								ReadRutFeatureInSpecialSpace( pFBuf, dwSize, dwStation, nBID, pOList ,pMemMngOfFeatureRut,pMemMngOfBuddyData);
							else
								ReadLineFeatureInSpecialSpace( pFBuf, dwSize, dwStation, nBID, pOList ,pMemMngOfFeatureLine,pMemMngOfBuddyData );
						}
						break;
					case 3:
						{
							if( !m_bNation && nLID == 0 )
								ReadPolyFeatureInSpecialSpace( pFBuf, dwSize, dwStation, nBID, pOList ,pMemMngOfFeaturePoly,pMemMngOfBuddyData,TRUE);
							else
								ReadPolyFeatureInSpecialSpace( pFBuf, dwSize, dwStation, nBID, pOList ,pMemMngOfFeaturePoly,pMemMngOfBuddyData);
							break;
						}
					default:
						{
							return FALSE;
						}
						break;
					}
				}
			}
		}
	}
	CATCH( CMemoryException, e )
	{
		return FALSE;
	}
	AND_CATCH(CFileException, e )
	{
       TRACE0("catch the CFileException when load spacedata.\n");
		if(INVALID_HANDLE_VALUE != gHLFile)
			CloseHandle( gHLFile );
		gHLFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	END_CATCH

        
	return TRUE;
}


BOOL CGIS_LFBand::ReadBreadthLayer( CFile &file, int nOffset, int nBID, CGIS_MapLayer *pLayer )
{
	ASSERT( pLayer );
	if( pLayer->GetBLCount(nBID) > 0 )
		return FALSE;
	DWORD dwStation = nOffset + pLayer->GetLayerInfo()->GetLayerID()*16;
	int nLInfo[4];
	file.Seek( dwStation, CFile::begin );
	file.Read( nLInfo, sizeof(int)*4 );
	ASSERT( nLInfo[0] == pLayer->GetLayerInfo()->GetLayerID() );
	if( (nLInfo[2] == 0 || nLInfo[3] == 0) ||
			(nLInfo[2] == 8 && nLInfo[3] == 8 ) )
		return FALSE;
	char *pBuf = new char[nLInfo[3]];
	if( pBuf == 0 )
		ASSERT(FALSE);
	char *pFBuf = new char[nLInfo[2]];
	if( pFBuf == 0 )
		ASSERT(FALSE);
	dwStation = nOffset + nLInfo[1];
	file.Seek( dwStation, CFile::begin );
	file.Read( pBuf, nLInfo[3] );
	DWORD dwSize = nLInfo[2];
	uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nLInfo[3] );
	delete [] pBuf;
	
	dwStation = 0;
	BOOL bRutType = FALSE;
	short sIconID = pLayer->GetLayerInfo()->GetIconID();
	//CPtrList *pOList = NULL;
	//pLayer->GetObjectListMap()->Lookup( nBID, pOList );
	std::list<CGIS_Feature*> pOList& = pLayer->GetObjectListMap()->m_pObjListMap[nBID];
	while( dwSize > dwStation )
	{
		int nLID = 0;
		Fread( pFBuf, dwSize, dwStation, &nLID, sizeof(int), 1 );
		if( nLID > 6 && nLID < 13 )
			bRutType = TRUE;
		else
			bRutType = FALSE;

		//if( pOList == NULL )
		//	continue;
		int nCount;
		Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
		BYTE bType = 0;
		for( int i=0;	i< nCount;	i++ )
		{
			Fread( pFBuf, dwSize, dwStation, &bType, sizeof(BYTE), 1 );
			switch( bType )
			{
			case 1:
				ReadPointFeature( pFBuf, dwSize, dwStation, nBID, sIconID, pOList );
				break;
			case 2:
				{
					if( bRutType )
						ReadRutFeature( pFBuf, dwSize, dwStation, nBID, pOList );
					else
						ReadLineFeature( pFBuf, dwSize, dwStation, nBID, pOList );
				}
				break;
			case 3:
				ReadPolyFeature( pFBuf, dwSize, dwStation, nBID, pOList );
				break;
			default:
				{
				ASSERT( FALSE );
				}
				break;
			}
		}
	}
	delete[] pFBuf;
	return TRUE;
}

BOOL CGIS_LFBand::ReadPointFeature( char *pFBuf, const DWORD dwSize, DWORD &dwStation, 
																	 short nBreadthID, short sIconID, std::list<CGIS_Feature*>& pOList ){

	unsigned short nPOIType = 0;
	ST_GEO_PTXY *pPt = NULL;

	BYTE nAddrLen = 0;
	char szAddr[256];
	BYTE nPhoneLen = 0;
	char szPhone[256];
	int nAnno = 0;
	char szAnno[256];
    
	CGIS_FeaturePoint *pPoint = NULL;
	
	Fread( pFBuf, dwSize, dwStation, &nPOIType, sizeof(unsigned short), 1 );
	Fread( pFBuf, dwSize, dwStation, &nAddrLen, sizeof(BYTE), 1 );
	if( nAddrLen > dwSize-dwStation )
		return FALSE;
	if( nAddrLen > 0 ){
		Fread( pFBuf, dwSize, dwStation, szAddr, sizeof(char)*nAddrLen, 1 );
		szAddr[nAddrLen] = '\0';
		nAddrLen += 1;
	}
	Fread( pFBuf, dwSize, dwStation, &nPhoneLen, sizeof(BYTE), 1 );
	if( nPhoneLen > dwSize-dwStation ){
		return FALSE;
	}
	if( nPhoneLen > 0 ){
		Fread( pFBuf, dwSize, dwStation, szPhone, sizeof(char)*nPhoneLen, 1 );
		szPhone[nPhoneLen] = '\0';
		nPhoneLen += 1;
	}
	Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
	if( nAnno > dwSize-dwStation ){
		return FALSE;
	}
	if( nAnno > 0 ){
		Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
		szAnno[nAnno] = '\0';
		nAnno += 1;
	}
	ST_GEO_PTXY aPt;
	Fread( pFBuf, dwSize, dwStation, &aPt, sizeof(ST_GEO_PTXY), 1 );
	pPoint = new CGIS_FeaturePoint( EN_GOTYPE_POINT );        
	pPoint->SetPtInfo(1,&aPt);
	pPoint->SetIconID( sIconID );
	if( nAnno > 0 ){
		pPoint->SetAnnotationInfo(nAnno ,szAnno );
	}		
	if( nAddrLen > 0 ){
		pPoint->SetAddressInfo( nAddrLen,szAddr );
	}
	if( nAnno > 0  ){
		pPoint->SetPhoneInfo( nPhoneLen , szPhone );
	}
	pPoint->SetPointObjectType( nPOIType );
	pPoint->m_nBreadthID = m_nBreadthID;
	pOList.push_back(pPoint);
	return TRUE;
}

BOOL CGIS_LFBand::ReadLineFeature( char *pFBuf, const DWORD dwSize, 
																	DWORD &dwStation, short nBreadthID, std::list<CGIS_Feature*>& pOList )
{
	int nPtNum = 0, nPartNum = 0, nAnno = 0;

	ST_GEO_PTXY *pPt = NULL;
	int *pPart = NULL;
	char *szAnno = NULL;
	CGIS_FeatureLine *pLine = NULL;


	Fread( pFBuf, dwSize, dwStation, &nPtNum, sizeof(int), 1 );
	Fread( pFBuf, dwSize, dwStation, &nPartNum, sizeof(int), 1 );
	if( nPartNum > (dwSize-dwStation)/4 )
		return FALSE;
	if( nPartNum > 0 ){
		pPart = new int[nPartNum];
		if( nPartNum == 1 ){
			Fread( pFBuf, dwSize, dwStation, pPart, sizeof(int)*nPartNum, 1 );
			pPart[0] = nPtNum;
		}else{
			int *pTemp = new int[nPartNum];
			Fread( pFBuf, dwSize, dwStation, pTemp, sizeof(int)*nPartNum, 1 );
			int i;
			for( i=0;	i< nPartNum-1;	i++ )
				pPart[i] = pTemp[i+1]-pTemp[i];
			pPart[i] = nPtNum - pTemp[i];
			delete[] pTemp;
		}
	}

	Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
	if( nAnno > dwSize-dwStation ){
		if( pPart )
			delete[] pPart;
		return FALSE;
	}
	if( nAnno > 0 ){
		szAnno = new char[nAnno+1];
		Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
		szAnno[nAnno] = '\0';
		nAnno += 1;
	}
	if( nPtNum <= 0 ){
		if( szAnno )
			delete[] szAnno;
		if( nPartNum >= 1 )
			delete[] pPart;
		return FALSE;
	}
	if( nPtNum > (dwSize-dwStation)/4 ){
		if( pPart )
			delete[] pPart;
		if( szAnno )
			delete[] szAnno;
		return FALSE;
	}
	pPt = new ST_GEO_PTXY[nPtNum];
	Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nPtNum, 1 );

	if( nPtNum <= 1 ){
		if( szAnno )
			delete[] szAnno;
		if( pPart )
			delete[] pPart;
		if( pPt )
			delete[] pPt;
		return FALSE;
	}
	pLine = new CGIS_FeatureLine( EN_GOTYPE_LINE );
	pLine->SetAnnotationInfo(nAnno,szAnno);
	if( szAnno ){         
		delete [] szAnno;
		szAnno = NULL;
	}
	pLine->SetPartInfo(nPartNum,pPart);
	if(pPart){
		delete []pPart;
		pPart = NULL;
	}
	ST_GEO_RECT rtObj;
	CalcRect( pPt, nPtNum, rtObj ); //最大矩形框
	pLine->SetRectObj( rtObj );
	pLine->SetPtInfo(nPtNum,pPt);
	if(pPt){
		delete []pPt;
		pPt = NULL;
	}
	pLine->m_nBreadthID = m_nBreadthID;
	pOList.push_back( pLine );
	return TRUE;
}

BOOL CGIS_LFBand::ReadPolyFeature( char *pFBuf, const DWORD dwSize, 
																	DWORD &dwStation, short nBreadthID, std::list<CGIS_Feature*>& pOList)
{
	int nPtNum = 0, nPartNum = 0, nAnno = 0;
	ST_GEO_PTXY *pPt = NULL;
	int *pPart = NULL;
	char *szAnno = NULL;
	CGIS_FeaturePoly *pPoly = NULL;
	TRY
	{
		Fread( pFBuf, dwSize, dwStation, &nPtNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nPartNum, sizeof(int), 1 );

		if( nPartNum > (dwSize-dwStation)/4 )
			return FALSE;
		if( nPartNum > 0 )
		{
			pPart = new int[nPartNum];
			if( nPartNum == 1 )
			{
				Fread( pFBuf, dwSize, dwStation, pPart, sizeof(int)*nPartNum, 1 );
				pPart[0] = nPtNum;
			}
			else
			{
				int *pTemp = new int[nPartNum];
				Fread( pFBuf, dwSize, dwStation, pTemp, sizeof(int)*nPartNum, 1 );
				int i;
				for( i=0;	i< nPartNum-1;	i++ )
					pPart[i] = pTemp[i+1]-pTemp[i];
				pPart[i] = nPtNum - pTemp[i];
				delete[] pTemp;
			}
		}

		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
		if( nAnno > dwSize-dwStation )
		{
			if( pPart )
				delete[] pPart;
			return FALSE;
		}
		if( nAnno > 0 )
		{
			szAnno = new char[nAnno+1];
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
			szAnno[nAnno] = '\0';
			nAnno += 1;
		}

		BYTE nShowClass = 0;
		Fread( pFBuf, dwSize, dwStation, &nShowClass, sizeof(BYTE), 1 );
		if( m_bNation )
			nShowClass = 9;

		if( nPtNum <= 0 )
		{
			if( szAnno )
				delete[] szAnno;
			if( nPartNum >= 1 )
				delete[] pPart;
			return FALSE;
		}
		if( nPtNum > (dwSize-dwStation)/4 )
		{
			if( pPart )
				delete[] pPart;
			if( szAnno )
				delete[] szAnno;
			return FALSE;
		}
		pPt = new ST_GEO_PTXY[nPtNum];
//		if( m_bNation )
			Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nPtNum, 1 );
//		else
//		{
//			unsigned short nVal;
//			for( int i=0;	i<nPtNum;	i++ )
//			{
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dx = BMAP_ORIGN_POS.dx + nVal/100000.0;
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dy = BMAP_ORIGN_POS.dy + nVal/100000.0;
//			}
//		}
		if( nPtNum <= 2 )
		{
			if( szAnno )
				delete[] szAnno;
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			return FALSE;
		}
		pPoly = new CGIS_FeaturePoly( EN_GOTYPE_POLY );
        pPoly->SetAnnotationInfo(nAnno,szAnno);
		if( szAnno )
        {         
            delete [] szAnno;
            szAnno = NULL;
        }
//		pPoly->SetPart( pPart );
//		pPoly->SetPartNum( nPartNum );
        pPoly->SetPartInfo(nPartNum,pPart);
        if( pPart )
				delete[] pPart;
        
		ST_GEO_RECT rtObj;
		CalcRect( pPt, nPtNum, rtObj );
		pPoly->SetRectObj( rtObj );
		pPoly->SetCenterPt( float((rtObj.dLeft+rtObj.dRight)/2.0), float((rtObj.dTop+rtObj.dBottom)/2.0) );
	
        pPoly->SetPtInfo(nPtNum,pPt);
        if(pPt)
        {
            delete pPt;
            pPt = NULL;
        }

		pPoly->m_nBreadthID = m_nBreadthID;
		pPoly->SetPolyShowClass( nShowClass );
	//	pLayer->GetObjectList()->AddTail( pPoly );
	}
	CATCH( CMemoryException, e )
	{
		if( pPoly )
			delete pPoly;
		else
		{
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			if( szAnno )
				delete[] szAnno;
		}
		return FALSE;
	}
	END_CATCH
	pOList->AddTail( pPoly );
	return TRUE;
}
BOOL CGIS_LFBand::ReadRutFeature( char *pFBuf, const DWORD dwSize, 
																 DWORD &dwStation, short nBreadthID, std::list<CGIS_Feature*>& pOList  )
{
	int nPtNum = 0, nPartNum = 0, nAnno = 0;
	int nID = 0;
	BYTE byDirection = 0;
	ST_GEO_PTXY *pPt = NULL;
	int *pPart = NULL;
	char *szAnno = NULL;
	CGIS_FeatureRut *pRut = NULL;
	TRY
	{
		Fread( pFBuf, dwSize, dwStation, &nPtNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nPartNum, sizeof(int), 1 );

		if( nPartNum > (dwSize-dwStation)/4 )
			return FALSE;
		if( nPartNum > 0 )
		{
			pPart = new int[nPartNum];
			if( nPartNum == 1 )
			{
				Fread( pFBuf, dwSize, dwStation, pPart, sizeof(int)*nPartNum, 1 );
				pPart[0] = nPtNum;
			}
			else
			{
				int *pTemp = new int[nPartNum];
				Fread( pFBuf, dwSize, dwStation, pTemp, sizeof(int)*nPartNum, 1 );
				int i;
				for( i=0;	i< nPartNum-1;	i++ )
					pPart[i] = pTemp[i+1]-pTemp[i];
				pPart[i] = nPtNum - pTemp[i];
				delete[] pTemp;
			}
		}
		Fread( pFBuf, dwSize, dwStation, &nID, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &byDirection, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
		if( nAnno > dwSize-dwStation )
		{
			if( pPart )
				delete[] pPart;
			return FALSE;
		}
		if( nAnno > 0 )
		{
			szAnno = new char[nAnno+1];
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
			szAnno[nAnno] = '\0';
			nAnno += 1;
		}
		if( nPtNum <= 0 )
		{
			if( szAnno )
				delete[] szAnno;
			if( nPartNum >= 1 )
				delete[] pPart;
			return FALSE;
		}
		if( nPtNum > (dwSize-dwStation)/4 )
		{
			if( pPart )
				delete[] pPart;
			if( szAnno )
				delete[] szAnno;
			return FALSE;
		}
		pPt = new ST_GEO_PTXY[nPtNum];
//		if( m_bNation )
			Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nPtNum, 1 );
//		else
//		{
//			unsigned short nVal;
//			for( int i=0;	i<nPtNum;	i++ )
//			{
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dx = BMAP_ORIGN_POS.dx + nVal/100000.0;
//				Fread( pFBuf, dwSize, dwStation, &nVal, sizeof(unsigned short), 1 );
//				pPt[i].dy = BMAP_ORIGN_POS.dy + nVal/100000.0;
//			}
//		}
		if( nPtNum <= 1 )
		{
			if( szAnno )
				delete[] szAnno;
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			return FALSE;
		}
		pRut = new CGIS_FeatureRut( EN_GOTYPE_RUT );
        pRut->SetAnnotationInfo(nAnno,szAnno);
		if( szAnno )
        {         
            delete [] szAnno;
            szAnno = NULL;
        }
		if( pPart )
			delete[] pPart;
		pPart = NULL;
		nPartNum = 0;
        

		ST_GEO_RECT rtObj;
		CalcRect( pPt, nPtNum, rtObj );
		pRut->SetRectObj( rtObj );
        
		pRut->SetPtInfo(nPtNum,pPt);
        if(pPt)
        {
            delete pPt;
            pPt = NULL;
        }

		pRut->SetRutID( nID );
		pRut->SetRutDirection( byDirection );
		pRut->m_nBreadthID = m_nBreadthID;
	}
	CATCH( CMemoryException, e )
	{
		if( pRut )
			delete pRut;
		else
		{
			if( pPart )
				delete[] pPart;
			if( pPt )
				delete[] pPt;
			if( szAnno )
				delete[] szAnno;
		}
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	pOList->AddTail( pRut );
	return TRUE;
}

int CGIS_LFBand::m_nMemoryDest = 1;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//开始将数据读入缓冲池 
BOOL CGIS_LFBand::ReadPointFeatureInSpecialSpace( 
    char *pFBuf,
    const DWORD dwSize, 
    DWORD &dwStation, 
    short nBreadthID,
    short sIconID,
    CPtrList* pOList,
    IMemoryMng* pMemmoryMngOfSelf,
    IMemoryMng* pMemoryMngOfBuddy)
{
	unsigned short nPOIType = 0;
	ST_GEO_PTXY *pPt = NULL;

	BYTE nAddrLen = 0;
	char* szAddr = NULL;
	BYTE nPhoneLen = 0;
	char* szPhone = NULL;
	//BYTE nAnno = 0;
	UINT nAnno = 0; // scott
	char* szAnno = NULL;
	BYTE barrier;
    
	CGIS_FeaturePoint *pPoint = NULL;
	
	TRY
	{
		
		Fread( pFBuf, dwSize, dwStation, &nPOIType, sizeof(unsigned short), 1 );//POI类型

		Fread( pFBuf, dwSize, dwStation, &nAddrLen, sizeof(BYTE), 1 ); //地址字串长度(此长充不含结束的'\0'字符)
		if( nAddrLen > dwSize-dwStation ) //数据过长了
        {
            AfxThrowMemoryException();
        }
		if( nAddrLen > 0 )
		{
			szAddr = (char*)pMemoryMngOfBuddy->newmalloc(nAddrLen+1); //分配空间存放地址串
            if(!szAddr)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
			Fread( pFBuf, dwSize, dwStation, szAddr, sizeof(char)*nAddrLen, 1 );
			szAddr[nAddrLen] = '\0';
			nAddrLen += 1;
		}

		Fread( pFBuf, dwSize, dwStation, &nPhoneLen, sizeof(BYTE), 1 ); //电话号码字串长度(此长充不含结束的'\0'字符)
		if( nPhoneLen > dwSize-dwStation )
		{
            AfxThrowMemoryException();
		}
		
		if( nPhoneLen > 0 )
		{
            szPhone = (char*)pMemoryMngOfBuddy->newmalloc(nPhoneLen+1);//电话号码字串

            if(!szPhone)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
			Fread( pFBuf, dwSize, dwStation, szPhone, sizeof(char)*nPhoneLen, 1 );
			szPhone[nPhoneLen] = '\0';
			nPhoneLen += 1;
		}
		// scott 
		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
		
		
		if( nAnno > dwSize-dwStation )
		{
            AfxThrowMemoryException();
		}
	 
		if( nAnno > 0 )
		{
            szAnno = (char*)pMemoryMngOfBuddy->newmalloc(nAnno + 1); //注解字串长度(此长充不含结束的'\0'字符)

            if(!szAnno)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
			szAnno[nAnno] = '\0';
			nAnno += 1; //注解字串

		}
        {
            pPt = (ST_GEO_PTXY*)pMemoryMngOfBuddy->newmalloc(sizeof(ST_GEO_PTXY));
            if(!pPt)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
            Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY), 1 ); //读出座标点
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        
        CGIS_FeaturePoint* pObjAddr = (CGIS_FeaturePoint*)pMemmoryMngOfSelf->newmalloc(sizeof(CGIS_FeaturePoint));
        
        pPoint = new(pObjAddr) CGIS_FeaturePoint( EN_GOTYPE_POINT );   //在指定堆上分配对象     
        pPoint->m_nMemmoryType = m_nMemoryDest;
        pPoint->SetPtInfoDirect(1,pPt);
        pPoint->SetPhoneInfoDirect( nPhoneLen , szPhone );
        pPoint->SetAddressInfoDirect( nAddrLen,szAddr );
        pPoint->SetAnnotationInfoDirect(nAnno ,szAnno );
        
        		

		pPoint->SetIconID( sIconID ); //地图上的图标样子
		pPoint->SetPointObjectType( nPOIType );
		pPoint->m_nBreadthID = m_nBreadthID;

	}CATCH( CUserException, e )
	{
		if( pPoint )
        {
            pMemmoryMngOfSelf->newfree(pPoint);
        }
		else
		{
			if(pPt)
				pMemoryMngOfBuddy->newfree(pPt);
			if( szAnno )
				pMemoryMngOfBuddy->newfree(szAnno);
			if( szPhone )
				pMemoryMngOfBuddy->newfree(szPhone);
			if( szAddr )
				pMemoryMngOfBuddy->newfree(szAddr);
		}
		THROW_LAST();
	}
	AND_CATCH( CMemoryException, e )
	{
		if( pPoint )
        {
            pMemmoryMngOfSelf->newfree(pPoint);
        }
		else
		{
			if(pPt)
				pMemoryMngOfBuddy->newfree(pPt);
			if( szAnno )
				pMemoryMngOfBuddy->newfree(szAnno);
			if( szPhone )
				pMemoryMngOfBuddy->newfree(szPhone);
			if( szAddr )
				pMemoryMngOfBuddy->newfree(szAddr);
		}
        return FALSE;
        //THROW(new CUserException(TRUE,EN_ET_MEMOVER));
	}
	END_CATCH
        
	pOList->AddTail( pPoint );
    
	return TRUE;
}

BOOL CGIS_LFBand::ReadLineFeatureInSpecialSpace(
                                                char *pFBuf,
                                                const DWORD dwSize, 
                                                DWORD &dwStation, 
                                                short nBreadthID, 
                                                CPtrList* pOList, 
                                                IMemoryMng* pMemmoryMngOfSelf, 
                                                IMemoryMng* pMemoryMngOfBuddy)
{
	int nPtNum = 0, nAnno = 0;
	short nPartNum = 0;
	ST_GEO_PTXY *pPt = NULL;
	int *pPart = NULL;
	char *szAnno = NULL;
	CGIS_FeatureLine *pLine = NULL;
	TRY
	{
		Fread( pFBuf, dwSize, dwStation, &nPtNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nPartNum, sizeof(short), 1 );
		if( nPartNum > (dwSize-dwStation)/4 )
			return FALSE;

        
		if( nPartNum > 0 )
		{
			//pPart = new int[nPartNum];
            pPart = (int*)pMemoryMngOfBuddy->newmalloc(sizeof(int)*nPartNum);
            if(!pPart)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
			if( nPartNum == 1 )
			{
				Fread( pFBuf, dwSize, dwStation, pPart, sizeof(int)*nPartNum, 1 );
				pPart[0] = nPtNum;
			}
			else
			{
				int *pTemp = new int[nPartNum];
				Fread( pFBuf, dwSize, dwStation, pTemp, sizeof(int)*nPartNum, 1 );
				int i;
				for( i=0;	i< nPartNum-1;	i++ )
					pPart[i] = pTemp[i+1]-pTemp[i];
				pPart[i] = nPtNum - pTemp[i];
				delete[] pTemp;
			}
		}

		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
		if( nAnno > dwSize-dwStation )
		{
            AfxThrowMemoryException();
		}
		if( nAnno > 0 )
		{
            szAnno = (char*)pMemoryMngOfBuddy->newmalloc(nAnno + 1);
            if(!szAnno)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
			szAnno[nAnno] = '\0';
			nAnno += 1;
		}       
		if( nPtNum <= 0 )
		{
            TRACE0("pt num error \n");
			AfxThrowMemoryException();
        }
        else
        {            
            if( nPtNum > (dwSize-dwStation)/4 )
            {
                AfxThrowMemoryException();
            }		
            {
                pPt = (ST_GEO_PTXY*)pMemoryMngOfBuddy->newmalloc(sizeof(ST_GEO_PTXY)*nPtNum);
                if(!pPt)
                {
                    THROW(new CUserException(TRUE,EN_ET_MEMOVER));
                }
                
                Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nPtNum, 1 );
            }
            if( nPtNum <= 1 )
            {
                TRACE0("pt num error \n");
                AfxThrowMemoryException();
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        
        CGIS_FeatureLine* pObjAddr = (CGIS_FeatureLine*)pMemmoryMngOfSelf->newmalloc(sizeof(CGIS_FeatureLine));
        pLine = new(pObjAddr) CGIS_FeatureLine( EN_GOTYPE_LINE );
        pLine->m_nMemmoryType = m_nMemoryDest;
        pLine->SetAnnotationInfoDirect(nAnno,szAnno);
        pLine->SetPartInfoDirect(nPartNum,pPart);
        pLine->SetPtInfoDirect(nPtNum,pPt);
        

		ST_GEO_RECT rtObj;
		CalcRect( pPt, nPtNum, rtObj );
		pLine->SetRectObj( rtObj );
        
		pLine->m_nBreadthID = m_nBreadthID;
	}
	CATCH( CUserException, e )
	{
		if( pLine )
        {
            pMemmoryMngOfSelf->newfree(pLine);
        }
		else
		{
			if( pPt )
				pMemoryMngOfBuddy->newfree(pPt);
			if( szAnno )
				pMemoryMngOfBuddy->newfree(szAnno);
			if( pPart )
				pMemoryMngOfBuddy->newfree(pPart);
		}
		THROW_LAST();
	}
	AND_CATCH( CMemoryException, e )
	{
		if( pLine )
        {
            pMemmoryMngOfSelf->newfree(pLine);
        }
		else
		{
			if( pPt )
				pMemoryMngOfBuddy->newfree(pPt);
			if( szAnno )
				pMemoryMngOfBuddy->newfree(szAnno);
			if( pPart )
				pMemoryMngOfBuddy->newfree(pPart);
		}

        return FALSE;
        //THROW(new CUserException(TRUE,EN_ET_MEMOVER));

	}
	END_CATCH
        
	pOList->AddTail( pLine );
	return TRUE;
}

BOOL CGIS_LFBand::ReadPolyFeatureInSpecialSpace(
                                                char *pFBuf, 
                                                const DWORD dwSize, 
                                                DWORD &dwStation, 
                                                short nBreadthID, 
                                                CPtrList* pOList,
                                                IMemoryMng* pMemmoryMngOfSelf, 
                                                IMemoryMng* pMemoryMngOfBuddy,
												BOOL bHaveID )
{
	int nPtNum = 0,  nAnno = 0 , nID=0 ;
	short nPartNum = 0;
	ST_GEO_PTXY *pPt = NULL;
	int *pPart = NULL;
	char *szAnno = NULL;
	CGIS_FeaturePoly *pPoly = NULL;
	TRY
	{
		Fread( pFBuf, dwSize, dwStation, &nPtNum, sizeof(int), 1 ); //拐点数量
		Fread( pFBuf, dwSize, dwStation, &nPartNum, sizeof(short), 1 ); //Part数量

		if( nPartNum > (dwSize-dwStation)/4 ) //数据过大
			return FALSE;
		if( nPartNum > 0 )
		{
            pPart = (int*)pMemoryMngOfBuddy->newmalloc(sizeof(int)*nPartNum); //存放指针的空间
            if(!pPart)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
			if( nPartNum == 1 ) //只有1块
			{
				Fread( pFBuf, dwSize, dwStation, pPart, sizeof(int)*nPartNum, 1 );
				pPart[0] = nPtNum; //所有的点都隶属与一块
			}
			else
			{//这里的结构文档中没有定义
				int *pTemp = new int[nPartNum];
				Fread( pFBuf, dwSize, dwStation, pTemp, sizeof(int)*nPartNum, 1 );
				int i;
				for( i=0;	i< nPartNum-1;	i++ )
					pPart[i] = pTemp[i+1]-pTemp[i];
				pPart[i] = nPtNum - pTemp[i];
				delete[] pTemp;
			}
		}
		
		if( bHaveID )
			Fread( pFBuf, dwSize, dwStation, &nID, sizeof(int), 1 ); //FeatureID

		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );//注解字串长度
		if( nAnno > dwSize-dwStation )
		{
			AfxThrowMemoryException();
		}
		if( nAnno > 0 )
		{
			szAnno = (char*)pMemoryMngOfBuddy->newmalloc(nAnno + 1);
            if(!szAnno)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 ); //注解字串
			szAnno[nAnno] = '\0';
			nAnno += 1;
		}

		BYTE nShowClass = 0;
		Fread( pFBuf, dwSize, dwStation, &nShowClass, sizeof(BYTE), 1 ); //显示等级
		if( m_bNation )
			nShowClass = 9;

		if( nPtNum <= 0 )
		{
            TRACE0("pt num error \n");
			AfxThrowMemoryException();
        }
        else
        { //这里看上去不会进来
            
            if( nPtNum > (dwSize-dwStation)/4 )
            {
                AfxThrowMemoryException();
            }
            
            {
                pPt = (ST_GEO_PTXY*)pMemoryMngOfBuddy->newmalloc(sizeof(ST_GEO_PTXY)*nPtNum);
                if(!pPt)
                {
                    THROW(new CUserException(TRUE,EN_ET_MEMOVER));
                }
                Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nPtNum, 1 );
            }
            
            if( nPtNum <= 2 )
            {
                TRACE0("pt num error \n");
                AfxThrowMemoryException();
           }
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        CGIS_FeaturePoly* pObjAddr = (CGIS_FeaturePoly*)pMemmoryMngOfSelf->newmalloc(sizeof(CGIS_FeaturePoly));
        
        
        pPoly = new(pObjAddr) CGIS_FeaturePoly( EN_GOTYPE_POLY ); //在已经分配好的堆上初始化对象
        pPoly->m_nMemmoryType = m_nMemoryDest;
        pPoly->SetAnnotationInfoDirect(nAnno,szAnno);
        pPoly->SetPartInfoDirect(nPartNum,pPart);
        pPoly->SetPtInfoDirect(nPtNum,pPt);
		pPoly->SetPolyID( nID );
        

		ST_GEO_RECT rtObj;
		CalcRect( pPt, nPtNum, rtObj ); //计算点集合的envelope rect
		pPoly->SetRectObj( rtObj );
		pPoly->SetCenterPt( float((rtObj.dLeft+rtObj.dRight)/2.0), float((rtObj.dTop+rtObj.dBottom)/2.0) ); //计算中心点
	

		pPoly->m_nBreadthID = m_nBreadthID;
		pPoly->SetPolyShowClass( nShowClass ); //设置显示等级
	}
    CATCH( CUserException, e )
	{
		if( pPoly )
        {
            pMemmoryMngOfSelf->newfree(pPoly);
        }
		else
		{
			if( pPt )
				pMemoryMngOfBuddy->newfree(pPt);
			if( szAnno )
				pMemoryMngOfBuddy->newfree(szAnno);
			if( pPart )
				pMemoryMngOfBuddy->newfree(pPart);
		}
		THROW_LAST();
	}
	AND_CATCH( CMemoryException, e )
	{
		if( pPoly )
        {
            pMemmoryMngOfSelf->newfree(pPoly);
        }
		else
		{
			if( pPt )
				pMemoryMngOfBuddy->newfree(pPt);
			if( szAnno )
				pMemoryMngOfBuddy->newfree(szAnno);
			if( pPart )
				pMemoryMngOfBuddy->newfree(pPart);
		}
        return FALSE;
        //THROW(new CUserException(TRUE,EN_ET_MEMOVER));
	}
	END_CATCH
        
	pOList->AddTail( pPoly );
	return TRUE;
}

BOOL CGIS_LFBand::ReadRutFeatureInSpecialSpace(
                                               char *pFBuf, 
                                               const DWORD dwSize, 
                                               DWORD &dwStation,
                                               short nBreadthID,
                                               CPtrList* pOList,
                                                IMemoryMng* pMemmoryMngOfSelf, 
                                                IMemoryMng* pMemoryMngOfBuddy)
{
	int nPtNum = 0, nAnno = 0;
	short nPartNum = 0;
	short nID = 0;
	BYTE byDirection = 0;
	ST_GEO_PTXY *pPt = NULL;
	int *pPart = NULL;
	char *szAnno = NULL;
	CGIS_FeatureRut *pRut = NULL;
    
	BYTE	byLaneNum = 0;
    DWORD dwLaneInfoOffset = 0;
	BYTE *  pbyStartLaneDir = NULL;
	BYTE *  pbyEndLaneDir = NULL;

	BYTE	bySignInfoCount = 0;
    CGIS_FeatureRut::LPSIGNINFO pSignInfo = NULL;

	BYTE	byImgInfoCount = 0;
	CGIS_FeatureRut::LPIMGINFO pImgInfo = NULL;
	
	//shunxing
	BYTE		byShunXingCount = 0;
	WORD *		pwShunXingOutRutID = NULL;


	TRY
	{
		Fread( pFBuf, dwSize, dwStation, &nPtNum, sizeof(int), 1 );
//		Fread( pFBuf, dwSize, dwStation, &nPartNum, sizeof(short), 1 );
//
//		if( nPartNum > (dwSize-dwStation)/4 )
//			return FALSE;
//		if( nPartNum > 0 )
//		{
//			pPart = (int*)pMemoryMngOfBuddy->newmalloc(sizeof(int)*nPartNum);
//            if(!pPart)
//            {
//                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
//            }
//			if( nPartNum == 1 )
//			{
//				Fread( pFBuf, dwSize, dwStation, pPart, sizeof(int)*nPartNum, 1 );
//				pPart[0] = nPtNum;
//			}
//			else
//			{
//				int *pTemp = new int[nPartNum];
//				Fread( pFBuf, dwSize, dwStation, pTemp, sizeof(int)*nPartNum, 1 );
//				for( int i=0;	i< nPartNum-1;	i++ )
//					pPart[i] = pTemp[i+1]-pTemp[i];
//				pPart[i] = nPtNum - pTemp[i];
//				delete[] pTemp;
//			}
//		}
		Fread( pFBuf, dwSize, dwStation, &nID, sizeof(nID), 1 ); //路段ID
		Fread( pFBuf, dwSize, dwStation, &byDirection, sizeof(BYTE), 1 ); //单双行信息
		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 ); //注解字串长充
		if( nAnno > dwSize-dwStation )
		{
			AfxThrowMemoryException();
		}
		if( nAnno > 0 )
		{
			szAnno = (char*)pMemoryMngOfBuddy->newmalloc(nAnno + 1);
            if(!szAnno)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
			Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
			szAnno[nAnno] = '\0';
			nAnno += 1;
		}

	{
//		//new add lane
//		//Lane
//        Fread( pFBuf, dwSize, dwStation, &byLaneNum, sizeof(byLaneNum), 1);
//		
//		//Start Lane
//		if (( byLaneNum >> 4) > 0)
//		{
//            pbyStartLaneDir = (BYTE*)pMemoryMngOfBuddy->newmalloc( sizeof(BYTE)*(byLaneNum>>4));
//            if(!pbyStartLaneDir)
//            {
//                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
//            }
//            Fread( pFBuf, dwSize, dwStation, pbyStartLaneDir, sizeof( pbyStartLaneDir[0]) * ( byLaneNum >> 4), 1);
//		}
//		
//		//End Lane
//		if (( byLaneNum & 0x0F ) > 0)
//		{
//            pbyEndLaneDir = (BYTE*)pMemoryMngOfBuddy->newmalloc( sizeof(BYTE)*(byLaneNum & 0x0F));
//            if(!pbyEndLaneDir)
//            {
//                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
//            }
//            Fread( pFBuf, dwSize, dwStation, pbyEndLaneDir, sizeof( pbyEndLaneDir[0]) * ( byLaneNum & 0x0F), 1);
//		}
        Fread( pFBuf, dwSize, dwStation, &dwLaneInfoOffset, sizeof(dwLaneInfoOffset), 1); // 车道信息 4bytes

		//sign(ic、dr)
        Fread( pFBuf, dwSize, dwStation, &bySignInfoCount, sizeof(bySignInfoCount), 1); //符号信息个数1 bytes
		if ( bySignInfoCount > 0)
		{
            void* pAddr = pMemoryMngOfBuddy->newmalloc( sizeof(CGIS_FeatureRut::SIGNINFO)*bySignInfoCount);
            if(!pAddr)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
            memset(pAddr,0,sizeof(CGIS_FeatureRut::SIGNINFO)*bySignInfoCount);
            pSignInfo = (CGIS_FeatureRut::SIGNINFO*)pAddr;
			for (int i = 0; i <  bySignInfoCount; i++)
			{
                Fread( pFBuf, dwSize, dwStation, &pSignInfo[i].wOutRutID, sizeof( pSignInfo[i].wOutRutID), 1);
                Fread( pFBuf, dwSize, dwStation, &pSignInfo[i].byNameLen, sizeof( pSignInfo[i].byNameLen), 1);
				if ( pSignInfo[i].byNameLen > 0)
				{
                    void* pAddr = pMemoryMngOfBuddy->newmalloc( sizeof(char)*pSignInfo[i].byNameLen+1);
                    if(!pAddr)
                    {
                        THROW(new CUserException(TRUE,EN_ET_MEMOVER));
                    }
                    pSignInfo[i].pszName = (char*)pAddr;
                    Fread( pFBuf, dwSize, dwStation, pSignInfo[i].pszName, sizeof( char)*pSignInfo[i].byNameLen, 1);
					pSignInfo[i].pszName[pSignInfo[i].byNameLen] = 0;
					pSignInfo[i].byNameLen++;
                }
			}
		}

		//imginfo	
        Fread( pFBuf, dwSize, dwStation, &byImgInfoCount, sizeof( byImgInfoCount), 1);
		if ( byImgInfoCount > 0)
		{
            void* pAddr = pMemoryMngOfBuddy->newmalloc( sizeof(CGIS_FeatureRut::IMGINFO)*byImgInfoCount);
            if(!pAddr)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }

            pImgInfo = (CGIS_FeatureRut::IMGINFO*) pAddr;
			for (int i = 0; i <  byImgInfoCount; i++)
			{
                Fread( pFBuf, dwSize, dwStation, &pImgInfo[i].wOutRutID, sizeof( pImgInfo[i].wOutRutID), 1);
                Fread( pFBuf, dwSize, dwStation, &pImgInfo[i].nImgID, sizeof( pImgInfo[i].nImgID), 1);
                Fread( pFBuf, dwSize, dwStation, &pImgInfo[i].nArrowID, sizeof( pImgInfo[i].nArrowID), 1);
			}
		}

        Fread( pFBuf, dwSize, dwStation, &byShunXingCount, sizeof( byShunXingCount), 1);
		if ( byShunXingCount > 0)
		{
            void* pAddr = pMemoryMngOfBuddy->newmalloc( sizeof(WORD)*byShunXingCount);
            if(!pAddr)
            {
                THROW(new CUserException(TRUE,EN_ET_MEMOVER));
            }
            pwShunXingOutRutID = (WORD*)pAddr;
            Fread( pFBuf, dwSize, dwStation, pwShunXingOutRutID, sizeof( pwShunXingOutRutID[0]) *  byShunXingCount, 1);
		}
	}//end if (nDataVersion == DataVersion2)

    
        if( nPtNum <= 0 )
        {
            TRACE0("pt num error \n");
            AfxThrowMemoryException();
        }
        else
        {
            
            if( nPtNum > (dwSize-dwStation)/4 )
            {
                AfxThrowMemoryException();
            }
            
            {
                pPt = (ST_GEO_PTXY*)pMemoryMngOfBuddy->newmalloc(sizeof(ST_GEO_PTXY)*nPtNum);
                if(!pPt)
                {
                    THROW(new CUserException(TRUE,EN_ET_MEMOVER));
                }
                Fread( pFBuf, dwSize, dwStation, pPt, sizeof(ST_GEO_PTXY)*nPtNum, 1 );
            }
            
            if( nPtNum <= 1 )
            {
                AfxThrowMemoryException();
            }
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        CGIS_FeatureRut* pObjAddr = (CGIS_FeatureRut*)pMemmoryMngOfSelf->newmalloc(sizeof(CGIS_FeatureRut));
        
        pRut = new(pObjAddr) CGIS_FeatureRut( EN_GOTYPE_RUT );
        pRut->m_nMemmoryType = m_nMemoryDest;
        pRut->SetAnnotationInfoDirect(nAnno,szAnno);
        pRut->SetPtInfoDirect(nPtNum,pPt);
        
        
		ST_GEO_RECT rtObj;
		CalcRect( pPt, nPtNum, rtObj );
		pRut->SetRectObj( rtObj );
    
		pRut->SetRutID( nID );
		pRut->SetRutDirection( byDirection );
        pRut->SetImgInfoDirect(byImgInfoCount,pImgInfo);
        pRut->SetSignInfoDirect(bySignInfoCount,pSignInfo);

        pRut->SetLaneInfoOffset( dwLaneInfoOffset);
		pRut->m_nBreadthID = m_nBreadthID;
	}
    CATCH( CUserException, e )
	{
		if( pRut )
        {
            pMemmoryMngOfSelf->newfree(pRut);
        }
		else
		{
			if( pPt )
				pMemoryMngOfBuddy->newfree(pPt);
			if( szAnno )
				pMemoryMngOfBuddy->newfree(szAnno);
			if( pPart )
				pMemoryMngOfBuddy->newfree(pPart);
		}
		THROW_LAST();
	}
	AND_CATCH( CMemoryException, e )
	{
		if( pRut )
        {
            pMemmoryMngOfSelf->newfree(pRut);
        }
		else
		{
			if( pPt )
				pMemoryMngOfBuddy->newfree(pPt);
			if( szAnno )
				pMemoryMngOfBuddy->newfree(szAnno);
			if( pPart )
				pMemoryMngOfBuddy->newfree(pPart);
		}
        return FALSE;

        //THROW(new CUserException(TRUE,EN_ET_MEMOVER));
	}
	END_CATCH
        
	pOList->AddTail( pRut );
	return TRUE;
}
