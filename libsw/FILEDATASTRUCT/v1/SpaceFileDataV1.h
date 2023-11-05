// SpaceFileDataV1.h: interface for the CSpaceFileDataV1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPACEFILEDATAV1_H__78D6B9D0_72D0_423B_B250_73DCB8513540__INCLUDED_)
#define AFX_SPACEFILEDATAV1_H__78D6B9D0_72D0_423B_B250_73DCB8513540__INCLUDED_

#include "./FileDataStruct/V1/SpaceFileStructV1.h"

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

#define MAX_BREADTH_COUNT 100000
class CGIS_Feature;
//////////////////////////////////////////////////////////////////////////
class CSpaceFileDataV1  
{
public:
	CSpaceFileDataV1();
	virtual ~CSpaceFileDataV1();
    BOOL Load(CStringArray& arFilePath);     
    BOOL write(LPCTSTR strFileName); 
  
    /**
     * @brief 读出省会城市和地级市的数据trace出来.
     *
     * @param 
     * @return
    */
    BOOL ReadCityPosData(CFile* pFile);
    BOOL InitialOperation();
    BOOL ClearOperation();
    
#ifdef _DEBUG
	virtual void DumpState();	
	virtual void AssertValid() const;	
#endif

//////////////////////////////////////////////////////////////////////////
//类型定义
public:
	typedef map<unsigned long,CGIS_Feature*> MapOfRoadSection;

	typedef map<unsigned long,CGIS_Feature*> MapOfFeature;
    typedef struct 
    {
        MapOfFeature* pFtrContainer;
        StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData* pIndex;
        unsigned long nLayerID;
        unsigned long nBreadthID;
        unsigned long nMapID;
    }stuLayerItem;
	typedef map<unsigned long,stuLayerItem*> MapOfLayer;
    
    typedef struct 
    {
        MapOfLayer* pLayerContainer;
        StuSpaceFileStructV1::StuIndexInfoOfBreadth*   pIndex;	//图幅索引
        unsigned long nBreadthID;
        unsigned long nMapID;
    }stuBreadthItem;
	typedef map<unsigned long,stuBreadthItem*> MapOfBreadth;

    typedef struct 
    {
        MapOfBreadth* pBreadthContainer;
        StuSpaceFileStructV1::StuIndexInfoOfMap* pIndex;		    //地图索引信息
        unsigned long nMapID;
    }stuMapItem;
    
	typedef map<unsigned long,stuMapItem*> MapOfGeoMap;
//////////////////////////////////////////////////////////////////////////
public:
    MapOfBreadth* GetBreadthContainer(){ return &m_BreadthContainer; }
    void SetMapContainer(MapOfGeoMap* pContainer){ m_pMapContainer = pContainer;};
//////////////////////////////////////////////////////////////////////////
//数据区
protected:
	CPtrList m_ListOfMap;
	CPtrList m_ListOfFeature;
    
	int m_LayerNewN2OldN    [MAX_BREADTH_COUNT][25];
	int m_LayerNewN2ID      [MAX_BREADTH_COUNT][25];
	int m_LayerNewN2Type    [MAX_BREADTH_COUNT][25];
	int m_LayerNewN2IconID  [MAX_BREADTH_COUNT][25];
	int m_LayerCount        [MAX_BREADTH_COUNT];

    CString m_strFilePath;
    
    StuSpaceFileStructV1::StuHeaderInfo*           m_pFileHeadInfo;				//文件头
    StuSpaceFileStructV1::StuIndexInfoOfMap*       m_pIndexInfoOfMap;		    //地图索引信息
    //------------------------------------------
    
    StuSpaceFileStructV1::StuIndexInfoOfBreadth*   m_pIndexInfoOfBreadth;	//图幅索引
    //------------------------------------------
    StuSpaceFileStructV1::StuIndexInfoOfLayerInBreadthData* m_pIndexInfoOfLayerInBreadthData;  //层索引区
    StuSpaceFileStructV1::StuHeaderInfoOfLayerInBreadthData*	m_pHeaderInfoOfLayerInBreadthData;	//层头

    MapOfBreadth m_BreadthContainer;
    MapOfGeoMap* m_pMapContainer;
//////////////////////////////////////////////////////////////////////////
protected:
	BOOL LoadIn2File( CString& strFName);
	BOOL ReadLayerAllInfoInBreadth(CFile* pFile,int nMapN,int nBreadthN);

    BOOL ReadSpaceData   (CFile* pFile);
	BOOL ReadMap    (CFile* pFile,stuMapItem* pMapItem);
	BOOL ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem);
	BOOL ReadLayer  (CFile* pFile,stuLayerItem* pLayerItem);
	BOOL ReadLayerV1(CFile* pFile,int nMapN,int nBreadthN,int nLayerN,CPtrList* pFtrList);

    BOOL WriteSpaceData   (CFile* pFile);
	BOOL WriteMap(CFile* pFile,stuMapItem* pItem);
	BOOL WriteBreadth(CFile* pFile,stuBreadthItem* pItem);
	BOOL WriteLayer(CFile* pFile,stuLayerItem* pItem);
 


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
#ifdef ANALYSE_DATA 
    class CDataAnalyse  
    {
    public:
        enum enConstValue
        {
            CELL_SIZE = 10*1024,
        };
    public:
        void Initial();
        void DumpState();

        //关于图幅的
        unsigned long m_nBreadthCount;
        unsigned long m_nEmptyBreadthCount;
        unsigned long m_nPackageCount;
        unsigned long m_nEmptyPackageCount;

        unsigned long m_nMinRoadSectionCountOfBreadth;
        unsigned long m_nMaxRoadSectionCountOfBreadth;
        
        unsigned long  m_dwMinBeforeCompressSize;
        unsigned long  m_dwMaxBeforeCompressSize;
        unsigned long  m_dwMinAfterCompressSize;
        unsigned long  m_dwMaxAfterCompressSize;
        
        int     nCountOfSpecialSizePackageBeforeCompress[11];//    [0-100k),    [100k - 200k),    [200k-300k)...[1000k,+@@]
        int     nCountOfSpecialSizePackageAfterCompress [11];//    [0-100k),    [100k - 200k),    [200k-300k)...[1000k,+@@]   
    };

    CDataAnalyse m_DataAnalyse;
    unsigned long m_BreadthRoadSectionCount;
#endif
//////////////////////////////////////////////////////////////////////////

};

#endif // !defined(AFX_SPACEFILEDATAV1_H__78D6B9D0_72D0_423B_B250_73DCB8513540__INCLUDED_)
