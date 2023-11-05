
#ifndef _GIS_FEATURE_H
#define _GIS_FEATURE_H

#include "swBase.h"

class IMemoryMng;
class IMemoryMng2;
//////////////////////////////////////////////////////////////////////////


#include "GeoGlobe.h"
#include "GIS_Attribute.h"
#include "../MemoryMngLib/IMemoryMng.h"
#include "../MemoryMngLib/IMemoryMng2.h"
class swEXPORT CGIS_Feature  {
public:
	CGIS_Feature( EnGeoObjType enOType );
	virtual ~CGIS_Feature();

	virtual long GetFeatureSize() {return 0;};//Added by HB.Deng 12/07/2005
	virtual CGIS_Feature* Clone();
	//设置和获取标注的信息
	//获取位置信息
	ST_GEO_PTXY* GetPtList( );
	unsigned short  GetPtNum( );
    //////////////////////////////////////////////////////////////////////////
    BOOL SetAnnotationInfo(int nLength,char* strText);
    BOOL SetPtInfo(int nLength,ST_GEO_PTXY* pList);
    void SetAnnotationInfoDirect(int nLength,char* strText);
    void SetPtInfoDirect(int nLength,ST_GEO_PTXY* pList);
    //////////////////////////////////////////////////////////////////////////
    
	EnGeoObjType GetObjType( );
	short GetAnnoNum( );
	//char* GetAnnotation( );
	swString GetAnnotation( );
	int m_nBreadthID;
protected:  
    BOOL SetAnnotationInfoAt(IMemoryMng* pMemoryMng,int nLength,char* strText);
    BOOL SetPtInfoAt(IMemoryMng* pMemoryMng,int nLength,ST_GEO_PTXY* pList);

protected:
	short		m_nAnno;
	char*		m_bstrAnno;						//控制点标注信息
	ST_GEO_PTXY	*m_pPtList;				//控制点列表
  unsigned short	m_nPtNum;
	EnGeoObjType m_enOType;				//对象的类型

#ifdef MEMORY_SELF_MANAGE
public:
    static IMemoryMng* m_pMemMngOfSelf;
    static IMemoryMng* m_pMemMngOfFeaturePoint;
    static IMemoryMng* m_pMemMngOfFeatureLine;
    static IMemoryMng* m_pMemMngOfFeaturePoly;
    static IMemoryMng* m_pMemMngOfFeatureRut;
    static IMemoryMng* m_pMemMngOfBuddyData;

    static IMemoryMng* m_pForQueryMemMngOfSelf;
    static IMemoryMng* m_pForQueryMemMngOfFeaturePoint;
    static IMemoryMng* m_pForQueryMemMngOfFeatureLine;
    static IMemoryMng* m_pForQueryMemMngOfFeaturePoly;
    static IMemoryMng* m_pForQueryMemMngOfFeatureRut;
    static IMemoryMng* m_pForQueryMemMngOfBuddyData;
    
    static IMemoryMng* m_pForNetworkMemMngOfSelf;
    static IMemoryMng* m_pForNetworkMemMngOfFeaturePoint;
    static IMemoryMng* m_pForNetworkMemMngOfFeatureLine;
    static IMemoryMng* m_pForNetworkMemMngOfFeaturePoly;
    static IMemoryMng* m_pForNetworkMemMngOfFeatureRut;
    static IMemoryMng* m_pForNetworkMemMngOfBuddyData;

    static DWORD m_dwOverflowTickCount;
public:
	DWORD m_nMemmoryType;		//0-从系统堆上分配 1-从空间数据堆上分配 2-从查询堆上分配 3.从路网堆上分配
#endif	
//////////////////////////////////////////////////////////////////////////
};

inline swString CGIS_Feature::GetAnnotation( )
{
	return swString::FromUTF8(m_bstrAnno);
}

inline ST_GEO_PTXY* CGIS_Feature::GetPtList( )
{
	return m_pPtList;
}

inline unsigned short  CGIS_Feature::GetPtNum( )
{
	return m_nPtNum;
}

inline EnGeoObjType CGIS_Feature::GetObjType( )
{
	return m_enOType;
}
inline CGIS_Feature* CGIS_Feature::Clone()
{
   // ASSERT(FALSE);
	return NULL;
}
inline short CGIS_Feature::GetAnnoNum( )
{
	return m_nAnno;
}

#endif 
