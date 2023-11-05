
#ifndef _GIS_MAPDATAMANAGER_H
#define _GIS_MAPDATAMANAGER_H

#include "swBase.h"
#include "IGIS_Map.h"
#include "IGIS_MapManager.h"
#include "Afxmt.h"
#include <boost/shared_ptr.hpp>
using namespace boost;

class CGIS_MapDataManager : public IGIS_MapManager
{
private:
	std::vector<shared_ptr<IGIS_Map> >	_mapset;	
	//CPtrList* m_MapSet;	//地图数据
	std::vector<ST_CCOUNTRY_INFO> _countrySet;
	//CPtrList* m_pCountrySet;//国家列表
	//CPtrList* m_pPOITypeSet; //POI分类信息
	std::vector<ST_POITYPE_INFO> _poiTypeSet;
	swString m_strMainPath;//地图所在路径
	//空间数据空间的同步信号量
	//CEvent *g_pSpaceMemEvent ;
	//查询结果数据空间的同步信号量
	//CEvent *g_pQueryMemEvent ;
public:
	CGIS_MapDataManager();
	virtual ~CGIS_MapDataManager();
	void ClearSpaceMem( );
	void ClearQueryMem( );
	//CEvent* GetSpaceMemEvent();
	//CEvent* GetQueryMemEvent();
public:
	virtual IGIS_Map* GetMapByName(swString szMapName) ;
	//virtual IGIS_Map* GetFirstMap() ;
	//virtual IGIS_Map* GetNext(IGIS_Map* pMap);
	virtual IGIS_Map* GetMapByID(UINT m_mapID); 
	//virtual IGIS_Map* GetLastMap();
	//返回值　0：成功　1：解密失败　2：加载失败
	virtual int OpenMap(swString szFilePath);
	virtual IGIS_Map* CreateMap( CGIS_MapInfo *pMapInfo );
	virtual std::vector<ST_CCOUNTRY_INFO>& GetCountryList( );
	//virtual CPtrList* GetCityList( unsigned short nCountryID,unsigned short nProID = CITY_LIST_ALL);
	std::vector<ST_CCITY_INFO>& GetCityList( unsigned short nCountryID,unsigned short nProID = CITY_LIST_ALL);
	
	virtual ST_CCITY_INFO* GetCityByID( unsigned short nCountryID,int nCityID );
	virtual CPtrList* GetProvinceList( unsigned short nCountryID ) ;
	virtual std::vector<ST_POITYPE_INFO>& GetPOIClassList( ) ;
	virtual std::vector<ST_SUBPOITYPE_INFO>& GetPOISubClassList( unsigned short nSubID ) ;
	virtual swString GetMainPath();
	BOOL ClearMapData( );
protected:
	//int FindMapFile( swString bstrPath, CPtrList *pCMSet=NULL );
	int FindMapFile( swString bstrPath);
	BOOL InitSet();
	void SetMainPath( swString szFilePath );

};

#endif
