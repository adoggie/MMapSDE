#ifndef _IGIS_MAP_MANAGER_H
#define _IGIS_MAP_MANAGER_H

#include "swBase.h"
#include "IGIS_Map.h"

class swEXPORT IGIS_MapManager : public swBaseObject {
public:
	IGIS_MapManager();
	virtual ~IGIS_MapManager();
public:
	virtual IGIS_Map* GetMapByName(swString szMapName) = 0;
	virtual IGIS_Map* GetFirstMap() = 0;
	virtual IGIS_Map* GetNext(IGIS_Map* pMap) = 0;
	virtual IGIS_Map* GetMapByID(UINT m_mapID) = 0; 
	virtual IGIS_Map* GetLastMap() = 0;
	//返回值　0：成功　1：解密失败　2：加载失败
	virtual int OpenMap(swString szFilePath) = 0;
	virtual IGIS_Map* CreateMap( CGIS_MapInfo *pMInfo ) = 0;
	virtual swString GetMainPath() = 0;
	//得到所有国家列表
	virtual CPtrList* GetCountryList( ) = 0;
	//得到一定条件的城市列表，默认为所有城市
	virtual CPtrList* GetCityList( unsigned short nCountryID,unsigned short nProID = CITY_LIST_ALL) = 0;
	virtual ST_CCITY_INFO* GetCityByID( unsigned short nCountryID,int nCityID ) = 0;
	//得到所有省的列表
	virtual CPtrList* GetProvinceList( unsigned short nCountryID ) = 0;
	//得到所有大类列表
	virtual CPtrList* GetPOIClassList( ) = 0;
	//得到某一大类的所有小类
	virtual CPtrList* GetPOISubClassList( unsigned short nSubID ) = 0;

//////////////////////////////////////////////////////////////////////////    
protected:
    CPtrList m_ListOfView;
public:
#ifdef _CVIEW
	BOOL AttechView(CView* pView);
    BOOL DetchView(CView* pView);
#else
	BOOL AttechView(CWnd* pView);
    BOOL DetchView(CWnd* pView);
#endif
//////////////////////////////////////////////////////////////////////////

};

#endif 
