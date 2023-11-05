

#ifndef _GIS_FEATURETRACK_H
#define _GIS_FEATURETRACK_H

#include "swBase.h"
#include "GIS_Feature.h"
#include "GIS_TrackPoint.h"

class swEXPORT CGIS_FeatureTrack : public CGIS_Feature  
{
public:
	CGIS_FeatureTrack( );
	CGIS_FeatureTrack( UINT nID );
	virtual ~CGIS_FeatureTrack();

public:
	virtual long GetFeatureSize();
	void SetTrackTrackID( UINT nID );
	int		GetPtNum();
	UINT	GetTrackFeatureID( );
	swString GetTrackFeatureName( );
	void	SetTrackFeatureName( swString strTName );
	CPtrList *GetTrackPointList( );
	void	AddTrackPoint( CGIS_TrackPoint * pTPoint );
	virtual CGIS_Feature* Clone();
private:
	UINT m_nID;
	swString m_strTName;
	CPtrList *m_pTPList;
};

#endif 
