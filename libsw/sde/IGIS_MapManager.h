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
	//����ֵ��0���ɹ���1������ʧ�ܡ�2������ʧ��
	virtual int OpenMap(swString szFilePath) = 0;
	virtual IGIS_Map* CreateMap( CGIS_MapInfo *pMInfo ) = 0;
	virtual swString GetMainPath() = 0;
	//�õ����й����б�
	virtual CPtrList* GetCountryList( ) = 0;
	//�õ�һ�������ĳ����б�Ĭ��Ϊ���г���
	virtual CPtrList* GetCityList( unsigned short nCountryID,unsigned short nProID = CITY_LIST_ALL) = 0;
	virtual ST_CCITY_INFO* GetCityByID( unsigned short nCountryID,int nCityID ) = 0;
	//�õ�����ʡ���б�
	virtual CPtrList* GetProvinceList( unsigned short nCountryID ) = 0;
	//�õ����д����б�
	virtual CPtrList* GetPOIClassList( ) = 0;
	//�õ�ĳһ���������С��
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
