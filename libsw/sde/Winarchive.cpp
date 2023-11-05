// Winarchive.cpp: implementation of the CWinArchive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sdelib.h"
#include "Winarchive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWinArchive::CWinArchive()
{
	m_pArchive= NULL;
}

CWinArchive::~CWinArchive()
{
	if ( m_pArchive)
	{
		delete m_pArchive;
		m_pArchive= NULL;
	}
}

void CWinArchive::Close()
{
	m_pArchive->Close();
}

BOOL CWinArchive::AddRef( swString strFPath,int nMode, int nID /*= 0*/ )
{
	swString		strFilePath;
	if ( nID > 0 )
	{
		strFilePath.Format( L"%s\\%d.dat", strFPath, nID );
	}
	else
	{
		strFilePath = strFPath;
	}

	BOOL	bOpen = FALSE;
	if ( nMode == IArchive::store )//存储
	{
		UINT nOpenFlag = 0;
		//打开数据文件，(航迹)不需要把文件清0
		if ( nID > 0 )
		{
			nOpenFlag = CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite;
		}
		else
			nOpenFlag = CFile::modeCreate | CFile::modeWrite;

		bOpen = m_file.Open( strFilePath, nOpenFlag );
	}
	else//读取
	{
		bOpen = m_file.Open( strFilePath, CFile::modeRead);
		if( bOpen )
		{
			//文件为空
			if( m_file.GetLength() <= 0 )
			{
				m_file.Close();
				return FALSE;
			}
		}
	}

	if ( !bOpen )
	{
		TRACE1( "The error code is %d\n", GetLastError() );
		return FALSE;
	}
	
	m_pArchive= new CArchive( &m_file, nMode );

	return TRUE;
}

BOOL CWinArchive::IsStoring() const
{
	return m_pArchive->IsStoring();
}

BOOL CWinArchive::IsLoading() const
{
	return m_pArchive->IsLoading();
}

void CWinArchive::ReleaseRef()
{
	Close();
	m_file.Close();

	if ( m_pArchive)
	{
		delete m_pArchive;
		m_pArchive= NULL;
	}
}

BOOL CWinArchive::Destroy( swString strFPath )
{
	return DeleteFile( strFPath );
}

IArchive & CWinArchive::Store( BYTE by )
{
	*m_pArchive<<by;
	return *this;
}

IArchive & CWinArchive::Store( WORD w  )
{
	*m_pArchive<<w;
	return *this;
}

IArchive & CWinArchive::Store( LONG l )
{
	*m_pArchive<<l;
	return *this;
}

IArchive & CWinArchive::Store( DWORD dw )
{
	*m_pArchive<<dw;
	return *this;
}

IArchive & CWinArchive::Store( float f )
{
	*m_pArchive<<f;
	return *this;
}

IArchive & CWinArchive::Store( double d )
{
	*m_pArchive<<d;
	return *this;
}

IArchive & CWinArchive::Store( int i )
{
	*m_pArchive<<i;
	return *this;
}

IArchive & CWinArchive::Store( short w )
{
	*m_pArchive<<w;
	return *this;
}

IArchive & CWinArchive::Store( char ch )
{
	*m_pArchive<<ch;
	return *this;
}

IArchive & CWinArchive::Store( unsigned u )
{
	*m_pArchive<<u;
	return *this;
}

IArchive & CWinArchive::Store( swString str )
{
	*m_pArchive<<str;
	return *this;
}

IArchive & CWinArchive::Load( BYTE& by )
{
	*m_pArchive>>by;
	return *this;
}

IArchive & CWinArchive::Load( WORD& w  )
{
	*m_pArchive>>w;
	return *this;
}

IArchive & CWinArchive::Load( LONG& l )
{
	*m_pArchive>>l;
	return *this;
}

IArchive & CWinArchive::Load( DWORD& dw )
{
	*m_pArchive>>dw;
	return *this;
}

IArchive & CWinArchive::Load( float &f )
{
	*m_pArchive>>f;
	return *this;
}

IArchive & CWinArchive::Load( double& d )
{
	*m_pArchive>>d;
	return *this;
}

IArchive & CWinArchive::Load( int& i )
{
	*m_pArchive>>i;
	return *this;
}

IArchive & CWinArchive::Load( short& w )
{
	*m_pArchive>>w;
	return *this;
}

IArchive & CWinArchive::Load( char& ch )
{
	*m_pArchive>>ch;
	return *this;
}

IArchive & CWinArchive::Load( unsigned& u )
{
	*m_pArchive>>u;
	return *this;
}

IArchive & CWinArchive::Load( swString& str )
{
	*m_pArchive>>str;
	return *this;
}

ULONGLONG CWinArchive::Seek( LONGLONG lOff, UINT nFrom )
{
	ASSERT( m_file.m_hFile );
	
	return m_file.Seek( lOff, nFrom );
}

DWORD CWinArchive::GetFileSize()
{
	ASSERT( m_file.m_hFile );

	return m_file.GetLength();
}