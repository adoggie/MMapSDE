// IGIS_NetWork.cpp: implementation of the IGIS_NetWork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IGIS_NetWork.h"

IMemoryMng* IGIS_NetWork::m_pMemMngOfBuddyData = NULL;

IGIS_NetWork::IGIS_NetWork():
    IGIS_SDETool(EN_SDETOOLTYPE_NETWORK)
{
	m_pNetworkRoute = NULL;
	m_pMapManager = NULL;
#ifdef ANALYSE_DATA
    m_dwMaxHeapUsedSize = 0;
#endif
}

IGIS_NetWork::~IGIS_NetWork()
{

}
void IGIS_NetWork::SetNetworkRoute( CGIS_NetworkRoute *pRoute )
{
	m_pNetworkRoute = pRoute;
}
CGIS_NetworkRoute* IGIS_NetWork::GetNetworkRoute( )
{
	return m_pNetworkRoute;
}
void IGIS_NetWork::SetMapManager( IGIS_MapManager *pMapManager )
{
	m_pMapManager = pMapManager;
}
