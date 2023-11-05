// RouteMng.h: interface for the CRouteMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROUTEMNG_H__C6165776_7AF2_4314_86DF_3B4888032CD1__INCLUDED_)
#define AFX_ROUTEMNG_H__C6165776_7AF2_4314_86DF_3B4888032CD1__INCLUDED_

#include "GeoGlobe.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4786)
#include <vector>
using namespace std;
//////////////////////////////////////////////////////////////////////////
#include <afxtempl.h>


class CGIS_FeatureRoute;
class CGIS_NetworkPathInfo;
//////////////////////////////////////////////////////////////////////////
class CRouteMng  
{
public:
	BOOL GetRoute( CGIS_FeatureRoute*& pRoute );
    BOOL GetPathInfo(CGIS_NetworkPathInfo*& pPathInfo);
    BOOL CalculateRoute(ST_GEO_PTXY* pPtArray,int nArraySize);
    BOOL CalculateRoute_LeeWay(ST_GEO_PTXY* pPtArray,int nArraySize);
    BOOL CalculateRoute_Detour(ST_GEO_PTXY* pPtArray,int nArraySize);
    BOOL ResetSpaceMemory();
	CGIS_FeatureRoute* GetCurRoute();
	bool ReCaculRoute(ST_GEO_PTXY ptCur);
	CRouteMng();
	virtual ~CRouteMng();
public:
    void ClearResult();
    void DumpResult();
protected:
    CGIS_FeatureRoute* m_pRoute;
    CGIS_NetworkPathInfo* m_pPathInfo;

    unsigned long m_nMaxHeapUsedSize;

    typedef vector<ST_GEO_PTXY> ArrayOfGeoPoint;
    struct stuNaviSourceItem
    {
        ArrayOfGeoPoint arGeoPoint;
        int nResult;
    };

//////////////////////////////////////////////////////////////////////////
    typedef vector<stuNaviSourceItem*> ArrayOfNaviSourceItem;
    
    typedef CMap<int,int,ArrayOfNaviSourceItem*,ArrayOfNaviSourceItem*> MapOfNaviSource;
    MapOfNaviSource m_NaviSourceContainer;


    void DumpNaviSourceByCalculateResult(int nResult);
    int GetCountOfNaviSourceByCalculateResult(int nResult);
    int GetCountOfAllNaviSource();


};

#endif // !defined(AFX_ROUTEMNG_H__C6165776_7AF2_4314_86DF_3B4888032CD1__INCLUDED_)
