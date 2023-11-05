// GIS_GeoRut.h: interface for the CGIS_FeatureRut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_GEORUT_H__B71CF5F5_9E4A_4E93_8684_66ED8596BBDC__INCLUDED_)
#define AFX_GIS_GEORUT_H__B71CF5F5_9E4A_4E93_8684_66ED8596BBDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GIS_Feature.h"


class __declspec(dllexport)CGIS_FeatureRut : public CGIS_Feature  
{
public:
	virtual CGIS_Feature* Clone();
	CGIS_FeatureRut(EnGeoObjType enOType);
	virtual ~CGIS_FeatureRut();

//	void SetPartNum( int nPartNum );
//	int  GetPartNum( );
	void SetPtNum( unsigned short nPtNum );
	unsigned short  GetPtNum( );
//	void SetPart( int *pPart );
//	int* GetPart( );
	void SetRectObj( ST_GEO_RECT rtObj );
	ST_GEO_RECT GetRectObj( );
	int GetRutID( );
	void SetRutID( int nRutID );
	BYTE GetRutDirection( );
	void SetRutDirection( BYTE byDirection );
private:
// 	int			m_nPartNum;
	unsigned short			m_nPtNum;
// 	int			*m_pPart;
	ST_GEO_RECT	m_rtObj;
	int m_nRutID;
	BYTE		m_byDirection;	//路段单双行标志
							///(0表示双行，1表示end到start,2表示start到end,3表示双向禁行)
};

#endif // !defined(AFX_GIS_GEORUT_H__B71CF5F5_9E4A_4E93_8684_66ED8596BBDC__INCLUDED_)
