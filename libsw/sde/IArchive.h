
#ifndef _IARCHIVE_H
#define _IARCHIVE_H

#include "swBase.h"
class IArchive  
{
public:
	IArchive();
	virtual ~IArchive();

public:
	//public interface
	virtual BOOL AddRef( swString strFPath, int nMode, int nID = 0 ) = 0;
	virtual void ReleaseRef() =	0;
	virtual BOOL Destroy( swString strFPath ) = 0;

	virtual ULONGLONG Seek( LONGLONG lOff, UINT nFrom ) = 0;
	//overwrite operator
	template <class T> IArchive & operator <<( T t ){ return Store( t ); }
	template <class T> IArchive & operator >>( T& t ){ return Load( t ); }

	enum MODE{store = 0, load};

private:
	virtual IArchive & Store( BYTE by )		= 0;
	virtual IArchive & Store( WORD w )		= 0;
	virtual IArchive & Store( LONG l )		= 0;
	virtual IArchive & Store( DWORD dw )	= 0;
	virtual IArchive & Store( float f )		= 0;
	virtual IArchive & Store( double d )	= 0;
	virtual IArchive & Store( int i )		= 0;
	virtual IArchive & Store( short w )		= 0;
	virtual IArchive & Store( char ch )		= 0;
	virtual IArchive & Store( unsigned u )	= 0;
	virtual IArchive & Store( swString str ) = 0;

	virtual IArchive & Load( BYTE& by )		= 0;
	virtual IArchive & Load( WORD& w )		= 0;
	virtual IArchive & Load( LONG& l )		= 0;
	virtual IArchive & Load( DWORD& dw )	= 0;
	virtual IArchive & Load( float& f )		= 0;
	virtual IArchive & Load( double& d )	= 0;
	virtual IArchive & Load( int& i )		= 0;
	virtual IArchive & Load( short& w )		= 0;
	virtual IArchive & Load( char& ch )		= 0;
	virtual IArchive & Load( unsigned& u )	= 0;
	virtual IArchive & Load( swString& str ) = 0;
};

#endif 
