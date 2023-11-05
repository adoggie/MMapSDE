// GIS_GeoLine.cpp: implementation of the CGIS_FeatureLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeatureLine::CGIS_FeatureLine(EnGeoObjType enOType)
	: CGIS_Feature( enOType )
{
	m_nPtNum = 0;
	m_pPart = NULL;
	m_nPartNum = 0;
}

CGIS_FeatureLine::~CGIS_FeatureLine()
{
	if( m_pPart )
		delete [] m_pPart;
	m_pPart = NULL;
	m_nPartNum = 0;
	m_nPtNum = 0;
}
void CGIS_FeatureLine::SetPartNum( int nPartNum )
{
	m_nPartNum = nPartNum;
}
int  CGIS_FeatureLine::GetPartNum( )
{
	return m_nPartNum;
}
void CGIS_FeatureLine::SetPtNum( unsigned short nPtNum )
{
	m_nPtNum = nPtNum;
}
unsigned short  CGIS_FeatureLine::GetPtNum( )
{
	return m_nPtNum;
}
void CGIS_FeatureLine::SetPart( int *pPart )
{
	m_pPart = pPart;
}
int* CGIS_FeatureLine::GetPart( )
{
	return m_pPart;
}
void CGIS_FeatureLine::SetRectObj( ST_GEO_RECT rtObj )
{
	memcpy(&m_rtObj, &rtObj, sizeof(ST_GEO_RECT));
//	m_rtObj = rtObj;
}
ST_GEO_RECT CGIS_FeatureLine::GetRectObj( )
{
	return m_rtObj;
}
CGIS_Feature* CGIS_FeatureLine::Clone()
{
	CGIS_FeatureLine* pFh = new CGIS_FeatureLine( m_enOType );
	int *pPart = new int[m_nPartNum];
	int i;
	for( i=0;	i<m_nPartNum;	i++ )
		pPart[i] = m_pPart[i];
	pFh->SetPart( pPart );
	pFh->SetPartNum(m_nPartNum);
	ST_GEO_PTXY *pPt = new ST_GEO_PTXY[m_nPtNum];
	for( i=0;	i<m_nPtNum;	i++ )
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
