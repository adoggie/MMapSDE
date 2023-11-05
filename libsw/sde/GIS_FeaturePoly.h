
#ifndef _GIS_FEATURE_POLY_H
#define _GIS_FEATURE_POLY_H

#include "GIS_Feature.h"
class swEXPORT CGIS_FeaturePoly : public CGIS_Feature {
public:
	CGIS_FeaturePoly(EnGeoObjType enOType);
	virtual ~CGIS_FeaturePoly();

public:
	virtual CGIS_Feature* Clone();
	virtual CGIS_Feature* CloneV2();
	int  GetPartNum( );
	int* GetPart( );
	void SetRectObj( ST_GEO_RECT rtObj );
	ST_GEO_RECT GetRectObj( );
	void SetCenterPt( float dx, float dy );
	ST_GEO_PTXY GetCenterPt();
	void SetPolyShowClass( BYTE nShowClass );
	BYTE GetPolyShowClass( );
  BOOL SetPartInfo(int nLength,int* pList);
  void SetPartInfoDirect(short nLength,int* pList);
	void SetPolyID( int nID );
	int GetPolyID();

#ifdef MEMORY_SELF_MANAGE
public:
	void* operator new(size_t size);
	void* operator new(size_t size,CGIS_FeaturePoly* pObjAddr);
	void* operator new(size_t size,IMemoryMng2* pMemoryMng);
	void operator delete (void*,size_t);
#endif	
protected:  
    BOOL SetPartInfoAt(IMemoryMng* pMemoryMng,int nLength,int* pList);
private:
	short			m_nPartNum;
	int			*m_pPart;
	ST_GEO_RECT	m_rtObj;
	ST_GEO_PTXY	m_CenterPt;
	//ÏÔÊ¾¼¶±ð
	BYTE m_nShowClass;
	int m_nID;
};

#endif 
