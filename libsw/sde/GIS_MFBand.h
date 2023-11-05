
#ifndef _GIS_MFBAND_H
#define _GIS_MFBAND_H

#include "swBase.h"
#include "IGIS_MapFile.h"





class CGIS_MFBand : public IGIS_MapFile  
{
public:
	CGIS_MFBand( swString bstrFName );
	virtual ~CGIS_MFBand();

public:
	virtual BOOL OpenMap( );
	virtual BOOL LoadMap( );
//	int LoadMaps( swString strFName, CPtrList *pMList );
	
	int LoadMaps( swString strFName, std::vector<shared_ptr<IGIS_Map> > & pMList );

	virtual BOOL SaveToFile( swString bstrFName );
private:
	BOOL EnCode( char* pCharCode , char* MyUUID,HANDLE hFile );
	BOOL UnCode( char* pCharCode , char* MyUUID );

};

#endif
