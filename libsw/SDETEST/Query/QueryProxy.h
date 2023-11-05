// QueryProxy.h: interface for the CQueryProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _QUERYPROXY_H
#define _QUERYPROXY_H

//#include "../SDE/IGIS_Map.h"
#include "..\SDE\GeoGlobe.h"
#include "..\SDE\GIS_NetworkRoute.h"
//��ѯ������Ͷ���
typedef enum tagSEARCHTYPE
{
	EN_SEARCHTYPE_FEATURE = 0,
	EN_SEARCHTYPE_ADDRESS,
	EN_SEARCHTYPE_PTXY
}SEARCHTYPE;

//��ѯ������ݽṹ����
typedef struct ST_SEARCH_DATA
{
	void	*	pData;		//��ѯ�������
	SEARCHTYPE	enType;		//��ѯ�����������

	ST_SEARCH_DATA & operator=(const ST_SEARCH_DATA&searchData);	//����=������
	ST_SEARCH_DATA( const ST_SEARCH_DATA&searchData);	//��������
	ST_SEARCH_DATA(){}
	ST_SEARCH_DATA * Clone();

private:
	void CopyData(const ST_SEARCH_DATA&searchData);

}SEARCHDATA;

typedef enum tagQUERYSTYLE
{
	EN_QUERYADDR = 0,	//��ѯ��ַ
	EN_QUERYPOI,	//��ѯPOI
	EN_QUERYNPOI,	//��ѯ����POI
	EN_QUERYPOSTCODE	//��ѯ��������
}QUERYSTYLE;

//�����ݽṹ
typedef struct ST_CONSID_DATA
{
	double	dx;		//����
	double	dy;		//γ��
	CString strName;	//����
}CONSIDDATA;

/*
//·�ڽṹ
typedef struct ST_ROAD_SECTION
{
	CString		strName;	//·������
	CGIS_Feature * pFea;	//·�ڶ�Ӧ��·��
}ROADSECTION;
*/

typedef ST_ROAD_INNAME ROADSECTION;

/////////////////////////////////////////////////////////////////////

#define		POI_GOVERNMENT	23//��������
#define		POI_PASSTIME	22//��������
#define		POI_FINANCE		21//���ڱ���
#define		POI_HOTEL		20//����ס��
#define		POI_MEDIA		19//�Ļ�ý��
#define		POI_HOSPITAL	18//ҽ������
#define		POI_EDUCATION	17//���н���
#define		POI_TELCOM		16//��������
#define		POI_PUBEST		15//������ʩ
#define		POI_OTHER		14//����
#define		POI_SHOPPING	13//����
#define		POI_TRAFFIC		12//��ͨ
////////////////////////////////////////////////////////////////////////

#define		NUM_NEARSEL		20
#define		NEARRANGE		5000	//5km
#define		NUM_WAYPOINT	8//��;��
#define		NUM_RECORD_COUNT	50

#define		WM_QUERYOVER	WM_USER+100	//��ѯ����

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
	void	POIQuery(	int nRange,	//��ѯ��Χ, nRange = 1:��ѯ����POI nRange=2:��ѯ��ǰλ����Χ(5km)POI
							int nType,	//��ѯ����, nType = 1:��ѯ����POI��nType = 2:��ѯָ������POI
							CString strName="");//��ѯ��POI������

	void	QueryPOIByType( int nRange );	//ͨ�������ѯPOI
	void	QueryPOIByName(CString strName);
	int		GetPOIType();
	void	SetCurPos(ST_GEO_PTXY ptCurPos);
	CGIS_Feature*	GetFeatureByIndex(ST_INDEX_NAME* pIndex);
	CPtrList *		GetPOIList();
	CPtrList*		GetPOISubTypeList();
	CPtrList*		GetPOITypeList();
	CPtrList*		GetCrossRoadList();
	CPtrList*		GetRoadSecList();	//ȡ��·���б�
	CPtrList*		GetCityList();		//ȡ�ó����б�
	CPtrList*		GetCityList( CString strCityName );//ȡ�ó����б�
	CPtrList*		GetCountryList();

	ST_CPRO_INFO*	GetLastProvince();
	ST_CPRO_INFO*	GetNextProvince();
	ST_CPRO_INFO*	GetFirstProvince();

	ST_CCOUNTRY_INFO* GetLastCountry();
	ST_CCOUNTRY_INFO* GetNextCountry();
	ST_CCOUNTRY_INFO* GetFirstCountry();

	void			SetSubPOIType(int nType);	//����POI����
	void			SetPOIType( int nType );	//����POI����
	int				GetQueryStyle();	//ȡ�ò�ѯ��ʽ
	void			SetQueryStyle(int nStyle);	//���ò�ѯ��ʽ
	
	void			QueryAllPOI( CString strPOIName );	//��ѯ������Ȥ��
	void			QueryRoadSection();		//��ѯ��··��
	CStringArray& 	GetRoadArray();		//ȡ�õ�·�����б�
	void			QueryRoadDoorplate(CString strDoorplate);		//��ѯ���ƺ���
	void			QueryRoadCross(CString strCross);		//��ѯ����·��
	void			QueryRoad(CString strRoad);		//��ѯ��·
	CGIS_Feature*	GetLastRoad();		//ȡ�õ�·�б������һ����·
	CGIS_Feature*	GetNextRoad();		//ȡ�õ�·�б�����һ����·
	CGIS_Feature*	GetFirstRoad();		//ȡ�õ�·�б��е�һ����·
	void			SetRoad(CString strRoad);		//���õ�·����

	void			SetCityID( int nCityID );		//���ó���ID
	void			SetProvinceID(int nProID);		//����ѡ���ʡ(ֱϽ��)����
	void			SetCountryID( int nCountryID );	//����ѡ��Ĺ���ID
//	void			SetCountry(CString strCountry);		//����ѡ��Ĺ���
	void			SetSearchData(SEARCHDATA*pData);	//���ò�ѯ���
	SEARCHDATA	*	GetSearchData();		//ȡ�ò�ѯ���
	void			InitMapList();		//��ʼ����ͼ�б�

public:
	void			RemoveAllRoad();	//ɾ����·�б������еĵ�·
	void			RemoveSubPOI();		//�����POI�б�
	void			RemoveAllPOI();		//�������POI�б�
	void			RemoveRSecList();	//���·���б�
private:
	SEARCHDATA	*	m_pSearchData;			//��ѯ���

	CPtrList	*		m_pRoadList;		//��·�б�
	CPtrList	*		m_pAllPOIList;		//������Ȥ���б�
	CPtrList	*		m_pSubPOIList;		//����������Ȥ���б�
	CPtrList	*		m_pPOIList;			//��Ȥ���б�
	CPtrList	*		m_pCityList;		//�����б�
	CPtrList	*		m_pProvinceList;	//ʡ���б�
	CPtrList	*		m_pCountryList;		//�����б�
	CPtrList	*		m_pMapList;			//��ͼ�б�
	CPtrList			m_pIndexList;		//�����б�
	CPtrList	*		m_pRSecList;		//·���б�
	CPtrList	*		m_pCrossRoadList;	//����·���б�
	CStringArray		m_arRoadName;		//��·����

	CString		m_strRoad;			//��ǰѡ��ĵ�·
	CString		m_strPostCode;		//��������
	CString		m_strNearSelPath;	//��ʷ��¼·��
	CString		m_strHistoryFile;	//��ʷ�ļ���

	int		m_nMapID;		//��ͼID
	int		m_nPOIType;		//��Ȥ������
	int		m_nSubPOIType;	//������Ȥ������
	int		m_nCityID;		//����ID
	int		m_nProvinceID;	//ʡID
	int		m_nCountryID;	//����ID
	int		m_nQueryStyle;	//��ǰ��ѯ�ķ�ʽ
	CString m_strPOIName;	//POI����

	ST_GEO_PTXY		m_ptCurPos;		//��ǰλ��

	IGIS_Map	*	m_pMap;		//��ͼ

	//�����Ǹ���CPtrList��λ��
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
	//��ֹ��ѯ
	void SetStopSearch();
	void	AddrQuery( int nType, CString strCross="");//��ַ������·�ڲ�ѯ.nType=1:��ַ(����strRoad),nType=2����·��
	void	QuerySubPOIByType();
	int		m_nRange;	//��ѯ��Ȥ��ʱ�Ŀռ䷶Χ1:���е���Ȥ�� 2:����30Km����Ȥ��
	int		m_nType;	//��ѯ����, nType = 1:��ѯ����POI��nType = 2:��ѯָ������POI
	CString m_strName;	//��ѯ��POI������
	CString m_strCross;
	CString m_strInputRoad;//����ĵ�·��(���ܲ���������·��)

	DWORD	m_dwMainThreadID;

protected:

};

#endif // !defined_QUERYPROXY_H
