
#ifndef _IGIS_GEOFILE_H
#define _IGIS_GEOFILE_H

#include "swBase.h"

class IGIS_GeoFile  
{
public:
	IGIS_GeoFile();
	virtual ~IGIS_GeoFile();

public:
	virtual BOOL SaveToFile( swString bstrFName ) = 0;
protected:
	//BOOL OpenMapFile( swString bstrFName, swString bstrMode, HANDLE &hFile, HANDLE &hFMap, char **pFileBuf, DWORD &dwStation, DWORD &dwSize );
	//BOOL CloseMapFile( char **pFileBuf, HANDLE &hFile, HANDLE &hFMap );
};

#endif 
