// SpaceFileDataV2.h: interface for the CSpaceFileDataV2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SPACEFILEDATAV2_ROADSECTION_H_)
#define _SPACEFILEDATAV2_ROADSECTION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4786 )  // Disable warning messages
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////

//#include "./FileDataStruct/V2/FileDataStructV2.h"
#include "./FileDataStruct/V2/SpaceFileStructV2.h"
//////////////////////////////////////////////////////////////////////////


class CGIS_FeatureRutV2;
class CGIS_FeatureV2;
//////////////////////////////////////////////////////////////////////////
class CSpaceFileDataV2_RoadSection  
{
public:
	CSpaceFileDataV2_RoadSection();
	virtual ~CSpaceFileDataV2_RoadSection();
    
    BOOL Load(LPCTSTR strFileName);     
    BOOL write(LPCTSTR strFileName); 
    BOOL InitialOperation();
    BOOL ClearOperation();
    
#ifdef _DEBUG
	virtual void DumpState();	
	virtual void AssertValid() const;	
#endif

//////////////////////////////////////////////////////////////////////////
//类型定义
public:
	typedef map<unsigned long,CGIS_FeatureV2*> MapOfFeature;
    typedef struct 
    {
        MapOfFeature* pFtrContainer;
        StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData* pIndex;//图层索引
        char* pBuddy;
    }stuLayerItem;
	typedef map<unsigned long,stuLayerItem*> MapOfLayer;
    
    typedef struct 
    {
        MapOfLayer* pLayerContainer;
        StuSpaceFileStructV2::StuIndexInfoOfBreadth*   pIndex;	//图幅索引
    }stuBreadthItem;
	typedef map<unsigned long,stuBreadthItem*> MapOfBreadth;

    typedef struct 
    {
        MapOfBreadth* pBreadthContainer;
        StuSpaceFileStructV2::StuIndexInfoOfMap* pIndex;		 //地图索引
    }stuMapItem;
	typedef map<unsigned long,stuMapItem*> MapOfGeoMap;
  
//////////////////////////////////////////////////////////////////////////
public:
    MapOfGeoMap* GetMapContainer(){ return m_pMapContainer; }
//////////////////////////////////////////////////////////////////////////
//数据区
protected:
    MapOfGeoMap* m_pMapContainer;
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL ReadSpaceData   (CFile* pFile);
	BOOL ReadMap        (CFile* pFile,stuMapItem* pItem);
	BOOL ReadBreadth    (CFile* pFile,stuBreadthItem* pBreadthItem);
	BOOL ReadLayer      (CFile* pFile,stuLayerItem* pLayerItem);

//////////////////////////////////////////////////////////////////////////
protected:
    unsigned long GetFtrCount() const;
};

#endif // !defined(_SPACEFILEDATAV2_ROADSECTION_H_)
