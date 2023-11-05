// GIS_QueryByShape.cpp: implementation of the CGIS_QueryByShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_QueryByShape.h"
#include "GIS_LayerManager.h"
#include "GIS_GeoMap.h"
#include "GIS_MapLayer.h"
#include "GIS_FeatureRoute.h"
#include ".\ZLib\zlib.h"

#include "MemoryDispatchForSpaceDataV2.h"
//////////////////////////////////////////////////////////////////////////
#include "./DebugFunctionLib/DebugFun.h"

 
/**
 * @brief   调试函数库(只在调试版本中被定义)
 *
 * 日志文件功能
 * 调试输出到屏幕功能
 * 函数调用跟踪功能
 *
 *    
 * @note    如果不想在调试版本中使能这些宏中的一项或几项,应该将这些宏在local文
 *          件中重新定义为空,即打开下面相应的注释即可.       
*/

//#define DBG2SCR0(sz)              
//#define DBG2SCR1(sz, p1)          
//#define DBG2SCR2(sz, p1, p2)      
//#define DBG2SCR3(sz, p1, p2, p3) 
#define TRACK_FUNCTION(sz) 
//#define DBG2LOG0(sz)                
//#define DBG2LOG1(sz, p1)            
//#define DBG2LOG2(sz, p1, p2)        
//#define DBG2LOG3(sz, p1, p2, p3)    
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_QueryByShape::CGIS_QueryByShape()
{
	m_Type = QUERY_BYSHARP;
	m_bStopSearch = FALSE;
	m_bNearNetWork = FALSE;
	m_pNetworkRoute = NULL;
	m_nNearRouteBID = 0;
}

CGIS_QueryByShape::~CGIS_QueryByShape()
{

}
BOOL CGIS_QueryByShape::QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,int nFeaNum )
{
    TRACK_FUNCTION("QueryByBuffer 1");

	if(pPt == NULL || pFeaSet == NULL)
		return FALSE;
	POSITION pos = pFeaSet->GetHeadPosition();
	CGIS_FeaturePoint* pTempFea;
	while(pos)
	{
		pTempFea = (CGIS_FeaturePoint*)(pFeaSet->GetNext(pos));
		if(pTempFea != NULL)
		{
			double dis = CalcDist(pPt->dx,pPt->dy,pTempFea->GetPtList()->dx,pTempFea->GetPtList()->dy);
			ST_FEATURE_POINT *pFeatureST = new ST_FEATURE_POINT;
			pFeatureST->pFeature = pTempFea->Clone();
			pFeatureST->dbDis = dis;
			//排序的过程
			POSITION possort = pReturnFeaSet.GetHeadPosition();
			ST_FEATURE_POINT *pTemp;
			if(pReturnFeaSet.GetCount() == 0)
				pReturnFeaSet.AddTail(pFeatureST);
			while(possort)
			{
				POSITION pos1 = possort;
				pTemp = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext(possort);
				if(dis <= pTemp->dbDis)
				{
					pReturnFeaSet.InsertBefore(pos1,pFeatureST);
					break;
				}
				if(!possort)
				{
					pReturnFeaSet.AddTail(pFeatureST);
					break;
				}
			}
		}
	}
	
	if( pReturnFeaSet.GetCount() > nFeaNum )
	{
		pos =	pReturnFeaSet.FindIndex( nFeaNum );
		ST_FEATURE_POINT *pFeaturePT = NULL;
		while( pos )
		{
			POSITION posRemove = pos;
			pFeaturePT = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext( pos );
			CGIS_FeaturePoint *pFeatureTemp = NULL;
			if( pFeaturePT )
			{
				pFeatureTemp = (CGIS_FeaturePoint*)pFeaturePT->pFeature;
				if( pFeatureTemp )
				{
					delete pFeatureTemp;
					pFeatureTemp = NULL;
				}
				delete pFeaturePT;
				pFeaturePT = NULL;
			}
			pReturnFeaSet.RemoveAt( posRemove );
			
		}
	}
	return TRUE;	
}
BOOL CGIS_QueryByShape::QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum )
{
    TRACK_FUNCTION("QueryByBuffer 2");

/*	if(pPt == NULL)
	{
		return FALSE;
	}
	if(m_pMap != NULL)
	{
		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
			ST_GEO_RECT TagetRect;
			double dis = dbDistance / 1852.0 / 60.0;
			TagetRect.dLeft = pPt->dx - dis;
			TagetRect.dRight = pPt->dx + dis;
			TagetRect.dBottom = pPt->dy + dis;
			TagetRect.dTop = pPt->dy - dis;
			NormalizeRect(TagetRect);

			int nBMIDs[3000];
			memset( nBMIDs, 0, sizeof(int)*3000);
			int nBNum = 0;
			CalculateBMapIDInRect(TagetRect, nBMIDs, nBNum );
			for( int i=0;	i<nBNum;	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nBMIDs[i] );
				if( pBMap )
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
							short    nKey;
							while(pos1 != NULL)
							{
								pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
								if(pLayer != NULL && ( MatchLayerType(pLayer->GetLayerInfo()->GetLayerID())==EnSharpType || EnSharpType==EN_OBJ_ALL ))
								{
									CGIS_MapLayer *pBLList[1];

									pBLList[0] = (CGIS_MapLayer*)pLayer;
									if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
										return FALSE;
//									pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );

									CPtrList *pOList = NULL;
									pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
									if( !pOList )
									{
										pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
										return FALSE;
									}
									QueryByBuffer(pReturnFeaSet,pPt,pOList,dbDistance,EnSharpType);
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								}
							
							}

						}
					}
//					m_pMap->UnLoadBreadthMap(pBMap);
				}
			}
		}
	}*/
	return FALSE;
}
void CGIS_QueryByShape::SortPOIByDistance( CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,unsigned short ClassID )
{
    TRACK_FUNCTION("SortPOIByDistance");

	if(pPt == NULL || pFeaSet == NULL)
		return ;
	POSITION pos = pFeaSet->GetHeadPosition();
	CGIS_FeaturePoint* pTempFea;
	while(pos)
	{
		pTempFea = (CGIS_FeaturePoint*)(pFeaSet->GetNext(pos));
		if(pTempFea != NULL)
		{
			if( ClassID == 0 || ClassID == pTempFea->GetPointObjectType() )
			{
				double dis = CalcDist(pPt->dx,pPt->dy,pTempFea->GetPtList()->dx,pTempFea->GetPtList()->dy);
				ST_FEATURE_POINT *pFeatureST = new ST_FEATURE_POINT;
				pFeatureST->pFeature = pTempFea->Clone();
				pFeatureST->dbDis = dis;
				//排序的过程
				POSITION possort = pReturnFeaSet.GetHeadPosition();
				ST_FEATURE_POINT *pTemp;
				if(pReturnFeaSet.GetCount() == 0)
					pReturnFeaSet.AddTail(pFeatureST);
				while(possort)
				{
					POSITION pos1 = possort;
					pTemp = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext(possort);
					if(dis <= pTemp->dbDis)
					{
						pReturnFeaSet.InsertBefore(pos1,pFeatureST);
						break;
					}
					if(!possort)
					{
						pReturnFeaSet.AddTail(pFeatureST);
						break;
					}
				}
			}
		}
	}
	return ;	
}
void CGIS_QueryByShape::SortPOIByName( CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,unsigned short ClassID , swString strName)
{
	if(pPt == NULL || pFeaSet == NULL)
		return ;
	POSITION pos = pFeaSet->GetHeadPosition();
	CGIS_FeaturePoint* pTempFea;
	while(pos)
	{
		pTempFea = (CGIS_FeaturePoint*)(pFeaSet->GetNext(pos));
		if(pTempFea != NULL)
		{
			if( ClassID == 0 || pTempFea->GetPointObjectType() == ClassID )
			{
				swString strFeaName;// = (swString)pTempFea->GetAnnotation();
				CharToUnicode( pTempFea->GetAnnotation() , &strFeaName );
				if( strFeaName.GetLength() >= strName.GetLength()  
					&&  strFeaName.Find( strName )!=-1 )
				{
					double dis = CalcDist(pPt->dx,pPt->dy,pTempFea->GetPtList()->dx,pTempFea->GetPtList()->dy);
					ST_FEATURE_POINT *pFeatureST = new ST_FEATURE_POINT;
					pFeatureST->pFeature = pTempFea->Clone();
					pFeatureST->dbDis = dis;
					//排序的过程
					POSITION possort = pReturnFeaSet.GetHeadPosition();
					ST_FEATURE_POINT *pTemp;
					if(pReturnFeaSet.GetCount() == 0)
						pReturnFeaSet.AddTail(pFeatureST);
					while(possort)
					{
						POSITION pos1 = possort;
						pTemp = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext(possort);
						if(dis <= pTemp->dbDis)
						{
							pReturnFeaSet.InsertBefore(pos1,pFeatureST);
							break;
						}
						if(!possort)
						{
							pReturnFeaSet.AddTail(pFeatureST);
							break;
						}
					}
				}
			}
		}
	}
	return ;	
}
BOOL CGIS_QueryByShape::QueryBufferByName(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum,UINT ILayID ,unsigned short ClassID,swString strName )
{
    TRACK_FUNCTION("QueryBufferByName 1");

	m_bStopSearch = FALSE;
	if(pPt == NULL)
	{
		return FALSE;
	}
	if( !m_pMap )
		return FALSE;
	if( m_pMap->GetMapFileType() != EN_MFTYPE_BAND )
		return FALSE;
	//航线附近
	if( m_bNearNetWork )
	{
		int nCount = 0;
		while( pReturnFeaSet.GetCount() < nFeaNum && nCount < m_nNearRouteBID )
		{
			int nID = m_NearRouteBIDs[nCount];
			nCount++;
			//////////////////////////////////////////////////////////////////////////
			CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nID );
			if( pBMap )
			{
				CGIS_LayerManager *pLManager = NULL;
				POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
				while(pos!= NULL)
				{
					pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
					if(pLManager != NULL)
					{
						IGIS_Layer *pLayer = NULL;	
						pLManager->GetLayerListMap()->Lookup(ILayID,pLayer);
						if(pLayer != NULL)
						{
							CGIS_MapLayer *pBLList[1];

							pBLList[0] = (CGIS_MapLayer*)pLayer;
							CPtrList pOList;
							LoadPOIByName( pOList , m_pMap , pBMap , pBLList , 1 , strName );
							//删除不同小类
							if( ClassID != 0 )
							{
								CGIS_FeaturePoint* pFeaturePT = NULL ;
								POSITION posAllObj = pOList.GetHeadPosition();
								while( posAllObj )
								{
									POSITION posAt = posAllObj;
									pFeaturePT = (CGIS_FeaturePoint*)pOList.GetNext( posAllObj );
									if( pFeaturePT && pFeaturePT->GetPointObjectType() != ClassID )
									{
										delete pFeaturePT;
										pFeaturePT = NULL;
										pOList.RemoveAt( posAt );
									}
								}
							}
							SortPOI(pReturnFeaSet,pPt,pOList);
							if( pReturnFeaSet.GetCount() >= nFeaNum )
								m_bStopSearch = TRUE;
/*							if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
								continue;
							
							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
							if( !pOList )
							{
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								continue;
							}
							SortPOIByName( pReturnFeaSet , pPt , pOList , ClassID,strName );
							pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
							*/
						}
						if( pReturnFeaSet.GetCount() >= nFeaNum )
						{
							m_bStopSearch = TRUE;
							break;
						}
					}
				}
				if( m_bStopSearch )
					break;
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}
	//点附近
	else
	{
		ST_GEO_RECT TagetRect;
		int nCount = 0;
		double dis = 0;

		//已经查询过的图幅集
		int nOldBMIDs[1000];
		memset( nOldBMIDs, 0, sizeof(int)*1000);
		int nOldBNum = 0;
		double dbDistance = dis;
		while( pReturnFeaSet.GetCount() < nFeaNum && dbDistance < BUFFER_QUERY_MAX )
		{
			dbDistance = dis + nCount*BUFFER_QUERY_STEP;
			double disCurrent = dbDistance / 1852.0 / 60.0;
			nCount++;
			TagetRect.dLeft = pPt->dx - disCurrent;
			TagetRect.dRight = pPt->dx + disCurrent;
			TagetRect.dBottom = pPt->dy + disCurrent;
			TagetRect.dTop = pPt->dy - disCurrent;
			NormalizeRect(TagetRect);

			//当前BUF内的所有图幅集
			int nBMIDs[1000];
			memset( nBMIDs, 0, sizeof(int)*1000);
			int nBNum = 0;
			CalculateBMapIDInRect(TagetRect, nBMIDs, nBNum );

			//需要查询的图幅集
			int nTagetBMIDs[1000];
			memset( nTagetBMIDs, 0, sizeof(int)*1000);
			int nTagetBNum = 0;
			//剔除已经查询过的图幅
			int nBID;
			BOOL bTaget = TRUE;
			int i;
			for( i = 0 ; i < nBNum ; i++ )
			{
				bTaget = TRUE;
				nBID = nBMIDs[i];
				int nOldBID ;
				int j;
				for( j = 0 ; j < nOldBNum ; j++ )
				{
					nOldBID = nOldBMIDs[j];
					if( nBID == nOldBID )
					{
						bTaget = FALSE;
						break;
					}
				}
				if( bTaget )
				{
					nTagetBMIDs[nTagetBNum] = nBID;
					nTagetBNum++; 
				}
			}
			//当前图幅集归为已经查询的图幅集
			for( i = nOldBNum ; i < nOldBNum+nTagetBNum ; i++ )
				nOldBMIDs[i] = nTagetBMIDs[i-nOldBNum];
			nOldBNum = nBNum;
			//对逐个图幅进行查询
			for( i=0;	i<nTagetBNum;	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nTagetBMIDs[i] );
				if( pBMap )
				{
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							IGIS_Layer *pLayer = NULL;	
							pLManager->GetLayerListMap()->Lookup(ILayID,pLayer);
							if(pLayer != NULL)
							{
								CGIS_MapLayer *pBLList[1];

								pBLList[0] = (CGIS_MapLayer*)pLayer;
								CPtrList pOList;
								LoadPOIByName( pOList , m_pMap , pBMap , pBLList , 1 , strName );
								//删除不同小类
								if( ClassID != 0 )
								{
									CGIS_FeaturePoint* pFeaturePT = NULL ;
									POSITION posAllObj = pOList.GetHeadPosition();
									while( posAllObj )
									{
										POSITION posAt = posAllObj;
										pFeaturePT = (CGIS_FeaturePoint*)pOList.GetNext( posAllObj );
										if( pFeaturePT && pFeaturePT->GetPointObjectType() != ClassID )
										{
											delete pFeaturePT;
											pFeaturePT = NULL;
											pOList.RemoveAt( posAt );
										}
									}
								}
								SortPOI(pReturnFeaSet,pPt,pOList);
								if( pReturnFeaSet.GetCount() >= nFeaNum )
									m_bStopSearch = TRUE;
/*								if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
									continue;
								
								CPtrList *pOList = NULL;
								pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
								if( !pOList )
								{
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
									continue;
								}
								SortPOIByName( pReturnFeaSet , pPt , pOList , ClassID,strName );
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								*/
							}
//							if( pReturnFeaSet.GetCount() >= nFeaNum )
//							{
//								m_bStopSearch = TRUE;
//								break;
//							}
							if( m_bStopSearch )
								break;
						}

					}

					if( m_bStopSearch )
						break;
				}
			}
			if( m_bStopSearch )
				break;
		}
	}
	if( pReturnFeaSet.GetCount() > nFeaNum )
	{
		POSITION pos =	pReturnFeaSet.FindIndex( nFeaNum );
		ST_FEATURE_POINT *pFeaturePT = NULL;
		while( pos )
		{
			POSITION posRemove = pos;
			pFeaturePT = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext( pos );
			CGIS_FeaturePoint *pFeatureTemp = NULL;
			if( pFeaturePT )
			{
				pFeatureTemp = (CGIS_FeaturePoint*)pFeaturePT->pFeature;
				if( pFeatureTemp )
				{
					delete pFeatureTemp;
					pFeatureTemp = NULL;
				}
				delete pFeaturePT;
				pFeaturePT = NULL;
			}
			pReturnFeaSet.RemoveAt( posRemove );
			
		}
	}
	return TRUE;
}
BOOL CGIS_QueryByShape::QueryBufferByName(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum,swString strName )
{
    TRACK_FUNCTION("QueryBufferByName 2");
	m_bStopSearch = FALSE;
	if(pPt == NULL)
	{
		return FALSE;
	}
	if( !m_pMap )
		return FALSE;
	if( m_pMap->GetMapFileType() != EN_MFTYPE_BAND )
		return FALSE;
	//航线附近
	if( m_bNearNetWork )
	{
		int nCount = 0;
		while( pReturnFeaSet.GetCount() < nFeaNum && nCount < m_nNearRouteBID )
		{
			int nID = m_NearRouteBIDs[nCount];
			nCount++;
			CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nID );
			if( pBMap )
			{
				CGIS_LayerManager *pLManager = NULL;
				POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
				while(pos!= NULL)
				{
					pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
					if(pLManager != NULL)
					{
						CGIS_MapLayer *pBLList[LAYER_PT_END-LAYER_PT_START+1];
						IGIS_Layer *pLayer = NULL;
						int nLayerCount = 0;
						for( int nLayerID = LAYER_PT_START ; nLayerID <= LAYER_PT_END ; nLayerID++ )
						{
							pLManager->GetLayerListMap()->Lookup(nLayerID,pLayer);
							if( pLayer )
							{
								pBLList[nLayerCount] = (CGIS_MapLayer*)pLayer;
								nLayerCount++;
							}
						}
						CPtrList pOList;
						LoadPOIByName( pOList , m_pMap , pBMap , pBLList , nLayerCount , strName );
						SortPOI(pReturnFeaSet,pPt,pOList);
						if( pReturnFeaSet.GetCount() >= nFeaNum )
							m_bStopSearch = TRUE;
						//////////////////////////////////////////////////////////////////////////
						
					/*	IGIS_Layer *pLayer = NULL;	
						for( int nLayerID = LAYER_PT_START ; nLayerID <= LAYER_PT_END ; nLayerID++ )
						{
							pLManager->GetLayerListMap()->Lookup(nLayerID,pLayer);
							if(pLayer != NULL)
							{
								CGIS_MapLayer *pBLList[1];

								pBLList[0] = (CGIS_MapLayer*)pLayer;
								if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
									continue;
								
								CPtrList *pOList = NULL;
								pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
								if( !pOList )
								{
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
									continue;
								}
								SortPOIByName( pReturnFeaSet , pPt , pOList ,0,strName );
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
							}
							if( pReturnFeaSet.GetCount() >= nFeaNum )
							{
								m_bStopSearch = TRUE;
								break;
							}
						}
						*/
					}
					if( m_bStopSearch )
						break;
				}

			}
			if( m_bStopSearch )
				break;
		}
	}
	else
	{
		ST_GEO_RECT TagetRect;
		int nCount = 0;
		double dis = 0;

		//已经查询过的图幅集
		int nOldBMIDs[1000];
		memset( nOldBMIDs, 0, sizeof(int)*1000);
		int nOldBNum = 0;
		double dbDistance = dis;
		while( pReturnFeaSet.GetCount() < nFeaNum && dbDistance < BUFFER_QUERY_MAX )
		{
			dbDistance = dis + nCount*BUFFER_QUERY_STEP;
			double disCurrent = dbDistance / 1852.0 / 60.0;
			nCount++;
			TagetRect.dLeft = pPt->dx - disCurrent;
			TagetRect.dRight = pPt->dx + disCurrent;
			TagetRect.dBottom = pPt->dy + disCurrent;
			TagetRect.dTop = pPt->dy - disCurrent;
			NormalizeRect(TagetRect);

			//当前BUF内的所有图幅集
			int nBMIDs[1000];
			memset( nBMIDs, 0, sizeof(int)*1000);
			int nBNum = 0;
			CalculateBMapIDInRect(TagetRect, nBMIDs, nBNum );

			//需要查询的图幅集
			int nTagetBMIDs[1000];
			memset( nTagetBMIDs, 0, sizeof(int)*1000);
			int nTagetBNum = 0;
			//剔除已经查询过的图幅
			int nBID;
			BOOL bTaget = TRUE;
			int i;
			for( i = 0 ; i < nBNum ; i++ )
			{
				bTaget = TRUE;
				nBID = nBMIDs[i];
				int nOldBID ;
				for( int j = 0 ; j < nOldBNum ; j++ )
				{
					nOldBID = nOldBMIDs[j];
					if( nBID == nOldBID )
					{
						bTaget = FALSE;
						break;
					}
				}
				if( bTaget )
				{
					nTagetBMIDs[nTagetBNum] = nBID;
					nTagetBNum++; 
				}
			}
			//当前图幅集归为已经查询的图幅集
			for( i = nOldBNum ; i < nOldBNum+nTagetBNum ; i++ )
				nOldBMIDs[i] = nTagetBMIDs[i-nOldBNum];
			nOldBNum = nBNum;
			//对逐个图幅进行查询
			for( i=0;	i<nTagetBNum;	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nTagetBMIDs[i] );
				if( pBMap )
				{
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							CGIS_MapLayer *pBLList[LAYER_PT_END-LAYER_PT_START+1];
							IGIS_Layer *pLayer = NULL;
							int nLayerCount = 0;
							for( int nLayerID = LAYER_PT_START ; nLayerID <= LAYER_PT_END ; nLayerID++ )
							{
								pLManager->GetLayerListMap()->Lookup(nLayerID,pLayer);
								if( pLayer )
								{
									pBLList[nLayerCount] = (CGIS_MapLayer*)pLayer;
									nLayerCount++;
								}
							}
							CPtrList pOList;
							LoadPOIByName( pOList , m_pMap , pBMap , pBLList , nLayerCount , strName );
							SortPOI(pReturnFeaSet,pPt,pOList);
							if( pReturnFeaSet.GetCount() >= nFeaNum )
								m_bStopSearch = TRUE;
							//////////////////////////////////////////////////////////////////////////
						/*	IGIS_Layer *pLayer = NULL;	
							for( int nLayerID = LAYER_PT_START ; nLayerID <= LAYER_PT_END ; nLayerID++ )
							{
								pLManager->GetLayerListMap()->Lookup(nLayerID,pLayer);
								if(pLayer != NULL)
								{
									CGIS_MapLayer *pBLList[1];

									pBLList[0] = (CGIS_MapLayer*)pLayer;
									DWORD dwTime = GetTickCount();
									if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
										continue;
									m_dwTime += ( GetTickCount() - dwTime );
									
									CPtrList *pOList = NULL;
									pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
									if( !pOList )
									{
										pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
										continue;
									}
									SortPOIByName( pReturnFeaSet , pPt , pOList ,0,strName );
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								}
								if( pReturnFeaSet.GetCount() >= nFeaNum )
								{
									m_bStopSearch = TRUE;
									break;
								}
							}
						*/	
						}
						if( m_bStopSearch )
							break;
					}

				}
				if( m_bStopSearch )
					break;
			}
			if( m_bStopSearch )
				break;
		}
	}
	if( pReturnFeaSet.GetCount() > nFeaNum )
	{
		POSITION pos =	pReturnFeaSet.FindIndex( nFeaNum );
		ST_FEATURE_POINT *pFeaturePT = NULL;
		while( pos )
		{
			POSITION posRemove = pos;
			pFeaturePT = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext( pos );
			CGIS_FeaturePoint *pFeatureTemp = NULL;
			if( pFeaturePT )
			{
				pFeatureTemp = (CGIS_FeaturePoint*)pFeaturePT->pFeature;
				if( pFeatureTemp )
				{
					delete pFeatureTemp;
					pFeatureTemp = NULL;
				}
				delete pFeaturePT;
				pFeaturePT = NULL;
			}
			pReturnFeaSet.RemoveAt( posRemove );
			
		}
	}
	return TRUE;
}

BOOL CGIS_QueryByShape::QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum,UINT ILayID ,unsigned short ClassID )
{
    //////////////////////////////////////////////////////////////////////////
    CSpaceMemoryLock aLock;    
    TRACK_FUNCTION("QueryByBuffer 3");
    //////////////////////////////////////////////////////////////////////////

	m_bStopSearch = FALSE;
	if(pPt == NULL)
	{
		return FALSE;
	}
	if( !m_pMap )
		return FALSE;
	if( m_pMap->GetMapFileType() != EN_MFTYPE_BAND )
		return FALSE;
	//航线附近
	if( m_bNearNetWork )
	{
		int nCount = 0;
		while( pReturnFeaSet.GetCount() < nFeaNum && nCount < m_nNearRouteBID )
		{
			int nID = m_NearRouteBIDs[nCount];
			nCount++;
			CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nID );
			if( pBMap )
			{
				CGIS_LayerManager *pLManager = NULL;
				POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
				while(pos!= NULL)
				{
					pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
					if(pLManager != NULL)
					{
						IGIS_Layer *pLayer = NULL;	
						pLManager->GetLayerListMap()->Lookup(ILayID,pLayer);
						if(pLayer != NULL)
						{
							CGIS_MapLayer *pBLList[1];

							pBLList[0] = (CGIS_MapLayer*)pLayer;
							if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
								return FALSE;
							
							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
							if( !pOList )
							{
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								return FALSE;
							}
							SortPOIByDistance( pReturnFeaSet , pPt , pOList ,ClassID );
							pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
						}
						
					}

				}

			}
			if( pReturnFeaSet.GetCount() > nFeaNum )
			{
				m_bStopSearch = TRUE;
				break;
			}
		}
	}
	else
	{
		ST_GEO_RECT TagetRect;
		int nCount = 0;
		double dis = 0;

		//已经查询过的图幅集
		int nOldBMIDs[1000];
		memset( nOldBMIDs, 0, sizeof(int)*1000);
		int nOldBNum = 0;
		double dbDistance = dis;;
		while( pReturnFeaSet.GetCount() < nFeaNum && dbDistance < BUFFER_QUERY_MAX )
		{
			dbDistance = dis + nCount*BUFFER_QUERY_STEP;
			double disCurrent = dbDistance / 1852.0 / 60.0;
			nCount++;
			TagetRect.dLeft = pPt->dx - disCurrent;
			TagetRect.dRight = pPt->dx + disCurrent;
			TagetRect.dBottom = pPt->dy + disCurrent;
			TagetRect.dTop = pPt->dy - disCurrent;
			NormalizeRect(TagetRect);

			//当前BUF内的所有图幅集
			int nBMIDs[1000];
			memset( nBMIDs, 0, sizeof(int)*1000);
			int nBNum = 0;
			CalculateBMapIDInRect(TagetRect, nBMIDs, nBNum );

			//需要查询的图幅集
			int nTagetBMIDs[1000];
			memset( nTagetBMIDs, 0, sizeof(int)*1000);
			int nTagetBNum = 0;
			//剔除已经查询过的图幅
			int nBID;
			BOOL bTaget = TRUE;
			int i;
			for( i = 0 ; i < nBNum ; i++ )
			{
				bTaget = TRUE;
				nBID = nBMIDs[i];
				int nOldBID ;
				for( int j = 0 ; j < nOldBNum ; j++ )
				{
					nOldBID = nOldBMIDs[j];
					if( nBID == nOldBID )
					{
						bTaget = FALSE;
						break;
					}
				}
				if( bTaget )
				{
					nTagetBMIDs[nTagetBNum] = nBID;
					nTagetBNum++; 
				}
			}
			//当前图幅集归为已经查询的图幅集
			for( i = nOldBNum ; i < nOldBNum+nTagetBNum ; i++ )
				nOldBMIDs[i] = nTagetBMIDs[i-nOldBNum];
			nOldBNum = nBNum;
			//对逐个图幅进行查询
			for( i=0;	i<nTagetBNum;	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nTagetBMIDs[i] );
				if( pBMap )
				{
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							IGIS_Layer *pLayer = NULL;	
							pLManager->GetLayerListMap()->Lookup(ILayID,pLayer);
							if(pLayer != NULL)
							{
								CGIS_MapLayer *pBLList[1];

								pBLList[0] = (CGIS_MapLayer*)pLayer;
								if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
									return FALSE;
								
								CPtrList *pOList = NULL;
								pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
								if( !pOList )
								{
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
									return FALSE;
								}
								SortPOIByDistance( pReturnFeaSet , pPt , pOList ,ClassID );
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
							}
							
						}

					}

				}
				if( pReturnFeaSet.GetCount() > nFeaNum )
				{
					m_bStopSearch = TRUE;
					break;
				}
			}
			if( m_bStopSearch )
				break;

		}
	}
	if( pReturnFeaSet.GetCount() > nFeaNum )
	{
		POSITION pos =	pReturnFeaSet.FindIndex( nFeaNum );
		ST_FEATURE_POINT *pFeaturePT = NULL;
		while( pos )
		{
			POSITION posRemove = pos;
			pFeaturePT = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext( pos );
			CGIS_FeaturePoint *pFeatureTemp = NULL;
			if( pFeaturePT )
			{
				pFeatureTemp = (CGIS_FeaturePoint*)pFeaturePT->pFeature;
				if( pFeatureTemp )
				{
					delete pFeatureTemp;
					pFeatureTemp = NULL;
				}
				delete pFeaturePT;
				pFeaturePT = NULL;
			}
			pReturnFeaSet.RemoveAt( posRemove );
			
		}
	}
	return TRUE;
}
BOOL CGIS_QueryByShape::QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,double dbDistance,EnQueryObjType EnSharpType)
{
    TRACK_FUNCTION("QueryByBuffer 4");

	ST_GEO_RECT TagetRect;
	double dis = dbDistance / 1852.0 / 60.0;
	TagetRect.dLeft = pPt->dx - dis;
	TagetRect.dRight = pPt->dx + dis;
	TagetRect.dBottom = pPt->dy + dis;
	TagetRect.dTop = pPt->dy - dis;
	NormalizeRect(TagetRect);
	if(pPt == NULL || pFeaSet == NULL)
	{
		return FALSE;
	}
	POSITION pos = pFeaSet->GetHeadPosition();
	CGIS_Feature* pTempFea;
	while(pos)
	{
		pTempFea = (CGIS_Feature*)(pFeaSet->GetNext(pos));
		if(pTempFea != NULL)
		{
			BOOL bIntersect = FALSE;
			//点物标
			if(pTempFea->GetObjType() == 0 || pTempFea->GetObjType() == 1)
			{
				if( EnSharpType == 0 || EnSharpType == 1)
				{
					if( !IsPtInRect( pTempFea->GetPtList()->dx,pTempFea->GetPtList()->dy,TagetRect.dLeft,TagetRect.dTop,TagetRect.dRight,TagetRect.dBottom ) )
						continue;

					dis = CalcDist(pPt->dx,pPt->dy,pTempFea->GetPtList()->dx,pTempFea->GetPtList()->dy);
					if(dis <= dbDistance)
					{
						if(EnSharpType == 0)
						{
							pReturnFeaSet.AddTail(pTempFea->Clone());
						}
						else
						{
							ST_FEATURE_POINT *pFeatureST = new ST_FEATURE_POINT;
							pFeatureST->pFeature = pTempFea->Clone();
							pFeatureST->dbDis = dis;
							//排序的过程
							POSITION possort = pReturnFeaSet.GetHeadPosition();
							ST_FEATURE_POINT *pTemp;
							if(pReturnFeaSet.GetCount() == 0)
								pReturnFeaSet.AddTail(pFeatureST);
							while(possort)
							{
								POSITION pos1 = possort;
								pTemp = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext(possort);
								if(dis <= pTemp->dbDis)
								{
									pReturnFeaSet.InsertBefore(pos1,pFeatureST);
									break;
								}
								if(!possort)
								{
									pReturnFeaSet.AddTail(pFeatureST);
									break;
								}
							}
						}
					}
				}
			}
			//线物标
			if(pTempFea->GetObjType() == 2 || pTempFea->GetObjType() == 3)
			{
				if( EnSharpType == 0 || EnSharpType == 2)
				{
					int j = 0;
					int nPart;
					ST_GEO_PTXY stLast;

					for(int i = 0; i < ((CGIS_FeatureLine*)pTempFea)->GetPartNum(); i++)
					{
						stLast.dx = ((CGIS_FeatureLine*)pTempFea)->GetPtList()[0].dx;
						stLast.dy = ((CGIS_FeatureLine*)pTempFea)->GetPtList()[0].dy;
						j++;
						nPart = ((CGIS_FeatureLine*)pTempFea)->GetPart()[i] - 1;
		
						while(nPart > 0)
						{

							if( fabs(((CGIS_FeatureLine*)pTempFea)->GetPtList()[j].dx - stLast.dx) > FLOAT_ZERO 
								 ||  fabs(((CGIS_FeatureLine*)pTempFea)->GetPtList()[j].dy - stLast.dy) > FLOAT_ZERO  )
							{
								bIntersect = LineIntersectRound( *pPt, dbDistance, stLast,((CGIS_FeatureLine*)pTempFea)->GetPtList()[j]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
								stLast.dx = ((CGIS_FeatureLine*)pTempFea)->GetPtList()[j].dx;
								stLast.dy = ((CGIS_FeatureLine*)pTempFea)->GetPtList()[j].dy;
							}
							j++;
							nPart--;
						}
						if(bIntersect)
						{
							break;
						}
					}
				}
			}
			//面物标
			if(pTempFea->GetObjType() == 4 || pTempFea->GetObjType() == 5)
			{
				if( EnSharpType == 0 || EnSharpType == 3)
				{
					BOOL bTemp = FALSE;
					if(PointInPoly(*pPt,(CGIS_FeaturePoly*)(pTempFea)))
					{
						pReturnFeaSet.AddTail(pTempFea->Clone());
						bTemp = TRUE;
					}
					if(!bTemp)
					{
						for(int i = 0; i < ( ((CGIS_FeaturePoly*)pTempFea)->GetPtNum() -1 ); i++)
						{
							//复杂多边形
							if(((CGIS_FeaturePoly*)pTempFea)->GetPartNum() > 1 )
							{
								int nPartnumCount = 0;
								BOOL bPolyTemp = FALSE;
								for(int j = 0 ; j < ((CGIS_FeaturePoly*)pTempFea)->GetPartNum()-1 ; j++)
								{
									nPartnumCount += ((CGIS_FeaturePoly*)pTempFea)->GetPart()[j];
									if(i == nPartnumCount-1)
									{
										bPolyTemp = TRUE;
										break;
									}
								}
								if(bPolyTemp)
								{
									continue;
								}
							}
							//简单多边形
							if( fabs(((CGIS_FeaturePoly*)pTempFea)->GetPtList()[i].dx - ((CGIS_FeaturePoly*)pTempFea)->GetPtList()[i+1].dx)>FLOAT_ZERO
								&& fabs(((CGIS_FeaturePoly*)pTempFea)->GetPtList()[i].dy - ((CGIS_FeaturePoly*)pTempFea)->GetPtList()[i+1].dy)>FLOAT_ZERO  )
							{
								bIntersect = LineIntersectRound(*pPt,dbDistance,((CGIS_FeaturePoly*)pTempFea)->GetPtList()[i],((CGIS_FeaturePoly*)pTempFea)->GetPtList()[i+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
						}
					}
				}
			}
			//道路
			if(pTempFea->GetObjType() == 6 || pTempFea->GetObjType() == 7)
			{
				if( EnSharpType == 0 || EnSharpType == 4)
				{
					if( GeoRectIntersect( &(((CGIS_FeatureRut*)pTempFea)->GetRectObj() ),&TagetRect ) == 0 )
						continue;
					for(int i = 0; i < ( ((CGIS_FeatureRut*)pTempFea)->GetPtNum() - 1 ); i++)
					{
						ST_GEO_PTXY ptTemp1,ptTemp2;
						ptTemp1 = ((CGIS_FeatureRut*)pTempFea)->GetPtList()[i];
						ptTemp2 = ((CGIS_FeatureRut*)pTempFea)->GetPtList()[i+1];
						if( fabs(ptTemp1.dx-ptTemp2.dx)>FLOAT_ZERO || fabs(ptTemp1.dy-ptTemp2.dy)>FLOAT_ZERO )
						{
							ST_GEO_RECT rtTemp;
							rtTemp.dLeft = min(ptTemp1.dx,ptTemp2.dx);
							rtTemp.dRight = max(ptTemp1.dx,ptTemp2.dx);
							rtTemp.dTop = min(ptTemp1.dy,ptTemp2.dy);
							rtTemp.dBottom = max(ptTemp1.dy,ptTemp2.dy);
							if( GeoRectIntersect( &rtTemp,&TagetRect ) == 0)
								continue;
							bIntersect = LineIntersectRound(*pPt,dbDistance,((CGIS_FeatureRut*)pTempFea)->GetPtList()[i],((CGIS_FeatureRut*)pTempFea)->GetPtList()[i+1]);
							if(bIntersect)
							{
								pReturnFeaSet.AddTail(pTempFea->Clone());
								break;
							}
						}
					}
				}
			}

		}
	}
	return TRUE;
}
BOOL CGIS_QueryByShape::QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,double dbDistance,EnQueryObjType EnSharpType)
{
    //////////////////////////////////////////////////////////////////////////
    CSpaceMemoryLock aLock;    
    TRACK_FUNCTION("QueryByBuffer 5");
    //////////////////////////////////////////////////////////////////////////

	m_bStopSearch = FALSE;
	if(pPt == NULL)
	{
		return FALSE;
	}
	if(m_pMap != NULL)
	{
		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
			ST_GEO_RECT TagetRect;
			double dis = dbDistance / 1852.0 / 60.0;
			TagetRect.dLeft = pPt->dx - dis;
			TagetRect.dRight = pPt->dx + dis;
			TagetRect.dBottom = pPt->dy + dis;
			TagetRect.dTop = pPt->dy - dis;
			NormalizeRect(TagetRect);

			int nBMIDs[1000];
			memset( nBMIDs, 0, sizeof(int)*1000);
			int nBNum = 0;
			CalculateBMapIDInRect(TagetRect, nBMIDs, nBNum );
			for( int i=0;	i<nBNum;	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nBMIDs[i] );
				if( pBMap )
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
							short    nKey;
							while(pos1 != NULL)
							{
								pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
								if(pLayer != NULL && ( MatchLayerType(pLayer->GetLayerInfo()->GetLayerID())==EnSharpType || EnSharpType==EN_OBJ_ALL ))
								{
									CGIS_MapLayer *pBLList[1];

									pBLList[0] = (CGIS_MapLayer*)pLayer;
									if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
										return FALSE;
//									pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );

									CPtrList *pOList = NULL;
									pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
									if( !pOList )
									{
										pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
										return FALSE;
									}
									QueryByBuffer(pReturnFeaSet,pPt,pOList,dbDistance,EnSharpType);
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								}
							
							}

						}
					}
//					m_pMap->UnLoadBreadthMap(pBMap);
				}
				if( m_bStopSearch )
					break;
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
					short    nKey;
					while(pos1 != NULL)
					{
						pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
						if(pLayer != NULL)
						{
							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( 0, pOList );
							if( !pOList )
								return FALSE;
							QueryByBuffer(pReturnFeaSet,pPt,pOList,dbDistance,EnSharpType);
//							QueryByBuffer(pReturnFeaSet,pPt,pLayer->GetObjectList(),dbDistance,EnSharpType);
						}
					
					}

				}
			}
			return TRUE;
		}
	}
	return FALSE;
	
}
BOOL CGIS_QueryByShape::QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,double dbDistance,UINT ILayID,unsigned short ClassID,EnQueryObjType EnSharpType)
{
    TRACK_FUNCTION("QueryByBuffer 6");

	if(pPt == NULL)
	{
		return FALSE;
	}
	if(m_pMap != NULL)
	{
		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
			ST_GEO_RECT TagetRect;
			double dis = dbDistance / 1852.0 / 60.0;
// 			double dis = dbDistance / 1852.0;
//			long outlat;
//			long outlog;
//			InvertCircle(pPt->dy*60000,pPt->dx*60000,dis,90,&outlat,&outlog);
//			TagetRect.dRight = outlog/60000.0;
//			TagetRect.dLeft = pPt->dx - (TagetRect.dRight - pPt->dx);
//			InvertCircle(pPt->dy*60000,pPt->dx*60000,dis,180,&outlat,&outlog);
//			TagetRect.dBottom = outlat/60000.0;
//			TagetRect.dTop = pPt->dy - (TagetRect.dBottom - pPt->dy);
			TagetRect.dLeft = pPt->dx - dis;
			TagetRect.dRight = pPt->dx + dis;
			TagetRect.dBottom = pPt->dy + dis;
			TagetRect.dTop = pPt->dy - dis;
			NormalizeRect(TagetRect);

			int nBMIDs[1000];
			memset( nBMIDs, 0, sizeof(int)*1000);
			int nBNum = 0;
			CalculateBMapIDInRect(TagetRect, nBMIDs, nBNum );
			for( int i=0;	i<nBNum;	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nBMIDs[i] );
				if( pBMap )
				{
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							IGIS_Layer *pLayer = NULL;	
							pLManager->GetLayerListMap()->Lookup(ILayID,pLayer);
							if(pLayer != NULL)
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
								if(ClassID == 0 )
								{
									QueryByBuffer(pReturnFeaSet,pPt,pOList,dbDistance,EnSharpType);
								}
								else
								{
									if(EnSharpType != EN_OBJ_PT)
									{
										pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
										return FALSE;
									}
									CPtrList pSet;
									QueryByBuffer(pSet,pPt,pOList,dbDistance,EnSharpType);
									POSITION pos1 = pSet.GetHeadPosition();
									CGIS_FeaturePoint*  pTempFea;	
									ST_FEATURE_POINT *pFeaturePT;
									while( pos1 )
									{
										pFeaturePT = (ST_FEATURE_POINT*)pSet.GetNext(pos1);
										pTempFea = (CGIS_FeaturePoint*)( pFeaturePT->pFeature );

										if (pTempFea->GetPointObjectType() == ClassID)
//										if (GetPoiID(pTempFea->GetPointObjectType(), ILayID) == ClassID)
										{
											//排序的过程
											POSITION posin = pReturnFeaSet.GetHeadPosition();
											ST_FEATURE_POINT *pTemp;
											if(pReturnFeaSet.GetCount() == 0)
												pReturnFeaSet.AddTail(pFeaturePT);
											while(posin)
											{
												POSITION posin1 = posin;
												pTemp = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext(posin);
												if(pFeaturePT->dbDis <= pTemp->dbDis)
												{
													pReturnFeaSet.InsertBefore(posin1,pFeaturePT);
													break;
												}
												if(!posin)
												{
													pReturnFeaSet.AddTail(pFeaturePT);
													break;
												}
											}
										}
										else
										{
											delete pTempFea;
											pTempFea = NULL;
											delete pFeaturePT;
											pFeaturePT = NULL;
										}
									}
									pSet.RemoveAll();									
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
					POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
					IGIS_Layer *pLayer = NULL;	
					short    nKey;
					while(pos1 != NULL)
					{
						pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
						if(pLayer != NULL)
						{
							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( 0, pOList );
							if( !pOList )
								return FALSE;
							QueryByBuffer(pReturnFeaSet,pPt,pOList,dbDistance,EnSharpType);
//							QueryByBuffer(pReturnFeaSet,pPt,pLayer->GetObjectList(),dbDistance,EnSharpType);
						}
					
					}

				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

//交叉路口查询
BOOL CGIS_QueryByShape::QueryByIntersect(CPtrList& pReturnFeaSet,CPtrList* pFirstFeaSet,CPtrList* pSecondFeaSet)
{
    TRACK_FUNCTION("QueryByIntersect 1");

	m_bStopSearch = FALSE;
	if(pFirstFeaSet == NULL || pSecondFeaSet == NULL)
	{
		return FALSE;
	}
	POSITION pos = pFirstFeaSet->GetHeadPosition();
	CGIS_Feature* pTempFea;
	ST_ROAD_PT *temp;
//	double x,y;
	
	while(pos)
	{
		pTempFea = (CGIS_Feature*)(pFirstFeaSet->GetNext(pos));
		if(pTempFea != NULL)
		{
			if(pTempFea->GetObjType() == 6 || pTempFea->GetObjType() == 7)
			{
				CGIS_FeatureRut *pTempFeaRut = (CGIS_FeatureRut*)pTempFea;
				//////////////////////////////////////////////////////////////////////////
				
				ST_GEO_RECT rtSource;
				rtSource.dLeft = pTempFeaRut->GetRectObj().dLeft - 0.001;
				rtSource.dRight = pTempFeaRut->GetRectObj().dRight + 0.001;
				rtSource.dTop = pTempFeaRut->GetRectObj().dTop - 0.001;
				rtSource.dBottom = pTempFeaRut->GetRectObj().dBottom + 0.001;
				//////////////////////////////////////////////////////////////////////////
				
				POSITION pos1 = pSecondFeaSet->GetHeadPosition();
				CGIS_Feature* pTempFea1;
				while(pos1)
				{
					pTempFea1 = (CGIS_Feature*)(pSecondFeaSet->GetNext(pos1));
					if(pTempFea1 != NULL)
					{
						swString strTempFea1,strTempFea2;
						CharToUnicode( pTempFea1->GetAnnotation() , &strTempFea1 );
						CharToUnicode( pTempFea->GetAnnotation() , &strTempFea2 );

						if( strTempFea1 == strTempFea2 )
							continue;
						if(pTempFea1->GetObjType() == 6 || pTempFea1->GetObjType() == 7)
						{
							ST_ROAD_PT *pRoadPt = NULL;
							CGIS_FeatureRut *pTempFeaRut1 = (CGIS_FeatureRut*)pTempFea1;
							ST_GEO_PTXY ptHead = pTempFea->GetPtList()[0];
							ST_GEO_PTXY ptTail = pTempFea->GetPtList()[pTempFeaRut->GetPtNum() - 1];
							
							ST_GEO_PTXY ptHead1 = pTempFea1->GetPtList()[0];
							ST_GEO_PTXY ptTail1 = pTempFea1->GetPtList()[pTempFeaRut1->GetPtNum() - 1];
							//////////////////////////////////////////////////////////////////////////
							if( !IsPtInRect(ptHead1.dx , ptHead1.dy , rtSource.dLeft,rtSource.dTop,rtSource.dRight , rtSource.dBottom )
								&&!IsPtInRect(ptTail1.dx , ptTail1.dy , rtSource.dLeft,rtSource.dTop,rtSource.dRight , rtSource.dBottom ))
								continue;
							//////////////////////////////////////////////////////////////////////////
							if( (fabs(ptHead.dx - ptHead1.dx) < FLOAT_ZERO && fabs(ptHead.dy-ptHead1.dy) < FLOAT_ZERO) || (fabs(ptHead.dx-ptTail1.dx) < FLOAT_ZERO && fabs(ptHead.dy-ptTail1.dy) < FLOAT_ZERO) )
							{
								pRoadPt = new ST_ROAD_PT;
								swString strRoad1,strRoad2;
								CharToUnicode( pTempFea->GetAnnotation() , &strRoad1 );
								CharToUnicode( pTempFea1->GetAnnotation() , &strRoad2 );
//								strRoad1 = (swString)pTempFea->GetAnnotation();
//								strRoad2 = (swString)pTempFea1->GetAnnotation();
								if(strRoad1.IsEmpty())
									strRoad1 = L"路";
								if(strRoad2.IsEmpty())
									strRoad2 = L"路";
								pRoadPt->Name = strRoad1 + L"*" + strRoad2;
								pRoadPt->pt.dx = ptHead.dx;
								pRoadPt->pt.dy = ptHead.dy;
							}
							else if( (fabs(ptTail.dx - ptHead1.dx) < FLOAT_ZERO && fabs(ptTail.dy-ptHead1.dy) < FLOAT_ZERO) || (fabs(ptTail.dx-ptTail1.dx) < FLOAT_ZERO && fabs(ptTail.dy-ptTail1.dy) < FLOAT_ZERO) )
							{
								pRoadPt = new ST_ROAD_PT;
								swString strRoad1,strRoad2;
								CharToUnicode( pTempFea->GetAnnotation() , &strRoad1 );
								CharToUnicode( pTempFea1->GetAnnotation() , &strRoad2 );
//								strRoad1 = (swString)pTempFea->GetAnnotation();
//								strRoad2 = (swString)pTempFea1->GetAnnotation();
								if(strRoad1.IsEmpty())
									strRoad1 = L"路";
								if(strRoad2.IsEmpty())
									strRoad2 = L"路";
								pRoadPt->Name = strRoad1 + L"*" + strRoad2;
								pRoadPt->pt.dx = ptTail.dx;
								pRoadPt->pt.dy = ptTail.dy;
							}
							if( pRoadPt )
							{
								POSITION pos2 = pReturnFeaSet.GetHeadPosition();
								BOOL BAdd = TRUE;
								while( pos2 )
								{
									temp = (ST_ROAD_PT*)pReturnFeaSet.GetNext(pos2);
									double dx = temp->pt.dx - pRoadPt->pt.dx;
									double dy = temp->pt.dy - pRoadPt->pt.dy;
									if (fabs(dx)<FLOAT_ZERO && fabs(dy)<FLOAT_ZERO)
									{
										delete pRoadPt;
										pRoadPt = NULL;
										BAdd = FALSE;
										swString str;
										CharToUnicode( pTempFeaRut1->GetAnnotation() , &str );
//										int bFind = temp->Name.Find( (swString)pTempFeaRut1->GetAnnotation() );
										int bFind = temp->Name.Find( str );
										if( ( bFind) == -1 ) 
											temp->Name = temp->Name + L"*" + str;
										break;
									}
										
								}
								if (BAdd == TRUE)
									pReturnFeaSet.AddTail(pRoadPt);
							}

						}						
					}
				if( m_bStopSearch )
					break;
				}
			}
			
		}
		if( m_bStopSearch )
			break;
	}
	return TRUE;
}




//相交查找
BOOL CGIS_QueryByShape::QueryByIntersect(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,CPtrList* pFeaSet,EnQueryObjType EnSharpType)
{
    TRACK_FUNCTION("QueryByIntersect 2");

	if(pFea == NULL || pFeaSet == NULL)
	{
		return FALSE;
	}
	POSITION pos = pFeaSet->GetHeadPosition();
	CGIS_Feature* pTempFea;
	while(pos)
	{
		pTempFea = (CGIS_Feature*)(pFeaSet->GetNext(pos));
		if(pTempFea != NULL)
		{
			//点物标
			if(pTempFea->GetObjType() == 0 || pTempFea->GetObjType() == 1)
			{
				if( EnSharpType == 0 || EnSharpType == 1)
				{
					//点
					if(pFea->GetObjType() == 0 || pFea->GetObjType() == 1) 
					{
						if( fabs(pFea->GetPtList()->dx - pTempFea->GetPtList()->dx)<FLOAT_ZERO
							&& fabs(pFea->GetPtList()->dy == pTempFea->GetPtList()->dy)<FLOAT_ZERO )
						{
							pReturnFeaSet.AddTail(pTempFea->Clone());
						}
					}
					//线
					if(pFea->GetObjType() == 2 || pFea->GetObjType() == 3)
					{
						for(int i = 0 ; i < ((CGIS_FeatureLine*)(pFea))->GetPtNum() - 1 ; i++)
						{
							if(PointOnLine(*pTempFea->GetPtList(),((CGIS_FeatureLine*)(pFea))->GetPtList()[i],((CGIS_FeatureLine*)(pFea))->GetPtList()[i+1]))
							{
								pReturnFeaSet.AddTail(pTempFea->Clone());
								break;
							}
						}
					}
					//面
					if(pFea->GetObjType() == 4 || pFea->GetObjType() == 5)
					{
						for(int i = 0 ; i<((CGIS_FeaturePoly*)(pFea))->GetPtNum() - 1 ; i++)
						{
							//复杂多边形
							if(((CGIS_FeaturePoly*)pFea)->GetPartNum() > 1 )
							{
								int nPartnumCount = 0;
								BOOL bPolyTemp = FALSE;
								for(int j = 0 ; j < ((CGIS_FeaturePoly*)pFea)->GetPartNum()-1 ; j++)
								{
									nPartnumCount += ((CGIS_FeaturePoly*)pFea)->GetPart()[j];
									if(i == nPartnumCount-1)
									{
										bPolyTemp = TRUE;
										break;
									}
								}
								if(bPolyTemp)
								{
									continue;
								}
							}
							if(PointOnLine(*pTempFea->GetPtList(),((CGIS_FeaturePoly*)(pFea))->GetPtList()[i],((CGIS_FeaturePoly*)(pFea))->GetPtList()[i+1]))
							{
								pReturnFeaSet.AddTail(pTempFea->Clone());
								break;
							}
						}
					}
					//道路
					if(pFea->GetObjType() == 6 || pFea->GetObjType() == 7)
					{
						for(int i = 0; i < ( ((CGIS_FeatureRut*)pFea)->GetPtNum() - 1 ); i++)
						{
							if(PointOnLine(*pTempFea->GetPtList(),((CGIS_FeatureRut*)(pFea))->GetPtList()[i],((CGIS_FeatureLine*)(pFea))->GetPtList()[i+1]))
							{
								pReturnFeaSet.AddTail(pTempFea->Clone());
								break;
							}
						}
					}
				}
			}

			//线物标
			if(pTempFea->GetObjType() == 2 || pTempFea->GetObjType() == 3)
			{
				if( EnSharpType == 0 || EnSharpType == 2)
				{
					//点
					if(pFea->GetObjType() == 0 || pFea->GetObjType() == 1)
					{
						for(int i = 0 ; i < ((CGIS_FeatureLine*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							if(PointOnLine(*pFea->GetPtList(),((CGIS_FeatureLine*)(pTempFea))->GetPtList()[i],((CGIS_FeatureLine*)(pTempFea))->GetPtList()[i+1]))
							{
								pReturnFeaSet.AddTail(pTempFea->Clone());
								break;
							}
						}
					}
					//线
					if(pFea->GetObjType() == 2 || pFea->GetObjType() == 3)
					{
						BOOL bIntersect = FALSE;
						for(int i = 0 ; i < ((CGIS_FeatureLine*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							for(int j = 0 ; j < ((CGIS_FeatureLine*)(pFea))->GetPtNum() - 1 ; j++)
							{
								bIntersect = LineToLine(((CGIS_FeatureLine*)(pTempFea))->GetPtList()[i],((CGIS_FeatureLine*)(pTempFea))->GetPtList()[i+1],((CGIS_FeatureLine*)(pFea))->GetPtList()[j],((CGIS_FeatureLine*)(pFea))->GetPtList()[j+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
							if(bIntersect)
							{
								break;
							}
						}
					}
					//面
					if(pFea->GetObjType() == 4 || pFea->GetObjType() == 5)
					{
						BOOL bIntersect = FALSE;
						for(int i = 0 ; i < ((CGIS_FeatureLine*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							for(int j = 0 ; j < ((CGIS_FeaturePoly*)(pFea))->GetPtNum() - 1 ; j++)
							{
								//复杂多边形
								if(((CGIS_FeaturePoly*)pFea)->GetPartNum() > 1 )
								{
									int nPartnumCount = 0;
									BOOL bPolyTemp = FALSE;
									for(int k = 0 ; k < ((CGIS_FeaturePoly*)pFea)->GetPartNum()-1 ; k++)
									{
										nPartnumCount += ((CGIS_FeaturePoly*)pFea)->GetPart()[k];
										if(i == nPartnumCount-1)
										{
											bPolyTemp = TRUE;
											break;
										}
									}
									if(bPolyTemp)
									{
										continue;
									}
								}
								bIntersect = LineToLine(((CGIS_FeatureLine*)(pTempFea))->GetPtList()[i],((CGIS_FeatureLine*)(pTempFea))->GetPtList()[i+1],((CGIS_FeaturePoly*)(pFea))->GetPtList()[j],((CGIS_FeaturePoly*)(pFea))->GetPtList()[j+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
							if(bIntersect)
							{
								break;
							}
						}
					}
					//道路
					if(pFea->GetObjType() == 6 || pFea->GetObjType() == 7)
					{
						BOOL bIntersect = FALSE;
						for(int i = 0; i <  ((CGIS_FeatureLine*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							for(int j = 0 ; j < ((CGIS_FeatureRut*)(pFea))->GetPtNum() - 1; j++)
							{
								bIntersect = LineToLine(((CGIS_FeatureLine*)(pTempFea))->GetPtList()[i],((CGIS_FeatureLine*)(pTempFea))->GetPtList()[i+1],((CGIS_FeatureRut*)(pFea))->GetPtList()[j],((CGIS_FeatureRut*)(pFea))->GetPtList()[j+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
							if(bIntersect)
							{
								break;
							}
						}
					}
				}

			}

			//面物标
			if(pTempFea->GetObjType() == 4 || pTempFea->GetObjType() == 5)
			{
				if( EnSharpType == 0 || EnSharpType == 3)
				{
					//点
					if(pFea->GetObjType() == 0 || pFea->GetObjType() == 1)
					{
						for(int i = 0 ; i < ((CGIS_FeaturePoly*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							//复杂多边形
							if(((CGIS_FeaturePoly*)pTempFea)->GetPartNum() > 1 )
							{
								int nPartnumCount = 0;
								BOOL bPolyTemp = FALSE;
								for(int j = 0 ; j < ((CGIS_FeaturePoly*)pTempFea)->GetPartNum()-1 ; j++)
								{
									nPartnumCount += ((CGIS_FeaturePoly*)pTempFea)->GetPart()[j];
									if(i == nPartnumCount-1)
									{
										bPolyTemp = TRUE;
										break;
									}
								}
								if(bPolyTemp)
								{
									continue;
								}
							}
							if(PointOnLine(*pFea->GetPtList(),((CGIS_FeaturePoly*)(pTempFea))->GetPtList()[i],((CGIS_FeaturePoly*)(pTempFea))->GetPtList()[i+1]))
							{
								pReturnFeaSet.AddTail(pTempFea->Clone());
								break;
							}
						}
					}
					//线
					if(pFea->GetObjType() == 2 || pFea->GetObjType() == 3)
					{
						BOOL bIntersect = FALSE;
						for(int i = 0 ; i < ((CGIS_FeaturePoly*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							//复杂多边形
							if(((CGIS_FeaturePoly*)pTempFea)->GetPartNum() > 1 )
							{
								int nPartnumCount = 0;
								BOOL bPolyTemp = FALSE;
								for(int j = 0 ; j < ((CGIS_FeaturePoly*)pTempFea)->GetPartNum()-1 ; j++)
								{
									nPartnumCount += ((CGIS_FeaturePoly*)pTempFea)->GetPart()[j];
									if(i == nPartnumCount-1)
									{
										bPolyTemp = TRUE;
										break;
									}
								}
								if(bPolyTemp)
								{
									continue;
								}
							}
							for(int j = 0 ; j < ((CGIS_FeatureLine*)(pFea))->GetPtNum() - 1 ; j++)
							{
								bIntersect = LineToLine(((CGIS_FeaturePoly*)(pTempFea))->GetPtList()[i],((CGIS_FeaturePoly*)(pTempFea))->GetPtList()[i+1],((CGIS_FeatureLine*)(pFea))->GetPtList()[j],((CGIS_FeatureLine*)(pFea))->GetPtList()[j+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
							if(bIntersect)
							{
								break;
							}
						}
					}
					//面
					if(pFea->GetObjType() == 4 || pFea->GetObjType() == 5)
					{
						BOOL bIntersect = FALSE;
						for(int i = 0 ; i < ((CGIS_FeaturePoly*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							//复杂多边形
							if(((CGIS_FeaturePoly*)pTempFea)->GetPartNum() > 1 )
							{
								int nPartnumCount = 0;
								BOOL bPolyTemp = FALSE;
								for(int j = 0 ; j < ((CGIS_FeaturePoly*)pTempFea)->GetPartNum()-1 ; j++)
								{
									nPartnumCount += ((CGIS_FeaturePoly*)pTempFea)->GetPart()[j];
									if(i == nPartnumCount-1)
									{
										bPolyTemp = TRUE;
										break;
									}
								}
								if(bPolyTemp)
								{
									continue;
								}
							}
							for(int j = 0 ; j < ((CGIS_FeaturePoly*)(pFea))->GetPtNum() - 1 ; j++)
							{
								//复杂多边形
								if(((CGIS_FeaturePoly*)pFea)->GetPartNum() > 1 )
								{
									int nPartnumCount = 0;
									BOOL bPolyTemp = FALSE;
									for(int k = 0 ; k < ((CGIS_FeaturePoly*)pFea)->GetPartNum()-1 ; k++)
									{
										nPartnumCount += ((CGIS_FeaturePoly*)pFea)->GetPart()[k];
										if(i == nPartnumCount-1)
										{
											bPolyTemp = TRUE;
											break;
										}
									}
									if(bPolyTemp)
									{
										continue;
									}
								}
								bIntersect = LineToLine(((CGIS_FeaturePoly*)(pTempFea))->GetPtList()[i],((CGIS_FeaturePoly*)(pTempFea))->GetPtList()[i+1],((CGIS_FeaturePoly*)(pFea))->GetPtList()[j],((CGIS_FeaturePoly*)(pFea))->GetPtList()[j+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
							if(bIntersect)
							{
								break;
							}
						}
					}
					//道路
					if(pFea->GetObjType() == 6 || pFea->GetObjType() == 7)
					{
						BOOL bIntersect = FALSE;
						for(int i = 0; i <  ((CGIS_FeaturePoly*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							for(int j = 0 ; j < ((CGIS_FeatureRut*)(pFea))->GetPtNum() - 1; j++)
							{
								bIntersect = LineToLine(((CGIS_FeaturePoly*)(pTempFea))->GetPtList()[i],((CGIS_FeaturePoly*)(pTempFea))->GetPtList()[i+1],((CGIS_FeatureRut*)(pFea))->GetPtList()[j],((CGIS_FeatureRut*)(pFea))->GetPtList()[j+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
							if(bIntersect)
							{
								break;
							}
						}
					}
				}

			}

			//道路
			if(pTempFea->GetObjType() == 6 || pTempFea->GetObjType() == 7)
			{
				if( EnSharpType == 0 || EnSharpType == 4)
				{
					//点
					if(pFea->GetObjType() == 0 || pFea->GetObjType() == 1)
					{
						for(int i = 0 ; i < ((CGIS_FeatureRut*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							if( PointOnLine(*pFea->GetPtList(),((CGIS_FeatureRut*)(pTempFea))->GetPtList()[i],((CGIS_FeatureRut*)(pTempFea))->GetPtList()[i+1]))
							{
								pReturnFeaSet.AddTail(pTempFea->Clone());
								break;
							}
						}
					}
					//线
					if(pFea->GetObjType() == 2 || pFea->GetObjType() == 3)
					{
						BOOL bIntersect = FALSE;
						for(int i = 0 ; i < ((CGIS_FeatureRut*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							for(int j = 0 ; j < ((CGIS_FeatureLine*)(pFea))->GetPtNum() - 1 ; j++)
							{
								bIntersect = LineToLine(((CGIS_FeatureRut*)(pTempFea))->GetPtList()[i],((CGIS_FeatureRut*)(pTempFea))->GetPtList()[i+1],((CGIS_FeatureLine*)(pFea))->GetPtList()[j],((CGIS_FeatureLine*)(pFea))->GetPtList()[j+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
							if(bIntersect)
							{
								break;
							}
						}
					}
					//面
					if(pFea->GetObjType() == 4 || pFea->GetObjType() == 5)
					{
						BOOL bIntersect = FALSE;
						for(int i = 0 ; i < ((CGIS_FeatureRut*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							for(int j = 0 ; j < ((CGIS_FeaturePoly*)(pFea))->GetPtNum() - 1 ; j++)
							{
								//复杂多边形
								if(((CGIS_FeaturePoly*)pFea)->GetPartNum() > 1 )
								{
									int nPartnumCount = 0;
									BOOL bPolyTemp = FALSE;
									for(int k = 0 ; k < ((CGIS_FeaturePoly*)pFea)->GetPartNum()-1 ; k++)
									{
										nPartnumCount += ((CGIS_FeaturePoly*)pFea)->GetPart()[k];
										if(i == nPartnumCount-1)
										{
											bPolyTemp = TRUE;
											break;
										}
									}
									if(bPolyTemp)
									{
										continue;
									}
								}
								bIntersect = LineToLine(((CGIS_FeatureRut*)(pTempFea))->GetPtList()[i],((CGIS_FeatureRut*)(pTempFea))->GetPtList()[i+1],((CGIS_FeaturePoly*)(pFea))->GetPtList()[j],((CGIS_FeaturePoly*)(pFea))->GetPtList()[j+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
							if(bIntersect)
							{
								break;
							}
						}
					}
					//道路
					if(pFea->GetObjType() == 6 || pFea->GetObjType() == 7)
					{
						BOOL bIntersect = FALSE;
						for(int i = 0; i <  ((CGIS_FeatureRut*)(pTempFea))->GetPtNum() - 1 ; i++)
						{
							for(int j = 0 ; j < ((CGIS_FeatureRut*)(pFea))->GetPtNum() - 1; j++)
							{
								bIntersect = LineToLine(((CGIS_FeatureRut*)(pTempFea))->GetPtList()[i],((CGIS_FeatureRut*)(pTempFea))->GetPtList()[i+1],((CGIS_FeatureRut*)(pFea))->GetPtList()[j],((CGIS_FeatureRut*)(pFea))->GetPtList()[j+1]);
								if(bIntersect)
								{
									pReturnFeaSet.AddTail(pTempFea->Clone());
									break;
								}
							}
							if(bIntersect)
							{
								break;
							}
						}
					}
				}
			}
		}
	}
	return TRUE;
}
BOOL CGIS_QueryByShape::QueryByIntersect(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,EnQueryObjType EnSharpType)
{
    TRACK_FUNCTION("QueryByIntersect 3");

	if(pFea == NULL)
	{
		return FALSE;
	}
	if(m_pMap != NULL)
	{
		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
//			for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( CalculateBMapID(pFea->GetPtList()[0].dx,pFea->GetPtList()[0].dy) );
				if( pBMap != NULL )
				{
//					if(!IsPtInRect(pFea->GetPtList()[0].dx,pFea->GetPtList()[0].dy,pBMap->m_rtBreadth.dLeft,pBMap->m_rtBreadth.dTop,pBMap->m_rtBreadth.dRight,pBMap->m_rtBreadth.dBottom))
//					{
//						continue;
//					}
//					m_pMap->LoadBreadthMap( pBMap );
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
							IGIS_Layer *pLayer = NULL;	
							short    nKey;
							while(pos1 != NULL)
							{
								pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
								if(pLayer != NULL)
								{
									CGIS_MapLayer *pBLList[1];

									pBLList[0] = (CGIS_MapLayer*)pLayer;
									if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
										return FALSE;
//									pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );

									CPtrList *pOList = NULL;
									pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
									if( !pOList )
									{
										pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
										return FALSE;
									}
									QueryByIntersect(pReturnFeaSet,pFea,pOList,EnSharpType);
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								}
							
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
					POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
					IGIS_Layer *pLayer = NULL;	
					short    nKey;
					while(pos1 != NULL)
					{
						pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
						if(pLayer != NULL)
						{
							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( 0, pOList );
							if( !pOList )
								return FALSE;
							QueryByIntersect(pReturnFeaSet,pFea,pOList,EnSharpType);
//							QueryByIntersect(pReturnFeaSet,pFea,pLayer->GetObjectList(),EnSharpType);
						}
					
					}

				}
			}
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CGIS_QueryByShape::QueryByIntersect(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,UINT ILayID,unsigned short ClassID,EnQueryObjType EnSharpType)
{
    TRACK_FUNCTION("QueryByIntersect 4");

	if(pFea == NULL)
	{
		return FALSE;
	}
	if(m_pMap != NULL)
	{
		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
//			for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( CalculateBMapID(pFea->GetPtList()[0].dx,pFea->GetPtList()[0].dy) );
				if( pBMap != NULL )
				{
//				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapList()[i];
//				if( pBMap != NULL )
//				{
//					if(!IsPtInRect(pFea->GetPtList()[0].dx,pFea->GetPtList()[0].dy,pBMap->m_rtBreadth.dLeft,pBMap->m_rtBreadth.dTop,pBMap->m_rtBreadth.dRight,pBMap->m_rtBreadth.dBottom))
//					{
//						continue;
//					}
//					m_pMap->LoadBreadthMap( pBMap );
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							IGIS_Layer *pLayer = NULL;	
							pLManager->GetLayerListMap()->Lookup( ILayID, pLayer );
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
									QueryByIntersect(pReturnFeaSet,pFea,pOList,EnSharpType);
								else
								{
									CPtrList pSet;
									QueryByIntersect(pSet,pFea,pOList,EnSharpType);

									POSITION pos1 = pSet.GetHeadPosition();
									CGIS_Feature*  pTempFea;									
									while( pos1 )
									{
										pTempFea = (CGIS_Feature*)(pSet.GetNext(pos1));
							
										if (pTempFea->GetObjType() == 0 )
										{
											CGIS_FeaturePoint* pTempFeaPoint = (CGIS_FeaturePoint*)pTempFea;										
											if (pTempFeaPoint->GetPointObjectType() == ClassID)
//											if (GetPoiID(pTempFeaPoint->GetPointObjectType(), ILayID) == ClassID)
											{
												pReturnFeaSet.AddTail(pTempFeaPoint);
											}
										}									
									}
									pSet.RemoveAll();								
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
					pLManager->GetLayerListMap()->Lookup( ILayID, pLayer );
					if(pLayer != NULL )
					{
						CPtrList *pOList = NULL;
						pLayer->GetObjectListMap()->Lookup( 0, pOList );
						if( !pOList )
							return FALSE;
						QueryByIntersect(pReturnFeaSet,pFea,pOList,EnSharpType);
					}

				}
			}
			return TRUE;
		}
	}
	return FALSE;
}
//包含查找
BOOL CGIS_QueryByShape::QueryByContain(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,CPtrList* pFeaSet)
{
    TRACK_FUNCTION("QueryByContain 1");

    if(pFea == NULL || pFeaSet == NULL)
	{
		return FALSE;
	}
	POSITION pos = pFeaSet->GetHeadPosition();
	CGIS_Feature* pTempFea;
	while(pos)
	{
		pTempFea = (CGIS_Feature*)(pFeaSet->GetNext(pos));
		if(pTempFea != NULL)
		{
			if(pTempFea->GetObjType() == 4 || pTempFea->GetObjType() == 5)
			{
				//点
				if(pFea->GetObjType() == 0 || pFea->GetObjType() == 1)
				{
					if(PointInPoly(*(pFea->GetPtList()),(CGIS_FeaturePoly*)pTempFea))
					{
						pReturnFeaSet.AddTail(pTempFea->Clone());
					}
				}
				//线
				if(pFea->GetObjType() == 2 || pFea->GetObjType() == 3)
				{
					if(MuLineInPoly((CGIS_FeatureLine*)(pFea),(CGIS_FeaturePoly*)pTempFea))
					{
						pReturnFeaSet.AddTail(pTempFea->Clone());
					}
				}
				//面
				if(pFea->GetObjType() == 4 || pFea->GetObjType() == 5)
				{
					if(PolyInPoly((CGIS_FeaturePoly*)pFea,(CGIS_FeaturePoly*)pTempFea))
					{
						pReturnFeaSet.AddTail(pTempFea->Clone());
					}
				}
				//道路
				if(pFea->GetObjType() == 6 || pFea->GetObjType() == 7)
				{
					BOOL bInPoly = FALSE;

					for(int i = 0 ; i < (((CGIS_FeatureRut*)pFea)->GetPtNum())- 1;i++)
					{
						bInPoly = LineInPoly(pFea->GetPtList()[i],pFea->GetPtList()[i+1],(CGIS_FeaturePoly*)pTempFea);
						if(!bInPoly)
						{
							break;
						}
					}
					if(bInPoly)
					{
						pReturnFeaSet.AddTail(pTempFea->Clone());
					}
				}
			}
		}
	}
	return TRUE;
}
BOOL CGIS_QueryByShape::QueryByContain(CPtrList& pReturnFeaSet,CGIS_Feature* pFea)
{
    TRACK_FUNCTION("QueryByContain 1");
	if(pFea == NULL)
	{
		return FALSE;
	}
	if(m_pMap != NULL)
	{
		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
//			for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( CalculateBMapID(pFea->GetPtList()[0].dx,pFea->GetPtList()[0].dy) );
				if( pBMap != NULL )
				{
//				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapList()[i];
//				if( pBMap != NULL )
//				{
//					if(!IsPtInRect(pFea->GetPtList()[0].dx,pFea->GetPtList()[0].dy,pBMap->m_rtBreadth.dLeft,pBMap->m_rtBreadth.dTop,pBMap->m_rtBreadth.dRight,pBMap->m_rtBreadth.dBottom))
//					{
//						continue;
//					}
//					m_pMap->LoadBreadthMap( pBMap );
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
							IGIS_Layer *pLayer = NULL;	
							short    nKey;
							while(pos1 != NULL)
							{
								pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
								if(pLayer != NULL)
								{
									CGIS_MapLayer *pBLList[1];

									pBLList[0] = (CGIS_MapLayer*)pLayer;
									if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
										return FALSE;
//									pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );

									CPtrList *pOList = NULL;
									pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
									if( !pOList )
									{
										pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
										return FALSE;
									}
									QueryByContain(pReturnFeaSet,pFea,pOList);
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								}
							
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
					POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
					IGIS_Layer *pLayer = NULL;
					short nKey;
					while(pos1 != NULL)
					{
						pLManager->GetLayerListMap()->GetNextAssoc( pos1,nKey, pLayer );
						if(pLayer != NULL)
						{
							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( 0, pOList );
							if( !pOList )
								return FALSE;
							QueryByContain(pReturnFeaSet,pFea,pOList);
//							QueryByContain(pReturnFeaSet,pFea,pLayer->GetObjectList());
						}
					
					}

				}
			}
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CGIS_QueryByShape::QueryByContain(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,UINT ILayID,unsigned short ClassID)
{
    TRACK_FUNCTION("QueryByContain 1");
	if(pFea == NULL)
	{
		return FALSE;
	}
	if(m_pMap != NULL)
	{

		if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
		{
//			for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
			{
				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( CalculateBMapID(pFea->GetPtList()[0].dx,pFea->GetPtList()[0].dy) );
				if( pBMap != NULL )
				{
//			for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
//			{
//				CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapList()[i];
//				if( pBMap != NULL )
//				{
//					if(!IsPtInRect(pFea->GetPtList()[0].dx,pFea->GetPtList()[0].dy,pBMap->m_rtBreadth.dLeft,pBMap->m_rtBreadth.dTop,pBMap->m_rtBreadth.dRight,pBMap->m_rtBreadth.dBottom))
//					{
//						continue;
//					}
//					m_pMap->LoadBreadthMap( pBMap );
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							IGIS_Layer *pLayer = NULL;	
							pLManager->GetLayerListMap()->Lookup( ILayID, pLayer );
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
									QueryByContain(pReturnFeaSet,pFea,pOList);
								else
								{
									CPtrList pSet;
									QueryByContain(pSet,pFea,pOList);

									POSITION pos1 = pSet.GetHeadPosition();
									CGIS_Feature*  pTempFea;									
									while( pos1 )
									{
										pTempFea = (CGIS_Feature*)(pSet.GetNext(pos1));
							
										if (pTempFea->GetObjType() == 0 )
										{
											CGIS_FeaturePoint* pTempFeaPoint = (CGIS_FeaturePoint*)pTempFea;										
											if (pTempFeaPoint->GetPointObjectType() == ClassID)
//											if (GetPoiID(pTempFeaPoint->GetPointObjectType(), ILayID) == ClassID)
											{
												pReturnFeaSet.AddTail(pTempFeaPoint);
											}
										}									
									}
									pSet.RemoveAll();									
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
					pLManager->GetLayerListMap()->Lookup( ILayID, pLayer );
					if( pLayer != NULL )
					{
						CPtrList *pOList = NULL;
						pLayer->GetObjectListMap()->Lookup( 0, pOList );
						if( !pOList )
							return FALSE;
						QueryByContain(pReturnFeaSet,pFea,pOList);
					}
//						QueryByContain(pReturnFeaSet,pFea,pLayer->GetObjectList());

	/*				POSITION pos1 = pLManager->GetLayerList()->GetHeadPosition();
					while(pos1 != NULL)
					{
						pLayer = (IGIS_Layer*)pLManager->GetLayerList()->GetNext(pos1);
						if(pLayer != NULL && pLayer->GetLayerInfo()->GetLayerID() == ILayID)
						{
							QueryByContain(pReturnFeaSet,pFea,pLayer->GetObjectList());
						}
					
					}
	*/
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CGIS_QueryByShape::QueryByIntersect(CPtrList& pReturnFeaSet,ST_INDEX_NAME *pRouteIndex )
{
    TRACK_FUNCTION("QueryByIntersect 5");

	if(m_pMap->GetMapFileType() == EN_MFTYPE_BAND)
	{
		CPtrList pListTemp;
		CGIS_BreadthMap *pBMap = NULL;
		for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ ) 
		{
//			pBMap = m_pMap->GetBreadthMapList()[i];
			pBMap = m_pMap->GetBreadthMapByID2( i );
			if( pBMap )
			{
				if( pBMap->m_nBMID == pRouteIndex->BandID )
					break;
			}
		}
		if (pBMap != NULL)
		{
			CGIS_LayerManager *pLManager = NULL;
			POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
			while(pos!= NULL)
			{
				pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
				if(pLManager != NULL)
				{
					IGIS_Layer *pLayer = NULL;	
					pLManager->GetLayerListMap()->Lookup(pRouteIndex->LayerID,pLayer);
					if(pLayer != NULL)
					{
						CGIS_MapLayer *pBLList[1];

						pBLList[0] = (CGIS_MapLayer*)pLayer;
						if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
							return FALSE;
//						pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );

						CPtrList *pOList = NULL;
						pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
						if( !pOList )
						{
							pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
							return FALSE;
						}

						swString strTempAnno;// = (swString)pRouteIndex->Name;
						CharToUnicode( pRouteIndex->Name , &strTempAnno );
						POSITION posOlist = pOList->GetHeadPosition();
						CGIS_Feature* pTempFea;
						while(posOlist)
						{
							pTempFea = (CGIS_Feature*)(pOList->GetNext(posOlist));
							if(pTempFea)
							{
								swString strTempFeaAnno;// = pTempFea->GetAnnotation();
								CharToUnicode( pTempFea->GetAnnotation() , &strTempFeaAnno );
								if(strTempFeaAnno.GetLength() >= strTempAnno.GetLength())
								{
									if( strTempFeaAnno.Find(strTempAnno)!=-1 )
									{
										pListTemp.AddTail(pTempFea->Clone());
									}
								}
							}
						}
						pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
					}
				}
			}
			if(pListTemp.GetCount() == 0)
				return FALSE;
			else
			{
				pLManager = NULL;
				pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
				while(pos!= NULL)
				{
					pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(pos);
					if(pLManager != NULL)
					{
						IGIS_Layer *pLayer = NULL;	
//						for(int i = 7 ; i < 13 ;i++)
						for(int i = 5 ; i < 12 ;i++)
						{
							pLManager->GetLayerListMap()->Lookup(i,pLayer);
							if(pLayer != NULL)
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
									continue;
								}
								QueryByIntersect(pReturnFeaSet,&pListTemp,pOList);
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
							}
						}
					}
				}
				CGIS_FeatureRut *pRt;
				POSITION pspt = pListTemp.GetHeadPosition();
				while(pspt)
				{
					pRt = (CGIS_FeatureRut*)pListTemp.GetNext(pspt);
					delete pRt;
					pRt = NULL;
				}
				pListTemp.RemoveAll();
			}
		}
		else
		{
			return FALSE;
		}
		
	}

	return TRUE;
}
/*
BOOL CGIS_QueryByShape::QueryByIntersect(CPtrList& pReturnFeaSet,CPtrList* pFeaSet)
{
	if(!pFeaSet || pFeaSet->GetCount() == 0)
		return FALSE;
	CPtrList listTemp;
	POSITION pos = pFeaSet->GetHeadPosition();
	while(pos)
	{
		CGIS_FeatureRut *pFeaRut =(CGIS_FeatureRut*)pFeaSet->GetNext(pos);
		POSITION posCur = pos;
		while(posCur)
		{
			CGIS_FeatureRut *pFeaRut1 = (CGIS_FeatureRut*)pFeaSet->GetNext(posCur);
			if(((swString)pFeaRut1->GetAnnotation()) == ((swString)pFeaRut->GetAnnotation()))
				continue;
			ST_ROAD_PT *pRoadPt = NULL;
			ST_GEO_PTXY ptHead = pFeaRut->GetPtList()[0];
			ST_GEO_PTXY ptTail = pFeaRut->GetPtList()[pFeaRut->GetPtNum() -1 ];
			
			ST_GEO_PTXY ptHead1 = pFeaRut1->GetPtList()[0];
			ST_GEO_PTXY ptTail1 = pFeaRut1->GetPtList()[pFeaRut->GetPtNum()-1];
			if( (fabs(ptHead.dx - ptHead1.dx) < FLOAT_ZERO && fabs(ptHead.dy-ptHead1.dy) < FLOAT_ZERO) || (fabs(ptHead.dx-ptTail1.dx) < FLOAT_ZERO && fabs(ptHead.dy-ptTail1.dy) < FLOAT_ZERO) )
			{
				pRoadPt = new ST_ROAD_PT;
				swString strRoad1,strRoad2;
				strRoad1 = (swString)pFeaRut->GetAnnotation();
				strRoad2 = (swString)pFeaRut1->GetAnnotation();
				if(strRoad1.IsEmpty())
					strRoad1 = L"路";
				if(strRoad2.IsEmpty())
					strRoad2 = L"路";
				pRoadPt->Name = strRoad1 + L"*" + strRoad2;
				pRoadPt->pt.dx = ptHead.dx;
				pRoadPt->pt.dy = ptHead.dy;
			}
			if( (fabs(ptTail.dx - ptHead1.dx) < FLOAT_ZERO && fabs(ptTail.dy-ptHead1.dy) < FLOAT_ZERO) || (fabs(ptTail.dx-ptTail1.dx) < FLOAT_ZERO && fabs(ptTail.dy-ptTail1.dy) < FLOAT_ZERO) )
			{
				pRoadPt = new ST_ROAD_PT;
				swString strRoad1,strRoad2;
				strRoad1 = (swString)pFeaRut->GetAnnotation();
				strRoad2 = (swString)pFeaRut1->GetAnnotation();
				if(strRoad1.IsEmpty())
					strRoad1 = L"路";
				if(strRoad2.IsEmpty())
					strRoad2 = L"路";
				pRoadPt->Name = strRoad1 + L"*" + strRoad2;
				pRoadPt->pt.dx = ptTail.dx;
				pRoadPt->pt.dy = ptTail.dy;
			}
			if(pRoadPt)
			{
				POSITION posIN = listTemp.GetHeadPosition();
				BOOL BAdd = TRUE;
				ST_ROAD_PT *temp;
				while( posIN )
				{
					temp = (ST_ROAD_PT*)listTemp.GetNext(posIN);
					if (fabs(temp->pt.dx - pRoadPt->pt.dx)<FLOAT_ZERO && fabs(temp->pt.dy - pRoadPt->pt.dy)<FLOAT_ZERO)
					{
						delete pRoadPt;
						pRoadPt = NULL;
						BAdd = FALSE;
						if( temp->Name.Find( (swString)pFeaRut1->GetAnnotation() ) == -1 )
						{
							temp->Name = temp->Name + L"*" + pFeaRut1->GetAnnotation();
							break;
						}
					}
						
				}
				if (BAdd == TRUE)
					listTemp.AddTail(pRoadPt);
			}
		}
	}
	if(listTemp.GetCount() > 0)
		pReturnFeaSet.AddTail(&listTemp);
	return TRUE;
}
*/
BOOL CGIS_QueryByShape::QueryByIntersect( CPtrList& pReturnFeaSet , CPtrList* pIndexList , EnIntersectType EnType)
{
    //////////////////////////////////////////////////////////////////////////
    CSpaceMemoryLock aLock;    
    TRACK_FUNCTION("QueryByIntersect 6");
    //////////////////////////////////////////////////////////////////////////
    

	m_bStopSearch = FALSE;
	if( !pIndexList || pIndexList->GetCount() == 0 )
		return FALSE;
	if( !m_pMap )
		return FALSE;
	POSITION pos = pIndexList->GetHeadPosition();
	ST_INDEX_NAME *pIndex = NULL;
	//同一图幅中所有返回值的临时链表
	CPtrList listTempFeature;
	//已经找过的项
	CPtrList listBeFind;

	while( pos )
	{
		pIndex = (ST_INDEX_NAME*)pIndexList->GetNext( pos );
		CPtrList pListTemp;
		CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( pIndex->BandID );
		if( !pBMap )
			return FALSE;
		//该项是否已经检索过了
		POSITION posFind = listBeFind.GetHeadPosition();
		BOOL bFind = FALSE;
		while( posFind )
		{
			POSITION posMem = posFind;
			ST_INDEX_NAME* pIndexFind = (ST_INDEX_NAME*)listBeFind.GetNext( posFind );
			if( pIndexFind == pIndex )
			{
				bFind = TRUE;
				listBeFind.RemoveAt( posMem );
				break;
			}
		}
		if( bFind )
			continue;
		CGIS_LayerManager *pLManager = NULL;
		POSITION posLayer = m_pMap->m_LayerManagerSet->GetHeadPosition();
		while( posLayer )
		{
			pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(posLayer);
			if( pLManager )
			{
				//加载所有该图幅的道路图层
//				DWORD dwTime = GetTickCount();
				for(int i = 5 ; i < 12 ;i++)
				{
					IGIS_Layer *pLayer = NULL;	
					pLManager->GetLayerListMap()->Lookup(i,pLayer);
					if( pLayer )
					{
						CGIS_MapLayer *pBLList[1];
						pBLList[0] = (CGIS_MapLayer*)pLayer;
						if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
							continue;
//						pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );
					}
					if( m_bStopSearch )
						break;
				}
//				TRACE( L"LOAD TIME: %d \n " , GetTickCount() - dwTime );
				//检索所查道路数据
				IGIS_Layer *pLayer = NULL;	
				pLManager->GetLayerListMap()->Lookup(pIndex->LayerID,pLayer);
				if( pLayer )
				{
					CPtrList *pOList = NULL;
					pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );

					swString strTempAnno;// = (swString)pIndex->Name;
					CharToUnicode( pIndex->Name , &strTempAnno );
					POSITION posObj = pOList->GetHeadPosition();
					CGIS_Feature* pTempFea;
					while(posObj)
					{
						pTempFea = (CGIS_Feature*)(pOList->GetNext(posObj));
						if(pTempFea)
						{
							swString strTempFeaAnno ;//= pTempFea->GetAnnotation();
							CharToUnicode( pTempFea->GetAnnotation() , &strTempFeaAnno );
							if(strTempFeaAnno.GetLength() >= strTempAnno.GetLength())
							{
								if( strTempFeaAnno.Find(strTempAnno)!=-1 )
									listTempFeature.AddTail( pTempFea );
							}
						}
					}
//					pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
				}
			}
		}
		//检索所有的交叉路口
		POSITION posFeature = listTempFeature.GetHeadPosition();
		CGIS_FeatureRut *pTempFeaRut,*pTempFeaRut1;
//		DWORD dwTime = GetTickCount();
		int nCount = 0;
		while( posFeature )
		{
			pTempFeaRut = (CGIS_FeatureRut*)listTempFeature.GetNext( posFeature );
			if(!pTempFeaRut)
				continue;
			ST_GEO_RECT rtSource;
			ST_GEO_PTXY ptHead = pTempFeaRut->GetPtList()[0];
			ST_GEO_PTXY ptTail = pTempFeaRut->GetPtList()[pTempFeaRut->GetPtNum() - 1];
			rtSource.dLeft = pTempFeaRut->GetRectObj().dLeft - 0.001;
			rtSource.dRight = pTempFeaRut->GetRectObj().dRight + 0.001;
			rtSource.dTop = pTempFeaRut->GetRectObj().dTop - 0.001;
			rtSource.dBottom = pTempFeaRut->GetRectObj().dBottom + 0.001;
			swString strHead , strTail;
			strHead.Empty();
			strTail.Empty();
			//0:无相交道路　1：有相交道路，但是无路名　2：有相交道路，也有路名
			int nHead,nTail;
			nHead = nTail = 0;
			pLManager = NULL;
			posLayer = m_pMap->m_LayerManagerSet->GetHeadPosition();
			while( posLayer )
			{
				pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(posLayer);
				if(pLManager != NULL)
				{
					IGIS_Layer *pLayer = NULL;	
					for(int i = 5 ; i < 12 ;i++)
					{
						pLManager->GetLayerListMap()->Lookup(i,pLayer);
						if(pLayer != NULL)
						{
							CPtrList *pOList = NULL;
							pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
							if( !pOList || pOList->GetCount() == 0 )
								continue;
							POSITION posObj = pOList->GetHeadPosition();
							BOOL bFinish = FALSE;
							while( posObj )
							{
								nCount++;
								pTempFeaRut1 = (CGIS_FeatureRut*)pOList->GetNext( posObj );
								if( !pTempFeaRut1 )
									continue;
								ST_GEO_PTXY ptHead1 = pTempFeaRut1->GetPtList()[0];
								ST_GEO_PTXY ptTail1 = pTempFeaRut1->GetPtList()[pTempFeaRut1->GetPtNum() - 1];
								if( !IsPtInRect(ptHead1.dx , ptHead1.dy , rtSource.dLeft,rtSource.dTop,rtSource.dRight , rtSource.dBottom )
									&&!IsPtInRect(ptTail1.dx , ptTail1.dy , rtSource.dLeft,rtSource.dTop,rtSource.dRight , rtSource.dBottom ))
									continue;
								swString strRut1 , strRut2;
								CharToUnicode( pTempFeaRut->GetAnnotation() , &strRut1 );
								CharToUnicode( pTempFeaRut1->GetAnnotation() , &strRut2 );
								if( strRut1 == strRut2 )
									continue;
								
								if(nHead!=2 && ((fabs(ptHead.dx - ptHead1.dx) < FLOAT_ZERO && fabs(ptHead.dy-ptHead1.dy) < FLOAT_ZERO) || (fabs(ptHead.dx-ptTail1.dx) < FLOAT_ZERO && fabs(ptHead.dy-ptTail1.dy) < FLOAT_ZERO)) )
								{
//									strHead = pTempFeaRut1->GetAnnotation();
									CharToUnicode( pTempFeaRut1->GetAnnotation() , &strHead );
									if( !strHead.IsEmpty() )
										nHead = 2;
									else
										nHead = 1;
								}
								if(nTail!=2 && ((fabs(ptTail.dx - ptHead1.dx) < FLOAT_ZERO && fabs(ptTail.dy-ptHead1.dy) < FLOAT_ZERO) || (fabs(ptTail.dx-ptTail1.dx) < FLOAT_ZERO && fabs(ptTail.dy-ptTail1.dy) < FLOAT_ZERO)) )
								{
//									strTail = pTempFeaRut1->GetAnnotation();
									CharToUnicode( pTempFeaRut1->GetAnnotation() , &strTail );
									if( !strTail.IsEmpty() )
										nTail = 2;
									else
										nTail = 1;

								}
								if( nHead==2 && nTail==2 )
								{
									bFinish = TRUE;
									break;
								}
							}
							if( bFinish )
								break;
						}
						if( m_bStopSearch )
							break;
					}
				}
				if( m_bStopSearch )
					break;
			}
			//至少有一条交叉路口有路名
//			if( bHead || bTail )
			{
				if( nHead!=2 )
					strHead = L"路";
				if( nTail!=2 )
					strTail = L"路";
				if( EnType == EN_ROUTE )
				{
					ST_ROAD_INNAME *pRoadIn = new ST_ROAD_INNAME;
					pRoadIn->pRoute = (CGIS_FeatureRut*)pTempFeaRut->Clone();
					if( nHead == 0 )
						pRoadIn->InName = strTail;
					else if( nTail == 0 )
						pRoadIn->InName = strHead;
					else
						pRoadIn->InName = strHead + L"*" + strTail;
					pReturnFeaSet.AddTail( pRoadIn );
				}
				else
				{
					swString strName;
					for( int i = 0 ; i < 2 ; i++ )
					{
						if( i == 0 )//路首
						{
							if( nHead == 0 ) 
								continue;
							strName = strHead;
						}
						else //路尾
						{
							if( nTail == 0 )
								continue;
							strName = strTail;
						}
						//防止重命名
						POSITION posTarget = pReturnFeaSet.GetHeadPosition();
						ST_ROAD_PT *pRoadPt = NULL;
						BOOL bFindSame = FALSE;
						while( posTarget )
						{
							pRoadPt = (ST_ROAD_PT*)pReturnFeaSet.GetNext( posTarget );
							if( pRoadPt->Name == strName )
							{
								bFindSame = TRUE;
								break;
							}
						}
						if( !bFindSame )
						{
							pRoadPt = new ST_ROAD_PT;
							if( i == 0 )
								pRoadPt->pt = ptHead;
							else
								pRoadPt->pt = ptTail;
							pRoadPt->Name = strName;
							pReturnFeaSet.AddTail( pRoadPt );
						}
					}
				}
			}
		}
		TRACE( L"SEARCH Count : %d , \n" , nCount );
		listTempFeature.RemoveAll();
		//查找所有在该图幅的道路及交叉路口
		POSITION posAgain = pos;
		ST_INDEX_NAME *pIndexAgain = NULL;
		while( posAgain )
		{
			pIndexAgain = (ST_INDEX_NAME*)pIndexList->GetNext( posAgain );
			if( !pIndexAgain )
				continue;
			if( pIndexAgain->BandID == pIndex->BandID )
			{
				IGIS_Layer* pLayer = NULL;
				if( !pLManager )
					break;
				pLManager->GetLayerListMap()->Lookup(pIndexAgain->LayerID,pLayer);
				if( pLayer )
				{
					CPtrList *pOList = NULL;
					pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );

					swString strTempAnno;// = (swString)pIndex->Name;
					CharToUnicode( pIndex->Name , &strTempAnno );
					POSITION posObj = pOList->GetHeadPosition();
					CGIS_Feature* pTempFea;
					while(posObj)
					{
						pTempFea = (CGIS_Feature*)(pOList->GetNext(posObj));
						if(pTempFea)
						{
							swString strTempFeaAnno;// = pTempFea->GetAnnotation();
							CharToUnicode( pTempFea->GetAnnotation() , &strTempFeaAnno );
							if(strTempFeaAnno.GetLength() >= strTempAnno.GetLength())
							{
								if( strTempFeaAnno.Find(strTempAnno)!=-1 )
									listTempFeature.AddTail( pTempFea );
							}
						}
					}
				}
				POSITION posFeature = listTempFeature.GetHeadPosition();
				CGIS_FeatureRut *pTempFeaRut,*pTempFeaRut1;
				while( posFeature )
				{
					pTempFeaRut = (CGIS_FeatureRut*)listTempFeature.GetNext( posFeature );
					if(!pTempFeaRut)
						continue;
					ST_GEO_RECT rtSource;
					ST_GEO_PTXY ptHead = pTempFeaRut->GetPtList()[0];
					ST_GEO_PTXY ptTail = pTempFeaRut->GetPtList()[pTempFeaRut->GetPtNum() - 1];
					rtSource.dLeft = pTempFeaRut->GetRectObj().dLeft - 0.001;
					rtSource.dRight = pTempFeaRut->GetRectObj().dRight + 0.001;
					rtSource.dTop = pTempFeaRut->GetRectObj().dTop - 0.001;
					rtSource.dBottom = pTempFeaRut->GetRectObj().dBottom + 0.001;
					swString strHead , strTail;
					strHead.Empty();
					strTail.Empty();
					int nHead,nTail;
					nHead = nTail = 0;
					pLManager = NULL;
					posLayer = m_pMap->m_LayerManagerSet->GetHeadPosition();
					while( posLayer )
					{
						pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(posLayer);
						if(pLManager != NULL)
						{
							IGIS_Layer *pLayer2 = NULL;	
							for(int i = 5 ; i < 12 ;i++)
							{
								pLManager->GetLayerListMap()->Lookup(i,pLayer2);
								if(pLayer2 != NULL)
								{
									CPtrList *pOList = NULL;
									pLayer2->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
									if( !pOList || pOList->GetCount() == 0 )
										continue;
									POSITION posObj = pOList->GetHeadPosition();
									BOOL bFinish = FALSE;
									while( posObj )
									{
										pTempFeaRut1 = (CGIS_FeatureRut*)pOList->GetNext( posObj );
										if( !pTempFeaRut1 )
											continue;
										ST_GEO_PTXY ptHead1 = pTempFeaRut1->GetPtList()[0];
										ST_GEO_PTXY ptTail1 = pTempFeaRut1->GetPtList()[pTempFeaRut1->GetPtNum() - 1];
										if( !IsPtInRect(ptHead1.dx , ptHead1.dy , rtSource.dLeft,rtSource.dTop,rtSource.dRight , rtSource.dBottom )
											&&!IsPtInRect(ptTail1.dx , ptTail1.dy , rtSource.dLeft,rtSource.dTop,rtSource.dRight , rtSource.dBottom ))
											continue;
										swString strRut1 , strRut2;
										CharToUnicode( pTempFeaRut->GetAnnotation() , &strRut1 );
										CharToUnicode( pTempFeaRut1->GetAnnotation() , &strRut2 );
										if( strRut1 == strRut2 )
											continue;
										
										if(nHead!=2 && ((fabs(ptHead.dx - ptHead1.dx) < FLOAT_ZERO && fabs(ptHead.dy-ptHead1.dy) < FLOAT_ZERO) || (fabs(ptHead.dx-ptTail1.dx) < FLOAT_ZERO && fabs(ptHead.dy-ptTail1.dy) < FLOAT_ZERO)) )
										{
//											strHead = pTempFeaRut1->GetAnnotation();
											CharToUnicode( pTempFeaRut1->GetAnnotation() , &strHead );
											if( !strHead.IsEmpty() )
												nHead = 2;
											else
												nHead = 1;
										}
										if(nTail!=2 && ((fabs(ptTail.dx - ptHead1.dx) < FLOAT_ZERO && fabs(ptTail.dy-ptHead1.dy) < FLOAT_ZERO) || (fabs(ptTail.dx-ptTail1.dx) < FLOAT_ZERO && fabs(ptTail.dy-ptTail1.dy) < FLOAT_ZERO)) )
										{
//											strTail = pTempFeaRut1->GetAnnotation();
											CharToUnicode( pTempFeaRut1->GetAnnotation() , &strTail );
											if( !strTail.IsEmpty() )
												nTail = 2;
											else
												nTail = 1;
										}
										if( nHead==2 && nTail==2 )
										{
											bFinish = TRUE;
											break;
										}
										if( m_bStopSearch )
											break;
									}
									if( bFinish )
										break;
								}
								if( m_bStopSearch )
									break;
							}
						}
						if( m_bStopSearch )
							break;
					}
					//至少有一条交叉路口有路名
//					if( bHead || bTail )
					{
						if( nHead != 2  )
							strHead = L"路";
						if( nTail != 2 )
							strTail = L"路";
						if( EnType == EN_ROUTE )
						{
							ST_ROAD_INNAME *pRoadIn = new ST_ROAD_INNAME;
							pRoadIn->pRoute = (CGIS_FeatureRut*)pTempFeaRut->Clone();
							if( nHead == 0 )
								pRoadIn->InName = strTail;
							else if( nTail == 0 )
								pRoadIn->InName = strHead;
							else
								pRoadIn->InName = strHead + L"*" + strTail;
							pReturnFeaSet.AddTail( pRoadIn );
						}
						else
						{
							swString strName;
							for( int i = 0 ; i < 2 ; i++ )
							{
								if( i == 0 )//路首
								{
									if( nHead == 0 ) 
										continue;
									strName = strHead;
								}
								else //路尾
								{
									if( nTail == 0 )
										continue;
									strName = strTail;
								}
								//防止重命名
								POSITION posTarget = pReturnFeaSet.GetHeadPosition();
								ST_ROAD_PT *pRoadPt = NULL;
								BOOL bFindSame = FALSE;
								while( posTarget )
								{
									pRoadPt = (ST_ROAD_PT*)pReturnFeaSet.GetNext( posTarget );
									if( pRoadPt->Name == strName )
									{
										bFindSame = TRUE;
										break;
									}
								}
								if( !bFindSame )
								{
									pRoadPt = new ST_ROAD_PT;
									if( i == 0 )
										pRoadPt->pt = ptHead;
									else
										pRoadPt->pt = ptTail;
									pRoadPt->Name = strName;
									pReturnFeaSet.AddTail( pRoadPt );
								}
							}
						}

					}
					if( m_bStopSearch )
						break;
				}
				listBeFind.AddTail( pIndexAgain );
				listTempFeature.RemoveAll();
			}
			if( m_bStopSearch )
				break;
		}
		//清理内存
		pLManager = NULL;
		posLayer = m_pMap->m_LayerManagerSet->GetHeadPosition();
		while( posLayer )
		{
			pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext(posLayer);
			if( pLManager )
			{
				//卸载所有该图幅的道路图层
				for(int i = 5 ; i < 12 ;i++)
				{
					IGIS_Layer *pLayer = NULL;	
					pLManager->GetLayerListMap()->Lookup(i,pLayer);
					if( pLayer )
						( (CGIS_MapLayer*)pLayer )->DecreaseLCount( pBMap->m_nBreadthID );
				}
			}
		}
		if( m_bStopSearch )
			break;
	}
	return TRUE;
}
BOOL CGIS_QueryByShape::QueryByIntersect(CPtrList& pReturnFeaSet, int nCityID, swString strRoad1,swString strRoad2)
{
    TRACK_FUNCTION("QueryByIntersect 7");

	m_bStopSearch = FALSE;
	m_QueryByAtt.SetMap(m_pMap);
	if(strRoad1.IsEmpty() && strRoad2.IsEmpty())
	{
		return FALSE;
	}
	swString strRoad;
	BOOL bOneRoad = FALSE;
	if(strRoad1.IsEmpty())
	{
		strRoad = strRoad2;
		bOneRoad = TRUE;
	}
	if(strRoad2.IsEmpty())
	{
		strRoad = strRoad1;
		bOneRoad = TRUE;
	}
	
	CPtrList list;
	CPtrList list1,list2;
	if(bOneRoad)
	{
		m_QueryByAtt.GetNameNIN(list,m_pMap->GetMapInfo()->GetMapFilePath(), nCityID, strRoad,QUERY_INDEX_ROAD);
		POSITION pos = list.GetHeadPosition();
		ST_INDEX_NAME *index = NULL;
		while(pos)
		{
			index = (ST_INDEX_NAME*)list.GetNext(pos);
			QueryByIntersect(pReturnFeaSet,index);
			delete index;
			index = NULL;
		}
		list.RemoveAll();
	}
	else
	{
		m_QueryByAtt.GetNameNIN(list1,m_pMap->GetMapInfo()->GetMapFilePath(), nCityID, strRoad1,QUERY_INDEX_ROAD);
		m_QueryByAtt.GetNameNIN(list2,m_pMap->GetMapInfo()->GetMapFilePath(), nCityID, strRoad2,QUERY_INDEX_ROAD);
		POSITION pos1 = list1.GetHeadPosition();
		ST_INDEX_NAME *index1 = NULL;
		ST_INDEX_NAME *index2 = NULL;
		while(pos1)
		{
			index1 = (ST_INDEX_NAME*)list1.GetNext(pos1);
			POSITION pos2 = list2.GetHeadPosition();
			while(pos2)
			{
				index2 = (ST_INDEX_NAME*)list2.GetNext(pos2);
				if(index1->BandID == index2->BandID)
				{
					CPtrList listStreet1;
					CPtrList listStreet2;
					swString strStreet1 , strStreet2;
					CharToUnicode( index1->Name , &strStreet1 );
					CharToUnicode( index2->Name , &strStreet2 );
					m_QueryByAtt.QueryByAnnotation(listStreet1,index1->BandID,index1->LayerID,index1->ClassID,strStreet1);
					m_QueryByAtt.QueryByAnnotation(listStreet2,index2->BandID,index2->LayerID,index2->ClassID,strStreet2);
					QueryByIntersect(pReturnFeaSet,&listStreet1,&listStreet2);
					POSITION posSt1 = listStreet1.GetHeadPosition();
					CGIS_FeatureRut *pRut;
					while(posSt1)
					{
						pRut = (CGIS_FeatureRut*)listStreet1.GetNext(posSt1);
						delete pRut;
						pRut = NULL;
					}
					posSt1 = listStreet2.GetHeadPosition();
					while(posSt1)
					{
						pRut = (CGIS_FeatureRut*)listStreet2.GetNext(posSt1);
						delete pRut;
						pRut = NULL;
					}
					listStreet1.RemoveAll();
					listStreet2.RemoveAll();

				}
				if( m_bStopSearch )
					break;
			}
			if( m_bStopSearch )
				break;
		}
		//清理现场
		pos1 = list1.GetHeadPosition();
		ST_INDEX_NAME *index = NULL;
		while( pos1 )
		{
			index = (ST_INDEX_NAME*)list1.GetNext(pos1);
			delete[] index->Name;
			delete index;
			index = NULL;
		}
		list1.RemoveAll();

		POSITION pos2 = list2.GetHeadPosition();
		while( pos2 )
		{
			index = (ST_INDEX_NAME*)list2.GetNext(pos2);
			delete[] index->Name;
			delete index;
			index = NULL;
		}
		list2.RemoveAll();
	}
	return TRUE;
}
EnQueryObjType CGIS_QueryByShape::MatchLayerType(int nLayerID)
{
	if( (nLayerID>=0 && nLayerID<=3) )
		return EN_OBJ_POLY;
	if( nLayerID>=5 && nLayerID<=11 )
		return EN_OBJ_RUT;
	if(  nLayerID==4 )
		return EN_OBJ_LINE;
	if( nLayerID>=12 && nLayerID<=23 )
		return EN_OBJ_PT;
	return EN_OBJ_ALL;
}
void CGIS_QueryByShape::SetbStopSearch( BOOL bStopSearch )
{
	m_bStopSearch = bStopSearch;
}
void CGIS_QueryByShape::SetBNearNetWork( BOOL bNearNetWork )
{
	m_bNearNetWork = bNearNetWork;
}
void CGIS_QueryByShape::SetNetWorkRoute( CGIS_NetworkRoute* pNetworkRoute )
{
	if( m_pNetworkRoute != pNetworkRoute )
	{
		m_nNearRouteBID = 0;
		m_pNetworkRoute = pNetworkRoute;
		if( m_pNetworkRoute )
		{
			if( !m_pMap )
				return;
			if( m_pMap->GetMapFileType() != EN_MFTYPE_BAND )
				return;
			CPtrList *pPathPointList = NULL;
			CGIS_NetworkPath *pNetworkPath = NULL;
			pNetworkPath = m_pNetworkRoute->GetNetworkPath();
			if( !pNetworkPath )
				return;
			pPathPointList = pNetworkPath->GetPathpointList();
			if( !pPathPointList )
				return;
			CGIS_Pathpoint *pPathPoint = NULL;
			EnPathpointType nType;
			POSITION pos = pPathPointList->GetHeadPosition();
			while( pos )
			{
				pPathPoint = (CGIS_Pathpoint*)pPathPointList->GetNext(pos);
				if( pPathPoint )
				{
					nType = pPathPoint->GetPathpointType();
					double dbDistance = 1000;
					double dis;
					ST_GEO_RECT TagetRect;
					ST_GEO_PTXY Pt = pPathPoint->GetPathpointPosition();
					if( nType != EN_PATHPOINTTYPE_INFLEXION )
					{
						
						dis = dbDistance / 1852.0 / 60.0;
						TagetRect.dLeft = Pt.dx - dis;
						TagetRect.dRight = Pt.dx + dis;
						TagetRect.dBottom = Pt.dy + dis;
						TagetRect.dTop = Pt.dy - dis;
						NormalizeRect(TagetRect);

						//临时图幅列表
						int nBMIDs[20];
						memset( nBMIDs, 0, sizeof(int)*20);
						int nBNum = 0;
						CalculateBMapIDInRect(TagetRect, nBMIDs, nBNum );
						//与已存在的图幅比较，删除重复图幅
						int nID;
						int nOldID;
						BOOL bAdd;
						for( int i = 0 ; i < nBNum ; i++ )
						{
							nID = nBMIDs[i];
							bAdd = TRUE;
							for( int j = 0 ; j < m_nNearRouteBID ; j++ )
							{
								nOldID = m_NearRouteBIDs[j];
								if( nID == nOldID )
								{
									bAdd = FALSE;
									break;
								}
							}
							if( bAdd )
							{
								m_NearRouteBIDs[m_nNearRouteBID] = nID;
								m_nNearRouteBID++;
							}
							
						}
					}
				}
			}
		}
	}
}
static HANDLE gHLFile = INVALID_HANDLE_VALUE;
BOOL CGIS_QueryByShape::LoadPOIByName( CPtrList& pReTurnFeaSet , CGIS_GeoMap *pMap, 
									  CGIS_BreadthMap *pBMap, CGIS_MapLayer **pLList, int nLNum, swString strName )
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
		// 由于读取数据可能失败，加上判断保证读出的是预定的数据

		CFile file(gHLFile);
		file.SeekToBegin();
		char szInfo[32];

		if (9 != file.Read( szInfo, 9 ))
			return FALSE;

		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;

		unsigned int nSize = sizeof(int);
		if (nSize != file.Read( &nMNum, nSize ))
			return FALSE;
		int i;
		for( i=0;	i<nMNum;	i++ )
		{
			if (nSize != file.Read( &nMID, nSize ))
				return FALSE;
			if (nSize != file.Read( &nMOff, nSize ))
				return FALSE;
			if( nMID == pMap->GetMapInfo()->GetMapID() )
				break;
		}
		int nBID = 0;
		int nBOff = nMOff + 13 + (pBMap->m_nBreadthID)*8;
		file.Seek( nBOff, CFile::begin );
		
		if (nSize != file.Read( &nBID, nSize ))
			return FALSE;
		if (nSize != file.Read( &nBOff, nSize ))
			return FALSE;
		file.Seek( nMOff+nBOff, CFile::begin );
		
		CGIS_MapLayer *pLayer = NULL;
		DWORD dwStation = 0;
		for( i=0;	i<nLNum;	i++ )
		{
			pLayer = pLList[i];
			ASSERT( pLayer );
			dwStation = nMOff+nBOff + pLayer->GetLayerInfo()->GetLayerID()*16;
			int nLInfo[4];
			file.Seek( dwStation, CFile::begin );

			if (nSize*4 != file.Read( nLInfo, nSize*4 ))
				return FALSE;
			
			ASSERT( nLInfo[0] == pLayer->GetLayerInfo()->GetLayerID() );
			if( (nLInfo[2] == 0 || nLInfo[3] == 0) ||
					(nLInfo[2] == 8 && nLInfo[3] == 8 ) )
				continue;
			pFBuf = new char[nLInfo[2]];
			if( pFBuf == 0 )
				ASSERT(FALSE);
			char *pBuf = new char[nLInfo[3]];
			if( pBuf == 0 )
				ASSERT(FALSE);
			dwStation = nMOff+nBOff + nLInfo[1];
			file.Seek( dwStation, CFile::begin );
//			DWORD dwTime = GetTickCount();
			file.Read( pBuf, nLInfo[3] );
			DWORD dwSize = nLInfo[2];
			uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nLInfo[3] );
//			m_dwUnZipIO += ( GetTickCount() - dwTime );
			delete [] pBuf;
			
			dwStation = 0;
			BOOL bRutType = FALSE;
			short sIconID = pLayer->GetLayerInfo()->GetIconID();
			while( dwSize > dwStation )
			{
				int nLID = 0;
				Fread( pFBuf, dwSize, dwStation, &nLID, sizeof(int), 1 );
				int nCount;
				Fread( pFBuf, dwSize, dwStation, &nCount, sizeof(int), 1 );
				BYTE bType = 0;
				for( int j=0;	j< nCount;	j++ )
				{
					Fread( pFBuf, dwSize, dwStation, &bType, sizeof(BYTE), 1 );
//					DWORD dwTime = GetTickCount();
					if ( !ReadPOIByName( pFBuf, dwSize, dwStation, pReTurnFeaSet, strName ) )
						continue;

//					m_dwRead += ( GetTickCount() - dwTime );
				}
			}
			delete[] pFBuf;
		}
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
		if(INVALID_HANDLE_VALUE != gHLFile)
			CloseHandle( gHLFile );
		gHLFile = INVALID_HANDLE_VALUE;
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
void CGIS_QueryByShape::SortPOI(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList& pFeaSet)
{
	if(pPt == NULL )
		return ;
	POSITION pos = pFeaSet.GetHeadPosition();
	CGIS_FeaturePoint* pTempFea;
	while(pos)
	{
		pTempFea = (CGIS_FeaturePoint*)(pFeaSet.GetNext(pos));
		if(pTempFea != NULL)
		{
			double dis = CalcDist(pPt->dx,pPt->dy,pTempFea->GetPtList()->dx,pTempFea->GetPtList()->dy);
			ST_FEATURE_POINT *pFeatureST = new ST_FEATURE_POINT;
			pFeatureST->pFeature = pTempFea->Clone();
			pFeatureST->dbDis = dis;
			//排序的过程
			POSITION possort = pReturnFeaSet.GetHeadPosition();
			ST_FEATURE_POINT *pTemp;
			if(pReturnFeaSet.GetCount() == 0)
				pReturnFeaSet.AddTail(pFeatureST);
			while(possort)
			{
				POSITION pos1 = possort;
				pTemp = (ST_FEATURE_POINT*)pReturnFeaSet.GetNext(possort);
				if(dis <= pTemp->dbDis)
				{
					pReturnFeaSet.InsertBefore(pos1,pFeatureST);
					break;
				}
				if(!possort)
				{
					pReturnFeaSet.AddTail(pFeatureST);
					break;
				}
			}
		}
	}
	return ;	
}
BOOL CGIS_QueryByShape::ReadPOIByName( char *pFBuf, const DWORD dwSize, DWORD &dwStation , CPtrList& pOList  , swString strName)
{
	unsigned short nPOIType = 0;
	ST_GEO_PTXY *pPt = NULL;
	BYTE nAddrLen = 0;
	char *szAddr = NULL;
	BYTE nPhoneLen = 0;
	char *szPhone = NULL;
	int nAnno = 0;
	char *szAnno = NULL;
	CGIS_FeaturePoint *pPoint = NULL;
	int nOff = 0;
	TRY
	{
		dwStation += 2;
		nOff += 2;

		BYTE *pByte = (BYTE*)(pFBuf + dwStation);
		dwStation += (*pByte +1);
		nOff += (*pByte+1);

		pByte = (BYTE*)(pFBuf + dwStation);
		dwStation += (*pByte +1);
		nOff += (*pByte+1);
		


		Fread( pFBuf, dwSize, dwStation, &nAnno, sizeof(int), 1 );
		nOff += 4;
		if( nAnno > dwSize-dwStation )
		{
			return FALSE;
		}
		swString strFeaName;
		BOOL bFind = FALSE;
		if( nAnno > 0 )
		{
			char* pCharTemp = new char[nAnno+1];
			memcpy( pCharTemp , pFBuf + dwStation , nAnno );
			pCharTemp[nAnno] = '\0';
			CharToUnicode( pCharTemp , &strFeaName );
//			strFeaName = swString(  ( pFBuf + dwStation) , nAnno );
			bFind = ( strFeaName.GetLength() >= strName.GetLength() &&
				strFeaName.Find( strName )!=-1 );
			dwStation += nAnno;
			nOff += nAnno;
			delete[] pCharTemp;
		}
		if( bFind )
		{
			//确定需要加载
			dwStation -= nOff;
			Fread( pFBuf, dwSize, dwStation, &nPOIType, sizeof(unsigned short), 1 );
			//////////////////////////////////////////////////////////////////////////
			Fread( pFBuf, dwSize, dwStation, &nAddrLen, sizeof(BYTE), 1 );
			if( nAddrLen > dwSize-dwStation )
				return FALSE;
			if( nAddrLen > 0 )
			{
				szAddr = new char[nAddrLen+1];
				Fread( pFBuf, dwSize, dwStation, szAddr, sizeof(char)*nAddrLen, 1 );
				szAddr[nAddrLen] = '\0';
				nAddrLen += 1;
			}
			Fread( pFBuf, dwSize, dwStation, &nPhoneLen, sizeof(BYTE), 1 );
			if( nPhoneLen > dwSize-dwStation )
			{
				if( szAddr )
					delete[] szAddr;
				return FALSE;
			}
			if( nPhoneLen > 0 )
			{
				szPhone = new char[nPhoneLen+1];
				Fread( pFBuf, dwSize, dwStation, szPhone, sizeof(char)*nPhoneLen, 1 );
				szPhone[nPhoneLen] = '\0';
				nPhoneLen += 1;
			}
			dwStation += 4;
			if( nAnno > 0 )
			{
				szAnno = new char[nAnno+1];
				Fread( pFBuf, dwSize, dwStation, szAnno, sizeof(char)*nAnno, 1 );
				szAnno[nAnno] = '\0';
				nAnno += 1;
			}

			//////////////////////////////////////////////////////////////////////////


			ST_GEO_PTXY aPt;
			Fread( pFBuf, dwSize, dwStation, &aPt, sizeof(ST_GEO_PTXY), 1 );

//			pPoint = new CGIS_FeaturePoint( EN_GOTYPE_POINT ); 

            //////////////////////////////////////////////////////////////////////////
            //注意,在这里改变了Ftr及其内部数据的创建方式. 
            CGIS_FeaturePoint* pFtrAddr = (CGIS_FeaturePoint*)malloc(sizeof(CGIS_FeaturePoint));
	        CGIS_FeaturePoint* pPoint = new (pFtrAddr) CGIS_FeaturePoint(EN_GOTYPE_POINT);
            pPoint->m_nMemmoryType = 0;

			pPoint->SetPtInfo(1,&aPt );                         //因为处部是局部空间,所以内部重新分配空间
            pPoint->SetPhoneInfoDirect( nPhoneLen , szPhone );  //因为外部已构造好空间,所以直接置入.
            pPoint->SetAddressInfoDirect( nAddrLen,szAddr );    //因为外部已构造好空间,所以直接置入.
            pPoint->SetAnnotationInfoDirect(nAnno ,szAnno );    //因为外部已构造好空间,所以直接置入.
            //////////////////////////////////////////////////////////////////////////
            
			pPoint->SetPointObjectType( nPOIType );
			pOList.AddTail( pPoint );
		}
		else
		{
			//不需要加载，清理内存，移动文件指针
//			if( szAddr )
//				delete[] szAddr;
//			if( szPhone )
//				delete[] szPhone;
//			if( szAnno )
//				delete[] szAnno;
			dwStation += 8;
			return FALSE;
		}
	}
	CATCH( CMemoryException, e )
	{
		if( pPoint )
			delete pPoint;
		else
		{
			if( pPt )
				delete pPt;
			if( szAnno )
				delete[] szAnno;
			if( szAddr )
				delete[] szAddr;
			if( szPhone )
				delete[] szPhone;
		}
//		THROW( e );
		return FALSE;
	}
	END_CATCH
	return TRUE;
}
//BOOL CGIS_QueryByShape::strstrFind( const char * str1,
//        const char * str2 , int ncharNum )
//{
//    char *cp = (char *) str1;
//    char *s1, *s2;
//    if ( !*str2 )
//        return FALSE;
//	int nCount = 0 ;
//    while (*cp && nCount < ncharNum)
//    {
//		nCount++;
//        s1 = cp;
//        s2 = (char *) str2;
//        while ( *s1 && *s2 && !(*s1-*s2) )
//                s1++, s2++;
//        if (!*s2)
//                return TRUE;
//        cp++;
//    }
//    return FALSE;
//}
//
BOOL CGIS_QueryByShape::GetCurrentCityID( ST_GEO_PTXY ptCurrent , int& nCityID )
{
    CSpaceMemoryLock aLock;    

	if( !m_pMap )
		return FALSE;
	//int nBMID;
	int nBMID = 0; // scott
	int nx,ny;
	CalculateBMapIDInfo( ptCurrent.dx, ptCurrent.dy, nx, ny );
	CalculateBMapID( nx , ny );
	CGIS_BreadthMap *pBMap = m_pMap->GetBreadthMapByID( nBMID );
	if( pBMap )
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
				short    nKey;
				if( pos1 )
				{
					pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
					if( pLayer )
					{
						CGIS_MapLayer *pBLList[1];
						pBLList[0] = (CGIS_MapLayer*)pLayer;
						if( !m_pMap->LoadBLData( pBMap, pBLList, 1 ) )
							return FALSE ;
						CPtrList *pOList = NULL;
						pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
						POSITION posObject = pOList->GetHeadPosition();
						CGIS_FeaturePoly* pFeaturePoly = NULL;
						while( posObject )
						{
							pFeaturePoly = (CGIS_FeaturePoly*)pOList->GetNext( posObject );
							if( pFeaturePoly )
							{
								if( PointInPoly( ptCurrent , pFeaturePoly ))
								{
									//返回城市ID
									//nCityID = 
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
									return TRUE;
								}
							}
						}

						pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
					}

				}
			}
		}
	}
	return FALSE;
}


