
//#include "stdafx.h"
#include "GIS_GeoMap.h"
#include "GIS_LayerManager.h"
#include "GeoView.h"
#include "GIS_Feature.h"
#include "GIS_MFBand.h"
#include "GIS_LFBand.h"
#include "GIS_QueryByShape.h"
#include "GIS_QueryByAtt.h"
#include "GIS_LayerTool.h"
#include "GIS_FeatureEditer.h"
#include "GIS_FeatureRut.h"
#include "GIS_NetworkRut.h"

#include "GIS_PathpointNode.h"
#include "GIS_PathpointNodeV2.h"
#include "GIS_BufStorageNetwork.h"
#include "GIS_MapBreadthNetwork.h"
//#include "GIS_MapNetwork.h"
#include "SDEControler.h"
#include "GIS_MapDataManager.h"
#include "MemoryDispatchForSpaceDataV2.h"
#include "GIS_NetworkException.h"
//////////////////////////////////////////////////////////////////////////
#include "./DebugFunctionLib/DebugFun.h"
#include "IGIS_MapNetwork.h"

#define IS_SAME_GEO_POINT(pt1,pt2) (fabs( (pt1).dx - (pt2).dx ) < FLOAT_DETA_XY && fabs( (pt1).dy - (pt2).dy) < FLOAT_DETA_XY )

CGIS_GeoMap::CGIS_GeoMap(){
	//m_pMapViewSet = new CPtrList;
	//m_LayerManagerSet = new CPtrList;
	//m_pMapInfo = new CGIS_MapInfo;
//	m_ToolSet = new CPtrList;
	//m_ToolSet = NULL;
	m_ToolSet.reserve(20);
	m_pBMapList = NULL;
	m_enMFType = EN_MFTYPE_BAND;
	m_pMapNetwork = NULL;

	m_pBMNet = NULL;
	int i;
	/*
	for( i=0;	i<MAX_RLAY_NUM;	i++ ){
		m_pLList[i] = NULL;
  }*/
	m_pLaneBlockAddr = NULL;
	m_pRTList = NULL;
	m_nMatchBID = -1;
	m_nFRLNum = 0;
	m_nReCount = 0;
	m_nCarRunDirect = 0;

	// ���ӶԳ�Ա�����ĳ�ʼ�� 20070326
	m_nBMapNum = 0; // ͼ����Ŀ
	for (i=0; i<MAX_RLAY_NUM; i++) // ͼ��
		m_pRLList[i] = NULL;
	//m_nRLNum = 0; // ͼ����Ŀ
	for (i=0; i<MAX_BRUT_NUM; i++) // ·���б�
		m_pFRList[i] = NULL;
	m_byMRNum = 0;
	m_pVNode = NULL;
	m_posVNode = NULL;
	m_pFRPre = NULL;
	m_nAPre = 0;
	m_nANext = 0;
	m_bTrunFlag = FALSE;
	m_bRFlag = FALSE;
	m_nRMOrd = 0; // �������ڵ����
	m_nBID = 0;
	::memset(&m_ptLastXy, 0, sizeof(ST_GEO_PTXY));
	m_nNextRutPt = 0;
}

CGIS_GeoMap::~CGIS_GeoMap()
{
	{
		CSpaceMemoryLock aLock;
		ClearStorageBMap();
	}
	IGIS_TOOL* pTempTool;
	std::vector<IGIS_TOOL*>::iterator itr;
	for(itr=m_ToolSet.begin();itr!=m_ToolSet.end();itr++){
		pTempTool = *itr;
		delete pTempTool; // virtual destruction function should be overrided
	}
	/*
	if(m_ToolSet != NULL)
	{
		POSITION pos = m_ToolSet->GetHeadPosition();
		while (pos)
		{
			pTempTool = (IGIS_TOOL*)m_ToolSet->GetNext(pos);
			if(pTempTool)
			{
				delete pTempTool;
				pTempTool = NULL;
			}
		}
		delete m_ToolSet;
		m_ToolSet = NULL;
	}*/
	/*
	if(m_pMapInfo != NULL)
	{
		delete m_pMapInfo;
		m_pMapInfo = NULL;
	}*/
	CGIS_LayerManager* pTempLayerManager;
	POSITION pos;
	/*
	if(m_pMapViewSet != NULL)
	{
		m_pMapViewSet->RemoveAll();
		delete m_pMapViewSet;
		m_pMapViewSet = NULL;
	}*/

	/*
	if(m_LayerManagerSet != NULL)
	{
		pos = m_LayerManagerSet->GetHeadPosition();
		while(pos != NULL)
		{
			pTempLayerManager = (CGIS_LayerManager*)(m_LayerManagerSet->GetNext(pos));
			if(pTempLayerManager != NULL)
			{
				delete pTempLayerManager;
				pTempLayerManager = NULL;
			}
		}
		delete m_LayerManagerSet;
		m_LayerManagerSet = NULL;
	}*/
	if( m_pMapNetwork )
		delete m_pMapNetwork;
	m_pMapNetwork = NULL;
	if( m_pRTList )
		delete[] m_pRTList;
	m_pRTList = NULL;
	
	if( m_pBMapList )
	{
		delete[] m_pBMapList;
		m_pBMapList = NULL;
	}
}

BOOL CGIS_GeoMap::ClearStorageBMap( ){
	CGIS_MapLayer *pLayer = NULL;
	for( int i=0;	i<MAX_RLAY_NUM;	i++ ){
		pLayer = m_pLList[i];
		if( pLayer && m_pBMNet )
			pLayer->DecreaseLCount( m_pBMNet->m_nBreadthID );
		m_pLList[i] = NULL;
	}
	memset( m_pFRList, 0, sizeof(CGIS_FeatureRut*)*MAX_BRUT_NUM );
	m_pBMNet = NULL;
    if(m_pLaneBlockAddr)
    {
        delete m_pLaneBlockAddr;
        m_pLaneBlockAddr = NULL;
    }
//	return m_MNBStor.ClearBMapBufStorage();	
	return TRUE;	
}
/*
BOOL CGIS_GeoMap::Draw(IGIS_MapView* pMapView)
{
	if(pMapView != NULL)
	{
		ST_GEO_RECT rtDraw ;
		if(pMapView->GetAngle() != 90 )
			rtDraw = pMapView->GetRotateRect();
		else
			rtDraw = pMapView->GetCurrentDrawrect();
		double dbScale = pMapView->TurnScale( pMapView->GetCurrentScaleLevel() );
		swRect rtPolt = pMapView->GetViewPlot();

		CGIS_LayerManager *pLManager = NULL;
		POSITION pos = m_LayerManagerSet->GetHeadPosition();
		while( pos )
		{
			pLManager = (CGIS_LayerManager*)m_LayerManagerSet->GetNext(pos);
			if( pLManager )
			{
				if( m_enMFType == EN_MFTYPE_GUIDE30 )
				{
//					if( m_pLFile == NULL )
//						pLManager->DrawLayer(pMapView, rtPolt, rtDraw, dbScale);
//					else 
//					{
//						if( m_pLFile->IsLoadMode() )
//							pLManager->DrawLayer(pMapView, rtPolt, rtDraw, dbScale);
//						else
//							pLManager->DrawLayer( m_pLFile, pMapView, rtPolt, rtDraw, dbScale);
//					}
				}
				else if( m_enMFType == EN_MFTYPE_BAND )
				{	//����ÿһͼ�� 
					pLManager->DrawLayer(pMapView, rtPolt, rtDraw, dbScale);
				}
			}
		}
		pMapView->DrawAnnotate();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

*/
BOOL CGIS_GeoMap::Attach(IGIS_MapView* pMapView){
	if(pMapView){
		//m_pMapViewSet->AddTail(pMapView);
		_mapViewSet.push_back(pMapView);
		return pMapView->SetMap(this);
	}
	return FALSE;
}
BOOL CGIS_GeoMap::Detache(IGIS_MapView* pMapView){
	std::list<IGIS_MapView*>::iterator itr;
	for(itr = _mapViewSet.begin();itr!=_mapViewSet.end();itr++){
		if( (*itr) == pMapView){
			_mapViewSet.erase(itr);
			pMapView->ReleaseMap(this);
			return TRUE;
		}
	}
	return FALSE;
}

CGIS_LayerInfo* CGIS_GeoMap::GetLayerInfo(UINT ILayerID){
	/*
	CGIS_LayerManager *pLManager = NULL;
	POSITION pos = m_LayerManagerSet->GetHeadPosition();
	while(pos != NULL)
	{
		pLManager = (CGIS_LayerManager*)m_LayerManagerSet->GetNext(pos);
		if(pLManager != NULL)
		{
			IGIS_Layer *pLayer = NULL;
			pLManager->GetLayerListMap()->Lookup( ILayerID, pLayer );
			if(pLayer != NULL)
			{
				if(ILayerID == pLayer->GetLayerInfo()->GetLayerID())
				{
					return pLayer->GetLayerInfo();
				}
			}
		}
	}*/
	std::list<CGIS_LayerManager>::iterator itrlm;
	for(itrlm = _layerMgnSet.begin();itrlm!=_layerMgnSet.end();itrlm++){
			CGIS_LayerMap& layerlist = itrlm->GetLayerListMap();
			CGIS_LayerMap::iterator itrLayer;
			itrLayer = layerlist.find(ILayerID);
			if( itrLayer!=layerlist.end() ){
				return itrLayer->second;
			}
	}
	return NULL;
}
void CGIS_GeoMap::SetBreadthMapList( CGIS_BreadthMap* pBMapList )
{
	m_pBMapList = pBMapList;
}

//ÿ��ͼ��Ŀɼ���������
void CGIS_GeoMap::SetBreadthMapRects( ST_GEO_RECT *pRTList, int nBNum )
{
	if( nBNum > 0 && pRTList )
	{
		if( m_pRTList )
			delete[] m_pRTList;
		m_pRTList = new ST_GEO_RECT[nBNum];
		memcpy( m_pRTList, pRTList, sizeof(ST_GEO_RECT)*nBNum);
	}
}
ST_GEO_RECT CGIS_GeoMap::GetBreadthMapRect( CGIS_BreadthMap *pBMap )
{
	ST_GEO_RECT rt;
	if( GetMapInfo()->GetMapType() == EN_MAPTYPE_NATION )
		rt = m_pRTList[pBMap->m_nBreadthID]; //��ͼһ��ֱ�Ӵ洢ͼ��ĵ������
	else if( m_pMapInfo.GetMapType() == EN_MAPTYPE_CITY )
		rt = CalculateBMapRect( pBMap->m_nBMID ); //����ͼ������
	else
		rt.dBottom = rt.dLeft = rt.dRight = rt.dTop = 0.0;
	return rt;
}

int CGIS_GeoMap::GetBreadthMapNum(){
	return m_nBMapNum;
}

void CGIS_GeoMap::SetBreadthMapNum( int nBMapNum ){
	m_nBMapNum = nBMapNum;
}

EnMapFileType CGIS_GeoMap::GetMapFileType( ){
	return m_enMFType;
}

IGIS_MapNetwork* CGIS_GeoMap::GetMapNetWork( )
{
	return m_pMapNetwork;
}
void CGIS_GeoMap::SetMapNetWork( IGIS_MapNetwork *pMapNetwork )
{
	m_pMapNetwork = pMapNetwork;
	m_byMRNum = 0;
	m_pFRPre = NULL;
	m_pVNode = NULL;
	m_nAPre = 0;
	m_nANext = 0;
	m_posVNode = NULL;
	m_bRFlag = FALSE;
	m_nRMOrd = 0;
	m_nBID = 0;
}
BOOL CGIS_GeoMap::Open(swString szFilePath){
	/*
	m_nRLNum = 0;
	CGIS_LayerManager *pLManager = NULL;
	POSITION pos = m_LayerManagerSet->GetHeadPosition();
	while( pos )
	{
		pLManager = (CGIS_LayerManager*)m_LayerManagerSet->GetNext(pos);
		if( pLManager )
		{
			IGIS_Layer *pLayer = NULL;
			for( int i=0;	i<LAYER_PT_END+1;	i++ )
			{
				pLManager->GetLayerListMap()->Lookup( i, pLayer );
				if( pLayer )
				{
					if( pLayer->GetLayerInfo()->GetLayerID() > 4 &&
						pLayer->GetLayerInfo()->GetLayerID() < 12 )
					{//����ͼ��
						m_pRLList[m_nRLNum] = (CGIS_MapLayer*)pLayer;
						m_nRLNum++;
					}
				}
			}
		}
	}*/
	_rl_list.clear();
	std::list<CGIS_LayerManager>::iterator itr;
	CGIS_LayerMap::iterator itrLayer;
	for(itr=_layerMgnSet.begin();itr!=_layerMgnSet.end();itr++){
		CGIS_LayerMap & layerlist = itr->GetLayerListMap();
		for(itrLayer = layerlist.begin();itrLayer!=layerlist.end();itrLayer++){
			IGIS_Layer *pLayer;
			pLayer = itrLayer->second;
			if( pLayer->GetLayerInfo()->GetLayerID() > 4 &&pLayer->GetLayerInfo()->GetLayerID() < 12 ){
				_rl_list.push_back((CGIS_MapLayer*)pLayer);
			}
		}
	}
	return TRUE;
}
BOOL CGIS_GeoMap::Load(swString szFilePath)
{
	return TRUE;
}
BOOL CGIS_GeoMap::UnLoad(UINT nMapID)
{
//	m_nCounter--;
//	if( m_nCounter == 0 )
//	{
//		if( m_pLFile->IsLoadMode() )
//		{
//			CGIS_LayerManager *pLManager = NULL;
//			POSITION pos = m_LayerManagerSet->GetHeadPosition();
//			while( pos )
//			{
//				pLManager = (CGIS_LayerManager*)m_LayerManagerSet->GetNext(pos);
//				if( pLManager )
//				{
//					POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
//					IGIS_Layer *pLayer = NULL;
//					unsigned short    nKey;
//					while( pos1 )
//					{
//						pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
//						if( pLayer )
//						{
//							if( pLayer->GetLayerType() == EN_LAYTYPE_MAP )
//								;//((CGIS_MapLayer*)pLayer)->UnloadLayer( );
//						}
//					}
//				}
//			}
//		}
//		m_pLFile = NULL;
//	}
//	if(m_nCounter < 0)
//		m_nCounter = 0;
	return TRUE;
}
BOOL CGIS_GeoMap::LoadBreadthMap( CGIS_BreadthMap *pBMap ){
   // ASSERT(FALSE);
	return TRUE;
}

BOOL CGIS_GeoMap::UnLoadBreadthMap( CGIS_BreadthMap *pBMap )
{
	pBMap->m_nLoadCounter--;
	if( pBMap->m_nLoadCounter == 0 )
	{
		POSITION pos = m_LayerManagerSet->GetHeadPosition();
		CGIS_LayerManager *pLManager = NULL;
		if( pos )
		{
			pLManager = (CGIS_LayerManager*)m_LayerManagerSet->GetNext( pos );
			if( pLManager )
			{
				POSITION ps = pLManager->GetLayerListMap()->GetStartPosition();
				IGIS_Layer *pTem = NULL;
				short    nKey;
				while( ps )
				{
					pLManager->GetLayerListMap()->GetNextAssoc( ps, nKey, pTem );
					if( pTem )
					{
						CPtrList *pOList = NULL;
						pTem->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
						if( pOList )
						{
							POSITION pos1 = pOList->GetHeadPosition();
							CGIS_Feature *pFeature = NULL;
							while( pos1 )
							{
								pFeature = (CGIS_Feature*)pOList->GetNext( pos1 );
								if( pFeature )
								{
									delete pFeature;
									pFeature = NULL;
								}
							}
							pOList->RemoveAll( );
						}
					}
				}
			}
		}
	}
	// �������������ע�͵��������޷���������������С��0������Ĳ�����������
//	if( pBMap->m_nLoadCounter<0 )
//		pBMap->m_nLoadCounter = 0;
	return TRUE;
}

/**
 * @brief ���ؿռ�����,��������Ϊ�ڴ治���������ʧ��,��ô����ȥ����һ��.
 *
 * @param 
 * @return: 0 succ, 2 failed
 ��һ�μ���ʧ�ܣ����ܻ���ռ䲻����Ȼ�����û���ռ䣬�ٴμ���
 ��ʧ�������û���ռ䷵��
*/
//BOOL CGIS_GeoMap::LoadBLData( CGIS_BreadthMap *pBMap, CGIS_MapLayer **pLList, int nLNum ,unsigned int* pErrorCode)
BOOL CGIS_GeoMap::LoadBLData( CGIS_BreadthMap *pBMap, std::list<CGIS_MapLayer*>& layers ,unsigned int* pErrorCode)
{
	CSDEControler* pControl = CSDEControler::GetSDEControl();
	//ASSERT( pControl );
    CMemoryDispatchForSpaceDataV2* pDispatch = pControl->GetMemoryDispatchForSpace();
    //ASSERT(pDispatch);
    if(pErrorCode)
    {
        *pErrorCode = 0;
    }
	CGIS_LFBand *pLFile = new CGIS_LFBand(); //��ͼ�������� 
	//�Ƿ�ռ�����
	BOOL bSpaceMemBreak = FALSE;
	BOOL bReturnLoad = FALSE;
//	TRY
//	{       //��ȡһ�����ڵ����е�ͼ������
		bReturnLoad = pLFile->LoadLayerV2( this, pBMap, pLList, nLNum );
/*	}
	CATCH( CUserException, e )
	{
        TRACE0("receive the CUserException when loadlayer!\n");
		bSpaceMemBreak = TRUE;
	}
    AND_CATCH(CFileException,e)
    {
        TRACE0("Img file load error when LoadLayer!\n");
    }
	END_CATCH
*/

	
    //���ÿռ����ݿ�
    if(bSpaceMemBreak)//��һ��װ������ʧ��,���Ѿ�װ����һ����
    {
      //  TRACE0("Reset space memory.\n");
        {
            CSDEControler* pSdeControl = CSDEControler::GetSDEControl();
       //     ASSERT(pSdeControl);
            CMemoryDispatchForSpaceDataV2* pSpace = pSdeControl->GetMemoryDispatchForSpace();
        //    ASSERT( pSpace );
            pSpace->RequireReset(); //���ڴ�������ռ�����֮����ȥ����һ��
            if( pErrorCode )
                *pErrorCode = 2;
        }
        
        bSpaceMemBreak = FALSE;
        bReturnLoad = FALSE;
       // TRY
       // {
        //    TRACE0("Try to loadlayer after reset!\n");
            bReturnLoad = pLFile->LoadLayerV2( this, pBMap, pLList, nLNum );
        //}
        //CATCH( CUserException, e )
        //{
        //    TRACE0("receive the CUserException when loadlayer!\n");
         //   bSpaceMemBreak = TRUE; //�ٴμ���ʧ��
       // }
        //AND_CATCH(CFileException,e)
        //{
        //    TRACE0("Img file load error when LoadLayer!\n");
        //}
        //END_CATCH
            
            
            if(!bSpaceMemBreak)
            {
               // TRACE0("Loadlayer success after reset.\n");
            }
            else
            {
               // TRACE0("Reset space memory again.\n"); //�ٴ�ʧ�ܣ���λ�ڴ滺����
                {
                    CSDEControler* pSdeControl = CSDEControler::GetSDEControl();
                  //  ASSERT(pSdeControl);
                    CMemoryDispatchForSpaceDataV2* pSpace = pSdeControl->GetMemoryDispatchForSpace();
                  //  ASSERT( pSpace );
                    pSpace->RequireReset();
                    if( pErrorCode )
                        *pErrorCode = 2;
                }
                
            }                  
    }
	delete pLFile;
	if(bSpaceMemBreak)
     return FALSE;
	return bReturnLoad;
}

BOOL CGIS_GeoMap::UnLoadBLData( int nBID, CGIS_MapLayer *pLayer )
{
	pLayer->DecreaseLCount( nBID );
	if( pLayer->GetBLCount(nBID) == 0 )
	{
		CPtrList *pOList = NULL;
		pLayer->GetObjectListMap()->Lookup( nBID, pOList );
		if( pOList )
		{
			POSITION pos1 = pOList->GetHeadPosition();
			CGIS_Feature *pFeature = NULL;
			while( pos1 )
			{
				pFeature = (CGIS_Feature*)pOList->GetNext( pos1 );
				if( pFeature )
				{
					delete pFeature;
					pFeature = NULL;
				}
			}
			pOList->RemoveAll( );
		}
	}
	return TRUE;
}

//����һ��������ڵ�·ͼ�������,����Ӧ�ò����߽����� 
BOOL CGIS_GeoMap::LoadBLRutData( CGIS_BreadthMap *pBMap ){
  //  ASSERT(FALSE);
	//if( !LoadBLData( pBMap, m_pRLList, m_nRLNum ) )
	if( !LoadBLData( pBMap, _rl_list) )
		return FALSE;
//	for( int i=0;	i<m_nRLNum;	i++ )
//		m_pRLList[i]->IncreaseLCount( pBMap->m_nBreadthID );
	return TRUE;
}
BOOL CGIS_GeoMap::UnLoadBLRutData( CGIS_BreadthMap *pBMap )
{
	/*
	for( int i =0;	i<m_nRLNum;	i++ )
		m_pRLList[i]->DecreaseLCount(pBMap->m_nBreadthID);
		*/
	std::list<CGIS_MapLayer*>::iterator itr;
	for(itr=_rl_list.begin();itr!=_rl_list.end();itr++){
		(*itr)->DecreaseLCount(pBMap->m_nBreadthID);
	}
	return TRUE;
}

//�ص���Ϣ��ÿһ����ͼ����
BOOL CGIS_GeoMap::Notify(IGIS_Message* pMessage){
	if( !pMessage){
		return FALSE;
	}
	std::list<IGIS_MapView*>::iterator itr;
	for(itr=_mapViewSet.begin();itr!=_mapViewSet.end();itr++){
		(*itr)->Update(pMessage);
	}
	return TRUE;
	/*
	if(pMessage && m_pMapViewSet)
	{
		POSITION pos = m_pMapViewSet->GetHeadPosition();
		IGIS_MapView* pTempView;
		while(pos)
		{
			pTempView = (IGIS_MapView*)m_pMapViewSet->GetNext(pos);
			if(pTempView)
			{
				pTempView->Update(pMessage);
			}
		}
		return TRUE;
	}

	return FALSE;*/
}
BOOL CGIS_GeoMap::SaveToFile( ){
	return TRUE;
}
CGIS_BreadthMap* CGIS_GeoMap::GetBreadthMapByID2( int nBID )
{
	ASSERT( nBID < m_nBMapNum );
	return &m_pBMapList[nBID];
}
CGIS_BreadthMap* CGIS_GeoMap::GetBreadthMapByID( int nBID )
{
	CGIS_BreadthMap *pBNet = NULL;
	// ��λ����
	int nStart = 0;
	int nEnd = m_nBMapNum; //������
	int nMid = 0;
	while( nEnd > nStart )
	{
		nMid = (nStart+nEnd)/2;
		if( m_pBMapList[nMid].m_nBMID == nBID )
			return &m_pBMapList[nMid];
		else if( m_pBMapList[nMid].m_nBMID > nBID )
			nEnd = nMid;
		else
			nStart = nMid;
		if( nEnd-nStart <= 1 )
		{
			if ( m_pBMapList[nStart].m_nBMID == nBID )
				return &m_pBMapList[nStart];
			else if ( m_pBMapList[nEnd].m_nBMID == nBID )
				return &m_pBMapList[nEnd];
			return NULL;
		}
	}
	return NULL;
}
///��õ����ڷַ���ͼ
BOOL CGIS_GeoMap::GetBreadthMapOfPoint( float dx, float dy, CPtrList &BMapList )
{
	CGIS_BreadthMap *pBMap = GetBreadthMapByID( CalculateBMapID(dx,dy) );
	if( pBMap )
	{
		BMapList.AddTail(pBMap);
		return TRUE;
	}
	return FALSE;
/*	BOOL bFind = FALSE;
	CGIS_BreadthMap *pBMap = NULL;
	for( int i=0;	i<m_nBMapNum;	i++ )
	{
		pBMap = m_pBMapList[i];
		if( pBMap )
		{
			if( IsPtInRect( dx, dy, pBMap->m_rtBreadth.dLeft,
					pBMap->m_rtBreadth.dTop, pBMap->m_rtBreadth.dRight,pBMap->m_rtBreadth.dBottom ) )
			{
				BMapList.AddTail( pBMap );
				bFind = TRUE;
			}
		}
	}
	return bFind;
*/
}

BOOL CGIS_GeoMap::GetRutObjList( CPtrList *pLMList, CGIS_BreadthMap *pBMap, CPtrList *pObjList )
{
//	m_MNBStor.InsertBMap( pBMap );
// 	LoadBLRutData( pBMap );
	IGIS_Layer* pLayer = NULL;
	POSITION pos = pLMList->GetHeadPosition();
	CGIS_LayerManager *pLManager = NULL;
	while( pos )
	{
		pLManager = (CGIS_LayerManager*)pLMList->GetNext(pos);
		if( pLManager )
		{
			for( int i=5;	i< 12;	i++ )
			{
				pLManager->GetLayerListMap()->Lookup( i, pLayer );
				if( pLayer )
				{
					CPtrList *pOList = NULL;
					pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
					if( pOList )
						pObjList->AddTail( pOList );
				}
			}
		}
	}
	m_nBID = pBMap->m_nBreadthID;
//	UnLoadBLRutData( pBMap );
	return TRUE;
}
BOOL CGIS_GeoMap::GetBreadthMapCurRutObj( ST_GEO_PTXY ptStart, CGIS_BreadthMap *pBMap, int nRutID, CGIS_FeatureRut **pFRut )
{
	CPtrList ObjList;
	GetRutObjList( m_LayerManagerSet, pBMap, &ObjList );
	POSITION pos = ObjList.GetHeadPosition();
	CGIS_FeatureRut *pRut = NULL;
	while( pos )
	{
		pRut = (CGIS_FeatureRut*)ObjList.GetNext( pos );
		if( pRut )
		{
			if( pRut->GetRutID() == nRutID )
			{
				*pFRut = (CGIS_FeatureRut*)pRut->Clone();
				return TRUE;
			}
		}
	}
	return FALSE;
}
///������ͼ������ͼ���д�·�εĵ���
BOOL CGIS_GeoMap::GetRoundBMapRutObj( ST_GEO_PTXY pt, int nRutID, CGIS_BreadthMap **pBMap, CGIS_FeatureRut **pFRut )
{
	CPtrList BMapList;
	if( GetBreadthMapOfPoint( pt.dx, pt.dy, BMapList ) )
	{
		POSITION pos = BMapList.GetHeadPosition();
		CGIS_BreadthMap *pBMTem = NULL;
		while( pos )
		{
			pBMTem = (CGIS_BreadthMap*)BMapList.GetNext( pos );
			if( pBMTem != *pBMap )
			{
				if( GetBreadthMapCurRutObj( pt, pBMTem, nRutID, pFRut ) )
				{
					*pBMap = pBMTem;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
BOOL CGIS_GeoMap::FullPathNodeArc( CGIS_Pathpoint *pPPoint, CGIS_FeatureRut *pFRut, ST_GEO_PTXY ptEnd, CGIS_BreadthMap *pBMap, BOOL bSequence, CPtrList *pPathpointList, POSITION &pos )
{
	ST_GEO_PTXY pt;
	int nPtNum = pFRut->GetPtNum();
	for( int i=1;	i<nPtNum-1;	i++ )
	{
		if( bSequence )
			pt = pFRut->GetPtList()[i];
		else
			pt = pFRut->GetPtList()[nPtNum-1-i];
		CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
		pPathPt->SetPathpointPosition( pt );
		pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
		pos = pPathpointList->InsertAfter( pos, pPathPt );
	}
	if( bSequence && pPPoint )
		pPPoint->SetPathpointPosition( pFRut->GetPtList()[0] );
	else if( pPPoint )
		pPPoint->SetPathpointPosition( pFRut->GetPtList()[pFRut->GetPtNum()-1] );
	return TRUE;
//	ST_GEO_PTXY pt;
//	int nPtNum = pFRut->GetPtNum();
//	for( int i=1;	i<nPtNum-1;	i++ )
//	{
//		if( bSequence )
//			pt = pFRut->GetPtList()[i];
//		else
//			pt = pFRut->GetPtList()[nPtNum-1-i];
//		CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
//		pPathPt->SetPathpointPosition( pt );
//		pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
//		pos = pPathpointList->InsertAfter( pos, pPathPt );
//	}
//	if( bSequence )
//		pt = pFRut->GetPtList()[nPtNum-1];
//	else
//		pt = pFRut->GetPtList()[0];
//	if( (fabs(pt.dx-ptEnd.dx) <FLOAT_DETA_XY ) && (fabs(pt.dy-ptEnd.dy) <FLOAT_DETA_XY) )
//		return TRUE;
//	CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
//	pPathPt->SetPathpointPosition( pt );
//	pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
//	pos = pPathpointList->InsertAfter( pos, pPathPt );
//
//	CGIS_FeatureRut *pFRutTemp = NULL;
//	CGIS_BreadthMap *pBMTem = pBMap;
//	GetRoundBMapRutObj( pt, pFRut->GetRutID(), &pBMTem, &pFRutTemp );
//	
//	if( pFRutTemp )
//	{
//		FullPathNodeArc( pFRutTemp, ptEnd, pBMTem, bSequence, pPathpointList, pos );
//		delete pFRutTemp;
//	}
//
//	return TRUE;
}
BOOL CGIS_GeoMap::FindNearRut( ST_GEO_PTXY ptPos, CPtrList *pRutList, CGIS_BreadthMap *pBMap, double &dbMinDis, double dbDisBuf, ST_WPNRS_RINFO &wpRInfo )
{
	BOOL bFind = FALSE;
	CGIS_FeatureRut *pRut = NULL;
	int nRutID;
	ST_GEO_PTXY ptProj;
	POSITION pos = pRutList->GetHeadPosition();
	while( pos )
	{
		pRut = (CGIS_FeatureRut*)pRutList->GetNext( pos );
		BOOL bFlag = FALSE;
		double dbDis = dbDisBuf;
		if( pRut )
		{
			if( pRut->GetRutID() < 0 )
				continue;
			for( int i=0;	i< pRut->GetPtNum()-1;	i++ )
			{
				if( fabs(pRut->GetPtList()[i].dx-pRut->GetPtList()[i+1].dx)>=FLOAT_ZERO || fabs(pRut->GetPtList()[i].dy-pRut->GetPtList()[i+1].dy)>=FLOAT_ZERO )
				{
					double dbTem = 0.0;
					ST_GEO_PTXY ptTem = DisPointToLine( ptPos, pRut->GetPtList()[i], pRut->GetPtList()[i+1] );
					BOOL bOnLine = PointOnLine(ptTem,pRut->GetPtList()[i], pRut->GetPtList()[i+1]);
					if(bOnLine)
						dbTem = CalcDist( ptTem.dx, ptTem.dy, ptPos.dx, ptPos.dy );
					else
					{
						double dis1 = CalcDist(ptPos.dx, ptPos.dy,pRut->GetPtList()[i].dx,pRut->GetPtList()[i].dy);
						double dis2 = CalcDist(ptPos.dx, ptPos.dy,pRut->GetPtList()[i+1].dx,pRut->GetPtList()[i+1].dy);
						if( dis1 < dis2 )
						{
							dbTem = CalcDist( ptPos.dx, ptPos.dy, pRut->GetPtList()[i].dx, pRut->GetPtList()[i].dy );
							ptTem = pRut->GetPtList()[i];
						}
						else 
						{
							dbTem = CalcDist( ptPos.dx, ptPos.dy, pRut->GetPtList()[i+1].dx, pRut->GetPtList()[i+1].dy );
							ptTem = pRut->GetPtList()[i+1];
						}
					}
					if( dbDis > dbTem )
					{
						dbDis = dbTem;
						nRutID = pRut->GetRutID();
						ptProj.dx = ptTem.dx;
						ptProj.dy = ptTem.dy;
						if( (dbMinDis >= dbTem) || (fabs(dbMinDis-dbTem) < 1.0) )
						{
							dbMinDis = dbTem;
							bFlag = TRUE;
						}
						else if( dbMinDis < 10.1 )
						{
							if( fabs(dbMinDis-dbTem) < 70.0 )
								bFlag = TRUE;
						}
					}
				}
			}
		}
		if( bFlag )
		{
			for( int i=0;	i<wpRInfo.nRNum;	i++ )
			{
				if( wpRInfo.pRDiss[i]> dbDis )
				{
					for( int j=wpRInfo.nRNum-1;	j>i;	j-- )
					{
						wpRInfo.pRList[j] = wpRInfo.pRList[j-1];
						wpRInfo.pBMaps[j] = wpRInfo.pBMaps[j-1];
						wpRInfo.pRDiss[j] = wpRInfo.pRDiss[j-1];
						wpRInfo.pPtProjs[j] = wpRInfo.pPtProjs[j-1];
					}
					wpRInfo.pRList[i] = pRut;
					wpRInfo.pBMaps[i] = &m_pBMapList[pRut->m_nBreadthID];
					wpRInfo.pRDiss[i] = dbDis;
					wpRInfo.pPtProjs[i] = ptProj;
					bFind = TRUE;
					break;
				}
			}
		}
	}
	return bFind;
}
const UINT DIS_MAX_BUF = 20000;
////�����뺽������ĵ�·�ĵ㼯,��Ϊһ�ݹ��㷨.
BOOL CGIS_GeoMap::CalWayPointRut(
    CGIS_WayPoint *pWayPoint,
    ST_GEO_PTXY ptEnd,
    int& nErrorCode,
    UINT nDisBuf,
    UINT nCount )
{
	//TRACE1("�������·��(���Ұ뾶:%d m).\n",nDisBuf);

	m_bRFlag = FALSE;
	m_nFRLNum = 0;
	m_nRMOrd = 0;
	if( nDisBuf >= DIS_MAX_BUF )
	{
        nErrorCode = CGIS_NetworkException::ERROR_CAN_NOT_FIND_ROADSECTION_CLOSE_TO;
		TRACE0("ָ���ķ�Χ���ں��㸽���Ҳ���·��.\n");
		return FALSE;
	}

	int nRutID = 0;
	double dbMinDis = DIS_MAX_BUF;
	ST_GEO_PTXY ptProj;
	memset( &ptProj, 0, sizeof(ST_GEO_PTXY) );

	ST_WPNRS_RINFO wpRInfo;
	wpRInfo.InitWPNRuts(0);
	
	CGIS_BreadthMap *pBMap = NULL;
	CPtrList ObjList;

	CGIS_QueryByShape *pQShape = (CGIS_QueryByShape*)GetTool( QUERY_BYSHARP );
	ASSERT(pQShape);
	
	//�����ʼ�㸽��������ftr·��
    BOOL bSuccess = FALSE;    
	// ���������һ����ʱ���� 20070327
	ST_GEO_PTXY ptPos = pWayPoint->GetWPPosition();
	bSuccess = pQShape->QueryByBuffer( ObjList, &ptPos, nDisBuf, EN_OBJ_RUT );
	if(ObjList.GetCount())
		bSuccess = TRUE;
	if(bSuccess)
	{
		//���ݿռ�λ���ҳ������һ��·��
		wpRInfo.InitWPNRuts( ObjList.GetCount() );
        wpRInfo.pFtrRutList->AddTail(&ObjList);
		FindNearRut( pWayPoint->GetWPPosition(), &ObjList, pBMap, dbMinDis, nDisBuf, wpRInfo);
		
		int nOrd = ((CGIS_MapBreadthNetwork*)m_pMapNetwork)->GetWPProjRut(
			pWayPoint, 
			this,
			wpRInfo.pRList,
			wpRInfo.pBMaps, 
			wpRInfo.pPtProjs,
			wpRInfo.nRNum, ptEnd ,nErrorCode);

		//����-1��ʾû�в鵽.
		if( nOrd >= 0 && nOrd < wpRInfo.nRNum )
		{
			wpRInfo.Clear();
			return TRUE;
		}
		else if( nOrd > wpRInfo.nRNum )
		{
			wpRInfo.Clear();
			return FALSE;
		}

        ASSERT(nOrd == -1);
        if(nErrorCode == CGIS_NetworkException::ERROR_OUT_OF_MEMORY)
        {
            wpRInfo.Clear();
			return FALSE;
        }
		wpRInfo.Clear();
		nCount++;
	}
	else
	{
		nCount++;
	}

	//����Ҳ���·��ô���������뾶����ȥ����.
	return CalWayPointRut( pWayPoint, ptEnd, nErrorCode, nDisBuf+100*nCount*nCount, nCount );
}

/**
 * @brief �ں��������һ����ͨ��node
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::FullPathNode( 
    CGIS_MapBreadthNetwork *pBMNetwork, 
    CGIS_FeatureRut *pFRList[MAX_BRUT_NUM],
    CGIS_NetworkVtx *pVtx, 
    CGIS_BreadthMap *pBMap,
    CGIS_NetworkRut *pRoadSectionPrevious, 
    CGIS_NetworkRut *pRoadSectionNext, 
    int nREBID, 
    int nRutEndID,
    CPtrList *pPathpointList, 
    POSITION &pos )
{

    
	CGIS_Pathpoint *pPPoint = NULL;
	CGIS_FeatureRut *pFRut = NULL;
	BYTE byRAType = 1;
	m_bTrunFlag = TRUE;

	//NODE����Ϣ���
	if( pRoadSectionPrevious->GetRutType() != 4 
        || 
        ( pRoadSectionPrevious->GetRutType() == 4 && (pRoadSectionNext->GetRutType()==0||m_pVNode==NULL)) 
        )
	{
        //////////////////////////////////////////////////////////////////////////
        //same name between enter and exit
		swString strPre = _T("");
		swString strNext = _T("");
		if( pFRList[pRoadSectionPrevious->GetRutID()] )
			if( pFRList[pRoadSectionPrevious->GetRutID()]->GetAnnotation() )
				CharToUnicode( pFRList[pRoadSectionPrevious->GetRutID()]->GetAnnotation() , &strPre );
		if( pFRList[pRoadSectionNext->GetRutID()] )
			if( pFRList[pRoadSectionNext->GetRutID()]->GetAnnotation() )
				CharToUnicode( pFRList[pRoadSectionNext->GetRutID()]->GetAnnotation() , &strNext );
        //////////////////////////////////////////////////////////////////////////
            
		if( pVtx->GetVRutNum() > 2 
            || 
            strPre.Compare(strNext) != 0
            || 
            pRoadSectionPrevious->GetRutType() != pRoadSectionNext->GetRutType() )
		{
			if( m_pVNode )
			{
				//·���ڵ�·ֱ���뻷��·�����������ڻ���
				if( pRoadSectionPrevious->GetRutType()==4 && pRoadSectionNext->GetRutType()==0 )
				{
					FullVRutInfo( pRoadSectionNext, m_pVNode->GetVRutInfo() );
					m_pVNode->GetVRutInfo()->byAdd = 10;
					m_pVNode->SetTurnAngle( m_nAPre-m_nANext );
					FullVNodeInfo( m_pFRPre, m_pVNode, nREBID, nRutEndID, m_pVNode->GetVNodeInfo() );
                    JudgePathNodeRutsConfuse((CGIS_MapBreadthNetwork*)m_pMapNetwork ,m_pVNode,pFRList,pVtx,pRoadSectionPrevious,pRoadSectionNext);

					m_pVNode = NULL;
				}
			}
            

            BOOL bCreateNode = TRUE;
          
            //////////////////////////////////////////////////////////////////////////
#if 1
            //20070615-����"��"��·�ڵ�NODE
            {
                int nRoadSectionNum = pVtx->GetVRutNum();
                CGIS_NetworkRut* pBranchRoadSection = NULL;
                if(
                    nRoadSectionNum == 3 
                    &&
                    pRoadSectionPrevious->GetRutLevel() == 5 
                    && 
                    pRoadSectionNext->GetRutLevel() == 5)
                {                         
                    for(int i = 0; i < nRoadSectionNum; i++)
                    {
                        unsigned short nRoadSectionID = pVtx->GetVRutList()[i];
                        if(nRoadSectionID != pRoadSectionPrevious->GetRutID() && nRoadSectionID != pRoadSectionNext->GetRutID())
                        {
                            pBranchRoadSection = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), nRoadSectionID);
                            //////////////////////////////////////////////////////////////////////////
                            if(!pBranchRoadSection)
                            {
                                CGIS_BreadthMap* pBreadthMap = GetBreadthMapByID2(pVtx->GetVBreadthID());
                                ASSERT(pBreadthMap);
                                int nReadErrorCode = 0;
                                if(!pBMNetwork->ReadBreadthNetworkRoadSectionByIDArray(&pBreadthMap,pVtx->GetVRutNum(),pVtx->GetVRutList(),nReadErrorCode))
                                {
                                    //ASSERT(FALSE);
                                    TRACE1("ReadBreadthNetworkRoadSectionByIDArray failed,error code(%d)\n",nReadErrorCode);
                                    pBranchRoadSection = NULL;
                                }
                                else
                                {
                                    pBranchRoadSection = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
                                }
                                
                            }              
                            //////////////////////////////////////////////////////////////////////////
                        }                    
                    }
                    
                    //ASSERT(pBranchRoadSection);
                    if(pBranchRoadSection)
                    {
#ifdef _DEBUG                    
                        if(pBranchRoadSection->GetRutLevel() != 5)
                        {
                            TRACE2("BranchRoadSection's level != 5,log:%.6f,lat:%.6f\n",
                                pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy);
                        }
#endif
                        
                        BYTE byNextRoadSectionType = pRoadSectionNext->GetRutType();
                        if(
                            byNextRoadSectionType != 5 
                            &&
                            byNextRoadSectionType != 3
                            &&
                            byNextRoadSectionType != 6
                            &&
                            byNextRoadSectionType != 7
                            )
                        {
                            if(
                                pBranchRoadSection->GetRutType() == 5
                                ||
                                pBranchRoadSection->GetRutType() == 6
                                ||
                                pBranchRoadSection->GetRutType() == 7
                                )
                            {

                                //////////////////////////////////////////////////////////////////////////
                                //070711 ֻ����ʻ��·��λ����֧·�ε�����"��"��·��
                                short nAngleEnter   = -1;
                                short nAngleExit    = -1;
                                short nAngleBranch  = -1;
                                CGIS_NetworkRut* pRoadSectionBranch = NULL;
                                for(int i = 0; i < 3; i++)
                                {
                                    if(pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID())
                                    {
                                        nAngleEnter = pVtx->GetVRTurnList()[i];
                                        continue;
                                    }
                                    
                                    if(pVtx->GetVRutList()[i] == pRoadSectionNext->GetRutID())
                                    {
                                        nAngleExit = pVtx->GetVRTurnList()[i];
                                        continue;
                                    }    
                                    
                                    nAngleBranch = pVtx->GetVRTurnList()[i];
                                }
                                ASSERT(nAngleEnter != -1);
                                ASSERT(nAngleExit  != -1);
                                ASSERT(nAngleBranch  != -1);

                                //----ע��ʻ��·��λ����֧·�ε����,��ʱ����������ʾ.
                                //

                                int nAngleDiff = nAngleExit - nAngleBranch;
                                if( nAngleDiff < 0 )
                                {
                                    if( abs(nAngleDiff) > 180 )
                                    {
                                        //�ѵ����Ҳ���������
                                        bCreateNode = FALSE;
                                    }
                                    else
                                    {
                                        //�ѵ�����ౣ������
                                    }
                                }
                                else
                                {
                                    if( nAngleDiff > 180 )
                                    {
                                        //�ѵ�����ౣ������
                                    }
                                    else
                                    {
                                        //�ѵ����Ҳ���������
                                        bCreateNode = FALSE;
                                    }
                                }



                                //////////////////////////////////////////////////////////////////////////
                                
                            }
                            
                        }
                        
                    }
                }
                
                
            }
#endif
            //////////////////////////////////////////////////////////////////////////
 

            if(bCreateNode)
            {
            
			m_pVNode = NULL;
			m_pVNode = new CGIS_PathpointNode;
			pPPoint = m_pVNode;
			m_pVNode->SetPathpointType( EN_PATHPOINTTYPE_NODE );
			m_pVNode->SetPathpointPosition( pVtx->GetVtxPosition() );
			m_pVNode->SetNodeVtxID( pVtx->GetVtxID() );
			m_pVNode->SetPBreadthID( m_nBID );

            //////////////////////////////////////////////////////////////////////////
            if(m_pFRPre)
            {
                pFRut = m_pFRPre;
                if(pFRut)
                {
                    CGIS_FeatureRut::SIGNINFO* pSignSrc = pFRut->GetSignInfoByExitRoadSectionID(pRoadSectionNext->GetRutID());
                    if(m_pVNode)
                    {
                        m_pVNode->SetSignInfo(pSignSrc);
                        if(pSignSrc && pSignSrc->byNameLen > 0)
                        {
                            m_pVNode->GetVRutInfo()->bHaveSign = 1;
                        }
                    }
                }
                
                if( IS_SAME_GEO_POINT(pFRut->GetPtList()[0],pVtx->GetVtxPosition()) )   
                {
                    if(m_pVNode)
                    {
                        //���س�����Ϣ
                        if(pFRut->GetLaneInfoOffset())
                        {
                            ASSERT(m_pLaneBlockAddr);
                            CGIS_FeatureRut::LPLANEINFO pLaneInfoInFeatureRut = new CGIS_FeatureRut::LANEINFO;
                            pFRut->ReadLaneInfo((BYTE*)m_pLaneBlockAddr,pFRut->GetLaneInfoOffset(),pLaneInfoInFeatureRut);
                            ST_LANDINFO* pLaneInfo = pLaneInfoInFeatureRut->ExtractStartLaneInfo(pRoadSectionNext->GetRutID());
                            delete pLaneInfoInFeatureRut;
                            pLaneInfoInFeatureRut = NULL;
                            m_pVNode->SetLaneInfoDirect(pLaneInfo);
                        }
                    }
                    //////////////////////////////////////////////////////////////////////////
                }
                else if( IS_SAME_GEO_POINT(pFRut->GetPtList()[pFRut->GetPtNum()-1],pVtx->GetVtxPosition()) )
                {
                    if(m_pVNode)
                    {
                        //���س�����Ϣ
                        if(pFRut->GetLaneInfoOffset())
                        {
                            ASSERT(m_pLaneBlockAddr);
                            CGIS_FeatureRut::LPLANEINFO pLaneInfoInFeatureRut = new CGIS_FeatureRut::LANEINFO;
                            pFRut->ReadLaneInfo((BYTE*)m_pLaneBlockAddr,pFRut->GetLaneInfoOffset(),pLaneInfoInFeatureRut);
                            ST_LANDINFO* pLaneInfo = pLaneInfoInFeatureRut->ExtractEndLaneInfo(pRoadSectionNext->GetRutID());
                            delete pLaneInfoInFeatureRut;
                            pLaneInfoInFeatureRut = NULL;
                            m_pVNode->SetLaneInfoDirect(pLaneInfo);
                        }
                    }
                    //////////////////////////////////////////////////////////////////////////
                }
                else
                {
                    double dbDis1 = CalcDist(pFRut->GetPtList()[0].dx,pFRut->GetPtList()[0].dy,pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy );
                    double dbDis2 = CalcDist(pFRut->GetPtList()[pFRut->GetPtNum()-1].dx,pFRut->GetPtList()[pFRut->GetPtNum()-1].dy,
                        pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy );
                    if( dbDis1 < dbDis2 )
                    {
                        if(m_pVNode)
                        {
                            ASSERT(m_pLaneBlockAddr);
                            CGIS_FeatureRut::LPLANEINFO pLaneInfoInFeatureRut = new CGIS_FeatureRut::LANEINFO;
                            pFRut->ReadLaneInfo((BYTE*)m_pLaneBlockAddr,pFRut->GetLaneInfoOffset(),pLaneInfoInFeatureRut);
                            ST_LANDINFO* pLaneInfo = pLaneInfoInFeatureRut->ExtractStartLaneInfo(pRoadSectionNext->GetRutID());
                            delete pLaneInfoInFeatureRut;
                            pLaneInfoInFeatureRut = NULL;
                            m_pVNode->SetLaneInfoDirect(pLaneInfo);
                        }
                        //////////////////////////////////////////////////////////////////////////
                    }
                    else
                    {
                        if(m_pVNode)
                        {
                        //���س�����Ϣ
                        if(pFRut->GetLaneInfoOffset())
                        {
                            ASSERT(m_pLaneBlockAddr);
                            CGIS_FeatureRut::LPLANEINFO pLaneInfoInFeatureRut = new CGIS_FeatureRut::LANEINFO;
                            pFRut->ReadLaneInfo((BYTE*)m_pLaneBlockAddr,pFRut->GetLaneInfoOffset(),pLaneInfoInFeatureRut);
                            ST_LANDINFO* pLaneInfo = pLaneInfoInFeatureRut->ExtractEndLaneInfo(pRoadSectionNext->GetRutID());
                            delete pLaneInfoInFeatureRut;
                            pLaneInfoInFeatureRut = NULL;
                            m_pVNode->SetLaneInfoDirect(pLaneInfo);
                        }
                        }
                        //////////////////////////////////////////////////////////////////////////
                    }
                }
            }
            //////////////////////////////////////////////////////////////////////////
            

			pos = pPathpointList->InsertAfter( pos, m_pVNode );
			int nVRNum = pVtx->GetVRutNum();
			ASSERT( nVRNum > 0 );
			CGIS_NetworkRut *pNRut = NULL;

            //������·��Ϊ����,�ҳ���·��Ϊ����,��ǰ���ڻ�����,��δʻ������
			if( pRoadSectionPrevious->GetRutType()==0 || pRoadSectionNext->GetRutType()==0 )
			{
                //��һ���ڵ㲻�ǻ��������ü���
				if( m_bRFlag == FALSE )
					m_nRMOrd = 0;
				//��䷽ʽ������·������ͼ��1����ͨ·�Ρ�2��ʮ��·�ڡ�3������
				byRAType = 3;
			}


			//ȡ�ú���ת��Ƕ�
			for( int i=0;	i < nVRNum;	i++ )
			{
				if( pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID() )
					m_nAPre = pVtx->GetVRTurnList()[i];
				else
				{
					if( pVtx->GetVRutList()[i] == pRoadSectionNext->GetRutID() )
						m_nANext = pVtx->GetVRTurnList()[i];
					pNRut = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
                    //////////////////////////////////////////////////////////////////////////
                    if(!pNRut)
                    {
                        CGIS_BreadthMap* pBreadthMap = GetBreadthMapByID2(pVtx->GetVBreadthID());
                        ASSERT(pBreadthMap);
                        int nReadErrorCode = 0;
                        if(!pBMNetwork->ReadBreadthNetworkRoadSectionByIDArray(&pBreadthMap,pVtx->GetVRutNum(),pVtx->GetVRutList(),nReadErrorCode))
                        {
                            //ASSERT(FALSE);
                            TRACE1("ReadBreadthNetworkRoadSectionByIDArray failed,error code(%d)\n",nReadErrorCode);
                            pNRut = NULL;
                        }
                        else
                        {
                            pNRut = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
                        }
                        
                    }              
                    //////////////////////////////////////////////////////////////////////////

                    //ȡ��ǰ����·��,
          			if( pNRut )
					{
						if( pNRut->GetRutType() == 4 && byRAType != 3 )//��������һ������·����·���԰�����·����·������
							byRAType = 2;
						else if( pNRut->GetRutType() == 0 )
							byRAType = 3;//���������·��Ϊ����·������Ȼ����·������ͼΪ������



						if( byRAType == 3 )
						{
							//ͳ�ƻ���������
							if( 
                                (pNRut != pRoadSectionNext || pRoadSectionNext->GetRutType() != 0)
                                && 
                                pNRut->GetRutType() != 0
                                && 
								pBMNetwork->GetNextRutVtx(pVtx, pNRut) != NULL 
                                )
								m_nRMOrd++;
						}
					}
                    else
                    {
#ifdef _DEBUG                        
                        if( byRAType == 3 )
                        {                           
                            TRACE2("���ڻ���������·��[%d][%d]�ѱ�����.�˻������ڲ�����ȷ����.\n",\
                                pVtx->GetVBreadthID(), pVtx->GetVRutList()[i]);
                        }                       
#endif
                    }
				}
			}
            
			if( byRAType == 3 )
            {
                m_pVNode->SetRMOrder( m_nRMOrd );
                
#ifdef _ANALYSE_FULLPATHDATA
                { 
                    if(pPathpointList->GetCount() == 172)
                    {
                        ASSERT(FALSE);
                    }
                }
                TRACE2("��%.5d���ǻ���,��������Ϊ%d\n",pPathpointList->GetCount(), m_nRMOrd);
                DUMPSTATE(m_pVNode);
                TRACE_SLEEP_BY_TIMES(50);
#endif
            }
            //////////////////////////////////////////////////////////////////////////
            //�л���
//            m_pVNode->GetVNodeInfo()->byConfusion = 16;
//            m_pVNode->GetVNodeInfo()->byConfusion2 = 16;
//            
//            switch(byRAType)
//            {
//            case 1:
//            case 2:
//                JudgePathNodeRutsConfuse(pBMNetwork,m_pVNode,pFRList,pVtx,pRoadSectionPrevious,pRoadSectionNext);
//                break;
//            case 3:
//                    break;
//            default:
//                ASSERT(FALSE);
//                break;
//            }
            
            //////////////////////////////////////////////////////////////////////////
			if( m_bRFlag == FALSE )
			{
				//���·������ͼ������Ϣ
				FullPathNodeRuts(
                    pBMNetwork,             
                    m_pVNode,               
                    pFRList,                
                    pVtx,                   
                    pRoadSectionPrevious,   
                    pRoadSectionNext,       
                    byRAType                
                    );
                
				if( byRAType == 3 )
					m_nAPre = -1800;
			}

            
			if( pRoadSectionNext->GetRutType() == 0 )
				m_bRFlag = TRUE;
			else 
				m_bRFlag = FALSE;
            }
            else
            {
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pVtx->GetVtxPosition() );
			pPPoint = pPathPt;
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathpointList->InsertAfter( pos, pPathPt );

            }
		}
		else
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pVtx->GetVtxPosition() );
			pPPoint = pPathPt;
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathpointList->InsertAfter( pos, pPathPt );
		}
	}
	else
	{
		CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
		pPPoint = pPathPt;
		pPathPt->SetPathpointPosition( pVtx->GetVtxPosition() );
		pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
		pos = pPathpointList->InsertAfter( pos, pPathPt );
	}


    //////////////////////////////////////////////////////////////////////////
    //�����Ǵ�������,������pathpoint,Ҳ������NODE
    //////////////////////////////////////////////////////////////////////////
    
	//·�����
	pFRut = pFRList[pRoadSectionNext->GetRutID()];
	//�����ͨ·�κͻ���
	if( pRoadSectionNext->GetRutType() != 4 || (byRAType != 2 && pRoadSectionPrevious->GetRutType() != 4) )
	{
		if( m_pVNode )
		{
            //ȡ��ʻ��·�εĽǶ�
			int nVRNum = pVtx->GetVRutNum();
			int nAngNextRut = 0;
			ASSERT( nVRNum > 0 );
			for( int i=0;	i<nVRNum;	i++ )
			{
				if( pVtx->GetVRutList()[i] == pRoadSectionNext->GetRutID() )
				{
					nAngNextRut = pVtx->GetVRTurnList()[i];
					break;
				}
			}
            //����ʻ��·�ε�ID
			m_pVNode->SetNextRutID( pRoadSectionNext->GetRutID() );


            //���ݿռ���������ʻ��·�ε�����
			if( pFRut )
			{
				if( pFRut->GetAnnotation() != NULL )
				{
					char* szAnno = new char[pFRut->GetAnnoNum()];
					memcpy( szAnno, pFRut->GetAnnotation(), pFRut->GetAnnoNum() );
					m_pVNode->SetRutAnno( szAnno );
					m_pVNode->SetRutAnnoNum( pFRut->GetAnnoNum() );
				}
			}
            //����ʻ��·�εĵȼ�,����
			m_pVNode->SetRType( pRoadSectionNext->GetType() );
			m_pVNode->SetRutType( pRoadSectionNext->GetRutType() );
            //����ʻ��·�εĻ�������(�շ�,������)
			FullVRutInfo( pRoadSectionNext, m_pVNode->GetVRutInfo() );
			//����˳�е�·����Ϣ
			if( m_bTrunFlag )
				m_pVNode->SetTurnAngle( m_nAPre-nAngNextRut );
			else
				m_pVNode->SetTurnAngle( 180 );
			FullVNodeInfo( m_pFRPre, m_pVNode, nREBID, nRutEndID, m_pVNode->GetVNodeInfo() );
            JudgePathNodeRutsConfuse((CGIS_MapBreadthNetwork*)m_pMapNetwork ,m_pVNode,pFRList,pVtx,pRoadSectionPrevious,pRoadSectionNext);

            //�����U�͹�������������·��������0˵����IMI,������·����·��
			if( m_pVNode->GetVNodeInfo()->byAngle == 5 && m_byMRNum > 0 )
				m_pVNode->GetVNodeInfo()->byIMI = 23;



            //////////////////////////////////////////////////////////////////////////
            //�ÿ�����
			m_pVNode = NULL;
            m_byMRNum = 0;
        }
        if( m_pFRPre )
            delete m_pFRPre;
        m_pFRPre = NULL;
        if( pFRut )
            m_pFRPre = (CGIS_FeatureRut*)pFRut->Clone();
    }
	else
		m_byMRNum++;

    //////////////////////////////////////////////////////////////////////////
	//·�οռ��������
	if( pFRut == NULL )
		return FALSE;
	if( IS_SAME_GEO_POINT(pFRut->GetPtList()[0],pVtx->GetVtxPosition()) )   
    {
        FullPathNodeArc( pPPoint, pFRut, pVtx->GetVtxPosition(), pBMap, TRUE, pPathpointList, pos );
    }
	else if( IS_SAME_GEO_POINT(pFRut->GetPtList()[pFRut->GetPtNum()-1],pVtx->GetVtxPosition()) )
    {
        FullPathNodeArc( pPPoint, pFRut, pVtx->GetVtxPosition(), pBMap, FALSE, pPathpointList, pos );
    }
	else
	{
		double dbDis1 = CalcDist(pFRut->GetPtList()[0].dx,pFRut->GetPtList()[0].dy,pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy );
		double dbDis2 = CalcDist(pFRut->GetPtList()[pFRut->GetPtNum()-1].dx,pFRut->GetPtList()[pFRut->GetPtNum()-1].dy,
							pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy );
		if( dbDis1 < dbDis2 )
        {
            FullPathNodeArc( pPPoint, pFRut, pVtx->GetVtxPosition(), pBMap, TRUE, pPathpointList, pos );
        }
		else
        {
            FullPathNodeArc( pPPoint, pFRut, pVtx->GetVtxPosition(), pBMap, FALSE, pPathpointList, pos );
        }
	}
	return TRUE;
}
/**
 * @brief ֻ�����ռ�����
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::FullPathNodeV2( 
    CGIS_MapBreadthNetwork *pBMNetwork, 
    CGIS_FeatureRut *pFRList[MAX_BRUT_NUM],
    CGIS_NetworkVtx *pVtx, 
    CGIS_BreadthMap *pBMap,
    CGIS_NetworkRut *pRoadSectionPrevious, 
    CGIS_NetworkRut *pRoadSectionNext, 
    int nREBID, 
    int nRutEndID,
    CPtrList *pPathpointList, 
    POSITION &pos )
{

	CGIS_Pathpoint *pPPoint = NULL;
	CGIS_FeatureRut *pFRut = NULL;

	pFRut = pFRList[pRoadSectionNext->GetRutID()];  
    //////////////////////////////////////////////////////////////////////////
	//·�οռ��������
	if( pFRut == NULL )
		return FALSE;
	if( fabs(pFRut->GetPtList()[0].dx-pVtx->GetVtxPosition().dx)<FLOAT_DETA_XY &&
			fabs(pFRut->GetPtList()[0].dy-pVtx->GetVtxPosition().dy)<FLOAT_DETA_XY )
		FullPathNodeArc( pPPoint, pFRut, pVtx->GetVtxPosition(), pBMap, TRUE, pPathpointList, pos );
	else if( fabs(pFRut->GetPtList()[pFRut->GetPtNum()-1].dx-pVtx->GetVtxPosition().dx)<FLOAT_DETA_XY &&
			fabs(pFRut->GetPtList()[pFRut->GetPtNum()-1].dy-pVtx->GetVtxPosition().dy)<FLOAT_DETA_XY )
		FullPathNodeArc( pPPoint, pFRut, pVtx->GetVtxPosition(), pBMap, FALSE, pPathpointList, pos );
	else
	{
		double dbDis1 = CalcDist(pFRut->GetPtList()[0].dx,pFRut->GetPtList()[0].dy,pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy );
		double dbDis2 = CalcDist(pFRut->GetPtList()[pFRut->GetPtNum()-1].dx,pFRut->GetPtList()[pFRut->GetPtNum()-1].dy,
							pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy );
		if( dbDis1 < dbDis2 )
			FullPathNodeArc( pPPoint, pFRut, pVtx->GetVtxPosition(), pBMap, TRUE, pPathpointList, pos );
		else
			FullPathNodeArc( pPPoint, pFRut, pVtx->GetVtxPosition(), pBMap, FALSE, pPathpointList, pos );
	}
	return TRUE;
}

void CGIS_GeoMap::GetBVRuts( CPtrList *pVList, POSITION &pos, int nRID )
{
	CGIS_MapLayer *pLayer = NULL;
	int i;
	for( i=0;	i<MAX_RLAY_NUM;	i++ )
	{
		pLayer = m_pLList[i];
		if( pLayer )
			pLayer->DecreaseLCount( m_pBMNet->m_nBreadthID );
		m_pLList[i] = NULL;
	}
	memset( m_pFRList, 0, sizeof(CGIS_FeatureRut*)*MAX_BRUT_NUM );
	
	BYTE pRNum[MAX_RLAY_NUM];
	memset( pRNum, 0, sizeof(BYTE)*MAX_RLAY_NUM );
	CGIS_NetworkVtx *pVtx = NULL;
	BYTE byBase = 5;
	BYTE byLID = 0;
	while( pos )
	{
		pVtx = (CGIS_NetworkVtx*)pVList->GetNext(pos);
		if( pVtx )
		{
			m_pBMNet = &m_pBMapList[pVtx->GetVBreadthID()];
			for( i=0;	i<pVtx->GetVRutNum();	i++ )
			{
				byLID = pVtx->GetVRLList()[i];
				if( pRNum[byLID-byBase] != 0 )
					continue;
				else
					pRNum[byLID-byBase] = 1;
			}
			if( pVtx->GetVtxEdge() == 1 )
				break;
		}
	}
	if( m_pBMNet == NULL )
		return ;
	ASSERT( m_pBMNet != NULL );
	m_nBID = m_pBMNet->m_nBreadthID;
	int nCount = 0;
	ASSERT( m_LayerManagerSet );
	POSITION ps = m_LayerManagerSet->GetHeadPosition();
	CGIS_LayerManager *pLManager = NULL;
	while( ps )
	{
		pLManager = (CGIS_LayerManager*)m_LayerManagerSet->GetNext(ps);
		if( pLManager )
		{
			short nKey = 0;
			IGIS_Layer* pLay = NULL;
			POSITION psLay = pLManager->GetLayerListMap()->GetStartPosition();
			while ( psLay )
			{
				pLManager->GetLayerListMap()->GetNextAssoc( psLay, nKey, pLay );
				if( pLay )
				{
					if( pLay->GetLayerInfo()->GetLayerID() >4 && pLay->GetLayerInfo()->GetLayerID() <12 
						&& pRNum[pLay->GetLayerInfo()->GetLayerID()-byBase] != 0 )
					{
						m_pLList[nCount] = (CGIS_MapLayer*)pLay;
						nCount++;
					}
				}
			}
		}
	}
	CGIS_FeatureRut *pRut = NULL;
    unsigned int nErrorCode = 0;
	LoadBLData( m_pBMNet, m_pLList, nCount ,&nErrorCode);
	m_nFRLNum = nCount;
	for( i=0;	i<nCount;	i++ )
	{
		CPtrList *pRList = NULL;
//		m_pLList[i]->IncreaseLCount( m_pBMNet->m_nBreadthID );
		m_pLList[i]->GetObjectListMap()->Lookup( m_pBMNet->m_nBreadthID, pRList );
		if( pRList )
		{
			ps = pRList->GetHeadPosition();
			while( ps )
			{
				pRut = (CGIS_FeatureRut*)pRList->GetNext(ps);
				if( pRut )
				{
					m_pFRList[pRut->GetRutID()] = pRut;
				}
			}
		}
	}
	if( m_pFRPre == NULL )
	{
		if( m_pFRList[nRID] )
			m_pFRPre = (CGIS_FeatureRut*)m_pFRList[nRID]->Clone();
	}

    //////////////////////////////////////////////////////////////////////////
    {
        if(m_pLaneBlockAddr)
        {
            delete m_pLaneBlockAddr;
            m_pLaneBlockAddr = NULL;
        }
        CGIS_LFBand aLFBand;
        aLFBand.LoadLaneLayerV2(this,m_pBMNet,m_pLaneBlockAddr);
    }  
    //////////////////////////////////////////////////////////////////////////
    
    
}

/**
 * @brief ��
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::FullEVNode(
    CGIS_WayPoint *pWTwo, 
    CGIS_NetworkVtx *pVtx, 
    CGIS_NetworkRut *pRoadSectionPrevious, 
    CGIS_NetworkRut *pRoadSectionNext, 
    CPtrList *pPPList,
    POSITION &pos )
{

    //////////////////////////////////////////////////////////////////////////
    //070509��Ӵ˴���,���ڷ�ֹ������ֹ�ڵ���һ���߽�ڵ�ʱҲ�����������.
    if(pVtx->GetVtxEdge() == 1)
    {
        CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
        pPathPt->SetPathpointPosition( pVtx->GetVtxPosition() );
        pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
        pos = pPPList->InsertAfter( pos, pPathPt );
        
        return TRUE;
    }


    //////////////////////////////////////////////////////////////////////////
    
	CGIS_PathpointNode *pVNode = NULL;
    
	if( pRoadSectionPrevious->GetRutType() != 4 && 
        pRoadSectionNext->GetRutType() !=4 && 
        m_pVNode == NULL )
	{
		EnMapType enMapType = m_pMapInfo.GetMapType();
		ST_GEO_PTXY *pPt1,*pPt2;
		pPt1 = (ST_GEO_PTXY*)(&pWTwo->GetWPRutPtList(enMapType)[0]);
		pPt2 = (ST_GEO_PTXY*)(&pWTwo->GetWPRutPtList(enMapType)[pWTwo->GetWPRutPtNum(enMapType)-1]);
        
		if( pVtx->GetVRutNum() == 2 && pRoadSectionPrevious->GetRutType() == pRoadSectionNext->GetRutType())
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			if( CalcDist(pPt1->dx,pPt1->dy,pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy) >
				CalcDist(pPt2->dx,pPt2->dy,pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy) )
				pPathPt->SetPathpointPosition( *pPt2 );
			else
				pPathPt->SetPathpointPosition( *pPt1 );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPPList->InsertAfter( pos, pPathPt );
			return TRUE;
		}
		if( m_pVNode )
			delete m_pVNode;
		m_pVNode = NULL;


		pVNode = new CGIS_PathpointNode;
		pVNode->SetPathpointType( EN_PATHPOINTTYPE_NODE );
        
        //////////////////////////////////////////////////////////////////////////
        {
            CGIS_FeatureRut* pFRut = m_pFRList[pRoadSectionPrevious->GetRutID()];
            if(pFRut)
            {
                CGIS_FeatureRut::SIGNINFO* pSignSrc = pFRut->GetSignInfoByExitRoadSectionID(pRoadSectionNext->GetRutID());
                pVNode->SetSignInfo(pSignSrc);
                if(pSignSrc && pSignSrc->byNameLen > 0)
                {
                    pVNode->GetVRutInfo()->bHaveSign = 1;
                }

                //pFRut->get
            }
            
        }
        //////////////////////////////////////////////////////////////////////////
        




		if( CalcDist(pPt1->dx,pPt1->dy,pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy) >
			CalcDist(pPt2->dx,pPt2->dy,pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy) )
			pVNode->SetPathpointPosition( *pPt2 );
		else
			pVNode->SetPathpointPosition( *pPt1 );
//		pVNode->SetPathpointPosition( pVtx->GetVtxPosition() );
		pVNode->SetNodeVtxID( pVtx->GetVtxID() );
		pVNode->SetPBreadthID( m_nBID );
		BYTE byRAType = 1;
		if( pRoadSectionPrevious->GetRutType()==0 || pRoadSectionNext->GetRutType()==0 )
		{
			byRAType = 3;
			if( m_bRFlag == FALSE )
				m_nRMOrd = 0;
		}
				
		pos = pPPList->InsertAfter( pos, pVNode );
		int nVRNum = pVtx->GetVRutNum();
		CGIS_NetworkRut *pNRut = NULL;
		CGIS_MapBreadthNetwork* pBMNetwork = (CGIS_MapBreadthNetwork*)m_pMapNetwork;
		ASSERT( nVRNum > 0 );
		for( int i=0;	i<nVRNum;	i++ )
		{
			if( pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID() )
				m_nAPre = pVtx->GetVRTurnList()[i];
			pNRut = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
			if( pNRut )
			{
				if( pNRut->GetRutType() == 4 )
					byRAType = 2;
				else if( pNRut->GetRutType() == 0 )
					byRAType = 3;
				if( byRAType == 3 )
				{
					if( (pNRut!=pRoadSectionNext || pRoadSectionNext->GetRutType()!=0) && pNRut->GetRutType() != 0 && 
						pBMNetwork->GetNextRutVtx(pVtx, pNRut) != NULL )
						m_nRMOrd++;
				}
			}
		}
		if( byRAType == 3 )
			pVNode->SetRMOrder( m_nRMOrd );
		if( m_bRFlag == FALSE )
			FullPathNodeRuts( pBMNetwork, pVNode, m_pFRList, pVtx, pRoadSectionPrevious, pRoadSectionNext, byRAType );
		if( pRoadSectionNext->GetRutType() == 0 )
			m_bRFlag = TRUE;
		else 
			m_bRFlag = FALSE;
	}
	int nAngNextRut = 0;
	int nVRNum = pVtx->GetVRutNum();
	ASSERT( nVRNum > 0 );
	for( int i=0;	i<nVRNum;	i++ )
	{
		if( pVtx->GetVRutList()[i] == pRoadSectionNext->GetRutID() )
		{
			nAngNextRut = pVtx->GetVRTurnList()[i];
			break;
		}
	}
	if( nVRNum <= 0 )
		nAngNextRut = m_nAPre-180;
	if( m_pVNode )
	{
		m_pVNode->SetRType( pRoadSectionNext->GetType() );
		m_pVNode->SetRutType( pRoadSectionNext->GetRutType() );
		FullVRutInfo( pRoadSectionNext, m_pVNode->GetVRutInfo() );
		if( m_bTrunFlag )
			m_pVNode->SetTurnAngle( m_nAPre-nAngNextRut );
		else
			m_pVNode->SetTurnAngle( 180 );
		FullVNodeInfo( m_pFRPre, m_pVNode, -1, -1, m_pVNode->GetVNodeInfo() );
        JudgePathNodeRutsConfuse((CGIS_MapBreadthNetwork*)m_pMapNetwork ,m_pVNode,NULL,pVtx,pRoadSectionPrevious,pRoadSectionNext);

		if( m_pVNode->GetVNodeInfo()->byAngle == 5 && m_byMRNum > 0 )
			m_pVNode->GetVNodeInfo()->byIMI = 23;
		if( m_pVNode->GetRutAnnoNum() <= 0 && !pWTwo->GetWPRutName().IsEmpty() )
		{
			char szInfo[32];
			int nNum = UnicodeToChar( pWTwo->GetWPRutName(), szInfo, 32 );
			char* szAnno = new char[nNum];
			memcpy( szAnno, szInfo, nNum );
			m_pVNode->SetRutAnno( szAnno );
			m_pVNode->SetRutAnnoNum( nNum );
		}
		m_pVNode = NULL;
		m_byMRNum = 0;
	}


	if( pVNode )
	{
		pVNode->SetNextRutID( pRoadSectionNext->GetRutID() );
		if( pWTwo )
		{
			if( !pWTwo->GetWPRutName().IsEmpty() )
			{
				char szInfo[32];
				int nNum = UnicodeToChar( pWTwo->GetWPRutName(), szInfo, 32 );
				char* szAnno = new char[nNum];
				memcpy( szAnno, szInfo, nNum );
				pVNode->SetRutAnno( szAnno );
				pVNode->SetRutAnnoNum( nNum );
			}
		}
		pVNode->SetRType( pRoadSectionNext->GetType() );
		pVNode->SetRutType( pRoadSectionNext->GetRutType() );
		FullVRutInfo( pRoadSectionNext, pVNode->GetVRutInfo() );


		if( m_bTrunFlag )
			pVNode->SetTurnAngle( m_nAPre-nAngNextRut );
		else
			pVNode->SetTurnAngle( 180 );


		FullVNodeInfo( m_pFRPre, pVNode, -1, -1, pVNode->GetVNodeInfo() );
        JudgePathNodeRutsConfuse((CGIS_MapBreadthNetwork*)m_pMapNetwork ,pVNode,NULL,pVtx,pRoadSectionPrevious,pRoadSectionNext);
		if( pVNode->GetVNodeInfo()->byAngle == 5 && m_byMRNum > 0 )
			pVNode->GetVNodeInfo()->byIMI = 23;
		m_pVNode = NULL;
		pVNode = NULL;
		m_byMRNum = 0;
		m_pFRPre = NULL;
		return TRUE;
	}
	CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
	pPathPt->SetPathpointPosition( pVtx->GetVtxPosition() );
	pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
	pos = pPPList->InsertAfter( pos, pPathPt );
	return TRUE;
}
//���ݿռ��������node������Ϣ.
BOOL CGIS_GeoMap::GetRutData( 
    CGIS_NetworkVtx *pVtx,
    CGIS_NetworkRut *pRoadSectionPrevious, 
    CGIS_NetworkRut* pRoadSectionNext, 
    int nRutEndBreadthID,
    int nRutEndID,
    CPtrList *pPathpointList, 
    POSITION &pos)
{
	///�������ݵ��ռ�����
	if( pVtx->GetVtxEdge() == 1 )
	{
		CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
		pPathPt->SetPathpointPosition( pVtx->GetVtxPosition() );
		pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
		pos = pPathpointList->InsertAfter( pos, pPathPt );

		CGIS_FeatureRut *pFRut = m_pFRList[pRoadSectionNext->GetRutID()];
		if( pFRut )
		{
			if( pFRut->GetPtNum() == 2 )
				return TRUE;
			if( fabs(pFRut->GetPtList()[0].dx-pVtx->GetVtxPosition().dx)<FLOAT_DETA_XY &&
					fabs(pFRut->GetPtList()[0].dy-pVtx->GetVtxPosition().dy)<FLOAT_DETA_XY )
				FullPathNodeArc( pPathPt, pFRut, pVtx->GetVtxPosition(), NULL, TRUE, pPathpointList, pos );
			else if( fabs(pFRut->GetPtList()[pFRut->GetPtNum()-1].dx-pVtx->GetVtxPosition().dx)<FLOAT_DETA_XY &&
					fabs(pFRut->GetPtList()[pFRut->GetPtNum()-1].dy-pVtx->GetVtxPosition().dy)<FLOAT_DETA_XY )
				FullPathNodeArc( pPathPt, pFRut, pVtx->GetVtxPosition(), NULL, FALSE, pPathpointList, pos );
			else
			{
				double dbDis1 = CalcDist( pVtx->GetVtxPosition().dx, pVtx->GetVtxPosition().dy, pFRut->GetPtList()[1].dx, pFRut->GetPtList()[1].dy );
				double dbDis2 = CalcDist( pVtx->GetVtxPosition().dx, pVtx->GetVtxPosition().dy, 
							pFRut->GetPtList()[pFRut->GetPtNum()-2].dx, pFRut->GetPtList()[pFRut->GetPtNum()-2].dy );
				if( dbDis1 > dbDis2 )
					FullPathNodeArc( pPathPt, pFRut, pVtx->GetVtxPosition(), NULL, FALSE, pPathpointList, pos );
				else 
					FullPathNodeArc( pPathPt, pFRut, pVtx->GetVtxPosition(), NULL, TRUE, pPathpointList, pos );
			}
		}
		return TRUE;
	}

	///��ͨnode�������ݵ��ռ�����
	FullPathNode( 
        (CGIS_MapBreadthNetwork*)m_pMapNetwork,
        m_pFRList, 
        pVtx, 
        NULL, 
        pRoadSectionPrevious, 
        pRoadSectionNext,
        nRutEndBreadthID,
        nRutEndID,
        pPathpointList,
        pos );
	return TRUE;
}


//���ݿռ��������node������Ϣ.
BOOL CGIS_GeoMap::GetRutDataV3( 
    CGIS_NetworkVtx *pVtx,
    CGIS_NetworkRut *pRoadSectionPrevious, 
    CGIS_NetworkRut* pRoadSectionNext, 
    int nRutEndBreadthID,
    int nRutEndID,
    CPtrList *pPathpointList, 
    POSITION &pos,
    CPtrList* pNodeList,
    POSITION posOfNodeList
    )
{
    //////////////////////////////////////////////////////////////////////////
    //���������ÿһ��·�ε�path��ȥ.
    CGIS_PathpointNodeV2* pNode = NULL;  
    {
        pNode = new CGIS_PathpointNodeV2;
        CGIS_PathpointNodeV2::stuRoadSectionInfo aNextRoadSectionInfo;
        aNextRoadSectionInfo.nLength = pRoadSectionNext->GetRutLength();
        aNextRoadSectionInfo.nType = pRoadSectionNext->GetType();
        pNode->SetNextRoadSectionInfo(&aNextRoadSectionInfo);
        pNode->SetNextRutID(pRoadSectionNext->GetRutID());
        CGIS_NetworkRut::stuRoadSectionLimitInfo* pLimitInfo = pRoadSectionPrevious->GetLimitInfo();
        if(pLimitInfo)
        {
            ASSERTVALID(pLimitInfo);
            pNode->SetLimitInfo(pLimitInfo->GetGroup(TRUE));
        }

        //////////////////////////////////////////////////////////////////////////
        //ȡ������Ϣ
        
        //////////////////////////////////////////////////////////////////////////
        
        pNode->SetPosOfPathPointList(pos);     
        posOfNodeList = pNodeList->InsertAfter(posOfNodeList,pNode);
        ASSERTVALID(pNode);
                
        //����Ψһͨ�б�־.
    }

    POSITION posCurrent = pos;
    
    //////////////////////////////////////////////////////////////////////////


    BOOL bRtn = GetRutData(pVtx,
    pRoadSectionPrevious, 
    pRoadSectionNext, 
    nRutEndBreadthID,
    nRutEndID,
    pPathpointList, 
    pos);

    //////////////////////////////////////////////////////////////////////////
    pPathpointList->GetNext(posCurrent);
    pNode->SetPosOfPathPointList(posCurrent);
    //////////////////////////////////////////////////////////////////////////
    
    return bRtn;
}

/**
 * @brief ֻ�����ռ�����,������NODE.
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::GetRutDataV2( 
    CGIS_NetworkVtx *pVtx,
    CGIS_NetworkRut *pRoadSectionPrevious, 
    CGIS_NetworkRut* pRoadSectionNext, 
    int nRutEndBreadthID,
    int nRutEndID,
    CPtrList *pPathpointList, 
    POSITION &pos)
{

	ASSERT( pVtx->GetVtxEdge() != 1 );
	FullPathNodeV2( 
        (CGIS_MapBreadthNetwork*)m_pMapNetwork,
        m_pFRList, 
        pVtx, 
        NULL, 
        pRoadSectionPrevious, 
        pRoadSectionNext,
        nRutEndBreadthID,
        nRutEndID,
        pPathpointList,
        pos );
	return TRUE;
} 

IGIS_TOOL* CGIS_GeoMap::GetTool(EnTooType enToolType){
	if( m_enMFType == EN_MFTYPE_GUIDE30 ){
		/*
		if( enToolType == LAYER_TOOL || enToolType == EDIT_FEATURE )
			;
		else
			Load( m_pMapInfo.GetMapFilePath() );
			*/
	}
	//�������
	/*
	IGIS_TOOL* pTempTool;
	if(m_ToolSet != NULL){
		POSITION pos = m_ToolSet->GetHeadPosition();
		while(pos)
		{
			pTempTool = (IGIS_TOOL*)m_ToolSet->GetNext(pos);
			if(pTempTool->GetType() == enToolType)
			{
				return pTempTool;
			}
		}
	}
	else
		m_ToolSet = new CPtrList;
	*/
	std::vector<IGIS_TOOL*>::iterator itr;
	for(itr =m_ToolSet.begin();itr!=m_ToolSet.end();itr++){
		if( (*itr)->GetType() == enToolType){
			return *itr;
		}
	}
	if(enToolType == QUERY_BYSHARP){ //��״��ѯ
		CGIS_QueryByShape* pQueryTool = new CGIS_QueryByShape;
		pQueryTool->SetMap(this);
		//m_ToolSet->AddTail(pQueryTool);
		m_ToolSet.push_back(pQueryTool);
		return (IGIS_TOOL*)pQueryTool;
	}else if(enToolType == QUERY_BYATT){//���Բ�ѯ
		CGIS_QueryByAtt* pQueryTool = new CGIS_QueryByAtt;
		pQueryTool->SetMap(this);
		//m_ToolSet->AddTail(pQueryTool);
		m_ToolSet.push_back(pQueryTool);
		return (IGIS_TOOL*)pQueryTool;
	}else if( enToolType == LAYER_TOOL ){
		CGIS_LayerTool *pLTool = new CGIS_LayerTool;
		pLTool->SetMap( this );
		//m_ToolSet->AddTail(pLTool);
		m_ToolSet.push_back(pLTool);
		return (IGIS_TOOL*)pLTool;
	}else if( enToolType == EDIT_FEATURE ){
		/*
		CGIS_FeatureEditer *pFTool = new CGIS_FeatureEditer;
		pFTool->SetMap( this );
		POSITION pos = m_LayerManagerSet->GetHeadPosition();
		CGIS_LayerManager *pLManager = NULL;
		if( pos )
			pLManager = (CGIS_LayerManager*)m_LayerManagerSet->GetNext( pos );
		IGIS_Layer *pLayer = NULL;
		if( pLManager )
			pLayer = pLManager->GetFirstLayer( );
		if( pLayer )
			pFTool->SetLayer( pLayer );
		else 
		{
			// ֱ���˳����Ѿ�������ڴ潫����й©���������ͷ��ڴ������
			if (pFTool != NULL)
			{
				delete pFTool;
				pFTool = NULL;
			}
			return NULL;
		}
		m_ToolSet->AddTail(pFTool);
		return (IGIS_TOOL*)pFTool;*/
		//return NULL; //Ŀǰδ���
	}
	return NULL;
}

BOOL CGIS_GeoMap::ReleaseTool(){
	if( m_enMFType == EN_MFTYPE_GUIDE30 ){
		UnLoad( m_pMapInfo.GetMapID() );
	}
	return TRUE;
}

void CGIS_GeoMap::CalMatchBMap( ST_GEO_PTXY pt )
{
    //////////////////////////////////////////////////////////////////////////
    CSpaceMemoryLock aLock;    

	int nBMIDs[4];
	memset( nBMIDs, 0, sizeof(int)*4);
	int nBNum = 0;
	ST_GEO_RECT rtCur;
	rtCur.dLeft = pt.dx - 0.001;
	rtCur.dRight = pt.dx + 0.001;
	rtCur.dTop = pt.dy - 0.001;
	rtCur.dBottom = pt.dy + 0.001;
	CalculateBMapIDInRect(rtCur, nBMIDs, nBNum );
	for( int i=0;	i<nBNum;	i++ )
	{
		CGIS_BreadthMap *pBMap = GetBreadthMapByID( nBMIDs[i] );
		if( pBMap )
		{
			ST_GEO_RECT rt = CalculateBMapRect( nBMIDs[i] );
			if(  IsPtInRect(pt.dx , pt.dy , rtCur.dLeft, rtCur.dTop , rtCur.dRight , rtCur.dBottom ) )
			{
				if( m_nMatchBID != pBMap->m_nBreadthID  )
				{
					CGIS_LayerManager *pLManager = NULL;
					POSITION pos = m_LayerManagerSet->GetHeadPosition();
					while(pos!= NULL)
					{
						pLManager = (CGIS_LayerManager*)m_LayerManagerSet->GetNext(pos);
						if(pLManager != NULL)
						{
							POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
							IGIS_Layer *pLayer = NULL;	
							short    nKey;
							while(pos1 != NULL)
							{
								pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
								if( pLayer )
								{
									int nLayerID = pLayer->GetLayerInfo()->GetLayerID();
									//��·ͼ��
									if( nLayerID > 4 && nLayerID < 12 )
									{
										CGIS_MapLayer *pBLList[1];
										pBLList[0] = (CGIS_MapLayer*)pLayer;
										if( !LoadBLData( pBMap, pBLList, 1 ) )
											return ;
//										pBLList[0]->IncreaseLCount( pBMap->m_nBreadthID );
										if( m_nMatchBID != -1 )
											pBLList[0]->DecreaseLCount( m_nMatchBID );
									}
								}
							}
						}
					}
					m_nMatchBID = pBMap->m_nBreadthID;
				}

				break;
			}
		}
	}
}
static ST_ROAD_CHOOSE g_RoadChoose[100];
EnMatchRoad CGIS_GeoMap::MatchRoad(ST_GEO_PTXY pt,ST_GEO_PTXY& Rpt,EnMathState enState,CGIS_FeatureRut** pLastRoat,int Routedis,BOOL bHighWay ,ST_GEO_PTXY ptMatchLast,int& nLeaveCount )
{
	CalMatchBMap( pt );
	int MatchDis;
	if( bHighWay )
		MatchDis = MATCH_HIGHWAY_NOMAL;
	else
		MatchDis = MATCH_CITYROAD_NORMAL;

	// �������ʱ��Ϊ�˱�����ܳ��ֲ�ȷ����ֵ�����丳��ֵ��
	CGIS_QueryByShape* pTempQuery = NULL;
	CGIS_FeatureRut* pTempRoat = NULL;
	ST_GEO_PTXY ptRTemp = {0, 0};
	int nDisLeave = 0;
	if( bHighWay )
		nDisLeave = MATCH_HIGHWAY_LEAVE;
	else
		nDisLeave = MATCH_CITYROAD_LEAVE;
	double mindis = 100000;
	BOOL bHaveLast = FALSE;
	if((*pLastRoat) != NULL)
	{
		int nNearNode = 0;
		bHaveLast = TRUE;
		int nCount = (*pLastRoat)->GetPtNum();
		for( int i = 0 ; i < nCount - 1 ; i++ )
		{
			ST_GEO_PTXY temppt = DisPointToLine(pt,(*pLastRoat)->GetPtList()[i],(*pLastRoat)->GetPtList()[i+1]);
			BOOL bOnLine = PointOnLine(temppt,(*pLastRoat)->GetPtList()[i],(*pLastRoat)->GetPtList()[i+1]);
			if(bOnLine)
			{
				double dbTemp = CalcDist(pt.dx,pt.dy,temppt.dx,temppt.dy);
				if(dbTemp  < mindis)
				{
					//������һ�ڵ㸽���������䷧ֵ��С
					if( m_nCarRunDirect == 2 && i == 0 )//β����ͷ
					{
						int nNodeDis = CalcDist( (*pLastRoat)->GetPtList()[0].dx , (*pLastRoat)->GetPtList()[0].dy , pt.dx , pt.dy );
						if( nNodeDis < MatchDis )
						{
							if( bHighWay )
								MatchDis = MATCH_HIGHWAY_VTX;
							else
								MatchDis = MATCH_CITYROAD_VTX;
						}
					}
					else if ( m_nCarRunDirect == 1  && i == nCount - 2 )//ͷ����β
					{
						int nNodeDis = CalcDist( (*pLastRoat)->GetPtList()[nCount-1].dx , 
							(*pLastRoat)->GetPtList()[nCount-1].dy , pt.dx , pt.dy );
						if( nNodeDis < MatchDis )
						{
							if( bHighWay )
								MatchDis = MATCH_HIGHWAY_VTX;
							else
								MatchDis = MATCH_CITYROAD_VTX;
						}
					}
					mindis = dbTemp;
					ptRTemp = temppt;
					nNearNode = i;
				}
			}
			else
			{
				double dis1 = CalcDist(pt.dx,pt.dy,(*pLastRoat)->GetPtList()[i].dx,(*pLastRoat)->GetPtList()[i].dy);
				double dis2 = CalcDist(pt.dx,pt.dy,(*pLastRoat)->GetPtList()[i+1].dx,(*pLastRoat)->GetPtList()[i+1].dy);

				if( dis1 < dis2 && dis1 < mindis)
				{
					//������һ�ڵ㸽���������䷧ֵ��С
					if( m_nCarRunDirect == 2 && i == 0 )//β����ͷ
					{
						int nNodeDis = CalcDist( (*pLastRoat)->GetPtList()[0].dx , (*pLastRoat)->GetPtList()[0].dy , pt.dx , pt.dy );
						if( nNodeDis < MatchDis )
						{
							if( bHighWay )
								MatchDis = MATCH_HIGHWAY_VTX;
							else
								MatchDis = MATCH_CITYROAD_VTX;
						}
					}
					else if ( m_nCarRunDirect == 1  && i == nCount - 2 )//ͷ����β
					{
						int nNodeDis = CalcDist( (*pLastRoat)->GetPtList()[nCount-1].dx , 
							(*pLastRoat)->GetPtList()[nCount-1].dy , pt.dx , pt.dy );
						if( nNodeDis < MatchDis )
						{
							if( bHighWay )
								MatchDis = MATCH_HIGHWAY_VTX;
							else
								MatchDis = MATCH_CITYROAD_VTX;
						}
					}
					mindis = dis1;
					ptRTemp = (*pLastRoat)->GetPtList()[i];
					nNearNode = i;
				}
				if(dis1 > dis2 && dis2 < mindis)
				{
					//������һ�ڵ㸽���������䷧ֵ��С
					if( m_nCarRunDirect == 2 && i == 0 )//β����ͷ
					{
						int nNodeDis = CalcDist( (*pLastRoat)->GetPtList()[0].dx , (*pLastRoat)->GetPtList()[0].dy , pt.dx , pt.dy );
						if( nNodeDis < MatchDis )
						{
							if( bHighWay )
								MatchDis = MATCH_HIGHWAY_VTX;
							else
								MatchDis = MATCH_CITYROAD_VTX;
						}
					}
					else if ( m_nCarRunDirect == 1  && i == nCount - 2 )//ͷ����β
					{
						int nNodeDis = CalcDist( (*pLastRoat)->GetPtList()[nCount-1].dx , 
							(*pLastRoat)->GetPtList()[nCount-1].dy , pt.dx , pt.dy );
						if( nNodeDis < MatchDis )
						{
							if( bHighWay )
								MatchDis = MATCH_HIGHWAY_VTX;
							else
								MatchDis = MATCH_CITYROAD_VTX;
						}
					}
					mindis = dis2;
					ptRTemp = (*pLastRoat)->GetPtList()[i+1];
					nNearNode = i;
				}
			}
			if(i ==nCount - 2 &&  mindis <= MatchDis)
			{
				//���ڵ�������,����5֡��Ϊ������ʻ,������ƥ���µĵ�·(���׽������״̬�µ�ͷ����)
				int nDirection = (*pLastRoat)->GetRutDirection();
				if( (nDirection == 1 || nDirection == 2) )
				{
					int nDisToPt = CalcDist( m_ptLastXy.dx , m_ptLastXy.dy , ptRTemp.dx , ptRTemp.dy );
					if( nDisToPt > 10 )
					{
						//�ж��Ƿ�Ϊ������ʻ
						double dbAng = CalcAngle( ptRTemp.dx , ptRTemp.dy , m_ptLastXy.dx , m_ptLastXy.dy );
						double dbAngRoad = 0;
						//�Ƿ��ܱȽ�
						BOOL bCanCom = TRUE;
						if( nDirection == 2 )
						{
							ST_GEO_PTXY ptCom = (*pLastRoat)->GetPtList()[nNearNode+1];
							while( fabs(ptRTemp.dx-ptCom.dx) < FLOAT_ZERO
								 && fabs(ptRTemp.dy-ptCom.dy) < FLOAT_ZERO )
							{
								nNearNode++;
								if( nNearNode == nCount -1 )
								{
									bCanCom = FALSE;
									break;
								}
								else
									ptCom = (*pLastRoat)->GetPtList()[nNearNode+1];
							}
							if( bCanCom )
								dbAngRoad = CalcAngle( ptRTemp.dx , ptRTemp.dy ,ptCom.dx,ptCom.dy );
//							dbAngRoad = CalcAngle( ptRTemp.dx , ptRTemp.dy ,(*pLastRoat)->GetPtList()[(*pLastRoat)->GetPtNum() - 1].dx,(*pLastRoat)->GetPtList()[(*pLastRoat)->GetPtNum() - 1].dy );
						}
						if( nDirection == 1 )
						{
							ST_GEO_PTXY ptCom = (*pLastRoat)->GetPtList()[nNearNode];
							while( fabs(ptRTemp.dx-ptCom.dx) < FLOAT_ZERO
								 && fabs(ptRTemp.dy-ptCom.dy) < FLOAT_ZERO )
							{
								nNearNode--;
								if( nNearNode == -1 )
								{
									bCanCom = FALSE;
									break;
								}
								else
									ptCom = (*pLastRoat)->GetPtList()[nNearNode];
							}
							if( bCanCom )
								dbAngRoad = CalcAngle( ptRTemp.dx , ptRTemp.dy ,ptCom.dx,ptCom.dy );
//							dbAngRoad = CalcAngle( ptRTemp.dx , ptRTemp.dy ,(*pLastRoat)->GetPtList()[0].dx,(*pLastRoat)->GetPtList()[0].dy );
						}
						int nAngAbs = abs( dbAng * 180.0 / PI - dbAngRoad * 180.0 / PI );
						if( !bCanCom ||  nAngAbs < 50 || nAngAbs > 310  )
							m_nReCount = 0;
						else
						{
							m_nReCount++;
							ptRTemp = m_ptLastXy;
						}
					}
					else
					{
						m_nReCount = 0;
						ptRTemp = m_ptLastXy;
					}
					
				}
				else if( nDirection == 0 )
					m_nReCount = 0;
				if( m_nReCount < 5 )
				{
					Rpt = ptRTemp;
					//ȷ�����з���
					if( m_nCarRunDirect == 0 )
					{
						int nDisToPt = CalcDist( m_ptLastXy.dx , m_ptLastXy.dy , ptRTemp.dx , ptRTemp.dy );
						if( nDisToPt > 10 )
						{
							double dbAng = CalcAngle( ptRTemp.dx , ptRTemp.dy , m_ptLastXy.dx , m_ptLastXy.dy );
							double dbAngRoadEnd = CalcAngle( ptRTemp.dx , ptRTemp.dy ,
								(*pLastRoat)->GetPtList()[(*pLastRoat)->GetPtNum() - 1].dx,(*pLastRoat)->GetPtList()[(*pLastRoat)->GetPtNum() - 1].dy );
							double dbAngRoadStart = CalcAngle( ptRTemp.dx , ptRTemp.dy ,(*pLastRoat)->GetPtList()[0].dx,(*pLastRoat)->GetPtList()[0].dy );
							int nAngAbsEnd = abs( dbAng * 180.0 / PI - dbAngRoadEnd * 180.0 / PI );
							int nAngAbsStart = abs( dbAng * 180.0 / PI - dbAngRoadStart * 180.0 / PI );
							if( nAngAbsStart > 180 )
								nAngAbsStart = 360 - nAngAbsStart;
							if( nAngAbsEnd > 180 )
								nAngAbsEnd = 360 - nAngAbsEnd;
							if( nAngAbsStart > nAngAbsEnd  )
							{
								m_nCarRunDirect = 2;//β����ͷ
								m_nNextRutPt = nNearNode;
							}
							else
							{
								m_nCarRunDirect = 1;//ͷ����β
								m_nNextRutPt = nNearNode+1;
							}
						}
					}
					else
					{
						if( m_nCarRunDirect == 1 )
							m_nNextRutPt = nNearNode+1;
						else
							m_nNextRutPt = nNearNode;
					}
					m_ptLastXy = Rpt;
					if(enState == EN_MATCH_REALNAV)
					{
						if( Routedis > nDisLeave )
						{
							nLeaveCount++;
							if( ( (nLeaveCount >= 5)||(nLeaveCount>=3 && m_nCarRunDirect!=0) ) && (*pLastRoat)->GetRutID() != -1 ) 
								return EN_NAV_OUT;
							else
								return EN_MATCH_OUT;
						}
						else
							return EN_MATCH_OUT;
					}
					else
						return EN_MATCH_WELL;
				}
				if( m_nReCount == 5 )
				{
					m_nReCount = 0;
					if( bHighWay )
						MatchDis = MATCH_HIGHWAY_NOMAL;
					else
						MatchDis = MATCH_CITYROAD_NORMAL;
					break;
				}
			}
		}
//		(*pLastRoat) = NULL;
		if( bHighWay )
			MatchDis = MATCH_HIGHWAY_NOMAL;
		else
			MatchDis = MATCH_CITYROAD_NORMAL;
	}
	nLeaveCount = 0;
//	m_bPassRoad = FALSE;
	m_nReCount = 0;
	pTempQuery = (CGIS_QueryByShape*)GetTool(QUERY_BYSHARP);
	CPtrList RList;
    BOOL bSuccess = FALSE;
	bSuccess = 	pTempQuery->QueryByBuffer(RList,&pt,MatchDis,EN_OBJ_RUT);
    
	//��̬�����ĸ���
	int nChooseCount = 0;
	//û�е�·ƥ���
	if( !bHaveLast  && ptMatchLast.dx == 181 && ptMatchLast.dy == 91)
	{
		m_nCarRunDirect = 0;
		if(RList.GetCount() == 0)
		{
			Rpt = pt;
			m_ptLastXy = Rpt;
			return EN_MATCH_NULL;
		}
		
		int nMinRoadID = 0;
		POSITION pos1 = RList.GetHeadPosition();
		while( pos1 )
		{
			pTempRoat = (CGIS_FeatureRut*)RList.GetNext(pos1);
			//�ų�����ͨ�еĵ�·
			if( pTempRoat->GetRutDirection() == 3 )
				continue;
			for(int i = 0 ; i < pTempRoat->GetPtNum() - 1 ; i++)
			{
				ST_GEO_PTXY temppt = DisPointToLine(pt,pTempRoat->GetPtList()[i],pTempRoat->GetPtList()[i+1]);
				BOOL bOnLine = PointOnLine(temppt,pTempRoat->GetPtList()[i],pTempRoat->GetPtList()[i+1]);
				if(bOnLine)
				{
					double dbTemp = CalcDist(pt.dx,pt.dy,temppt.dx,temppt.dy);
					if(dbTemp  < mindis)
					{
						mindis = dbTemp;
						nMinRoadID = pTempRoat->GetRutID();
						ptRTemp = temppt;
					}
				}
				else
				{
					double dis1 = CalcDist(pt.dx,pt.dy,pTempRoat->GetPtList()[i].dx,pTempRoat->GetPtList()[i].dy);
					double dis2 = CalcDist(pt.dx,pt.dy,pTempRoat->GetPtList()[i+1].dx,pTempRoat->GetPtList()[i+1].dy);

					if( dis1 < dis2 && dis1 < mindis)
					{
						mindis = dis1; 
						ptRTemp = pTempRoat->GetPtList()[i];
						nMinRoadID = pTempRoat->GetRutID();
					}
					if(dis1 > dis2 && dis2 < mindis)
					{
						mindis = dis2;
						ptRTemp = pTempRoat->GetPtList()[i+1];
						nMinRoadID = pTempRoat->GetRutID();
					}
				}
			}
		}
		Rpt = ptRTemp;
		POSITION pos2 = RList.GetHeadPosition();
		while( pos2 )
		{
			pTempRoat = (CGIS_FeatureRut*)RList.GetNext(pos2);
			if(nMinRoadID == pTempRoat->GetRutID())
			{
				if( (*pLastRoat) )
				{
					delete (*pLastRoat);
					(*pLastRoat) = NULL;
				}
				(*pLastRoat) = (CGIS_FeatureRut*)pTempRoat->Clone();
			}
			delete pTempRoat;
			pTempRoat = NULL;
		}
		RList.RemoveAll();
		m_ptLastXy = Rpt;
		if(enState == EN_MATCH_REALNAV)
		{
			if( Routedis > nDisLeave )
				nLeaveCount = 1;
			return EN_MATCH_OUT;
		}
		else
			return EN_MATCH_WELL;
	}
	if(RList.GetCount() == 0)
	{
		Rpt = pt;
		m_ptLastXy = Rpt;
		if( (*pLastRoat) )
		{
			delete (*pLastRoat);
			(*pLastRoat) = NULL;
		}
		return EN_MATCH_NULL;
	}
	m_nCarRunDirect = 0;
	//��·��������
	CPtrList listRoad;
	POSITION pos1 = RList.GetHeadPosition();
	ST_ROAD_CHOOSE* pSaveLastRoad = NULL;
	BOOL bSame = FALSE;
	while( pos1 )
	{
		mindis = 100000;
		POSITION posRoad = pos1;
		pTempRoat = (CGIS_FeatureRut*)RList.GetNext(pos1);
		//�ų�����ͨ�еĵ�·
		if( pTempRoat->GetRutDirection() == 3 )
			continue;

		//������ƥ��ͬһ����·
		if( !bSame && (*pLastRoat) && pTempRoat->GetRutID() == (*pLastRoat)->GetRutID() )
			bSame = TRUE;
//		ST_ROAD_CHOOSE *stRoad = new ST_ROAD_CHOOSE;
		ST_ROAD_CHOOSE *stRoad = &(g_RoadChoose[nChooseCount]);
		nChooseCount++;
		stRoad->pos = posRoad;
		for(int i = 0 ; i < pTempRoat->GetPtNum() - 1 ; i++)
		{
			ST_GEO_PTXY temppt = DisPointToLine(pt,pTempRoat->GetPtList()[i],pTempRoat->GetPtList()[i+1]);
			BOOL bOnLine = PointOnLine(temppt,pTempRoat->GetPtList()[i],pTempRoat->GetPtList()[i+1]);
			if(bOnLine)
			{
				double dbTemp = CalcDist(pt.dx,pt.dy,temppt.dx,temppt.dy);
				if(dbTemp  < mindis)
				{
					mindis = dbTemp;
					ptRTemp = temppt;
				}
			}
			else
			{
				double dis1 = CalcDist(pt.dx,pt.dy,pTempRoat->GetPtList()[i].dx,pTempRoat->GetPtList()[i].dy);
				double dis2 = CalcDist(pt.dx,pt.dy,pTempRoat->GetPtList()[i+1].dx,pTempRoat->GetPtList()[i+1].dy);

				if( dis1 < dis2 && dis1 < mindis)
				{
					mindis = dis1; 
					ptRTemp = pTempRoat->GetPtList()[i];
				}
				if(dis1 > dis2 && dis2 < mindis)
				{
					mindis = dis2;
					ptRTemp = pTempRoat->GetPtList()[i+1];
				}
			}
		}
		//�������
		stRoad->nMin = mindis;
		stRoad->pt = ptRTemp;
		if( bSame && !pSaveLastRoad )
		{
			pSaveLastRoad = stRoad;
			continue;
		}
		POSITION posSort = listRoad.GetHeadPosition();
		if( !posSort )
			listRoad.AddHead( stRoad );
		ST_ROAD_CHOOSE *stTemp = NULL;
		while( posSort )
		{
			POSITION posInsert = posSort;
			stTemp = ( ST_ROAD_CHOOSE* )listRoad.GetNext( posSort );
			if( stRoad->nMin < stTemp->nMin )
			{
				listRoad.InsertBefore( posInsert , stRoad );
				break;
			}
			if( !posSort )
				listRoad.AddTail( stRoad );
		}
	}
	if( pSaveLastRoad )
	{
		if( listRoad.GetCount() == 0  )
			listRoad.AddTail( pSaveLastRoad );
//		else
//		{
//			delete pSaveLastRoad;
//			pSaveLastRoad = NULL;
//		}
	}
	//ѡ������Ķ�������ʻ����һ�µĵ�·
	POSITION posBeChoose = listRoad.GetHeadPosition();
	if( !posBeChoose )
	{
		Rpt = pt;
		m_ptLastXy = Rpt;
		if( (*pLastRoat) )
		{
			delete (*pLastRoat);
			(*pLastRoat) = NULL;
		}
		POSITION posQuery = RList.GetHeadPosition();
		while( posQuery )
		{
			pTempRoat = (CGIS_FeatureRut*)RList.GetNext(posQuery);
			if( pTempRoat )
			{
				delete pTempRoat;
				pTempRoat = NULL;
			}
		}
		RList.RemoveAll();
		return EN_MATCH_NULL;
	}
	ST_ROAD_CHOOSE *stTemp = NULL;
//	ST_GEO_PTXY ptHeadOld = (*pLastRoat)->GetPtList()[0];
//	ST_GEO_PTXY ptTailOld = (*pLastRoat)->GetPtList()[(*pLastRoat)->GetPtNum() - 1];
	//���ǵ���������ƫ�����������ʻ�������ж�ƥ���·
	ST_GEO_PTXY ptLastXy;
	if( enState == EN_MATCH_REALNAV && ptMatchLast.dx != 181 && ptMatchLast.dy != 91 )
		ptLastXy = ptMatchLast;
	else
		ptLastXy = m_ptLastXy;
	while( posBeChoose )
	{
		stTemp = (ST_ROAD_CHOOSE*)listRoad.GetNext( posBeChoose );
		CGIS_FeatureRut *pTempFeaRut = (CGIS_FeatureRut*)RList.GetAt( stTemp->pos );
		//��·��ͨ�з���
		int nDirection = pTempFeaRut->GetRutDirection();
		if(  nDirection == 0 )//˫��
		{
			if( (*pLastRoat) )
				delete (*pLastRoat);
			(*pLastRoat) = (CGIS_FeatureRut*)pTempFeaRut->Clone();
			Rpt = stTemp->pt;
			break;
		}
		else if( nDirection == 1 || nDirection == 2 )//3��ʾ˫�����
		{
			//1��ʾend��start 2��ʾstart��end
			double dbAng = CalcAngle( stTemp->pt.dx , stTemp->pt.dy , ptLastXy.dx , ptLastXy.dy );
			double dbAngRoad = 0;
			if( nDirection == 2 )
				dbAngRoad = CalcAngle( stTemp->pt.dx , stTemp->pt.dy ,pTempFeaRut->GetPtList()[pTempFeaRut->GetPtNum() - 1].dx,pTempFeaRut->GetPtList()[pTempFeaRut->GetPtNum() - 1].dy );
			if( nDirection == 1 )
				dbAngRoad = CalcAngle( stTemp->pt.dx , stTemp->pt.dy ,pTempFeaRut->GetPtList()[0].dx,pTempFeaRut->GetPtList()[0].dy );
			int nAngAbs = abs( dbAng * 180.0 / PI - dbAngRoad * 180.0 / PI );
			if( nAngAbs < 50 || nAngAbs > 310  )
			{
				if( (*pLastRoat) )
					delete (*pLastRoat);
				(*pLastRoat) = (CGIS_FeatureRut*)pTempFeaRut->Clone();
				Rpt = stTemp->pt;
				break;
			}
		}
		if( !posBeChoose )
		{
			stTemp = (ST_ROAD_CHOOSE*)listRoad.GetHead();
			pTempFeaRut = (CGIS_FeatureRut*)RList.GetAt( stTemp->pos );
			if( (*pLastRoat) )
				delete (*pLastRoat);
			(*pLastRoat) = (CGIS_FeatureRut*)pTempFeaRut->Clone();
			Rpt = stTemp->pt;
		}
		
	}
	//�����ڴ�
//	POSITION posClean = listRoad.GetHeadPosition();
//	while( posClean )
//	{
//		stTemp = (ST_ROAD_CHOOSE*)listRoad.GetNext( posClean );
//		delete stTemp;
//		stTemp = NULL;
//	}
	listRoad.RemoveAll();
	POSITION pos2 = RList.GetHeadPosition();
	while( pos2 )
	{
		pTempRoat = (CGIS_FeatureRut*)RList.GetNext(pos2);
		delete pTempRoat;
		pTempRoat = NULL;
	}
	RList.RemoveAll();
	m_ptLastXy = Rpt;
	if(enState == EN_MATCH_REALNAV)
	{
		if( Routedis > nDisLeave )
			nLeaveCount = 1;
		return EN_MATCH_OUT;
	}
	else
		return EN_MATCH_WELL;
}

BOOL CGIS_GeoMap::FullVRutInfo( CGIS_NetworkRut *pRut, ST_VOICE_RUT *pstVoiceRut )
{
	if( pRut == NULL )
		return FALSE;
	//����·������
//	if( pRut->GetType() == 0 || pRut->GetType() == 1 )
	if( pRut->GetType() == 0  )
		pstVoiceRut->byType = 6;
	else
		pstVoiceRut->byType = 8;
	if( pRut->GetRutType() == 3 
        ||
        pRut->GetRutType() == 5 )

		pstVoiceRut->byType = 7;
	else if( pRut->GetRutType() == 15 )
		pstVoiceRut->byType = 33;
	else if( pRut->GetRutType() == 14 )
		pstVoiceRut->byType = 34;
	///·�εĸ�����Ϣ
	if( pRut->GetRutType() == 0 )
		pstVoiceRut->byAdd = 9;
	else
		pstVoiceRut->byAdd = 10;
	///·���Ƿ��ͨ������
	pstVoiceRut->byDrivable = 13;
	///·���Ƿ�Ϊ�շ�·��
	if( pRut->GetRutCharge() == 0 )
		pstVoiceRut->byCharge = 36;
	else
		pstVoiceRut->byCharge = 35;

    return TRUE;
}
//�жϽڵ㴦�����··��/��ڵ�·�ͳ��ڵ�·�Ƿ�Ϊͬһ����·
	//�Ƿ��������ڵ�·ͬ���ĵ�·/�Ƿ��������ڵ�·ͬ���ĵ�·
void CGIS_GeoMap::JudgeNodeRutNameInfo( CGIS_FeatureRut *pPreFRut, CGIS_PathpointNode *pPNode, ST_VOICE_NODE *pVNode )
{
	//·�γ����··��
//	pVNode->byConfusion = 16;
	pVNode->bySameName = 12;
	if( pPreFRut == NULL )
	{
		pVNode->byEnName = 20;
		pVNode->bySameName = 11;
		pVNode->bySameRoad = 32;
		pVNode->byExName = 18;
		return;
	}
	if( pPreFRut->GetAnnotation() && pPNode->GetRutAnno() )
	{
		if( strcmp( pPreFRut->GetAnnotation(), pPNode->GetRutAnno() ) == 0 )
		{
			pVNode->bySameName = 11;
//			pVNode->byConfusion = 15;
		}
	}
	else if( pPreFRut->GetAnnotation().Length()==0 && pPNode->GetRutAnno().Length() == 0 )
		pVNode->bySameName = 11;
	//��ڵ�·�ͳ��ڵ�·�Ƿ�Ϊͬһ����·
	pVNode->bySameRoad = 32;
	if( pPreFRut->GetRutID() == pPNode->GetNextRutID() )
		pVNode->bySameRoad = 31;

	///�Ƿ��������ڵ�·ͬ���ĵ�·
	pVNode->byExName = 18;
	if( pPreFRut->GetAnnotation() )
	{
		POSITION ps = pPNode->GetNodeRutList()->GetHeadPosition();
		CGIS_NetworkNodeRut *pNNRut;
		while( ps )
		{
			pNNRut = (CGIS_NetworkNodeRut*)pPNode->GetNodeRutList()->GetNext(ps);
			if( pNNRut )
			{
				if( pNNRut->GetRutAnno() )
				{
					if( strcmp( pNNRut->GetRutAnno(), pPreFRut->GetAnnotation() ) == 0 )
					{
						pVNode->byExName = 17;
//						pVNode->byConfusion = 15;
						break;
					}
				}
			}
		}
	}
	//�Ƿ��������ڵ�·ͬ���ĵ�·
	pVNode->byEnName = 20;
	if( pPNode->GetRutAnno() )
	{
		POSITION ps = pPNode->GetNodeRutList()->GetHeadPosition();
		CGIS_NetworkNodeRut *pNNRut;
		while( ps )
		{
			pNNRut = (CGIS_NetworkNodeRut*)pPNode->GetNodeRutList()->GetNext(ps);
			if( pNNRut )
			{
				if( pNNRut->GetRutAnno() )
				{
					if( strcmp( pNNRut->GetRutAnno(), pPNode->GetRutAnno() ) == 0 )
					{
						pVNode->byEnName = 19;
//						pVNode->byConfusion = 15;
						break;
					}
				}
			}
		}
	}
}

BOOL CGIS_GeoMap::FullVNodeInfo( 
    CGIS_FeatureRut *pPreFRut, 
    CGIS_PathpointNode *pPNode,
    int nREBID,
    int nEndRutID, 
    ST_VOICE_NODE *pstVoiceNode )
{
/*	BYTE byOtherStreet;	//�Ƿ���Ҫ������ڵ�·���������·��21����22����)
	BYTE byIMI;			//�Ƿ����IMI(23����24����)
//	BYTE byDestination;	//�Ƿ񵼵�Ŀ�ĵأ�25����26����)
	BYTE byRound;		//��ڵ�·����ڵ�·֮���Ƿ��л���(29����30����)
*/
	pstVoiceNode->byDestination = 26;
	if( pPNode->GetNextRutID() == nEndRutID && 
			pPNode->GetPBreadthID() == nREBID )
		pstVoiceNode->byDestination = 25;

	CGIS_NetworkPathInfo::JudgeNodeAngleAndTurn( 
        pPNode->GetTurnAngle(),
		pstVoiceNode->byAngle,
        pstVoiceNode->byTurn );
	JudgeNodeRutNameInfo( pPreFRut, pPNode, pstVoiceNode );
	pstVoiceNode->byOtherStreet = 21;
	pstVoiceNode->byIMI = 24;
	pstVoiceNode->byRound = 30;

	return TRUE;
}
BOOL CGIS_GeoMap::InitVNodeInfo_DEL( ST_VOICE_NODE *pVNode )
{
	pVNode->byAngle         = 1;	//����Ƕ����ͣ�1.ֱ�� 2.����3.����4.����5.U�͹գ�
	pVNode->bySameName      = 11;	//�����··���Ƿ�һ�£�11����12����)
	pVNode->byConfusion     = 16;	//�Ƿ���ڻ���·�ڣ�15����16����)
	pVNode->byConfusion2    = 16;	//�Ƿ���ڻ���·�ڣ�15����16����)
	pVNode->byOtherStreet   = 21;	//�Ƿ���Ҫ������ڵ�·���������·��21����22����)
	pVNode->byIMI           = 24;	//�Ƿ����IMI(23����24����)
	pVNode->byDestination   = 25;	//�Ƿ񵼵�Ŀ�ĵأ�25����26����)
	pVNode->byTurn          = 0;	//���䷽��(27����28����)
	pVNode->byRound         = 30;	//��ڵ�·����ڵ�·֮���Ƿ��л���(29����30����)
	pVNode->bySameRoad      = 31;	//��ڵ�·�ͳ��ڵ�·�Ƿ�Ϊͬһ����·��31����32����)
	pVNode->byExName        = 18;	//�Ƿ��������ڵ�·ͬ���ĵ�·������ڵ�·�⣩(RUT)17�� ��18�� ��
	pVNode->byEnName        = 20;	//�Ƿ��������ڵ�·ͬ���ĵ�·�������ڵ�·�⣩(RUT)19�� ��20�� ��
	return TRUE;
}

const unsigned short MAX_PTNUM_ROUND = 32;

/**
 * @brief ����ӿ��������մ�ȫ��·���д������Ĳ��������·������
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::FullPathNodeRutsV2(
    CGIS_MapBreadthNetwork *pBMNetwork,         //·������
    CGIS_PathpointNode *pVNode,                 //����NODE����
    CGIS_FeatureRut *pFRList[MAX_BRUT_NUM],     //�ռ�·������
    unsigned long nVertexMergeID,
    unsigned long nPreviousRoadSectionMergeID,
    unsigned long nNextRoadSectionMergeID,
    BYTE byRAType )                             //��ǰ��������
{
    CGIS_NetworkVtx *pVtx                   = pBMNetwork->GetNetworkVtx((nVertexMergeID>>15),nVertexMergeID&0x7fff);                                //�ڵ�       
    CGIS_NetworkRut *pRoadSectionPrevious   = pBMNetwork->GetNetworkRut((nPreviousRoadSectionMergeID>>15),nPreviousRoadSectionMergeID&0x7fff);      //ʻ��·��
    CGIS_NetworkRut *pRoadSectionNext       = pBMNetwork->GetNetworkRut((nNextRoadSectionMergeID>>15),nNextRoadSectionMergeID&0x7fff);              //ʻ��·��
    ASSERT(pVtx->GetVtxEdge() != 1);

    
//  �����·��������
    

    //////////////////////////////////////////////////////////////////////////
    //ʹ������ͼ��·������ͼ��ʽ
    CGIS_FeatureRut *pFtrRoadSectionPrevious = pFRList[pRoadSectionPrevious->GetRutID()];
    if(pFtrRoadSectionPrevious)
    {
        CGIS_FeatureRut::LPIMGINFO pImgInfo = 
            pFtrRoadSectionPrevious->GetImgInfoByExitRoadSectionID(pRoadSectionNext->GetRutID());
        if(pImgInfo)
        {
            pVNode->SetCrossStatus(CGIS_PathpointNode::STATUS_IMAGE);
            pVNode->SetCrossImgID(pImgInfo->nImgID);
            pVNode->SetCrossArrowID(pImgInfo->nArrowID); 
            
            return TRUE;
        }
    }


    //////////////////////////////////////////////////////////////////////////
    //ԭ�������·������ͼ�ķ�ʽ
    pVNode->SetCrossStatus(CGIS_PathpointNode::STATUS_GDI_DRAW);
	ST_GEO_PTXY pPtList[MAX_PTNUM_ROUND];
	int nPtNum = 1;
	pPtList[0] = pVNode->GetPathpointPosition();
	pVNode->GetVNodeInfo()->byConfusion = 16;
	pVNode->GetVNodeInfo()->byConfusion2 = 16;
	if( byRAType == 1 )
		return FullPathNodeRutsSVtx ( pBMNetwork, pVNode, pFRList, pVtx, pRoadSectionPrevious, pRoadSectionNext, pPtList, nPtNum );
	else if( byRAType == 2 )
		return FullPathNodeRutsAss  ( pBMNetwork, pVNode, pFRList, pVtx, pRoadSectionPrevious, pRoadSectionNext, pPtList, nPtNum );
	else if( byRAType == 3 )
		return FullPathNodeRutsRound( pBMNetwork, pVNode, pFRList, pVtx, pRoadSectionPrevious, pRoadSectionNext, pPtList, nPtNum );
	return FALSE;
}





BOOL CGIS_GeoMap::FullPathNodeRuts(
    CGIS_MapBreadthNetwork *pBMNetwork,         //·������
    CGIS_PathpointNode *pVNode,                 //����NODE����
    CGIS_FeatureRut *pFRList[MAX_BRUT_NUM],     //�ռ�·������
    CGIS_NetworkVtx *pVtx,                      //�ڵ�
    CGIS_NetworkRut *pRoadSectionPrevious,      //ʻ��·��
    CGIS_NetworkRut *pRoadSectionNext,          //ʻ��·��
    BYTE byRAType )                             //��ǰ��������
{

    //////////////////////////////////////////////////////////////////////////
    //ʹ������ͼ��·������ͼ��ʽ
    CGIS_FeatureRut *pFtrRoadSectionPrevious = pFRList[pRoadSectionPrevious->GetRutID()];
    if(pFtrRoadSectionPrevious)
    {
        CGIS_FeatureRut::LPIMGINFO pImgInfo = 
            pFtrRoadSectionPrevious->GetImgInfoByExitRoadSectionID(pRoadSectionNext->GetRutID());
        if(pImgInfo)
        {
            pVNode->SetCrossStatus(CGIS_PathpointNode::STATUS_IMAGE);
            pVNode->SetCrossImgID(pImgInfo->nImgID);
            pVNode->SetCrossArrowID(pImgInfo->nArrowID); 
            
            return TRUE;
        }
    }


    //////////////////////////////////////////////////////////////////////////
    //ԭ�������·������ͼ�ķ�ʽ
    pVNode->SetCrossStatus(CGIS_PathpointNode::STATUS_GDI_DRAW);
	ST_GEO_PTXY pPtList[MAX_PTNUM_ROUND];
	int nPtNum = 1;
	pPtList[0] = pVNode->GetPathpointPosition();
	pVNode->GetVNodeInfo()->byConfusion = 16;
	pVNode->GetVNodeInfo()->byConfusion2 = 16;
	if( byRAType == 1 )
		return FullPathNodeRutsSVtx ( pBMNetwork, pVNode, pFRList, pVtx, pRoadSectionPrevious, pRoadSectionNext, pPtList, nPtNum );
	else if( byRAType == 2 )
		return FullPathNodeRutsAss  ( pBMNetwork, pVNode, pFRList, pVtx, pRoadSectionPrevious, pRoadSectionNext, pPtList, nPtNum );
	else if( byRAType == 3 )
		return FullPathNodeRutsRound( pBMNetwork, pVNode, pFRList, pVtx, pRoadSectionPrevious, pRoadSectionNext, pPtList, nPtNum );
	return FALSE;
}

/**
 * @brief �����ͨ·��
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::FullPathNodeRutsSVtx( 
    CGIS_MapBreadthNetwork *pBMNetwork, 
    CGIS_PathpointNode *pVNode,
    CGIS_FeatureRut *pFRList[MAX_BRUT_NUM],
    CGIS_NetworkVtx *pVtx, 
    CGIS_NetworkRut *pRoadSectionPrevious, 
    CGIS_NetworkRut *pRoadSectionNext,
    ST_GEO_PTXY pPtList[], int &nPtNum )
{
    JudgePathNodeRutsConfuse(pBMNetwork,pVNode,pFRList,pVtx,pRoadSectionPrevious,pRoadSectionNext);


	pVNode->CreateCRect( pPtList, nPtNum );
    

	swString strRut = _T("");

	CGIS_FeatureRut *pFRut = pFRList[pRoadSectionNext->GetRutID()];
	if( pFRut )
	{
		if( pFRut->GetAnnotation() )
			CharToUnicode( pFRut->GetAnnotation() , &strRut );
//			strRut = pFRut->GetAnnotation();
	}

    
	CGIS_NetworkRut *pRut = NULL;

	for( int i=0;	i<pVtx->GetVRutNum();	i++ )
	{
        //////////////////////////////////////////////////////////////////////////
        //070606 add for ͨ�б�־
        BOOL bPassSign = FALSE;
        pRut = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
        if(!pRut)
        {
            CGIS_BreadthMap* pBreadthMap = GetBreadthMapByID2(pVtx->GetVBreadthID());
            ASSERT(pBreadthMap);
            int nReadErrorCode = 0;
            if(!pBMNetwork->ReadBreadthNetworkRoadSectionByIDArray(&pBreadthMap,pVtx->GetVRutNum(),pVtx->GetVRutList(),nReadErrorCode))
            {
                //ASSERT(FALSE);
                TRACE1("ReadBreadthNetworkRoadSectionByIDArray failed,error code(%d)\n",nReadErrorCode);
                pRut = NULL;
            }
            else
            {
                pRut = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
            }

        }
                           
        if( pRut )
        {
            bPassSign = pBMNetwork->CouldReachAnotherVertexInRoadSection(pVtx, pRut);
        }
        else
        {
            //070607�޸�,��ʾ��δ���ص�����ʱ��Ϊ��ͨ��
            bPassSign = TRUE;
        }

        //////////////////////////////////////////////////////////////////////////

 		if( pVtx->GetVRutNum() == 3 && m_bTrunFlag == FALSE )
		{
			if( !(pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID() || pVtx->GetVRutList()[i] == pRoadSectionNext->GetRutID()) )
			{
				if( bPassSign)
					m_bTrunFlag = TRUE;	
			}
		}

        //////////////////////////////////////////////////////////////////////////
        //�Ѻ�����ڵ�������·�εĿռ����ݶ�ȡ����.
		pFRut = pFRList[pVtx->GetVRutList()[i]];
		if( pFRut )
		{           
			CGIS_NetworkNodeRut *pNRut = new CGIS_NetworkNodeRut;
			if( pFRut->GetAnnotation() )
			{
				char *szAnno = new char[pFRut->GetAnnoNum()];
				memcpy( szAnno, pFRut->GetAnnotation(), pFRut->GetAnnoNum() );
				pNRut->SetRutAnno( szAnno );
				pNRut->SetRutAnnoNum( pFRut->GetAnnoNum() );

			}
			
			// Ϊ����ǿ�Ķ��ԣ������������ò�Ҫ����ͬ������
			// ����Ա������и�������
			// ԭ����ST_GEO_PTXY *pPtList �ָ�����pPtRutList
			ST_GEO_PTXY *pPtRutList = new ST_GEO_PTXY[pFRut->GetPtNum()];
			memcpy( pPtRutList, pFRut->GetPtList(), sizeof(ST_GEO_PTXY)*pFRut->GetPtNum() );
			pNRut->SetRutPtList( pPtRutList );
			pNRut->SetRutPtNum( pFRut->GetPtNum() );
			pNRut->SetRutID( pFRut->GetRutID() );
			pVNode->GetNodeRutList()->AddTail( pNRut );
			if( 
                IsPtInRect(
                    pFRut->GetPtList()[0].dx,
                    pFRut->GetPtList()[0].dy,
                    pVNode->GetCRect().dLeft,
					pVNode->GetCRect().dTop,
                    pVNode->GetCRect().dRight,
                    pVNode->GetCRect().dBottom
                    ) 
                && 
				IsPtInRect(
                    pFRut->GetPtList()[pFRut->GetPtNum()-1].dx,
                    pFRut->GetPtList()[pFRut->GetPtNum()-1].dy,
					pVNode->GetCRect().dLeft,
                    pVNode->GetCRect().dTop,
                    pVNode->GetCRect().dRight,
                    pVNode->GetCRect().dBottom
                    ) 
               )
			{
				CGIS_NetworkRut *pNRut1 = NULL;
				CGIS_NetworkVtx *pNVtx = NULL;
				pNRut1 = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pFRut->GetRutID() );
				if( pNRut1 )
				{
					if( pVtx->GetVtxID() == pNRut1->GetRutStartVtxID() )
						pNVtx = pBMNetwork->GetNetworkVtx( pVtx->GetVBreadthID(), pNRut1->GetRutEndVtxID() );
					else
						pNVtx = pBMNetwork->GetNetworkVtx( pVtx->GetVBreadthID(), pNRut1->GetRutEndVtxID() );
					if( pNVtx )
					{
						for( int j=0;	j<pNVtx->GetVRutNum();	j++ )
						{
							pFRut = pFRList[pNVtx->GetVRutList()[j]];
							if( pFRut )
							{
								if( pFRut->GetRutID() == pNRut1->GetRutID() )
									continue;
								pNRut = new CGIS_NetworkNodeRut;
								if( pFRut->GetAnnotation() )
								{
									char *szAnno = new char[pFRut->GetAnnoNum()];
									memcpy( szAnno, pFRut->GetAnnotation(), pFRut->GetAnnoNum() );
									pNRut->SetRutAnno( szAnno );
									pNRut->SetRutAnnoNum( pFRut->GetAnnoNum() );
								}

								// Ϊ����ǿ�Ķ��ԣ������������ò�Ҫ����ͬ������
								// ����Ա������и�������
								// ԭ����ST_GEO_PTXY *pPtList �ָ�����pPtRutInList

								ST_GEO_PTXY *pPtRutInList = new ST_GEO_PTXY[pFRut->GetPtNum()];
								memcpy( pPtRutInList, pFRut->GetPtList(), sizeof(ST_GEO_PTXY)*pFRut->GetPtNum() );
								pNRut->SetRutPtList( pPtRutInList );
								pNRut->SetRutPtNum( pFRut->GetPtNum() );
								pNRut->SetRutID( pFRut->GetRutID() );
								pVNode->GetNodeRutList()->AddTail( pNRut );
							}
						}
					}
				}
			}
		}
	}

	return TRUE;
}

/**
 * @brief ���ʮ��·��
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::FullPathNodeRutsAss( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRoadSectionPrevious, CGIS_NetworkRut *pRoadSectionNext, ST_GEO_PTXY pPtList[], int &nPtNum )
{
    JudgePathNodeRutsConfuse(pBMNetwork,pVNode,pFRList,pVtx,pRoadSectionPrevious,pRoadSectionNext);

    m_bTrunFlag = TRUE;
	BOOL bReturn = GetNextAssRut( pBMNetwork, pVNode, pFRList, pVtx, pRoadSectionPrevious, pPtList, nPtNum );
	pVNode->CreateCRect( pPtList, nPtNum );
	return bReturn;
}
/**
 * @brief 
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::GetNextAssRut( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], 
			CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRoadSectionPrevious, ST_GEO_PTXY pPtList[], int &nPtNum )
{
	CGIS_FeatureRut *pFRut = NULL;
	CGIS_NetworkVtx *pVNext = NULL;
	CGIS_NetworkRut *pRoadSectionNext = NULL;
	for( int i=0;	i<pVtx->GetVRutNum();	i++ )
	{
//		if( pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID() )
//			continue;
		pFRut = pFRList[pVtx->GetVRutList()[i]];
		if( pFRut )
		{
			CGIS_NetworkNodeRut *pNRut = new CGIS_NetworkNodeRut;
			if( pFRut->GetAnnotation() )
			{
				char *szAnno = new char[pFRut->GetAnnoNum()];
				memcpy( szAnno, pFRut->GetAnnotation(), pFRut->GetAnnoNum() );
				pNRut->SetRutAnno( szAnno );
				pNRut->SetRutAnnoNum( pFRut->GetAnnoNum() );
			}
			
			// Ϊ����ǿ�Ķ��ԣ������������ò�Ҫ����ͬ������
			// ����Ա������и�������
			// ԭ����ST_GEO_PTXY *pPtList �ָ�����pPtRutList
			ST_GEO_PTXY *pPtRutList = new ST_GEO_PTXY[pFRut->GetPtNum()];
			memcpy( pPtRutList, pFRut->GetPtList(), sizeof(ST_GEO_PTXY)*pFRut->GetPtNum() );
			pNRut->SetRutPtList( pPtRutList );
			pNRut->SetRutPtNum( pFRut->GetPtNum() );
			pNRut->SetRutID( pFRut->GetRutID() );
			pVNode->GetNodeRutList()->AddTail( pNRut );
		}
		if( pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID() )
			continue;
		pRoadSectionNext = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
		if( pRoadSectionNext )
		{
			if( pRoadSectionNext->GetRutType() == 4 )
			{
				if( pVtx->GetVtxID() == pRoadSectionNext->GetRutStartVtxID() )
					pVNext = pBMNetwork->GetNetworkVtx( pVtx->GetVBreadthID(), pRoadSectionNext->GetRutEndVtxID() );
				else
					pVNext = pBMNetwork->GetNetworkVtx( pVtx->GetVBreadthID(), pRoadSectionNext->GetRutStartVtxID() );
				if( pVNext )
				{
					pVtx->GetVtxSInfo()->byStatusOfNormalSearch = 3;
					if( pVNext->GetVtxSInfo()->byStatusOfNormalSearch != 3 )
					{
						pPtList[nPtNum] = pVNext->GetVtxPosition();
						nPtNum++;
						GetNextAssRut( pBMNetwork, pVNode, pFRList, pVNext, pRoadSectionNext, pPtList, nPtNum );
					}
				}
			}
		}
	}
	return TRUE;
}

/**
 * @brief ��价��·��NODE
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::FullPathNodeRutsRound( 
    CGIS_MapBreadthNetwork *pBMNetwork,
    CGIS_PathpointNode *pVNode,
    CGIS_FeatureRut *pFRList[MAX_BRUT_NUM],
    CGIS_NetworkVtx *pVtx,
    CGIS_NetworkRut *pRoadSectionPrevious, 
    CGIS_NetworkRut *pRoadSectionNext,
    ST_GEO_PTXY pPtList[], 
    int &nPtNum )
{
	CGIS_NetworkRut *pNRStart = NULL;

    //-------------------------------------------------------------
    //��ʼ·�κ���ֹ·�α���һ��Ϊ����·��
	if( pRoadSectionPrevious->GetRutType() == 0 )
		pNRStart = pRoadSectionPrevious;
	else if( pRoadSectionNext->GetRutType() == 0 )
		pNRStart = pRoadSectionNext;
	else
		return FALSE;
    //-------------------------------------------------------------


	CGIS_NetworkRut *pNRNext = NULL;
	CGIS_NetworkRut *pNRut = NULL;
	CGIS_FeatureRut *pFRut = NULL;


	for( int i=0;	i<pVtx->GetVRutNum();	i++ )
	{
        //-------------------------------------------------------------
        //�ҳ���һ������·��
		if( pVtx->GetVRutList()[i] != pNRStart->GetRutID() )
		{
			pNRut = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
			if( pNRut )
			{
				if( pNRut->GetRutType() == 0 )
					pNRNext = pNRut;
			}
		}
        //-------------------------------------------------------------
        //ȡ���ռ�·��
		pFRut = pFRList[pVtx->GetVRutList()[i]];
		if( pFRut )
		{
			CGIS_NetworkNodeRut *pNNRut = new CGIS_NetworkNodeRut;
			if( pFRut->GetAnnotation() )
			{
				char *szAnno = new char[pFRut->GetAnnoNum()];
				memcpy( szAnno, pFRut->GetAnnotation(), pFRut->GetAnnoNum() );
				pNNRut->SetRutAnno( szAnno );
				pNNRut->SetRutAnnoNum( pFRut->GetAnnoNum() );
			}

			// Ϊ����ǿ�Ķ��ԣ������������ò�Ҫ����ͬ������
			// ����Ա������и�������
			// ԭ����ST_GEO_PTXY *pPtList �ָ�����pPtRutList
			ST_GEO_PTXY *pPtRutList = new ST_GEO_PTXY[pFRut->GetPtNum()];
			memcpy( pPtRutList, pFRut->GetPtList(), sizeof(ST_GEO_PTXY)*pFRut->GetPtNum() );
			pNNRut->SetRutPtList( pPtRutList );
			pNNRut->SetRutPtNum( pFRut->GetPtNum() );
			pNNRut->SetRutID( pFRut->GetRutID() );
			pVNode->GetNodeRutList()->AddTail( pNNRut );
		}
	}

	if( pNRNext == NULL )
	{
		pVNode->CreateCRect( pPtList, nPtNum );
		return FALSE;
	}

    //////////////////////////////////////////////////////////////////////////
    

    //ȡ������һ�˵Ľڵ�
	if( pVtx->GetVtxID() == pNRNext->GetRutStartVtxID() )
    {
        pVtx = pBMNetwork->GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutEndVtxID() );
    }
	else
    {
        pVtx = pBMNetwork->GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutStartVtxID() );
    }
    //////////////////////////////////////////////////////////////////////////
	if( pVtx )
	{
        //������ڵ��λ����䵽����������ȥ
		pPtList[nPtNum] = pVtx->GetVtxPosition();
		nPtNum++;
        BOOL bReturn = FALSE; 
        {
            TRACK_FUNCTION("GetNextRoundRut()");
		    bReturn= GetNextRoundRut( pBMNetwork, pVNode, pFRList, pVtx, pNRStart, pNRNext, pPtList, nPtNum );

        }
		pVNode->CreateCRect( pPtList, nPtNum );//����·������ͼ�ľ���
		return bReturn;
	}
    
	pVNode->CreateCRect( pPtList, nPtNum );
	return FALSE;
}
/**
 * @brief �����һ������·��
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::GetNextRoundRut(
    CGIS_MapBreadthNetwork *pBMNetwork, 
    CGIS_PathpointNode *pVNode, 
    CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], 
	CGIS_NetworkVtx *pVtx, 
    CGIS_NetworkRut *pRStart,
    CGIS_NetworkRut *pRoadSectionNext, 
    ST_GEO_PTXY pPtList[],
    int &nPtNum )
{
    //·�κ���ֹ·��һ�±����˻����ѱ�����
	if( pRStart == pRoadSectionNext )
		return TRUE;

	CGIS_NetworkRut *pNRNext = NULL;
	CGIS_NetworkRut *pNRut = NULL;
	CGIS_FeatureRut *pFRut = NULL;
	CGIS_BreadthMap *pBMNet = NULL;
	CGIS_MapLayer **pLList = NULL;
	CGIS_FeatureRut **pFRuts = pFRList;


	if( pVtx->GetVtxEdge() == 1 )
	{
		if( pVtx->GetVBreadthID() != m_pBMNet->m_nBMID )
		{

			pBMNet = GetBreadthMapByID( pVtx->GetVAbutBID() );
            ASSERT(pBMNet);
			if( pBMNet == NULL )
				return FALSE;
			pLList = (CGIS_MapLayer**)malloc(sizeof(CGIS_MapLayer*)*MAX_RLAY_NUM);
			memcpy( pLList, m_pLList, sizeof(CGIS_MapLayer*)*MAX_RLAY_NUM );
			pFRuts = (CGIS_FeatureRut**)malloc(sizeof(CGIS_FeatureRut*)*MAX_BRUT_NUM);
			memset( pFRuts, 0, sizeof(CGIS_FeatureRut*)*MAX_BRUT_NUM );
			CGIS_FeatureRut *pRut = NULL;

            //////////////////////////////////////////////////////////////////////////
            unsigned int nErrorCode = 0;
			LoadBLData( pBMNet, pLList, m_nFRLNum ,&nErrorCode);
            if(nErrorCode == 2)
            {
                TRACE0("GetNextRoundRut faild!\n");
                THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_FULL_PATH_LEG_DATA,CGIS_NetworkException::ERROR_OUT_OF_MEMORY));
            }
            //////////////////////////////////////////////////////////////////////////
			POSITION ps = NULL;
			for( int i=0;	i<m_nFRLNum;	i++ )
			{
				CPtrList *pRList = NULL;
// 				pLList[i]->IncreaseLCount( pBMNet->m_nBreadthID );
				pLList[i]->GetObjectListMap()->Lookup( pBMNet->m_nBreadthID, pRList );
				if( pRList )
				{
					ps = pRList->GetHeadPosition();
					while( ps )
					{
						pRut = (CGIS_FeatureRut*)pRList->GetNext(ps);
						if( pRut )
							pFRuts[pRut->GetRutID()] = pRut;
					}
				}
			}
            int nReadErrorCode = 0;
            if(!pBMNetwork->ReadBreadthNetwork( &pBMNet, 1, nReadErrorCode))
            {
                TRACE0("GetNextRoundRut faild!\n");
                THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_FULL_PATH_LEG_DATA,(CGIS_NetworkException::enErrorCode)nReadErrorCode));
            }
            
		}
		else
			pFRuts = m_pFRList;
		pFRut = pFRuts[pVtx->GetVAbutRID()];
		if( pFRut )
		{
			CGIS_NetworkNodeRut *pNNRut = new CGIS_NetworkNodeRut;
			if( pFRut->GetAnnotation() )
			{
				char *szAnno = new char[pFRut->GetAnnoNum()];
				memcpy( szAnno, pFRut->GetAnnotation(), pFRut->GetAnnoNum() );
				pNNRut->SetRutAnno( szAnno );
				pNNRut->SetRutAnnoNum( pFRut->GetAnnoNum() );
			}

			// Ϊ����ǿ�Ķ��ԣ������������ò�Ҫ����ͬ������
			// ����Ա������и�������
			// ԭ����ST_GEO_PTXY *pPtList �ָ�����pPtRutList
			ST_GEO_PTXY *pPtRutList = new ST_GEO_PTXY[pFRut->GetPtNum()];
			memcpy( pPtRutList, pFRut->GetPtList(), sizeof(ST_GEO_PTXY)*pFRut->GetPtNum() );
			pNNRut->SetRutPtList( pPtRutList );
			pNNRut->SetRutPtNum( pFRut->GetPtNum() );
			pNNRut->SetRutID( pFRut->GetRutID() );
			pVNode->GetNodeRutList()->AddTail( pNNRut );
		}
		if( pBMNet )
			pNRNext = pBMNetwork->GetNetworkRut( pBMNet->m_nBreadthID, pVtx->GetVAbutRID() );
		else
			pNRNext = pBMNetwork->GetNetworkRut( m_pBMNet->m_nBreadthID, pVtx->GetVAbutRID() );
	}
	else
	{
		for( int i=0;	i<pVtx->GetVRutNum();	i++ )
		{
			if( pVtx->GetVRutList()[i] == pRoadSectionNext->GetRutID() )
				continue;
			pNRut = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
			if( pNRut )
			{
				if( pNRut->GetRutType() == 0 )
					pNRNext = pNRut;
			}
			pFRut = pFRuts[pVtx->GetVRutList()[i]];
			if( pFRut )
			{
				CGIS_NetworkNodeRut *pNNRut = new CGIS_NetworkNodeRut;
				if( pFRut->GetAnnotation() )
				{
					char *szAnno = new char[pFRut->GetAnnoNum()];
					memcpy( szAnno, pFRut->GetAnnotation(), pFRut->GetAnnoNum() );
					pNNRut->SetRutAnno( szAnno );
					pNNRut->SetRutAnnoNum( pFRut->GetAnnoNum() );
				}

				// Ϊ����ǿ�Ķ��ԣ������������ò�Ҫ����ͬ������
				// ����Ա������и�������
				// ԭ����ST_GEO_PTXY *pPtList �ָ�����pPtRutInList
				ST_GEO_PTXY *pPtRutInList = new ST_GEO_PTXY[pFRut->GetPtNum()];
				memcpy( pPtRutInList, pFRut->GetPtList(), sizeof(ST_GEO_PTXY)*pFRut->GetPtNum() );
				pNNRut->SetRutPtList( pPtRutInList );
				pNNRut->SetRutPtNum( pFRut->GetPtNum() );
				pNNRut->SetRutID( pFRut->GetRutID() );
				pVNode->GetNodeRutList()->AddTail( pNNRut );
			}
		}
	}
	if( pNRNext == NULL )
	{
		if( pBMNet )
		{
			CGIS_MapLayer *pLayer = NULL;
			for( int i=0;	i<MAX_RLAY_NUM;	i++ )
			{
				pLayer = pLList[i];
				if( pLayer )
					pLayer->DecreaseLCount( pBMNet->m_nBreadthID );
				pLList[i] = NULL;
			}
		}
		return FALSE;
	}
	if( pVtx->GetVtxEdge() == 1 && pBMNet)
	{
		CGIS_NetworkVtx *pVTemp = NULL;
		CGIS_NetworkRut *pNERut = NULL;
		CGIS_BreadthMap *pBMap = NULL;
		
		// ���������Ѿ���pBMNet�����жϣ�����������жϾ�û������
//		if( pBMNet )
//			pBMap = pBMNet;
//		else
//			pBMap = m_pBMNet;
		// ֻѡ������Ψһ�����
		pBMap = pBMNet;

		pNERut = pBMNetwork->GetNetworkRut( pBMap->m_nBreadthID, pVtx->GetVAbutRID() );
		if( pNERut )
		{
			CGIS_NetworkVtx *pSVtx = pBMNetwork->GetNetworkVtx( pBMap->m_nBreadthID, pNERut->GetRutStartVtxID() );
			CGIS_NetworkVtx *pEVtx = pBMNetwork->GetNetworkVtx( pBMap->m_nBreadthID, pNERut->GetRutEndVtxID() );
			if( pSVtx && pEVtx )
			{
				if( pSVtx->GetVtxEdge() == 1 && pEVtx->GetVtxEdge() == 1 )
				{
					CGIS_BreadthMap *pBTemp = GetBreadthMapByID2( pVtx->GetVBreadthID() );
					if( pBTemp )
					{
						if( pBTemp->m_nBMID == pSVtx->GetVAbutBID() && pVtx->GetVRutList()[0] == pSVtx->GetVAbutRID() )
							pVTemp = pSVtx;
						else
							pVTemp = pEVtx;
					}
					else
						ASSERT( FALSE );
				}
				else if( pSVtx->GetVtxEdge() == 1 )
					pVTemp = pSVtx;
				else if( pEVtx->GetVtxEdge() == 1 )
					pVTemp = pEVtx;
			}
		}
		if( pVTemp )
		{
			if( pVTemp->GetVtxID() == pNRNext->GetRutStartVtxID() )
				pVtx = pBMNetwork->GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutEndVtxID() );
			else
				pVtx = pBMNetwork->GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutStartVtxID() );
		}
		else
			pVtx = NULL;
	}
	else
	{
		if( pVtx->GetVtxID() == pNRNext->GetRutStartVtxID() )
			pVtx = pBMNetwork->GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutEndVtxID() );
		else
			pVtx = pBMNetwork->GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutStartVtxID() );
	}
	if( pVtx == NULL )
		return FALSE;
	pPtList[nPtNum] = pVtx->GetVtxPosition();
	nPtNum++;
	if( nPtNum >= MAX_PTNUM_ROUND )
		return FALSE;
	GetNextRoundRut( pBMNetwork, pVNode, pFRuts, pVtx, pRStart, pNRNext, pPtList, nPtNum );
	if( pBMNet )
	{
		CGIS_MapLayer *pLayer = NULL;
		for( int i=0;	i<MAX_RLAY_NUM;	i++ )
		{
			pLayer = pLList[i];
			if( pLayer )
				pLayer->DecreaseLCount( pBMNet->m_nBreadthID );
			pLList[i] = NULL;
		}
	}
	return TRUE;
}
ST_GEO_PTXY CGIS_GeoMap::GetLastPtOnRoad()
{
	return m_ptLastXy;
}
int CGIS_GeoMap::GetCarDirect()
{
	return m_nCarRunDirect;
}
short CGIS_GeoMap::GetNextRutPt()
{
	return m_nNextRutPt;
}
void CGIS_GeoMap::ResetMapState()
{
	m_nMatchBID = -1;
}

/**
 * @brief ����·�����ݽ��д���,���ټ��ؿռ�����
 *
 * @param 
 * @return
*/
BOOL CGIS_GeoMap::JudgePathNodeRutsConfuse( 
    CGIS_MapBreadthNetwork *pBMNetwork, 
    CGIS_PathpointNode *pVNode,
    CGIS_FeatureRut *pFRList[MAX_BRUT_NUM],
    CGIS_NetworkVtx *pVtx, 
    CGIS_NetworkRut *pRoadSectionPrevious, 
    CGIS_NetworkRut *pRoadSectionNext
    )
{
	int nAngleEnter = 0;
	int nAngleExit = 0;
	int nAngleOther[10];//�����·��֮�������·�εĽǶ�.
	int nCountOfAngleOther = 0;
	memset( nAngleOther, 0, sizeof(nAngleOther) );
	swString strRut = _T("");
	m_bTrunFlag = FALSE;


 	CGIS_NetworkRut *pRut = NULL;
    
	if( 
        pVtx->GetVRutNum() != 3 
        || 
        pRoadSectionPrevious->GetRutLength() >= 80.1 
        )
		m_bTrunFlag = TRUE;

	int i;    
	for( i=0;	i<pVtx->GetVRutNum();	i++ )
	{
        //////////////////////////////////////////////////////////////////////////
        //070606 add for ͨ�б�־
        BOOL bPassSign = FALSE;
        pRut = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
        if(!pRut)
        {
            CGIS_BreadthMap* pBreadthMap = GetBreadthMapByID2(pVtx->GetVBreadthID());
            ASSERT(pBreadthMap);
            int nReadErrorCode = 0;
            if(!pBMNetwork->ReadBreadthNetworkRoadSectionByIDArray(&pBreadthMap,pVtx->GetVRutNum(),pVtx->GetVRutList(),nReadErrorCode))
            {
                //ASSERT(FALSE);
                TRACE1("ReadBreadthNetworkRoadSectionByIDArray failed,error code(%d)\n",nReadErrorCode);
                pRut = NULL;
            }
            else
            {
                pRut = pBMNetwork->GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
            }

        }
        //////////////////////////////////////////////////////////////////////////
                           
        if( pRut )
        {
            bPassSign = pBMNetwork->CouldReachAnotherVertexInRoadSection(pVtx, pRut);
        }
        else
        {
            //070607�޸�,��ʾ��δ���ص�����ʱ��Ϊ��ͨ��
            bPassSign = TRUE;
        }

        //////////////////////////////////////////////////////////////////////////

 		if( pVtx->GetVRutNum() == 3 && m_bTrunFlag == FALSE )
		{
			if( !(pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID() || pVtx->GetVRutList()[i] == pRoadSectionNext->GetRutID()) )
			{
				if( bPassSign)
					m_bTrunFlag = TRUE;	
			}
		}
		if( pRut )
		{
			if( pRut->GetRutID() == pRoadSectionNext->GetRutID() )
			{
				nAngleExit = pVtx->GetVRTurnList()[i];
			}
			else if(pRut->GetRutID() == pRoadSectionPrevious->GetRutID())
			{
				nAngleEnter = pVtx->GetVRTurnList()[i];
			}
			else 
			{

                //////////////////////////////////////////////////////////////////////////
                if(bPassSign)
                {
                    if(pRut->GetRutType() != 4)
                    {
                        nAngleOther[nCountOfAngleOther] = pVtx->GetVRTurnList()[i];
                        nCountOfAngleOther += 1;
                    }
                }
                //////////////////////////////////////////////////////////////////////////
			}           							
		}
	}
	
	int nAngleRightNeighbour = -1;
	int nAngleLeftNeighbour = -1;
    pVNode->GetVNodeInfo()->byConfusion2 = 16;
	for( i=0;	i < nCountOfAngleOther;	i++ )
	{
		if( abs(nAngleExit-nAngleOther[i])<=50 )
		{
			if( nAngleExit > nAngleOther[i] )
			{
				if( pVNode->GetVNodeInfo()->byConfusion2 == 37 )
				{
					pVNode->GetVNodeInfo()->byConfusion2 = 38;
					nAngleRightNeighbour = nAngleOther[i];

					break;
				}
				else
				{
					pVNode->GetVNodeInfo()->byConfusion2 = 15;
					nAngleRightNeighbour = nAngleOther[i];
				}
			}
			else
			{
				if( pVNode->GetVNodeInfo()->byConfusion2 == 15 )
				{
					pVNode->GetVNodeInfo()->byConfusion2 = 38;
					nAngleLeftNeighbour = nAngleOther[i];
					break;
				}
				else
				{
					pVNode->GetVNodeInfo()->byConfusion2 = 37;
					nAngleLeftNeighbour = nAngleOther[i];

				}
			}
		}
		else if( 360-abs(nAngleExit-nAngleOther[i])<=50 )
		{
			if( nAngleExit < nAngleOther[i] )
			{
				if( pVNode->GetVNodeInfo()->byConfusion2 == 37 )
				{
					pVNode->GetVNodeInfo()->byConfusion2 = 38;
					nAngleRightNeighbour = nAngleOther[i];
					break;
				}
				else
				{
					pVNode->GetVNodeInfo()->byConfusion2 = 15;
					nAngleRightNeighbour = nAngleOther[i];

				}
			}
			else
			{
				if( pVNode->GetVNodeInfo()->byConfusion2 == 15 )
				{
					pVNode->GetVNodeInfo()->byConfusion2 = 38;
                    nAngleLeftNeighbour = nAngleOther[i];
					break;
				}
				else
                {
                    pVNode->GetVNodeInfo()->byConfusion2 = 37;
                    nAngleLeftNeighbour = nAngleOther[i];

                }
			}
		}
	}
	
#if 1

#ifdef _DEBUG
        if(
            pRoadSectionPrevious->GetRutID()        == pRoadSectionNext->GetRutID()
            && 
            pRoadSectionPrevious->GetRBreadthID()   == pRoadSectionNext->GetRBreadthID()
            )
        {
            TRACE0("�����ʱ�����ظ�·��\n");
            //ASSERT(FALSE);
        }
#endif
    //////////////////////////////////////////////////////////////////////////
    //20070704������BUG5�Ľ������
    //start here
    {       
        if(pRoadSectionPrevious->GetRutID() != pRoadSectionNext->GetRutID() || pRoadSectionPrevious->GetRBreadthID() != pRoadSectionNext->GetRBreadthID())
        {
            if(pVtx->GetVRutNum() == 3 && pVNode->GetVNodeInfo()->byConfusion2 == 15)
            {
                BOOL bSameRutType = FALSE;    
                if(pRoadSectionNext->GetRutType() == pRoadSectionPrevious->GetRutType())
                {
                    if( pRoadSectionNext->GetRutType() == 2)
                        bSameRutType = TRUE;
                }
                
                BOOL bSameName = TRUE;//Ĭ����ͬ����
                //˵��,��δ����������ж������Ƿ���ͬ��,�����޸Ĺ���,��ʱ����
//                {
//                    CGIS_FeatureRut* pFtrRoadSectionPrevious = pFRList[pRoadSectionPrevious->GetRutID()];
//                    CGIS_FeatureRut* pFtrRoadSectionNext    = pFRList[pRoadSectionNext->GetRutID()];
//                    
//                    swString strPre = _T("");
//                    swString strNext = _T("");//�����ȡ����Ҳ��ͬ����
//                    if(pFtrRoadSectionPrevious )
//                    { 
//                        if( pFtrRoadSectionPrevious->GetAnnotation() )
//                            CharToUnicode( pFtrRoadSectionPrevious->GetAnnotation() , &strPre );
//                    }
//                    if( pFtrRoadSectionNext )
//                    {
//                        if( pFtrRoadSectionNext->GetAnnotation() )
//                            CharToUnicode( pFtrRoadSectionNext->GetAnnotation() , &strNext );
//                    }
//                    if(strPre.Compare(strNext) != 0)
//                    {
//                        bSameName = FALSE;
//                    }
//                }
            
                
                short nAngleEnter   = -1;
                short nAngleExit    = -1;
                CGIS_NetworkRut* pRoadSectionBranch = NULL;
                for(int i = 0; i < 3; i++)
                {
                    if(pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID())
                    {
                        nAngleEnter = pVtx->GetVRTurnList()[i];
                        continue;
                    }
                    
                    if(pVtx->GetVRutList()[i] == pRoadSectionNext->GetRutID())
                    {
                        nAngleExit = pVtx->GetVRTurnList()[i];
                        continue;
                    }    
                    
                    pRoadSectionBranch = pBMNetwork->GetNetworkRut(pVtx->GetVBreadthID(),pVtx->GetVRutList()[i]);
                }
                ASSERT(nAngleEnter != -1);
                ASSERT(nAngleExit  != -1);
                
                
                if(pRoadSectionBranch)
                {
                    if( abs( abs(nAngleExit - nAngleEnter)-180 ) < 12
                        &&
                        bSameRutType
                        &&
                        bSameName
                        &&
                        (pRoadSectionBranch->GetRutType() == 1 )
                        )
                    {
                        pVNode->GetVNodeInfo()->byConfusion2 = 16;
                    }  
                    
                }
                
            }  
        }
        
    }
    //end here
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    //20070704������BUG13�Ľ������
    //start here
    if(pRoadSectionPrevious->GetRutID() != pRoadSectionNext->GetRutID() || pRoadSectionPrevious->GetRBreadthID() != pRoadSectionNext->GetRBreadthID())
    {
        if(pVtx->GetVRutNum() == 3)
        {
            BOOL bExitRoadSectionIsOnly = FALSE;
            short nAngleEnter   = -1;
            short nAngleExit    = -1;
            CGIS_NetworkRut* pRoadSectionBranch = NULL;
            for(int i = 0; i < 3; i++)
            {
                if(pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID())
                {
                    nAngleEnter = pVtx->GetVRTurnList()[i];
                    continue;
                }
                
                if(pVtx->GetVRutList()[i] == pRoadSectionNext->GetRutID())
                {
                    nAngleExit = pVtx->GetVRTurnList()[i];
                    continue;
                }    
                
                pRoadSectionBranch = pBMNetwork->GetNetworkRut(pVtx->GetVBreadthID(),pVtx->GetVRutList()[i]);
            }
            ASSERT(nAngleEnter != -1);
            ASSERT(nAngleExit  != -1);
            
            if(pRoadSectionBranch)
            {
                if(!pBMNetwork->CouldReachAnotherVertexInRoadSection(pVtx, pRoadSectionBranch))
                    bExitRoadSectionIsOnly = TRUE;
            }
            
            if(abs( abs(nAngleExit - nAngleEnter)-180 ) < 60 && bExitRoadSectionIsOnly)
            {
                pVNode->GetVNodeInfo()->byAngle = 1;
            }
            
        }
    }
    //end here
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    //20070705������BUG11�Ľ������
	//start here
	if(pVNode->GetVNodeInfo()->byAngle > 1)
	{
		if(pVNode->GetVNodeInfo()->byConfusion2 == 15)
		{
			ASSERT(nAngleRightNeighbour != -1);

			if(abs( abs(nAngleRightNeighbour - nAngleEnter)-180 ) < 30)
			{
				pVNode->GetVNodeInfo()->byConfusion2 = 16;
			}
		}
        else if(pVNode->GetVNodeInfo()->byConfusion2 == 37)
        {
            ASSERT(nAngleLeftNeighbour != -1);
            if(abs( abs(nAngleLeftNeighbour - nAngleEnter)-180 ) < 30)
            {
                pVNode->GetVNodeInfo()->byConfusion2 = 16;
            }

        }
	}
    //end here
    //////////////////////////////////////////////////////////////////////////

#endif	
	return TRUE;
}

CGIS_LayerManager* CGIS_GeoMap::getLayerManager(){
	if( _layerMgnSet.size()){
		return &_layerMgnSet.front();
	}
	return NULL;
}

