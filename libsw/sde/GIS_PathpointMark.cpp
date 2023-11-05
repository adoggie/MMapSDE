// GIS_PathpointMark.cpp: implementation of the CGIS_PathpointMark class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_PathpointMark.h"
#include "./MemoryMngLib/IMemoryMng.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_PathpointMark::CGIS_PathpointMark()
{
	m_enPathpointType = EN_PATHPOINTTYPE_INTERMEDIATE;

//	m_pVRut = new ST_VOICE_RUT;		//路段信息
//	m_pVNode = new ST_VOICE_NODE;
#ifdef MEMORY_SELF_MANAGE
	m_pVRut		= (ST_VOICE_RUT*)m_pMemMngOfBuddyData->newmalloc( sizeof(ST_VOICE_RUT));		//路段信息
	m_pVNode	= (ST_VOICE_NODE*)m_pMemMngOfBuddyData->newmalloc( sizeof(ST_VOICE_NODE));
#else
	m_pVRut		= (ST_VOICE_RUT*)malloc( sizeof(ST_VOICE_RUT));		//路段信息
	m_pVNode	= (ST_VOICE_NODE*)malloc( sizeof(ST_VOICE_NODE));  
#endif

	m_pVRut->Init(); 
	m_pVNode->Init();

}

CGIS_PathpointMark::~CGIS_PathpointMark()
{
#ifdef MEMORY_SELF_MANAGE
	if( m_pVRut )
	{
		m_pMemMngOfBuddyData->newfree(m_pVRut);
		m_pVRut = NULL;
	}
	if( m_pVNode )
	{
		m_pMemMngOfBuddyData->newfree(m_pVNode);
		m_pVNode = NULL;
	}
#else
	if( m_pVRut )
	{
		free(m_pVRut);
		m_pVRut = NULL;
	}
	if( m_pVNode )
	{
		free(m_pVNode);
		m_pVNode = NULL;
	}
#endif

	m_pWayPoint = NULL;
}
void CGIS_PathpointMark::SetWayPoint( CGIS_WayPoint* pWayPoint )
{
	m_pWayPoint = pWayPoint;
}
CGIS_WayPoint* CGIS_PathpointMark::GetWayPoint( )
{
	return m_pWayPoint;
}
ST_VOICE_RUT* CGIS_PathpointMark::GetVRutInfo( )
{
	return m_pVRut;
}
ST_VOICE_NODE* CGIS_PathpointMark::GetVNodeInfo( )
{
	ASSERT( m_pVNode );
	return m_pVNode;
}
