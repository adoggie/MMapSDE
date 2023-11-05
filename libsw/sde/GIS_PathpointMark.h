
#ifndef _GIS_PATHPOINT_MARK_H
#define _GIS_PATHPOINT_MARK_H

#include "swBase.h"

#include "GIS_WayPoint.h"
#include "GIS_Pathpoint.h"

class swEXPORT CGIS_PathpointMark  : public CGIS_Pathpoint
{
public:
	CGIS_PathpointMark();
	virtual ~CGIS_PathpointMark();

	void SetWayPoint( CGIS_WayPoint* pWayPoint );
	CGIS_WayPoint* GetWayPoint( );
	ST_VOICE_RUT* GetVRutInfo( );
	ST_VOICE_NODE* GetVNodeInfo( );

private:
	int m_nNextRutID;	//����·�ε�ID
	CGIS_WayPoint *m_pWayPoint;	//Ŀ�����Ϣ
	ST_VOICE_RUT  *m_pVRut;		//·����Ϣ
	ST_VOICE_NODE *m_pVNode;
	
};

#endif
