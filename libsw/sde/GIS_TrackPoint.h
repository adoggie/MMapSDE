
#ifndef _GIS_TRACKPOINT_H
#define _GIS_TRACKPOINT_H

#include "swBase.h"

#include "GeoGlobe.h"

//¹ì¼£µã
class swEXPORT CGIS_TrackPoint  
{
public:
	CGIS_TrackPoint();
	virtual ~CGIS_TrackPoint();

public:
	ST_GEO_PTXY& GetLocation();
	void		SetLocation( const ST_GEO_PTXY & location );
	short		GetOrientation() const;
	void		SetOrientation( const short &orientation );
	double		GetCollectionTime() const;
	void		SetCollectionTime( const double & dbTime );	

private:
	double		m_Time;			
	float			m_Velocity;	 //ËÙ¶È
	short			m_Orientation;
	ST_GEO_PTXY	m_Location;

};

#endif
