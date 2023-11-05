
//#include "stdafx.h"
#include "GIS_WayPoint.h"

CGIS_WayPoint::CGIS_WayPoint(UINT nID){
	m_nID = nID;
	m_nCRPtNum = m_nNRPtNum = 0;
	m_pCRPtList = NULL;			//城市中最近道路的点集
	m_pNRPtList = NULL;		//全国中最近道路的点集
	m_nWPCityVtxID = 0;		//城市中离途经点最近的有效节点
	m_nWPNationVtxID = 0;	//全国中离途经点最近的有效节点
	m_nWPCityRutID = 0;		//城市中离途径点最近的道路
	m_nWPNationRutID = 0;	//全国中离途径点最近的道路
	m_nCPPos = 0;			//城市中投影点位置在哪个拐点旁边
	m_nNPPos = 0;			//全国中投影点位置在哪个拐点旁边
	m_nMapID = 1;
	m_nAnnoNum = 0;
	m_szAnno = NULL;
	
	// 增加对成员变量的初始化 20070327
	::memset(&m_ptWPPos, 0, sizeof(ST_GEO_PTXY));
	m_nWPCityRutNID = 0;
	m_nWPNationRutNID = 0;
	::memset(&m_ptCityWPProjPos, 0, sizeof(ST_GEO_PTXY));
	::memset(&m_ptNationWPProjPos, 0, sizeof(ST_GEO_PTXY));
	m_enWPType = EN_WAYPOINTTYPE_ORIGIN;
}

CGIS_WayPoint::~CGIS_WayPoint()
{
	if( m_pNRPtList )
		delete[] m_pNRPtList;
	m_pNRPtList = NULL;
	if( m_pCRPtList )
		delete[] m_pCRPtList;
	m_pCRPtList = NULL;
	if( m_szAnno )
		delete[] m_szAnno;
	m_nID = 0;
	m_nWPCityVtxID = 0;		//城市中离途经点最近的有效节点
	m_nWPNationVtxID = 0;	//全国中离途经点最近的有效节点
	m_nWPCityRutID = 0;		//城市中离途径点最近的道路
	m_nWPNationRutID = 0;	//全国中离途径点最近的道路
	m_nCRPtNum = m_nNRPtNum = 0;
	m_nCPPos = 0;			//城市中投影点位置在哪个拐点旁边
	m_nNPPos = 0;			//全国中投影点位置在哪个拐点旁边
	m_nMapID = 1;
	m_nAnnoNum = 0;
	m_szAnno = NULL;
}

UINT CGIS_WayPoint::GetWPID( )
{
	return m_nID;
}
ST_GEO_PTXY CGIS_WayPoint::GetWPPosition( )
{
	return m_ptWPPos;
}
void CGIS_WayPoint::SetWPPosition( ST_GEO_PTXY ptWaypointPos )
{
	m_ptWPPos.dx = ptWaypointPos.dx;
	m_ptWPPos.dy = ptWaypointPos.dy;
}

ST_GEO_PTXY CGIS_WayPoint::GetWPProjPosition( EnMapType enMapType)
{
	ST_GEO_PTXY ptPos;
	if( enMapType == EN_MAPTYPE_CITY )
		ptPos = m_ptCityWPProjPos;
	else
		ptPos = m_ptNationWPProjPos;

	return ptPos;
}
void CGIS_WayPoint::SetWPProjPosition(ST_GEO_PTXY ptProjectPos, EnMapType enMapType)
{
	if( enMapType == EN_MAPTYPE_CITY )
		m_ptCityWPProjPos = ptProjectPos;
	else
		m_ptNationWPProjPos = ptProjectPos;
}
int CGIS_WayPoint::GetWPVtxID( EnMapType enMapType )
{
	int nVtxID = 0;
	if( enMapType == EN_MAPTYPE_CITY )
		nVtxID = m_nWPCityVtxID;
	else
		nVtxID = m_nWPNationVtxID;
	return nVtxID;
}
void CGIS_WayPoint::SetWPVtxID( int nWPVtxID, EnMapType enMapType )
{
	if( enMapType == EN_MAPTYPE_CITY )
		m_nWPCityVtxID = nWPVtxID;
	else
		m_nWPNationVtxID = nWPVtxID;
}
int CGIS_WayPoint::GetWPRutID( EnMapType enMapType)
{
	int nRutID = 0;
	if( enMapType == EN_MAPTYPE_CITY )
		nRutID = m_nWPCityRutID;
	else
		nRutID = m_nWPNationRutID;
	return nRutID;
}
void CGIS_WayPoint::SetWPRutID( int nWPRutID, EnMapType enMapType )
{
	if( enMapType == EN_MAPTYPE_CITY )
		m_nWPCityRutID = nWPRutID;
	else
		m_nWPNationRutID = nWPRutID;
}
int CGIS_WayPoint::GetWPRutNID( EnMapType enMapType )
{
	int nRutNID = 0;
	if( enMapType == EN_MAPTYPE_CITY )
		nRutNID = m_nWPCityRutNID;
	else
		nRutNID = m_nWPNationRutNID;
	return nRutNID;
}
void CGIS_WayPoint::SetWPRutNID( int nWPRutNID, EnMapType enMapType )
{
	if( enMapType == EN_MAPTYPE_CITY )
		m_nWPCityRutNID = nWPRutNID;
	else
		m_nWPNationRutNID = nWPRutNID;
}

BYTE CGIS_WayPoint::GetWPRutLevel( EnMapType enMapType )
{
	BYTE nRtn = 0;
	if( enMapType == EN_MAPTYPE_CITY )
		nRtn = m_nWPCityRutLevel;
	else
		nRtn = m_nWPNationRutLevel;
	return nRtn;
}
void CGIS_WayPoint::SetWPRutLevel( BYTE nLevel, EnMapType enMapType )
{
    if( enMapType == EN_MAPTYPE_CITY )
        m_nWPCityRutLevel = nLevel;
    else
        m_nWPNationRutLevel = nLevel;
}



EnWaypointType CGIS_WayPoint::GetWPType()
{
	return m_enWPType;
}
void CGIS_WayPoint::SetWPType(EnWaypointType enWPType)
{
	m_enWPType = enWPType;
}
//清除目标点的路段
void CGIS_WayPoint::ClearWPRutInfo( )
{
	if( m_pCRPtList )
		delete[] m_pCRPtList;
	m_nCRPtNum = 0;
	m_nCPPos = 0;
	m_pCRPtList = NULL;
}
void CGIS_WayPoint::SetWPRutPtList( ST_GEO_PTXY *pPtList, unsigned short nPtNum, unsigned short nPPos, EnMapType enMapType )
{
	if( enMapType == EN_MAPTYPE_CITY && nPtNum >= 2 )
	{
		m_pCRPtList = new ST_GEO_PTXY[nPtNum];
		memcpy( m_pCRPtList, pPtList, sizeof(ST_GEO_PTXY)*nPtNum );
		m_nCRPtNum = nPtNum;
		m_nCPPos = nPPos;
	}
	else if( enMapType == EN_MAPTYPE_NATION && nPtNum >= 2 )
	{
		m_pNRPtList = new ST_GEO_PTXY[nPtNum];
		memcpy( m_pNRPtList, pPtList, sizeof(ST_GEO_PTXY)*nPtNum );
		m_nNRPtNum = nPtNum;
		m_nNPPos = nPPos;
	}
}
ST_GEO_PTXY* CGIS_WayPoint::GetWPRutPtList( EnMapType enMapType)
{
	if( enMapType == EN_MAPTYPE_CITY )
		return m_pCRPtList;
	else 
		return m_pNRPtList;
}
unsigned short CGIS_WayPoint::GetWPRutPtNum( EnMapType enMapType )
{
	if( enMapType == EN_MAPTYPE_CITY )
		return m_nCRPtNum;
	else 
		return m_nNRPtNum;
}
unsigned short CGIS_WayPoint::GetWPProjPos( EnMapType enMapType )
{
	if( enMapType == EN_MAPTYPE_CITY )
		return m_nCPPos;
	else 
		return m_nNPPos;
}
unsigned short CGIS_WayPoint::GetWPMapID( )
{
	return m_nMapID;
}
void CGIS_WayPoint::SetWPMapID( unsigned short nMapID, EnMapType enMapType )
{
	if( enMapType == EN_MAPTYPE_CITY )
		m_nMapID = nMapID;
}

BYTE CGIS_WayPoint::GetWPAnnoNum( )
{
	return m_nAnnoNum;
}
void CGIS_WayPoint::SetWPAnnoNum( BYTE nAnnoNum )
{
	m_nAnnoNum = nAnnoNum;
}
char* CGIS_WayPoint::GetWPAnnoation( )
{
	return m_szAnno;
}
void CGIS_WayPoint::SetWPAnnoation( char* szAnno )
{
	m_szAnno = szAnno;
}
void CGIS_WayPoint::SetWPRutName( swString strWPRName )
{
	m_strWPRName = strWPRName;
}
swString CGIS_WayPoint::GetWPRutName( )
{
	return m_strWPRName;
}
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_WayPoint::DumpState() const	
{  
    TRACE0("********************************************************************\n");
    TRACE1("m_nID:%d\n",this->m_nID);
    TRACE2("m_ptWPPos:%.5f,%.5f\n",this->m_ptWPPos.dx,this->m_ptWPPos.dy);
    TRACE1("m_nWPCityVtxID:%d\n",this->m_nWPCityVtxID);
    TRACE1("m_nWPCityRutID:%d\n",this->m_nWPCityRutID);
    TRACE1("m_nWPCityRutNID:%d\n",this->m_nWPCityRutNID);
    TRACE1("m_nCPPos:%d\n",this->m_nCPPos);
    TRACE1("m_nCRPtNum:%d\n",this->m_nCRPtNum);
    TRACE1("m_pCRPtList:%d\n",this->m_nWPCityRutNID);

    TRACE2("m_ptCityWPProjPos:%.5f,%.5f\n",this->m_ptCityWPProjPos.dx,this->m_ptCityWPProjPos.dy);
    TRACE1("m_nMapID:%d\n",this->m_nMapID);
    TRACE1("m_strWPRName:%s\n",this->m_strWPRName);
    TRACE1("m_enWPType:%d\n",this->m_enWPType);
    //TRACE1("m_nAnnoNum:%d\n",swString(this->m_nAnnoNum));
	TRACE1("m_nAnnoNum:%d\n",this->m_nAnnoNum);
    TRACE1("m_szAnno:%s\n",this->m_szAnno);
    TRACE0("********************************************************************\n");
    
    Sleep(100);
}

void CGIS_WayPoint::AssertValid() const	
{   


}
#endif
//////////////////////////////////////////////////////////////////////////