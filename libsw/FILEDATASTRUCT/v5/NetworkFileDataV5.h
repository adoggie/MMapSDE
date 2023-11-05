// NetworkFileDataV5.h: interface for the CNetworkFileDataV5 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKFILEDATAV5_H__490FB35A_D2C0_4D1B_A80E_13F708BE709B__INCLUDED_)
#define AFX_NETWORKFILEDATAV5_H__490FB35A_D2C0_4D1B_A80E_13F708BE709B__INCLUDED_

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
#pragma warning (disable : 4786)
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////

#include "./FileDataStruct/V5/NetworkFileStructV5.h"
class CGIS_NetworkRoadSectionV5;
class CGIS_NetworkVertexV5;
//////////////////////////////////////////////////////////////////////////
class CNetworkFileDataV5  
{
public:
	CNetworkFileDataV5();
	virtual ~CNetworkFileDataV5();

    BOOL Load(LPCTSTR strFileName);     
    BOOL Write(LPCTSTR strFileName);  
    BOOL ReSort();
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	virtual void DumpState();	
	virtual void AssertValid() const;	
#endif    
//////////////////////////////////////////////////////////////////////////
//���Ͷ���
public:
	typedef map<unsigned long,CGIS_NetworkRoadSectionV5*> MapOfRoadSection;
	typedef map<unsigned long,CGIS_NetworkVertexV5*> MapOfVertex;
    struct stuBreadthItem
    {
        MapOfRoadSection*	pRoadSectionContainer;
        MapOfVertex*		pVertexContainer;
        char*               pRoadSectionBuddy;
        char*               pVertexBuddy;
        
		StuNetworkFileStructV5::StuBreadthHeaderInfo*     pHeader;
        StuNetworkFileStructV5::StuIndexInfoOfBreadth*    pIndex;	//ͼ������
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
        StuNetworkFileStructV5::StuIndexInfoOfMap* pIndex;		    //��ͼ������Ϣ
        
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
    MapOfGeoMap* GetMapContainer(){ return m_pMapContainer; }
//-------------------------------------------------------------
protected:    
    CPtrList m_ListOfMap;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
protected:
    MapOfBreadth* m_pBreadthContainer;
public:
    MapOfBreadth* GetBreadthContainer(){ return m_pBreadthContainer; }
//////////////////////////////////////////////////////////////////////////

protected:
    BOOL WriteNetwork   (CFile* pFile);
    BOOL WriteMap       (CFile* pFile,stuMapItem* pMapItem);
    BOOL WriteBreadth   (CFile* pFile,stuBreadthItem* pBreadthItem);
 
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
protected:
    BOOL ReadNetwork(CFile* pFile);
    BOOL ReadMap(CFile* pFile,stuMapItem* pMapItem);
    BOOL ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem,int nLoadFlag);

//Read
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

        unsigned long m_nRoadSectionCount;
        unsigned long m_nVertexCount;
        unsigned long m_nEdgeVertexCount;

        unsigned long m_nTotalBeforeCompressSize;
        unsigned long m_nTotalAfterCompressSize;
        
        int     nCountOfSpecialSizePackageBeforeCompress[11];//    [0-100k),    [100k - 200k),    [200k-300k)...[1000k,+@@]
        int     nCountOfSpecialSizePackageAfterCompress [11];//    [0-100k),    [100k - 200k),    [200k-300k)...[1000k,+@@]   
    };

    CDataAnalyse m_DataAnalyse;
    unsigned long m_BreadthRoadSectionCount;
#endif
//////////////////////////////////////////////////////////////////////////    
protected:
    BOOL Check(LPCTSTR strFilePath);
    BOOL CheckEdgeVertex(CGIS_NetworkVertexV5* pVertex);

    unsigned short* m_pBreadthIDArray;
    unsigned long m_nSizeOfBreadthIDArray;



};

#endif // !defined(AFX_NETWORKFILEDATAV5_H__490FB35A_D2C0_4D1B_A80E_13F708BE709B__INCLUDED_)
