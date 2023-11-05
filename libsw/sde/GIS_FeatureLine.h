
#ifndef _GIS_FEATURELINE_H
#define _GIS_FEATURELINE_H


#include "GIS_Feature.h"

class swEXPORT CGIS_FeatureLine : public CGIS_Feature  {
public:
	CGIS_FeatureLine(EnGeoObjType enOType);
	virtual ~CGIS_FeatureLine();
public:
	virtual CGIS_Feature* Clone();
	virtual CGIS_Feature* CloneV2();

	int* GetPart( );
	int  GetPartNum( );
    
	void SetRectObj( ST_GEO_RECT rtObj );
	ST_GEO_RECT GetRectObj( );
  BOOL SetPartInfo(int nLength,int* pList);
  void SetPartInfoDirect(short nLength,int* pList);

#ifdef MEMORY_SELF_MANAGE
public:
	void* operator new(size_t size);
	void* operator new(size_t size,CGIS_FeatureLine* pObjAddr);
    void* operator new(size_t size,IMemoryMng2* pMemoryMng);
	void operator delete (void*,size_t);
#endif	

protected:  
    BOOL SetPartInfoAt(IMemoryMng* pMemoryMng,int nLength,int* pList);
private:
	short				m_nPartNum;
	int		*			m_pPart;
	ST_GEO_RECT	m_rtObj; // max boundary
};

#endif
