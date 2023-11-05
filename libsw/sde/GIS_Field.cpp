
//#include "stdafx.h"
#include "GIS_Field.h"


CGIS_Field::CGIS_Field(  ){
	m_nType = 0;	
	m_nSize = 0;	
	m_nID = 0;		
	m_nOffset = 0;	
	
}

CGIS_Field::~CGIS_Field()
{
	m_nType = 0;	
	m_nSize = 0;	
	m_nID = 0;		
	m_nOffset = 0;	

}
swString CGIS_Field::GetFieldName( ){
	return m_szName;
}

void CGIS_Field::SetFieldName( swString bstrName ){
	//wcscpy(m_szName, bstrName);
	m_szName = bstrName;
}
UINT CGIS_Field::GetFieldType( )
{
	return m_nType;
}
void CGIS_Field::SetFieldType( UINT nType )
{
	m_nType = nType;
}
UINT CGIS_Field::GetFieldSize( )
{
	return m_nSize;
}
void CGIS_Field::SetFieldSize( UINT nSize )
{
	m_nSize = nSize;
}
swString CGIS_Field::GetFieldDefVal( )
{
	return m_szDefVal;
}
void CGIS_Field::SetFieldDefVal( swString bstrDefVal )
{
	//wcscpy(m_szDefVal, bstrDefVal);
	m_szDefVal = bstrDefVal;
}
UINT CGIS_Field::GetFieldID( )
{
	return m_nID;
}
void CGIS_Field::SetFieldID( UINT nID )
{
	m_nID = nID;
}
UINT CGIS_Field::GetFieldOffset( )
{
	return m_nOffset;
}
void CGIS_Field::SetFieldOffset( UINT nOffset )
{
	m_nOffset = nOffset;
}