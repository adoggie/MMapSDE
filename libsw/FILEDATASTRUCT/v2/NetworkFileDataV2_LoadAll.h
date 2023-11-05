// NetworkFileDataV2.h: interface for the CNetworkFileDataV2_LoadAll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NETWORKFILEDATAV2_LOADALL_H_)
#define _NETWORKFILEDATAV2_LOADALL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4786)
#include <map>
using namespace std;
//////////////////////////////////////////////////////////////////////////

#include "./FileDataStruct/V2/NetworkFileStructV2C.h"
class CGIS_NetworkRoadSectionV2;
class CGIS_NetworkVertexV2;
//////////////////////////////////////////////////////////////////////////

class CNetworkFileDataV2_LoadAll  
{
public:
	CNetworkFileDataV2_LoadAll();
	virtual ~CNetworkFileDataV2_LoadAll();

    BOOL Load(LPCTSTR strFileName);     
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
 	typedef map<unsigned long,CGIS_NetworkRoadSectionV2*> MapOfRoadSection;
	typedef map<unsigned long,CGIS_NetworkVertexV2*> MapOfVertex;

	typedef map<unsigned long,MapOfRoadSection*> MapOfBreadthOfRoadSection;
	typedef map<unsigned long,MapOfVertex*> MapOfBreadthOfVertex;
	typedef map<unsigned long,char*> MapOfBreadthOfRoadSectionBuddy;
	typedef map<unsigned long,char*> MapOfBreadthOfVertexBuddy;

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

	typedef struct 
    {
        MapOfBreadth* pBreadthContainer;
        StuNetworkFileStructV2C::StuIndexInfoOfMap* pIndex;		    //地图索引信息
    }stuMapItem;
	typedef map<unsigned long,stuMapItem*> MapOfGeoMap;
//////////////////////////////////////////////////////////////////////////
public:
    MapOfGeoMap* GetMapContainer(){ return m_pMapContainer; }
    //MapOfBreadthOfRoadSection*  GetBreadthContainerOfRoadSection()  {   return &m_BreadthContainerOfRoadSection;}
    //MapOfBreadthOfVertex*       GetBreadthContainerOfVertex()       {   return &m_BreadthContainerOfVertex;    }
//////////////////////////////////////////////////////////////////////////
protected:
    //原始路网图幅容器
    //--------------------------------------------------------------------
    MapOfBreadthOfRoadSection   m_BreadthContainerOfRoadSection;  //容纳所有路网路段的容器对象
	MapOfBreadthOfVertex        m_BreadthContainerOfVertex;       //容纳所有路网节点的容器对象.

	MapOfBreadthOfRoadSectionBuddy  m_BreadthBuddyOfRoadSection;
	MapOfBreadthOfVertexBuddy       m_BreadthBuddyOfVertex;
protected:
    MapOfGeoMap* m_pMapContainer;
//////////////////////////////////////////////////////////////////////////
protected:    
    BOOL ReadNetwork(CFile* pFile);
    BOOL ReadMap(CFile* pFile,stuMapItem* pMapItem);
    BOOL ReadBreadth(CFile* pFile,stuBreadthItem* pBreadthItem,int nLoadFlag);

    BOOL ExtractNetwork();
    BOOL ExtractNetwork_Vertex(CGIS_NetworkVertexV2* pFtr);

//////////////////////////////////////////////////////////////////////////
protected:
    unsigned long GetRoadSectionCount();
    unsigned long GetVertexCount();

};

#endif // !defined(_NETWORKFILEDATAV2_LOADALL_H_)
