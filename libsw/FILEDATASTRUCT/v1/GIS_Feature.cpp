// GIS_Feature.cpp: implementation of the CGIS_Feature class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_Feature.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_Feature::CGIS_Feature(EnGeoObjType enOType)
{
	m_enOType =  enOType;
//	m_pAtt = NULL;
	m_pPtList = NULL;
	m_nBreadthID = 0;
	m_bstrAnno = NULL;
	m_nAnno = 0;
}

CGIS_Feature::~CGIS_Feature()
{
	if( m_enOType == EN_GOTYPE_UPOINT || m_enOType == EN_GOTYPE_ULINE || 
		m_enOType == EN_GOTYPE_UPOLY || m_enOType == EN_GOTYPE_URUT	||
		m_enOType == EN_GOTYPE_ROUTE || m_enOType == EN_GOTYPE_TRACK ||
		m_enOType == EN_GOTYPE_ADDRESS || m_nBreadthID >=	 0 )
	{
		if( m_pPtList )
			delete[] m_pPtList;
		if( m_bstrAnno )
			delete[] m_bstrAnno;
	}
//	if( m_pAtt )
//		delete m_pAtt;
//	m_pAtt = NULL;
	m_pPtList = NULL;
	m_nBreadthID = 0;
	m_bstrAnno = NULL;
	m_nAnno = 0;
}

//void CGIS_Feature::SetAttribute( CGIS_Attribute *pAtt )
//{
//	m_pAtt = pAtt;
//}
//CGIS_Attribute* CGIS_Feature::GetAttribute( )
//{
//	return m_pAtt;
//}
void CGIS_Feature::SetAnnotation( char* bstrAnno )
{
	m_bstrAnno = bstrAnno;
}
char* CGIS_Feature::GetAnnotation( )
{
	return m_bstrAnno;
}
void CGIS_Feature::SetPtList( ST_GEO_PTXY *pPtList )
{
	m_pPtList = pPtList;
}
ST_GEO_PTXY* CGIS_Feature::GetPtList( )
{
	return m_pPtList;
}
CGIS_Feature::EnGeoObjType CGIS_Feature::GetObjType( )
{
	return m_enOType;
}
CGIS_Feature* CGIS_Feature::Clone()
{
	return NULL;
}
void CGIS_Feature::SetAnnoNum( short nAnno )
{
	m_nAnno = nAnno;
}
short CGIS_Feature::GetAnnoNum( )
{
	return m_nAnno;
}
