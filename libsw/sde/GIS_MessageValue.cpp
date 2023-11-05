
#include "stdafx.h"
#include "GIS_MessageValue.h"

CGIS_MessageValue::CGIS_MessageValue()
{
	m_pValue = NULL;
	m_pDefVal = NULL;
}

CGIS_MessageValue::~CGIS_MessageValue()
{
	if(m_pDefVal != NULL)
	{
		delete m_pDefVal;
		m_pDefVal = NULL;
	}
	if(m_pValue != NULL)
	{
		delete m_pValue;
		m_pValue = NULL;
	}
}
