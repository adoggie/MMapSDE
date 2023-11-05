
#ifndef _GIS_FIELD_H
#define _GIS_FIELD_H

#include "swBase.h"

//���ֶ���Ϣ
class swEXPORT CGIS_Field  
{
public:
	CGIS_Field( );
	~CGIS_Field();

	//��ú������ֶε�����
	swString GetFieldName( );
	void SetFieldName( swString bstrName );
	//��ú������ֶε�����
	UINT GetFieldType( );
	void SetFieldType( UINT nType );
	//��ú������ֶεĴ�С
	UINT GetFieldSize( );
	void SetFieldSize( UINT nSize );
	//��ú������ֶε�ȱʡֵ
	swString GetFieldDefVal( );
	void SetFieldDefVal( swString bstrDefVal );
	UINT GetFieldID( );
	void SetFieldID( UINT nID );
	UINT GetFieldOffset( );
	void SetFieldOffset( UINT nOffset );
private:
	//TCHAR m_szName[32];	//�ֶ���
	//TCHAR m_szDefVal[128];	//ȱʡֵ
	swString m_szName;
	swString m_szDefVal;
	UINT m_nType;		//����
	UINT m_nSize;		//��С
	UINT m_nID;			//�ֶ�ID
	UINT m_nOffset;		//�ֶ�ƫ��λ��
};

#endif 
