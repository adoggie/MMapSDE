// IGIS_UtilityTool.cpp: implementation of the IGIS_UtilityTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IGIS_UtilityTool.h"
#include ".\ZLib\Zlib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IGIS_UtilityTool::IGIS_UtilityTool()
{

}

IGIS_UtilityTool::~IGIS_UtilityTool()
{

}
void IGIS_UtilityTool::Compress(CString strFileName,UINT VER)
{
	HANDLE hFile, hMFile;
	char *pFileBuf;
	DWORD dwStation, dwSize;
//	HANDLE hFile1, hMFile1;
	char *pFileBuf1;
//	DWORD dwStation1, dwSize1, dwCount;
	DWORD dwCount;
	
	if(OpenMapFile( strFileName, hFile, hMFile, &pFileBuf, dwStation, dwSize ))
	{
		pFileBuf1 = new char[dwSize];
		CString FileName = strFileName + L"z";
		CFile file(FileName, CFile::modeWrite | CFile::modeCreate);
		compress2( (unsigned char*)pFileBuf1, &(dwCount), (unsigned char*)pFileBuf, dwSize, Z_DEFAULT_COMPRESSION );
//		uncompress((unsigned char*)(pFileBuf2), &dwCount, (unsigned char*)(pFileBuf), dwSize );
		file.Write(&VER,sizeof(VER));
		file.Write(&dwSize,sizeof(dwSize));
		file.Write(pFileBuf1,dwCount);
		CloseMapFile( &pFileBuf, hFile, hMFile );
		file.Close();
	}
}
BOOL IGIS_UtilityTool::OpenMapFile( CString bstrFName,HANDLE &hFile, HANDLE &hFMap, char **pFileBuf, DWORD &dwStation, DWORD &dwSize )
{
	dwStation = 0;
	DWORD dwFileSize;
	
	hFile = CreateFileForMapping(bstrFName, GENERIC_READ, FILE_SHARE_READ, NULL, 
					OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 0);

	if( INVALID_HANDLE_VALUE != hFile )	// 句柄有效
	{
		dwFileSize = GetFileSize( hFile, NULL );
		hFMap = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, 0);

		if( hFMap != NULL )
		{
			*pFileBuf = (char*)MapViewOfFile( hFMap, FILE_MAP_READ, 0, 0, 0);
			if( pFileBuf != NULL)
			{
				dwSize = dwFileSize;
				return TRUE;
			}
			else
			{
				CloseHandle( hFMap );
				hFMap = NULL;
			}
		}

		CloseHandle( hFile );
		hFile = NULL;
	}
	else
	{
		return FALSE;
	}
	return FALSE;
}
BOOL IGIS_UtilityTool::CloseMapFile( char **pFileBuf, HANDLE &hFile, HANDLE &hFMap )
{
	if( hFMap != NULL )
	{
		UnmapViewOfFile( *pFileBuf );
		// hansom 此处可能会有问题
		*pFileBuf = NULL;
		CloseHandle( hFMap );
	}
	hFMap = NULL;
	if( hFile != NULL )
	{
		CloseHandle( hFile );
		if( *pFileBuf != NULL )
		{
			delete [] *pFileBuf;
			*pFileBuf = NULL;
		}
	}
	hFile = NULL;
	return TRUE;
}
void IGIS_UtilityTool::UnCompress(CString strFileName)
{
	HANDLE hFile, hMFile;
	char *pFileBuf;
	DWORD dwStation, dwSize;
//	HANDLE hFile1, hMFile1;
	char *pFileBuf1;
//	DWORD dwStation1, dwSize1, dwCount;
	DWORD dwCount;
		
	if(OpenMapFile( strFileName, hFile, hMFile, &pFileBuf, dwStation, dwSize ))
	{
		UINT VER;
		DWORD BufSize;
		memcpy(&VER, pFileBuf, sizeof(UINT));
		memcpy(&BufSize,pFileBuf+sizeof(UINT),sizeof(DWORD));
		pFileBuf1 = new char[BufSize];
		CString FileName = strFileName + L"2";
		CFile file(FileName, CFile::modeWrite | CFile::modeCreate);
	//	compress2( (unsigned char*)pFileBuf1, &(dwCount), (unsigned char*)pFileBuf, dwSize, Z_DEFAULT_COMPRESSION );
		uncompress((unsigned char*)(pFileBuf1), &dwCount, (unsigned char*)(pFileBuf + sizeof(UINT) + sizeof(DWORD)), dwSize );
		file.Write(pFileBuf1,dwCount);
		CloseMapFile( &pFileBuf, hFile, hMFile );
		file.Close();
	}
}