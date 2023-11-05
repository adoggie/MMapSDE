// GIS_GeoLine.h: interface for the CGIS_FeatureLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_GEOLINE_H__1CB85222_A5FB_46FD_A5D9_B432D886250D__INCLUDED_)
#define AFX_GIS_GEOLINE_H__1CB85222_A5FB_46FD_A5D9_B432D886250D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GIS_Feature.h"

class __declspec(dllexport)CGIS_FeatureLine : public CGIS_Feature  
{
public:
	CGIS_FeatureLine(EnGeoObjType enOType);
	virtual ~CGIS_FeatureLine();

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
private:
	int			m_nPartNum;
	unsigned short	m_nPtNum;
	int			*m_pPart;
	ST_GEO_RECT	m_rtObj;
};

#endif // !defined(AFX_GIS_GEOLINE_H__1CB85222_A5FB_46FD_A5D9_B432D886250D__INCLUDED_)
