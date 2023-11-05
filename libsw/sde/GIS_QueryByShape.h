
#ifndef _GIS_QUERYBYSHAPE_H
#define _GIS_QUERYBYSHAPE_H

#include "swBase.h"
#include "IGIS_Query.h"
#include "GIS_QueryByAtt.h"
#include "GIS_NetworkRoute.h"
#include "GIS_BreadthMap.h"
#include "GIS_MapLayer.h"


class swEXPORT CGIS_QueryByShape : public IGIS_Query
{
public:
	CGIS_QueryByShape();
	virtual ~CGIS_QueryByShape();
public:
	//缓冲区查找(定距离)
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,double dbDistance,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,double dbDistance,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,double dbDistance,UINT ILayID ,unsigned short ClassID,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	//缓冲区查找(定数量)
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,int nFeaNum );
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum );
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum,UINT ILayID ,unsigned short ClassID );
	//对附近某个名称进行查找
	BOOL QueryBufferByName(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum,swString strName );
	//对附近某个大类或小类名称进行查找
	BOOL QueryBufferByName(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum,UINT ILayID ,unsigned short ClassID,swString strName );
	//根据某位置的距离排序
	void SortPOIByDistance( CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,unsigned short ClassID);
	//把符合名称的POI按距离排序
	void SortPOIByName( CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,unsigned short ClassID,swString strName);
	//相交查找
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,CPtrList* pFeaSet,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,UINT ILayID ,unsigned short ClassID,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	//包含查找
	BOOL QueryByContain(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,CPtrList* pFeaSet);
	BOOL QueryByContain(CPtrList& pReturnFeaSet,CGIS_Feature* pFea);
	BOOL QueryByContain(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,UINT ILayID,unsigned short ClassID);
	//找道路交叉口（即相交道路）
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CPtrList* pFirstFeaSet,CPtrList* pSecondFeaSet);
//	//返回所给道路集所有的交点
//	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CPtrList* pFeaSet);
	//查找所有给出道路的交点
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,ST_INDEX_NAME *pRouteIndex);
	//根据名字查找道路交叉口
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet, int nCityID, swString strRoad1,swString strRoad2);
	//根据TYPE来区别返回路段数据，还是交叉路口数据
	BOOL QueryByIntersect( CPtrList& pReturnFeaSet , CPtrList* pIndexList , EnIntersectType EnType = EN_INTERSECT );
	//设置终止查询
	void SetbStopSearch( BOOL bStopSearch );
	//查询条件是否为否靠近航线
	void SetBNearNetWork( BOOL bNearNetWork );
	//设置作为附近查询条件的航线
	void SetNetWorkRoute( CGIS_NetworkRoute* pNetworkRoute );
	//得到当前位置所属的行政区域
	BOOL GetCurrentCityID( ST_GEO_PTXY ptCurrent ,int& nCityID );
private:
	CGIS_QueryByAtt m_QueryByAtt;
	EnQueryObjType MatchLayerType(int nLayerID);
	BOOL LoadPOIByName( CPtrList& pReTurnFeaSet , CGIS_GeoMap *pMap, CGIS_BreadthMap *pBMap, CGIS_MapLayer **pLList, int nLNum  , swString strName);
	void SortPOI(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList& pFeaSet);
	BOOL ReadPOIByName( char *pFBuf, const DWORD dwSize, DWORD &dwStation , CPtrList& pOList, swString strName);
//	BOOL strstrFind( const char * str1, const char * str2 , int ncharNum );
	BOOL m_bStopSearch;
	BOOL m_bNearNetWork;
	CGIS_NetworkRoute* m_pNetworkRoute ;
	//航线附近所有图幅
	int m_NearRouteBIDs[3000];
	//航线附近所有图幅个数
	int m_nNearRouteBID;
	//临时计算时间
//	DWORD m_dwTime;
//	DWORD m_dwRead;
//	DWORD m_dwUnZipIO;
	
};

#endif 
