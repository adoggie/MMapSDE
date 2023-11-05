
#ifndef _GIS_MAPINFO_H
#define _GIS_MAPINFO_H	

//#include "swBase.h"
#include "GeoGlobe.h"
class swEXPORT CGIS_MapInfo 
{
public:
	CGIS_MapInfo();
	virtual ~CGIS_MapInfo();

	swString GetMapName( );
	void SetMapName( swString strMapName );
	swString GetMapFilePath( );
	void SetMapFilePath( swString strMFPath );
	ST_GEO_RECT GetMapRect( );
	void SetMapRect( ST_GEO_RECT rtMapRect );
	BYTE GetMapLayNum( );
	void SetMapLayNum( BYTE nLNum );
	unsigned short GetMapID( );
	void SetMapID( unsigned short nMapID );
	ST_GEO_PTXY* GetMapBndList( );
	void SetMapBndList( ST_GEO_PTXY *pBndPtList );
	unsigned short GetMapBndPtNum( );
	void SetMapBndPtNum( unsigned short nPtNum );
	EnMapType GetMapType();
	void SetMapType( EnMapType enMapType );

private:
	swString		m_strMapName;		// 地图名	
	swString		m_strMFPath;		//地图文件的路径
	ST_GEO_RECT	m_rtMapRect;		//地图的矩形范围
	BYTE		m_nLNum;			//图层层数
	unsigned short m_nMapID;		//地图ID
	ST_GEO_PTXY *m_pBndPtList;		//地图边界坐标点链表
	unsigned short m_nBndPtNum;		//地图边界坐标点数目
	EnMapType m_enMapType;
};

#endif 
