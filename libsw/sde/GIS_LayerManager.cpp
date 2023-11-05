
#include "GIS_LayerManager.h"
#include "GIS_UserLayer.h"

CGIS_LayerManager::CGIS_LayerManager(){
	//m_pLayListMap = new CGIS_LayerMap;  //地图层列表
}

CGIS_LayerManager::~CGIS_LayerManager()
{
//	if( m_pLayListMap )
//	{
//		short nKey;
//		IGIS_Layer* pLayer;
//		POSITION pos = m_pLayListMap->GetStartPosition();
//		while (pos != NULL)
//		{
//			m_pLayListMap->GetNextAssoc( pos, nKey, pLayer );
//			m_pLayListMap->RemoveKey( nKey );
//			if( pLayer )
//				delete pLayer;
//		}
//		m_pLayListMap->RemoveAll();
//		delete m_pLayListMap;
//	}
//	m_pLayListMap = NULL;
}

//创建用户图层，然后加入hash表内
BOOL CGIS_LayerManager::CreateUserLayer( CGIS_LayerInfo *pInfo,CGIS_LayerAttInfo *pLAttInfo ){
	CGIS_UserLayer *pLayer = new CGIS_UserLayer(pInfo, pLAttInfo);
	//m_pLayListMap->SetAt( pInfo->GetLayerID(), pLayer );
	_layerList[pInfo->GetLayerID()] = pLayer;
	return TRUE;
}
BOOL CGIS_LayerManager::DeleteUserLayer( UINT nLayerID )
{
	//ASSERT( m_pLayListMap );
	IGIS_Layer *pLayer;
	//m_pLayListMap->Lookup( nLayerID, pLayer );
	CGIS_LayerMap::iterator itr;
	itr =_layerList.find(nLayerID); 
	if( itr == _layerList.end()){
		return TRUE;
	}
	delete (itr->second);
	_layerList.erase(itr);
/*
	if( pLayer )
		delete pLayer;
	//m_pLayListMap->RemoveKey( nLayerID );
	*/
	return TRUE;
}
BOOL CGIS_LayerManager::UpdateUserLayer( UINT nLID )
{
	return TRUE;
}
BOOL CGIS_LayerManager::SaveUserLayer( )
{
	return TRUE;
}
IGIS_Layer* CGIS_LayerManager::GetLayer( UINT nLayerID ){
	/*
	IGIS_Layer *pLayer = NULL;
	m_pLayListMap->Lookup( nLayerID, pLayer );
	return pLayer;
	*/
	return _layerList[nLayerID];
}
/*
IGIS_Layer* CGIS_LayerManager::GetFirstLayer( )
{
	POSITION pos = m_pLayListMap->GetStartPosition();
	short    nKey;
	IGIS_Layer* pLayer = NULL;
	if( pos != NULL )
		m_pLayListMap->GetNextAssoc( pos, nKey, pLayer );
	
	return pLayer;
}
IGIS_Layer* CGIS_LayerManager::GetNextLayer( )
{
	return NULL;
}
IGIS_Layer* CGIS_LayerManager::GetLastLayer( )
{
	IGIS_Layer *pLayer = NULL;
	int nPos = m_pLayListMap->GetCount();
	m_pLayListMap->Lookup( nPos, pLayer );
	return pLayer;
}
*/

CGIS_LayerMap &CGIS_LayerManager::GetLayerListMap(){
	return _layerList;
}

/*
void CGIS_LayerManager::DrawLayer( IGIS_LayerFile *pLFile, IGIS_MapView *pView, CRect rtPolt, ST_GEO_RECT rtDraw, double dbScale )
{
	ASSERT( pView );
	POSITION pos = m_pLayListMap->GetStartPosition();
	short    nKey;
	IGIS_Layer* pLayer = NULL;
	while( pos != NULL )
	{
		m_pLayListMap->GetNextAssoc( pos, nKey, pLayer );
		if( pLayer )
			if( pLayer->GetLayerInfo()->GetLayerState() )
				pLayer->DrawLayer( pLFile, pView, rtPolt, rtDraw, dbScale );
	}
}*/

/*
void CGIS_LayerManager::DrawLayer( IGIS_MapView *pView, CRect rtPolt, ST_GEO_RECT rtDraw, double dbScale )
{
	ASSERT( pView );
	IGIS_Layer* pLayer = NULL;
	for( int i=0;	i<LAYER_PT_END+1;	i++ )
	{
		m_pLayListMap->Lookup( i, pLayer );
		if( pLayer )
			if( pLayer->GetLayerInfo()->GetLayerState())
				pLayer->DrawLayer( pView, rtPolt, rtDraw, dbScale );
	}
}
*/