// GIS_NetworkRut.cpp: implementation of the CGIS_NetworkRut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkRut.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkRut::CGIS_NetworkRut()
{
	m_nRutID = 0;		
	m_byDirection = 0;	
	m_byType = 0;		
	m_byCharge = 0;		
//	m_byLaneNum = 0;		
//	m_bySpeed = 0;		
	m_byRutLevel = 0;	
	m_byRutType = 0;	
	m_dRLength = 0.0;	
	m_nVtxStartID = 0;	
	m_nVtxEndID = 0;	
	m_pRLInfo = NULL;
//	m_byLimit = 0;		
//	m_byRLimit = 0;		
//	m_pLRList = NULL;		
//	m_pRLRList = NULL;	

//	m_pRVInfo = NULL;
//	m_byVirtual = 0;	
//	m_pVORList = NULL;		
//	m_pVIDs = NULL;	
	m_nRBID = 0;
}

CGIS_NetworkRut::~CGIS_NetworkRut()
{
	if( m_pRLInfo )
	{
		if( m_pRLInfo->pLRList )
			delete[] m_pRLInfo->pLRList;
		if( m_pRLInfo->pRLRList )
			delete[] m_pRLInfo->pRLRList;
		delete m_pRLInfo;
	}
	m_pRLInfo = NULL;
//	if( m_pRVInfo )
//	{
//		if( m_pRVInfo->pVORList )
//			delete[] m_pRVInfo->pVORList;
//		if( m_pRVInfo->pVIDs )
//			delete[] m_pRVInfo->pVIDs;
//		delete m_pRVInfo;
//	}
//	m_pRVInfo = NULL;

//	if( m_pVORList )
//		delete[] m_pVORList;
//	m_pVORList = NULL;		
//	if( m_pVIDs )
//		delete[] m_pVIDs;
//	m_pVIDs = NULL;		
}
void CGIS_NetworkRut::SetRutInfo( unsigned short nRID, BYTE byDirtect, BYTE byType, BYTE byCharge,
		BYTE byLNum,	BYTE bySpeed, BYTE byRLevel, BYTE byRType,
		float dRLength, unsigned short nVStart, unsigned short nVEnd, int nRBID )
{
	m_nRutID = nRID;		
	m_byDirection = byDirtect;	
	m_byType = byType;		
	m_byCharge = byCharge;		
//	m_byLaneNum = byLNum;		
//	m_bySpeed = bySpeed;		
	m_byRutLevel = byRLevel;	
	m_byRutType = byRType;	
	m_dRLength = dRLength;	
	m_nVtxStartID = nVStart;	
	m_nVtxEndID = nVEnd;
	m_nRBID = nRBID;
}
void CGIS_NetworkRut::SetRutLimitInfo( BYTE byLimit, unsigned short *pLRList )
{
	if( byLimit > 0 )
	{
		if( m_pRLInfo == NULL )
		{
			m_pRLInfo = new ST_GEO_RLIMIT;
			memset( m_pRLInfo, 0, sizeof(ST_GEO_RLIMIT) );
		}
		m_pRLInfo->byLimit = byLimit;
		m_pRLInfo->pLRList = new unsigned short[byLimit];
		memcpy( m_pRLInfo->pLRList, pLRList, sizeof(unsigned short)*byLimit );
	}
//	m_byLimit = byLimit;		
//	if( m_pLRList )
//		delete m_pLRList;
//	m_pLRList = NULL;
//	if( m_byLimit > 0 )
//	{
//		m_pLRList = new unsigned short[m_byLimit];
//		memcpy( m_pLRList, pLRList, sizeof(unsigned short)*m_byLimit );
//	}
}
void CGIS_NetworkRut::SetRutRLimitInfo( BYTE byLimit, unsigned short *pLRList )
{
	if( byLimit > 0 )
	{
		if( m_pRLInfo == NULL )
		{
			m_pRLInfo = new ST_GEO_RLIMIT;
			memset( m_pRLInfo, 0, sizeof(ST_GEO_RLIMIT) );
		}
		m_pRLInfo->byRLimit = byLimit;
		m_pRLInfo->pRLRList = new unsigned short[byLimit];
		memcpy( m_pRLInfo->pRLRList, pLRList, sizeof(unsigned short)*byLimit );
	}
//	m_byRLimit = byLimit;		
//	if( m_pRLRList )
//		delete[] m_pRLRList;
//	m_pRLRList = NULL;
//	if( m_byRLimit > 0 )
//	{
//		m_pRLRList = new unsigned short[m_byRLimit];
//		memcpy( m_pRLRList, pLRList, sizeof(unsigned short)*m_byRLimit );
//	}
}
void CGIS_NetworkRut::SetRutVirtualInfo( BYTE byVirtual, unsigned short *pVORList, int *pVIDs )
{
//	if( m_pRVInfo )
//	{
//		if( m_pRVInfo->pVORList )
//			delete[] m_pRVInfo->pVORList;
//		if( m_pRVInfo->pVIDs )
//			delete[] m_pRVInfo->pVIDs;
//		delete m_pRVInfo;
//	}
//	m_pRVInfo = NULL;
//	if( byVirtual > 0 )
//	{
//		m_pRVInfo = new ST_GEO_RVDRAFT;
//		m_pRVInfo->byVirtual = byVirtual;
//		m_pRVInfo->pVORList = new unsigned short[byVirtual];
//		memcpy( m_pRVInfo->pVORList, pVORList, sizeof(unsigned short)*byVirtual );
//		m_pRVInfo->pVIDs = new int[byVirtual];
//		memcpy( m_pRVInfo->pVIDs, pVIDs, sizeof(int)*byVirtual );
//	}
	
//	m_byVirtual = byVirtual;	
//	if( m_pVORList )
//		delete m_pVORList;
//	m_pVORList = NULL;		
//	if( m_pVIDs )
//		delete m_pVIDs;
//	m_pVIDs = NULL;
//	if( m_byVirtual > 0 )
//	{
//		m_pVORList = new unsigned short[m_byVirtual];
//		memcpy( m_pVORList, pVORList, sizeof(unsigned short)*m_byVirtual );
//		m_pVIDs = new int[m_byVirtual];
//		memcpy( m_pVIDs, pVIDs, sizeof(int)*m_byVirtual );
//	}
}

int CGIS_NetworkRut::GetRutID( )
{
	return m_nRutID;
}
BYTE CGIS_NetworkRut::GetRutDirection( )
{
	return m_byDirection;
}
BYTE CGIS_NetworkRut::GetRutType( )
{
	return m_byRutType;
}
BYTE CGIS_NetworkRut::GetType( )
{
	return m_byType;
}
BYTE CGIS_NetworkRut::GetRutLaneNum()
{
	return 0;
//	return m_byLaneNum;
}
BYTE CGIS_NetworkRut::GetRutCharge( )
{
	return m_byCharge;
}
BYTE CGIS_NetworkRut::GetRutSpeed( )
{
	return 0;
//	return m_bySpeed;
}
BYTE CGIS_NetworkRut::GetRutLevel( )
{
	return m_byRutLevel;
}
float CGIS_NetworkRut::GetRutLength( )
{
	return m_dRLength;
}
unsigned short CGIS_NetworkRut::GetRutStartVtxID( )
{
	return m_nVtxStartID;
}
unsigned short CGIS_NetworkRut::GetRutEndVtxID( )
{
	return m_nVtxEndID;
}
int CGIS_NetworkRut::GetRBreadthID( )
{
	return m_nRBID;
}
BYTE CGIS_NetworkRut::GetLimitRNum()
{
	if( m_pRLInfo )
		return m_pRLInfo->byLimit;
	return 0;
}
unsigned short* CGIS_NetworkRut::GetLimitRList( )
{
	if( m_pRLInfo )
		return m_pRLInfo->pLRList;
	return NULL;
}
BYTE CGIS_NetworkRut::GetRLimitRNum()
{
	if( m_pRLInfo )
		return m_pRLInfo->byRLimit;
	return 0;
}
unsigned short* CGIS_NetworkRut::GetRLimitRList( )
{
	if( m_pRLInfo )
		return m_pRLInfo->pRLRList;
	return NULL;
}
