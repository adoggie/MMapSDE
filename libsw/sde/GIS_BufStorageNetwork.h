
#ifndef _GIS_BUFSTORAGE_NETWORK_H
#define _GIS_BUFSTORAGE_NETWORK_H


#include "swBase.h"
#include "GIS_BufStorage.h"
#include "GIS_BreadthMap.h"

class CGIS_BufStorageNetwork //: public CGIS_BufStorage  
{
public:
	CGIS_BufStorageNetwork();
	virtual ~CGIS_BufStorageNetwork();
//	virtual BOOL ClearBufStorage( );
	BOOL ClearBMapBufStorage( );
	BOOL ClearBNetBufStorage( );
	BOOL InsertBMap( CGIS_BreadthMap *pBMap );
	BOOL InsertBNet( CGIS_BreadthMap *pBMap );
private:
	CPtrList m_BMapList;
	CGIS_GeoMap *m_pMap;
};

#endif 
