// GIS_NetworkNodeRut.cpp: implementation of the CGIS_NetworkNodeRut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkNodeRut.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkNodeRut::CGIS_NetworkNodeRut()
{
	m_nAnnoNum = 0;
	m_szAnno = NULL;
	m_nPtNum = 0;
	m_pPtList = NULL;
	m_nRutID = 0;
}

CGIS_NetworkNodeRut::~CGIS_NetworkNodeRut()
{
	if( m_pPtList )
		delete[] m_pPtList;
	if( m_szAnno )
		delete[] m_szAnno;
	m_nAnnoNum = 0;
	m_szAnno = NULL;
	m_nPtNum = 0;
	m_pPtList = NULL;
	m_nRutID = 0;
}
void CGIS_NetworkNodeRut::SetRutID( int nRutID )
{
	m_nRutID = nRutID;
}
int CGIS_NetworkNodeRut::GetRutID( )
{
	return m_nRutID;
}

void CGIS_NetworkNodeRut::SetRutPtNum( short nPtNum )
{
	m_nPtNum = nPtNum;
}
short CGIS_NetworkNodeRut::GetRutPtNum( )
{
	return m_nPtNum;
}
void CGIS_NetworkNodeRut::SetRutPtList( ST_GEO_PTXY *pPtList )
{
	m_pPtList = pPtList;
}
ST_GEO_PTXY* CGIS_NetworkNodeRut::GetRutPtList( )
{
	return m_pPtList;
}
void CGIS_NetworkNodeRut::SetRutAnnoNum( BYTE nAnnoNum )
{
	m_nAnnoNum = nAnnoNum;
}
BYTE CGIS_NetworkNodeRut::GetRutAnnoNum( )
{
	return m_nAnnoNum;
}
void CGIS_NetworkNodeRut::SetRutAnno( char *szAnno )
{
	m_szAnno = szAnno;
}
char* CGIS_NetworkNodeRut::GetRutAnno( )
{
	return m_szAnno;
}
