
#include "GIS_TrackPoint.h"

CGIS_TrackPoint::CGIS_TrackPoint(){
	m_Time			=	0.0;
	m_Velocity		=	0.0f;
	m_Orientation	=	0;
}

CGIS_TrackPoint::~CGIS_TrackPoint()
{

}

void CGIS_TrackPoint::SetCollectionTime(const double & dbTime)
{
	m_Time = dbTime;
} 

double CGIS_TrackPoint::GetCollectionTime() const
{
	return m_Time;
}

void CGIS_TrackPoint::SetOrientation(const short & orientation)
{
	m_Orientation = orientation;
}

short CGIS_TrackPoint::GetOrientation() const
{
	return m_Orientation;
}

void CGIS_TrackPoint::SetLocation( const ST_GEO_PTXY & location )
{
	m_Location = location;
}

ST_GEO_PTXY& CGIS_TrackPoint::GetLocation()
{
	return m_Location;
}
