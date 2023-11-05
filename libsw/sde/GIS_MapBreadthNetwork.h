
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
	///���ص�ͼ����
	virtual BOOL LoadMapNetwork( );
	///ж�ص�ͼ����
	virtual BOOL UnLoadMapNetwork( CGIS_BreadthMap *pBMap = NULL );
	///���㵼��·��
	virtual int CalculateNavigatePath( CPtrList* WayPointList,CPtrList* pPathpointlist );

	//��network�Ľڵ������в��ҽڵ�
    CGIS_NetworkVtx* GetNetworkVtx( int nVBID, int nID )
    {       
        if( nID >= m_nVtxNum[nVBID] )
            return NULL;
        return m_pVtxList[nVBID][nID];             
    }
	//��network��·�������в���·��
    CGIS_NetworkRut* GetNetworkRut( int nRBID, int nID )
    {
        if( nID >= m_nRutNum[nRBID] )
            return NULL;
        return m_pRutList[nRBID][nID];
    }

	//������������
	void ClearNetwork( );
	//����ָ����ͼ���������
	void ClearBNetwork( CGIS_BreadthMap *pBMap );
	//������������
	void ResetNetwork( );


    //////////////////////////////////////////////////////////////////////////
    //ͼ�����غ���
    /**
     * @brief �Է���ֵ�ʹ������ʾ���ؽ��,���쳣�׳�
    */
	BOOL ReadBreadthNetwork( CGIS_BreadthMap **pBNet, BYTE byLFlag ,int& nErrorCode );
 
    /**
     * @brief ����ͼ�����Ҷ����������ID����.
    */
    BOOL ReadBreadthNetworkV2( 
    CGIS_BreadthMap **pBNet, BYTE byLFlag,
    int& nLayer0RoadSectionMax, int& nLayer1RoadSectionMax,int& nLayer2RoadSectionMax,
    int& nLayer0VertexMax, int& nLayer1VertexMax,int& nLayer2VertexMax);

    /**
     * @brief ����·�ε�ID����·������
    */
    BOOL ReadBreadthNetworkRoadSectionByIDArray(CGIS_BreadthMap **pBreadthMap, BYTE byRoadSectionNum,unsigned short* pArrayOfRoadSectionID,int& nErrorCode);
    //////////////////////////////////////////////////////////////////////////

    //����-1��ʾ����ʧ��.
	int GetWPProjRut( CGIS_WayPoint *pWayPoint, IGIS_Map *pMap, CGIS_FeatureRut **pRList, CGIS_BreadthMap **pBMaps, ST_GEO_PTXY *pPPtList, int nRNum, ST_GEO_PTXY ptEnd ,int& nErrorCode );
	///�õ��¸��ڵ�
	CGIS_NetworkVtx* GetNextRutVtx( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, BOOL bHead = TRUE );
	BOOL CouldReachAnotherVertexInRoadSection( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut);
protected:
	int CalculateNavigatePathActual( CPtrList* WayPointList,CPtrList* pPathpointlist );

    
	BOOL FullWayPointData( CGIS_WayPoint *pWayPoint, CGIS_GeoMap *pMap, ST_GEO_PTXY ptEnd,int& nErrorCode );
	int FullWayPointData2( CGIS_WayPoint *pWayPoint, CGIS_GeoMap *pMap, CGIS_NetworkPathInfo *pPPInfo, ST_GEO_PTXY ptEnd,    int& nErrorCode );
	///���Ҫ����·����;����Ľڵ���Ϣ
	BOOL GetRealVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CGIS_NetworkVtx **pStartVtx, CGIS_NetworkVtx **pEndVtx );
	
	///��������·��(*****)
	///�������ӿ������ܲ���̫�����.
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


	///�õ��¸��ڵ�
// 	CGIS_NetworkVtx* GetNextRutVtx( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, BOOL bHead = TRUE );
	CGIS_NetworkVtx* GetNextRutVtx2( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, BOOL bHead = TRUE );
	///�����ʼwaypoint·����Ϣ
	BOOL FullWPStartPath( CGIS_WayPoint *pWPOne, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, CGIS_GeoMap *pMap, BOOL &bRutSame, CPtrList *pPathPointList, POSITION &pos );
	BOOL FullWPStartPathV2( CGIS_WayPoint *pWPOne, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, CGIS_GeoMap *pMap, BOOL &bRutSame, CPtrList *pPathPointList, POSITION &pos );
	///�����ֹwaypoint·����Ϣ
	BOOL FullWPEndPath_DELETE( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, int nRutID, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///�����ֹwaypoint·����Ϣ
	BOOL FullWPEndPathV2( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	BOOL FullWPEndPathV3( CGIS_WayPoint *pWPTwo, CGIS_NetworkVtx *pVtx, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///���·��pathleg����
	BOOL FullPathLegData( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CPtrList *pRutList, CPtrList *pVtxList, CGIS_GeoMap *pMap, CPtrList *pPathPointList );
	///���·�ߵ���waypoint��ͬһ·�ε����
	BOOL FullWPOneAndWPTwoOnSameRut( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	///���·�ߵ���waypoint��ͬһ�ڵ�����
	BOOL FullWPOneAndWPTwoOnSameVtx( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos );
	//��䳤�������ʱ��ʼ�˺���ֹ�˵ĺ�����Ϣ��
	int FullPathHeadTailData( CGIS_GeoMap *pMap, CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_NetworkNode *pNStart, CGIS_NetworkNode *pNEnd,     CPtrList* pPathMid, CPtrList* pPathPointList);
	///��õ����ڷַ���ͼ
	BOOL GetBreadthMapOfPoint( float dx, float dy, CPtrList &BMapList );
	//��ö�ӦID�ķַ�·��
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


	char m_byErrCode;		//�˱���ֻ��·�����ݼ����б���ֵ,��CGIS_NetworkException�Ĵ�������ͬ
                            

	BYTE m_LoadNetworkFlag;		//1��ȫ�����أ�2������1��2��·����3������һ��·��[�����ͳ�Ա����]
	BYTE m_byRLevel;	//��·�ȼ�
	
	BYTE m_byCFlag;                         //1-���� 2-����
	BYTE m_byClear;

	int m_nMaxSLBNum;
	int m_nMaxELBNum;
	int m_pSLBID[NAV_MAX_LBNNUM];	//���������ʱ�Ѽ���ͼ��ID
	int m_nSLBNum;					//�Ѽ���ͼ������
	int m_pELBID[NAV_MAX_LBNNUM];	//���������ʱ�Ѽ���ͼ��ID
	int m_nELBNum;					//�Ѽ���ͼ������
    
	int m_nBID;						//��ӻ�������·��ʱ��¼ͼ��id

//////////////////////////////////////////////////////////////////////////
    BOOL ExitJudge();
//////////////////////////////////////////////////////////////////////////
//lijun add this code for debug
#ifdef _DEBUG
//----------------------
public:
	CGeoView* m_pMapView;

	//����ģ��״̬�Ĺ�ϵ,Ӧ�ò����Ե��ô˽ӿ�,ֻ����sde�ڲ�ʹ��
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
//070429 ��·�����ݰ���ѹ��Ҫ�Ŀռ�תΪ��·���ռ����ڲ�����.�Լ��ٶԾ�̬�ռ���ڴ�ռ������.
protected:
    char m_pBufOfBeforeCompress[500000];
    char m_pBufOfAfterCompress[300000];
//////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //070512 add this code for �յ�������·����
    CGIS_NetworkRut* m_pDestRut;
    //////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
    //07-05-31��Ӵ˴���
public:
    struct stuTacticOfSearchPathHead
    {

        int nRoadSectionLevel;            //·�ε����LEVEL
        int nMinRoadSectionLevel;            //̽·����͹��ܼ���,�˼������µ�·��������

        CPtrList*               pLimitRoadSectionList;  //���е�·���б�(����һ�׶ε�����ʧ�ܵ�����һ�εĽ����Ϊ����)
        CPtrList*               pLimitNodeList;         //���õ�NODE�б�(����һ�׶ε�����ʧ�ܵ�����һ�εĽ����Ϊ������)

        //ResidentialAreaNetwork//������·��
        //LowNetwork//�͵ȼ�·��
        //HighNetwork;//�ߵȼ�·��
        //////////////////////////////////////////////////////////////////////////
        //������·������̽���õĲ���
        BOOL EnableLimitInfo;
        BOOL EnableDirInfo;
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        //�͵ȼ�·������̽���õĲ���
        //
        struct stuParaForSearchUpNode
        {
        CPtrList* pHeadForbidNodeList;//�������̽���Ľ����(����ݸߵȼ�·�����������޸�)
        CPtrList* pTailForbidNodeList;//�յ�����̽���Ľ����(����ݸߵȼ�·�����������޸�)
            int nCountOfLinkedHignRoadSection;      //�ϸߵȼ�·���ĵ����ӵĿ�ͨ��·����Ҫ���ڴ�ֵ
            int nCountOfAppropriateNodeUpperLimit;  //�����Ҽ�������������node
            int nCountOfAppropriateNodeLowerLimit;  //�����Ҽ�������������node
            BOOL bProjectOnLine;                    //�ϸߵȼ�·���ĵ��Ƿ�Ҫ��ͶӰ������㵽Ŀ��㹹�ɵ��߶���,���⻷�ĳ���
        };
        stuParaForSearchUpNode stParaForSearchUpNode;
        //
        //////////////////////////////////////////////////////////////////////////



        //////////////////////////////////////////////////////////////////////////
        //˫�������м佻��ʱ�Ĳ���//0-������ʼ�ϸ��ٵ�node 1-������ֹ�ϸ��ٵ�NODE,2-˫��ʱ��NODE
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

        BOOL bSearchStartNodeSign;  //����ʱ״̬,��ʾ��ǰ�Ƿ��Ǽ����һ����

        //ʧ��ԭ��
        
        enum enFailCause
        {
            FAIL_UNDIFINED,
            FAIL_BY_HIGN_NETWORK_HEAD_REPEAT,
            FAIL_BY_HIGN_NETWORK_TAIL_REPEAT,
        };

        enFailCause eFailedCause;//·��������ʧ�ܵĻ�Ҫ�鿴�˴�����

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
     * @brief ���߽�������·��
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
        stuTacticOfSearchPathHead* pTactic,     //����
        BOOL bSFlag = FALSE);

//////////////////////////////////////////////////////////////////////////
//070601
    //���׳��쳣.
    BOOL SearchExitOfResidentialArea(
         CPtrList *              pOpenList,      //Ӧ���Ѿ���һ������չ�Ľڵ�
        CPtrList *              pCloseList,
        CGIS_NetworkVtx *       pDestVtx,       //Ŀ��ڵ�
        CGIS_NetworkPathInfo *  pPathInfo,  
        BYTE                    nRLevel,        //���ڴ˵ȼ���·�β���չ
        BOOL                    bHead,          //�������һ��Ƿ�������
        BOOL                    &bFind,         //[out]�Ƿ��ҵ�Ŀ���
        stuTacticOfSearchPathHead* pTactic);     //����

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//5�����㺯��ȫ������Ϊ����ֵΪBOOL��,���׳�CGIS_NetworkException�쳣�ķ�ʽ����ʧ��ԭ��
  //Ŀǰ��������㺯��������������һ���Ƿ���VALUE_TRUE��ʾ����ɹ�.���������������߲�Ҳ���ý����쳣��׽
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
     * @brief �ҵ����ʵ��ϸ߼ܵĵ�
     *
     * @param 
     * @return TRUE-�ҵ����ʵ��ϸ߼ܵĵ� FALSE-δ�ҵ����ʵ��ϸ߼ܵĵ�
    */
	enBoolValue CalculateOptimalPathHead( 
        CPtrList *              pOpenList, 
        CPtrList *              pCloseList,
        CGIS_NetworkVtx *       pVtx,
        CGIS_NetworkPathInfo *  pPathInfo,
        BYTE                    nRLevel,   //��·����͹��ܼ���
        BOOL                    bHead,     //�������һ��Ƿ�������
        BOOL                    &bFind,   //�Ƿ��ҵ�Ŀ���
        stuTacticOfSearchPathHead* pTactic);

	enBoolValue CalculateOptimalPathHeadForUpToTrunk( 
        CPtrList *              pOpenList, 
        CPtrList *              pCloseList,
        CGIS_NetworkVtx *       pVtx,
        CGIS_NetworkPathInfo *  pPathInfo,
        BYTE                    nRLevel,   //��·����͹��ܼ���
        BOOL                    bHead,     //�������һ��Ƿ�������
        BOOL                    &bFind,   //�Ƿ��ҵ�Ŀ���
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
    //�˰汾�����ȫ��·���ϼ���ʧ����ص���ͼȥ��
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
