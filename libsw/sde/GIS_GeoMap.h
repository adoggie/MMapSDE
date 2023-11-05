
#ifndef _GIS_GEOMAP_H
#define _GIS_GEOMAP_H

#include "swBase.h"

#include "IGIS_MapView.h"
#include "IGIS_Map.h"
#include "GIS_MapInfo.h"
#include "IGIS_Message.h"
#include "IGIS_LayerFile.h"
#include "GIS_BreadthMap.h"
#include "IGIS_Tool.h"
#include "GIS_WayPoint.h"
#include "GIS_MapLayer.h"
#include "IGIS_MapNetwork.h"
#include "GIS_NetworkVtx.h"
#include "GIS_BufStorageNetwork.h"
#include "Afxmt.h"
#include "GIS_NetworkRut.h"
#include "GIS_PathpointNode.h"

struct ST_WPNRS_RINFO{
    ST_WPNRS_RINFO(){
		pFtrRutList = NULL;
		pRList = NULL;
		pRDiss = NULL;
		pPtProjs = NULL;
		pBMaps = NULL;
		nRNum = 0;
	}

    ~ST_WPNRS_RINFO(){Clear();};
//////////////////////////////////////////////////////////////////////////

	void InitWPNRuts( int nNum ){
		nRNum = nNum;
		pRList = NULL;
		pRDiss = NULL;
		pPtProjs = NULL;
		pBMaps = NULL;
		if( nRNum > 0 ){
			pRList = (CGIS_FeatureRut**)malloc(sizeof(CGIS_FeatureRut*)*nRNum);
			pRDiss = new float[nRNum];
			pPtProjs = new ST_GEO_PTXY[nRNum];
			pBMaps = (CGIS_BreadthMap**)malloc(sizeof(CGIS_BreadthMap*)*nRNum);
			for( int i=0;	i<nRNum;	i++ )
			{
				pRDiss[i] = 100000.0;
				pBMaps[i] = NULL;
			}
			memset( pRList, 0, sizeof(CGIS_FeatureRut*)*nRNum );
			memset( pPtProjs, 0, sizeof(ST_GEO_PTXY)*nRNum );
		}

        //////////////////////////////////////////////////////////////////////////
        if(nRNum > 0)
        {
            ASSERT(!pFtrRutList);
            pFtrRutList = new CPtrList;
        }
        //////////////////////////////////////////////////////////////////////////
        
	}
	void Clear( )
	{
		for( int i=0;	i<nRNum;	i++ )
        {
            //delete pRList[i];
        }
		if( pRList )
			free(pRList);
		if( pRDiss )
			delete[] pRDiss;
		if( pPtProjs )
			delete[] pPtProjs;
		if( pBMaps )
			free(pBMaps);
		nRNum = 0;
		pRList = NULL;
		pRDiss = NULL;
		pPtProjs = NULL;
		pBMaps = NULL;
        {
            if(pFtrRutList)
            {              
                POSITION pos = pFtrRutList->GetHeadPosition();
                while(pos)
                {
                    CGIS_FeatureRut* pRut = (CGIS_FeatureRut*)pFtrRutList->GetNext(pos);
                    ASSERT(pRut);
                    delete pRut;
                    pRut = NULL;
                }      
                
                pFtrRutList->RemoveAll();
                delete pFtrRutList;
                pFtrRutList = NULL;
            }

        }
	}
	CGIS_FeatureRut **pRList;
	float *pRDiss;
	ST_GEO_PTXY *pPtProjs;
	CGIS_BreadthMap **pBMaps;
	unsigned short nRNum;
	CPtrList* pFtrRutList;
};
class IGIS_MapNetwork;
class CGIS_MapBreadthNetwork;
class  CGIS_GeoMap : public IGIS_Map
{
public:
	CGIS_GeoMap();
	virtual ~CGIS_GeoMap();
public:
	std::list<IGIS_MapView*> _mapViewSet; //视图集合
	//CPtrList*  m_pMapViewSet;
	//CPtrList* m_LayerManagerSet;	//地图层集
	std::list<CGIS_LayerManager> _layerMgnSet;	//
	CGIS_LayerManager* getLayerManager();
public:
	//virtual BOOL Draw(IGIS_MapView* pMapView);
	virtual BOOL Attach(IGIS_MapView* pMapView);
	virtual BOOL Detache(IGIS_MapView* pMapView);
	virtual BOOL Notify(IGIS_Message* pMessage);
	virtual CGIS_LayerInfo* GetLayerInfo(UINT ILayerID);
//	virtual CGIS_MapInfo* GetMapInfo();
	virtual IGIS_TOOL* GetTool(EnTooType enToolType); 
	virtual BOOL ReleaseTool();
	virtual BOOL SaveToFile();
	virtual EnMatchRoad MatchRoad(ST_GEO_PTXY pt,ST_GEO_PTXY& Rpt,EnMathState enState,CGIS_FeatureRut** pLastRoat,int Routedis,BOOL bHighWay ,ST_GEO_PTXY ptMatchLast,int& nLeaveCount);

public:
	//用地图数据填充路线上该节点的数据以及下一段数据
	BOOL GetRutData( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut* pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos);
    //
	BOOL GetRutDataV3( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut* pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos,CPtrList* pNodeList,POSITION posOfNodeList);
	
    //用于Trunk与Detail的相接的NODE的填充,只填充空间数据,不构造NODE
    BOOL GetRutDataV2( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut* pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos);
	BOOL FullEVNode( CGIS_WayPoint *pWTwo, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, CPtrList *pPPList, POSITION &pos );

	BOOL Open(swString szFilePath);
	BOOL Load(swString szFilePath);
	BOOL UnLoad(UINT nMapID);
	BOOL LoadBreadthMap( CGIS_BreadthMap *pBMap );
	BOOL UnLoadBreadthMap( CGIS_BreadthMap *pBMap );
    /* @param pErrorCode[out] 0-无异常 1-未定义异常 2-发生了内存重置
     * @return
    */
	//BOOL LoadBLData( CGIS_BreadthMap *pBMap, CGIS_MapLayer **pLList, int nLNum ,unsigned int* pErrorCode = NULL);
	BOOL LoadBLData( CGIS_BreadthMap *pBMap, std::list<CGIS_MapLayer*>&,unsigned int* pErrorCode = NULL);
	BOOL UnLoadBLData( int nBID, CGIS_MapLayer *pLayer );
	BOOL LoadBLRutData( CGIS_BreadthMap *pBMap );
	BOOL UnLoadBLRutData( CGIS_BreadthMap *pBMap );
	
	void SetBreadthMapList( CGIS_BreadthMap* pBMapList );
	int GetBreadthMapNum();
	void SetBreadthMapNum( int nBMapNum );

	void SetBreadthMapRects( ST_GEO_RECT *pRTList = NULL, int nBNum = 0 );
	ST_GEO_RECT GetBreadthMapRect( CGIS_BreadthMap *pBMap );
	
	EnMapFileType GetMapFileType( );
//	EnMapType GetMapType();
//	void SetMapType( EnMapType enMapType );
	IGIS_MapNetwork* GetMapNetWork( );
	void SetMapNetWork( IGIS_MapNetwork *pMapNetwork );

////////////////////////////////////////////
	////计算途经点的路段
	BOOL CalWayPointRut( CGIS_WayPoint *pWayPoint, ST_GEO_PTXY ptEnd, int& nErrorCode,UINT nDisBuf = 100, UINT nCount = 0 );
	BOOL ClearStorageBMap( );

	void GetBVRuts( CPtrList *pVList, POSITION &pos, int nRID = -1 );
	CGIS_BreadthMap* GetBreadthMapByID( int nBID );
	CGIS_BreadthMap* GetBreadthMapByID2( int nBID );
	
	BOOL FullVRutInfo   ( CGIS_NetworkRut*  pRut, ST_VOICE_RUT*   pstVoiceRut );
	BOOL FullVNodeInfo  ( CGIS_FeatureRut*  pPreFRut, CGIS_PathpointNode*   pPNode,  int nREBID, int nEndRutID, ST_VOICE_NODE*  pstVoiceNode );
	BOOL InitVNodeInfo_DEL  ( ST_VOICE_NODE*    pstVoiceNode );
	//判断节点处出入道路路名/入口道路和出口道路是否为同一条道路
		//是否存在与出口道路同名的道路/是否存在与入口道路同名的道路
	void JudgeNodeRutNameInfo( CGIS_FeatureRut *pPreFRut, CGIS_PathpointNode *pPNode, ST_VOICE_NODE *pVNode );
	//返回最后一次匹配在道路上的位置
	ST_GEO_PTXY GetLastPtOnRoad();
	//返回车辆行驶方向
	int GetCarDirect();
	//返回车辆在所行驶道路上的下个节点
	short GetNextRutPt();
	//清空图幅缓存状态
	void ResetMapState();

private:
	///获得路线上该节点的路段信息及路线信息
	BOOL FullPathNode( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_BreadthMap *pBMap, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos );
	BOOL FullPathNodeV2( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_BreadthMap *pBMap, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos );
	///获取此ID的所有路段
	inline BOOL FullPathNodeArc( CGIS_Pathpoint *pPPoint, CGIS_FeatureRut *pFRut, ST_GEO_PTXY ptEnd, CGIS_BreadthMap *pBMap, BOOL bSequence, CPtrList *pPathpointList, POSITION &pos );

	///取出此图幅中所有路段信息
	BOOL GetRutObjList( CPtrList *pLMList, CGIS_BreadthMap *pBMap, CPtrList *pObjList );
	///取出此图幅中此路段信息
	BOOL GetBreadthMapCurRutObj( ST_GEO_PTXY ptStart, CGIS_BreadthMap *pBMap, int nRutID, CGIS_FeatureRut **pFRut );
	///查出与此图幅相邻图幅中此路段的地物
	BOOL GetRoundBMapRutObj( ST_GEO_PTXY pt, int nRutID, CGIS_BreadthMap **pBMap, CGIS_FeatureRut **pFRut );
	///获得点所在分幅地图
	BOOL GetBreadthMapOfPoint( float dx, float dy, CPtrList &BMapList );

	/////////////////////
	BOOL FindNearRut( ST_GEO_PTXY ptPos, CPtrList *pRutList, CGIS_BreadthMap *pBMap, double &dbMinDis, double dbDisBuf, ST_WPNRS_RINFO &wpRInfo );

	//////////
	BOOL FullPathNodeRutsV2( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM],    unsigned long nVertexMergeID, unsigned long nPreviousRoadSectionMergeID,unsigned long nNextRoadSectionMergeID, BYTE byRAType );
	
    BOOL JudgePathNodeRutsConfuse( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext);
	BOOL FullPathNodeRuts( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, BYTE byRAType );
	BOOL FullPathNodeRutsSVtx( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, ST_GEO_PTXY pPtList[], int &nPtNum );
	BOOL FullPathNodeRutsAss( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, ST_GEO_PTXY pPtList[], int &nPtNum );
	BOOL FullPathNodeRutsRound( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, ST_GEO_PTXY pPtList[], int &nPtNum );
	BOOL GetNextRoundRut( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], 
				CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRStart, CGIS_NetworkRut *pRNext, ST_GEO_PTXY pPtList[], int &nPtNum );
	BOOL GetNextAssRut( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_PathpointNode *pVNode, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], 
				CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, ST_GEO_PTXY pPtList[], int &nPtNum );
	//对车辆所在位置的图幅道路进行缓存
	void CalMatchBMap( ST_GEO_PTXY pt );
//	CGIS_MapInfo* m_MapInfo;
//	CPtrList	*m_pBMapList;	//分幅地图链表
//	CGIS_BreadthMap *m_pBMapList[190];
	ST_GEO_RECT *m_pRTList;	
	CGIS_BreadthMap *m_pBMapList;
	int m_nBMapNum;
	//道路层列表
	//CGIS_MapLayer *m_pRLList[MAX_RLAY_NUM];
	//BYTE m_nRLNum;
	std::list<CGIS_MapLayer*> _rl_list; //道路层集合

	EnMapFileType m_enMFType;
// 	EnMapType m_enMapType;
//	UINT m_nCounter;
//	IGIS_LayerFile *m_pLFile;
	//CPtrList* m_ToolSet;
	std::vector<IGIS_TOOL*> m_ToolSet;
	//	ST_GEO_PTXY m_MatchPt;
//	CGIS_FeatureRut* m_LastRoat;
//	UINT m_LastRoatID;
	IGIS_MapNetwork *m_pMapNetwork;

	CGIS_BreadthMap *m_pBMNet;
	//CGIS_MapLayer *m_pLList[MAX_RLAY_NUM];  //道路层7
	std::vector<CGIS_MapLayer*> m_pLList; 
	char * m_pLaneBlockAddr;
	CGIS_FeatureRut *m_pFRList[MAX_BRUT_NUM];
	short m_nFRLNum;

//	CGIS_BufStorageNetwork m_MNBStor;
	BYTE	m_byMRNum;		//路段间路段的个数
	CGIS_PathpointNode *m_pVNode;	//创建的Node节点
	POSITION m_posVNode;			//node放置的位置
	CGIS_FeatureRut	 *m_pFRPre;	//前一段路段
	int				 m_nAPre;	//前一段路段角度
	int				 m_nANext;	//
	BYTE			 m_bTrunFlag;	//0-顺行 1-非顺行
	BOOL			 m_bRFlag;	//是否为环岛的标志
	short			 m_nRMOrd;
	int				 m_nBID;	//加载路段数据所在的图幅ID（编号后）
	int				 m_nMatchBID; //为道路匹配缓存的某图幅的ID
	ST_GEO_PTXY      m_ptLastXy;	//最后一次匹配后的返回值
	int				 m_nReCount;	//车辆在单行线上往错误方向行驶的计数器值
	int				 m_nCarRunDirect;//车辆行驶方向1：路段头－》路段尾 2：路段尾－》路段头0：未确定
	short			 m_nNextRutPt;//车辆行驶所在道路的下一个拐点
//public:
//    void SetRoundaboutOrder(short nRoundaboutOrder)
//    {
//        m_nRMOrd = nRoundaboutOrder;
//    }
//    void SetRoundaboutFlag(BOOL	bRFlag)
//    {
//        m_bRFlag = bRFlag;
//    }
//    BOOL GetRoundaboutOrder(short& nRoundaboutOrder)
//    {
//        nRoundaboutOrder = m_nRMOrd;
//        return TRUE;
//    }
//    BOOL GetRoundaboutFlag(BOOL&	bRFlag)
//    {
//        bRFlag = m_bRFlag;
//        return TRUE;
//    }

};

#endif
