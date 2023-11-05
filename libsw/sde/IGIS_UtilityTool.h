
#ifndef _IGIS_UTILITYTOOL_H
#define _IGIS_UTILITYTOOL_H

#include "swBase.h"

class IGIS_UtilityTool  
{
public:
	IGIS_UtilityTool();
	virtual ~IGIS_UtilityTool();
public:
	static void Compress(CString strFileName,UINT VER);
	static void UnCompress(CString strFileName);
	static void Lock();
	static void UnLockLon(double dbLon);
	static void UnLockLat(double dbLat);
private:
	static BOOL OpenMapFile( CString bstrFName,HANDLE &hFile, HANDLE &hFMap, char **pFileBuf, DWORD &dwStation, DWORD &dwSize );
	static BOOL CloseMapFile( char **pFileBuf, HANDLE &hFile, HANDLE &hFMap );
};

#endif
