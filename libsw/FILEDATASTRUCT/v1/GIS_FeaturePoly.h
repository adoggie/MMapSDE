// GIS_FeaturePoly.h: interface for the CGIS_FeaturePoly class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_GEOPOLY_H__4D44C775_2DF0_4B17_9E05_63374BA8C3F9__INCLUDED_)
#define AFX_GIS_GEOPOLY_H__4D44C775_2DF0_4B17_9E05_63374BA8C3F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GIS_Feature.h"

class __declspec(dllexport)CGIS_FeaturePoly : public CGIS_Feature 
{
public:
	CGIS_FeaturePoly(EnGeoObjType enOType);
	virtual ~CGIS_FeaturePoly();

public:
	virtual CGIS_Feature* Clone();
	void SetPartNum( int nPartNum );
	int  GetPartNum( );
	void SetPtNum( unsigned short nPtNum );
	unsigned short  GetPtNum( );
	void SetPart( int *pPart );
	int* GetPart( );
	void SetRectObj( ST_GEO_RECT rtObj );
	ST_GEO_RECT GetRectObj( );
	void SetCenterPt( float dx, float dy );
	ST_GEO_PTXY GetCenterPt();
	void SetPolyShowClass( BYTE nShowClass );
	BYTE GetPolyShowClass( );
private:
	int			m_nPartNum;
	unsigned short			m_nPtNum;
	int			*m_pPart;
	ST_GEO_RECT	m_rtObj;
	ST_GEO_PTXY	m_CenterPt;
	//ÏÔÊ¾¼¶±ð
	BYTE m_nShowClass;
};

#endif // !defined(AFX_GIS_GEOPOLY_H__4D44C775_2DF0_4B17_9E05_63374BA8C3F9__INCLUDED_)
