// QueryProxy.h: interface for the CQueryProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _QUERYPROXY_H
#define _QUERYPROXY_H

//#include "../SDE/IGIS_Map.h"
#include "..\SDE\GeoGlobe.h"
#include "..\SDE\GIS_NetworkRoute.h"
//查询结果类型定义
typedef enum tagSEARCHTYPE
{
	EN_SEARCHTYPE_FEATURE = 0,
	EN_SEARCHTYPE_ADDRESS,
	EN_SEARCHTYPE_PTXY
}SEARCHTYPE;

//查询结果数据结构定义
typedef struct ST_SEARCH_DATA
{
	void	*	pData;		//查询结果数据
	SEARCHTYPE	enType;		//查询结果数据类型

	ST_SEARCH_DATA & operator=(const ST_SEARCH_DATA&searchData);	//重载=操作符
	ST_SEARCH_DATA( const ST_SEARCH_DATA&searchData);	//拷贝构造
	ST_SEARCH_DATA(){}
	ST_SEARCH_DATA * Clone();

private:
	void CopyData(const ST_SEARCH_DATA&searchData);

}SEARCHDATA;

typedef enum tagQUERYSTYLE
{
	EN_QUERYADDR = 0,	//查询地址
	EN_QUERYPOI,	//查询POI
	EN_QUERYNPOI,	//查询附近POI
	EN_QUERYPOSTCODE	//查询邮政编码
}QUERYSTYLE;

//点数据结构
typedef struct ST_CONSID_DATA
{
	double	dx;		//经度
	double	dy;		//纬度
	CString strName;	//名字
}CONSIDDATA;

/*
//路口结构
typedef struct ST_ROAD_SECTION
{
	CString		strName;	//路口名字
	CGIS_Feature * pFea;	//路口对应的路段
}ROADSECTION;
*/

typedef ST_ROAD_INNAME ROADSECTION;

/////////////////////////////////////////////////////////////////////

#define		POI_GOVERNMENT	23//政府机构
#define		POI_PASSTIME	22//休闲娱乐
#define		POI_FINANCE		21//金融保险
#define		POI_HOTEL		20//餐饮住宿
#define		POI_MEDIA		19//文化媒体
#define		POI_HOSPITAL	18//医疗卫生
#define		POI_EDUCATION	17//科研教育
#define		POI_TELCOM		16//邮政电信
#define		POI_PUBEST		15//公共设施
#define		POI_OTHER		14//其他
#define		POI_SHOPPING	13//购物
#define		POI_TRAFFIC		12//交通
////////////////////////////////////////////////////////////////////////

#define		NUM_NEARSEL		20
#define		NEARRANGE		5000	//5km
#define		NUM_WAYPOINT	8//中途点
#define		NUM_RECORD_COUNT	50

#define		WM_QUERYOVER	WM_USER+100	//查询结束

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class IGIS_MapManager;
class IGIS_Map;
class CQueryCommonObject;
class CQueryProxy  
{
public:
	CQueryProxy();
	~CQueryProxy();
	
public:
	void	RoadQuery(CString strInputRoad);
	void	POIQuery(	int nRange,	//查询范围, nRange = 1:查询城市POI nRange=2:查询当前位置周围(5km)POI
							int nType,	//查询类型, nType = 1:查询所有POI，nType = 2:查询指定类型POI
							CString strName="");//查询的POI的名字

	void	QueryPOIByType( int nRange );	//通过大类查询POI
	void	QueryPOIByName(CString strName);
	int		GetPOIType();
	void	SetCurPos(ST_GEO_PTXY ptCurPos);
	CGIS_Feature*	GetFeatureByIndex(ST_INDEX_NAME* pIndex);
	CPtrList *		GetPOIList();
	CPtrList*		GetPOISubTypeList();
	CPtrList*		GetPOITypeList();
	CPtrList*		GetCrossRoadList();
	CPtrList*		GetRoadSecList();	//取得路段列表
	CPtrList*		GetCityList();		//取得城市列表
	CPtrList*		GetCityList( CString strCityName );//取得城市列表
	CPtrList*		GetCountryList();

	ST_CPRO_INFO*	GetLastProvince();
	ST_CPRO_INFO*	GetNextProvince();
	ST_CPRO_INFO*	GetFirstProvince();

	ST_CCOUNTRY_INFO* GetLastCountry();
	ST_CCOUNTRY_INFO* GetNextCountry();
	ST_CCOUNTRY_INFO* GetFirstCountry();

	void			SetSubPOIType(int nType);	//设置POI子类
	void			SetPOIType( int nType );	//设置POI大类
	int				GetQueryStyle();	//取得查询方式
	void			SetQueryStyle(int nStyle);	//设置查询方式
	
	void			QueryAllPOI( CString strPOIName );	//查询所有兴趣点
	void			QueryRoadSection();		//查询道路路段
	CStringArray& 	GetRoadArray();		//取得道路名称列表
	void			QueryRoadDoorplate(CString strDoorplate);		//查询门牌号码
	void			QueryRoadCross(CString strCross);		//查询交叉路口
	void			QueryRoad(CString strRoad);		//查询道路
	CGIS_Feature*	GetLastRoad();		//取得道路列表中最后一条道路
	CGIS_Feature*	GetNextRoad();		//取得道路列表中下一条道路
	CGIS_Feature*	GetFirstRoad();		//取得道路列表中第一条道路
	void			SetRoad(CString strRoad);		//设置道路名称

	void			SetCityID( int nCityID );		//设置城市ID
	void			SetProvinceID(int nProID);		//设置选择的省(直辖市)、州
	void			SetCountryID( int nCountryID );	//设置选择的国家ID
//	void			SetCountry(CString strCountry);		//设置选择的国家
	void			SetSearchData(SEARCHDATA*pData);	//设置查询结果
	SEARCHDATA	*	GetSearchData();		//取得查询结果
	void			InitMapList();		//初始化地图列表

public:
	void			RemoveAllRoad();	//删除道路列表中所有的道路
	void			RemoveSubPOI();		//清除子POI列表
	void			RemoveAllPOI();		//清除所有POI列表
	void			RemoveRSecList();	//清除路段列表
private:
	SEARCHDATA	*	m_pSearchData;			//查询结果

	CPtrList	*		m_pRoadList;		//道路列表
	CPtrList	*		m_pAllPOIList;		//所有兴趣点列表
	CPtrList	*		m_pSubPOIList;		//子项所有兴趣点列表
	CPtrList	*		m_pPOIList;			//兴趣点列表
	CPtrList	*		m_pCityList;		//城市列表
	CPtrList	*		m_pProvinceList;	//省的列表
	CPtrList	*		m_pCountryList;		//国家列表
	CPtrList	*		m_pMapList;			//地图列表
	CPtrList			m_pIndexList;		//索引列表
	CPtrList	*		m_pRSecList;		//路段列表
	CPtrList	*		m_pCrossRoadList;	//交叉路口列表
	CStringArray		m_arRoadName;		//道路名字

	CString		m_strRoad;			//当前选择的道路
	CString		m_strPostCode;		//邮政编码
	CString		m_strNearSelPath;	//历史记录路径
	CString		m_strHistoryFile;	//历史文件名

	int		m_nMapID;		//地图ID
	int		m_nPOIType;		//兴趣点类型
	int		m_nSubPOIType;	//子项兴趣点类型
	int		m_nCityID;		//城市ID
	int		m_nProvinceID;	//省ID
	int		m_nCountryID;	//国家ID
	int		m_nQueryStyle;	//当前查询的方式
	CString m_strPOIName;	//POI名字

	ST_GEO_PTXY		m_ptCurPos;		//当前位置

	IGIS_Map	*	m_pMap;		//地图

	//以下是各个CPtrList的位置
	POSITION		m_posRoad;
	POSITION		m_posPOI;
	POSITION		m_posSubPOI;
	POSITION		m_posNearSel;
	POSITION		m_posCountry;
	POSITION		m_posProvince;
	IGIS_MapManager	* m_pMapMng;

	int		m_nQueryBaseOnType;
	ST_GEO_PTXY		m_ptBaseOn;

public:
	void RemovePOIList();
	ST_GEO_PTXY GetQueryBaseOnPt();
	void SetQueryBaseOnPt( ST_GEO_PTXY ptBaseOn);
	int GetQueryBaseOnType();
	void SetQueryBaseOnType( int nType );
	void SetBNetwork( BOOL bNetwork );
	void SetNetWorkRoute( CGIS_NetworkRoute* pNetWork);
	void SetPOIName( CString strName );
	//终止查询
	void SetStopSearch();
	void	AddrQuery( int nType, CString strCross="");//地址、交叉路口查询.nType=1:地址(忽略strRoad),nType=2交叉路口
	void	QuerySubPOIByType();
	int		m_nRange;	//查询兴趣点时的空间范围1:所有的兴趣点 2:附近30Km的兴趣点
	int		m_nType;	//查询类型, nType = 1:查询所有POI，nType = 2:查询指定类型POI
	CString m_strName;	//查询的POI的名字
	CString m_strCross;
	CString m_strInputRoad;//输入的道路名(可能不是完整的路名)

	DWORD	m_dwMainThreadID;

protected:

};

#endif // !defined_QUERYPROXY_H
