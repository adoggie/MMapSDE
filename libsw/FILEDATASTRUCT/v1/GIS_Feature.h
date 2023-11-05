// GIS_Feature.h: interface for the CGIS_Feature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATURE_H__F746687D_F9E8_44EB_A906_C818150F8A44__INCLUDED_)
#define AFX_GIS_FEATURE_H__F746687D_F9E8_44EB_A906_C818150F8A44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GeoGlobe.h"
class __declspec(dllexport)CGIS_Feature  
{
public:
	///��������
	typedef enum tagENGEOOBJECTTYPE{
		EN_GOTYPE_NONE,			//��ͼ�յ���
		EN_GOTYPE_FIRST,
		
		EN_GOTYPE_POINT = EN_GOTYPE_FIRST,	//��ͼ�����
		EN_GOTYPE_UPOINT,		//�û������
		EN_GOTYPE_LINE,			//��ͼ�ߵ���
		EN_GOTYPE_ULINE,		//�û��ߵ���
		EN_GOTYPE_POLY,			//��ͼ�����
		EN_GOTYPE_UPOLY,		//�û������
		EN_GOTYPE_RUT,			//��ͼ��·����
		EN_GOTYPE_URUT,			//�û���·����
		EN_GOTYPE_ROUTE,		//���ߵ�������
		EN_GOTYPE_TRACK,		//������������
		EN_GOTYPE_ADDRESS,		//��ַ������
		
		EN_GOTYPE_LAST,
		EN_GOTYPE_COUNT = EN_GOTYPE_LAST - EN_GOTYPE_FIRST
	}EnGeoObjType;

public:

	CGIS_Feature( EnGeoObjType enOType );
	virtual ~CGIS_Feature();

	virtual long GetFeatureSize() {return 0;};//Added by HB.Deng 12/07/2005
	virtual CGIS_Feature* Clone();
//	void SetAttribute( CGIS_Attribute *pAtt );
//	CGIS_Attribute* GetAttribute( );
	//���úͻ�ȡ��ע����Ϣ
	void SetAnnotation( char* bstrAnno );
	char* GetAnnotation( );
	//���úͻ�ȡλ�õ���Ϣ
	void SetPtList( ST_GEO_PTXY *pPtList );
	ST_GEO_PTXY* GetPtList( );
	EnGeoObjType GetObjType( );
	void SetAnnoNum( short nAnno );
	short GetAnnoNum( );
	int m_nBreadthID;

protected:
	short		m_nAnno;
	char*		m_bstrAnno;				//���Ƶ��ע��Ϣ
	ST_GEO_PTXY	*m_pPtList;				//���Ƶ��б�
	EnGeoObjType m_enOType;				//���������
//	CGIS_Attribute	*m_pAtt;
};

#endif // !defined(AFX_GIS_FEATURE_H__F746687D_F9E8_44EB_A906_C818150F8A44__INCLUDED_)
