
#ifndef _GIS_ADDRESS_H
#define _GIS_ADDRESS_H

#include "GIS_Feature.h"

class swEXPORT CGIS_FeatureAddress : public CGIS_Feature  
{
public:
	CGIS_FeatureAddress();
	CGIS_FeatureAddress( int nID );
	virtual ~CGIS_FeatureAddress();
public:
	void		SetZipCode( swString strZipCode );
	void		SetStreetName( swString strStreetName );
	void		SetPhoneNum( swString strPhoneNum );
	swString		GetZipCode();
	swString		GetStreetName();
	swString		GetPhoneNum();
	int			GetAddressID();
	BOOL		GetVoice();
	void		SetVoice( BOOL bVoice );
	BOOL		GetAvoid();
	void		SetAvoid( BOOL bAvoid );
	swString		GetAdminName() const;
	void		SetAdminName( const swString & strAdminName );
	UINT		GetMapID() const;
	void		SetMapID( const UINT & nID );
	UINT		GetCityID() const;
	void		SetCityID( const UINT & nID );
	UINT		GetAdminID() const;
	void		SetAdminID( const UINT & nID );
	swString		GetAddressName() const;
	void		SetAddressName( const swString & strAddressName );
	void		SetLocation( ST_GEO_PTXY ptLocation );
	ST_GEO_PTXY GetLocation();
	virtual CGIS_Feature* Clone();
	virtual long GetFeatureSize();			//�������洢ռ�õĿռ�

private:
	UINT		m_nID;
	swString		m_strAddName;
	UINT		m_nAdminID;
	swString		m_strAdminName;
	swString		m_strStreetName;
	swString		m_strZipCode;
	UINT		m_nCityID;
	UINT		m_nMapID;
	ST_GEO_PTXY m_Location;
	swString		m_strPhoneNum;//�绰����
	BOOL		m_bVoice;				//����������õ�ַ50��ʱ��ʾ����
	BOOL		m_bAvoid;				//����·��ʱ�ܿ�
};

#endif 
