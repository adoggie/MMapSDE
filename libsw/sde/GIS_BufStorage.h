
#ifndef _GIS_BUFSTORAGE_H
#define _GIS_BUFSTORAGE_H


class CGIS_BufStorage  
{
public:
	CGIS_BufStorage();
	virtual ~CGIS_BufStorage();

	virtual BOOL ClearBufStorage( ) = 0;
	void SetBufStorageType( EnBufStorType enBSType );
	EnBufStorType GetBufStorageType( );
protected:
	EnBufStorType m_enBSType;
};

#endif 
