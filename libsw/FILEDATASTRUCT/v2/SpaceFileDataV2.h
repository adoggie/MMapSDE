// SpaceFileDataV2.h: interface for the CSpaceFileDataV2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPACEFILEDATAV2_H__5CCCF6F1_2631_4919_B576_64AF6F50AA7C__INCLUDED_)
#define AFX_SPACEFILEDATAV2_H__5CCCF6F1_2631_4919_B576_64AF6F50AA7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#ifdef ANALYSE_DATA
#undef ANALYSE_DATA
#endif
#define ANALYSE_DATA
#endif
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4786 )  // Disable warning messages
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////

#include "./FileDataStruct/V2/SpaceFileStructV2.h"
class CGIS_FeatureV2;
//////////////////////////////////////////////////////////////////////////

class CSpaceFileDataV2  
{
public:
	CSpaceFileDataV2();
	virtual ~CSpaceFileDataV2();
    
    BOOL Load(LPCTSTR strFileName);     
    BOOL write(LPCTSTR strFileName); 

    BOOL Analyse();//加载并分析结果

    BOOL InitialOperation();
    BOOL ClearOperation();
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	virtual void DumpState();	
	virtual void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////
//类型定义
public:
	typedef map<unsigned long,CGIS_FeatureV2*> MapOfRoadSection;

	typedef map<unsigned long,CGIS_FeatureV2*> MapOfFeature;
    typedef struct 
    {
        MapOfFeature* pFtrContainer;
        StuSpaceFileStructV2::StuIndexInfoOfLayerInBreadthData* pIndex;
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
        StuSpaceFileStructV2::StuIndexInfoOfMap* pIndex;		    //地图索引信息
    }stuMapItem;
	typedef map<unsigned long,stuMapItem*> MapOfGeoMap;
//////////////////////////////////////////////////////////////////////////
public:
    //MapOfBreadth* GetBreadthContainer(){ return &m_BreadthContainer; }
    void SetMapContainer(MapOfGeoMap* pContainer){ m_pMapContainer = pContainer;};
//////////////////////////////////////////////////////////////////////////
//数据区
protected:

    //MapOfBreadth m_BreadthContainer;
    MapOfGeoMap* m_pMapContainer;
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL ReadSpaceData   (CFile* pFile);
	BOOL ReadMap(CFile* pFile,stuMapItem* pItem);
	BOOL ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem);
	BOOL ReadLayer(CFile* pFile,stuLayerItem* pItem);

    BOOL WriteSpaceData   (CFile* pFile);
	BOOL WriteMap(CFile* pFile,stuMapItem* pItem);
	BOOL WriteBreadth(CFile* pFile,stuBreadthItem* pItem);
	BOOL WriteLayer(CFile* pFile,stuLayerItem* pItem);
 
//////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_DATA 
    class CDataAnalyse  
    {
    protected:
        enum enConstValue
        {
            CELL_SIZE = 10*1024,
        };
    public:
        void Initial();
        void DumpState();

        unsigned long m_nMapCount;
        unsigned long m_nBreadthCount;
        unsigned long m_nEmptyBreadthCount;
        unsigned long m_nPackageCount;
        unsigned long m_nEmptyPackageCount;

        unsigned long m_nMinFtrCountOfBreadth;
        unsigned long m_nMaxFtrCountOfBreadth;
        
        unsigned long  m_dwMinBeforeCompressSize;
        unsigned long  m_dwMaxBeforeCompressSize;
        unsigned long  m_dwMinAfterCompressSize;
        unsigned long  m_dwMaxAfterCompressSize;

        unsigned long m_nFtrCount;
        unsigned long m_nMinPointCountOfFeature;
        unsigned long m_nMaxPointCountOfFeature;
        
        typedef map <int,int> MapOfInt_2Dimension;

        MapOfInt_2Dimension* m_pPointCountContainer;
        MapOfInt_2Dimension* m_pCountOfSpecialSizePackageBeforeCompressContainer;
        MapOfInt_2Dimension* m_pCountOfSpecialSizePackageAfterCompressContainer;

    };

    CDataAnalyse m_DataAnalyse;
    unsigned long m_BreadthRoadSectionCount;
#endif

//////////////////////////////////////////////////////////////////////////
protected:
    unsigned long GetFtrCount();

    BOOL GetBuddyDataSize( 
        MapOfFeature* pFtrContainer,
        int nType,
        unsigned long& nBuddyDataSize);
    
    BOOL CollectBuddyData(
        MapOfFeature* pFtrContainer,
        char* pBuddy,
        int nType,
        unsigned long& nCharDataBuddyLength,
        unsigned long& nShortDataBuddyLength,
        unsigned long& nDwordDataBuddyLength);

//////////////////////////////////////////////////////////////////////////

    BOOL AnalyseSpaceData();

};

#endif // !defined(AFX_SPACEFILEDATAV2_H__5CCCF6F1_2631_4919_B576_64AF6F50AA7C__INCLUDED_)
