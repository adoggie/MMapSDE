// GIS_FeatureTrack.cpp: implementation of the CGIS_FeatureTrack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_FeatureTrack.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGIS_FeatureTrack::CGIS_FeatureTrack( )
	:CGIS_Feature( EN_GOTYPE_TRACK )
{
	m_pTPList = new CPtrList;
	m_nID = 0;
}

CGIS_FeatureTrack::CGIS_FeatureTrack( UINT nID )
	:CGIS_Feature( EN_GOTYPE_TRACK )
{
	m_pTPList = new CPtrList;
	m_nID = nID;
}

CGIS_FeatureTrack::~CGIS_FeatureTrack()
{
	if ( m_pTPList )
	{
		CGIS_TrackPoint * pTPoint;
		POSITION pos = m_pTPList->GetHeadPosition();
		while ( pos )
		{
			pTPoint = (CGIS_TrackPoint *)m_pTPList->GetNext( pos );
			if ( pTPoint )
				delete pTPoint;
		}
		m_pTPList->RemoveAll();
		delete m_pTPList;
	}	
}
UINT CGIS_FeatureTrack::GetTrackFeatureID( )
{
	return m_nID;
}
CPtrList *CGIS_FeatureTrack::GetTrackPointList( )
{
	return m_pTPList;
}
swString CGIS_FeatureTrack::GetTrackFeatureName( )
{
	return m_strTName;
}
void CGIS_FeatureTrack::SetTrackFeatureName( swString strTName )
{
	m_strTName = strTName;
}

int CGIS_FeatureTrack::GetPtNum()
{
	return m_pTPList->GetCount();
}

void CGIS_FeatureTrack::AddTrackPoint( CGIS_TrackPoint * pTPoint )
{
	ASSERT( pTPoint );

	m_pTPList->AddTail( pTPoint );
}

CGIS_Feature* CGIS_FeatureTrack::Clone()
{
	int nID = GetTrackFeatureID();
	CGIS_FeatureTrack * pNewTrack = new CGIS_FeatureTrack( nID );

	pNewTrack->SetTrackFeatureName( GetTrackFeatureName() );

	return pNewTrack;
}

void CGIS_FeatureTrack::SetTrackTrackID(UINT nID)
{
	m_nID = nID;
}

long CGIS_FeatureTrack::GetFeatureSize()
{
	int nSize = 0;

	nSize += sizeof( m_nID );
	nSize += m_strTName.GetLength();

	POSITION pos = m_pTPList->GetHeadPosition();
	while ( pos )
	{
		CGIS_TrackPoint * pTrack = (CGIS_TrackPoint *)m_pTPList->GetNext( pos );
		if ( pTrack )
		{
			nSize += sizeof( ST_GEO_PTXY );	//pTrack->GetLocation()
			nSize += sizeof( double );		//pTrack->GetCollectionTime()
			nSize += sizeof( short );		//pTrack->GetOrientation()
//			nSize += sizeof( float );		//pTrack->m_Velocity
		}
	}

	return nSize;
}
