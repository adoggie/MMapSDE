// GIS_MFBand.cpp: implementation of the CGIS_MFBand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_MFBand.h"
#include "GIS_GeoMap.h"
#include "GIS_LayerManager.h"
#include "GIS_UserLayer.h"
#include "GIS_MapLayer.h"
#include ".\ZLib\zlib.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 地图文件头部数据信息结构体
int MAP_ORIGIN_BID = 444624;
int MAP_ORIGIN_XNUM = 372;
int MAP_ORIGIN_YNUM = 354;
double MAP_ORIGIN_CENTERX = 106.5625;
double MAP_ORIGIN_CENTERY = 29.54166;
#ifdef _WIN32_WCE
extern "C" __declspec(dllimport) BOOL KernelIoControl( DWORD dwIoControlCode, 
				 LPVOID lpInBuf, 
				 DWORD nInBufSize, 
				 LPVOID lpOutBuf, 
				 DWORD nOutBufSize, 
				 LPDWORD lpBytesReturned);

#define IOCTL_HAL_GET_DEVICEID CTL_CODE(FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS) 

#define IOCTL_HAL_GET_UUID     CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif
CGIS_MFBand::CGIS_MFBand(swString bstrFName)
{
	m_bstrFName = bstrFName;
}

CGIS_MFBand::~CGIS_MFBand()
{

}
/*
// 读取文件数据
size_t CGIS_MFBand::Fread(char *pFileBuf,DWORD dwSize,DWORD &dwStation,void *buffer, size_t size, size_t count)
{
	if(pFileBuf == NULL)
	{
		return 0;
	}

	int nNum = size * count;
	int nRtn;
	if(dwStation + nNum > dwSize)
	{
		nRtn = dwSize - dwStation;
	}
	else
	{
		nRtn = nNum;
	}

	memcpy(buffer, pFileBuf + dwStation, nRtn);
	dwStation += nRtn;

	return nRtn / size;
}*/
BOOL CGIS_MFBand::SaveToFile( swString bstrFName )
{
	return FALSE;
}
BOOL CGIS_MFBand::OpenMap( )
{
	return TRUE;
}
BOOL CGIS_MFBand::LoadMap( )
{
//	HANDLE hFile = CreateFile(m_bstrFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
//					FILE_ATTRIBUTE_ARCHIVE, NULL);
//	if(INVALID_HANDLE_VALUE == hFile)
//		return FALSE;
//
//	DWORD dwCSize = GetFileSize( hFile, NULL );
//	DWORD dwCount = 0;
//	char *pCFBuf = new char[dwCSize];
//	if( pCFBuf == 0 )
//		ASSERT(FALSE);
//	ReadFile( hFile, pCFBuf, dwCSize, &dwCount, NULL);
//	if( dwCSize > dwCount )
//	{
//		delete[] pCFBuf;
//		CloseHandle( hFile );
//		return FALSE;
//	}
//	dwCount = 0;
//	int nUnCom = 5;
//	Fread( pCFBuf, dwCSize, dwCount, &nUnCom, sizeof(int), 1 );
//	int nCom = dwCSize - sizeof( int );
//	char *pFBuf = new char[nUnCom];
//	if( pFBuf == 0 )
//		ASSERT(FALSE);
//	char *pBuf = new char[nCom];
//	if( pBuf == 0 )
//		ASSERT(FALSE);
//	Fread( pCFBuf, dwCSize, dwCount, pBuf, nCom, 1 );
//	DWORD dwSize = nUnCom;
//	uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nCom );
//	delete [] pCFBuf;
//	delete [] pBuf;
//
///*	DWORD dwSize = GetFileSize( hFile, NULL );
//	DWORD dwCount = 0;
//	char *pFBuf = new char[dwSize];
//	ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
//	if( dwSize > dwCount )
//		return FALSE;
//*/
//	DWORD dwStation = 0;
//	char szInfo[32];
//	Fread( pFBuf, dwSize, dwStation, szInfo, 6, 1 );
//	if( swString(szInfo).Find( L"INX V1" ) < 0 )
//	{
//		delete[] pFBuf;
//		CloseHandle( hFile );
//		return FALSE;
//	}
//	CGIS_GeoMap *pMap = (CGIS_GeoMap*)m_pMap;
//	int n0 = m_bstrFName.ReverseFind(L'\\');
//	swString str = m_bstrFName.Left( n0+1 );
//	pMap->GetMapInfo()->SetMapFilePath( str );
//
//	int nNum = 0;
//	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//	Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
//	if( nNum )
//		szInfo[nNum] = '\0';
//	pMap->GetMapInfo()->SetMapName( szInfo );
//	if( swString(szInfo).Find(L"中国地图") >= 0 )
//		pMap->GetMapInfo()->SetMapType( EN_MAPTYPE_NATION );
//	
//	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//	pMap->GetMapInfo()->SetMapID(nNum);
//
//	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//	pMap->GetMapInfo()->SetMapBndPtNum( nNum );
//	if( nNum != 0 )
//	{
//		ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
//		Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
//		pMap->GetMapInfo()->SetMapBndList( pBndPtList );
//	}
//	
//	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//	pMap->GetMapInfo()->SetMapLayNum(nNum);
//	
//	BYTE byNum = 0;
//	float dbNum = 0.0;
//	CGIS_LayerManager *pLManager = new CGIS_LayerManager();
//	for( int i=0;	i< pMap->GetMapInfo()->GetMapLayNum();	i++ )
//	{
//		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
//		CGIS_LayerInfo *pLInfo = new CGIS_LayerInfo( byNum );
//		ST_GEO_PEN pen;
//		ST_GEO_BRUSH brush;
//		Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
//		Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
//		Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
//		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//		pen.nStyle = nNum;
//		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//		brush.nStyle = nNum;
//		pLInfo->SetPen( pen );
//		pLInfo->SetBrush( brush );
//		///图层的显示
//		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
//		pLInfo->SetLayerState( byNum );
//		//图层的scale范围
//		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		pLInfo->SetMinGRScale(dbNum);
//		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		pLInfo->SetMaxGRScale(dbNum);
//		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		pLInfo->SetMinARScale(dbNum);
//		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
//		pLInfo->SetMaxARScale(dbNum);
//		///
//		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//		pLInfo->SetIconID(nNum);
//		
//		CGIS_MapLayer *pLayer = new CGIS_MapLayer( pLInfo );
//		pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
//	}
////	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//	BYTE nBMapID = 0;
//	ST_GEO_RECT rtBreadth;
//	Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(BYTE), 1 );
//	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
//	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
//	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
//	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );
//	pMap->GetMapInfo()->SetMapRect( rtBreadth );
//	CGIS_BreadthMap *pBMapList[200];
//	unsigned short nBMapNum = 0;
//	while( dwStation < dwSize )
//	{
//		CGIS_BreadthMap *pBMap = new CGIS_BreadthMap(pMap);
//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_nBreadthID, sizeof(BYTE), 1 );
//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dTop, sizeof(float), 1 );
//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dBottom, sizeof(float), 1 );
//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dLeft, sizeof(float), 1 );
//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dRight, sizeof(float), 1 );
////		pMap->GetBreadthMapList()->AddTail( pBMap );
//		pBMapList[nBMapNum] = pBMap;
//		nBMapNum++;
//		/////init breadthmap layer object list;
//		POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
//		unsigned short    nKey;
//		IGIS_Layer* pLay = NULL;
//		while( pos != NULL )
//		{
//			pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
//			if( pLay )
//				pLay->InitObjectListMap( pBMap->m_nBreadthID );
//		}
//	}
//	pMap->SetBreadthMapNum( nBMapNum );
//	pMap->SetBreadthMapList( pBMapList );
//	pMap->m_LayerManagerSet->AddTail( pLManager );
//	delete[] pFBuf;
//	CloseHandle( hFile );
//	return TRUE;
/*	HANDLE hFile = CreateFile(m_bstrFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_ARCHIVE, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwCSize = GetFileSize( hFile, NULL );
	DWORD dwCount = 0;
	char *pCFBuf = new char[dwCSize];
	if( pCFBuf == 0 )
		ASSERT(FALSE);
	ReadFile( hFile, pCFBuf, dwCSize, &dwCount, NULL);
	if( dwCSize > dwCount )
	{
		delete[] pCFBuf;
		CloseHandle( hFile );
		return FALSE;
	}
	dwCount = 0;
	int nUnCom = 5;
	Fread( pCFBuf, dwCSize, dwCount, &nUnCom, sizeof(int), 1 );
	int nCom = dwCSize - sizeof( int );
	char *pFBuf = new char[nUnCom];
	if( pFBuf == 0 )
		ASSERT(FALSE);
	char *pBuf = new char[nCom];
	if( pBuf == 0 )
		ASSERT(FALSE);
	Fread( pCFBuf, dwCSize, dwCount, pBuf, nCom, 1 );
	DWORD dwSize = nUnCom;
	uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nCom );
	delete [] pCFBuf;
	delete [] pBuf;
*/
/*	DWORD dwSize = GetFileSize( hFile, NULL );
	DWORD dwCount = 0;
	char *pFBuf = new char[dwSize];
	ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
	if( dwSize > dwCount )
		return FALSE;
*/
/*	DWORD dwStation = 0;
	char szInfo[32];
	Fread( pFBuf, dwSize, dwStation, szInfo, 6, 1 );
	if( swString(szInfo).Find( L"INX V1" ) < 0 )
	{
		delete[] pFBuf;
		CloseHandle( hFile );
		return FALSE;
	}
	CGIS_GeoMap *pMap = (CGIS_GeoMap*)m_pMap;
	int n0 = m_bstrFName.ReverseFind(L'\\');
	swString str = m_bstrFName.Left( n0+1 );
	pMap->GetMapInfo()->SetMapFilePath( str );

	int nNum = 0;
	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
	if( nNum )
		szInfo[nNum] = '\0';
	pMap->GetMapInfo()->SetMapName( szInfo );
	if( swString(szInfo).Find(L"中国地图") >= 0 )
		pMap->GetMapInfo()->SetMapType( EN_MAPTYPE_NATION );
	
	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapID(nNum);

	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapBndPtNum( nNum );
	if( nNum != 0 )
	{
		ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
		Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
		pMap->GetMapInfo()->SetMapBndList( pBndPtList );
	}
	
	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	pMap->GetMapInfo()->SetMapLayNum(nNum);
	
	BYTE byNum = 0;
	float dbNum = 0.0;
	CGIS_MapLayer *pLList[25];
	int nLLNum = pMap->GetMapInfo()->GetMapLayNum();
	CGIS_LayerManager *pLManager = new CGIS_LayerManager();
	for( int i=0;	i< pMap->GetMapInfo()->GetMapLayNum();	i++ )
	{
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		CGIS_LayerInfo *pLInfo = new CGIS_LayerInfo( byNum );
		ST_GEO_PEN pen;
		ST_GEO_BRUSH brush;
		Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pen.nStyle = nNum;
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		brush.nStyle = nNum;
		pLInfo->SetPen( pen );
		pLInfo->SetBrush( brush );
		///图层的显示
		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
		pLInfo->SetLayerState( byNum );
		//图层的scale范围
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
		pLInfo->SetMinGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
		pLInfo->SetMaxGRScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
		pLInfo->SetMinARScale(dbNum);
		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
		pLInfo->SetMaxARScale(dbNum);
		///
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pLInfo->SetIconID(nNum);
		
		CGIS_MapLayer *pLayer = new CGIS_MapLayer( pLInfo );
		pLList[i] = pLayer;
		pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
	}
//	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
	BYTE nBMapID = 0;
	ST_GEO_RECT rtBreadth;
	Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(BYTE), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );
	pMap->GetMapInfo()->SetMapRect( rtBreadth );
	CGIS_BreadthMap *pBMapList[200];
	unsigned short nBMapNum = 0;
	unsigned short nBMapMaxID = 0;
	while( dwStation < dwSize )
	{
		CGIS_BreadthMap *pBMap = new CGIS_BreadthMap(pMap);
		Fread( pFBuf, dwSize, dwStation, &pBMap->m_nBreadthID, sizeof(BYTE), 1 );
		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dTop, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dBottom, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dLeft, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dRight, sizeof(float), 1 );
//		pMap->GetBreadthMapList()->AddTail( pBMap );
		pBMapList[nBMapNum] = pBMap;
		nBMapNum++;
		if( pBMap->m_nBreadthID > nBMapMaxID )
			nBMapMaxID = pBMap->m_nBreadthID;
		/////init breadthmap layer object list;
		POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
		unsigned short    nKey;
		IGIS_Layer* pLay = NULL;
		while( pos != NULL )
		{
			pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
			if( pLay )
				pLay->InitObjectListMap( pBMap->m_nBreadthID );
		}
	}
	pMap->SetBreadthMapNum( nBMapNum );
	pMap->SetBreadthMapList( pBMapList );
	POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
	unsigned short    nKey;
	IGIS_Layer* pLay = NULL;
//	CGIS_LFBand *pFile = new CGIS_LFBand;
	while( pos != NULL )
	{
		pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
		if( pLay )
		{
			((CGIS_MapLayer*)pLay)->InitLBCount( nBMapMaxID );
		}
	}

//	for( i=0;	i< nBMapNum;	i++ )
//	{
//		pMap->LoadBLData( pBMapList[i], pLList, nLLNum );
//		for( int j=0;	j<nLLNum;	j++ )
//		{
//			pLList[j]->IncreaseLCount( pBMapList[i]->m_nBreadthID );
//			pLList[j]->DecreaseLCount( pBMapList[i]->m_nBreadthID );
//		}
//	}
	pMap->m_LayerManagerSet->AddTail( pLManager );
	delete[] pFBuf;
	CloseHandle( hFile );
*/	return TRUE;
}
//#include "GIS_MapBreadthNetwork.h"
//BOOL CGIS_MFBand::LoadMaps( swString strFName, CPtrList *pMList, CPtrList *pCMList )
//{
//	HANDLE hFile = CreateFile(strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
//					FILE_ATTRIBUTE_ARCHIVE, NULL);
//	if(INVALID_HANDLE_VALUE == hFile)
//		return FALSE;
//
////	DWORD dwCSize = GetFileSize( hFile, NULL );
////	DWORD dwCount = 0;
////	char *pCFBuf = new char[dwCSize];
////	if( pCFBuf == 0 )
////		ASSERT(FALSE);
////	ReadFile( hFile, pCFBuf, dwCSize, &dwCount, NULL);
////	if( dwCSize > dwCount )
////	{
////		delete[] pCFBuf;
////		CloseHandle( hFile );
////		return FALSE;
////	}
////	dwCount = 0;
////	int nUnCom = 5;
////	Fread( pCFBuf, dwCSize, dwCount, &nUnCom, sizeof(int), 1 );
////	int nCom = dwCSize - sizeof( int );
////	char *pFBuf = new char[nUnCom];
////	if( pFBuf == 0 )
////		ASSERT(FALSE);
////	char *pBuf = new char[nCom];
////	if( pBuf == 0 )
////		ASSERT(FALSE);
////	Fread( pCFBuf, dwCSize, dwCount, pBuf, nCom, 1 );
////	DWORD dwSize = nUnCom;
////	uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nCom );
////	delete [] pCFBuf;
////	delete [] pBuf;
//
//	DWORD dwSize = GetFileSize( hFile, NULL );
//	DWORD dwCount = 0;
//	char *pFBuf = new char[dwSize];
//	ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
//	if( dwSize > dwCount )
//		return FALSE;
//
//	DWORD dwStation = 0;
//	char szInfo[32];
//	CGIS_GeoMap *pMap;// = new CGIS_GeoMap;
//	int n0 = strFName.ReverseFind(L'\\');
//	swString strPath = strFName.Left( n0+1 );
////	pMap->GetMapInfo()->SetMapFilePath( strFName.Left( n0+1 ) );
//	int nLNum = 0;
////	Fread( pFBuf, dwSize, dwStation, &nLNum, sizeof(int), 1 );
////	pMap->GetMapInfo()->SetMapLayNum(nLNum);
//	BYTE byNum = 0;
//	float dbNum = 0.0;
////	CGIS_MapLayer *pLList[25];
//	CGIS_LayerManager *pLManager;// = new CGIS_LayerManager();
////	for( int i=0;	i< nLNum;	i++ )
////	{
////		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
////		CGIS_LayerInfo *pLInfo = new CGIS_LayerInfo( byNum );
////		ST_GEO_PEN pen;
////		ST_GEO_BRUSH brush;
////		Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
////		Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
////		Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
////		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
////		pen.nStyle = nNum;
////		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
////		brush.nStyle = nNum;
////		pLInfo->SetPen( pen );
////		pLInfo->SetBrush( brush );
////		///图层的显示
////		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
////		pLInfo->SetLayerState( byNum );
////		//图层的scale范围
////		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
////		pLInfo->SetMinGRScale(dbNum);
////		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
////		pLInfo->SetMaxGRScale(dbNum);
////		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
////		pLInfo->SetMinARScale(dbNum);
////		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
////		pLInfo->SetMaxARScale(dbNum);
////		///
////		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
////		pLInfo->SetIconID(nNum);
////		
////		CGIS_MapLayer *pLayer = new CGIS_MapLayer( pLInfo );
//////		pLList[i] = pLayer;
////		pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
////	}
//	int nNum = 0;
////	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
////	Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
////	if( nNum )
////		szInfo[nNum] = '\0';
////	pMap->GetMapInfo()->SetMapName( szInfo );
////	if( swString(szInfo).Find(L"中国地图") >= 0 )
////		pMap->GetMapInfo()->SetMapType( EN_MAPTYPE_NATION );
//	int nMID = 0;
////	Fread( pFBuf, dwSize, dwStation, &nMID, sizeof(int), 1 );
////	pMap->GetMapInfo()->SetMapID(nMID);
////
////	Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
////	pMap->GetMapInfo()->SetMapBndPtNum( nNum );
////	if( nNum != 0 )
////	{
////		ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
////		Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
////		pMap->GetMapInfo()->SetMapBndList( pBndPtList );
////	}
//	int nBMID = 0;
//	int nBMapID = 0;
//	ST_GEO_RECT rtBreadth;
////	Fread( pFBuf, dwSize, dwStation, &nBMID, sizeof(int), 1 );
////	Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(unsigned short), 1 );
////	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
////	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
////	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
////	Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );
////	pMap->GetMapInfo()->SetMapRect( rtBreadth );
//	int nBMNum = 0;
////	Fread( pFBuf, dwSize, dwStation, &nBMNum, sizeof(int), 1 );
////	CGIS_BreadthMap *pBMapList[200];
//	unsigned short nBMapNum = 0;
//	unsigned short nBMapMaxID = 0;
////	for( i=0;	i<nBMNum;	i++ )
////	{
////		CGIS_BreadthMap *pBMap = new CGIS_BreadthMap(pMap);
////		Fread( pFBuf, dwSize, dwStation, &pBMap->m_nBMID, sizeof(int), 1 );
////		Fread( pFBuf, dwSize, dwStation, &pBMap->m_nBreadthID, sizeof(unsigned short), 1 );
////		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dTop, sizeof(float), 1 );
////		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dBottom, sizeof(float), 1 );
////		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dLeft, sizeof(float), 1 );
////		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dRight, sizeof(float), 1 );
////		pBMapList[nBMapNum] = pBMap;
////		nBMapNum++;
////		if( pBMap->m_nBreadthID > nBMapMaxID )
////			nBMapMaxID = pBMap->m_nBreadthID;
////		/////init breadthmap layer object list;
////		POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
////		unsigned short    nKey;
////		IGIS_Layer* pLay = NULL;
////		while( pos != NULL )
////		{
////			pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
////			if( pLay )
////				pLay->InitObjectListMap( pBMap->m_nBreadthID );
////		}
////	}
////	pMap->SetBreadthMapNum( nBMapNum );
////	pMap->SetBreadthMapList( pBMapList );
//	POSITION pos = NULL;//pLManager->GetLayerListMap()->GetStartPosition();
//	unsigned short    nKey;
//	IGIS_Layer* pLay = NULL;
////	while( pos != NULL )
////	{
////		pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
////		if( pLay )
////		{
////			((CGIS_MapLayer*)pLay)->InitLBCount( nBMapMaxID );
////		}
////	}
//////	for( i=0;	i< nBMapNum;	i++ )
//////	{
//////		pMap->LoadBLData( pBMapList[i], pLList, nLNum );
//////		for( int j=0;	j<nLNum;	j++ )
//////		{
//////			pLList[j]->IncreaseLCount( pBMapList[i]->m_nBreadthID );
//////			pLList[j]->DecreaseLCount( pBMapList[i]->m_nBreadthID );
//////		}
//////		CGIS_MapBreadthNetwork *pNetwork = new CGIS_MapBreadthNetwork;
//////		pNetwork->SetGeoMap( pMap );
//////		pNetwork->ReadBreadthNetwork( &pBMapList[i] );
//////		pNetwork->ClearNetwork();
//////		delete pNetwork;
//////	}
////	pMap->m_LayerManagerSet->AddTail( pLManager );
////	pMap->Open( "" );
////	pMList->AddTail( pMap );
//	
//	/////////////////////////////////////////////////////
//	CGIS_LayerInfo LIList[25];
//	Fread( pFBuf, dwSize, dwStation, &nLNum, sizeof(int), 1 );
//	for( int i=0;	i<nLNum;	i++ )
//	{
//		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
//		LIList[i].SetLayerID( byNum );
//		ST_GEO_PEN pen;
//		ST_GEO_BRUSH brush;
//		Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
//		Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
//		Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
//		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//		pen.nStyle = nNum;
//		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//		brush.nStyle = nNum;
//		LIList[i].SetPen( pen );
//		LIList[i].SetBrush( brush );
//		///图层的显示
//		Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
//		LIList[i].SetLayerState( byNum );
//		//图层的scale范围
//		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
////		LIList[i].SetMinGRScale(0.0);
//		LIList[i].SetMinGRScale(dbNum);
//		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
////		LIList[i].SetMaxGRScale(1000000.0);
//		LIList[i].SetMaxGRScale(dbNum);
//		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
////		LIList[i].SetMinARScale(0.0);
//		LIList[i].SetMinARScale(dbNum);
//		Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
////		LIList[i].SetMaxARScale(1000000.0);
//		LIList[i].SetMaxARScale(dbNum);
//		///
//		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//		if( i>10 )
//			LIList[i].SetIconID(nNum);
//		else
//			LIList[i].SetIconID(0);
//	}
//	int nMNum = 0;
//	Fread( pFBuf, dwSize, dwStation, &nMNum, sizeof(int), 1 );
//	for( i=0;	i<nMNum;	i++ )
//	{
//		pMap = new CGIS_GeoMap;
//		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//		Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
//		if( nNum )
//			szInfo[nNum] = '\0';
//		pMap->GetMapInfo()->SetMapName( szInfo );
//		pMap->GetMapInfo()->SetMapFilePath( strFName.Left( n0+1 ) );
////		pMap->GetMapInfo()->SetMapFilePath( strPath );
//		Fread( pFBuf, dwSize, dwStation, &nMID, sizeof(int), 1 );
//		pMap->GetMapInfo()->SetMapID(nMID);
//		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
//		pMap->GetMapInfo()->SetMapBndPtNum( nNum );
//		if( nNum != 0 )
//		{
//			ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
//			Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
//			pMap->GetMapInfo()->SetMapBndList( pBndPtList );
//		}
//		pLManager = new CGIS_LayerManager();
//		for( int j=0;	j<nLNum;	j++ )
//		{
//			CGIS_LayerInfo *pLInfo = new CGIS_LayerInfo();
//			memcpy( pLInfo, &LIList[j], sizeof(CGIS_LayerInfo) );
//			CGIS_MapLayer *pLayer = new CGIS_MapLayer( pLInfo );
////			pLList[j] = pLayer;
//			pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
//		}
////		Fread( pFBuf, dwSize, dwStation, &nBMID, sizeof(int), 1 );
//		Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
//		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
//		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
//		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
//		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );
////		Fread( pFBuf, dwSize, dwStation, &rtBreadth, sizeof(ST_GEO_RECT), 1 );
//		pMap->GetMapInfo()->SetMapRect( rtBreadth );
//		Fread( pFBuf, dwSize, dwStation, &nBMNum, sizeof(int), 1 );
//		CGIS_BreadthMap *pBMapList[600];
//		unsigned short nBMapNum = 0;
//		unsigned short nBMapMaxID = 0;
//		for( j=0;	j<nBMNum;	j++ )
//		{
//			CGIS_BreadthMap *pBMap = new CGIS_BreadthMap(pMap);
//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_nBMID, sizeof(int), 1 );
//			Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
//			pBMap->m_nBreadthID = nBMapID;
////			Fread( pFBuf, dwSize, dwStation, &pBMap->m_nBreadthID, sizeof(unsigned short), 1 );
//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dTop, sizeof(float), 1 );
//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dBottom, sizeof(float), 1 );
//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dLeft, sizeof(float), 1 );
//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dRight, sizeof(float), 1 );
////			pBMapList[nBMapNum] = pBMap;
//			pBMapList[pBMap->m_nBreadthID] = pBMap;
//			nBMapNum++;
//			if( pBMap->m_nBreadthID > nBMapMaxID )
//				nBMapMaxID = pBMap->m_nBreadthID;
//			/////init breadthmap layer object list;
//			POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
//			unsigned short    nKey;
//			IGIS_Layer* pLay = NULL;
//			while( pos != NULL )
//			{
//				pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
//				if( pLay )
//					pLay->InitObjectListMap( pBMap->m_nBreadthID );
//			}
//		}
//		pMap->SetBreadthMapNum( nBMapNum );
//		pMap->SetBreadthMapList( pBMapList );
//		pos = pLManager->GetLayerListMap()->GetStartPosition();
//		while( pos != NULL )
//		{
//			pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
//			if( pLay )
//			{
//				((CGIS_MapLayer*)pLay)->InitLBCount( nBMapMaxID );
//			}
//		}
////		for( int ii=0;	ii< nBMapNum;	ii++ )
////		{
////			pMap->LoadBLData( pBMapList[ii], pLList, nLNum );
////			for( int j=0;	j<nLNum;	j++ )
////			{
////				pLList[j]->IncreaseLCount( pBMapList[ii]->m_nBreadthID );
////				pLList[j]->DecreaseLCount( pBMapList[ii]->m_nBreadthID );
////			}
////			CGIS_MapBreadthNetwork *pNetwork = new CGIS_MapBreadthNetwork;
////			pNetwork->SetGeoMap( pMap );
////			pNetwork->ReadBreadthNetwork( &pBMapList[ii] );
////			pNetwork->ClearNetwork();
////			delete pNetwork;
////		}
//		pMap->m_LayerManagerSet->AddTail( pLManager );
//		pMap->Open( "" );
//		pMList->AddTail( pMap );
//	}
//	if( pCMList )
//	{
//		ST_CMAP_INFO *pCMInfo = new ST_CMAP_INFO;
//		pCMInfo->nMapID = 2;
//		pCMInfo->strMName = _T("北京");
//		pCMInfo->nBNum = 0;
//		pCMInfo->pBIDList = NULL;
//		pCMList->AddTail(pCMInfo);
//		pCMInfo = new ST_CMAP_INFO;
//		pCMInfo->nMapID = 3;
//		pCMInfo->strMName = _T("上海");
//		pCMInfo->nBNum = 0;
//		pCMInfo->pBIDList = NULL;
//		pCMList->AddTail(pCMInfo);	
//	}
//	delete[] pFBuf;
//	CloseHandle( hFile );
//	return TRUE;
//}
#include "GIS_MapBreadthNetwork.h"
int CGIS_MFBand::LoadMaps( swString strFName, std::vector<shared_ptr<IGIS_Map> > &pMList )
{
	TRY
	{
		WIN32_FIND_DATA FileData;
		FindFirstFile( strFName , &FileData );
		FileData.dwFileAttributes = FILE_SHARE_WRITE;
		SetFileAttributes( strFName , FileData.dwFileAttributes );

		HANDLE hFile = CreateFile(strFName, GENERIC_READ|GENERIC_WRITE ,FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == hFile)
			return 2;

		DWORD dwSize = GetFileSize( hFile, NULL );
		DWORD dwCount = 0;

		char *pFBuf = new char[dwSize];
		ReadFile( hFile, pFBuf, dwSize, &dwCount, NULL);
		if( dwSize > dwCount )
		{
			delete[] pFBuf;
			pFBuf = NULL;
			return 2;
		}

		DWORD dwStation = 0;
		char szInfo[32];

		//解密加密过程
		DWORD dwReturnBytes = 0;
		char MyUUID[16];
		Fread( pFBuf, dwSize, dwStation, szInfo, 32, 1 );
#ifdef _WIN32_WCE
#ifdef _ENCRYPT_CHECK_ENABLE
		BOOL bIsOK = KernelIoControl(IOCTL_HAL_GET_UUID, 0, 0, MyUUID, sizeof(UUID), &dwReturnBytes );
		if( !bIsOK )
			return 1;
		if( !EnCode( szInfo , MyUUID ,  hFile) )
		{
			if( !UnCode(szInfo , MyUUID) )
				return 1;
		}
#endif
#endif

		CGIS_GeoMap *pMap = new CGIS_GeoMap;
		int n0 = strFName.ReverseFind(L'\\');
		swString strPath = strFName.Left( n0+1 );
		pMap->GetMapInfo()->SetMapFilePath( strFName.Left( n0+1 ) );
		int nNum = 0;
		int nLNum = 0;
		Fread( pFBuf, dwSize, dwStation, &nLNum, sizeof(int), 1 ); //层数量
		pMap->GetMapInfo()->SetMapLayNum(nLNum);
		if( nLNum > LAYER_PT_END+1 ) //层数过多 
		{
			delete[] pFBuf;
			delete pMap;
			ExitProcess( -1 );
			return 2;
		}
		BYTE byNum = 0;
		float dbNum = 0.0;
		CGIS_MapLayer *pLList[30];
		CGIS_LayerManager *pLManager = new CGIS_LayerManager();
		int i;
		//循环每一个图层
		for( int i=0;	i< nLNum;	i++ )
		{
			Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
			CGIS_LayerInfo *pLInfo = new CGIS_LayerInfo( byNum );
			ST_GEO_PEN pen;
			ST_GEO_BRUSH brush;
			Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
			Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			pen.nStyle = nNum;
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			brush.nStyle = nNum;
			pLInfo->SetPen( pen );
			pLInfo->SetBrush( brush );
			///图层的显示
			Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
			pLInfo->SetLayerState( byNum );
			//图层的scale范围
			Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
	//		dbNum = 0.1;
			//首都：5 直辖市：4 省会：3 地级市：2
			if( i == 2 )
				dbNum = 70;
			if( i == 4 || i == 3 || i == 5 )
				dbNum = 20;
			pLInfo->SetMinGRScale(dbNum);
			Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
	//		dbNum = 10000000.1;
			pLInfo->SetMaxGRScale(dbNum);
			Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
	//		dbNum = 0.1;
			//首都：5 直辖市：4 省会：3 地级市：2
			if( i == 2 )
				dbNum = 70;
			if( i == 4 || i == 3 || i == 5 )
				dbNum = 20;
			pLInfo->SetMinARScale(dbNum);
			Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
	//		dbNum = 10000000.1;
			pLInfo->SetMaxARScale(dbNum);
			///
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			pLInfo->SetIconID(nNum);
			
			CGIS_MapLayer *pLayer = new CGIS_MapLayer( pLInfo );
			pLList[i] = pLayer;
			pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
		}
		//开始NATION_MAP
		//地图名称长度
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
		if( nNum )
			szInfo[nNum] = '\0';
		pMap->GetMapInfo()->SetMapName( szInfo );
//		if( swString(szInfo).Find(L"NAV_2005_SChina") >= 0 )
			pMap->GetMapInfo()->SetMapType( EN_MAPTYPE_NATION );
		int nMID = 0;
		Fread( pFBuf, dwSize, dwStation, &nMID, sizeof(int), 1 );
		pMap->GetMapInfo()->SetMapID(nMID);
		
		Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
		pMap->GetMapInfo()->SetMapBndPtNum( nNum );
		if( nNum != 0 )
		{
			ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
			Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
			pMap->GetMapInfo()->SetMapBndList( pBndPtList );
		}
		int nBMID = 0;
		int nBMapID = 0;
		ST_GEO_RECT rtBreadth;
		Fread( pFBuf, dwSize, dwStation, &nBMID, sizeof(int), 1 );
	//	Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
		Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );
		pMap->GetMapInfo()->SetMapRect( rtBreadth );
		int nBMNum = 0;
		Fread( pFBuf, dwSize, dwStation, &nBMNum, sizeof(int), 1 );
		int nBMapNum = 0;
		int nBMapMaxID = 0;
		CGIS_BreadthMap *pBMapList = NULL;
	//	pBMapList = (CGIS_BreadthMap**)malloc( sizeof(int)*nBMNum );
	//	pBMapList = (CGIS_BreadthMap**)malloc( sizeof(CGIS_BreadthMap)*nBMNum );
		pBMapList = new CGIS_BreadthMap[nBMNum];
		memset( pBMapList, 0, sizeof(CGIS_BreadthMap)*nBMNum );
		ST_GEO_RECT pRTList[128]; //每个地图块的矩形区域大小并没有直接存入BreadthMap对象
		for( i=0;	i<nBMNum;	i++ )//Nation地图最大128个图块定义
		{
	//		CGIS_BreadthMap *pBMap = new CGIS_BreadthMap(pMap);
			Fread( pFBuf, dwSize, dwStation, &pBMapList[nBMapNum].m_nBMID, sizeof(int), 1 );
	//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_nBreadthID, sizeof(unsigned short), 1 );
			Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &pRTList[i].dTop, sizeof(float), 1 );
			Fread( pFBuf, dwSize, dwStation, &pRTList[i].dBottom, sizeof(float), 1 );
			Fread( pFBuf, dwSize, dwStation, &pRTList[i].dLeft, sizeof(float), 1 );
			Fread( pFBuf, dwSize, dwStation, &pRTList[i].dRight, sizeof(float), 1 );
	//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dTop, sizeof(float), 1 );
	//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dBottom, sizeof(float), 1 );
	//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dLeft, sizeof(float), 1 );
	//		Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dRight, sizeof(float), 1 );
			pBMapList[nBMapNum].m_nBreadthID = nBMapID;
	//		pBMap->m_nBMID = nBMapID;
			if( pBMapList[nBMapNum].m_nBreadthID > nBMapMaxID )
				nBMapMaxID = pBMapList[nBMapNum].m_nBreadthID;
			nBMapNum++;
	//		/////init breadthmap layer object list;
	//		POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
	//		short    nKey;
	//		IGIS_Layer* pLay = NULL;
	//		while( pos != NULL )
	//		{
	//			pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
	//			if( pLay )
	//				pLay->InitObjectListMap( pBMap->m_nBreadthID );
	//		}
		}
		pMap->SetBreadthMapNum( nBMapNum );
		pMap->SetBreadthMapList( pBMapList );
		pMap->SetBreadthMapRects( pRTList, nBMNum );

		POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
		short    nKey;
		IGIS_Layer* pLay = NULL;
		while( pos != NULL )
		{
			pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
			if( pLay )
			{
				((CGIS_MapLayer*)pLay)->InitLBCount( nBMapMaxID ); //每个图层初始化存放图块数据的数据结构
			}
		}
		pMap->m_LayerManagerSet->AddTail( pLManager );
	//	////////////////////////////////////test
	//	for( i=0;	i< nBMapNum;	i++ )
	//	{
	//		pMap->LoadBLData( pBMapList[i], pLList, nLNum );
	//		for( int j=0;	j<nLNum;	j++ )
	//		{
	//			pLList[j]->IncreaseLCount( pBMapList[i]->m_nBreadthID );
	//			pLList[j]->DecreaseLCount( pBMapList[i]->m_nBreadthID );
	//		}
	//	}
	//	////////////////////////////////////
		pMap->Open( "" );
		pMList->AddTail( pMap );
	//	free( pBMapList );
		pBMapList = NULL;
		
		/////////////////////////////////////////////////////
		//详细图
		CGIS_LayerInfo LIList[30]; //最多30个图层
		Fread( pFBuf, dwSize, dwStation, &nLNum, sizeof(int), 1 ); //层数量
		for( i=0;	i<nLNum;	i++ )
		{
			Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
			LIList[i].SetLayerID( byNum );
			ST_GEO_PEN pen;
			ST_GEO_BRUSH brush;
			Fread( pFBuf, dwSize, dwStation, &pen.nColor, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &pen.nWidth, sizeof(BYTE), 1 );
			Fread( pFBuf, dwSize, dwStation, &brush.nForeColor, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			pen.nStyle = nNum;
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			brush.nStyle = nNum;
			LIList[i].SetPen( pen );
			LIList[i].SetBrush( brush );
			///图层的显示
			Fread( pFBuf, dwSize, dwStation, &byNum, sizeof(BYTE), 1 );
			LIList[i].SetLayerState( byNum );
			//图层的scale范围
			Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
	//		LIList[i].SetMinGRScale(0.0);
			LIList[i].SetMinGRScale(dbNum);
			Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
	//		LIList[i].SetMaxGRScale(1000000.0);
			LIList[i].SetMaxGRScale(dbNum);
			Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
	//		LIList[i].SetMinARScale(0.0);
			LIList[i].SetMinARScale(dbNum);
			Fread( pFBuf, dwSize, dwStation, &dbNum, sizeof(float), 1 );
	//		LIList[i].SetMaxARScale(1000000.0);
			LIList[i].SetMaxARScale(dbNum);
			///
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			if( i>10 ) //图层编号大于10，图层编号设置为0
				LIList[i].SetIconID(nNum);
			else
				LIList[i].SetIconID(0);
		}
		int nMNum = 0;
		Fread( pFBuf, dwSize, dwStation, &nMNum, sizeof(int), 1 );
		for( i=0;	i<nMNum;	i++ )
		{
			pMap = new CGIS_GeoMap;
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, szInfo, sizeof(char)*nNum, 1 );
			if( nNum )
				szInfo[nNum] = '\0';
			pMap->GetMapInfo()->SetMapName( szInfo );
			pMap->GetMapInfo()->SetMapFilePath( strFName.Left( n0+1 ) );
	//		pMap->GetMapInfo()->SetMapFilePath( strPath );
			Fread( pFBuf, dwSize, dwStation, &nMID, sizeof(int), 1 ); //地图编号
			pMap->GetMapInfo()->SetMapID(nMID);
			Fread( pFBuf, dwSize, dwStation, &nNum, sizeof(int), 1 );
			pMap->GetMapInfo()->SetMapBndPtNum( nNum );
			if( nNum != 0 )
			{
				ST_GEO_PTXY *pBndPtList = new ST_GEO_PTXY[nNum];
				Fread( pFBuf, dwSize, dwStation, pBndPtList, sizeof(ST_GEO_PTXY)*nNum, 1 );
				pMap->GetMapInfo()->SetMapBndList( pBndPtList );
			}
			pLManager = new CGIS_LayerManager();
			int j;
			for( j=0;	j<nLNum;	j++ ) //构建层数组
			{
				CGIS_LayerInfo *pLInfo = new CGIS_LayerInfo();
				memcpy( pLInfo, &LIList[j], sizeof(CGIS_LayerInfo) );
				CGIS_MapLayer *pLayer = new CGIS_MapLayer( pLInfo );
				pLList[j] = pLayer;
				pLManager->GetLayerListMap()->SetAt( pLInfo->GetLayerID(), pLayer );
			}
	//		Fread( pFBuf, dwSize, dwStation, &nBMID, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 );
			Fread( pFBuf, dwSize, dwStation, &rtBreadth.dTop, sizeof(float), 1 );
			Fread( pFBuf, dwSize, dwStation, &rtBreadth.dBottom, sizeof(float), 1 );
			Fread( pFBuf, dwSize, dwStation, &rtBreadth.dLeft, sizeof(float), 1 );
			Fread( pFBuf, dwSize, dwStation, &rtBreadth.dRight, sizeof(float), 1 );
	//		Fread( pFBuf, dwSize, dwStation, &rtBreadth, sizeof(ST_GEO_RECT), 1 );
			pMap->GetMapInfo()->SetMapRect( rtBreadth );
			Fread( pFBuf, dwSize, dwStation, &nBMNum, sizeof(int), 1 ); //地图网格数量

			Fread( pFBuf, dwSize, dwStation, &MAP_ORIGIN_BID, sizeof(int), 1 );
			int nn3 = 0;
			Fread( pFBuf, dwSize, dwStation, &nn3, sizeof(int), 1 );
			MAP_ORIGIN_XNUM = nn3;
			Fread( pFBuf, dwSize, dwStation, &nn3, sizeof(int), 1 );
			MAP_ORIGIN_YNUM = nn3;
			float dValue = 0.0;
			Fread( pFBuf, dwSize, dwStation, &dValue, sizeof(float), 1 );
			MAP_ORIGIN_CENTERY = dValue;
			Fread( pFBuf, dwSize, dwStation, &dValue, sizeof(float), 1 );
			MAP_ORIGIN_CENTERX = dValue;

			nBMapNum = 0;
			nBMapMaxID = 0;
	//		pBMapList = (CGIS_BreadthMap**)malloc( sizeof(int)*nBMNum );
			pBMapList = new CGIS_BreadthMap[nBMNum]; //开始读取地图的图块了
			memset( pBMapList, 0, sizeof(CGIS_BreadthMap)*nBMNum );
			for( j=0;	j<nBMNum;	j++ )
			{
	//			CGIS_BreadthMap *pBMap = new CGIS_BreadthMap(pMap);
				Fread( pFBuf, dwSize, dwStation, &pBMapList[nBMapNum].m_nBMID, sizeof(int), 1 ); // //厂商定义的图块编号
				Fread( pFBuf, dwSize, dwStation, &nBMapID, sizeof(int), 1 ); //0 开始的图块编号
				pBMapList[nBMapNum].m_nBreadthID = nBMapID;
	//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_nBreadthID, sizeof(unsigned short), 1 );
	//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dTop, sizeof(float), 1 );
	//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dBottom, sizeof(float), 1 );
	//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dLeft, sizeof(float), 1 );
	//			Fread( pFBuf, dwSize, dwStation, &pBMap->m_rtBreadth.dRight, sizeof(float), 1 );
	//			pBMapList[nBMapNum] = pBMap;
				if( pBMapList[nBMapNum].m_nBreadthID > nBMapMaxID )
					nBMapMaxID = pBMapList[nBMapNum].m_nBreadthID;
				nBMapNum++;
				/////init breadthmap layer object list;
	/*			POSITION pos = pLManager->GetLayerListMap()->GetStartPosition();
				unsigned short    nKey;
				IGIS_Layer* pLay = NULL;
				while( pos != NULL )
				{
					pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
					if( pLay )
						pLay->InitObjectListMap( pBMap->m_nBreadthID );
				}
	*/		}
			pMap->SetBreadthMapNum( nBMapNum );
			pMap->SetBreadthMapList( pBMapList );
			pos = pLManager->GetLayerListMap()->GetStartPosition();
			while( pos != NULL )
			{
				pLManager->GetLayerListMap()->GetNextAssoc( pos, nKey, pLay );
				if( pLay )
				{
					((CGIS_MapLayer*)pLay)->InitLBCount( nBMapMaxID ); //初始化图层内部数据
				}
			}
			pMap->m_LayerManagerSet->AddTail( pLManager ); //图层管理器加入列表
			pMap->Open( "" );
			pMList->AddTail( pMap );
	//		////////////////////////////////////////test
	//		CGIS_MapBreadthNetwork *pNetwork = new CGIS_MapBreadthNetwork;
	//		pNetwork->SetGeoMap( pMap );
	//		for( int ii=0;	ii< nBMapNum;	ii++ )
	//		{
	//			pMap->LoadBLData( pBMapList[ii], pLList, nLNum );
	//			for( int j=0;	j<nLNum;	j++ )
	//			{
	//				pLList[j]->IncreaseLCount( pBMapList[ii]->m_nBreadthID );
	//				pLList[j]->DecreaseLCount( pBMapList[ii]->m_nBreadthID );
	//			}
	//			pNetwork->ReadBreadthNetwork( &pBMapList[ii] );
	//			pNetwork->ClearNetwork();
	//		}
	//		delete pNetwork;
	//		//////////////////////////////////////////
	//		free( pBMapList );
			pBMapList = NULL;
		}
		/*
		if( pCMList )
		{
			ST_CCITY_INFO *pCMInfo = new ST_CCITY_INFO;
			pCMInfo->nCityID = 2;
			wcscpy( (unsigned short*)pCMInfo->strCityName,_T("北京"));
	//		pCMInfo->strMName = _T("北京");
	//		pCMInfo->nBNum = 0;
	//		pCMInfo->pBIDList = NULL;
			pCMList->AddTail(pCMInfo);
			pCMInfo = new ST_CCITY_INFO;
			pCMInfo->nCityID = 3;
			wcscpy( (unsigned short*)pCMInfo->strCityName,_T("上海"));
	//		pCMInfo->strMName = _T("上海");
	//		pCMInfo->nBNum = 0;
	//		pCMInfo->pBIDList = NULL;
			pCMList->AddTail(pCMInfo);	
		}
		*/
		delete[] pFBuf;
		CloseHandle( hFile );
	}
	CATCH( CMemoryException, e )
	{
		ExitProcess( -1 );
	}
	AND_CATCH( CFileException, e )
	{
		ExitProcess( -1 );
	}
	AND_CATCH( CException, e )
	{
		ExitProcess( -1 );
	}
	END_CATCH
	return 0;
}
BOOL CGIS_MFBand::EnCode( char* pCharCode , char* MyUUID ,HANDLE hFile)
{
	int nByteCount = sizeof(UUID);
	int i;
	for( i = 0 ; i < nByteCount ; i++ )
	{
		if( pCharCode[i] != 0 )
			return FALSE;
	}
	int nC = 0;
	char szTemp[32];
	char szConfuse[32];
	char szResult[32];
	memcpy( szConfuse , MyUUID , 16 );
	memcpy( &(szConfuse[16]) , MyUUID , 16 );
	for( i = 0 ; i < nByteCount*2 ; i++ )
	{
		if( i%2 == 0 )
		{
			szTemp[i] = MyUUID[nC];
			nC++;
		}
		else
		{
			if( i==1 || i==17 )
				szTemp[i] = 0x45;
			if( i==3 || i==19 )
				szTemp[i] = 0x43;
			if( i==5 || i==21 )
				szTemp[i] = 0x4F;
			if( i==7 || i==23 )
				szTemp[i] = 0x4D;
			if( i==9 || i==25 )
				szTemp[i] = 0x50;
			if( i==11 || i==27 )
				szTemp[i] = 0x41;
			if( i==13 || i==29 )
				szTemp[i] = 0x53;
			if( i==15 || i==31 )
				szTemp[i] = 0x53;
		}
	}

	for( i = 0 ; i < 32 ; i++ )
		szResult[i] = szConfuse[i]^szTemp[i];

	DWORD dwCount = 0;
	BOOL bSuccess = FALSE;
	SetFilePointer( hFile , 0 , NULL , FILE_BEGIN );

	bSuccess = WriteFile( hFile, szResult, 32, &dwCount, NULL );
	
	return bSuccess;
}
BOOL CGIS_MFBand::UnCode( char* pCharCode , char* MyUUID )
{
	int nByteCount = sizeof(UUID)*2;

	//////////////////////////////////////////////////////////////////////////

	int nC = 0;
	char szTemp[32];
	char szConfuse[32];
	char szResult[32];
	memcpy( szConfuse , MyUUID , 16 );
	memcpy( &(szConfuse[16]) , MyUUID , 16 );
	int i;
	for( i = 0 ; i < nByteCount ; i++ )
	{
		if( i%2 == 0 )
		{
			szTemp[i] = MyUUID[nC];
			nC++;
		}
		else
		{
			if( i==1 || i==17 )
				szTemp[i] = 0x45;
			if( i==3 || i==19 )
				szTemp[i] = 0x43;
			if( i==5 || i==21 )
				szTemp[i] = 0x4F;
			if( i==7 || i==23 )
				szTemp[i] = 0x4D;
			if( i==9 || i==25 )
				szTemp[i] = 0x50;
			if( i==11 || i==27 )
				szTemp[i] = 0x41;
			if( i==13 || i==29 )
				szTemp[i] = 0x53;
			if( i==15 || i==31 )
				szTemp[i] = 0x53;
		}
	}

	for( i = 0 ; i < 32 ; i++ )
	{
		szResult[i] = szConfuse[i]^szTemp[i];
		if( pCharCode[i] != szResult[i] )
			return FALSE;

	}

	//////////////////////////////////////////////////////////////////////////

	return TRUE;
}
