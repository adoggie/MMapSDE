
#ifndef _GIS_WAYPOINT_MARK_H
#define _GIS_WAYPOINT_MARK_H

#include "GIS_WayPoint.h"
#include "swBase.h"

class CGIS_WayPointMark : public CGIS_WayPoint
{
public:
	CGIS_WayPointMark();
	virtual ~CGIS_WayPointMark();
	EnWaypointMarkType GetWPMarkType();
	void SetWPMarkType(EnWaypointMarkType nType);
	unsigned short GetMapID();
	void SetMapID(unsigned short nMapID);
private:
	EnWaypointMarkType m_nType; 
	unsigned short m_nMapID;

};

#endif 
