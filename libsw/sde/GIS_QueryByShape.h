
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
	//����������(������)
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,double dbDistance,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,double dbDistance,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,double dbDistance,UINT ILayID ,unsigned short ClassID,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	//����������(������)
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,int nFeaNum );
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum );
	BOOL QueryByBuffer(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum,UINT ILayID ,unsigned short ClassID );
	//�Ը���ĳ�����ƽ��в���
	BOOL QueryBufferByName(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum,swString strName );
	//�Ը���ĳ�������С�����ƽ��в���
	BOOL QueryBufferByName(CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,int nFeaNum,UINT ILayID ,unsigned short ClassID,swString strName );
	//����ĳλ�õľ�������
	void SortPOIByDistance( CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,unsigned short ClassID);
	//�ѷ������Ƶ�POI����������
	void SortPOIByName( CPtrList& pReturnFeaSet,ST_GEO_PTXY* pPt,CPtrList* pFeaSet,unsigned short ClassID,swString strName);
	//�ཻ����
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,CPtrList* pFeaSet,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,UINT ILayID ,unsigned short ClassID,EnQueryObjType EnSharpType = EN_OBJ_ALL);
	//��������
	BOOL QueryByContain(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,CPtrList* pFeaSet);
	BOOL QueryByContain(CPtrList& pReturnFeaSet,CGIS_Feature* pFea);
	BOOL QueryByContain(CPtrList& pReturnFeaSet,CGIS_Feature* pFea,UINT ILayID,unsigned short ClassID);
	//�ҵ�·����ڣ����ཻ��·��
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CPtrList* pFirstFeaSet,CPtrList* pSecondFeaSet);
//	//����������·�����еĽ���
//	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,CPtrList* pFeaSet);
	//�������и�����·�Ľ���
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet,ST_INDEX_NAME *pRouteIndex);
	//�������ֲ��ҵ�·�����
	BOOL QueryByIntersect(CPtrList& pReturnFeaSet, int nCityID, swString strRoad1,swString strRoad2);
	//����TYPE�����𷵻�·�����ݣ����ǽ���·������
	BOOL QueryByIntersect( CPtrList& pReturnFeaSet , CPtrList* pIndexList , EnIntersectType EnType = EN_INTERSECT );
	//������ֹ��ѯ
	void SetbStopSearch( BOOL bStopSearch );
	//��ѯ�����Ƿ�Ϊ�񿿽�����
	void SetBNearNetWork( BOOL bNearNetWork );
	//������Ϊ������ѯ�����ĺ���
	void SetNetWorkRoute( CGIS_NetworkRoute* pNetworkRoute );
	//�õ���ǰλ����������������
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
	//���߸�������ͼ��
	int m_NearRouteBIDs[3000];
	//���߸�������ͼ������
	int m_nNearRouteBID;
	//��ʱ����ʱ��
//	DWORD m_dwTime;
//	DWORD m_dwRead;
//	DWORD m_dwUnZipIO;
	
};

#endif 
