
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
	std::list<IGIS_MapView*> _mapViewSet; //��ͼ����
	//CPtrList*  m_pMapViewSet;
	//CPtrList* m_LayerManagerSet;	//��ͼ�㼯
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
	//�õ�ͼ�������·���ϸýڵ�������Լ���һ������
	BOOL GetRutData( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut* pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos);
    //
	BOOL GetRutDataV3( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut* pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos,CPtrList* pNodeList,POSITION posOfNodeList);
	
    //����Trunk��Detail����ӵ�NODE�����,ֻ���ռ�����,������NODE
    BOOL GetRutDataV2( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut* pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos);
	BOOL FullEVNode( CGIS_WayPoint *pWTwo, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, CPtrList *pPPList, POSITION &pos );

	BOOL Open(swString szFilePath);
	BOOL Load(swString szFilePath);
	BOOL UnLoad(UINT nMapID);
	BOOL LoadBreadthMap( CGIS_BreadthMap *pBMap );
	BOOL UnLoadBreadthMap( CGIS_BreadthMap *pBMap );
    /* @param pErrorCode[out] 0-���쳣 1-δ�����쳣 2-�������ڴ�����
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
	////����;�����·��
	BOOL CalWayPointRut( CGIS_WayPoint *pWayPoint, ST_GEO_PTXY ptEnd, int& nErrorCode,UINT nDisBuf = 100, UINT nCount = 0 );
	BOOL ClearStorageBMap( );

	void GetBVRuts( CPtrList *pVList, POSITION &pos, int nRID = -1 );
	CGIS_BreadthMap* GetBreadthMapByID( int nBID );
	CGIS_BreadthMap* GetBreadthMapByID2( int nBID );
	
	BOOL FullVRutInfo   ( CGIS_NetworkRut*  pRut, ST_VOICE_RUT*   pstVoiceRut );
	BOOL FullVNodeInfo  ( CGIS_FeatureRut*  pPreFRut, CGIS_PathpointNode*   pPNode,  int nREBID, int nEndRutID, ST_VOICE_NODE*  pstVoiceNode );
	BOOL InitVNodeInfo_DEL  ( ST_VOICE_NODE*    pstVoiceNode );
	//�жϽڵ㴦�����··��/��ڵ�·�ͳ��ڵ�·�Ƿ�Ϊͬһ����·
		//�Ƿ��������ڵ�·ͬ���ĵ�·/�Ƿ��������ڵ�·ͬ���ĵ�·
	void JudgeNodeRutNameInfo( CGIS_FeatureRut *pPreFRut, CGIS_PathpointNode *pPNode, ST_VOICE_NODE *pVNode );
	//�������һ��ƥ���ڵ�·�ϵ�λ��
	ST_GEO_PTXY GetLastPtOnRoad();
	//���س�����ʻ����
	int GetCarDirect();
	//���س���������ʻ��·�ϵ��¸��ڵ�
	short GetNextRutPt();
	//���ͼ������״̬
	void ResetMapState();

private:
	///���·���ϸýڵ��·����Ϣ��·����Ϣ
	BOOL FullPathNode( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_BreadthMap *pBMap, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos );
	BOOL FullPathNodeV2( CGIS_MapBreadthNetwork *pBMNetwork, CGIS_FeatureRut *pFRList[MAX_BRUT_NUM], CGIS_NetworkVtx *pVtx, CGIS_BreadthMap *pBMap, CGIS_NetworkRut *pRPre, CGIS_NetworkRut *pRNext, int nREBID, int nRutEndID, CPtrList *pPathpointList, POSITION &pos );
	///��ȡ��ID������·��
	inline BOOL FullPathNodeArc( CGIS_Pathpoint *pPPoint, CGIS_FeatureRut *pFRut, ST_GEO_PTXY ptEnd, CGIS_BreadthMap *pBMap, BOOL bSequence, CPtrList *pPathpointList, POSITION &pos );

	///ȡ����ͼ��������·����Ϣ
	BOOL GetRutObjList( CPtrList *pLMList, CGIS_BreadthMap *pBMap, CPtrList *pObjList );
	///ȡ����ͼ���д�·����Ϣ
	BOOL GetBreadthMapCurRutObj( ST_GEO_PTXY ptStart, CGIS_BreadthMap *pBMap, int nRutID, CGIS_FeatureRut **pFRut );
	///������ͼ������ͼ���д�·�εĵ���
	BOOL GetRoundBMapRutObj( ST_GEO_PTXY pt, int nRutID, CGIS_BreadthMap **pBMap, CGIS_FeatureRut **pFRut );
	///��õ����ڷַ���ͼ
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
	//�Գ�������λ�õ�ͼ����·���л���
	void CalMatchBMap( ST_GEO_PTXY pt );
//	CGIS_MapInfo* m_MapInfo;
//	CPtrList	*m_pBMapList;	//�ַ���ͼ����
//	CGIS_BreadthMap *m_pBMapList[190];
	ST_GEO_RECT *m_pRTList;	
	CGIS_BreadthMap *m_pBMapList;
	int m_nBMapNum;
	//��·���б�
	//CGIS_MapLayer *m_pRLList[MAX_RLAY_NUM];
	//BYTE m_nRLNum;
	std::list<CGIS_MapLayer*> _rl_list; //��·�㼯��

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
	//CGIS_MapLayer *m_pLList[MAX_RLAY_NUM];  //��·��7
	std::vector<CGIS_MapLayer*> m_pLList; 
	char * m_pLaneBlockAddr;
	CGIS_FeatureRut *m_pFRList[MAX_BRUT_NUM];
	short m_nFRLNum;

//	CGIS_BufStorageNetwork m_MNBStor;
	BYTE	m_byMRNum;		//·�μ�·�εĸ���
	CGIS_PathpointNode *m_pVNode;	//������Node�ڵ�
	POSITION m_posVNode;			//node���õ�λ��
	CGIS_FeatureRut	 *m_pFRPre;	//ǰһ��·��
	int				 m_nAPre;	//ǰһ��·�νǶ�
	int				 m_nANext;	//
	BYTE			 m_bTrunFlag;	//0-˳�� 1-��˳��
	BOOL			 m_bRFlag;	//�Ƿ�Ϊ�����ı�־
	short			 m_nRMOrd;
	int				 m_nBID;	//����·���������ڵ�ͼ��ID����ź�
	int				 m_nMatchBID; //Ϊ��·ƥ�仺���ĳͼ����ID
	ST_GEO_PTXY      m_ptLastXy;	//���һ��ƥ���ķ���ֵ
	int				 m_nReCount;	//�����ڵ�����������������ʻ�ļ�����ֵ
	int				 m_nCarRunDirect;//������ʻ����1��·��ͷ����·��β 2��·��β����·��ͷ0��δȷ��
	short			 m_nNextRutPt;//������ʻ���ڵ�·����һ���յ�
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
