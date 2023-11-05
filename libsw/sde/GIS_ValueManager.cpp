
//#include "stdafx.h"
#include "GIS_ValueManager.h"


CGIS_ValueManager::CGIS_ValueManager()
{
	//m_pValueSet = new CPtrList;
}

CGIS_ValueManager::~CGIS_ValueManager()
{
/*
	if(m_pValueSet != NULL)
	{
		POSITION pos = m_pValueSet->GetHeadPosition();
		CGIS_MessageValue* pTempMessageValue;
		while(pos)
		{
			pTempMessageValue = (CGIS_MessageValue*)(m_pValueSet->GetNext(pos));
			if(pTempMessageValue)
			{
				delete pTempMessageValue;
				pTempMessageValue = NULL;
			}
		}
		delete m_pValueSet;
	
	}
	m_pValueSet = NULL;	*/
	std::vector< CGIS_MessageValue*>::iterator itr;
	for(itr=m_pValueSet.begin();itr!=m_pValueSet.end();itr++){
		CGIS_MessageValue* value = (*itr);
		delete value;
	}
}
BOOL CGIS_ValueManager::LoadAValue(CGIS_MessageValue* pMessageValue)
{
	if(pMessageValue != NULL)
	{
		//m_pValueSet->AddTail(pMessageValue);
		m_pValueSet.push_back(pMessageValue);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL CGIS_ValueManager::UnLoadAValue(UINT ValueID)
{
	return TRUE;
}
