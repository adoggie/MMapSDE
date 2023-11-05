
#ifndef _GIS_WAYPOINT_H
#define _GIS_WAYPOINT_H

#include "swBase.h"
#include "GIS_NetworkVtx.h"

/**
途经点
*/
class swEXPORT  CGIS_WayPoint  
{
public:
	CGIS_WayPoint(UINT nID);
	virtual ~CGIS_WayPoint();
	UINT GetWPID( );
	ST_GEO_PTXY GetWPPosition( );
	void SetWPPosition( ST_GEO_PTXY ptWaypointPos );
	
	//设置/取得投影位置
	ST_GEO_PTXY GetWPProjPosition( EnMapType enMapType);
	void SetWPProjPosition(ST_GEO_PTXY ptProjectPos, EnMapType enMapType);
	
	//设置/取得 城市中离途经点最近的有效节点ID
	int GetWPVtxID( EnMapType enMapType );
	void SetWPVtxID( int nWPVtxID, EnMapType enMapType );

	//设置/取得 城市中离途径点最近的道路ID
  int GetWPRutID( EnMapType enMapType);
	void SetWPRutID( int nWPRutID, EnMapType enMapType );
	
	//设置/取得 本途经点的类型
	EnWaypointType GetWPType();
	void SetWPType(EnWaypointType enWPType);

	//设置/取得 城市中最近道路的点集
	void SetWPRutPtList( ST_GEO_PTXY *pPtList, unsigned short nPtNum, unsigned short nPPos, EnMapType enMapType );
	ST_GEO_PTXY* GetWPRutPtList( EnMapType enMapType);
	unsigned short GetWPRutPtNum( EnMapType enMapType );
	unsigned short GetWPProjPos( EnMapType enMapType);
	//清除目标点的路段
	void ClearWPRutInfo( );

	//标注和标注长度
	BYTE GetWPAnnoNum( );
	void SetWPAnnoNum( BYTE nAnnoNum );
	char* GetWPAnnoation( );
	void SetWPAnnoation( char* szAnno );

	//设置/取得地图ID
	unsigned short GetWPMapID( );
	void SetWPMapID( unsigned short nMapID, EnMapType enMapType );

	//设置/取得 城市中离途径点最近的道路所在路网ID
	int GetWPRutNID( EnMapType enMapType );
	void SetWPRutNID( int nWPRutNID, EnMapType enMapType );
	
	///设置/取得 途径点所在路段名
	void SetWPRutName( swString strWPRName );
	swString GetWPRutName();

	//设置/取得 城市中离途径点最近的道路的等级
	BYTE GetWPRutLevel( EnMapType enMapType );
	void SetWPRutLevel( BYTE nLevel, EnMapType enMapType );

	
private:
	UINT	m_nID;
	ST_GEO_PTXY m_ptWPPos;				//途经点的位置
	int m_nWPCityVtxID;					//城市中离途经点最近的有效节点
	int m_nWPNationVtxID;				//全国中离途经点最近的有效节点
	int m_nWPCityRutID;					//城市中离途径点最近的道路ID
	int m_nWPCityRutNID;				//城市中离途径点最近的道路所在路网ID
	int m_nWPNationRutID;				//全国中离途径点最近的道路
	int m_nWPNationRutNID;				//城市中离途径点最近的道路所在路网ID
	
	unsigned short	m_nCPPos;			//城市中投影点位置在哪个拐点旁边
	unsigned short	m_nNPPos;			//全国中投影点位置在哪个拐点旁边
	unsigned short	m_nCRPtNum;			//城市中最近道路的点个数
	unsigned short	m_nNRPtNum;			//全国中最近道路的点个数
	ST_GEO_PTXY		*m_pCRPtList;		//城市中最近道路的点集
	ST_GEO_PTXY		*m_pNRPtList;		//全国中最近道路的点集
	ST_GEO_PTXY m_ptCityWPProjPos;		//城市中途经点在路段上的投影点位置
	ST_GEO_PTXY m_ptNationWPProjPos;	//全国中途经点在路段上的投影点位置

    BYTE m_nWPCityRutLevel;              //城市中离途径点最近的道路的等级
	BYTE m_nWPNationRutLevel;;           //全国中离途径点最近的道路的等级
	unsigned short m_nMapID;
	swString m_strWPRName;				//途径点所在路段名
	EnWaypointType m_enWPType;			//途经点的类型
	char *m_szAnno;						//途经点标注
	BYTE m_nAnnoNum;					//标注长度

#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif

};

#endif
