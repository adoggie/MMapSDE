// IGIS_Query2.h: interface for the IGIS_Query2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IGIS_QUERY2_H__DB23B3F2_6F12_4538_931A_2FAD609BCF5E__INCLUDED_)
#define AFX_IGIS_QUERY2_H__DB23B3F2_6F12_4538_931A_2FAD609BCF5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include "../sde/SDEControler.h"
#include "QueryProxy.h"
#include "../SDE/GIS_Address.h"
#include "../sde/GeoGlobe.h"


//////////////////////////////////////////////////////////////////////////
//#include <stdexcept>
//#include <string>
//using namespace std;
//
//class QueryException : public logic_error
//{
//public:
//	QueryException(const string& message = "")
//		: logic_error(message.c_str())
//	{}
//};
 
////////////////////////////////////////////////////////////////////////

class CQueryCommonObject : public CObject
{
public:	
	CQueryCommonObject()
	{
		m_strName =_T("");
		m_nId = -1;
		m_dx = -1.0;
		m_dy = -1.0;
	}
	CQueryCommonObject(LPCTSTR strName,int nId,float fLat,float fLog)
	{
		m_strName = strName;
		m_nId = nId;
		m_dy = fLat;
		m_dx = fLog;
	}

	CQueryCommonObject& operator =(CQueryCommonObject& rSrc)
	{
		m_strName = rSrc.m_strName; 
		m_nId = rSrc.m_nId; 
		m_dy = rSrc.m_dy;
		m_dx = rSrc.m_dx;
		return *this;
	}
	CQueryCommonObject(CQueryCommonObject &rSrc)
	{
		m_strName = rSrc.m_strName; 
		m_nId = rSrc.m_nId;
		m_dy = rSrc.m_dy;
		m_dx = rSrc.m_dx;
	}  

	CString		m_strName;
	int			m_nId;
	float		m_dy;
	float		m_dx;
	
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CQueryCommonObject)
};

class CCountryObject : public CQueryCommonObject
{
public:	
	CCountryObject(){m_strName =_T("");m_nId = -1;}
	CCountryObject(LPCTSTR strName,int nId){m_strName = strName;m_nId = nId;}

	CCountryObject& operator =(CCountryObject& rSrc){m_strName = rSrc.m_strName; m_nId = rSrc.m_nId; return *this;}
	CCountryObject(CCountryObject &rSrc){m_strName = rSrc.m_strName; m_nId = rSrc.m_nId;}  

//	CString		m_strName;
//	int			m_nId;
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CCountryObject)
};

class CProvinceObject : public CQueryCommonObject
{
public:	
	CProvinceObject(){m_strName =_T("");m_nId = -1;}
	CProvinceObject(LPCTSTR strName,int nId){m_strName = strName;m_nId = nId;}

	CProvinceObject& operator =(CProvinceObject& rSrc){m_strName = rSrc.m_strName; m_nId = rSrc.m_nId; return *this;}
	CProvinceObject(CProvinceObject &rSrc){m_strName = rSrc.m_strName; m_nId = rSrc.m_nId;} 

//	CString		m_strName;
//	int			m_nId;
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CProvinceObject)
};

class CCityObject : public CQueryCommonObject
{
public:	
	CCityObject(){m_strName =_T("");m_nId = -1;}
	CCityObject(LPCTSTR strName,int nId){m_strName = strName;m_nId = nId;}

	CCityObject& operator =(CCityObject& rSrc){m_strName = rSrc.m_strName; m_nId = rSrc.m_nId; return *this;}
	CCityObject(CCityObject &rSrc){m_strName = rSrc.m_strName; m_nId = rSrc.m_nId;}  

//	CString		m_strName;
//	int			m_nId;
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CCityObject)
};

class CRoadObject : public CQueryCommonObject
{
public:	
	CRoadObject(){m_strName =_T("");}
	CRoadObject(LPCTSTR strName){m_strName = strName;}

	CRoadObject& operator =(CRoadObject& rSrc){m_strName = rSrc.m_strName; return *this;}
	CRoadObject(CRoadObject &rSrc){m_strName = rSrc.m_strName;}  

//	CString		m_strName;
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CRoadObject)
};

class CRoadPointObject : public CQueryCommonObject
{
public:	
	CRoadPointObject(){m_strName =_T(""); m_dx = -1.0; m_dy = -1.0;}
	CRoadPointObject(LPCTSTR strName,float dx,float dy){m_strName = strName; m_dx = dx; m_dy = dy;}

	CRoadPointObject& operator =(CRoadPointObject& rSrc){m_strName = rSrc.m_strName; m_dx = rSrc.m_dx; m_dy = rSrc.m_dy; return *this;}
	CRoadPointObject(CRoadPointObject &rSrc){m_strName = rSrc.m_strName; m_dx = rSrc.m_dx; m_dy = rSrc.m_dy;}  

//	CString		m_strName;
//	float m_dx;
//	float m_dy;
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CRoadPointObject)
};


class CStIndexOfPOI : public CQueryCommonObject
{
public:	
	CStIndexOfPOI()
	{
		m_BandID	= -1;
		m_LayerID	= -1;
		m_ClassID	= -1;
	}
	CStIndexOfPOI(ST_INDEX_NAME* pObj)
	{
		ASSERT(pObj);
		
		//m_strName	= CString(pObj->Name);
		CharToUnicode(pObj->Name, &m_strName);
		m_BandID	= pObj->BandID;
		m_LayerID	= pObj->LayerID;
		m_ClassID	= pObj->ClassID;
		
	}
	CStIndexOfPOI& operator =(CStIndexOfPOI& rSrc)
	{
		m_strName	= rSrc.m_strName;
		m_BandID	= rSrc.m_BandID;
		m_LayerID	= rSrc.m_LayerID;
		m_ClassID	= rSrc.m_ClassID;
		return *this;
	}
	CStIndexOfPOI(CStIndexOfPOI &rSrc)
	{
		m_strName	= rSrc.m_strName;
		m_BandID	= rSrc.m_BandID;
		m_LayerID	= rSrc.m_LayerID;
		m_ClassID	= rSrc.m_ClassID;
	}  
	
	int  m_BandID;
	int  m_LayerID;
	int  m_ClassID;

	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CStIndexOfPOI)
 };

//ϲ����
class CStFavoritePoint : public CQueryCommonObject
{
public:	
	CStFavoritePoint(){m_pObj = NULL;}
	~CStFavoritePoint()
	{
		if(m_pObj)
		{
			delete m_pObj;
			m_pObj = NULL;
		}
	}
	
	CStFavoritePoint(CGIS_FeatureAddress* pObj)
	{
		m_pObj = pObj;
	}
	
	CStFavoritePoint& operator =(CStFavoritePoint& rSrc)
	{
		ASSERT(rSrc.m_pObj);
		m_pObj		= (CGIS_FeatureAddress*)rSrc.m_pObj->Clone();
		m_strName	= rSrc.m_strName;
		m_dx = rSrc.m_dx;
		m_dy = rSrc.m_dy;
		return *this;
	}
	CStFavoritePoint(CStFavoritePoint &rSrc)
	{
		ASSERT(rSrc.m_pObj);

		m_pObj		= (CGIS_FeatureAddress*)rSrc.m_pObj->Clone();
		m_strName	= rSrc.m_strName;
		m_dx = rSrc.m_dx;
		m_dy = rSrc.m_dy;
	}  
	
	CGIS_FeatureAddress * m_pObj;
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CStFavoritePoint)
};

///POI����
class CStGISFeature : public CQueryCommonObject
{
public:	
	CStGISFeature(){m_pObj = NULL;}
	~CStGISFeature()
	{
		if(m_pObj)
		{
			delete m_pObj;
			m_pObj = NULL;
		}
	}

	CStGISFeature(CGIS_Feature* pObj)
	{ 
		ASSERT(pObj);
		m_pObj = pObj;
		
		//m_strName = CString(m_pObj->GetAnnotation());
		CharToUnicode(m_pObj->GetAnnotation(), &m_strName);
		ST_GEO_PTXY* pListOfPt = m_pObj->GetPtList();
		ASSERT(pListOfPt);
		m_dx = pListOfPt->dx;
		m_dy = pListOfPt->dy;
	}
	
	CStGISFeature& operator =(CStGISFeature& rSrc)
	{
		ASSERT(rSrc.m_pObj);

		m_pObj = rSrc.m_pObj->Clone(); 
		ASSERT(m_pObj);
		//m_strName = CString(m_pObj->GetAnnotation());
		CharToUnicode(m_pObj->GetAnnotation(), &m_strName);
		ST_GEO_PTXY* pListOfPt = m_pObj->GetPtList();
		ASSERT(pListOfPt);
		m_dx = pListOfPt->dx;
		m_dy = pListOfPt->dy;
		return *this;
	}
	CStGISFeature(CStGISFeature &rSrc)
	{
		ASSERT(rSrc.m_pObj);

		m_pObj = rSrc.m_pObj->Clone(); 
		ASSERT(m_pObj);
		//m_strName = CString(m_pObj->GetAnnotation());
		CharToUnicode(m_pObj->GetAnnotation(), &m_strName);
		ST_GEO_PTXY* pListOfPt = m_pObj->GetPtList();
		ASSERT(pListOfPt);
		m_dx = pListOfPt->dx;
		m_dy = pListOfPt->dy;
	}  
	
	CGIS_Feature* m_pObj;
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CStGISFeature)
};

class CSmallClass : public CQueryCommonObject
{
public:	
	CSmallClass(){m_strName =_T("");m_nId = -1;}
	CSmallClass(LPCTSTR strName,int nId){m_strName = strName;m_nId = nId;}

	CSmallClass& operator =(CSmallClass& rSrc){m_strName = rSrc.m_strName; m_nId = rSrc.m_nId; return *this;}
	CSmallClass(CSmallClass &rSrc){m_strName = rSrc.m_strName; m_nId = rSrc.m_nId;}  

//	CString		m_strName;
//	int			m_nId;
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CSmallClass)
};

class CRoadSection : public CQueryCommonObject
{
public:	
	CRoadSection(){/*m_pObj = NULL;*/}
	CRoadSection(ST_ROAD_INNAME* pObj)
	{ 
//		m_pObj = pObj->pRoute;

		m_strName = pObj->InName;
		ST_GEO_PTXY* pListOfPt = pObj->pRoute->GetPtList();
		int nNum = pObj->pRoute->GetPtNum();
		ASSERT(pListOfPt);
		m_dx = pListOfPt[nNum/2].dx;
		m_dy = pListOfPt[nNum/2].dy;
	}
	
	CRoadSection& operator =(CRoadSection& rSrc)
	{
//		m_pObj = rSrc.m_pObj; 
		m_strName = rSrc.m_strName;
//		ST_GEO_PTXY* pListOfPt = m_pObj->GetPtList();
//		ASSERT(pListOfPt);
		m_dx = rSrc.m_dx;
		m_dy = rSrc.m_dy;
		return *this;
	}
	CRoadSection(CRoadSection &rSrc)
	{
//		m_pObj = rSrc.m_pObj;
		m_strName = rSrc.m_strName;
		m_dx = rSrc.m_dx;
		m_dy = rSrc.m_dy;
	}  
	
//	CGIS_Feature* m_pObj;
	virtual void Serialize( CArchive& archive );
	DECLARE_SERIAL(CRoadSection)
};

class IGIS_Query2  
{
public:
	IGIS_Query2();
	virtual ~IGIS_Query2();

	enum enSearchCondition
	{
		CONDITION_NONE			= 0X00000000,//������
		CONDITION_COUNTRY		= 0X00000001,//����
		CONDITION_PROVINCE		= 0X00000002,//ʡ��
		CONDITION_CITY			= 0X00000004,//�ǳ�
		
		CONDITION_POSTCODE		= 0X00000008,//�ʱ�
		CONDITION_ROAD			= 0X00000010,//��·

		CONDITION_BIGCLASS		= 0X00000020,//����
		CONDITION_SMALLCLASS	= 0X00000040,//С��

		CONDITION_CROSSROAD		= 0X00000080,//·��
		CONDITION_HOUSENUMBER	= 0X00000100,//����
		
		CONDITION_LATLOG		= 0X00000200,//��γ��
	
		CONDITION_ROAD2			= 0X00000400,//��·2(���ڲ齻��·��)

		CONDITION_BASE_ON		= 0X00000800,//��ѯPOIʱ�Ļ�׼
		CONDITION_MATCH_TEXT	= 0X00001000,//��ѯPOIʱ�Ļ�׼

		CONDITION_INDEX_OF_POI	= 0X00002000,//��ѯPOIʱ�Ļ�׼
	};
	enum enSearchObject
	{
		OBJECT_NONE = 0,
		OBJECT_FIRST = 1,

		OBJECT_COUNTRY	 = OBJECT_FIRST,//��ѯ�����б�
		OBJECT_PROVINCE	,				//��ѯʡ���б�
		OBJECT_CITY		,				//��ѯ�ǳ��б�
		OBJECT_POSTCODE	,				//��ѯ�ʱ��б�
		OBJECT_ROAD		,				//��·
		OBJECT_CROSSROAD,				//·��
		OBJECT_HOUSENUMBER,				//����
		OBJECT_POI,						//POI
		OBJECT_BIGCLASS,				//��ѯ�����б�
		OBJECT_SMALLCLASS,				//��ѯС���б�
		OBJECT_NEAREST,					//����������ѯ
		OBJECT_ROADSECTION,			//��ѯ·��
		OBJECT_POI_BY_INDEX,			//��ѯPOI(��������)
		OBJECT_INDEX_OF_POI,			//��ѯ����
		
		OBJECT_LAST
	};

	enum enSearchRangeBaseon
	{
		RANGE_BASE_ON_NONE = 0,
		RANGE_BASE_ON_FIRST = 1,
			
		RANGE_BASE_ON_POINT	 = RANGE_BASE_ON_FIRST,	
		RANGE_BASE_ON_CITY,							
		RANGE_BASE_ON_LINE,

		RANGE_BASE_ON_LAST
	};
	
	enum enUpdateAspect
	{
		UPDATE_NONE = 0,
		UPDATE_FIRST = 1,

		UPDATE_NEW_OBJECT	 = UPDATE_FIRST,//��ѯ���µĶ���
		UPDATE_STOP,//��ѯ�Ѿ�����

		UPDATE_LAST
	};
	typedef void (CALLBACK* UPDATEPROC) (enUpdateAspect eAspect, DWORD dwPara);
	struct tagSearchCondition
	{
//		CString strCountry;
//		CString strProvince;
//		CString strCity;
		CString strMatchText;	//ƥ���ı�(��)
		int nCountryId;
		int nProvinceId;
		int nCityId;
		int nBigClass;
		int nSmallClass;
		CString strRoad;
		CString strRoad2;
		CString strCrossRoad;
		CString strHouseNumber;
		CString strPostCode;
		void* pLineBaseOn;

		CString strNameOfIndex;
		int  BandIDOfIndex;
		int  LayerIDOfIndex;
		int  ClassIDOfIndex;

	
		enSearchRangeBaseon eSearchRangeBaseon;
		UPDATEPROC lpfnUpdateProc; 

		float fLog;
		float fLat;

		UINT nConditionMask;//��������
		UINT nResult;	//���
	};
	BOOL SetCondition(tagSearchCondition& pData);//�ڲ�ѯ�߳�ʱ�����Խ�����������
	void GetCondition(tagSearchCondition& pData);

	BOOL SearchStart();
	BOOL SearchStop();
	
//////////////////////////////////////////////////////////////////////////
private:
	CWinThread* m_pThread;
	//##ModelId=444C9A9E02A5
	CEvent m_ExitEvent;

	struct tagSearchThreadData
	{
		HWND hWnd;
		HANDLE hExitEvent;
		IGIS_Query2* pGis_Query2 ;
	};
	tagSearchThreadData m_SearchThreadData;
	static UINT SearchThread(void* pParam);
	BOOL WaitSearchThreadExit();
	

	BOOL IfConditionEnough();
	tagSearchCondition m_Condition;
	CQueryProxy m_QueryTool;

	//////////////////////////////////////////////////////////////////////////
	//���մ˽ṹҪ�Ӵ�����ȥ��,��Ϊռ����̫����ڴ�
	POSITION m_CurrentPos;

	CObList m_lstObject;

	BOOL Initial();
	void DeleteAllList();
	CSDEControler* m_pSDEControl;

//////////////////////////////////////////////////////////////////////////
//lijun add 2006-7-21
	CObList m_lstDirectlyCityRawData;
	
//////////////////////////////////////////////////////////////////////////

};

#endif // !defined(AFX_IGIS_QUERY2_H__DB23B3F2_6F12_4538_931A_2FAD609BCF5E__INCLUDED_)
