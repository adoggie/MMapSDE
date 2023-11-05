
#include "GIS_FeatureEditer.h"

CGIS_FeatureEditer::CGIS_FeatureEditer(){
	m_Type = EDIT_FEATURE;	//工具类型 
	m_pLayer = NULL;
}

CGIS_FeatureEditer::~CGIS_FeatureEditer(){

}

BOOL CGIS_FeatureEditer::CreateFeature( CGIS_Feature *pFeature ){
	/*
	CPtrList *pOList = NULL;
	m_pLayer->GetObjectListMap()->Lookup( 0, pOList );
	if( !pOList )
		return FALSE;
	pOList->AddTail( pFeature );
	return TRUE;*/ //旧代码简单的指向第一个网格块进行添加，错误!
	return FALSE;
	//未支持对象创建
}

BOOL CGIS_FeatureEditer::UpdateFeature( CGIS_Feature *pFeature ){
	return FALSE;
}

BOOL CGIS_FeatureEditer::DeleteFeature( CGIS_Feature *pFeature ){
	/*
	CPtrList *pOList = NULL;
	m_pLayer->GetObjectListMap()->Lookup( 0, pOList );
	if( !pOList )
		return FALSE;
	POSITION pos = pOList->GetHeadPosition();
	CGIS_Feature *pTem = NULL;
	POSITION psTem = NULL;
	while( pos )
	{
		psTem = pos;
		pTem = (CGIS_Feature*)pOList->GetNext(pos);
		if( pTem )
		{
			if( pTem == pFeature )
			{
				delete pTem;
				pTem = NULL;
				pOList->RemoveAt( psTem );
				return TRUE;
			}
		}
	}*/
	return FALSE;
}

void CGIS_FeatureEditer::SetLayer( IGIS_Layer *pLayer ){
	m_pLayer = pLayer;
}