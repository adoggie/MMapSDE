// GIS_FeaturePoly.cpp: implementation of the CGIS_FeaturePoly class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeaturePoly.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_FeaturePoly::CGIS_FeaturePoly(EnGeoObjType enOType)
	: CGIS_Feature( enOType )
{
	m_pPart = NULL;
	m_nPartNum = 0;
	m_nPtNum = 0;
}

CGIS_FeaturePoly::~CGIS_FeaturePoly()
{
	if( m_pPart )
		delete[] m_pPart;
	m_pPart = NULL;
	m_nPartNum = 0;
	m_nPtNum = 0;
}
void CGIS_FeaturePoly::SetPartNum( int nPartNum )
{
	m_nPartNum = nPartNum;
}
int  CGIS_FeaturePoly::GetPartNum( )
{
	return m_nPartNum;
}
void CGIS_FeaturePoly::SetPtNum( unsigned short nPtNum )
{
	m_nPtNum = nPtNum;
}
unsigned short  CGIS_FeaturePoly::GetPtNum( )
{
	return m_nPtNum;
}
void CGIS_FeaturePoly::SetPart( int *pPart )
{
	m_pPart = pPart;
}
int* CGIS_FeaturePoly::GetPart( )
{
	return m_pPart;
}
void CGIS_FeaturePoly::SetRectObj( ST_GEO_RECT rtObj )
{
	memcpy(&m_rtObj, &rtObj, sizeof(ST_GEO_RECT));
//	m_rtObj = rtObj;
}
ST_GEO_RECT CGIS_FeaturePoly::GetRectObj( )
{
	return m_rtObj;
}
void CGIS_FeaturePoly::SetCenterPt( float dx, float dy )
{
	m_CenterPt.dx = dx;
	m_CenterPt.dy = dy;
}
ST_GEO_PTXY CGIS_FeaturePoly::GetCenterPt()
{
	return m_CenterPt;
}
CGIS_Feature* CGIS_FeaturePoly::Clone()
{
	int i;
	CGIS_FeaturePoly* pFh = new CGIS_FeaturePoly( m_enOType );
	int *pPart = new int[m_nPartNum];
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
	pFh->SetCenterPt( m_CenterPt.dx, m_CenterPt.dy );
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
void CGIS_FeaturePoly::SetPolyShowClass( BYTE nShowClass )
{
	m_nShowClass = nShowClass;
}
BYTE CGIS_FeaturePoly::GetPolyShowClass( )
{
	return m_nShowClass;
}
