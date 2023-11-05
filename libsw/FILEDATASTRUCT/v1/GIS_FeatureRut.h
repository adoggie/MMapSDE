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
	BYTE		m_byDirection;	//·�ε�˫�б�־
							///(0��ʾ˫�У�1��ʾend��start,2��ʾstart��end,3��ʾ˫�����)
};

#endif // !defined(AFX_GIS_GEORUT_H__B71CF5F5_9E4A_4E93_8684_66ED8596BBDC__INCLUDED_)
