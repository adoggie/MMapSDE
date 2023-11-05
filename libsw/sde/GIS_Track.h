
#ifndef _GIS_TRACK_H
#define _GIS_TRACK_H

#include "swBase.h"

#include "GIS_TrackPoint.h"
#include "GIS_Feature.h"

class swEXPORT CGIS_Track : public CGIS_Feature
{
public:
	CGIS_Track( );
	virtual ~CGIS_Track();

public:
	void		SerializeTInfo( CArchive & ar );
	CString		GetTrackFile() const;
	void		SetTrackFileName( const CString & strTrackFileName );
	void		SetTrackID( const int & nID );
	BOOL		DeleteTrack( const CString & strTrackFile );
	BOOL		GetTrack( const CString & strTrackFile, CPtrList * pTrackList );
	BOOL		SaveTrack();
	void		SetTrackName( const CString & strTrackName );
	void		AddTrackPoint( CGIS_TrackPoint * trackPoint );

private:
	void		SerializeTrack( CArchive & ar );

private:
	int				m_ID;//����ID
	CPtrList	*	m_TrackPointLst;//����(�����б�)
	CString			m_strTrackName;//��������

	CString			m_strTrackFileName;
};

#endif 
