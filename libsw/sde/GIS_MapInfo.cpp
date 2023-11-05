

#include "GIS_MapInfo.h"

CGIS_MapInfo::CGIS_MapInfo()
{
	m_nLNum = 0;
	m_nBndPtNum = 0;
	m_nMapID = 0;
	m_pBndPtList = NULL;
	m_enMapType = EN_MAPTYPE_CITY;
}

CGIS_MapInfo::~CGIS_MapInfo()
{
	if( m_pBndPtList )
		delete[] m_pBndPtList;
	m_nLNum = 0;
	m_nBndPtNum = 0;
	m_nMapID = 0;
	m_pBndPtList = NULL;
	m_enMapType = EN_MAPTYPE_CITY;
}
swString CGIS_MapInfo::GetMapName( )
{
	return m_strMapName;
}
void CGIS_MapInfo::SetMapName( swString strMapName )
{
	m_strMapName = strMapName;
}
swString CGIS_MapInfo::GetMapFilePath( )
{
	return m_strMFPath;
}
void CGIS_MapInfo::SetMapFilePath( swString strMFPath )
{
	m_strMFPath = strMFPath;
}
ST_GEO_RECT CGIS_MapInfo::GetMapRect( )
{
	return m_rtMapRect;
}
//设置地图的课件区域
void CGIS_MapInfo::SetMapRect( ST_GEO_RECT rtMapRect ){
	memcpy( &m_rtMapRect, &rtMapRect, sizeof(ST_GEO_RECT) );
}
BYTE CGIS_MapInfo::GetMapLayNum( )
{
	return m_nLNum;
}
void CGIS_MapInfo::SetMapLayNum( BYTE nLNum )
{
	m_nLNum = nLNum;
}
unsigned short CGIS_MapInfo::GetMapID( )
{
	return m_nMapID;
}
void CGIS_MapInfo::SetMapID( unsigned short nMapID )
{
	m_nMapID = nMapID;
}
ST_GEO_PTXY* CGIS_MapInfo::GetMapBndList( )
{
	return m_pBndPtList;
}
void CGIS_MapInfo::SetMapBndList( ST_GEO_PTXY *pBndPtList )
{
	m_pBndPtList = pBndPtList;
}
unsigned short CGIS_MapInfo::GetMapBndPtNum( )
{
	return m_nBndPtNum;
}
void CGIS_MapInfo::SetMapBndPtNum( unsigned short nPtNum )
{
	m_nBndPtNum = nPtNum;
}
EnMapType CGIS_MapInfo::GetMapType()
{
	return m_enMapType;
}
void CGIS_MapInfo::SetMapType( EnMapType enMapType )
{
	m_enMapType = enMapType;
}
