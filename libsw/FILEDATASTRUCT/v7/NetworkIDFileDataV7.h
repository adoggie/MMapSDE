// NetworkIDFileDataV7.h: interface for the CNetworkIDFileDataV7 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKIDFILEDATAV7_H__35468147_E388_453C_B2A9_6CD9858055B1__INCLUDED_)
#define AFX_NETWORKIDFILEDATAV7_H__35468147_E388_453C_B2A9_6CD9858055B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4786)
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////

#include "NetworkIDFileStructV7.h"
//////////////////////////////////////////////////////////////////////////

class CNetworkIDFileDataV7  
{
public:
	CNetworkIDFileDataV7();
	virtual ~CNetworkIDFileDataV7();

    BOOL Load(LPCTSTR strFileName);     
    BOOL Write(LPCTSTR strFileName);  


//////////////////////////////////////////////////////////////////////////
public:
    
    typedef map<unsigned long,unsigned long> MapOfID; 
    typedef map<unsigned long,unsigned long> MapOfRoadSection; 
    typedef map<unsigned long,unsigned long> MapOfVertex; 

    struct stuBreadthItem
    {
        MapOfRoadSection*	pRoadSectionContainer;
        MapOfVertex*        pVertexContainer;

        StuNetworkIDFileStructV7::StuBreadthHeaderInfo*     pHeader;
        StuNetworkIDFileStructV7::StuIndexInfoOfBreadth*    pIndex;	//图幅索引
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

        StuNetworkIDFileStructV7::StuIndexInfoOfMap* pIndex;		    //地图索引信息
        
        stuMapItem()
        {
            memset(this,0,sizeof(stuMapItem));
        };
    };
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
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL ReadNetwork(CFile* pFile);
    BOOL ReadMap(CFile* pFile,stuMapItem* pMapItem);
    BOOL ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem,int nLoadFlag);
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL WriteNetworkV2   (CFile* pFile);
    BOOL WriteMapV2       (CFile* pFile,stuMapItem* pMapItem);

};

#endif // !defined(AFX_NETWORKIDFILEDATAV7_H__35468147_E388_453C_B2A9_6CD9858055B1__INCLUDED_)
