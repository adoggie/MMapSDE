// GIS_Feature.h: interface for the CGIS_Feature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATURE_H__F746687D_F9E8_44EB_A906_C818150F8A44__INCLUDED_)
#define AFX_GIS_FEATURE_H__F746687D_F9E8_44EB_A906_C818150F8A44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GeoGlobe.h"
class __declspec(dllexport)CGIS_Feature  
{
public:
	///地物类型
	typedef enum tagENGEOOBJECTTYPE{
		EN_GOTYPE_NONE,			//地图空地物
		EN_GOTYPE_FIRST,
		
		EN_GOTYPE_POINT = EN_GOTYPE_FIRST,	//地图点地物
		EN_GOTYPE_UPOINT,		//用户点地物
		EN_GOTYPE_LINE,			//地图线地物
		EN_GOTYPE_ULINE,		//用户线地物
		EN_GOTYPE_POLY,			//地图面地物
		EN_GOTYPE_UPOLY,		//用户面地物
		EN_GOTYPE_RUT,			//地图道路地物
		EN_GOTYPE_URUT,			//用户道路地物
		EN_GOTYPE_ROUTE,		//航线地物类型
		EN_GOTYPE_TRACK,		//航迹地物类型
		EN_GOTYPE_ADDRESS,		//地址薄类型
		
		EN_GOTYPE_LAST,
		EN_GOTYPE_COUNT = EN_GOTYPE_LAST - EN_GOTYPE_FIRST
	}EnGeoObjType;

public:

	CGIS_Feature( EnGeoObjType enOType );
	virtual ~CGIS_Feature();

	virtual long GetFeatureSize() {return 0;};//Added by HB.Deng 12/07/2005
	virtual CGIS_Feature* Clone();
//	void SetAttribute( CGIS_Attribute *pAtt );
//	CGIS_Attribute* GetAttribute( );
	//设置和获取标注的信息
	void SetAnnotation( char* bstrAnno );
	char* GetAnnotation( );
	//设置和获取位置的信息
	void SetPtList( ST_GEO_PTXY *pPtList );
	ST_GEO_PTXY* GetPtList( );
	EnGeoObjType GetObjType( );
	void SetAnnoNum( short nAnno );
	short GetAnnoNum( );
	int m_nBreadthID;

protected:
	short		m_nAnno;
	char*		m_bstrAnno;				//控制点标注信息
	ST_GEO_PTXY	*m_pPtList;				//控制点列表
	EnGeoObjType m_enOType;				//对象的类型
//	CGIS_Attribute	*m_pAtt;
};

#endif // !defined(AFX_GIS_FEATURE_H__F746687D_F9E8_44EB_A906_C818150F8A44__INCLUDED_)
