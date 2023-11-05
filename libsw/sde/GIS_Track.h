
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
	int				m_ID;//º½¼£ID
	CPtrList	*	m_TrackPointLst;//º½¼£(º½µãÁÐ±í)
	CString			m_strTrackName;//º½¼£Ãû³Æ

	CString			m_strTrackFileName;
};

#endif 
