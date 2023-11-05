// GIS_FeatureRut.cpp: implementation of the CGIS_FeatureRut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureRut.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureRut::CGIS_FeatureRut(EnGeoObjType enOType)
	: CGIS_Feature( enOType )
{
	m_nRutID = 0;
//	m_nPartNum = 0;
	m_nPtNum = 0;
// 	m_pPart = NULL;
}

CGIS_FeatureRut::~CGIS_FeatureRut()
{
//	if( m_pPart )
//		delete[] m_pPart;
	m_nRutID = 0;
// 	m_nPartNum = 0;
	m_nPtNum = 0;
// 	m_pPart = NULL;
}
//void CGIS_FeatureRut::SetPartNum( int nPartNum )
//{
//	m_nPartNum = nPartNum;
//}
//int  CGIS_FeatureRut::GetPartNum( )
//{
//	return m_nPartNum;
//}
void CGIS_FeatureRut::SetPtNum( unsigned short nPtNum )
{
	m_nPtNum = nPtNum;
}
unsigned short  CGIS_FeatureRut::GetPtNum( )
{
	return m_nPtNum;
}
//void CGIS_FeatureRut::SetPart( int *pPart )
//{
//	m_pPart = pPart;
//}
//int* CGIS_FeatureRut::GetPart( )
//{
//	return m_pPart;
//}
void CGIS_FeatureRut::SetRectObj( ST_GEO_RECT rtObj )
{
	memcpy(&m_rtObj, &rtObj, sizeof(ST_GEO_RECT));
//	m_rtObj = rtObj;
}
ST_GEO_RECT CGIS_FeatureRut::GetRectObj( )
{
	return m_rtObj;
}
int CGIS_FeatureRut::GetRutID( )
{
	return m_nRutID;
}
void CGIS_FeatureRut::SetRutID( int nRutID )
{
	m_nRutID = nRutID;
}
BYTE CGIS_FeatureRut::GetRutDirection( )
{
	return m_byDirection;
}
void CGIS_FeatureRut::SetRutDirection( BYTE byDirection )
{
	m_byDirection = byDirection;
}

CGIS_Feature* CGIS_FeatureRut::Clone()
{
	CGIS_FeatureRut* pFh = new CGIS_FeatureRut( m_enOType );
//	int *pPart = new int[m_nPartNum];
//	for( int i=0;	i<m_nPartNum;	i++ )
//		pPart[i] = m_pPart[i];
//	pFh->SetPart( pPart );
//	pFh->SetPartNum(m_nPartNum);
	ST_GEO_PTXY *pPt = new ST_GEO_PTXY[m_nPtNum];
	for( int i=0;	i<m_nPtNum;	i++ )
	{
		pPt[i].dx = m_pPtList[i].dx;
		pPt[i].dy = m_pPtList[i].dy;
	}
	pFh->SetPtList(pPt);
	pFh->SetPtNum(m_nPtNum);
	char *szAnno = NULL;
	if( m_nAnno>0 )
	{
		szAnno = new char[m_nAnno];
		memcpy( szAnno, m_bstrAnno, m_nAnno );
		pFh->SetAnnotation( szAnno );
	}
	pFh->SetAnnoNum( m_nAnno );
	pFh->m_nBreadthID = m_nBreadthID;
	pFh->SetRectObj( m_rtObj );
	pFh->SetRutID( m_nRutID );
	pFh->SetRutDirection( m_byDirection );
//	if( m_pAtt )
//	{
/*		CGIS_Attribute *pAtt = new CGIS_Attribute;
		void *pV = m_pAtt->GetValue();
		char *pVal = new char[sizeof(pV)];
		memcpy( pVal, m_pAtt->GetValue(), sizeof(pV) );
		pAtt->SetValue( pVal );
		pFh->SetAttribute( pAtt );
*/// 	}
	return pFh;
}
