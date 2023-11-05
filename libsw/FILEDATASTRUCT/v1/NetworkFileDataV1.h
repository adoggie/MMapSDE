// NetworkFileDataV1.h: interface for the CNetworkFileDataV1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKFILEDATAV1_H__CD67E196_2157_416A_8FA9_2C8E82DC278A__INCLUDED_)
#define AFX_NETWORKFILEDATAV1_H__CD67E196_2157_416A_8FA9_2C8E82DC278A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4786)
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////

#include "./FileDataStruct/V1/NetworkFileStructV1.h"

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#ifdef ANALYSE_DATA
#undef ANALYSE_DATA
#endif
#define ANALYSE_DATA
#endif
//////////////////////////////////////////////////////////////////////////

class CGIS_NetworkRut;
class CGIS_NetworkVtx;
//////////////////////////////////////////////////////////////////////////
class CNetworkFileDataV1  
{
public:
	CNetworkFileDataV1();
	virtual ~CNetworkFileDataV1();
	
    BOOL Load(CStringArray& arFilePath);     
    BOOL Write(LPCTSTR strFileName);  
    
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
 	typedef map<unsigned long,CGIS_NetworkRut*> MapOfRoadSection;
	typedef map<unsigned long,CGIS_NetworkVtx*> MapOfVertex;

    typedef struct 
    {
        MapOfRoadSection*	pRoadSectionContainer;
        MapOfVertex*		pVertexContainer;
		StuNetworkFileStructV1::StuBreadthHeaderInfo* pHeader;
        StuNetworkFileStructV1::StuIndexInfoOfBreadth*   pIndex;	//图幅索引
        unsigned long       nMapID;
    }stuBreadthItem;
	typedef map<unsigned long,stuBreadthItem*> MapOfBreadth;

	typedef struct 
    {
        MapOfBreadth* pBreadthContainer;
        StuNetworkFileStructV1::StuIndexInfoOfMap* pIndex;		    //地图索引信息
    }stuMapItem;
	typedef map<unsigned long,stuMapItem*> MapOfGeoMap;
	//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
protected:
    MapOfGeoMap* m_pMapContainer;
public:
    BOOL GetMapContainer(MapOfGeoMap*& pMapContainer){ pMapContainer = m_pMapContainer; return TRUE;}
//-------------------------------------------------------------
protected:    
    CPtrList m_ListOfMap;
//////////////////////////////////////////////////////////////////////////
protected:

	BOOL LoadIn2File( CString& strFName);
    BOOL ReadNetwork(CFile* pFile);
    BOOL ReadMap(CFile* pFile,
		StuNetworkFileStructV1::StuIndexInfoOfMap* pIndexInfoOfMap,
		MapOfBreadth*& pBreadthContainer);
    BOOL ReadBreadth(
		CFile* pFile,
		StuNetworkFileStructV1::StuIndexInfoOfBreadth* pIndexInfoOfBreadth,
		MapOfRoadSection*& pRoadSectionMap,
		MapOfVertex*& pVertexMap,
		BYTE nLoadFlag//1-只加载第一等级,3-加载一二等级,7,全部加载.
        );
    BOOL ReadBreadthHeadInfo(
		CFile* pFile,
		StuNetworkFileStructV1::StuIndexInfoOfBreadth* pIndexInfoOfBreadth,
		StuNetworkFileStructV1::StuBreadthHeaderInfo* pBreadthHeader,
		int nLoadFlag);
	void ClearBreadth(
		MapOfRoadSection*& pRoadSectionMap,
		MapOfVertex*& pVertexMap);

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
protected:
    unsigned long GetRoadSectionCount(int nLevel);
    unsigned long GetVertexCount(int nLevel);
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_DATA
    class CDataAnalyse  
    {
    public:
        void Initial();
        void DumpState();

        unsigned long m_nMaxBeforeCompressSize;
        unsigned long m_nMaxAfterCompressSize;

        //关于图幅的
        unsigned long m_nBreadthCount;
        unsigned long m_nEmptyBreadthCount;
        
        //关于节点的
        unsigned long m_nVertexCount;
        unsigned long m_nSideVertexCount;
        
        unsigned long m_nVertexWithSpecialEdgeCount[11];
        unsigned long m_nVertexCountOfHighLevel;
        unsigned long m_nVertexCountOfMidLevel;
        unsigned long m_nVertexCountOfLowLevel;
        
        //关于路段的
        unsigned long m_nRoadSectionCount;
        unsigned long m_nRoadSectionCountOfHighLevel;
        unsigned long m_nRoadSectionCountOfMidLevel;
        unsigned long m_nRoadSectionCountOfLowLevel;      
    };

    CDataAnalyse m_DataAnalyse;
#endif
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL Check();
    BOOL CheckEdgeVertex(CGIS_NetworkVtx* pVertex);
    
    BOOL ExtractNetwork();
    BOOL ExtractNetwork_Vertex(CGIS_NetworkVtx* pFtr);

    BOOL WriteBreadthFtrCount();
//////////////////////////////////////////////////////////////////////////
protected:
   	CGIS_BreadthMap*    m_pArrayOfBreadthMap;
    int                 m_nBreadthCount;
    CGIS_BreadthMap* GetBreadthMapByID( int nRawBreadthID );

};

#endif // !defined(AFX_NETWORKFILEDATAV1_H__CD67E196_2157_416A_8FA9_2C8E82DC278A__INCLUDED_)
