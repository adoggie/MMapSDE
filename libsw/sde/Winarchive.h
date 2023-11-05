
#ifndef _WINARCHIVE_H
#define _WINARCHIVE_H

#include "swBase.h"
#include "IArchive.h"

class CWinArchive : public IArchive  
{
public:
	CWinArchive();
	virtual ~CWinArchive();

public:
	ULONGLONG Seek( LONGLONG lOff, UINT nFrom );
	BOOL	Destroy( swString strFPath );
	void	ReleaseRef();
	BOOL	IsLoading() const;
	BOOL	IsStoring() const;
	BOOL	AddRef( swString strFPath,int nMode, int nID = 0 );
	void	Close();
	DWORD	GetFileSize();

private:
	virtual IArchive & Store( BYTE by );
	virtual IArchive & Store( WORD w );
	virtual IArchive & Store( LONG l );
	virtual IArchive & Store( DWORD dw );
	virtual IArchive & Store( float f );
	virtual IArchive & Store( double d );
	virtual IArchive & Store( int i );
	virtual IArchive & Store( short w );
	virtual IArchive & Store( char ch );
	virtual IArchive & Store( unsigned u );
	virtual IArchive & Store( swString str );
	
	virtual IArchive & Load( BYTE& by );
	virtual IArchive & Load( WORD& w );
	virtual IArchive & Load( LONG& l );
	virtual IArchive & Load( DWORD& dw );
	virtual IArchive & Load( float& f );
	virtual IArchive & Load( double& d );
	virtual IArchive & Load( int& i );
	virtual IArchive & Load( short& w );
	virtual IArchive & Load( char& ch );
	virtual IArchive & Load( unsigned& u );
	virtual IArchive & Load( swString& str );

private:
	CArchive	*	m_pArchive;
	CFile		m_file;
};

#endif 
