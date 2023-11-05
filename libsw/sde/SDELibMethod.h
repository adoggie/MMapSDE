// SDELibMethod.h: interface for the CSDELibMethod class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDELIBMETHOD_H__553F7C9D_737B_4245_859B_7E1A1DC47E94__INCLUDED_)
#define AFX_SDELIBMETHOD_H__553F7C9D_737B_4245_859B_7E1A1DC47E94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Globe.h"
#include "GIS_FeaturePoly.h"

class CSDELibMethod  
{
public:
	///create the icon
	static void CreateImageList( );
	static CImageList* GetImageList( );

	static void xyMapToView(CRect plotRect, ST_GEO_RECT drawRect, double dScale, 
				 double dx, double dy, int& nx, int& ny);
	static EnClipType GeoRectIntersect( ST_GEO_RECT *sourceRect, ST_GEO_RECT *refRect );
	static ST_GEO_PTXY* GetClipRgn(ST_GEO_PTXY* pPt, int nNum, ST_GEO_RECT &rect, 
						int *pnPtNum, EnLObjectType enObjType);
	static void DestructResource( );

	////annotation function
	static void AddToAnnoList( ST_GEO_ANNO *pAnnotation );
	static void ClearAnnoList( );
	static BOOL CalcAnnoStation( RECT &rect );
	// 判断指定区域内是否有注释信息
	static BOOL EstimateAnnotateRect(RECT rect);

	// 计算多边形的重心
	static void CalcCenter(CGIS_FeaturePoly* pObj);

	static BOOL IsDrawState( );
	static void SetDrawState( BOOL bDrawState );

private:
	static CImageList	*m_pImageList;
	static CPtrList		*m_pAnnoList;

	static BOOL m_bDrawState;
};

#endif // !defined(AFX_SDELIBMETHOD_H__553F7C9D_737B_4245_859B_7E1A1DC47E94__INCLUDED_)
