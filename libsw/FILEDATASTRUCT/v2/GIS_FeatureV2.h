// GIS_FeatureV2.h: interface for the CGIS_FeatureV2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATUREV2_H__AE57F5C7_BAA5_42E2_94DD_5B6D3F1928A9__INCLUDED_)
#define AFX_GIS_FEATUREV2_H__AE57F5C7_BAA5_42E2_94DD_5B6D3F1928A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGIS_FeatureV2  
{
//////////////////////////////////////////////////////////////////////////
//数据类型与结构定义
public:
    typedef enum
    {
        EN_GOTYPE_NONE,			//地图空地物
        EN_GOTYPE_FIRST,
        
        EN_GOTYPE_POINT = EN_GOTYPE_FIRST,	//地图点地物
        EN_GOTYPE_UPOINT,		//用户点地物
        EN_GOTYPE_LINE,			//地图线地物
        EN_GOTYPE_ULINE,		//用户线地物
        EN_GOTYPE_POLY,			//地图面地物
        EN_GOTYPE_UPOLY,		//用户面地物
        EN_GOTYPE_ROADSECTION,			//地图道路地物
        EN_GOTYPE_URUT,			//用户道路地物
        EN_GOTYPE_ROUTE,		//航线地物类型
        EN_GOTYPE_TRACK,		//航迹地物类型
        EN_GOTYPE_ADDRESS,		//地址薄类型
        
        EN_GOTYPE_LAST,
        EN_GOTYPE_COUNT = EN_GOTYPE_LAST - EN_GOTYPE_FIRST
    }EnGeoObjType;

public:
    struct ST_GEO_PTXY
    {
        float dx;
        float dy;
    };
    // 地理范围结构(左、顶 、右、底)
    struct ST_GEO_RECT
    {
        float dLeft;
        float dTop;
        float dRight;
        float dBottom;
    };
//////////////////////////////////////////////////////////////////////////
public:
	CGIS_FeatureV2( EnGeoObjType enOType );
	virtual ~CGIS_FeatureV2();

    EnGeoObjType GetObjectType(){return m_eObjectType;};
//////////////////////////////////////////////////////////////////////////
protected:    
	EnGeoObjType m_eObjectType;				//对象的类型
    BOOL m_bBuddyIndependent;
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	virtual void DumpState() const;		
	virtual void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////

};

















#endif // !defined(AFX_GIS_FEATUREV2_H__AE57F5C7_BAA5_42E2_94DD_5B6D3F1928A9__INCLUDED_)
