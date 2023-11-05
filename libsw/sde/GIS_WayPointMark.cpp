// GIS_WayPointMark.cpp: implementation of the CGIS_WayPointMark class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_WayPointMark.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_WayPointMark::CGIS_WayPointMark()
{

}

CGIS_WayPointMark::~CGIS_WayPointMark()
{

}
EnWaypointMarkType CGIS_WayPointMark::GetWPMarkType()
{
	return m_nType;
}
void CGIS_WayPointMark::SetWPMarkType(EnWaypointMarkType nType)
{
	m_nType = nType;
}
unsigned short CGIS_WayPointMark::GetMapID()
{
	return m_nMapID;
}
void CGIS_WayPointMark::SetMapID(unsigned short nMapID)
{
	m_nMapID = nMapID;
}
