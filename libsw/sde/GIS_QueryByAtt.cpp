
#include "stdafx.h"
#include "GIS_QueryByAtt.h"
#include "GIS_LayerManager.h"
#include "GIS_GeoMap.h"
#include "SDEControler.h"
#include "GIS_MapDataManager.h"
#include "./Zlib/zlib.h"

#include "MemoryDispatchForSpaceDataV2.h"
//////////////////////////////////////////////////////////////////////////
#include "./DebugFunctionLib/DebugFun.h"

static HANDLE gRichPOIFile = INVALID_HANDLE_VALUE;
CGIS_QueryByAtt::CGIS_QueryByAtt()
{
	m_Type = QUERY_BYATT;
	m_bStopSearch = FALSE;
	m_pListLevel = new CPtrList;
}

CGIS_QueryByAtt::~CGIS_QueryByAtt(){

	std::list<RICHPOI_LEVEL1*>::iterator itr;
	for(itr = m_pListLevel.begin();itr!=m_pListLevel.end();itr++){
		RICHPOI_LEVEL1 * p = *itr;
		delete p;
	}
	/*
	if( m_pListLevel )
	{
		RICHPOI_LEVEL1* pRichPOILevel1;
		RICHPOI_LEVEL2* pRichPOILevel2;
		RICHPOI_LEVEL3* pRichPOILevel3;
		POSITION pos = m_pListLevel->GetHeadPosition();
		while( pos )
		{
			pRichPOILevel1 = (RICHPOI_LEVEL1*)m_pListLevel->GetNext( pos );
			if( pRichPOILevel1 && pRichPOILevel1->pLevel2List.GetCount() >0 )
			{
				POSITION pos2 = pRichPOILevel1->pLevel2List.GetHeadPosition();
				while( pos2 )
				{
					pRichPOILevel2 = (RICHPOI_LEVEL2*)( pRichPOILevel1->pLevel2List.GetNext( pos2 ) );
					if( pRichPOILevel2 && pRichPOILevel2->pLevel3List.GetCount() >0 )
					{
						POSITION pos3 = pRichPOILevel2->pLevel3List.GetHeadPosition();
						while( pos3 )
						{
							pRichPOILevel3 = (RICHPOI_LEVEL3*)( pRichPOILevel2->pLevel3List.GetNext( pos3 ) );
							if( pRichPOILevel3 )
							{
								delete pRichPOILevel3;
								pRichPOILevel3 = NULL;
							}
						}
						pRichPOILevel2->pLevel3List.RemoveAll();
					}
					delete pRichPOILevel2;
					pRichPOILevel2 = NULL;
				}
				pRichPOILevel1->pLevel2List.RemoveAll();
			}
			delete pRichPOILevel1;
			pRichPOILevel1 = NULL;
		}
		delete m_pListLevel;
		m_pListLevel = NULL;
	}*/
}


int CGIS_QueryByAtt:: GetNameID(swString strName){
	char *str;
	str = new char[2];
	
	//UnicodeToChar(strName, str, 2);
	str = strName.ToUTF8().data()

	int nID =  GetJPID( str );
	delete[] str;
	return nID;
}
static HANDLE gHAFile = INVALID_HANDLE_VALUE;
BOOL CGIS_QueryByAtt::GetNameNIN(CPtrList& pReturnFeaSet, swString strMapPath, int nCityID, swString strName, BYTE ntype,int nPostID )
{
    TRACK_FUNCTION("GetNameNIN 1");
	if( gHAFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.NIN", strMapPath );
		gHAFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHAFile)
			return FALSE;
	}
	char *pFBuf = NULL;
	TRY
	{
		// 由于读取可能失败，判断需要读出的数据和已读出的是否匹配

		CFile file(gHAFile);
		file.SeekToBegin();
		char szInfo[32];
		if (9 != file.Read( szInfo, 9 ))
			return FALSE;
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		int nFSize = 0;
		int nUnCom = 5;
		int nIndexPOILeng  = 0;
		int nIndexRoadLeng = 0;

		unsigned int nSize = sizeof(int);
		if (nSize != file.Read( &nMNum, nSize))
			return FALSE;

		BOOL bFind = FALSE;
		for( int i=0;	i<nMNum;	i++ )
		{
			if (nSize != file.Read( &nMID, nSize ))
				return FALSE;
			if (nSize != file.Read( &nMOff, nSize ))
				return FALSE;
			if (nSize != file.Read( &nFSize, nSize ))
				return FALSE;
			if (nSize != file.Read( &nUnCom, nSize ))
				return FALSE;
			if (nSize != file.Read( &nIndexPOILeng, nSize ))
				return FALSE;
			if (nSize != file.Read( &nIndexRoadLeng, nSize ))
				return FALSE;
			if( nMID == nCityID )
			{
				bFind = TRUE;
				break;
			}
		}
		if( !bFind )
			return 0;
		
		file.Seek( nMOff, CFile::begin );
//		file.Read( &nUnCom, sizeof(int) );
//		int nCom = nFSize - sizeof( int );
		pFBuf = new char[nUnCom];
		if( pFBuf == 0 )
			return 0;
		char *pBuf = new char[nFSize];
		if( pBuf == 0 )		
		{
			delete[]pFBuf;
			pFBuf = NULL;
			return 0;
		}
		file.Read( pBuf, nFSize );
		DWORD dwSize = nUnCom;  //定位到索引数据，进行解压缩
		uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nFSize );
		delete [] pBuf;


		DWORD dwStation = nIndexPOILeng + nIndexRoadLeng;

		Fread( pFBuf, dwSize, dwStation, szInfo, 3, 1 );
		swString strszInfo;
		szInfo[3] = '\0';
		CharToUnicode( szInfo , &strszInfo );
		if( strszInfo.Find( L"NIN" ) < 0 )
		{
			delete[] pFBuf;
			return FALSE;
		}
		swString  strName1 = strName;
		int nID = GetNameID(strName);
		
		if (ntype == 1)
			dwStation = dwStation + nID * 16;
		else
			dwStation = dwStation + nID * 16 + 8;

		int nOffset,nCount;
		
		Fread( pFBuf, dwSize, dwStation, &nOffset, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
		
		if ( dwSize > dwStation )
		{
			dwStation = nOffset + nIndexPOILeng + nIndexRoadLeng ;
		
			int   nCharNumber;
			int  BandId = 0;
			BYTE  LayerId = 0;
			BYTE  ClassId = 0;
			BYTE  nExit = 0;
			for (int i=0; i<nCount; i++)
			{			
				if (nExit == 2)
					break;
				
				Fread( pFBuf, dwSize, dwStation, &nCharNumber, sizeof(int), 1 );
				char *szAnno = NULL;
				if( nCharNumber > 0 )
				{
					szAnno = new char[nCharNumber+1];
					Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nCharNumber, 1 );
					szAnno[nCharNumber] = '\0';				
				}
				Fread(pFBuf, dwSize, dwStation, &BandId, sizeof(int), 1);
				Fread( pFBuf,dwSize,dwStation,&LayerId,sizeof(BYTE),1 ); 
				Fread( pFBuf,dwSize,dwStation,&ClassId,sizeof(BYTE),1 ); 
				swString sName;// = szAnno;
				CharToUnicode( szAnno , &sName );
				if (sName.Find(strName) == 0)
				{
					ST_INDEX_NAME  *index = new ST_INDEX_NAME;
					index->Name = szAnno;
					index->BandID = BandId;
					index->LayerID = LayerId;
					index->ClassID = ClassId;
					if (BandId != 0)
					{
						pReturnFeaSet.AddTail(index);
						if (nExit == 0) 
							nExit = 1;	
					}
					else
					{
						delete[] szAnno;
						delete index;
						index = NULL;
					}
				}
				else 
				{
					delete[] szAnno;
					if (nExit == 1)
						nExit = 2;					
				}						
			}			

		}
		delete[] pFBuf;
	}
	CATCH( CMemoryException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
		if(INVALID_HANDLE_VALUE != gHAFile)
			CloseHandle( gHAFile );
		gHAFile = INVALID_HANDLE_VALUE;
//		THROW( e );
		return FALSE;
	}
	AND_CATCH( CException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	return TRUE;
}

BOOL CGIS_QueryByAtt::QueryByType(CPtrList& pReturnFeaSet,CPtrList* pFeaSet,UINT LayID, unsigned short ClassID)
{
    TRACK_FUNCTION("QueryByType 1");

    if(pFeaSet == NULL)
	{
		return FALSE;
	}
	if(ClassID == 0)
	{
		return FALSE;
	}
	unsigned short id;
    POSITION pos = pFeaSet->GetHeadPosition();
	while(pos)
	{
		CGIS_FeaturePoint* pTempFea = (CGIS_FeaturePoint*)(pFeaSet->GetNext(pos));

		if(pTempFea)
		{
			if (pTempFea->GetObjType() == EN_GOTYPE_POINT )
			{				
//				id = GetPoiID(pTempFea->GetPointObjectType(), LayID);
				id = pTempFea->GetPointObjectType();
				if (id == ClassID)										
					pReturnFeaSet.AddTail( pTempFea->Clone( ));
			}
			else
				pReturnFeaSet.AddTail( pTempFea->Clone( ));
		}
	}
	return TRUE;		
}

BOOL CGIS_QueryByAtt::QueryByType(CPtrList& pReturnFeaSet,UINT LayID,unsigned short ClassID)
{
    TRACK_FUNCTION("QueryByType 2");

	if(m_pMap != NULL)
	{
		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
			for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
			{
				CGIS_MapLayer *pBLList[1];

//				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapList()[i];
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID2( i );
				if( pBMap != NULL )
				{
//					m_pMap->LoadBreadthMap( pBMap );
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							IGIS_Layer *pLayer = NULL;	
							pLManager->GetLayerListMap()->Lookup( LayID, pLayer );
							if(pLayer != NULL )
							{
								pBLList[0] = (CGIS_MapLayer*)pLayer;
								if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
									return FALSE;
//								pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );

								CPtrList *pOList = NULL;
								pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
								if( !pOList )
								{
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
									return FALSE;
								}
								POSITION pos1 = pOList->GetHeadPosition();
								CGIS_Feature*  pTempFea;				
								while( pos1 )
								{
									pTempFea = (CGIS_Feature*)(pOList->GetNext(pos1));
								
									if (pTempFea->GetObjType() == EN_GOTYPE_POINT )
									{
										CGIS_FeaturePoint* pTempFeaPoint = (CGIS_FeaturePoint*)pTempFea;										
										if (ClassID == 0 || pTempFeaPoint->GetPointObjectType() == ClassID)										
//										if (ClassID == 0 || GetPoiID(pTempFeaPoint->GetPointObjectType(), LayID) == ClassID)										
										pReturnFeaSet.AddTail( pTempFeaPoint->Clone( ));

									}
									else
										pReturnFeaSet.AddTail( pTempFea->Clone( ));
								}
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
							}
						}					



					}
//					m_pMap->UnLoadBreadthMap(pBMap);
				}
			}
		}
		
		if(m_pMap->GetMapFileType() == EN_MFTYPE_GUIDE30)
		{
			CGIS_LayerManager *pLManager = NULL;
			POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
			while(pos!= NULL)
			{
				pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
				if(pLManager != NULL)
				{
					IGIS_Layer *pLayer = NULL;	
					pLManager->GetLayerListMap()->Lookup( LayID, pLayer );
					if( pLayer )
					{	
						CPtrList *pOList = NULL;
						pLayer->GetObjectListMap()->Lookup( 0, pOList );
						if( !pOList )
							return FALSE;
						///miss my head
						POSITION ps = pOList->GetHeadPosition();
						while( ps )
							pReturnFeaSet.AddTail( ((CGIS_Feature*)pOList->GetNext(ps) )->Clone( ) );
					}

				}
			}		
		}
		return TRUE;

	}
	else
	{
		return FALSE;
	}	

}

BOOL CGIS_QueryByAtt::QueryByAnnotation(CPtrList& pReturnFeaSet,CPtrList* pFeaSet,swString strAnno)
{
    TRACK_FUNCTION("QueryByAnnotation 1");

	if(pFeaSet == NULL)
	{
		return FALSE;
	}
	swString strTempAnno = (swString)strAnno;
    POSITION pos = pFeaSet->GetHeadPosition();
	CGIS_Feature* pTempFea;
	while(pos)
	{
		pTempFea = (CGIS_Feature*)(pFeaSet->GetNext(pos));
		if(pTempFea)
		{
			swString strTempFeaAnno ;//= pTempFea->GetAnnotation();
			CharToUnicode( pTempFea->GetAnnotation() , &strTempFeaAnno );
			if(strTempFeaAnno.GetLength() >= strTempAnno.GetLength())
			{
				if( strTempFeaAnno == strTempAnno )
//				if( strTempFeaAnno.Find(strTempAnno)!=-1 )
				{
					pReturnFeaSet.AddTail(pTempFea->Clone());
				}
			}
		}
	}
	return TRUE;
}

BOOL CGIS_QueryByAtt::QueryByAnnotation(CPtrList& pReturnFeaSet,int BreadthMapId,int nLayerID,int nClassID,swString strAnno)
{
    //////////////////////////////////////////////////////////////////////////
    CSpaceMemoryLock aLock;    
    TRACK_FUNCTION("QueryByAnnotation 2");
    //////////////////////////////////////////////////////////////////////////
    
	if(m_pMap != NULL)
	{
		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
			CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( BreadthMapId );
//			CGIS_BreadthMap *pBMap = NULL;
//			for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ ) 
//			{
//				pBMap = m_pMap->GetBreadthMapList()[i];
//				if( pBMap )
//				{
//					if( pBMap->m_nBreadthID == BreadthMapId )
//						break;
//				}
//			}
			if (pBMap != NULL)
			{
//				m_pMap->LoadBreadthMap( pBMap );
				CGIS_LayerManager *pLManager = NULL;
				POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
				while(pos!= NULL)
				{
					pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
					if(pLManager != NULL)
					{
						IGIS_Layer *pLayer = NULL;	
						pLManager->GetLayerListMap()->Lookup(nLayerID,pLayer);
						if(pLayer != NULL)
						{
							CGIS_MapLayer *pBLList[1];

							pBLList[0] = (CGIS_MapLayer*)pLayer;
//							if( pBLList[0]->GetBLCount(pBMap->m_nBreadthID) == 0 )
							{
								if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
									return FALSE;
//								pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );
							}

							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
							if( !pOList )
							{
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								return FALSE;
							}
							QueryByAnnotation(pReturnFeaSet,pOList,strAnno);
							pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
						}

					}
				}
//				m_pMap->UnLoadBreadthMap(pBMap);
			}
			else
			{
				return FALSE;
			}

		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL CGIS_QueryByAtt::QueryByAnnotation( CPtrList& pReturnFeaSet , CPtrList* IndexList )
{
    TRACK_FUNCTION("QueryByAnnotation 3");
	if( !m_pMap )
		return FALSE;
	if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
	{
		CGIS_LayerManager *pLManager = NULL;
		POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
		while(pos!= NULL)
		{
			pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
			if(pLManager != NULL)
			{
				POSITION posIndex = IndexList->GetHeadPosition();
				CPtrList listBeFind;
				while( posIndex )
				{
//					POSITION posMem = posIndex;
					ST_INDEX_NAME* pindex;
					pindex = (ST_INDEX_NAME*)IndexList->GetNext(posIndex);
					//判断是否已经检索过该图层数据
					POSITION posBeFind = listBeFind.GetHeadPosition();
					ST_INDEX_NAME *pBeFindIn = NULL;
					BOOL bHaveSearch = FALSE;
					while( posBeFind )
					{
						POSITION posRemove = posBeFind;
						pBeFindIn = (ST_INDEX_NAME*)listBeFind.GetNext( posBeFind );
						if( pBeFindIn == pindex )
						{
							listBeFind.RemoveAt( posRemove );
							bHaveSearch = TRUE;
							break;
						}
					}
					if( bHaveSearch )
						continue;
					CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( pindex->BandID );
					if( !pBMap )
						continue;
					IGIS_Layer *pLayer = NULL;	
					pLManager->GetLayerListMap()->Lookup(pindex->LayerID,pLayer);
					if( !pLayer )
						continue;
					CGIS_MapLayer *pBLList[1];

					pBLList[0] = (CGIS_MapLayer*)pLayer;
//					if( ( (CGIS_MapLayer*)pLayer )->GetBLCount(pBMap->m_nBreadthID) ==0  )
					{
						if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
							continue;
//						pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );
					}

					
					CPtrList *pOList = NULL;
					pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
					if( !pOList )
						continue;
					swString str;
					CharToUnicode( pindex->Name , &str );
					QueryByAnnotation(pReturnFeaSet,pOList,str);
					//查找所有该图层的数据
					POSITION posFindAll = posIndex;
					ST_INDEX_NAME *pFindAll = NULL;
					while( posFindAll )
					{
						pFindAll = (ST_INDEX_NAME*)IndexList->GetNext( posFindAll );
						if( pFindAll->BandID == pindex->BandID && pFindAll->LayerID == pindex->LayerID )
						{
							swString str;
							CharToUnicode( pFindAll->Name , &str );
							QueryByAnnotation(pReturnFeaSet,pOList,str );
							listBeFind.AddTail( pFindAll );
						}
					}
					pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
				}
			}
		}
	}
	return TRUE;
}

/*
BOOL CGIS_QueryByAtt::QueryByAnnotation(CPtrList& pReturnFeaSet,swString strAnno)
{
	if(m_pMap != NULL)
	{
		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
			for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapList()[i];
				if( pBMap != NULL )
				{
					m_pMap->LoadBreadthMap( pBMap );
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
							IGIS_Layer *pLayer = NULL;	
							unsigned short    nKey;
							while(pos1 != NULL)
							{
								pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
								if(pLayer != NULL)
								{
									CPtrList *pOList = NULL;
									pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
									if( !pOList )
										return FALSE;
									QueryByAnnotation(pReturnFeaSet,pOList,strAnno);
								}
							
							}

						}
					}
					m_pMap->UnLoadBreadthMap(pBMap);
				}
			}
		}
		if(m_pMap->GetMapFileType() == EN_MFTYPE_GUIDE30)
		{
			CGIS_LayerManager *pLManager = NULL;
			POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
			while(pos!= NULL)
			{
				pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
				if(pLManager != NULL)
				{
					POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
					IGIS_Layer *pLayer = NULL;	
					unsigned short    nKey;
					while(pos1 != NULL)
					{
						pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
						if(pLayer != NULL)
						{
							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( 0, pOList );
							if( !pOList )
								return FALSE;
							QueryByAnnotation(pReturnFeaSet, pOList, strAnno);
						}
					}
				}
			}
		return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

}*/

BOOL CGIS_QueryByAtt::QueryByAnnotation(CPtrList& pReturnFeaSet,UINT LayID,unsigned short ClassID,swString strAnno)
{
    TRACK_FUNCTION("QueryByAnnotation 4");
	if(m_pMap != NULL)
	{

		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
			for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
			{
//				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapList()[i];
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID2( i );
				if( pBMap != NULL )
				{
//					m_pMap->LoadBreadthMap( pBMap );
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							IGIS_Layer *pLayer = NULL;	
							pLManager->GetLayerListMap()->Lookup( LayID, pLayer );
							if(pLayer != NULL )
							{
								CGIS_MapLayer *pBLList[1];

								pBLList[0] = (CGIS_MapLayer*)pLayer;
								if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
									return FALSE;
//								pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );

								CPtrList *pOList = NULL;
								pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
								if( !pOList )
								{
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
									return FALSE;
								}
								if(ClassID == 0)
									QueryByAnnotation(pReturnFeaSet,pOList,strAnno);
								else
								{
									POSITION pos1 = pOList->GetHeadPosition();
									CGIS_Feature*  pTempFea;				
									while( pos1 )
									{
										pTempFea = (CGIS_Feature*)(pOList->GetNext(pos1));
							
										if (pTempFea->GetObjType() == 0 )
										{
											CGIS_FeaturePoint* pTempFeaPoint = (CGIS_FeaturePoint*)pTempFea;										
											if (pTempFeaPoint->GetPointObjectType() == ClassID)
//											if (GetPoiID(pTempFeaPoint->GetPointObjectType(), LayID) == ClassID)
											{
												swString strTempFeaAnno;// = pTempFeaPoint->GetAnnotation();
												CharToUnicode( pTempFeaPoint->GetAnnotation() , &strTempFeaAnno );
												if(strTempFeaAnno.GetLength() >= strAnno.GetLength())
												{
													if( strTempFeaAnno.Find(strAnno)!=-1 )
													{
														pReturnFeaSet.AddTail(pTempFeaPoint->Clone());
													}
												}												
											}
											
										}
										else
										{
											QueryByAnnotation(pReturnFeaSet,pOList,strAnno);
											break;
										}
									}									
								}
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
							}
						}
					}
//					m_pMap->UnLoadBreadthMap(pBMap);
				}
			}
		}

		if(m_pMap->GetMapFileType() == EN_MFTYPE_GUIDE30)
		{
			CGIS_LayerManager *pLManager = NULL;
			POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
			while(pos!= NULL)
			{
				pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
				if(pLManager != NULL)
				{
					IGIS_Layer *pLayer = NULL;	
					pLManager->GetLayerListMap()->Lookup( LayID, pLayer );
					if( pLayer )
					{
						CPtrList *pOList = NULL;
						pLayer->GetObjectListMap()->Lookup( 0, pOList );
						if( !pOList )
							return FALSE;
						QueryByAnnotation(pReturnFeaSet,pOList,strAnno);
					}

				}
			}
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
BOOL CGIS_QueryByAtt::QueryByID(CGIS_Feature** pFeature,UINT nBreathMapID,UINT nFeatureID)
{
    TRACK_FUNCTION("QueryByID 1");
	if(!m_pMap)
		return FALSE;
	if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
	{
		for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
		{
//			CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapList()[i];
			CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID2( i );
			if( pBMap != NULL && pBMap->m_nBreadthID == nBreathMapID)
			{
//				m_pMap->LoadBreadthMap( pBMap );
				CGIS_LayerManager *pLManager = NULL;
				POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
				while(pos!= NULL)
				{
					pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
					if(pLManager)
					{
						IGIS_Layer *pLayer = NULL;	
						short nKey;
						POSITION pss = pLManager->GetLayerListMap()->GetStartPosition();
						while(pss)
						{
							CGIS_MapLayer *pBLList[1];

							pLManager->GetLayerListMap()->GetNextAssoc(pss,nKey,pLayer);
							pBLList[0] = (CGIS_MapLayer*)pLayer;
							if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
								return FALSE;
//							pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );

							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
							if( !pOList )
							{
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								return FALSE;
							}
							POSITION pos1 = pOList->GetHeadPosition();
							CGIS_Feature*  pTempFea;				
							while( pos1 )
							{
								pTempFea = (CGIS_Feature*)(pOList->GetNext(pos1));
								if(pTempFea->GetObjType() == EN_GOTYPE_RUT)
								{
									CGIS_FeatureRut* pFeatureRut = (CGIS_FeatureRut*)pTempFea;
									if(pFeatureRut->GetRutID() == nFeatureID)
									{
										*pFeature = (CGIS_Feature*)pFeatureRut->Clone();
										{
											pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
											return TRUE;
										}
									}
								}
							}									
							pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
						}
					}
				}
//				m_pMap->UnLoadBreadthMap(pBMap);
			}
		}
	}
	return FALSE;
}
BOOL CGIS_QueryByAtt::QueryByID(CGIS_Feature** pFeature,UINT nBreathMapID,UINT nLayID,UINT nFeatureID)
{
    TRACK_FUNCTION("QueryByID 2");
	if(!m_pMap)
		return FALSE;
	if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
	{
		for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
		{
//			CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapList()[i];
			CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID2( i );
			if( pBMap != NULL && pBMap->m_nBreadthID == nBreathMapID)
			{
//				m_pMap->LoadBreadthMap( pBMap );
				CGIS_LayerManager *pLManager = NULL;
				POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
				while(pos!= NULL)
				{
					pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
					if(pLManager)
					{
						IGIS_Layer *pLayer;	
						pLManager->GetLayerListMap()->Lookup(nLayID,pLayer);
						if(pLayer)
						{
							CGIS_MapLayer *pBLList[1];

							pBLList[0] = (CGIS_MapLayer*)pLayer;
							if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) ) 
								return FALSE;
//							pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );

							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
							if( !pOList )
							{
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								return FALSE;
							}
							POSITION pos1 = pOList->GetHeadPosition();
							CGIS_Feature*  pTempFea;				
							while( pos1 )
							{
								pTempFea = (CGIS_Feature*)(pOList->GetNext(pos1));
								if(pTempFea->GetObjType() == EN_GOTYPE_RUT)
								{
									CGIS_FeatureRut* pFeatureRut = (CGIS_FeatureRut*)pTempFea;
									if(pFeatureRut->GetRutID() == nFeatureID)
									{
										*pFeature = (CGIS_Feature*)pFeatureRut->Clone();
										pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
										return TRUE;
									}
								}
							}
							pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
						}
					}
				}
//				m_pMap->UnLoadBreadthMap(pBMap);
			}
		}
	}
	return FALSE;
}
void CGIS_QueryByAtt::SetbStopSearch( BOOL bStopSearch )
{
    TRACK_FUNCTION("SetbStopSearch 1");
	m_bStopSearch = bStopSearch;
}
BOOL CGIS_QueryByAtt::GetLayerNIN(CPtrList& pReturnFeaSet, swString strMapPath,
								  int nCityID, int nLayerID,int nPostID,int nMax, 
								  int nClassID,swString strFind)
{
    TRACK_FUNCTION("GetLayerNIN 1");
	m_bStopSearch = FALSE;
	if( gHAFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.NIN", strMapPath );
		gHAFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHAFile)
			return FALSE;
	}
	char *pFBuf = NULL;
	TRY
	{
		CFile file(gHAFile);
		file.SeekToBegin();
		char szInfo[32];

		if (9 != file.Read( szInfo, 9 ))
			return FALSE;
		
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		int nFSize = 0;
		int nUnCom = 5;
		int nIndexPOILeng  = 0;
		int nIndexRoadLeng = 0;

		unsigned int nSize = sizeof(int);
		if (nSize != file.Read( &nMNum, nSize ))
			return FALSE;

		BOOL bFind = FALSE;
		for( int i=0;	i<nMNum;	i++ )
		{
			if (nSize != file.Read( &nMID, nSize ))
				return FALSE;
			if (nSize != file.Read( &nMOff, nSize ))
				return FALSE;
			if (nSize != file.Read( &nFSize, nSize ))
				return FALSE;
			if (nSize != file.Read( &nUnCom, nSize ))
				return FALSE;
			if (nSize != file.Read( &nIndexPOILeng, nSize ))
				return FALSE;
			if (nSize != file.Read( &nIndexRoadLeng, nSize ))
				return FALSE;
			if( nMID == nCityID )
			{
				bFind = TRUE;
				break;
			}
		}
		if( !bFind )
			return 0;
		
		file.Seek( nMOff, CFile::begin );
//		file.Read( &nUnCom, sizeof(int) );
//		int nCom = nFSize - sizeof( int );
		pFBuf = new char[nUnCom];
		if( pFBuf == 0 )
			return 0;
		char *pBuf = new char[nFSize];
		if( pBuf == 0 )
			return 0;
		file.Read( pBuf, nFSize );
		DWORD dwSize = nUnCom;
		uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nFSize );
		delete [] pBuf;
		DWORD dwStation = nIndexPOILeng + nIndexRoadLeng;

		Fread( pFBuf, dwSize, dwStation, szInfo, 3, 1 );
		swString strszInfo;
		szInfo[3] = '\0';
		CharToUnicode( szInfo , &strszInfo );
		if( strszInfo.Find( L"NIN" ) < 0 )
		{
			delete[] pFBuf;
			return FALSE;
		}

		//查询结果的计数器
		int nPOICount = 0;
		//是否已经结束查询
		bFind = FALSE;
		int nPos = dwStation;
		for(int nID = 0 ; nID < 24 ; nID++)
		{
			dwStation = nPos;
			dwStation = dwStation + nID * 16;
			int nOffset,nCount;
			Fread( pFBuf, dwSize, dwStation, &nOffset, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
			
			
			if ( dwSize > dwStation )
			{
				dwStation = nOffset + nIndexPOILeng + nIndexRoadLeng ;
				
			
				int   nCharNumber;
				int  BandId = 0;
				BYTE  LayerId = 0;
				BYTE  ClassId = 0;
				BYTE  nExit = 0;
				for (int i=0; i<nCount; i++)
				{			
					Fread( pFBuf, dwSize, dwStation, &nCharNumber, sizeof(int), 1 );
					char *szAnno = NULL;
					if( nCharNumber > 0 )
					{
						szAnno = new char[nCharNumber+1];
						Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nCharNumber, 1 );
						szAnno[nCharNumber] = '\0';				
					}
					Fread(pFBuf, dwSize, dwStation, &BandId, sizeof(int), 1);
					Fread( pFBuf,dwSize,dwStation,&LayerId,sizeof(BYTE),1 ); 
					Fread( pFBuf,dwSize,dwStation,&ClassId,sizeof(BYTE),1 ); 
					swString strTemp;// = (swString)szAnno;
					CharToUnicode( szAnno , &strTemp );
					if(  (nMax == 0 || nPOICount < nMax ) 	)
					{
						if ( (LayerId == nLayerID || nLayerID == -1 ) && 
							(nClassID == 0 || nClassID == ClassId)
							&& (strFind.IsEmpty() || strTemp.Find(strFind) != -1 ) )
						{
							ST_INDEX_NAME  *index = new ST_INDEX_NAME;
							index->Name = szAnno;
							index->BandID = BandId;
							index->LayerID = LayerId;
							index->ClassID = ClassId;
							nPOICount++;
							if (BandId != 0)
							{
								if( nLayerID != LAYER_PT_END-1 )
									pReturnFeaSet.AddTail( index );
								//政府机构要按等级排序
								else							
								{
									BOOL bFindPosition = FALSE;
									ST_INDEX_NAME  *indexTemp = NULL;
									POSITION posStation = NULL;
									POSITION posTemp = pReturnFeaSet.GetHeadPosition();
									while( posTemp )
									{
										posStation = posTemp;
										indexTemp = (ST_INDEX_NAME*)pReturnFeaSet.GetNext( posTemp );
										if( indexTemp->ClassID >= ClassId )
										{
											pReturnFeaSet.InsertBefore( posStation , index );
											bFindPosition = TRUE;
											break;
										}
									}
									if( !bFindPosition )
										pReturnFeaSet.AddTail( index );

								}

								if (nExit == 0) 
									nExit = 1;	
							}
							else
							{
								delete[] szAnno;
								delete index;
							}
						}
						else
						{
							delete[] szAnno;
						}
					}
					else
					{
						bFind = TRUE;
						break;
					}
					if( m_bStopSearch )
						break;
				}			
			}
			if( bFind )
				break;
			if( m_bStopSearch )
				break;
		}
		delete[] pFBuf;
	}
	CATCH( CMemoryException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
		if(INVALID_HANDLE_VALUE != gHAFile)
			CloseHandle( gHAFile );
		gHAFile = INVALID_HANDLE_VALUE;
//		THROW( e );
		return FALSE;
	}
	AND_CATCH( CException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	return TRUE;
}
BOOL CGIS_QueryByAtt::GetClassNIN(CPtrList& pReturnFeaSet,CPtrList* pFeaSet, swString strMapPath,int nClassID)
{
    TRACK_FUNCTION("GetClassNIN 1");
	POSITION pos = pFeaSet->GetHeadPosition();
	ST_INDEX_NAME *pIndex;
	while(pos)
	{
		pIndex = (ST_INDEX_NAME*)pFeaSet->GetNext(pos);
		if(pIndex->ClassID == nClassID)
		{
			ST_INDEX_NAME *pTemp = new ST_INDEX_NAME;
			pTemp->BandID = pIndex->BandID;
			pTemp->ClassID = pIndex->ClassID;
			pTemp->LayerID = pIndex->LayerID;
			int nNum = strlen( pIndex->Name );
			pTemp->Name = new char[nNum+1];
			memcpy( pTemp->Name , pIndex->Name,nNum );
			pTemp->Name[nNum] = '\0';
//			pTemp->Name = pIndex->Name;
			pReturnFeaSet.AddTail(pTemp);
		}
	}
	return TRUE;
}
BOOL CGIS_QueryByAtt::GetNINByName(CPtrList& pReturnFeaSet,CPtrList* pFeaSet, swString strName)
{
    TRACK_FUNCTION("GetNINByName 1");
	if(pFeaSet == NULL)
	{
		return FALSE;
	}
	swString strTempAnno = (swString)strName;
    POSITION pos = pFeaSet->GetHeadPosition();
	ST_INDEX_NAME* pTempFea;
	while(pos)
	{
		pTempFea = (ST_INDEX_NAME*)(pFeaSet->GetNext(pos));
		if(pTempFea)
		{
			swString strTempFeaAnno;// = (swString)pTempFea->Name;
			CharToUnicode( pTempFea->Name , &strTempFeaAnno );
			if(strTempFeaAnno.GetLength() >= strTempAnno.GetLength())
			{
				if( strTempFeaAnno.Find(strTempAnno)!=-1 )
				{
					ST_INDEX_NAME *pTemp = new ST_INDEX_NAME;
					pTemp->BandID = pTempFea->BandID;
					pTemp->ClassID = pTempFea->ClassID;
					pTemp->LayerID = pTempFea->LayerID;
					int nNum = strlen( pTempFea->Name );
					pTemp->Name = new char[nNum+1];
					memcpy( pTemp->Name , pTempFea->Name,nNum );
					pTemp->Name[nNum] = '\0';

					pReturnFeaSet.AddTail(pTemp);
				}
			}
		}
	}
	return TRUE;
}
BOOL CGIS_QueryByAtt::GetByNameAndLay(CPtrList& pReturnFeaSet, swString strMapPath, int nMapID, swString strName, int nLayerID)
{
    TRACK_FUNCTION("GetByNameAndLay 1");

    if( gHAFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.NIN", strMapPath );
		gHAFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHAFile)
			return FALSE;
	}
	char *pFBuf = NULL;
	TRY
	{
		CFile file(gHAFile);
		file.SeekToBegin();
		char szInfo[32];

		if (9 != file.Read( szInfo, 9 ))
			return FALSE;

		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		int nFSize = 0;
		int nUnCom = 5;
		int nIndexPOILeng  = 0;
		int nIndexRoadLeng = 0;

		unsigned int nSize = sizeof(int);
		if (nSize != file.Read( &nMNum, nSize ))
			return FALSE;
		
		BOOL bFind = FALSE;
		for( int i=0;	i<nMNum;	i++ )
		{
			if (nSize != file.Read( &nMID, nSize ))
				return FALSE;
			if (nSize != file.Read( &nMOff, nSize ))
				return FALSE;
			if (nSize != file.Read( &nFSize, nSize ))
				return FALSE;
			if (nSize != file.Read( &nUnCom, nSize ))
				return FALSE;
			if (nSize != file.Read( &nIndexPOILeng, nSize ))
				return FALSE;
			if (nSize != file.Read( &nIndexRoadLeng, nSize ))
				return FALSE;
			if( nMID == nMapID )
			{
				bFind = TRUE;
				break;
			}
		}
		if( !bFind )
			return 0;
		
		file.Seek( nMOff, CFile::begin );
//		file.Read( &nUnCom, sizeof(int) );
//		int nCom = nFSize - sizeof( int );
		pFBuf = new char[nUnCom];
		if( pFBuf == 0 )
			return 0;
		char *pBuf = new char[nFSize];
		if( pBuf == 0 )
			return 0;
		file.Read( pBuf, nFSize );
		DWORD dwSize = nUnCom;
		uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nFSize );
		delete [] pBuf;
		DWORD dwStation = nIndexPOILeng + nIndexRoadLeng;

		Fread( pFBuf, dwSize, dwStation, szInfo, 3, 1 );
		swString strszInfo;
		szInfo[3] = '\0';
		CharToUnicode( szInfo , &strszInfo );
		if( strszInfo.Find( L"NIN" ) < 0 )
		{
			delete[] pFBuf;
			return FALSE;
		}

		int nPos = dwStation;
		for(int nID = 0 ; nID < 24 ; nID++)
		{
			dwStation = nPos;
			dwStation = dwStation + nID * 16;
			int nOffset,nCount;
			Fread( pFBuf, dwSize, dwStation, &nOffset, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
			
			
			if ( dwSize > dwStation )
			{
				dwStation = nOffset + nIndexPOILeng + nIndexRoadLeng ;
			
				int   nCharNumber;
				int  BandId = 0;
				BYTE  LayerId = 0;
				BYTE  ClassId = 0;
				BYTE  nExit = 0;
				for (int i=0; i<nCount; i++)
				{			
					Fread( pFBuf, dwSize, dwStation, &nCharNumber, sizeof(int), 1 );
					char *szAnno = NULL;
					if( nCharNumber > 0 )
					{
						szAnno = new char[nCharNumber+1];
						Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nCharNumber, 1 );
						szAnno[nCharNumber] = '\0';				
					}
					Fread(pFBuf, dwSize, dwStation, &BandId, sizeof(int), 1);
					Fread( pFBuf,dwSize,dwStation,&LayerId,sizeof(BYTE),1 ); 
					Fread( pFBuf,dwSize,dwStation,&ClassId,sizeof(BYTE),1 ); 
					swString szName ;//= (swString)szAnno;
					CharToUnicode( szAnno , &szName );
					if (LayerId == nLayerID  && szName.Find(strName) != -1)
					{
						ST_INDEX_NAME  *index = new ST_INDEX_NAME;
						index->Name = szAnno ;
						index->BandID = BandId;
						index->LayerID = LayerId;
						index->ClassID = ClassId;
						if (BandId != 0)
						{
							pReturnFeaSet.AddTail(index);
							if (nExit == 0) 
								nExit = 1;	
						}
						else
						{
							delete[] index->Name;
							delete index;
						}
					}
					else
						delete[] szAnno;
				}			
			}
		}
		delete[] pFBuf;
	}
	CATCH( CMemoryException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
		if(INVALID_HANDLE_VALUE != gHAFile)
			CloseHandle( gHAFile );
		gHAFile = INVALID_HANDLE_VALUE;
//		THROW( e );
		return FALSE;
	}
	AND_CATCH( CException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	return TRUE;
}
BOOL CGIS_QueryByAtt::GetByClaAndLay(CPtrList& pReturnFeaSet, swString strMapPath, int nMapID, int nLayerID,int nClassID)
{
    TRACK_FUNCTION("GetByClaAndLay 1");
	if( gHAFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.NIN", strMapPath );
		gHAFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHAFile)
			return FALSE;
	}
	char *pFBuf = NULL;
	TRY
	{
		CFile file(gHAFile);
		file.SeekToBegin();
		char szInfo[32];

		if (9 != file.Read( szInfo, 9 ))
			return FALSE;

		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		int nFSize = 0;
		int nUnCom = 5;
		int nIndexPOILeng  = 0;
		int nIndexRoadLeng = 0;

		unsigned int nSize = sizeof(int);
		if (nSize != file.Read( &nMNum, nSize ))
			return FALSE;

		BOOL bFind = FALSE;
		for( int i=0;	i<nMNum;	i++ )
		{
			if (nSize != file.Read( &nMID, nSize ))
				return FALSE;
			if (nSize != file.Read( &nMOff, nSize ))
				return FALSE;
			if (nSize != file.Read( &nFSize, nSize ))
				return FALSE;
			if (nSize != file.Read( &nUnCom, nSize ))
				return FALSE;
			if (nSize != file.Read( &nIndexPOILeng, nSize ))
				return FALSE;
			if (nSize != file.Read( &nIndexRoadLeng, nSize ))
				return FALSE;
			if( nMID == nMapID )
			{
				bFind = TRUE;
				break;
			}
		}
		if( !bFind )
			return 0;
		
		file.Seek( nMOff, CFile::begin );
//		file.Read( &nUnCom, sizeof(int) );
//		int nCom = nFSize - sizeof( int );
		pFBuf = new char[nUnCom];
		if( pFBuf == 0 )
			return 0;
		char *pBuf = new char[nFSize];
		if( pBuf == 0 )
			return 0;
		file.Read( pBuf, nFSize );
		DWORD dwSize = nUnCom;
		uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nFSize );
		delete [] pBuf;
		DWORD dwStation = nIndexPOILeng + nIndexRoadLeng;

		Fread( pFBuf, dwSize, dwStation, szInfo, 3, 1 );
		swString strszInfo;
		szInfo[3] = '\0';
		CharToUnicode( szInfo , &strszInfo );
		if( strszInfo.Find( L"NIN" ) < 0 )
		{
			delete[] pFBuf;
			return FALSE;
		}

		int nPos = dwStation;
		for(int nID = 0 ; nID < 24 ; nID++)
		{
			dwStation = nPos;
			dwStation = dwStation + nID * 16;
			int nOffset,nCount;
			Fread( pFBuf, dwSize, dwStation, &nOffset, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
			
			
			if ( dwSize > dwStation )
			{
				dwStation = nOffset + nIndexPOILeng + nIndexRoadLeng ;
			
				int   nCharNumber;
				int  BandId = 0;
				BYTE  LayerId = 0;
				BYTE  ClassId = 0;
				BYTE  nExit = 0;
				for (int i=0; i<nCount; i++)
				{			
					Fread( pFBuf, dwSize, dwStation, &nCharNumber, sizeof(int), 1 );
					char *szAnno = NULL;
					if( nCharNumber > 0 )
					{
						szAnno = new char[nCharNumber+1];
						Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nCharNumber, 1 );
						szAnno[nCharNumber] = '\0';				
					}
					Fread(pFBuf, dwSize, dwStation, &BandId, sizeof(int), 1);
					Fread( pFBuf,dwSize,dwStation,&LayerId,sizeof(BYTE),1 ); 
					Fread( pFBuf,dwSize,dwStation,&ClassId,sizeof(BYTE),1 ); 
					if (LayerId == nLayerID  && nClassID == ClassId)
					{
						ST_INDEX_NAME  *index = new ST_INDEX_NAME;
						index->Name = szAnno ;
						index->BandID = BandId;
						index->LayerID = LayerId;
						index->ClassID = ClassId;
						if (BandId != 0)
						{
							pReturnFeaSet.AddTail(index);
							if (nExit == 0) 
								nExit = 1;	
						}
						else
						{
							delete[] index->Name;
							delete index;
						}
					}
					else
						delete[] szAnno;
				}			
			}
		}
		delete[] pFBuf;
	}
	CATCH( CMemoryException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
		if(INVALID_HANDLE_VALUE != gHAFile)
			CloseHandle( gHAFile );
		gHAFile = INVALID_HANDLE_VALUE;
//		THROW( e );
		return FALSE;
	}
	AND_CATCH( CException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	return TRUE;
}
int CGIS_QueryByAtt::GetInputHead( CPtrList& pReturnFeaSet, int nCityID, char* InputHead, 
								  int nInputCount,BYTE ntype ,int nBigClass,int nSmallClass)
{
	if( !m_pMap )
		return FALSE;
	swString strMapPath = m_pMap->GetMapInfo()->GetMapFilePath();

	if( gHAFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.NIN", strMapPath );
		gHAFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHAFile)
			return 0;
	}
	char *pFBuf = NULL;
	TRY
	{
		CFile file(gHAFile);
		file.SeekToBegin();
		char szInfo[32];
		file.Read( szInfo, 9 );
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		int nFSize = 0;
		int nUnCom = 5;
		int nIndexPOILeng  = 0;
		int nIndexRoadLeng = 0;
		file.Read( &nMNum, sizeof(int) );
		BOOL bFind = FALSE;
		for( int i=0;	i<nMNum;	i++ )
		{
			file.Read( &nMID, sizeof(int) );
			file.Read( &nMOff, sizeof(int) );
			file.Read( &nFSize, sizeof(int) );
			file.Read( &nUnCom, sizeof(int) );
			file.Read( &nIndexPOILeng, sizeof(int) );
			file.Read( &nIndexRoadLeng, sizeof(int) );
			if( nMID == nCityID )
			{
				bFind = TRUE;
				break;
			}
		}
		if( !bFind )
			return 0;
		
		file.Seek( nMOff, CFile::begin );
		pFBuf = new char[nUnCom];
		if( pFBuf == 0 )
			return 0;
		char *pBuf = new char[nFSize];
		if( pBuf == 0 )
			return 0;
		file.Read( pBuf, nFSize );
		DWORD dwSize = nUnCom;
		uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nFSize );
		delete [] pBuf;
		DWORD dwStation = 0;
		//遍历简拼索引
		int nIndexLeng;
		if( ntype == 1 )
			nIndexLeng = nIndexPOILeng;
		else
		{
			dwStation += nIndexPOILeng;
			nIndexLeng = nIndexRoadLeng+nIndexPOILeng;
		}
		BOOL bMath = FALSE;
		int nDataOff = nIndexRoadLeng+nIndexPOILeng;
		int nTempOff;
		Fread( pFBuf, dwSize, dwStation, &nTempOff, sizeof(int), 1 );
		DWORD dwStart = dwStation;
		DWORD dwEnd = dwStation;
		int nMaxCount = 0;
		//记录上一次查询结果的偏移值
		DWORD dwLastOffset = 0;
		while( dwStation+nInputCount < nIndexLeng )
		{
			if( !bMath )
			{
				bMath = TRUE;
				for( int nTemp = 0 ; nTemp < nInputCount ; nTemp++ )
				{
					if( pFBuf[dwStation+nTemp] != InputHead[nTemp] )
					{
						bMath = FALSE;
						break;
					}
				}
				if( bMath )
					dwStation+=(nInputCount-1);
			}
			dwStation++;
			if( pFBuf[dwStation-1] == '\0')
			{
				dwEnd = dwStation-1;
				if( bMath )
				{
					ST_PYINDEX_NAME* pPyIndex = new ST_PYINDEX_NAME;
//					char* pCharTemp = new char[dwEnd-dwStart];
//					memcpy( pCharTemp,&pFBuf[dwStart],dwEnd-dwStart );
					CharToUnicode( &pFBuf[dwStart] , &pPyIndex->strPYName );
					//真实数据的偏移
					DWORD dwRealDate = nDataOff + nTempOff;
					int nCharNumber;
					Fread( pFBuf, dwSize, dwRealDate, &nCharNumber, sizeof(int), 1 );
					char *szAnno = NULL;
					if( nCharNumber > 0 )
					{
						szAnno = new char[nCharNumber+1];
						Fread( pFBuf, dwSize, dwRealDate, szAnno, sizeof(char)*nCharNumber, 1 );
						szAnno[nCharNumber] = '\0';				
					}
					int BandId;
					BYTE LayerId , ClassId;
					Fread(pFBuf, dwSize, dwRealDate, &BandId, sizeof(int), 1);
					Fread( pFBuf,dwSize,dwRealDate,&LayerId,sizeof(BYTE),1 ); 
					Fread( pFBuf,dwSize,dwRealDate,&ClassId,sizeof(BYTE),1 ); 
					pPyIndex->Name = szAnno;
					pPyIndex->BandID = BandId;
					pPyIndex->LayerID = LayerId;
					pPyIndex->ClassID = ClassId;
					ST_PYINDEX_NAME* pPOITemp = NULL;
					if( ntype == 1 )
					{
						if( dwLastOffset != dwRealDate  )
						{
							BOOL bAddIndex = TRUE;
							//分类简拼检索
							if( nBigClass != -1 )
							{
								if( LayerId != nBigClass 
									|| ( LayerId==nBigClass&&nSmallClass!=0&&nSmallClass!=ClassId ) )
								{
									if( szAnno )
									{
										delete[] szAnno;
										szAnno = NULL;
									}
									if( pPyIndex )
									{
										delete pPyIndex;
										pPyIndex = NULL;
									}
									bAddIndex = FALSE;
								}
							}
							if( bAddIndex )
							{
								pReturnFeaSet.AddTail(pPyIndex);
								dwLastOffset = dwRealDate;
								nMaxCount++;
							}
						}
						else//解决多音字问题
						{
							pPOITemp = (ST_PYINDEX_NAME*)pReturnFeaSet.GetTail();
							pPOITemp->strPYName += _T(",");
							pPOITemp->strPYName += pPyIndex->strPYName;
							if( szAnno )
							{
								delete[] szAnno;
								szAnno = NULL;
							}
							if( pPyIndex )
							{
								delete pPyIndex;
								pPyIndex = NULL;
							}
						}
					}
					else
					{
						if( dwLastOffset != dwRealDate  )
						{
							BOOL bFindIndex = TRUE;
							POSITION posReturn = pReturnFeaSet.GetHeadPosition();
							while( posReturn )
							{
								pPOITemp = (ST_PYINDEX_NAME*)pReturnFeaSet.GetNext(posReturn);
								if( pPOITemp && (swString)(pPOITemp->Name) == (swString)szAnno )
								{
									if( szAnno )
									{
										delete[] szAnno;
										szAnno = NULL;
									}
									if( pPyIndex )
									{
										delete pPyIndex;
										pPyIndex = NULL;
									}
									bFindIndex = FALSE;
									break;
								}
							}
							if( bFindIndex )
							{
								BOOL bAddIndex = TRUE;
								//分类简拼检索
								if( nBigClass != -1 )
								{
									if( LayerId != nBigClass 
										|| ( LayerId==nBigClass&&nSmallClass!=0&&nSmallClass!=ClassId ) )
									{
										if( szAnno )
										{
											delete[] szAnno;
											szAnno = NULL;
										}
										if( pPyIndex )
										{
											delete pPyIndex;
											pPyIndex = NULL;
										}
										bAddIndex = FALSE;
									}
								}
								if( bAddIndex )
								{
									pReturnFeaSet.AddTail(pPyIndex);
									dwLastOffset = dwRealDate;
									nMaxCount++;
								}
							}
						}
						else//解决多音字问题
						{
							pPOITemp = (ST_PYINDEX_NAME*)pReturnFeaSet.GetTail();
							pPOITemp->strPYName += _T(",");
							pPOITemp->strPYName += pPyIndex->strPYName;
							if( szAnno )
							{
								delete[] szAnno;
								szAnno = NULL;
							}
							if( pPyIndex )
							{
								delete pPyIndex;
								pPyIndex = NULL;
							}
						}
							
					}
					bMath = FALSE;
				}
				Fread( pFBuf, dwSize, dwStation, &nTempOff, sizeof(int), 1 );
				dwStart = dwStation;
			}
			if( nMaxCount == 300 )
			{
				delete[] pFBuf;
				return 2; 
			}
		}
		delete[] pFBuf;
	}
	CATCH( CMemoryException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return 0;
	}
	AND_CATCH( CFileException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
		if(INVALID_HANDLE_VALUE != gHAFile)
			CloseHandle( gHAFile );
		gHAFile = INVALID_HANDLE_VALUE;
//		THROW( e );
		return 0;
	}
	AND_CATCH( CException, e )
	{
		if( pFBuf )
			delete[] pFBuf;
//		THROW( e );
		return 0;
	}
	END_CATCH
	return 1;
}
CPtrList* CGIS_QueryByAtt::GetLevelList()
{
	if( m_pListLevel->GetCount() == 0  )
		InitRichPOILevel();
	return m_pListLevel;
}
BOOL CGIS_QueryByAtt::InitRichPOILevel()
{
	if( !m_pMap )
		return FALSE;
	if( m_pListLevel->GetCount() > 0 )
		return TRUE;
	swString strMapPath = m_pMap->GetMapInfo()->GetMapFilePath();

	if( gRichPOIFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sautomap.chr", strMapPath );
		gRichPOIFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gRichPOIFile)
			return FALSE;
	}
	DWORD dwLevelIdxPos;
	int  nLevel1Cnt = 0;
	TRY
	{
		char szInfo[60];
		//获得类别索引的位置
		CFile file( gRichPOIFile );
		SetFilePointer(gRichPOIFile,sizeof(RICHPOI_FILEHEAD)+sizeof(DWORD),NULL,FILE_BEGIN);
		file.Read(&dwLevelIdxPos,sizeof(DWORD));

		//获取一级类别个数
		SetFilePointer(gRichPOIFile,dwLevelIdxPos,NULL,FILE_BEGIN);
		file.Read(&nLevel1Cnt,sizeof(DWORD));

		for (int iLev1=0; iLev1 < nLevel1Cnt; iLev1++)
		{
			SetFilePointer(gRichPOIFile,dwLevelIdxPos+4+iLev1*44,NULL,FILE_BEGIN);
			RICHPOI_LEVEL1* info = new RICHPOI_LEVEL1;

			int nOffset2,nLevel2Cnt;

			file.Read(&info->dwLevel1ID,sizeof(DWORD));
			file.Read(szInfo,32*sizeof(char));
			CharToUnicode( szInfo , &info->strLevel1Name );
			file.Read(&nOffset2, sizeof(DWORD));
			file.Read(&nLevel2Cnt, sizeof(DWORD));

			m_pListLevel->AddTail(info);
			
			for( int iLev2=0;iLev2<nLevel2Cnt;iLev2++ )
			{
				SetFilePointer(gRichPOIFile,nOffset2+iLev2*44,NULL,FILE_BEGIN);	
				RICHPOI_LEVEL2* info2 = new RICHPOI_LEVEL2;
				int nOffset3,nLevel3Cnt;
				file.Read(&info2->dwLevel2ID, sizeof(DWORD));
				file.Read(szInfo, 32*sizeof(char));
				CharToUnicode( szInfo , &info2->strLevel2Name );
				file.Read(&nOffset3, sizeof(DWORD));
				file.Read(&nLevel3Cnt, sizeof(DWORD));

				info->pLevel2List.AddTail( info2 );

				SetFilePointer(gRichPOIFile,nOffset3,NULL,FILE_BEGIN);
				for( int iLev3=0;iLev3<nLevel3Cnt;iLev3++ )
				{
					RICHPOI_LEVEL3* info3 = new RICHPOI_LEVEL3;

					file.Read(&info3->dwLevel3ID,sizeof(DWORD));
					file.Read(szInfo, 32*sizeof(char));
					CharToUnicode( szInfo , &info3->strLevel3Name );
					file.Read(&info3->dwDataPackageOffset, sizeof(DWORD));
					file.Read(&info3->dwDataPackageCnt, sizeof(DWORD));
					
					info2->pLevel3List.AddTail( info3 );
					
				}				

			}
			
		}
	}
	CATCH( CMemoryException, e )
	{
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
		return FALSE;
	}
	AND_CATCH( CException, e )
	{
		return FALSE;
	}
	END_CATCH		
	return TRUE;
}
BOOL CGIS_QueryByAtt::GetRichPOIByCityID( CPtrList& pReturnFeaSet, int nType, int nCityID )
{
	if( !InitRichPOILevel() )
		return FALSE;

	RICHPOI_LEVEL1* pRichPOILevel1;
	RICHPOI_LEVEL2* pRichPOILevel2;
	RICHPOI_LEVEL3* pRichPOILevel3;

	POSITION pos1 = m_pListLevel->GetHeadPosition();
	while( pos1 )
	{
		pRichPOILevel1 = (RICHPOI_LEVEL1*)m_pListLevel->GetNext( pos1 );
		//确定大类
		if( pRichPOILevel1 && pRichPOILevel1->dwLevel1ID == nType )
		{
			POSITION pos2 = pRichPOILevel1->pLevel2List.GetHeadPosition();
			while( pos2 )
			{
				pRichPOILevel2 = (RICHPOI_LEVEL2*)( pRichPOILevel1->pLevel2List.GetNext(pos2) );
				if( pRichPOILevel2 )
				{
					POSITION pos3 = pRichPOILevel2->pLevel3List.GetHeadPosition();
					while( pos3 )
					{
						pRichPOILevel3 = (RICHPOI_LEVEL3*)( pRichPOILevel2->pLevel3List.GetNext( pos3 ) );
						//确定城市
						if( pRichPOILevel3 && pRichPOILevel3->dwLevel3ID == nCityID )
						{
							//////////////////////////////////////////////////////////////////////////
							BYTE *pbCAddr_Name = NULL;
							BYTE *pbUCAddr_Name = NULL;
							BYTE *pbCAddr_Data = NULL;
							BYTE *pbUCAddr_Data = NULL;
							TRY
							{

								char szInfo[60];
								CFile file( gRichPOIFile );
								DWORD dwRichPoiCnt,dwRichPoiNameOffset,dwRichPoiNameLenUZip,dwRichPoiDataOffset
									,dwRichPoiDataLen,dwRichPoiDataLenUZip,dwRichPoiNameLen;
								for(int j=0;j<(int)pRichPOILevel3->dwDataPackageCnt;j++)
								{
									SetFilePointer(gRichPOIFile,pRichPOILevel3->dwDataPackageOffset+28*j,
										NULL,FILE_BEGIN);

									file.Read(&dwRichPoiCnt,sizeof(DWORD));
									file.Read(&dwRichPoiNameOffset,sizeof(DWORD));
									file.Read(&dwRichPoiNameLen, sizeof(DWORD));
									file.Read(&dwRichPoiNameLenUZip, sizeof(DWORD));
									file.Read(&dwRichPoiDataOffset, sizeof(DWORD));
									file.Read(&dwRichPoiDataLen, sizeof(DWORD));
									file.Read(&dwRichPoiDataLenUZip, sizeof(DWORD));

									int iNameLen = dwRichPoiNameLen;
									int iUZipNameLen = dwRichPoiNameLenUZip;
									int iDataLen = dwRichPoiDataLen;
									int iUZipDataLen = dwRichPoiDataLenUZip;
									pbCAddr_Name = new BYTE[iNameLen];
									pbUCAddr_Name = new BYTE[iUZipNameLen];
									pbCAddr_Data = new BYTE[iDataLen];
									pbUCAddr_Data = new BYTE[iUZipDataLen];
									memset(pbCAddr_Name,0,iNameLen);
									memset(pbUCAddr_Name,0,iUZipNameLen);
									memset(pbCAddr_Data,0,iDataLen);
									memset(pbUCAddr_Data,0,iUZipDataLen);

									SetFilePointer(gRichPOIFile,dwRichPoiNameOffset,NULL,FILE_BEGIN);
									file.Read(pbCAddr_Name, iNameLen);
									if (Z_OK != uncompress(pbUCAddr_Name, (unsigned long*)&iUZipNameLen, pbCAddr_Name, iNameLen))
									{
										delete []pbCAddr_Data;
										pbCAddr_Data = NULL;
										delete []pbUCAddr_Data;
										pbUCAddr_Data = NULL;
										delete []pbCAddr_Name;
										pbCAddr_Name = NULL;
										delete []pbUCAddr_Name;
										pbUCAddr_Name = NULL;
										return FALSE;
									}

									SetFilePointer(gRichPOIFile,dwRichPoiDataOffset,NULL,FILE_BEGIN);
									file.Read(pbCAddr_Data, iDataLen);
									if (Z_OK != uncompress(pbUCAddr_Data, (unsigned long*)&iUZipDataLen, pbCAddr_Data, iDataLen))
									{
										delete []pbCAddr_Data;
										pbCAddr_Data = NULL;
										delete []pbUCAddr_Data;
										pbUCAddr_Data = NULL;
										delete []pbCAddr_Name;
										pbCAddr_Name = NULL;
										delete []pbUCAddr_Name;
										pbUCAddr_Name = NULL;
										return FALSE;
									}

									BYTE* pPos_name = pbUCAddr_Name;
									BYTE* pPos_data = pbUCAddr_Data;

									for(int i=0;i<(int)dwRichPoiCnt;i++)
									{
										RICHPOI_INFO* info = new RICHPOI_INFO;
										//name
										int nameLen  = *pPos_name;
										pPos_name+=sizeof(BYTE);
										if( nameLen > 0 )
										{
											memcpy(szInfo,pPos_name,nameLen*sizeof(char));
											szInfo[nameLen] = '\0';
											CharToUnicode( szInfo , &info->strRichPoiName );
											pPos_name+=nameLen*sizeof(char);
										}
										else
											info->strRichPoiName.Empty();
										//data
										memcpy(&info->dwRichPOIID,pPos_data,sizeof(DWORD));
										pPos_data+=sizeof(DWORD);
										memcpy(&info->fLon,pPos_data,sizeof(float));
										pPos_data+=sizeof(float);
										memcpy(&info->fLat,pPos_data,sizeof(float));
										pPos_data+=sizeof(float);
										memcpy(&info->dwContentInfoOffset,pPos_data,sizeof(DWORD));
										pPos_data+=sizeof(DWORD);
										memcpy(&info->dwContentTxtLen,pPos_data,sizeof(DWORD));
										pPos_data+=sizeof(DWORD);
										memcpy(&info->dwContentTxtLenUZip,pPos_data,sizeof(DWORD));
										pPos_data+=sizeof(DWORD);
										memcpy(&info->iContentMediaCnt,pPos_data,sizeof(int));
										pPos_data+=sizeof(int);
										memcpy(&info->dwAlarmSpeed,pPos_data,sizeof(BYTE));
										pPos_data+=sizeof(BYTE);
										memcpy(&nameLen,pPos_data,sizeof(BYTE));
										pPos_data+=sizeof(BYTE);
										if( nameLen >0 )
										{
											memcpy(&szInfo,pPos_data,nameLen*sizeof(char));
											szInfo[nameLen] = '\0';
											CharToUnicode( szInfo , &(info->strPhoneCode) );
											pPos_data+=nameLen*sizeof(char);
										}
										else
											info->strPhoneCode.Empty();

						//				for(int i=0;i<info.iContentMediaCnt;i++)
						//				{
						//					RICHPOI_MEDIA_RECORDINFO _info;
						//					memset(&_info,0,sizeof(RICHPOI_MEDIA_RECORDINFO));
						//					_info.dwRichPOIID = info.dwRichPOIID;
						//					memcpy(&_info.iMediaType,pPos_data,sizeof(int));
						//					pPos_data+=sizeof(int);
						//					memcpy(&_info.dwMediaOffset,pPos_data,sizeof(DWORD));
						//					pPos_data+=sizeof(DWORD);
						//					memcpy(&_info.dwMediaLen,pPos_data,sizeof(DWORD));
						//					pPos_data+=sizeof(DWORD);
						//					memcpy(&_info.dwMediaLenUZip,pPos_data,sizeof(DWORD));
						//					pPos_data+=sizeof(DWORD);
						//
						//					m_listMediaInfo.AddTail(_info);
						//				}
										pReturnFeaSet.AddTail( info );


									}

									delete []pbCAddr_Data;
									pbCAddr_Data = NULL;
									delete []pbUCAddr_Data;
									pbUCAddr_Data = NULL;
									delete []pbCAddr_Name;
									pbCAddr_Name = NULL;
									delete []pbUCAddr_Name;
									pbUCAddr_Name = NULL;
										
								}

							}
							CATCH( CMemoryException, e )
							{
								if( pbCAddr_Data )
								{
									delete []pbCAddr_Data;
									pbCAddr_Data = NULL;
								}
								if( pbUCAddr_Data )
								{
									delete []pbUCAddr_Data;
									pbUCAddr_Data = NULL;
								}
								if( pbCAddr_Name )
								{
									delete []pbCAddr_Name;
									pbCAddr_Name = NULL;
								}
								if( pbUCAddr_Name )
								{
									delete []pbUCAddr_Name;
									pbUCAddr_Name = NULL;
								}
								return FALSE;
							}
							AND_CATCH( CFileException, e )
							{
								if( pbCAddr_Data )
								{
									delete []pbCAddr_Data;
									pbCAddr_Data = NULL;
								}
								if( pbUCAddr_Data )
								{
									delete []pbUCAddr_Data;
									pbUCAddr_Data = NULL;
								}
								if( pbCAddr_Name )
								{
									delete []pbCAddr_Name;
									pbCAddr_Name = NULL;
								}
								if( pbUCAddr_Name )
								{
									delete []pbUCAddr_Name;
									pbUCAddr_Name = NULL;
								}
								return FALSE;
							}
							AND_CATCH( CException, e )
							{
								if( pbCAddr_Data )
								{
									delete []pbCAddr_Data;
									pbCAddr_Data = NULL;
								}
								if( pbUCAddr_Data )
								{
									delete []pbUCAddr_Data;
									pbUCAddr_Data = NULL;
								}
								if( pbCAddr_Name )
								{
									delete []pbCAddr_Name;
									pbCAddr_Name = NULL;
								}
								if( pbUCAddr_Name )
								{
									delete []pbUCAddr_Name;
									pbUCAddr_Name = NULL;
								}
								return FALSE;
							}
							END_CATCH		

							//////////////////////////////////////////////////////////////////////////
							
							
						}

					}
				}
			}
		}
	}
	return TRUE;
}
BOOL CGIS_QueryByAtt::GetRichPOIText( RICHPOI_INFO RichPOIInfo , swString& strRichPOI )
{
	DWORD dwTxtPos=0;
	DWORD dwTxtLen=0;
	DWORD dwTxtLenUZip=0;

	dwTxtPos = RichPOIInfo.dwContentInfoOffset;
	dwTxtLen = RichPOIInfo.dwContentTxtLen;
	dwTxtLenUZip = RichPOIInfo.dwContentTxtLenUZip;
	if( RichPOIInfo.dwContentTxtLen <= 0  )
	{
		strRichPOI.Empty();
		return TRUE;
	}
	BYTE* btTxt = NULL;
	char* chTxt = NULL;

	TRY
	{

		CFile file( gRichPOIFile );

		btTxt = new BYTE[dwTxtLen];
		chTxt = new char[dwTxtLenUZip+1];
		memset(btTxt,0,dwTxtLen);
		memset(chTxt,0,dwTxtLenUZip);

		SetFilePointer(gRichPOIFile,dwTxtPos,NULL,FILE_BEGIN);
		file.Read(btTxt, dwTxtLen);
		if (Z_OK != uncompress((BYTE*)chTxt, (unsigned long*)&dwTxtLenUZip, btTxt, dwTxtLen))
		{
			delete[] btTxt;
			btTxt = NULL;
			delete[] chTxt;
			chTxt = NULL;
			return FALSE;
		}
		chTxt[dwTxtLenUZip] = '\0';
		CharToUnicode(chTxt,&strRichPOI);

		delete[] btTxt;
		btTxt = NULL;
		delete[] chTxt;
		chTxt = NULL;
	}
	CATCH( CMemoryException, e )
	{
		if( btTxt )
		{
			delete[] btTxt;
			btTxt = NULL;
		}
		if( chTxt )
		{
			delete[] chTxt;
			chTxt = NULL;
		}
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
		if( btTxt )
		{
			delete[] btTxt;
			btTxt = NULL;
		}
		if( chTxt )
		{
			delete[] chTxt;
			chTxt = NULL;
		}
		return FALSE;
	}
	AND_CATCH( CException, e )
	{
		if( btTxt )
		{
			delete[] btTxt;
			btTxt = NULL;
		}
		if( chTxt )
		{
			delete[] chTxt;
			chTxt = NULL;
		}
		return FALSE;
	}
	END_CATCH		

	return TRUE;
}

