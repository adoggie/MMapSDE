// GIS_Track.cpp: implementation of the CGIS_Track class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sdelib.h"
#include "GIS_Track.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_Track::CGIS_Track():CGIS_Feature(EN_GOTYPE_ROUTE)
{
	m_ID				=	0;
	m_TrackPointLst		=	new CPtrList;
	m_strTrackName		=	"";
}

CGIS_Track::~CGIS_Track()
{
	if( m_TrackPointLst )
	{
		CGIS_TrackPoint *	pTPoint;

		POSITION pos = m_TrackPointLst->GetHeadPosition();
		while( pos )
		{
			pTPoint = (CGIS_TrackPoint*)m_TrackPointLst->GetNext( pos );
			if( pTPoint )
			{
				delete pTPoint;
			}
		}

		m_TrackPointLst->RemoveAll();
		
		delete m_TrackPointLst;
	}
}

void CGIS_Track::AddTrackPoint( CGIS_TrackPoint *trackPoint )
{
	ASSERT( trackPoint );

	m_TrackPointLst->AddTail( trackPoint );
}

void CGIS_Track::SetTrackName(const CString &strTrackName)
{
	m_strTrackName = strTrackName;
}

//保存一个TRACK
BOOL CGIS_Track::SaveTrack()
{
	CFile	file;
	BOOL	bOpen = file.Open( m_strTrackFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary );

	if ( !bOpen )
	{
		return FALSE;
	}
	
	CArchive	ar( &file, CArchive::store );
	SerializeTrack( ar );

	ar.Close();
	file.Close();

	return TRUE;
}

BOOL CGIS_Track::GetTrack( const CString & strTrackFile, CPtrList * pTrackList )
{
	CFile	file;
	BOOL	bOpen = file.Open( strTrackFile, CFile::modeRead | CFile::typeBinary );

	if ( !bOpen )
	{
		return FALSE;
	}

	CArchive	ar( &file, CArchive::load );
	SerializeTrack( ar );

	pTrackList = m_TrackPointLst;

	ar.Close();
	file.Close();

	return TRUE;
}

BOOL CGIS_Track::DeleteTrack( const CString & strTrackFile )
{
	if ( !DeleteFile( strTrackFile ) )
	{
		return FALSE;
	}

	return TRUE;
}

void CGIS_Track::SetTrackID(const int &nID)
{
	ASSERT( nID >= 0 );

	m_ID = nID;
}

void CGIS_Track::SetTrackFileName(const CString &strTrackFileName)
{
	m_strTrackFileName = strTrackFileName;
}

CString CGIS_Track::GetTrackFile() const
{
	return m_strTrackFileName;
}

//序列化航线(Trackpoint)
void CGIS_Track::SerializeTrack(CArchive &ar)
{
	POSITION	pos = m_TrackPointLst->GetHeadPosition();
	while ( pos )
	{
		CGIS_TrackPoint * pTrackPoint = (CGIS_TrackPoint *)m_TrackPointLst->GetNext( pos );
		pTrackPoint->Serialize( ar );
	}

	m_TrackPointLst->Serialize( ar );
	
}

//序列化航线信息
void CGIS_Track::SerializeTInfo(CArchive &ar)
{
	if ( ar.IsStoring() )
	{
		ar<<m_ID<<m_strTrackFileName<<m_strTrackName;
	}
	else
	{
		ar>>m_ID>>m_strTrackFileName>>m_strTrackName;
	}
}
