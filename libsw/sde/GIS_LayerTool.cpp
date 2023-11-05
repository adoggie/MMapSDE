
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
 * @brief �����û���.
 *
 * @param pLInfo:����Ϣ�ṹָ��
 * @param pAttInfo:��������Ϣ�ṹָ��
 *
 * @return
*/
BOOL CGIS_LayerTool::CreateUserLayer( CGIS_LayerInfo *pLInfo, CGIS_LayerAttInfo *pAttInfo ){
	//ASSERT( m_pMap );
	//�ӵ�ͼ��ȡ������������䴴��һ���²�.(�������Ŀǰֻ��һ����?bsxy)
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
 * @brief ɾ���û���
 *
 * @param nLID:��id
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
 * @brief �����û���
 *
 * @param nLID:��id
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