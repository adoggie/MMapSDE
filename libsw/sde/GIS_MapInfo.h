
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
	swString		m_strMapName;		// ��ͼ��	
	swString		m_strMFPath;		//��ͼ�ļ���·��
	ST_GEO_RECT	m_rtMapRect;		//��ͼ�ľ��η�Χ
	BYTE		m_nLNum;			//ͼ�����
	unsigned short m_nMapID;		//��ͼID
	ST_GEO_PTXY *m_pBndPtList;		//��ͼ�߽����������
	unsigned short m_nBndPtNum;		//��ͼ�߽��������Ŀ
	EnMapType m_enMapType;
};

#endif 
