
#ifndef _GIS_MBBNETWORK_H
#define _GIS_MBBNETWORK_H

#include "swBase.h"
#include <Afxtempl.h>
#include "../FileDataStruct/V7/GIS_FeatureRoadSectionV7.h"
#include "../FileDataStruct/V7/GIS_FeatureRoadV7.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CGIS_NetworkRoadSectionV7;
class CGIS_NetworkVertexV7;
class CGeoView;
//////////////////////////////////////////////////////////////////////////

typedef struct
{
	char chSign[0x10];
	unsigned long nMapCount;
}StuHeaderInfo;
typedef struct
{
	unsigned long nID;              //地图的ID
	unsigned long nOffset;          //地图的文件偏移
	unsigned long nBreadthCount;    //地图内的图幅的数量
}StuIndexInfoOfMap;
typedef struct
{
	unsigned long BreadthMapID;
	unsigned long BreadthMapOffset;
	unsigned long nAfterCompressSize;
	unsigned long nBeforeCompressSize;

}StuIndexInfoOfBreadth;
typedef struct{
	unsigned char	nLCnt;	//图幅加载计数器
}StuBreadthMapInfo;
typedef struct ST_GEO_SNINFO
{
	CGIS_NetworkRoadSectionV7*  pRoadSection;
	CGIS_NetworkVertexV7*       pVertex;
	float dGValue;
    float dFValue;
	ST_GEO_SNINFO *pParent;

    ST_GEO_SNINFO(){memset(this,0,sizeof(ST_GEO_SNINFO));}
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////    
}CGIS_NetworkNodeV7;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "GIS_NetworkPathInfo.h"
#include "GIS_NetworkNode.h"
#include "GIS_GeoMap.h"
#include "FileDataStruct/v7/NetworkFileStructV7.h"
const unsigned short NAV_MAX_BCBNUM = 400;
    //////////////////////////////////////////////////////////////////////////
    typedef CMap<unsigned long,unsigned long,CGIS_NetworkRoadSectionV7*,CGIS_NetworkRoadSectionV7*> MapOfRoadSection;
	typedef CMap<unsigned long,unsigned long,CGIS_NetworkVertexV7*,CGIS_NetworkVertexV7*> MapOfVertex;

    struct stuBreadthItem
    {
        MapOfRoadSection*	pRoadSectionContainer;
        MapOfVertex*		pVertexContainer;
        char*               pRoadSectionBuddy;
        char*               pVertexBuddy;
        
        stuBreadthItem()
        {
            memset(this,0,sizeof(stuBreadthItem));
        };

    };
	typedef CMap<unsigned long,unsigned long,stuBreadthItem*,stuBreadthItem*> MapOfBreadth;

//////////////////////////////////////////////////////////////////////////

class IMemoryMng;
//////////////////////////////////////////////////////////////////////////
class CGIS_MBBNetwork  
{
public:
	CGIS_MBBNetwork();
	virtual ~CGIS_MBBNetwork();
//////////////////////////////////////////////////////////////////////////
public:
    enum enErrorCode
    {
        ERROR_NONE,
        ERROR_UNDEFINED,
        ERROR_OUT_OF_MEMORY,
        ERROR_NETWORK_DISCONNECT,
        ERROR_FILE_IO,
        ERROR_STOP_BY_OUTSIDE,
        ERROR_INVALID_START_VERTEX,
        ERROR_INVALID_END_VERTEX,
    };

    /**
     * @brief 在主干路网上进行运算
     *
     * @param pStartNode:开始结点(使用详图上的数据结构)
     * @param pEndNode:终止结点(使用详图上的数据结构)
     * @param pPathInfo:计算参数(使用详图上的数据结构)
     * @param pErrorCode:错误码
     *
     * @return  BOOL
    */
	BOOL CalculateOptimalPathMid(
        CGIS_NetworkNode *pStartNode,   //开始结点
        CGIS_NetworkNode *pEndNode,     //终止结点
        CGIS_NetworkPathInfo *pPathInfo,//计算参数
        CPtrList *pPathPointListMid, //填充的路径空间数据
        enErrorCode* pErrorCode); 
//////////////////////////////////////////////////////////////////////////
protected:
	BOOL CalculateOptimalPathMidActual( 
        CGIS_NetworkNodeV7 *pSNode, 
        CGIS_NetworkNodeV7 *pENode,
        CGIS_NetworkPathInfo *pPathInfo,
        CPtrList *pPMid ,
        enErrorCode* pErrorCode); 
	BOOL CalculateOptimalPathMidActualV2( 
        CGIS_NetworkNodeV7 *pSNode, 
        CGIS_NetworkNodeV7 *pENode,
        CGIS_NetworkPathInfo *pPathInfo,
        CPtrList *pPMid ,
        enErrorCode* pErrorCode); 
//////////////////////////////////////////////////////////////////////////
//InitialOperation and ClearOperation
public:
    BOOL InitialOperation(enErrorCode* pErrorCode);
    int ClearOperation();
    BOOL IfUpNode(int nDetailBreadthID,int nDetailVertexID,enErrorCode* pErrorCode);

    int LoadBreadthDataV2( unsigned long nBID );
	CGIS_NetworkRoadSectionV7* GetNetworkRut( unsigned long nOGID, unsigned long nRID );
	CGIS_NetworkVertexV7* GetNetworkVtx( unsigned long nOGID, unsigned long nVID );
	void SetGeoMap( CGIS_GeoMap *pMap );

protected:
    BOOL ClearNodeList(CPtrList* pNodeList);
    BOOL ClearUnnecessaryNetwork();
    BOOL MemoryInitial();
	BOOL MemoryClear();


    BOOL LoadNetWorkMapInfo(enErrorCode* );
	BOOL LoadSpaceMapInfo(enErrorCode* );
    BOOL FreeNetWorkMapInfo(void);
	BOOL FreeSpaceMapInfo(void);

    BOOL LoadNetWorkIDInfo(unsigned long nDetailBreadthID,enErrorCode* );
    BOOL FreeNetWorkIDInfo();

//////////////////////////////////////////////////////////////////////////
	BOOL LoadBreadthData( unsigned long nBID,enErrorCode* pErrorCode);
	int FreeBreadthData( unsigned long nBID );
    int FreeNetwork();
    BOOL CouldReachAnotherVertexInRoadSection( CGIS_NetworkVertexV7 *pVtx, CGIS_NetworkRoadSectionV7 *pRut );

	CGIS_NetworkVertexV7* GetNextRutVtx( CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRut, BOOL bHead );
	CGIS_NetworkVertexV7* GetNextRutVtx2( CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRut, BOOL bHead );
	CGIS_NetworkVertexV7 *GetVAbutVtx( CGIS_NetworkVertexV7 *pVertex );
	StuBreadthMapInfo* GetNWBInfo( unsigned long nBID );
	
	void ParseBMapID( int nBMID, int &nBMXNum, int &nBMYNum );
	BOOL DetailFeatureID2TrunkFeatureID(int nDetailBreadthID,unsigned short nDetailFeatureID, unsigned long& nTrunkFeatureID);
	BOOL ExtendNode(
        CGIS_NetworkNodeV7*,
        CPtrList *pOList, 
		ST_GEO_PTXY ptEnd,
        CGIS_NetworkPathInfo *pPathInfo,
        BYTE byLRNum,
        unsigned short *pLRList,
        BYTE nRLevel, 
        BOOL bCFlag,
        BOOL bSFlag,
        enErrorCode* pErrorCode);
	BOOL CalculateOptimalPathRut(
        CPtrList *pOList, 
        CPtrList *pCList,
        unsigned long *pVIDs, 
        int *pVBIDs,
        unsigned short &nVNum, 
		ST_GEO_PTXY ptEnd,
        CGIS_NetworkPathInfo *pPathInfo,
        BYTE byLRNum,
        unsigned short *pLRList,
        BYTE nRLevel, 
        BOOL bCFlag,
        BOOL bSFlag,
        enErrorCode* pErrorCode);
	BOOL CalculateOptimalPathRutV2(
        CPtrList *pOList, 
        CPtrList *pCList,
        unsigned long *pVIDs, 
        int *pVBIDs,
        unsigned short &nVNum, 
		ST_GEO_PTXY ptEnd,
        CGIS_NetworkPathInfo *pPathInfo,
        BYTE byLRNum,
        unsigned short *pLRList,
        BYTE nRLevel, 
        BOOL bCFlag,
        BOOL bSFlag,
        enErrorCode* pErrorCode);
	void InsertNode(CGIS_NetworkNodeV7* pNode,CPtrList *pList, BOOL bHead = TRUE );
	BOOL CalculateNetworkNode( CGIS_NetworkNodeV7 *pNode, CPtrList *pOList, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead );
	BOOL CalculateRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
    //////////////////////////////////////////////////////////////////////////
    //权值计算函数族
    BOOL CalculateCarRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead );
	BOOL CalculateTruckRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead );
	BOOL CalculateBusRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	BOOL CalculateEmergencyRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	BOOL CalculateTaxiRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	BOOL CalculateDeliveryRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	BOOL CalculateBicycleRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
	BOOL CalculatePedestrianRutVal( CGIS_NetworkNodeV7 *pNode, CGIS_NetworkPathInfo *pPathInfo, ST_GEO_PTXY ptEnd, BYTE byAngle, BOOL bHead  );
    //////////////////////////////////////////////////////////////////////////
	StuNetworkFileStructV7::StuIndexInfoOfBreadth* GetBreadthIndexByBreadthID(unsigned long nBID);

private:
    
	int				m_nBNum;
	StuIndexInfoOfBreadth	*m_pIIBList;
	int				m_nBCnt;
	unsigned long	*m_pBIDs;
	CGIS_NetworkRoadSectionV7   **m_pRutList[NAV_MAX_BCBNUM];
	CGIS_NetworkVertexV7        **m_pVertexList[NAV_MAX_BCBNUM];
    char*              m_pRoadSectionBuddyList[NAV_MAX_BCBNUM]; 
    char*              m_pVertexBuddyList[NAV_MAX_BCBNUM];      
    
    char* m_pBreadthMapLoadCount;

        
	unsigned short m_nRutNum[NAV_MAX_BCBNUM];
	unsigned short m_nVtxNum[NAV_MAX_BCBNUM];




	CGIS_GeoMap *m_pMap;
	double m_dbMaxDis;          //起点终点间的最大距离
	unsigned char m_byCRType;//0,短距离 1.长距离.(1-4)

	BYTE			 m_bTFlay;	//TRUE-报连续机动 FALSE-不报连续机动
	int				 m_nAPre;	//当前NODE的前一段路段角度
	int				 m_nANext;	//当前NODE的后一段路段角度
    //是否为环岛的标志
	BOOL			 m_bRoundaboutFlag;	
    
    //环岛出口计数
	short			 m_nRoundaboutOrder;
	BYTE	m_byMRNum;		//路口内路段的个数(如机动为U拐时此值会累加为3)

    
	CGIS_PathpointNode*             m_pVoiceNode;	//创建的Node节点
	CGIS_FeatureRoadSectionV7*      m_pFRPre;	//前一段路段

//////////////////////////////////////////////////////////////////////////
protected:
    //MapOfBreadth* m_pBreadthContainer;
	//判断节点处出入道路路名/入口道路和出口道路是否为同一条道路
		//是否存在与出口道路同名的道路/是否存在与入口道路同名的道路
	void JudgeNodeRutNameInfo( CGIS_FeatureRoadSectionV7 *pPreFRut, CGIS_PathpointNode *pPNode, ST_VOICE_NODE *pVNode );
	//判断节点处转弯方向和类型
	void JudgeNodeAngleAndTurn( int nAngle, BYTE &byAngle, BYTE &byTurn );
	BOOL InitVNodeInfo( ST_VOICE_NODE *pVNode );
	BOOL FullVoiceNodeInfo( CGIS_FeatureRoadSectionV7 *pPreFRut, CGIS_PathpointNode *pPNode, ST_VOICE_NODE *pVNode );
	BOOL FullVoiceRutInfo( CGIS_NetworkRoadSectionV7 *pRut, ST_VOICE_RUT *pVRut );
	/////填充路径的数据
	int FullPathMData(CPtrList *pRList,CPtrList *pVertexList,CPtrList *pPMid);
	int FullPathMDataV2(CPtrList *pRList,CPtrList *pVertexList,CPtrList *pPMid);
	//加载图幅中的空间属性数据
	int LoadSpaceBreadthData(char* pBufOfBeforeCompress,int nBufferSize,int nBreadthID,CGIS_FeatureRoadSectionV7 **pFRList, int &nFNum );
	int FreeSpaceBreadthData(int nBreadthID,CGIS_FeatureRoadSectionV7 **pFRList, int &nFNum );
	BOOL FullPathNodeArc( CGIS_Pathpoint *pPPoint, CGIS_FeatureRoadSectionV7 *pFRut, ST_GEO_PTXY ptEnd, BOOL bSequence, CPtrList *pPathpointList, POSITION &pos );
	BOOL FullPathNode( CGIS_FeatureRoadSectionV7 **pFRList, CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRPre, CGIS_NetworkRoadSectionV7 *pRNext, CPtrList *pPathpointList, POSITION &pos );
	BOOL FullPathNodeV2( CGIS_FeatureRoadSectionV7 **pFRList, CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRPre, CGIS_NetworkRoadSectionV7 *pRNext, CPtrList *pPathpointList, POSITION &pos );
	//填充路段上的数据
	void GetRutData( CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRPre, CGIS_NetworkRoadSectionV7 *pRNext, CGIS_FeatureRoadSectionV7 **pFRList, CPtrList *pPMid, POSITION &pos );
	void GetRutDataV2( CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRPre, CGIS_NetworkRoadSectionV7 *pRNext, CGIS_FeatureRoadSectionV7 **pFRList, CPtrList *pPMid, POSITION &pos );
	BOOL FullPathNodeRuts( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRPre, CGIS_NetworkRoadSectionV7 *pRNext, BYTE byRAType );
	BOOL FullPathNodeRutsSVtx( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRPre, CGIS_NetworkRoadSectionV7 *pRNext, ST_GEO_PTXY pPtList[], int &nPtNum );
	BOOL FullPathNodeRutsAss( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRPre, CGIS_NetworkRoadSectionV7 *pRNext, ST_GEO_PTXY pPtList[], int &nPtNum );
	BOOL GetNextAssRut( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], 
			CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRPre, ST_GEO_PTXY pPtList[], int &nPtNum );
	BOOL FullPathNodeRutsRound( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRPre, CGIS_NetworkRoadSectionV7 *pRNext, ST_GEO_PTXY pPtList[], int &nPtNum );
	BOOL GetNextRoundRut( CGIS_PathpointNode *pVNode, CGIS_FeatureRoadSectionV7 *pFRList[MAX_BRUT_NUM], 
			CGIS_NetworkVertexV7 *pVertex, CGIS_NetworkRoadSectionV7 *pRStart, CGIS_NetworkRoadSectionV7 *pRNext, ST_GEO_PTXY pPtList[], int &nPtNum );
//////////////////////////////////////////////////////////////////////////
public:
    struct stuPairIDItem
    {
        unsigned long nSourceID;
        unsigned long nDestID;
    };

    struct stuMergeIDArray
    {
        unsigned long   nArraySize;       
        stuPairIDItem* pArray;
        stuMergeIDArray()
        {
            nArraySize = 0;
            pArray = NULL;
        }
        ~stuMergeIDArray()
        {
            ASSERT(nArraySize == 0);
            ASSERT(!pArray);

            nArraySize = 0;
            pArray = NULL;
        }
        stuPairIDItem* GetDestIDBySourceID(unsigned long nSourceID);
    };


    unsigned long m_nDetailBreadthIDOfCurrentIDArray;
    stuMergeIDArray m_arRoadSectionIDArrayDetailToTrunk;
    stuMergeIDArray m_arVertexIDArrayDetailToTrunk;

    stuMergeIDArray m_arRoadSectionIDArrayTrunkToDetail;
    stuMergeIDArray m_arVertexIDArrayTrunkToDetail;

protected:
    BOOL LoadNetWorkIDInfoActual(unsigned long nDetailBreadthID,enErrorCode* ,stuMergeIDArray*,stuMergeIDArray* );
    BOOL FreeNetWorkIDInfoActual(stuMergeIDArray*,stuMergeIDArray*);
  
    stuPairIDItem* GetRoadSectionMergeIDByDetailID (unsigned long nDetailID)
    {
        return m_arRoadSectionIDArrayDetailToTrunk.GetDestIDBySourceID(nDetailID);       
    }
    stuPairIDItem* GetVertexMergeIDByDetailID      (unsigned long nDetailID)
    {
        return m_arVertexIDArrayDetailToTrunk.GetDestIDBySourceID(nDetailID);    
    }
    stuPairIDItem* GetRoadSectionDetailIDByMergeID (unsigned long nMergeID)
    {
        return m_arRoadSectionIDArrayTrunkToDetail.GetDestIDBySourceID(nMergeID);       
    }
    stuPairIDItem* GetVertexDetailIDByMergeID      (unsigned long nMergeID)
    {
        return m_arVertexIDArrayTrunkToDetail.GetDestIDBySourceID(nMergeID);   
    }
    

public:
#ifndef _WIN32_WCE
    stuPairIDItem* GetRoadSectionMergeIDByDetailIDV2(unsigned long nDetailID);
    stuPairIDItem* GetVertexMergeIDByDetailIDV2(unsigned long nDetailID);
#endif
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//lijun add this code for debug
//start
#ifdef _DEBUG
public:
	CGeoView* m_pMapView;
	//由于模块状态的关系,应用不可以调用此接口,只能由sde内部使用
	void SetMapViewSrc(CGeoView* pView)
	{
		m_pMapView = pView;
	}
	CGeoView* GetMapView()
	{
		 return m_pMapView;
	}


    //-------------------------------------------------------------
    BOOL TestCalculateOptimalPath();
    //-------------------------------------------------------------
#endif
//end
//////////////////////////////////////////////////////////////////////////
public:
    static IMemoryMng* m_pMemMngOfNetworkData;
protected:
    BOOL CheckCalculateCondition(CGIS_NetworkNode *pSNode,CGIS_NetworkNode *pENode,enErrorCode* pErrorCode);

//////////////////////////////////////////////////////////////////////////
protected:
    CGIS_FeatureRoadSectionV7*  m_pFRList[1000]; 
    CGIS_FeatureRoadSectionV7   m_pFtrRoadSectionArray[1000]; 

    char* m_pBeforeCompressBuffer;
    char* m_pAfterCompressBuffer;
    int m_nSizeOfBeforeCompressBuffer;
    int m_nSizeOfAfterCompressBuffer;
public:
    void SetUnCompressBuffer(char* pBeforeBuffer,char* pAfterBuffer,int nSizeOfBefore,int nSizeOfAfter)
    {
        m_pBeforeCompressBuffer = pBeforeBuffer;
        m_pAfterCompressBuffer = pAfterBuffer;
        m_nSizeOfBeforeCompressBuffer = nSizeOfBefore;
        m_nSizeOfAfterCompressBuffer = nSizeOfAfter;
    }
};

#endif 
