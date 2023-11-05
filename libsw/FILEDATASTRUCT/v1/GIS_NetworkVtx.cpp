// GIS_NetworkVtx.cpp: implementation of the CGIS_NetworkVtx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkVtx.h"

#include "GIS_NetworkRut.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkVtx::CGIS_NetworkVtx( )
{
	m_nVtxID = 0;
//	m_byEdge = 0;	
//	m_nABID = 0;	
//	m_nARID = 0;
	m_pVEInfo = NULL;
	m_byRNum = 0;	
	m_pVSInfo = NULL;
//	m_posOHead = m_posOEnd = NULL;
//	m_bySHead = m_bySEnd = 0;
	m_pVRList = NULL;	
	m_pRTList = NULL;
	m_pRLList = NULL;
	m_nVBID	= 0;
}
CGIS_NetworkVtx::~CGIS_NetworkVtx()
{
	m_nVtxID = 0;	
//	m_byEdge = 0;	
//	m_nABID = 0;	
//	m_nARID = 0;
	if( m_pVEInfo )
		delete m_pVEInfo;
	m_pVEInfo = NULL;
	m_byRNum = 0;	
	m_nVBID	= 0;
	if( m_pVSInfo )
		delete m_pVSInfo;
	m_pVSInfo = NULL;
//	m_posOHead = m_posOEnd = NULL;
//	m_bySHead = m_bySEnd = 0;
	if( m_pVRList )
		delete[] m_pVRList;
	m_pVRList = NULL;	
	if( m_pRTList )
		delete[] m_pRTList;
	m_pRTList = NULL;	
	if( m_pRLList )
		delete[] m_pRLList;
	m_pRLList = NULL;	
}
void CGIS_NetworkVtx::SetVtxInfo( unsigned short nVID, ST_GEO_PTXY ptPostion, int nVBID )
{
	m_nVtxID = nVID;
	m_ptPosition = ptPostion;
	m_nVBID = nVBID;
}
void CGIS_NetworkVtx::SetVRutInfo( BYTE byRNum, unsigned short *pVRList, unsigned short *pRTList, BYTE *pRLList )
{
	m_byRNum = byRNum;
	if( m_pVRList )
		delete[] m_pVRList;
	m_pVRList = NULL;	
	if( m_pRTList )
		delete[] m_pRTList;
	m_pRTList = NULL;	
	if( m_pRLList )
		delete[] m_pRLList;
	m_pRLList = NULL;	
	if( m_byRNum > 0 )
	{
		m_pVRList = new unsigned short[byRNum];
		memcpy( m_pVRList, pVRList, sizeof(unsigned short)*byRNum );
		m_pRTList = new unsigned short[byRNum];
		memcpy( m_pRTList, pRTList, sizeof(unsigned short)*byRNum );
		m_pRLList = new BYTE[byRNum];
		memcpy( m_pRLList, pRLList, sizeof(BYTE)*byRNum );
	}
}
void CGIS_NetworkVtx::SetVEInfo( BYTE byEdge, int nABID, unsigned short nARID )
{
	if( m_pVEInfo )
		delete m_pVEInfo;
	m_pVEInfo = NULL;
	if( byEdge == 1 )
	{
		m_pVEInfo = new ST_GEO_VEDGE;
		m_pVEInfo->byEdge = byEdge;
		m_pVEInfo->nABID = nABID;
		m_pVEInfo->nARID = nARID;
	}
//	m_byEdge = byEdge;
//	if( m_byEdge == 1 )
//	{
//		m_nABID = nABID;
//		m_nARID = nARID;
//	}
}

unsigned short CGIS_NetworkVtx::GetVtxID( )
{
	return m_nVtxID;
}
ST_GEO_PTXY CGIS_NetworkVtx::GetVtxPosition( )
{
	return m_ptPosition;
}
BYTE CGIS_NetworkVtx::GetVRutNum( )
{
	return m_byRNum;
}
unsigned short* CGIS_NetworkVtx::GetVRutList( )
{
	return m_pVRList;
}
unsigned short* CGIS_NetworkVtx::GetVRTurnList( )
{
	return m_pRTList;
}
BYTE* CGIS_NetworkVtx::GetVRLList( )
{
	return m_pRLList;
}
BYTE CGIS_NetworkVtx::GetVtxEdge( )
{
	if( m_pVEInfo )
		return m_pVEInfo->byEdge;
	return 0;
}
int CGIS_NetworkVtx::GetVAbutBID( )
{
	if( m_pVEInfo )
		return m_pVEInfo->nABID;
	return -1;
}
unsigned short CGIS_NetworkVtx::GetVAbutRID( )
{
	if( m_pVEInfo )
		return m_pVEInfo->nARID;
	return -1;
}
int CGIS_NetworkVtx::GetVBreadthID()
{
	return m_nVBID;
}
ST_GEO_VSORT* CGIS_NetworkVtx::GetVtxSInfo( )
{
	if( m_pVSInfo == NULL )
	{
		m_pVSInfo = new ST_GEO_VSORT;
		memset( m_pVSInfo, 0, sizeof(ST_GEO_VSORT) );
	}
	return m_pVSInfo;
}
