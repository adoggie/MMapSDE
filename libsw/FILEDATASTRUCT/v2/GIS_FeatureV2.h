// GIS_FeatureV2.h: interface for the CGIS_FeatureV2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_FEATUREV2_H__AE57F5C7_BAA5_42E2_94DD_5B6D3F1928A9__INCLUDED_)
#define AFX_GIS_FEATUREV2_H__AE57F5C7_BAA5_42E2_94DD_5B6D3F1928A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGIS_FeatureV2  
{
//////////////////////////////////////////////////////////////////////////
//����������ṹ����
public:
    typedef enum
    {
        EN_GOTYPE_NONE,			//��ͼ�յ���
        EN_GOTYPE_FIRST,
        
        EN_GOTYPE_POINT = EN_GOTYPE_FIRST,	//��ͼ�����
        EN_GOTYPE_UPOINT,		//�û������
        EN_GOTYPE_LINE,			//��ͼ�ߵ���
        EN_GOTYPE_ULINE,		//�û��ߵ���
        EN_GOTYPE_POLY,			//��ͼ�����
        EN_GOTYPE_UPOLY,		//�û������
        EN_GOTYPE_ROADSECTION,			//��ͼ��·����
        EN_GOTYPE_URUT,			//�û���·����
        EN_GOTYPE_ROUTE,		//���ߵ�������
        EN_GOTYPE_TRACK,		//������������
        EN_GOTYPE_ADDRESS,		//��ַ������
        
        EN_GOTYPE_LAST,
        EN_GOTYPE_COUNT = EN_GOTYPE_LAST - EN_GOTYPE_FIRST
    }EnGeoObjType;

public:
    struct ST_GEO_PTXY
    {
        float dx;
        float dy;
    };
    // ����Χ�ṹ(�󡢶� ���ҡ���)
    struct ST_GEO_RECT
    {
        float dLeft;
        float dTop;
        float dRight;
        float dBottom;
    };
//////////////////////////////////////////////////////////////////////////
public:
	CGIS_FeatureV2( EnGeoObjType enOType );
	virtual ~CGIS_FeatureV2();

    EnGeoObjType GetObjectType(){return m_eObjectType;};
//////////////////////////////////////////////////////////////////////////
protected:    
	EnGeoObjType m_eObjectType;				//���������
    BOOL m_bBuddyIndependent;
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	virtual void DumpState() const;		
	virtual void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////

};

















#endif // !defined(AFX_GIS_FEATUREV2_H__AE57F5C7_BAA5_42E2_94DD_5B6D3F1928A9__INCLUDED_)
