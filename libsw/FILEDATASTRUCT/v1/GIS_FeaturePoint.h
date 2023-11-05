// GIS_FeaturePoint.h: interface for the CGIS_FeaturePoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_GEOPOINT_H__CDB72E78_C305_4B34_A951_2A72EF154C77__INCLUDED_)
#define AFX_GIS_GEOPOINT_H__CDB72E78_C305_4B34_A951_2A72EF154C77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GIS_Feature.h"


class CGIS_FeaturePoint : public CGIS_Feature
{
public:
	CGIS_FeaturePoint(EnGeoObjType enOType);
	virtual ~CGIS_FeaturePoint();

public:
	virtual CGIS_Feature* Clone();
	void SetIconID( UINT nIconID );
	UINT GetIConID( );
	unsigned short GetPointObjectType( );
	void SetPointObjectType( unsigned short nPOIType );
	BYTE GetStrAddressLength( );
	void SetStrAddressLength( BYTE nAddLen );
	char* GetAddress( );
	void SetAddress( char *szAddress );
	BYTE GetStrPhoneLength( );
	void SetStrPhoneLength( BYTE nPhoneLen );
	char* GetPhoneNum( );
	void SetPhoneNum( char *szPhone );


private:
	UINT	m_nIconID;
	unsigned short m_nPOIType;	//������
	BYTE	m_nAddLen;			//��ַ������
	char	*m_szAddress;		//��ַ
	BYTE	m_nPhoneLen;		//�绰������
	char	*m_szPhone;			//�绰����
};

#endif // !defined(AFX_GIS_GEOPOINT_H__CDB72E78_C305_4B34_A951_2A72EF154C77__INCLUDED_)
