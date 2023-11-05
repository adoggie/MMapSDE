// GIS_FeaturePoint.cpp: implementation of the CGIS_FeaturePoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeaturePoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeaturePoint::CGIS_FeaturePoint(EnGeoObjType enOType)
	: CGIS_Feature( enOType )
{
	m_szAddress = NULL;
	m_szPhone = NULL;
	m_nAddLen = 0;
	m_nPhoneLen = 0;
	m_nPOIType = 0;
	m_nIconID = 0;
}

CGIS_FeaturePoint::~CGIS_FeaturePoint()
{
	if( m_szAddress )
		delete[] m_szAddress;
	if( m_szPhone )
		delete[] m_szPhone;
	m_szAddress = NULL;
	m_szPhone = NULL;
	m_nAddLen = 0;
	m_nPhoneLen = 0;
	m_nPOIType = 0;
	m_nIconID = 0;
}
void CGIS_FeaturePoint::SetIconID( UINT nIconID )
{
	m_nIconID = nIconID;
}
UINT CGIS_FeaturePoint::GetIConID( )
{
	return m_nIconID;
}
unsigned short CGIS_FeaturePoint::GetPointObjectType( )
{
	return m_nPOIType;
}
void CGIS_FeaturePoint::SetPointObjectType( unsigned short nPOIType )
{
	m_nPOIType = nPOIType;
}
BYTE CGIS_FeaturePoint::GetStrAddressLength( )
{
	return m_nAddLen;
}
void CGIS_FeaturePoint::SetStrAddressLength( BYTE nAddLen )
{
	m_nAddLen = nAddLen;
}
char* CGIS_FeaturePoint::GetAddress( )
{
	return m_szAddress;
}
void CGIS_FeaturePoint::SetAddress( char *szAddress )
{
	m_szAddress = szAddress;
}
BYTE CGIS_FeaturePoint::GetStrPhoneLength( )
{
	return m_nPhoneLen;
}
void CGIS_FeaturePoint::SetStrPhoneLength( BYTE nPhoneLen )
{
	m_nPhoneLen = nPhoneLen;
}
char* CGIS_FeaturePoint::GetPhoneNum( )
{
	return m_szPhone;
}
void CGIS_FeaturePoint::SetPhoneNum( char *szPhone )
{
	m_szPhone = szPhone;
}
CGIS_Feature* CGIS_FeaturePoint::Clone()
{
	CGIS_FeaturePoint* pFh = new CGIS_FeaturePoint( m_enOType );
	ST_GEO_PTXY *pPt = new ST_GEO_PTXY;
	pPt->dx = m_pPtList->dx;
	pPt->dy = m_pPtList->dy;
	pFh->SetPtList(pPt);
	pFh->SetPointObjectType(m_nPOIType);
	char *szAnno = NULL;
	if( m_nAnno>0 )
	{
		szAnno = new char[m_nAnno];
		memcpy( szAnno, m_bstrAnno, m_nAnno );
		pFh->SetAnnotation( szAnno );
	}
	pFh->SetAnnoNum( m_nAnno );
	char *szAdd = NULL;
	if( m_nAddLen>0 )
	{
		szAdd = new char[m_nAddLen];
		memcpy( szAdd, m_szAddress, m_nAddLen );
		pFh->SetAddress( szAdd );
		pFh->SetStrAddressLength( m_nAddLen );
	}
	char *szPhone = NULL;
	if( m_nPhoneLen>0 )
	{
		szPhone = new char[m_nPhoneLen];
		memcpy( szPhone, m_szPhone, m_nPhoneLen );
		pFh->SetPhoneNum( szPhone );
		pFh->SetStrPhoneLength( m_nPhoneLen );
	}
	pFh->m_nBreadthID = m_nBreadthID;
	pFh->SetIconID( m_nIconID );
//	if( m_pAtt )
//	{
///*		CGIS_Attribute *pAtt = new CGIS_Attribute;
//		void *pV = m_pAtt->GetValue();
//		char *pVal = new char[sizeof(pV)];
//		memcpy( pVal, m_pAtt->GetValue(), sizeof(pV) );
//		pAtt->SetValue( pVal );
//		pFh->SetAttribute( pAtt );
//*/	}
	return pFh;
}
