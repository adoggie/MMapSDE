
#ifndef _GIS_MAPBREADTH_NETWORK_H
#define _GIS_MAPBREADTH_NETWORK_H


#include "swBase.h"
#ifdef _DEBUG
#include "GeoView.h"
#endif
//////////////////////////////////////////////////////////////////////////

#include "IGIS_MapNetwork.h"
#include "GIS_BreadthMap.h"


const int NAV_MAX_BNETNUM = 100000;
const int NAV_MAX_LBNNUM = 768;

class CGIS_MBBNetwork;
//////////////////////////////////////////////////////////////////////////
class CGIS_MapBreadthNetwork : public IGIS_MapNetwork  
{
public:
	CGIS_MapBreadthNetwork();
	virtual ~CGIS_MapBreadthNetwork();

public:
	///加载地图网络
	virtual BOOL LoadMapNetwork( );
	///卸载地图网络
	virtual BOOL UnLoadMapNetwork( CGIS_BreadthMap *pBMap = NULL );
	///计算导航路径
	virtual int CalculateNavigatePath( CPtrList* WayPointList,CPtrList* pPathpointlist );

	//从network的节点链表中查找节点
    CGIS_NetworkVtx* GetNetworkVtx( int nVBID, int nID )
    {       
        if( nID >= m_nVtxNum[nVBID] )
            return NULL;
        return m_pVtxList[nVBID][nID];             
    }
	//从network的路段链表中查找路段
    CGIS_NetworkRut* GetNetworkRut( int nRBID, int nID )
    {
        if( nID >= m_nRutNum[nRBID] )
            return NULL;
        return m_pRutList[nRBID][nID];
    }

	//清理网格数据
	void ClearNetwork( );
	//清理指定的图幅风格数据
	void ClearBNetwork( CGIS_BreadthMap *pBMap );
	//清理网格数据
	void ResetNetwork( );


    //////////////////////////////////////////////////////////////////////////
    //图幅加载函数
    /**
     * @brief 以返回值和错误码表示加载结果,无异常抛出
    */
	BOOL ReadBreadthNetwork( CGIS_BreadthMap **pBNet, BYTE byLFlag ,int& nErrorCode );
 
    /**
     * @brief 加载图幅并且读出各个层的ID上限.
    */
    BOOL ReadBreadthNetworkV2( 
    CGIS_BreadthMap **pBNet, BYTE byLFlag,
    int& nLayer0RoadSectionMax, int& nLayer1RoadSectionMax,int& nLayer2RoadSectionMax,
    int& nLayer0VertexMax, int& nLayer1VertexMax,int& nLayer2VertexMax);

    /**
     * @brief 根据路段的ID加载路段数据
    */
    BOOL ReadBreadthNetworkRoadSectionByIDArray(CGIS_BreadthMap **pBreadthMap, BYTE byRoadSectionNum,unsigned short* pArrayOfRoadSectionID,int& nErrorCode);
    //////////////////////////////////////////////////////////////////////////

    //返回-1表示计算失败.
	int GetWPProjRut( CGIS_WayPoint *pWayPoint, IGIS_Map *pMap, CGIS_FeatureRut **pRList, CGIS_BreadthMap **pBMaps, ST_GEO_PTXY *pPPtList, int nRNum, ST_GEO_PTXY ptEnd ,int& nErrorCode );
	///得到下个节点
	CGIS_NetworkVtx* GetNextRutVtx( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, BOOL bHead = TRUE );
	BOOL CouldReachAnotherVertexInRoadSection( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut);
protected:
	int CalculateNavigatePathActual( CPtrList* WayPointList,CPtrList* pPathpointlist );

    
	BOOL FullWayPointData( CGIS_WayPoint *pWayPoint, CGIS_GeoMap *pMap, ST_GEO_PTXY ptEnd,int& nErrorCode );
	int FullWayPointData2( CGIS_WayPoint *pWayPoint, CGIS_GeoMap *pMap, CGIS_NetworkPathInfo *pPPInfo, ST_GEO_PTXY ptEnd,    int& nErrorCode );
	///获得要计算路径的途径点的节点信息
	BOOL GetRealVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CGIS_NetworkVtx **pStartVtx, CGIS_NetworkVtx **pEndVtx );
	
	///计算最优路段(*****)
	///这两个接口听功能并无太大差异.
	BOOL CalculateOptimalPathRut12( 
		CPtrList *pOList, 
		CPtrList *pCList, 
		unsigned short *pVIDs, 
		int *pVBIDs, 
		unsigned short &nVNum, 
		CGIS_NetworkVtx *pEndVtx,
		CGIS_NetworkPathInfo *pPathInfo, 
		BYTE nRLevel = 6, 
		BOOL bCFlag = TRUE, 
		BOOL bSFlag = FALSE );

	BOOL CalculateOptimalPathRut(
		CPtrList *pOList, 
		CPtrList *pCList,
		CGIS_NetworkVtx *pMVtx, 
		CGIS_NetworkVtx *pEndVtx, 
		CGIS_NetworkPathInfo *pPathInfo, 
        CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfo = NULL,
		BYTE nRLevel = 6, 
		BOOL bCFlag = TRUE, 
		BOOL bSFlag = FALSE );


    void ExtendEdgeNode(    
	CGIS_NetworkNode *pParentNode,
    CPtrList *pOpenList, 
	CGIS_NetworkVtx *pEndVtx, 
    CGIS_NetworkPathInfo *pPathInfo, 
    BYTE nRLevel, 
    BOOL bFromHeadFlag) ;
    void ExtendNormalNode(	CGIS_NetworkNode *pParentNode,
    CPtrList *pOpenList, 
	CGIS_NetworkVtx *pEndVtx, 
    CGIS_NetworkPathInfo *pPathInfo, 
    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup, 
    BYTE nRLevel, 
    BOOL bFromHeadFlag,
    BOOL bSearchStartNodeSign);


	///得到下个节点
// 	CGIS_NetworkVtx* GetNextRutVtx( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, BOOL bHead = TRUE );
	CGIS_NetworkVtx* GetNextRutVtx2( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, BOOL bHead = TRUE );
	///填充起始waypoint路段信息
	BOOL FullWPStartPath( CGIS_WayPoint *pWPOne, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, CGIS_GeoMap *pMap, BOOL &bRutSame, CPtrList *pPathPointList, POSITION &pos );
	BOOL FullWPStartPathV2( CGIS_WayPoint *pWPOne, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, CGIS_GeoMap *pMap, BOOL &bRutSame, CPtrList *pPathPointList, POSITION &pos );
	///填充终止waypoint路段信息
	BOOL FullWPEndPath_DELETE( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, int nRutID, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///填充终止waypoint路段信息
	BOOL FullWPEndPathV2( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	BOOL FullWPEndPathV3( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///填充路线pathleg数据
	BOOL FullPathLegData( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CPtrList *pRutList, CPtrList *pVtxList, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
	///填充路线当两waypoint在同一路段的情况
	BOOL FullWPOneAndWPTwoOnSameRut( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///填充路线当两waypoint在同一节点的情况
	BOOL FullWPOneAndWPTwoOnSameVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	//填充长距离计算时起始端和终止端的航线信息。
	int FullPathHeadTailData( CGIS_GeoMap *pMap, CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkNode *pNStart, CGIS_NetworkNode *pNEnd,     CPtrList* pPathMid, CPtrList* pPathPointList);
	///获得点所在分幅地图
	BOOL GetBreadthMapOfPoint( float dx, float dy, CPtrList &BMapList );
	//获得对应ID的分幅路网
	CGIS_BreadthMap *GetBreadthMapOfNetwork( int nNID );
	CGIS_BreadthMap *GetBreadthMapOfNetwork2( int nBID );
	CGIS_NetworkVtx *GetVAbutVtx( CGIS_NetworkVtx *pVtx );
	void ClearHeadNetwork( CGIS_NetworkNode *pSNode, CGIS_NetworkNode *pENode, CPtrList *pSOList, CPtrList *pSCList, CPtrList *pEOList, CPtrList *pECList );
	void ClearNodeNetwork( CPtrList *pRList, CPtrList *pVList, CPtrList *pNList );
	void ClearNodeNetwork( CGIS_NetworkNode *pNode, CPtrList *pRList, CPtrList *pVList );
	BOOL FullPathNodeRuts( CPtrList *pRList, CPtrList *pVList, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, BYTE byRAType );
	BOOL FullPathNodeRutsSVtx( CPtrList *pRList, CPtrList *pVList, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext );
	BOOL FullPathNodeRutsAss( CPtrList *pRList, CPtrList *pVList, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext );
	BOOL GetNextAssRut( CPtrList *pRList, CPtrList *pVList, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre );
	BOOL FullPathNodeRutsRound( CPtrList *pRList, CPtrList *pVList, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext );
	BOOL GetNextRoundRut( CPtrList *pRList, CPtrList *pVList, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRStart, CGIS_NetworkRut *pRNext );
	///////
    
    void ResetNodeStateInNodeList   ( CPtrList *pNodeList );
    void ClearNWNList( CPtrList *pNList );
	void ClearNWHead( CGIS_NetworkNode *pNStart, CGIS_NetworkNode *pNEnd, CPtrList *pSONList, CPtrList *pSCNList, CPtrList *pEONList, CPtrList *pECNList );
	void GetNWNodeRVInfo( CGIS_NetworkNode *pNode, CPtrList *pCNList, CPtrList *pRList, CPtrList *pVList );
	CGIS_NetworkNode* GetHeadNode( CPtrList *pONList, CPtrList *pCNList, BYTE byRLevel,BYTE byValidRoadSectionCount = 2 );
	CGIS_NetworkNode* PeekHeadNode( CPtrList *pONList, CPtrList *pCNList, BYTE byRLevel,BYTE byValidRoadSectionCount = 2 );
	void AddNWNode( CGIS_NetworkNode *pNode, CPtrList *pCNList );
	void CalculateRouteBuf( ST_GEO_PTXY ptStart, ST_GEO_PTXY ptEnd, ST_GEO_PTXY pRBuf[], int &nRBNum );
	void ClearNWBreadth( int *pLBID, int nLBNum );
	void ClearInvalNetwork(int *pSLBID, int nSLBNum, int nSMLBNum, int *pELBID, int nELBNum, int nEMLBNum );
	int Clear( int nVal );
protected:

	CGIS_NetworkRut **m_pRutList[NAV_MAX_BNETNUM];
	CGIS_NetworkVtx **m_pVtxList[NAV_MAX_BNETNUM];
	unsigned short m_nRutNum[NAV_MAX_BNETNUM];
	unsigned short m_nVtxNum[NAV_MAX_BNETNUM];


	char m_byErrCode;		//此变量只在路网数据加载中被赋值,与CGIS_NetworkException的错误码相同
                            

	BYTE m_LoadNetworkFlag;		//1、全部加载，2、加载1和2级路网，3、加载一级路网[参数型成员变量]
	BYTE m_byRLevel;	//道路等级
	
	BYTE m_byCFlag;                         //1-正向 2-反向
	BYTE m_byClear;

	int m_nMaxSLBNum;
	int m_nMaxELBNum;
	int m_pSLBID[NAV_MAX_LBNNUM];	//长距离计算时已加载图幅ID
	int m_nSLBNum;					//已加载图幅个数
	int m_pELBID[NAV_MAX_LBNNUM];	//长距离计算时已加载图幅ID
	int m_nELBNum;					//已加载图幅个数
    
	int m_nBID;						//添加机动附近路段时记录图幅id

//////////////////////////////////////////////////////////////////////////
    BOOL ExitJudge();
//////////////////////////////////////////////////////////////////////////
//lijun add this code for debug
#ifdef _DEBUG
//----------------------
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

//----------------------	
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//lijun add this code for use trunk network
protected:
    CGIS_MBBNetwork* m_pTrunkNetwork;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//070429 把路网数据包解压需要的空间转为在路网空间区内部分配.以减少对静态空间和内存空间的依赖.
protected:
    char m_pBufOfBeforeCompress[500000];
    char m_pBufOfAfterCompress[300000];
//////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //070512 add this code for 终点在限行路段上
    CGIS_NetworkRut* m_pDestRut;
    //////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
    //07-05-31添加此代码
public:
    struct stuTacticOfSearchPathHead
    {

        int nRoadSectionLevel;            //路段的最高LEVEL
        int nMinRoadSectionLevel;            //探路的最低功能级别,此级别以下的路不做处理

        CPtrList*               pLimitRoadSectionList;  //禁行的路段列表(在下一阶段的搜索失败导致上一次的结果变为禁行)
        CPtrList*               pLimitNodeList;         //限用的NODE列表(在下一阶段的搜索失败导致上一次的结果变为不可用)

        //ResidentialAreaNetwork//居民区路网
        //LowNetwork//低等级路网
        //HighNetwork;//高等级路网
        //////////////////////////////////////////////////////////////////////////
        //居民区路网向上探索用的参数
        BOOL EnableLimitInfo;
        BOOL EnableDirInfo;
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        //低等级路网向上探索用的参数
        //
        struct stuParaForSearchUpNode
        {
        CPtrList* pHeadForbidNodeList;//起点向上探索的禁入点(会根据高等级路网的运算结果修改)
        CPtrList* pTailForbidNodeList;//终点向上探索的禁入点(会根据高等级路网的运算结果修改)
            int nCountOfLinkedHignRoadSection;      //上高等级路网的点连接的可通行路段数要大于此值
            int nCountOfAppropriateNodeUpperLimit;  //至多找几个符合条件的node
            int nCountOfAppropriateNodeLowerLimit;  //至少找几个符合条件的node
            BOOL bProjectOnLine;                    //上高等级路网的点是否要求投影在由起点到目标点构成的线段上,以免环的出现
        };
        stuParaForSearchUpNode stParaForSearchUpNode;
        //
        //////////////////////////////////////////////////////////////////////////



        //////////////////////////////////////////////////////////////////////////
        //双向搜索中间交汇时的参数//0-搜索起始上高速的node 1-搜索终止上高速的NODE,2-双向时的NODE
        int nRLevel[3];
        int nLoadNetworkFlag[3];
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        CPtrList* pOpenListForHeadSearchExitOfResidentialArea;
        CPtrList* pClosListForHeadSearchExitOfResidentialArea;
        CPtrList* pOpenListForTailSearchExitOfResidentialArea;
        CPtrList* pClosListForTailSearchExitOfResidentialArea;
        
        CPtrList* pOpenListForSearchHeadNodeUpToTrunk;
        CPtrList* pClosListForSearchHeadNodeUpToTrunk;
        CPtrList* pOpenListForSearchTailNodeUpToTrunk;
        CPtrList* pClosListForSearchTailNodeUpToTrunk;
        
        CPtrList* pOpenListForSearchMidNormal;
        CPtrList* pClosListForSearchMidNormal;
        CPtrList* pOpenListForSearchMidReverse;
        CPtrList* pClosListForSearchMidReverse;
       
        enCalculateScene eCalculateSceneForSearchHead;
        enCalculateScene eCalculateSceneForSearchMid;

        //////////////////////////////////////////////////////////////////////////

        BOOL bSearchStartNodeSign;  //运行时状态,表示当前是否是计算第一个点

        //失败原因
        
        enum enFailCause
        {
            FAIL_UNDIFINED,
            FAIL_BY_HIGN_NETWORK_HEAD_REPEAT,
            FAIL_BY_HIGN_NETWORK_TAIL_REPEAT,
        };

        enFailCause eFailedCause;//路网上运算失败的话要查看此错误码

        //-------------------------------------------------------------

        stuTacticOfSearchPathHead()
        {
            InitialOperation();
        }

        BOOL InitialOperation()
        {
            stParaForSearchUpNode.bProjectOnLine = TRUE;
            stParaForSearchUpNode.nCountOfLinkedHignRoadSection= 2;
            stParaForSearchUpNode.nCountOfAppropriateNodeUpperLimit = 1;  
            stParaForSearchUpNode.nCountOfAppropriateNodeLowerLimit = 0;  
            stParaForSearchUpNode.pHeadForbidNodeList = NULL;
            stParaForSearchUpNode.pTailForbidNodeList = NULL;
            
            EnableLimitInfo = TRUE;
            EnableDirInfo = TRUE;
                        
            nRLevel[0] = 0;
            nRLevel[1] = 0;
            nRLevel[2] = 0;
            nLoadNetworkFlag[0] = 0;
            nLoadNetworkFlag[1] = 0;
            nLoadNetworkFlag[2] = 0;

            pOpenListForHeadSearchExitOfResidentialArea= NULL;
            pClosListForHeadSearchExitOfResidentialArea= NULL;
            pOpenListForTailSearchExitOfResidentialArea= NULL;
            pClosListForTailSearchExitOfResidentialArea= NULL;
            
            pOpenListForSearchHeadNodeUpToTrunk= NULL;
            pClosListForSearchHeadNodeUpToTrunk= NULL;
            pOpenListForSearchTailNodeUpToTrunk= NULL;
            pClosListForSearchTailNodeUpToTrunk= NULL;
            
            pOpenListForSearchMidNormal= NULL;
            pClosListForSearchMidNormal= NULL;
            pOpenListForSearchMidReverse= NULL;
            pClosListForSearchMidReverse= NULL;

            eCalculateSceneForSearchHead = SCENE_NONE;
            eCalculateSceneForSearchMid = SCENE_NONE;
            return TRUE;
        } 
    };


    stuTacticOfSearchPathHead m_stTacticOfSearchPathHead;

     /**
     * @brief 两边交汇搜索路径
     *
     * @param 
     * @return
    */
	BOOL CalculateOptimalPathMid(
        CPtrList *pSOList, 
        CPtrList *pSCList, 
        CPtrList *pEOList,
        CPtrList *pECList, 
        CGIS_NetworkPathInfo *pPathInfo,
        BYTE nRLevel, 
        CGIS_NetworkNode **pSNode,
        CGIS_NetworkNode **pENode, 
        stuTacticOfSearchPathHead* pTactic,     //策略
        BOOL bSFlag = FALSE);

//////////////////////////////////////////////////////////////////////////
//070601
    //会抛出异常.
    BOOL SearchExitOfResidentialArea(
         CPtrList *              pOpenList,      //应该已经有一个等扩展的节点
        CPtrList *              pCloseList,
        CGIS_NetworkVtx *       pDestVtx,       //目标节点
        CGIS_NetworkPathInfo *  pPathInfo,  
        BYTE                    nRLevel,        //低于此等级的路段不扩展
        BOOL                    bHead,          //正方向找还是反方向找
        BOOL                    &bFind,         //[out]是否找到目标点
        stuTacticOfSearchPathHead* pTactic);     //策略

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//5个计算函数全部调整为返回值为BOOL型,以抛出CGIS_NetworkException异常的方式给出失败原因
  //目前这五个计算函数如正常返回则一定是返回VALUE_TRUE表示计算成功.且这五个函数在最高层也不用进行异常捕捉
 protected:
     enum enBoolValue
     {
         VALUE_FALSE = 0,
         VALUE_TRUE = 1,
     };
    enBoolValue CalculateNavigatePathShortLeg    ( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
    enBoolValue CalculateNavigatePathMidLeg      ( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
    enBoolValue CalculateNavigatePathLongLeg     ( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
    enBoolValue CalculateNavigatePathMidLongLeg  ( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
    enBoolValue CalculateNavigatePathBigLongLeg  ( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkPathInfo *pPathInfo, CGIS_GeoMap *pMap, CPtrList *pPathPointList );


   /**
     * @brief 找到合适的上高架的点
     *
     * @param 
     * @return TRUE-找到合适的上高架的点 FALSE-未找到合适的上高架的点
    */
	enBoolValue CalculateOptimalPathHead( 
        CPtrList *              pOpenList, 
        CPtrList *              pCloseList,
        CGIS_NetworkVtx *       pVtx,
        CGIS_NetworkPathInfo *  pPathInfo,
        BYTE                    nRLevel,   //找路的最低功能级别
        BOOL                    bHead,     //正方向找还是反方向找
        BOOL                    &bFind,   //是否找到目标点
        stuTacticOfSearchPathHead* pTactic);

	enBoolValue CalculateOptimalPathHeadForUpToTrunk( 
        CPtrList *              pOpenList, 
        CPtrList *              pCloseList,
        CGIS_NetworkVtx *       pVtx,
        CGIS_NetworkPathInfo *  pPathInfo,
        BYTE                    nRLevel,   //找路的最低功能级别
        BOOL                    bHead,     //正方向找还是反方向找
        BOOL                    &bFind,   //是否找到目标点
        stuTacticOfSearchPathHead* pTactic);

//////////////////////////////////////////////////////////////////////////
    BOOL LeaveResidentialArea(     
        CPtrList* pOpenList, 
        CPtrList* pCloseList,
        CGIS_GeoMap* pMap , 
        CGIS_NetworkPathInfo* pPathInfo ,
        CGIS_NetworkNode*& pStartNode,
        CGIS_NetworkVtx* pEndVtx,
        CGIS_WayPoint* pWPOne,
        BOOL bHead,
        BOOL& bFind);
    BOOL CalculateNavigatePath_SingleSourceSearch(
        CGIS_GeoMap* pMap , 
        CGIS_NetworkPathInfo* pPathInfo ,
        CGIS_NetworkNode*& pStartNode,
        CGIS_NetworkNode*& pEndNode,                                                        
        CGIS_WayPoint* pWPOne,
        CGIS_WayPoint* pWPTwo,
        BOOL& bFind,
        stuTacticOfSearchPathHead* pTactic);
    BOOL CalculateNavigatePath_DoubleSourceSearch(
        CGIS_GeoMap* pMap , 
        CGIS_NetworkPathInfo* pPathInfo ,
        CGIS_NetworkNode*& pStartNode,
        CGIS_NetworkNode*& pEndNode,                                                        
        CGIS_WayPoint* pWPOne,
        CGIS_WayPoint* pWPTwo,
        BOOL& bFind,
        stuTacticOfSearchPathHead* pTactic);
    BOOL CalculateNavigatePath_DoubleSourceSearchInTrunkNetwork_DEL(
        CGIS_GeoMap* pMap , 
        CGIS_NetworkPathInfo* pPathInfo ,
        CGIS_NetworkNode*& pStartNode,
        CGIS_NetworkNode*& pEndNode,                                                        
        CGIS_WayPoint* pWPOne,
        CGIS_WayPoint* pWPTwo,
        BOOL& bFind,
        stuTacticOfSearchPathHead* pTactic,
        CPtrList* pPathMid);
    //此版本如果在全国路网上计算失败则回到详图去算
    BOOL CalculateNavigatePath_DoubleSourceSearchInTrunkNetworkV2(
        CGIS_GeoMap* pMap , 
        CGIS_NetworkPathInfo* pPathInfo ,
        CGIS_NetworkNode*& pStartNode,
        CGIS_NetworkNode*& pEndNode,                                                        
        CGIS_WayPoint* pWPOne,
        CGIS_WayPoint* pWPTwo,
        BOOL& bFind,
        stuTacticOfSearchPathHead* pTactic,
        CPtrList* pPathMid,
        BOOL& bFindInTrunk);

};

class CLocalNodeListMng
{
public:
    CLocalNodeListMng(CPtrList* pList);
    ~CLocalNodeListMng();
protected:
    CPtrList* m_pList;
};


#endif
