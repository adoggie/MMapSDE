
#include "GIS_LayerAttInfo.h"


CGIS_LayerAttInfo::CGIS_LayerAttInfo(){
	//m_pLAInfoMap = new CGIS_FieldMap;
}

CGIS_LayerAttInfo::~CGIS_LayerAttInfo(){
	/*
	if( m_pLAInfoMap )
	{
		CGIS_Field *pField = NULL;
		for( int i=0;	i< m_pLAInfoMap->GetCount();	i++ )
		{
			m_pLAInfoMap->Lookup(i,pField);
			if( pField )
				delete pField;
			pField = NULL;
		}
		m_pLAInfoMap->RemoveAll();
		delete m_pLAInfoMap;
	}
	m_pLAInfoMap = NULL;*/
}
CGIS_FieldMap* CGIS_LayerAttInfo::GetGISFieldMap(){
	return &m_pLAInfoMap;
}
void CGIS_LayerAttInfo::AddFieldInfo( CGIS_Field* pField )
{
	//ASSERT( m_pLAInfoMap );
	//ASSERT( pField );
	UINT nOffset = 0;
	CGIS_FieldMap::iterator itr; 
	itr = m_pLAInfoMap.find( m_pLAInfoMap.size()-1);
	if( itr!=m_pLAInfoMap.end()){
		nOffset = itr->second.GetFieldOffset()+itr->second.GetFieldSize();
	}
	/*
	if( m_pLAInfoMap->GetCount() > 0 )
	{
		CGIS_Field *pTem;
		if( m_pLAInfoMap->Lookup( m_pLAInfoMap->GetCount()-1, pTem ) )
			nOffset = pTem->GetFieldOffset()+pTem->GetFieldSize();
	}*/
	//pField->SetFieldID( m_pLAInfoMap->GetCount() );
	pField->SetFieldID( m_pLAInfoMap.size());
	pField->SetFieldOffset( nOffset );
	//m_pLAInfoMap->SetAt(pField->GetFieldID(), pField );
	m_pLAInfoMap[pField->GetFieldID()] = *pField;
}
