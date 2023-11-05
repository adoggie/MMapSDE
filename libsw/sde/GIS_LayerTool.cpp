
//#include "stdafx.h"
#include "GIS_LayerTool.h"
#include "GIS_LayerManager.h"
#include "GIS_GeoMap.h"

CGIS_LayerTool::CGIS_LayerTool(){
	m_Type = LAYER_TOOL;
}

CGIS_LayerTool::~CGIS_LayerTool(){

}

/**
 * @brief 创建用户层.
 *
 * @param pLInfo:层信息结构指针
 * @param pAttInfo:层属性信息结构指针
 *
 * @return
*/
BOOL CGIS_LayerTool::CreateUserLayer( CGIS_LayerInfo *pLInfo, CGIS_LayerAttInfo *pAttInfo ){
	//ASSERT( m_pMap );
	//从地图中取到层管理器让其创建一个新层.(层管理器目前只有一个吗?bsxy)
	/*
	CGIS_LayerManager *pLManager = NULL;
	POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
	if( pos )
		pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext( pos );
	if( pLManager )
		return pLManager->CreateUserLayer( pLInfo, pAttInfo );

	return FALSE;*/
	if( m_pMap->getLayerManager() ){
		m_pMap->getLayerManager()->CreateUserLayer( pLInfo, pAttInfo );
	}
}

/**
 * @brief 删除用户层
 *
 * @param nLID:层id
 * @return
*/
BOOL CGIS_LayerTool::DeleteUserLayer( UINT nLID ){
	/*
	ASSERT( m_pMap );
	CGIS_LayerManager *pLManager = NULL;
	POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
	while( pos )
	{
		pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext( pos );
		if( pLManager )
			if( pLManager->DeleteUserLayer(nLID) )
				return TRUE;
	}
	return FALSE;
	*/
	return FALSE;
}

/**
 * @brief 更新用户层
 *
 * @param nLID:层id
 * @return
*/
BOOL CGIS_LayerTool::UpdateUserLayer( UINT nLID ){
	/*
	CGIS_LayerManager *pLManager = NULL;
	POSITION pos = m_pMap->m_LayerManagerSet->GetHeadPosition();
	while( pos )
	{
		pLManager = (CGIS_LayerManager*)m_pMap->m_LayerManagerSet->GetNext( pos );
		if( pLManager )
			if( pLManager->UpdateUserLayer(nLID) )
				return TRUE;
	}	*/
	return FALSE;
}