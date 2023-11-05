// NetworkFileDataV2.h: interface for the CNetworkFileDataV2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKFILEDATAV2_H__13A6E33D_E1DA_46E8_BAFA_1813957F8B1F__INCLUDED_)
#define AFX_NETWORKFILEDATAV2_H__13A6E33D_E1DA_46E8_BAFA_1813957F8B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#ifdef ANALYSE_RUNTIME
#undef ANALYSE_RUNTIME
#endif
#define ANALYSE_RUNTIME
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4786)
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////
#include <string>

#include "./FileDataStruct/v2/NetworkFileStructV2C.h"

class CGIS_NetworkRoadSectionV2;
class CGIS_NetworkVertexV2;
class CSpaceFileDataV2_RoadSection;
class CGIS_FeatureRoadSectionV2;
//////////////////////////////////////////////////////////////////////////
class CNetworkFileDataV2  
{
public:
	CNetworkFileDataV2();
	virtual ~CNetworkFileDataV2();
    void SetLoadLayerSign(int nSign)
    {
        m_nLoadLayerSign = nSign;
    }
    BOOL Load(LPCTSTR strFileName);     
    BOOL Write(LPCTSTR strFileName);  
    
    BOOL Extract();//
    BOOL Analyse();//加载并分析结果
    BOOL AnalyseWithSpaceData(CSpaceFileDataV2_RoadSection*);//加载并分析结果

   
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
 	typedef map<unsigned long,CGIS_NetworkRoadSectionV2*> MapOfRoadSection;
	typedef map<unsigned long,CGIS_NetworkVertexV2*> MapOfVertex;

    struct stuBreadthItem
    {
        MapOfRoadSection*	pRoadSectionContainer;
        MapOfVertex*		pVertexContainer;
        char*               pRoadSectionBuddy;
        char*               pVertexBuddy;
		StuNetworkFileStructV2C::StuBreadthHeaderInfo*     pHeader;
        StuNetworkFileStructV2C::StuIndexInfoOfBreadth*    pIndex;	//图幅索引
        unsigned long nID;

        stuBreadthItem()
        {
            memset(this,0,sizeof(stuBreadthItem));
        };

    };
	typedef map<unsigned long,stuBreadthItem*> MapOfBreadth;

	struct stuMapItem
    {
        MapOfBreadth* pBreadthContainer;
        StuNetworkFileStructV2C::StuIndexInfoOfMap* pIndex;		    //地图索引信息

        stuMapItem()
        {
            memset(this,0,sizeof(stuMapItem));
        };
    };
	typedef map<unsigned long,stuMapItem*> MapOfGeoMap;
//////////////////////////////////////////////////////////////////////////
protected:
    MapOfGeoMap* m_pMapContainer;
public:
    MapOfGeoMap* GetMapContainer(){return m_pMapContainer;};
    BOOL AddBreadthItem(stuBreadthItem* pBreadthItem);
    BOOL AddMapContainer(MapOfGeoMap* pMapContainer);
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL ReadNetwork(CFile* pFile);
    BOOL ReadMap(CFile* pFile,stuMapItem* pMapItem);
    BOOL ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem,int nLoadFlag);

    BOOL ReadBreadthHeadInfo(
		CFile* pFile,
		StuNetworkFileStructV2C::StuIndexInfoOfBreadth* pIndexInfoOfBreadth,
		StuNetworkFileStructV2C::StuBreadthHeaderInfo* pBreadthHeader,
		int nLoadFlag);
//-------------------------------------------------------------
    BOOL WriteNetwork   (CFile* pFile);
    BOOL WriteMap       (
        CFile* pFile,
        StuNetworkFileStructV2C::StuIndexInfoOfMap* pIndexInfoOfMap,
        MapOfBreadth* pBreadthContainer);
    BOOL WriteBreadth   (
		CFile* pFile,
        stuBreadthItem* pItem);

    BOOL CollectBuddyDataOfRoadSection(
        MapOfRoadSection* pFtrContainer,
        char* pBuddy,
        unsigned long& nCharDataBuddyLength,
        unsigned long& nShortDataBuddyLength,
        unsigned long& nDwordDataBuddyLength);
    BOOL CollectBuddyDataOfVertex(
        MapOfVertex* pFtrContainer,
        char* pBuddy,
        unsigned long& nCharDataBuddyLength,
        unsigned long& nShortDataBuddyLength,
        unsigned long& nDwordDataBuddyLength);

    BOOL GetBuddyDataOfRoadSectionSize(
        MapOfRoadSection* pFtrContainer,
        unsigned long& nBuddyDataSize);
    BOOL GetBuddyDataOfVertexSize(
        MapOfVertex* pFtrContainer,
        unsigned long& nBuddyDataSize);
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
protected:
    unsigned long GetRoadSectionCount();//0.根据加载的数据
    unsigned long GetVertexCount();
#endif

    //////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_RUNTIME 
    class CRunTimeAnalyse  
    {
    protected:
        enum enConstValue
        {
            CELL_SIZE = 10*1024,
        };
    public:
        void Initial();
        void DumpState();


        //////////////////////////////////////////////////////////////////////////
        //time spending
        //////////////////////////////////////////////////////////////////////////
        //space spending

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


        unsigned long m_nEdgeExtractCount;
        unsigned long m_nCountOfExtractVertexWhichHave3Edge;
        
        int     nCountOfSpecialSizePackageBeforeCompress[11];//    [0-100k),    [100k - 200k),    [200k-300k)...[1000k,+@@]
        int     nCountOfSpecialSizePackageAfterCompress [11];//    [0-100k),    [100k - 200k),    [200k-300k)...[1000k,+@@]       
    };

    CRunTimeAnalyse m_RunTimeAnalyse;
    
    unsigned long m_BreadthRoadSectionCount;
#endif
//////////////////////////////////////////////////////////////////////////
//Analyse 用数据
    class CDataAnalyse  
    {
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
        
        unsigned long m_nFtrCount;
        unsigned long m_nEdgeVertexCount;
        unsigned long m_nVertexWithSpecialEdgeCount[11];

        
        //////////////////////////////////////////////////////////////////////////
        //对路段的统计
        typedef map <int,int> MapOfInt_2Dimension;
        typedef map <int,MapOfInt_2Dimension*> MapOfInt_3Dimension;
        typedef map <int,MapOfInt_3Dimension*> MapOfInt_4Dimension;
        typedef map <CString,int> MapOfString_2Dimension;
        typedef map <CString,MapOfString_2Dimension*> MapOfString_3Dimension;
        class CRoadSectionData
        {
        public:
            MapOfInt_4Dimension LevelCount;
            MapOfInt_4Dimension TypeCount;
            MapOfInt_4Dimension LayerCount;
        };
        CRoadSectionData* m_pRoadSectionData;

        //节点的模式:
        //第一维:路段数,
        //第二维,层次数
        //第三维,特定层次的数段数.
        class CVertexData
        {
        public:
            MapOfString_3Dimension m_PatternContainer1;
            MapOfString_3Dimension m_PatternContainer2;//只根据路段类型进行的统计
            MapOfString_3Dimension m_PatternContainer3;//根据路段类型,方向,收费的统计
            MapOfString_3Dimension m_PatternContainer3Match;//根据路段方向,收费进行的可合并统计.
        };

        CVertexData* m_pVertexData;

        unsigned long m_nTempCount;

    };

    CDataAnalyse m_DataAnalyse;
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL CheckEdgeVertex(CGIS_NetworkVertexV2* pVertex);
    BOOL CheckNormalVertex(CGIS_NetworkVertexV2* pVertex);
    BOOL Check();
    
    BOOL ExtractNetwork();
    BOOL ExtractNetwork_Vertex(CGIS_NetworkVertexV2* pFtr);
    BOOL AnalyseNetwork();

    BOOL WriteBreadthFtrCount();

//////////////////////////////////////////////////////////////////////////
protected:
    int m_nLoadLayerSign;//0-第一等级 1-一二等级 2-一二三等级
//////////////////////////////////////////////////////////////////////////
//analyse
    BOOL AnalyseVertex(stuBreadthItem*,CGIS_NetworkVertexV2*);

    CSpaceFileDataV2_RoadSection* m_pSpaceFile;
    CGIS_FeatureRoadSectionV2* GetFtrRoadSection(int nBreadthID,int nRoadSectionID);

};

#endif // !defined(AFX_NETWORKFILEDATAV2_H__13A6E33D_E1DA_46E8_BAFA_1813957F8B1F__INCLUDED_)
