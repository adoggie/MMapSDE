// SpaceFileDataV7.h: interface for the CSpaceFileDataV7 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPACEFILEDATAV7_H__281EF405_54F1_48E0_86B4_7EA7A3213120__INCLUDED_)
#define AFX_SPACEFILEDATAV7_H__281EF405_54F1_48E0_86B4_7EA7A3213120__INCLUDED_

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
#include <vector>
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////

#include "./FileDataStruct/V7/SpaceFileStructV7.h"
class CGIS_FeatureRoadSectionV7;
//////////////////////////////////////////////////////////////////////////

class CSpaceFileDataV7  
{
public:
	CSpaceFileDataV7();
	virtual ~CSpaceFileDataV7();

    BOOL Load   (LPCTSTR strFilePath);     
    BOOL Write  (LPCTSTR strFilePath);  
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	virtual void DumpState();	
	virtual void AssertValid() const;	
#endif
protected:
    unsigned long GetFtrCount();

//////////////////////////////////////////////////////////////////////////
    
//////////////////////////////////////////////////////////////////////////
//类型定义
public:
    typedef map<unsigned long,CGIS_FeatureRoadSectionV7*> MapOfFeature;    
    typedef struct 
    {
        MapOfFeature* pFtrContainer;
        StuSpaceFileStructV7::StuIndexInfoOfBreadth*   pIndex;	//图幅索引
        char* pBuddy;

    }stuBreadthItem;
	typedef map<unsigned long,stuBreadthItem*> MapOfBreadth;

    typedef struct 
    {
        MapOfBreadth* pBreadthContainer;
        StuSpaceFileStructV7::StuIndexInfoOfMap* pIndex;		 //地图索引
    }stuMapItem;
//////////////////////////////////////////////////////////////////////////
public:
    MapOfBreadth* GetBreadthContainer(){ return m_pBreadthContainer; }
//////////////////////////////////////////////////////////////////////////
protected:
    //MapOfBreadth m_BreadthContainer;
    vector<unsigned long> m_SortIDArray;

    MapOfBreadth* m_pBreadthContainer;
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL WriteSpaceData   (CFile* pFile);
    BOOL WriteMap       (
        CFile* pFile,
        StuSpaceFileStructV7::StuIndexInfoOfMap* pIndexInfoOfMap);
    BOOL WriteBreadth   (
        CFile* pFile,
        StuSpaceFileStructV7::StuIndexInfoOfBreadth* pIndexInfoOfBreadth,
        MapOfFeature* pRoadSectionContainer);
//    BOOL WriteLayer( 
//        CFile* pFile,
//        StuSpaceFileStructV7::StuIndexInfoOfLayerInBreadthData*    pIndexInfoOfLayer,
//        MapOfFeature* pRoadSectionContainer);

protected:
    long Dichotomy(unsigned long* pnArr, int nNumElement, unsigned long nValue);
    long GetIndexByID(unsigned long nID);

    BOOL GetBuddyDataSize( 
        MapOfFeature* pRoadSectionContainer,
        int nType,
        unsigned long& nBuddyDataSize);
    
    BOOL CollectBuddyData(
        MapOfFeature* pRoadSectionContainer,
        char* pBuddy,
        int nType,
        unsigned long& nCharDataBuddyLength,
        unsigned long& nShortDataBuddyLength,
        unsigned long& nDwordDataBuddyLength);
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL ReadSpaceData   (CFile* pFile);
	BOOL ReadMap        (CFile* pFile,stuMapItem* pItem);
	BOOL ReadBreadth    (CFile* pFile,stuBreadthItem* pItem);

    
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
        
        int     nCountOfSpecialSizePackageBeforeCompress[11];//    [0-100k),    [100k - 200k),    [200k-300k)...[1000k,+@@]
        int     nCountOfSpecialSizePackageAfterCompress [11];//    [0-100k),    [100k - 200k),    [200k-300k)...[1000k,+@@]   
    };

    CDataAnalyse m_DataAnalyse;
    unsigned long m_BreadthRoadSectionCount;
#endif

};

#endif // !defined(AFX_SPACEFILEDATAV7_H__281EF405_54F1_48E0_86B4_7EA7A3213120__INCLUDED_)
