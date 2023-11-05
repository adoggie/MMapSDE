
#ifndef _GIS_FIELD_H
#define _GIS_FIELD_H

#include "swBase.h"

//表字段信息
class swEXPORT CGIS_Field  
{
public:
	CGIS_Field( );
	~CGIS_Field();

	//获得和设置字段的名称
	swString GetFieldName( );
	void SetFieldName( swString bstrName );
	//获得和设置字段的类型
	UINT GetFieldType( );
	void SetFieldType( UINT nType );
	//获得和设置字段的大小
	UINT GetFieldSize( );
	void SetFieldSize( UINT nSize );
	//获得和设置字段的缺省值
	swString GetFieldDefVal( );
	void SetFieldDefVal( swString bstrDefVal );
	UINT GetFieldID( );
	void SetFieldID( UINT nID );
	UINT GetFieldOffset( );
	void SetFieldOffset( UINT nOffset );
private:
	//TCHAR m_szName[32];	//字段名
	//TCHAR m_szDefVal[128];	//缺省值
	swString m_szName;
	swString m_szDefVal;
	UINT m_nType;		//类型
	UINT m_nSize;		//大小
	UINT m_nID;			//字段ID
	UINT m_nOffset;		//字段偏移位置
};

#endif 
