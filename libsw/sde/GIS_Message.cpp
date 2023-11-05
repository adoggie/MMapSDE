
//#include "stdafx.h"
#include "GIS_Message.h"


CGIS_Message::CGIS_Message()
{
	m_pValueManager = new CGIS_ValueManager;
}

CGIS_Message::~CGIS_Message()
{
	if(m_pValueManager != NULL)
	{
		delete m_pValueManager;
		m_pValueManager = NULL;
	}
}
UINT CGIS_Message::GetMessageType()
{
	return m_Type;
}

CGIS_ValueManager* CGIS_Message::GetMessageData()
{
	return m_pValueManager;
}
BOOL CGIS_Message::SetValueManager(CGIS_ValueManager* pValueManager)
{
	if(pValueManager != NULL)
	{
		m_pValueManager = pValueManager;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}
BOOL CGIS_Message::SetType(UINT IType)
{
	m_Type = IType;
	return TRUE;
}
