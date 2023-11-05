
#include "stdafx.h"
#include "sdelib.h"
#include "GIS_Address.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif


CGIS_FeatureAddress::CGIS_FeatureAddress():CGIS_Feature(EN_GOTYPE_ADDRESS)
{
	m_nID = 0;
	m_strAddName = "";
	m_nAdminID = 0;
	m_strAdminName = "";
	m_strStreetName = "";
	m_strZipCode = "";
	m_nCityID = 0;
	m_nMapID = 0;
	m_strPhoneNum = "";//电话号码
	m_bVoice = FALSE;//当车辆距离该地址50米时提示声音
	m_bAvoid = FALSE;//计算路线时避开
	m_Location.dx = 0.0f;
	m_Location.dy = 0.0f;
}
CGIS_FeatureAddress::CGIS_FeatureAddress( int nID ):CGIS_Feature(EN_GOTYPE_ADDRESS)
{
	m_nID = nID;
	m_strAddName = "";
	m_nAdminID = 0;
	m_strAdminName = "";
	m_strStreetName = "";
	m_strZipCode = "";
	m_nCityID = 0;
	m_nMapID = 0;
	m_strPhoneNum = "";//电话号码
	m_bVoice = FALSE;//当车辆距离该地址50米时提示声音
	m_bAvoid = FALSE;//计算路线时避开
}

CGIS_FeatureAddress::~CGIS_FeatureAddress()
{

}

void CGIS_FeatureAddress::SetAddressName(const swString &strAddressName)
{
	m_strAddName = strAddressName;
}

swString CGIS_FeatureAddress::GetAddressName() const
{
	return m_strAddName;
}

void CGIS_FeatureAddress::SetAdminID(const UINT &nID)
{
	m_nAdminID = nID;
}

UINT CGIS_FeatureAddress::GetAdminID() const
{
	return m_nAdminID;
}

void CGIS_FeatureAddress::SetCityID(const UINT &nID)
{
	m_nCityID = nID;
}

UINT CGIS_FeatureAddress::GetCityID() const
{
	return m_nCityID;
}

void CGIS_FeatureAddress::SetMapID(const UINT &nID)
{
	m_nMapID = nID;
}

UINT CGIS_FeatureAddress::GetMapID() const
{
	return m_nMapID;
}

void CGIS_FeatureAddress::SetAdminName(const swString &strAdminName)
{
	m_strAdminName = strAdminName;
}

swString CGIS_FeatureAddress::GetAdminName() const
{
	return m_strAdminName;
}

void CGIS_FeatureAddress::SetAvoid(BOOL bAvoid)
{
	m_bAvoid = bAvoid;
}

BOOL CGIS_FeatureAddress::GetAvoid()
{
	return m_bAvoid;
}

void CGIS_FeatureAddress::SetVoice(BOOL bVoice)
{
	m_bVoice = bVoice;
}

BOOL CGIS_FeatureAddress::GetVoice()
{
	return m_bVoice;
}

int CGIS_FeatureAddress::GetAddressID(){
	return m_nID;
}

swString CGIS_FeatureAddress::GetPhoneNum(){
	return m_strPhoneNum;
}

swString CGIS_FeatureAddress::GetStreetName()
{
	return m_strStreetName;
}

swString CGIS_FeatureAddress::GetZipCode()
{
	return m_strZipCode;
}

void CGIS_FeatureAddress::SetPhoneNum(swString strPhoneNum)
{
	m_strPhoneNum = strPhoneNum;
}

void CGIS_FeatureAddress::SetStreetName(swString strStreetName){
	m_strStreetName = strStreetName;
}

void CGIS_FeatureAddress::SetZipCode(swString strZipCode){
	m_strZipCode = strZipCode;
}

void CGIS_FeatureAddress::SetLocation(ST_GEO_PTXY ptLocation)
{
	m_Location = ptLocation;
}

ST_GEO_PTXY CGIS_FeatureAddress::GetLocation(){
	return m_Location;
}

CGIS_Feature* CGIS_FeatureAddress::Clone(){
	CGIS_FeatureAddress * pAdd = new CGIS_FeatureAddress(m_nID);
	pAdd->SetAddressName(m_strAddName);
	pAdd->SetAdminID(m_nAdminID);
	pAdd->SetAdminName(m_strAdminName);
	pAdd->SetAvoid(m_bAvoid);
	pAdd->SetCityID(m_nCityID);
	pAdd->SetLocation(m_Location);
	pAdd->SetMapID(m_nMapID);
	pAdd->SetPhoneNum(m_strPhoneNum);
	pAdd->SetStreetName(m_strStreetName);
	pAdd->SetVoice(m_bVoice);
	pAdd->SetZipCode(m_strZipCode);
	return pAdd;
}

long CGIS_FeatureAddress::GetFeatureSize(){
	long nSize = 0;

	nSize += 2 * sizeof( BOOL );
	nSize += 4 * sizeof( UINT );
	nSize += sizeof( ST_GEO_PTXY );
	nSize += m_strAddName.GetLength();
	nSize += m_strAdminName.GetLength();
	nSize += m_strPhoneNum.GetLength();
	nSize += m_strStreetName.GetLength();
	nSize += m_strZipCode.GetLength();

	return nSize;
}