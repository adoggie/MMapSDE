// GIS_MapBreadthNetwork.cpp: implementation of the CGIS_MapBreadthNetwork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./DebugFunctionLib/Watch.h"
//////////////////////////////////////////////////////////////////////////
#include "DebugFunctionLib/DebugFun.h"
/**
 * @brief   ���Ժ�����(ֻ�ڵ��԰汾�б�����)
 *
 * ��־�ļ�����
 * �����������Ļ����
 * �������ø��ٹ���
 *
 *    
 * @note    ��������ڵ��԰汾��ʹ����Щ���е�һ�����,Ӧ�ý���Щ����local��
 *          �������¶���Ϊ��,����������Ӧ��ע�ͼ���.       
*/

#define DBG2SCR0(sz)              
#define DBG2SCR1(sz, p1)          
#define DBG2SCR2(sz, p1, p2)      
#define DBG2SCR3(sz, p1, p2, p3) 
//#define TRACK_FUNCTION(sz) 
//#define DBG2LOG0(sz)                
//#define DBG2LOG1(sz, p1)            
//#define DBG2LOG2(sz, p1, p2)        
//#define DBG2LOG3(sz, p1, p2, p3)    
//////////////////////////////////////////////////////////////////////////

#include "GIS_NetworkStatus.h"
#include "GIS_NetworkException.h"
#include "GIS_MapBreadthNetwork.h"
#include "GIS_PathpointMark.h"
#include "SDEControler.h"
#include "GIS_MapDataManager.h"
#include "./Zlib/zlib.h"
#include "./MemoryMngLib/MemoryMngV3A.h"
#include "./MemoryMngLib/MemoryMng2V1.h"
#include "./IMemoryMngSet.h"

#include "MemoryDispatchForSpaceDataV2.h"
#include "GIS_MBBNetwork.h"
#include "GIS_PathpointNodeV2.h"



//////////////////////////////////////////////////////////////////////////
#include <vector>
#include <algorithm>
//////////////////////////////////////////////////////////////////////
#include <memory>
using namespace std;
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG

#define  DUMPRUTLIST(p) {\
            CPtrList* pFtrList = (p);\
            if(pFtrList)\
            {\
                TRACE0("********************************************************************\n");\
                TRACE1("RutList [Count=%d]\n",pFtrList->GetCount());\
                POSITION pos = pFtrList->GetHeadPosition();\
                CGIS_NetworkRut* pFtr = NULL;\
                while(pos)\
                {\
                    pFtr = (CGIS_NetworkRut*)pFtrList->GetNext(pos);\
                    ASSERT(pFtr);\
                    TRACE2("\tRut%.4d(%.5d)\n",pFtr->GetRutID(),pFtr->GetRBreadthID());\
                }\
                TRACE0("********************************************************************\n");\
            }\
            Sleep(100);\
        }

#define  DUMPVTXLIST(p) {\
            CPtrList* pFtrList = (p);\
            if(pFtrList)\
            {\
                TRACE0("********************************************************************\n");\
                TRACE1("VtxList [Count=%d]\n",pFtrList->GetCount());\
                POSITION pos = pFtrList->GetHeadPosition();\
                CGIS_NetworkVtx* pFtr = NULL;\
                while(pos)\
                {\
                    pFtr = (CGIS_NetworkVtx*)pFtrList->GetNext(pos);\
                    ASSERT(pFtr);\
                    TRACE2("\tVtx%.4d(%.5d)\n",pFtr->GetVtxID(),pFtr->GetVBreadthID());\
                }\
                TRACE0("********************************************************************\n");\
            }\
            Sleep(100);\
        }

#define  DUMPNODELIST(p) {\
            CPtrList* pFtrList = (p);\
            if(pFtrList)\
            {\
                TRACE0("********************************************************************\n");\
                TRACE1("NodeList [Count=%d]\n",pFtrList->GetCount());\
                POSITION pos = pFtrList->GetHeadPosition();\
                CGIS_NetworkNode* pFtr = NULL;\
                while(pos)\
                {\
                    pFtr = (CGIS_NetworkNode*)pFtrList->GetNext(pos);\
                    ASSERT(pFtr);\
                    DUMPSTATE(pFtr);\
                }\
                TRACE0("********************************************************************\n");\
            }\
            Sleep(100);\
        }

#define TRACE_SCRIPT_MOVE_TO_NODE(p) {\
            TRACE0("\n#################################\n");\
            TRACE2("%.6f\n%.6f\n",p->m_pVtx->GetVtxPosition().dx,p->m_pVtx->GetVtxPosition().dy);\
            TRACE0("ͬ������:��ʾ���Ķ�λ\n#################################\n\n");\
        }

#define DUMPNODESTATE(sz,p) {\
            TRACE0(sz);\
            TRACE0("----\n");\
            DUMPSTATE(p);\
        }
            
#else // _DEBUG

#define  DUMPRUTLIST(p) 
#define  DUMPVTXLIST(p)
#define  DUMPNODELIST(p) 
#define  TRACE_SCRIPT_MOVE_TO_NODE(p) 
#define  DUMPNODESTATE(p,sz)

#endif


//////////////////////////////////////////////////////////////////////////
static HANDLE gHNFile = INVALID_HANDLE_VALUE;


//////////////////////////////////////////////////////////////////////////
//�˳����.
//�ڱ�ģ���е��˳�Ӧ�������µط�.
//1.����ʱҪ����·������֮��.
//2.���ʱҪ���ؿռ�����֮��.
BOOL CGIS_MapBreadthNetwork::ExitJudge()
{
    if(CGIS_NetworkStatus::m_bCalculateStopSign)
    {
		TRACE0("�˳���־��λ,��������.\n");	
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_STOP_BY_OUTSIDE));
    }

    return CGIS_NetworkStatus::m_bCalculateStopSign;
}

//////////////////////////////////////////////////////////////////////////
//���Ժ�����


//////////////////////////////////////////////////////////////////////////
//�򻯴���ĺ�
#define CLEARNODELISTANDRESETVERTEXSTATE(p) ClearNWNList(p)
#define CLEARNODELISTANDRESETVERTEXSTATEV2(p) {\
            CPtrList* pList = (p);\
            ASSERT(pList);\
            POSITION pos = pList->GetHeadPosition();\
            CGIS_NetworkNode* pNode = NULL;\
            while(pos)\
            {\
                pNode = (CGIS_NetworkNode*)pList->GetNext( pos );\
                if( pNode )\
                {\
                    ASSERT(pNode->m_pVtx);\
                    pNode->m_pVtx->ResetSortInfo();\
                    delete pNode;\
                    pNode = NULL;\
                }\
            }\
            pList->RemoveAll();\
        }


#define SET_NODERUT_USED_STATUS_THROUGH_TO_ANCESTOR(p,v) {\
            CGIS_NetworkNode *pNode = p;\
            while( pNode )\
            {\
                pNode->m_pRut->SetUsedStatus(v);\
                pNode = pNode->m_pParent;\
            }\
        }
/*
//#define GET_LIMIT_INFO(pNode,bHead,pLRList,byLRNum) {\
			CGIS_NetworkNode* pNTem = pNode;\
 			while( pNTem )\
			{\
				if( pNTem->m_pRut )\
				{\
					if( pNTem->m_pRut->GetRutType() != 4 )\
						break;\
				}\
				pNTem = pNTem->m_pParent;\
			}\
			if( pNTem )\
			{\
				if( bHead )\
				{\
					byLRNum = pNTem->m_pRut->GetLimitRNum();\
                    memcpy(pLRList,pNTem->m_pRut->GetLimitRList(),sizeof(pLRList[0]*byLRNum));\
				}\
				else\
				{\
					byLRNum = pNTem->m_pRut->GetRLimitRNum();\
                    memcpy(pLRList,pNTem->m_pRut->GetRLimitRList(),sizeof(pLRList[0]*byLRNum));\
				}\
			}\
        }

//#define GET_LIMIT_INFO_V2(pNode,bHead,pLimitInfo) {\
//			CGIS_NetworkNode* pNTem = pNode;\
// 			while( pNTem )\
//			{\
//				if( pNTem->m_pRut )\
//				{\
//					if( pNTem->m_pRut->GetRutType() != 4 )\
//						break;\
//				}\
//				pNTem = pNTem->m_pParent;\
//			}\
//			if( pNTem )\
//			{\
//                pLimitInfo = pNTem->m_pRut->GetLimitInfo();\
//			}\
//         }
*/
#define GET_LIMIT_INFO_V3(pNode,bHead,pGroup) {\
			CGIS_NetworkNode* pNTem = pNode;\
 			while( pNTem )\
			{\
				if( pNTem->m_pRut )\
				{\
					if( pNTem->m_pRut->GetRutType() != 4 )\
						break;\
				}\
				pNTem = pNTem->m_pParent;\
			}\
			if( pNTem )\
			{\
                CGIS_NetworkRut::stuRoadSectionLimitInfo* pLimitInfo = pNTem->m_pRut->GetLimitInfo();\
                if(pLimitInfo)\
                {\
                    pGroup = pLimitInfo->GetGroup(bHead);\
                }\
			}\
        }

#define GET_NORMAL_ROADSECTION_ID(pNode,nID) {\
			CGIS_NetworkNode* pNTem = pNode;\
			while( pNTem )\
			{\
				if( pNTem->m_pRut )\
				{\
					if( pNTem->m_pRut->GetRutType() != 4 )\
						break;\
				}\
				pNTem = pNTem->m_pParent;\
			}\
			if( pNTem )\
			{\
				nID = pNTem->m_pRut->GetRutID();\
			}\
        }
#define SINGLE_SRC_SEARCH_BACKDATE_PATH(r,v,n) \
    {\
		CPtrList* pRutList = r;\
		CPtrList* pVtxList = v;\
        CGIS_NetworkNode* pNode = n;\
		while( pNode )\
		{\
			pRutList->AddHead( pNode->m_pRut );\
			pVtxList->AddHead( pNode->m_pVtx );\
			pNode = pNode->m_pParent;\
		}\
        pRutList->RemoveHead();\
    }

#define DOUBLE_SRC_SEARCH_BACKDATE_PATH(r,v,s,e) \
    {\
		CPtrList* pRutList = r;\
		CPtrList* pVtxList = v;\
        ASSERT( pStartNode && pEndNode );\
        CGIS_NetworkNode* pNode = NULL;\
        pNode = s;\
        while( pNode )\
        {\
            pRutList->AddHead( pNode->m_pRut );\
            pVtxList->AddHead( pNode->m_pVtx );\
            pNode = pNode->m_pParent;\
        }\
        if( pVtxList->GetCount() > 0 )\
            pVtxList->RemoveTail();\
        pNode = e;\
        while( pNode )\
        {\
            if( pNode->m_pVtx->GetVtxEdge() == 1 )\
            {\
                CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork2( pNode->m_pVtx->GetVAbutBID() );\
                if( pBMap )\
                {\
                    CGIS_NetworkRut *pNRut = GetNetworkRut( pBMap->m_nBreadthID, pNode->m_pVtx->GetVAbutRID() );\
                    if( pNRut )\
                    {\
                        RutList.AddTail( pNode->m_pRut );\
                        CGIS_NetworkVtx *pNSVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNRut->GetRutStartVtxID() );\
                        CGIS_NetworkVtx *pNEVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNRut->GetRutEndVtxID() );\
                        ASSERT( pNSVtx && pNEVtx );\
                        if( pNSVtx==NULL || pNEVtx==NULL )\
                        {\
                            bFind = FALSE;\
                            break;\
                        }\
                        if( pNSVtx->GetVtxEdge() == 1 || pNEVtx->GetVtxEdge() == 1 )\
                        {\
                            if( pNSVtx->GetVtxEdge() == 1 &&\
                                (fabs(pNSVtx->GetVtxPosition().dx-pNode->m_pVtx->GetVtxPosition().dx)<FLOAT_ZERO &&\
                                fabs(pNSVtx->GetVtxPosition().dy-pNode->m_pVtx->GetVtxPosition().dy)<FLOAT_ZERO ) )\
                                VtxList.AddTail( pNSVtx );\
                            else if( pNEVtx->GetVtxEdge() == 1 &&\
                                (fabs(pNEVtx->GetVtxPosition().dx-pNode->m_pVtx->GetVtxPosition().dx)<FLOAT_ZERO &&\
                                fabs(pNEVtx->GetVtxPosition().dy-pNode->m_pVtx->GetVtxPosition().dy)<FLOAT_ZERO ) )\
                                VtxList.AddTail( pNEVtx );\
                            else \
                                ASSERT( FALSE );\
                        }\
                        else\
                            ASSERT( FALSE );\
                        pNode = pNode->m_pParent;\
                        continue;\
                    }\
                }\
            }\
            pRutList->AddTail( pNode->m_pRut );\
            pVtxList->AddTail( pNode->m_pVtx );\
            pNode = pNode->m_pParent;\
        }\
        RutList.RemoveHead( );\
        RutList.RemoveTail( );\
    }
   
#define DEFINE_VARIABLE_USED_BY_READ_ROADSECTION_FEATURE \
		unsigned short nRutID = 0;\
		BYTE	byDirection = 0;\
		BYTE	byType = 0;\
		BYTE	byCharge = 0;\
		BYTE	bySpeed = 0;\
		BYTE	nLaneNum = 0;\
		BYTE	nRutLevel = 0;\
		BYTE	byRutType = 0;\
		float	fRutLength = 0.0;\
		int		nVtxStartID = 0;\
		int		nVtxEndID = 0;\
		int     nBreadthID = (*pBNet)->m_nBreadthID;\
	    WORD pwSToRutID[20][10];\
        WORD pwEToRutID[20][10];\
        CGIS_NetworkRut::TURNINFO	pSTurnInfoArray[20];\
        CGIS_NetworkRut::TURNINFO	pETurnInfoArray[20];\
        {\
        for(int i = 0; i < 20; i++)\
        {\
            pSTurnInfoArray[i].pwToRutID = pwSToRutID[i]; \
            pETurnInfoArray[i].pwToRutID = pwEToRutID[i]; \
        }\
        }\
        CGIS_NetworkRut::stuRoadSectionLimitInfo aLimitInfo;\
        CGIS_NetworkRut::stuRoadSectionLimitInfo* pLimitInfo = &aLimitInfo;\
        memset(pLimitInfo,0,sizeof(CGIS_NetworkRut::stuRoadSectionLimitInfo));\
        pLimitInfo->sGroup.pTurnInfoArray = pSTurnInfoArray;\
        pLimitInfo->eGroup.pTurnInfoArray = pETurnInfoArray;\

  
#define READ_ROADSECTION_FEATURE \
    		Fread( pFBuf, dwSize, dwStation, &nRutID, sizeof(unsigned short), 1 );\
			if( nRutID > nReadNum )\
			{\
                TRACE0("RutID is wrong!\n");\
				m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;\
				ClearBNetwork( *pBNet );\
				return FALSE;\
			}\
			Fread( pFBuf, dwSize, dwStation, &byDirection, sizeof(BYTE), 1 );\
			Fread( pFBuf, dwSize, dwStation, &byType, sizeof(BYTE), 1 );\
			Fread( pFBuf, dwSize, dwStation, &byCharge, sizeof(BYTE), 1 );\
			Fread( pFBuf, dwSize, dwStation, &bySpeed, sizeof(BYTE), 1 );\
			Fread( pFBuf, dwSize, dwStation, &nRutLevel, sizeof(BYTE), 1 );\
			Fread( pFBuf, dwSize, dwStation, &byRutType, sizeof(BYTE), 1 );\
			Fread( pFBuf, dwSize, dwStation, &fRutLength, sizeof(float), 1 );\
			Fread( pFBuf, dwSize, dwStation, &nVtxStartID, sizeof(unsigned short), 1 );\
			Fread( pFBuf, dwSize, dwStation, &nVtxEndID, sizeof(unsigned short), 1 );\
			if( (nVtxStartID > nVtxNum) || (nVtxEndID > nVtxNum) )\
			{\
                TRACE0("rut's StartID/EndID is wrong!\n");\
                m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;\
				ClearBNetwork( *pBNet );\
				return FALSE;\
			}\
            Fread( pFBuf, dwSize, dwStation, &pLimitInfo->sGroup.byTurnCount, sizeof(pLimitInfo->sGroup.byTurnCount), 1 );\
            if ( pLimitInfo->sGroup.byTurnCount > 0)\
            {\
                ASSERT(pLimitInfo->sGroup.byTurnCount <= 20 );\
                if( pLimitInfo->sGroup.byTurnCount > 20 )\
                {\
                    TRACE0("rut's Limit num is wrong!\n");\
                    m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;\
                    ClearBNetwork( *pBNet );\
                    return FALSE;\
                }\
                for(int i = 0; i < pLimitInfo->sGroup.byTurnCount; i++)\
                {\
                    Fread( pFBuf, dwSize, dwStation, &pLimitInfo->sGroup.pTurnInfoArray[i].byToRouteIDCount,sizeof(pLimitInfo->sGroup.pTurnInfoArray[i].byToRouteIDCount),1);\
                    ASSERT(pLimitInfo->sGroup.pTurnInfoArray[i].byToRouteIDCount <= 10);\
                    if(pLimitInfo->sGroup.pTurnInfoArray[i].byToRouteIDCount > 10)\
                    {\
                        TRACE0("rut's Limit num is wrong!\n");\
                        m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;\
                        ClearBNetwork( *pBNet );\
                        return FALSE;\
                    }\
                    Fread( pFBuf, dwSize, dwStation, pLimitInfo->sGroup.pTurnInfoArray[i].pwToRutID,sizeof(pLimitInfo->sGroup.pTurnInfoArray[0].pwToRutID[0])*pLimitInfo->sGroup.pTurnInfoArray[i].byToRouteIDCount,1);\
                    Fread( pFBuf, dwSize, dwStation, &pLimitInfo->sGroup.pTurnInfoArray[i].byStartTime,sizeof(pLimitInfo->sGroup.pTurnInfoArray[i].byStartTime),1);\
                    Fread( pFBuf, dwSize, dwStation, &pLimitInfo->sGroup.pTurnInfoArray[i].byEndTime,sizeof(pLimitInfo->sGroup.pTurnInfoArray[i].byEndTime),1);\
                 }\
            }\
            Fread( pFBuf, dwSize, dwStation, &pLimitInfo->eGroup.byTurnCount, sizeof(pLimitInfo->eGroup.byTurnCount), 1 );\
            if ( pLimitInfo->eGroup.byTurnCount > 0)\
            {\
                ASSERT(pLimitInfo->eGroup.byTurnCount <= 20 );\
                if( pLimitInfo->eGroup.byTurnCount > 20 )\
                {\
                    TRACE0("rut's Limit num is wrong!\n");\
                    m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;\
                    ClearBNetwork( *pBNet );\
                    return FALSE;\
                }\
                for(int i = 0; i < pLimitInfo->eGroup.byTurnCount; i++)\
                {\
                    Fread( pFBuf, dwSize, dwStation, &pLimitInfo->eGroup.pTurnInfoArray[i].byToRouteIDCount,sizeof(pLimitInfo->eGroup.pTurnInfoArray[i].byToRouteIDCount),1);\
                    ASSERT(pLimitInfo->eGroup.pTurnInfoArray[i].byToRouteIDCount <= 10);\
                    if(pLimitInfo->eGroup.pTurnInfoArray[i].byToRouteIDCount > 10)\
                    {\
                        TRACE0("rut's Limit num is wrong!\n");\
                        m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;\
                        ClearBNetwork( *pBNet );\
                        return FALSE;\
                    }\
                    Fread( pFBuf, dwSize, dwStation, pLimitInfo->eGroup.pTurnInfoArray[i].pwToRutID,sizeof(pLimitInfo->eGroup.pTurnInfoArray[0].pwToRutID[0])*pLimitInfo->eGroup.pTurnInfoArray[i].byToRouteIDCount,1);\
                    Fread( pFBuf, dwSize, dwStation, &pLimitInfo->eGroup.pTurnInfoArray[i].byStartTime,sizeof(pLimitInfo->eGroup.pTurnInfoArray[i].byStartTime),1);\
                    Fread( pFBuf, dwSize, dwStation, &pLimitInfo->eGroup.pTurnInfoArray[i].byEndTime,sizeof(pLimitInfo->eGroup.pTurnInfoArray[i].byEndTime),1);\
                 }\
            }

#define DEFINE_VARIABLE_USED_BY_READ_VERTEX_FEATURE \
    unsigned short	nVtxID = 0;		/*�ڵ�ID*/ \
	ST_GEO_PTXY		ptPosition;		/*�ڵ�����*/ \
	BYTE			byEdge = 0;		/*�Ƿ���ͼ����Ե:0-����,1-��*/ \
	int				nABID = 0;		/*�ڽ�ͼ����*/ \
	unsigned short	nARID = 0;		/*����Rut_id*/ \
	BYTE			byRNum = 0;		/*�����ӵ�·����*/ \
	unsigned short	pVRList[11];	/*�����ӵ�·��ID*/ \
	unsigned short	pRTList[11];	/*��ˮƽ����ļн�����*/ \
	BYTE			pRLList[11];	/*·������ͼ��ID*/ \
	int		nVBID = 0;				/*�ڵ�����ͼ��ID*/ \
	unsigned short nPos = 0;

#define READ_VERTEX_FEATURE \
    Fread( pFBuf, dwSize, dwStation, &nVtxID, sizeof(unsigned short), 1 );\
	if( nVtxID > nReadNum )\
	{\
        TRACE0("vtx's nVtxID is wrong!(> vtx's num)\n");\
		m_byErrCode = 1;\
		ClearBNetwork( *pBNet );\
		return FALSE;\
	}\
	Fread( pFBuf, dwSize, dwStation, &ptPosition, sizeof(ST_GEO_PTXY), 1 );\
	Fread( pFBuf, dwSize, dwStation, &byEdge, sizeof(BYTE), 1 );\
	if( byEdge == 1 )\
	{\
		Fread( pFBuf, dwSize, dwStation, &nABID, sizeof(int), 1 );\
		Fread( pFBuf, dwSize, dwStation, &nARID, sizeof(unsigned short), 1 );\
	}\
	Fread( pFBuf, dwSize, dwStation, &byRNum, sizeof(BYTE), 1 );\
	if( byRNum > 10 || byRNum == 0 )\
	{\
        TRACE0("vtx's rutnum is wrong!(> 10 ||  == 0)\n");\
		m_byErrCode = 1;\
		ClearBNetwork( *pBNet );\
		return FALSE;\
	}\
	ASSERT( byRNum > 0 );\
	Fread( pFBuf, dwSize, dwStation, pVRList, sizeof(unsigned short)*byRNum, 1 );\
	Fread( pFBuf, dwSize, dwStation, pRTList, sizeof(unsigned short)*byRNum, 1 );\
	Fread( pFBuf, dwSize, dwStation, pRLList, sizeof(BYTE)*byRNum, 1 );

//////////////////////////////////////////////////////////////////////////
//locale function
static BOOL IfRouteHaveRepeatNode(
    CGIS_NetworkNode* pDetailStartNode,
    CGIS_NetworkNode* pTrunkStartNode,
    CGIS_NetworkNode* pLinkNode)
{
    //����ʼ�ڵ�ǰ��NODE��RUT���ñ��
    SET_NODERUT_USED_STATUS_THROUGH_TO_ANCESTOR(pDetailStartNode,TRUE);
                                                                             
    BOOL bRoadSectionReUse = FALSE;
    
    CGIS_NetworkNode* pNode = pTrunkStartNode;
    while( pNode )
    {
        if(pNode->m_pRut == pLinkNode->m_pRut && pNode->m_pVtx == pLinkNode->m_pVtx)
            break;
        if(pNode->m_pRut->GetUsedStatus())
        {
            TRACE0("�����ظ�·��.\n");
            DUMPSTATE(pNode);
            TRACE_SCRIPT_MOVE_TO_NODE(pNode);
            
            
            bRoadSectionReUse = TRUE;
            break;
        }
        
        pNode = pNode->m_pParent;
    }
    
    //����ʼ�ڵ�ǰ��NODE��RUT������
    SET_NODERUT_USED_STATUS_THROUGH_TO_ANCESTOR(pDetailStartNode,FALSE);

    return bRoadSectionReUse; 
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGIS_MapBreadthNetwork::CGIS_MapBreadthNetwork()
{
	m_LoadNetworkFlag = 1;
	m_nRBNum = 5;
	m_nMaxSLBNum = m_nMaxELBNum = NAV_MAX_LBNNUM;
	m_pRBuf[0].dx = m_pRBuf[4].dx = m_pRBuf[3].dx = 70.00f;
	m_pRBuf[0].dy = m_pRBuf[4].dy = m_pRBuf[1].dy = 10.00f;
	m_pRBuf[1].dx = m_pRBuf[2].dx = 137.20f;
	m_pRBuf[2].dy = m_pRBuf[3].dy = 55.50f;


    memset(m_pRutList,0,sizeof(m_pRutList));
    memset(m_pVtxList,0,sizeof(m_pVtxList));
    memset(m_nRutNum,0,sizeof(m_nRutNum));
    memset(m_nVtxNum,0,sizeof(m_nVtxNum));
    memset(m_pBufOfBeforeCompress,0,sizeof(m_pBufOfBeforeCompress));
    memset(m_pBufOfAfterCompress,0,sizeof(m_pBufOfAfterCompress));

    m_pTrunkNetwork = NULL;
    m_pDestRut = NULL;

}

CGIS_MapBreadthNetwork::~CGIS_MapBreadthNetwork()
{
    if(m_pTrunkNetwork)
    {
        m_pTrunkNetwork->~CGIS_MBBNetwork();
        m_pMemMngOfBuddyData->newfree(m_pTrunkNetwork);
        m_pTrunkNetwork = NULL;
    }
}

int CGIS_MapBreadthNetwork::CalculateNavigatePath( CPtrList* WayPointList,CPtrList* pPathpointlist )
{
    TRY
    {
        return CalculateNavigatePathActual(WayPointList,pPathpointlist);
    }
    CATCH(CGIS_NetworkException,e)
    {
        return Clear(e->GetErrorCode());
    }
    END_CATCH

    ASSERT(FALSE);
    return 1;
}


unsigned char FULL_ROUND_COUNT = 0;
BOOL CGIS_MapBreadthNetwork::FullPathNodeRuts(
    CPtrList *pRList, 
    CPtrList *pVList, 
    CGIS_NetworkVtx *pVtx, 
    CGIS_NetworkRut *pRoadSectionPrevious,
    CGIS_NetworkRut *pRNext,
    BYTE byRAType )
{
	FULL_ROUND_COUNT = 0;
	m_nBID = pVtx->GetVBreadthID();
	if( byRAType == 1 )
		return FullPathNodeRutsSVtx( pRList, pVList, pVtx, pRoadSectionPrevious, pRNext );
	else if( byRAType == 2 )
		return FullPathNodeRutsAss( pRList, pVList, pVtx, pRoadSectionPrevious, pRNext );
	else if( byRAType == 3 )
		return FullPathNodeRutsRound( pRList, pVList, pVtx, pRoadSectionPrevious, pRNext );
	return FALSE;
}
BOOL CGIS_MapBreadthNetwork::FullPathNodeRutsSVtx(
    CPtrList *pRList,
    CPtrList *pVList,
    CGIS_NetworkVtx *pVtx,
    CGIS_NetworkRut *pRoadSectionPrevious,
    CGIS_NetworkRut *pRNext )
{
	ASSERT( pRoadSectionPrevious && pRNext );
	CGIS_NetworkRut *pRut = NULL;
	for( int i=0;
    i<pVtx->GetVRutNum();	i++ )
	{
		if( pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID() || 
			pVtx->GetVRutList()[i] == pRNext->GetRutID() )
			continue;
		pRut = GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVtxID() );
		if( pRut )
		{
			pRList->AddTail( pRut );
//			m_pRutList[pVtx->GetVBreadthID()][pRut->GetRutID()] = NULL;
		}
	}
	return TRUE;
}
BOOL CGIS_MapBreadthNetwork::FullPathNodeRutsAss( CPtrList *pRList, CPtrList *pVList, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRoadSectionPrevious, CGIS_NetworkRut *pRNext )
{
	return GetNextAssRut( pRList, pVList, pVtx, pRoadSectionPrevious );
}
BOOL CGIS_MapBreadthNetwork::GetNextAssRut( CPtrList *pRList, CPtrList *pVList, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRoadSectionPrevious )
{
	ASSERT( pRoadSectionPrevious );
	CGIS_NetworkVtx *pVNext = NULL;
	CGIS_NetworkRut *pRNext = NULL;
	for( int i=0;	i<pVtx->GetVRutNum();	i++ )
	{
		if( pVtx->GetVRutList()[i] == pRoadSectionPrevious->GetRutID() )
			continue;
		pRNext = GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
		if( pRNext )
		{
			pRList->AddTail( pRNext );
			if( pRNext->GetRutType() == 4 )
			{
				if( pVtx->GetVtxID() == pRNext->GetRutStartVtxID() )
					pVNext = GetNetworkVtx( pVtx->GetVBreadthID(), pRNext->GetRutEndVtxID() );
				else
					pVNext = GetNetworkVtx( pVtx->GetVBreadthID(), pRNext->GetRutStartVtxID() );
				if( pVNext )
				{
					pVList->AddTail( pVNext );
					pVtx->GetVtxSInfo()->byStatusOfNormalSearch = 4;
					if( pVNext->GetVtxSInfo()->byStatusOfNormalSearch != 4 )
						GetNextAssRut( pRList, pVList, pVNext, pRNext );
				}
			}
		}
	}
	return TRUE;
}

BOOL CGIS_MapBreadthNetwork::FullPathNodeRutsRound( CPtrList *pRList, CPtrList *pVList, CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRoadSectionPrevious, CGIS_NetworkRut *pRNext )
{
	CGIS_NetworkRut *pNRStart = NULL;
	if( pRoadSectionPrevious->GetRutType() == 0 )
		pNRStart = pRoadSectionPrevious;
	else if( pRNext->GetRutType() == 0 )
		pNRStart = pRNext;
	else
		return FALSE;
	CGIS_NetworkRut *pNRNext = NULL;
	CGIS_NetworkRut *pNRut = NULL;
	CGIS_FeatureRut *pFRut = NULL;
	for( int i=0;	i<pVtx->GetVRutNum();	i++ )
	{
		if( pVtx->GetVRutList()[i] != pNRStart->GetRutID() )
		{
			pNRut = GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
			if( pNRut )
			{
				if( pNRut->GetRutType() == 0 )
					pNRNext = pNRut;
			}
		}
	}
	ASSERT( pNRNext );
	if( pNRNext == NULL )
		return FALSE;
	if( pVtx->GetVtxID() == pNRNext->GetRutStartVtxID() )
		pVtx = GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutEndVtxID() );
	else
		pVtx = GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutStartVtxID() );
	FULL_ROUND_COUNT++;
	if( FULL_ROUND_COUNT > 32 )
		return FALSE;
	return GetNextRoundRut( pRList, pVList, pVtx, pNRStart, pNRNext );
//	CGIS_NetworkRut *pNRStart = NULL;
//	if( pRoadSectionPrevious->GetRutType() == 0 )
//		pNRStart = pRoadSectionPrevious;
//	else if( pRNext->GetRutType() == 0 )
//		pNRStart = pRNext;
//	else
//		return FALSE;
//	CGIS_NetworkRut *pNRNext = NULL;
//	CGIS_NetworkRut *pNRut = NULL;
//	for( int i=0;	i<pVtx->GetVRutNum();	i++ )
//	{
//		if( pVtx->GetVRutList()[i] != pNRStart->GetRutID() )
//		{
//			pNRut = GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
//			if( pNRut )
//			{
//				if( pNRut->GetRutType() == 0 )
//					pNRNext = pNRut;
//			}
//		}
//	}
//	ASSERT( pNRNext );
//	if( pVtx->GetVtxID() == pNRNext->GetRutStartVtxID() )
//		pVtx = GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutEndVtxID() );
//	else
//		pVtx = GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutStartVtxID() );
//	return GetNextRoundRut( pRList, pVList, pVtx, pNRStart, pNRNext );
}
BOOL CGIS_MapBreadthNetwork::GetNextRoundRut(
    CPtrList *pRList, 
    CPtrList *pVList, 
    CGIS_NetworkVtx *pVtx,
    CGIS_NetworkRut *pRStart, 
    CGIS_NetworkRut *pRNext )
{
	if( pRStart == pRNext )
		return TRUE;
	CGIS_NetworkRut *pNRNext = NULL;
	CGIS_NetworkRut *pNRut = NULL;
	CGIS_BreadthMap *pBMNet = NULL;
	CGIS_FeatureRut *pFRut = NULL;

	
	if( pVtx->GetVtxEdge() == 1 )
	{
		if( pVtx->GetVBreadthID() != m_nBID )
		{
			pBMNet = m_pMap->GetBreadthMapByID( pVtx->GetVAbutBID() );
			if( pBMNet == NULL )
				return FALSE;


            int nReadErrorCode = 0;
			if(!ReadBreadthNetwork(&pBMNet,1,nReadErrorCode))
            {
                return FALSE;
            }
		}
		else
		{
			if( pBMNet == NULL )
				pBMNet = m_pMap->GetBreadthMapByID2( pVtx->GetVBreadthID() );
			if( pBMNet == NULL )
				return FALSE;
		}
		pNRNext = GetNetworkRut( pBMNet->m_nBreadthID, pVtx->GetVAbutRID() );
	}
	else
	{
		for( int i=0;	i<pVtx->GetVRutNum();	i++ )
		{
			if( pVtx->GetVRutList()[i] == pRNext->GetRutID() )
				continue;
			pNRut = GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
			if( pNRut )
			{
				if( pNRut->GetRutType() == 0 )
					pNRNext = pNRut;
			}
		}
	}
	if( pNRNext == NULL )
		return FALSE;
	pRList->AddTail( pNRut );
	if( pVtx->GetVtxID() == pNRNext->GetRutStartVtxID() )
		pVtx = GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutEndVtxID() );
	else
		pVtx = GetNetworkVtx( pNRNext->GetRBreadthID(), pNRNext->GetRutStartVtxID() );
	FULL_ROUND_COUNT++;
	if( FULL_ROUND_COUNT > 32 )
		return FALSE;
	if( pVtx )
	{
		pVList->AddTail( pVtx );
		return GetNextRoundRut( pRList, pVList, pVtx, pRStart, pNRNext );
	}
	return FALSE;	
	
}

void CGIS_MapBreadthNetwork::ClearNodeNetwork( CGIS_NetworkNode *pNode, CPtrList *pRList, CPtrList *pVList )
{
	CGIS_NetworkRut *pRut = NULL;
	CGIS_NetworkVtx *pVtx = NULL;
	CGIS_NetworkNode *pNTem = pNode->m_pParent;
	short nVRNum = 0;
	BOOL bFFlag = TRUE;
	while( pNTem )
	{
		pRut = pNTem->m_pRut;
		pVtx = pNTem->m_pVtx;
		if( pVtx && pRut )
		{
			pRList->AddTail( pRut );
			pVList->AddTail( pVtx );

            nVRNum = pVtx->GetVRutNum();
			ASSERT( nVRNum > 0 );
			CGIS_NetworkRut *pNRut = NULL;
			BYTE byRAType = 1;
			for( int i=0;	i<nVRNum;	i++ )
			{
				if( pVtx->GetVRutList()[i] == pRut->GetRutID() )
					continue;
				else
				{
					pNRut = GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
					if( pNRut )
					{
						if( pNRut->GetRutType() == 4 )
							byRAType = 2;
						else if( pNRut->GetRutType() == 0 )
							byRAType = 3;
					}
				}
			}
			pNTem = pNTem->m_pParent;
			if( pNTem )
			{
				if( pNTem->m_pRut && pNTem->m_pVtx )
				{
					if( bFFlag )
						FullPathNodeRuts( pRList, pVList, pVtx, pRut, pNTem->m_pRut, byRAType );
					if( pNTem->m_pRut->GetRutType() == 0 || pNTem->m_pRut->GetRutType() == 4 )
						bFFlag = FALSE;
					else 
						bFFlag = TRUE;
				}
			}
		}
		else
			ASSERT( FALSE );
	}
}
void CGIS_MapBreadthNetwork::ClearNodeNetwork( CPtrList *pRList, CPtrList *pVList, CPtrList *pNList )
{
	CGIS_NetworkRut *pRut = NULL;
	CGIS_NetworkVtx *pVtx = NULL;
	CGIS_NetworkVtx *pNVTem = NULL;
	CGIS_NetworkNode *pNode = NULL;
	POSITION pos = pNList->GetHeadPosition();
	while( pos )
	{
		pNode = (CGIS_NetworkNode*)pNList->GetNext(pos);
		if( pNode )
		{
			pRut = pNode->m_pRut;
            pVtx = pNode->m_pVtx;
            if( pVtx && pRut )
            {
                pRList->AddTail( pRut );
                m_pRutList[pRut->GetRBreadthID()][pRut->GetRutID()] = NULL;
                
                pNVTem = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutStartVtxID() );
                if( pNVTem )
                {
                    pVList->AddTail( pNVTem );
                    m_pVtxList[pNVTem->GetVBreadthID()][pNVTem->GetVtxID()] = NULL;
                }
                pNVTem = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutEndVtxID() );
                if( pNVTem )
                {
                    pVList->AddTail( pNVTem );
                    m_pVtxList[pNVTem->GetVBreadthID()][pNVTem->GetVtxID()] = NULL;
                }
                
            }
            else 
                ASSERT( FALSE );
		}
	}
}

void CGIS_MapBreadthNetwork::ClearHeadNetwork( CGIS_NetworkNode *pStartNode, CGIS_NetworkNode *pEndNode, CPtrList *pSOList, CPtrList *pSCList, CPtrList *pEOList, CPtrList *pECList )
{
	CPtrList RList, VList;
//	ClearNodeNetwork( pStartNode, &RList, &VList );
//	ClearNodeNetwork( pEndNode, &RList, &VList );
	ClearNodeNetwork( &RList, &VList, pSOList );
	ClearNodeNetwork( &RList, &VList, pSCList );
	ClearNodeNetwork( &RList, &VList, pEOList );
	ClearNodeNetwork( &RList, &VList, pECList );
	CGIS_NetworkRut *pNRut = NULL;
	CGIS_NetworkVtx *pNVtx = NULL;
	CGIS_NetworkVtx *pNVTem = NULL;
	CGIS_BreadthMap *pBMap = NULL;
	int i;
	for( i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
	{
//		pBMap = m_pMap->GetBreadthMapList()[i];
		pBMap = m_pMap->GetBreadthMapByID2( i );
		if( pBMap )
		{
			if( pBMap->m_nLoadCounter == 0 )
				continue;
			int j;
			for( j =0;	j<m_nRutNum[pBMap->m_nBreadthID]; j++ )
			{
				pNRut = GetNetworkRut(pBMap->m_nBreadthID,j);
				if( pNRut )
				{
					if( pNRut->GetRutLevel() >= m_byRLevel )
					{
						RList.AddTail( pNRut );
						pNVTem = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutStartVtxID() );
						if( pNVTem )
						{
							VList.AddTail( pNVTem );
							m_pVtxList[pBMap->m_nBreadthID][pNVTem->GetVtxID()] = NULL;
						}
						pNVTem = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutEndVtxID() );
						if( pNVTem )
						{
							VList.AddTail( pNVTem );
							m_pVtxList[pBMap->m_nBreadthID][pNVTem->GetVtxID()] = NULL;
						}
					}
					else 
					{
						delete pNRut;
						pNRut = NULL;
					}
				}
				m_pRutList[pBMap->m_nBreadthID][j] = NULL;
			}
			for( j=0;	j<m_nVtxNum[pBMap->m_nBreadthID];	j++ )
			{
				pNVtx = m_pVtxList[pBMap->m_nBreadthID][j];
				if( pNVtx )
				{
					delete pNVtx;
					pNVtx = NULL;
				}
				m_pVtxList[pBMap->m_nBreadthID][j] = NULL;
			}
		}
	}
	POSITION pos = RList.GetHeadPosition();
	while( pos )
	{
		pNRut = (CGIS_NetworkRut*)RList.GetNext(pos);
		if( pNRut )
			m_pRutList[pNRut->GetRBreadthID()][pNRut->GetRutID()] = pNRut;
	}
	pos = VList.GetHeadPosition();
	while( pos )
	{
		pNVtx = (CGIS_NetworkVtx*)VList.GetNext(pos);
		if( pNVtx )
			m_pVtxList[pNVtx->GetVBreadthID()][pNVtx->GetVtxID()] = pNVtx;
	}
}
void CGIS_MapBreadthNetwork::ClearNetwork()
{
	CGIS_NetworkRut *pNRut = NULL;
	CGIS_NetworkVtx *pNVtx = NULL;
	CGIS_BreadthMap *pBMap = NULL;
	int i;
	for( i = 0;	i < m_pMap->GetBreadthMapNum();	i++ )
	{
		pBMap = m_pMap->GetBreadthMapByID2( i );
		if(pBMap)
		{
			if( pBMap->m_nLoadCounter == 0 )
				continue;
            int j;
			for( j =0;	j<m_nRutNum[pBMap->m_nBreadthID]; j++ )
			{
				pNRut = GetNetworkRut(pBMap->m_nBreadthID,j);
				if( pNRut )
				{
					delete pNRut;
					pNRut = NULL;
				}
				m_pRutList[pBMap->m_nBreadthID][j] = NULL;
			}
			m_pMemMngOfBuddyData->newfree(m_pRutList[pBMap->m_nBreadthID]);
			for( j=0;	j<m_nVtxNum[pBMap->m_nBreadthID];	j++ )
			{
				pNVtx = GetNetworkVtx(pBMap->m_nBreadthID,j);
				if( pNVtx )
				{
					delete pNVtx;
					pNVtx = NULL;
				}
				m_pVtxList[pBMap->m_nBreadthID][j] = NULL;
			}
			m_pMemMngOfBuddyData->newfree(m_pVtxList[pBMap->m_nBreadthID]);
			pBMap->m_nLoadCounter = 0;
		}
	}
}

void CGIS_MapBreadthNetwork::ResetNetwork()
{
	CGIS_NetworkRut *pNRut = NULL;
	CGIS_NetworkVtx *pNVtx = NULL;
	CGIS_BreadthMap *pBMap = NULL;
	for( int i = 0;	i < m_pMap->GetBreadthMapNum();	i++ )
	{
		pBMap = m_pMap->GetBreadthMapByID2( i );
		if(pBMap)
		{
			if( pBMap->m_nLoadCounter == 0 )
				continue;
            
			for(int j=0;	j<m_nVtxNum[pBMap->m_nBreadthID];	j++ )
			{
				pNVtx = m_pVtxList[pBMap->m_nBreadthID][j];
				if( pNVtx )
				{
					pNVtx->ResetSortInfo();
				}

			}
		}
	}
}

void CGIS_MapBreadthNetwork::ClearBNetwork( CGIS_BreadthMap *pBMap )
{
	if( !pBMap )
        return;

	CGIS_NetworkRut *pNRut = NULL;
	CGIS_NetworkVtx *pNVtx = NULL;
    int nBreadthID = pBMap->m_nBreadthID;

    if(m_pRutList[nBreadthID])
	{
        CGIS_NetworkRut** pFtrArray = m_pRutList[nBreadthID];
        int nArraySize = m_nRutNum[nBreadthID];
		for( int j =0;	j < nArraySize; j++ )
		{
			pNRut = pFtrArray[j];
			if( pNRut )
			{
				delete pNRut;
				pNRut = NULL;
			}
			pFtrArray[j] = NULL;
		}

        
		m_pMemMngOfBuddyData->newfree(m_pRutList[nBreadthID]);
        m_pRutList[nBreadthID] = NULL;
    }
    m_nRutNum[nBreadthID] = 0;
    
    if(m_pVtxList[nBreadthID])
    {
        CGIS_NetworkVtx** pFtrArray = m_pVtxList[nBreadthID];
        int nArraySize = m_nVtxNum[nBreadthID];

		for(int j = 0;	j < nArraySize;	j++ )
		{
			pNVtx = pFtrArray[j];
			if( pNVtx )
			{
				delete pNVtx;
				pNVtx = NULL;
			}
            pFtrArray[j] = NULL;
        }
        
        m_pMemMngOfBuddyData->newfree(m_pVtxList[nBreadthID]);
        m_pVtxList[nBreadthID] = NULL;
    }
    m_nVtxNum[nBreadthID] = 0;
         
    pBMap->m_nLoadCounter = 0;	
}
BOOL CGIS_MapBreadthNetwork::LoadMapNetwork( )
{
	return TRUE;
}
BOOL CGIS_MapBreadthNetwork::UnLoadMapNetwork( CGIS_BreadthMap *pBMap )
{
	return TRUE;
}
///��õ����ڷַ���ͼ
BOOL CGIS_MapBreadthNetwork::GetBreadthMapOfPoint( float dx, float dy, CPtrList &BMapList )
{
	CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork2( CalculateBMapID( dx, dy ) );
	if( pBMap )
	{
		BMapList.AddTail( pBMap );
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief ����ָ����ͼ���е���������(������ͼ����id����),
 *
 * @param 
 * @return
 * @note �ж�ͼ���Ƿ���ʹ���е�����������SORT��Ϣ�е�sλ����ֵ,
 *          ��eλ����ֵ��������һ���߽�ڵ�
*/
void CGIS_MapBreadthNetwork::ClearNWBreadth( int *pArrayOfBreadthID, int nBreadthIndex )
{
    
	CPtrList RList,VList;
	CGIS_NetworkRut *pRut = NULL;
	CGIS_NetworkVtx *pVtx = NULL;

    int nBreadthID = pArrayOfBreadthID[nBreadthIndex];
    //------------------------------------------
    //����
	int i;
	for( i=0;	i<m_nVtxNum[nBreadthID];	i++ )
	{
		pVtx = m_pVtxList[nBreadthID][i];
		if( pVtx )
		{
			if( (pVtx->GetVtxSInfo()->byStatusOfNormalSearch != 0 || pVtx->GetVtxSInfo()->byStatusOfReverseSearch != 0) || pVtx->GetVtxEdge() == 1 )
			{
				if( pVtx->GetVtxSInfo()->byStatusOfNormalSearch != 0 || pVtx->GetVtxSInfo()->byStatusOfReverseSearch != 0 )
				{
					for( int j=0;	j<pVtx->GetVRutNum();	j++ )
					{
						pRut = GetNetworkRut(nBreadthID,pVtx->GetVRutList()[j]);
						if( pRut )
						{
							RList.AddTail( pRut );
							m_pRutList[nBreadthID][pVtx->GetVRutList()[j]] = NULL;
						}
					}
					VList.AddTail( pVtx );
					m_pVtxList[nBreadthID][i] = NULL;
				}
				else if( pVtx->GetVtxEdge() == 1 )
				{
					for( int j=0;	j<pVtx->GetVRutNum();	j++ )
					{
						pRut = GetNetworkRut(nBreadthID,pVtx->GetVRutList()[j]);
						if( pRut )
						{
							RList.AddTail( pRut );
							m_pRutList[nBreadthID][pVtx->GetVRutList()[j]] = NULL;
						}
					}
					VList.AddTail( pVtx );
					m_pVtxList[nBreadthID][i] = NULL;
				}
			}
		}
	}
    //----------------------------------------
    //����
	for( i=0;	i<m_nRutNum[nBreadthID];	i++ )
	{
		pRut = m_pRutList[nBreadthID][i];
		if( pRut )
		{
			delete pRut;
			pRut = NULL;
		}
		m_pRutList[nBreadthID][i] = NULL;
	}
	for( i=0;	i<m_nVtxNum[nBreadthID];	i++ )
	{
		pVtx = m_pVtxList[nBreadthID][i];
		if( pVtx )
		{
			delete pVtx;
			pVtx = NULL;
		}
		m_pVtxList[nBreadthID][i] = NULL;
	}
    //------------------------------------------
    //�ָ�
	POSITION pos = RList.GetHeadPosition();
	while( pos )
	{
		pRut = (CGIS_NetworkRut*)RList.GetNext(pos);
		if( pRut )
			m_pRutList[nBreadthID][pRut->GetRutID()] = pRut;			
	}
	pos = VList.GetHeadPosition();
	while( pos )
	{
		pVtx = (CGIS_NetworkVtx*)VList.GetNext(pos);
		if( pVtx )
			m_pVtxList[nBreadthID][pVtx->GetVtxID()] = pVtx;
	}
    //------------------------------------------
}
void CGIS_MapBreadthNetwork::ClearInvalNetwork(int *pSLBID, int nSLBNum, int nSMLBNum, int *pELBID, int nELBNum, int nEMLBNum )
{
	int nNum = nSLBNum>nSMLBNum?nSMLBNum:nSLBNum;
	int i;
	for( i=0;	i<nNum;	i++ )
		ClearNWBreadth( pSLBID, i );
	nNum = nELBNum>nEMLBNum?nEMLBNum:nELBNum;
	for( i=0;	i<nNum;	i++ )
		ClearNWBreadth( pELBID, i );
}


BOOL CGIS_MapBreadthNetwork::ReadBreadthNetwork( 
    CGIS_BreadthMap **pBNet, 
    BYTE byLFlag ,
    int& nErrorCode)
{
    //////////////////////////////////////////////////////////////////////////
    //���˳��ж�
	ExitJudge();
    //////////////////////////////////////////////////////////////////////////

	if( (*pBNet)->m_nLoadCounter != 0 )
	{
		(*pBNet)->m_nLoadCounter++;
		return TRUE;
	}
	if( gHNFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.Rut", m_pMap->GetMapInfo()->GetMapFilePath() );
		gHNFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHNFile)
			return FALSE;
	}

	ST_GEO_RECT rtRect = m_pMap->GetBreadthMapRect( *pBNet );
	if( !RectInPoly(rtRect,m_pRBuf, m_nRBNum) )
    {       
        TRACE0("the breadth which is be loaded is not in buffer poly!\n");
        return TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////


	TRY
	{
//		TRACE( _T("BLID:%d,%d\n"),(*pBNet)->m_nBreadthID,(*pBNet)->m_nBMID );
		CFile file(gHNFile);
        CFile* pFile = &file;
		pFile->SeekToBegin();
		char szInfo[32];
		pFile->Read( szInfo, 9 );
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		pFile->Read( &nMNum, sizeof(int) );
		int i;
		for( i=0;	i<nMNum;	i++ )
		{
			pFile->Read( &nMID, sizeof(int) );
			pFile->Read( &nMOff, sizeof(int) );
			if( nMID == m_pMap->GetMapInfo()->GetMapID() )
				break;
		}
		int nRawBreadthBID = 0;
		int dwCSize = 0;
		int nBOff = nMOff + 13 + ((*pBNet)->m_nBreadthID)*12;
		pFile->Seek( nBOff, CFile::begin );
		pFile->Read( &nRawBreadthBID, sizeof(int) );
        ASSERT(nRawBreadthBID == (*pBNet)->m_nBMID);
		pFile->Read( &nBOff, sizeof(int) );
		pFile->Read( &dwCSize, sizeof(int) );
		pFile->Seek( nMOff+nBOff, CFile::begin );

		int nUnCom = 5;
		pFile->Read( &nUnCom, sizeof(int) );
		int nCom = dwCSize - sizeof( int );
		if( nUnCom > nCom*8 )
			return FALSE;
        
//        auto_ptr <char> pFBuf   (new char[nUnCom]);
//        auto_ptr <char> pBuf    (new char[nCom]);
        char* pFBuf = m_pBufOfBeforeCompress;
        char* pBuf = m_pBufOfAfterCompress;
          
//////////////////////////////////////////////////////////////////////////
        DWORD dwSize = 0;
        DWORD dwDataPos = pFile->GetPosition();
        int nReadCountLimit = 2;//��������
        for(int nReadCount = 0; nReadCount < 2; nReadCount++)
        {
            pFile->Seek(dwDataPos,CFile::begin);    
                 
            UINT nRead = pFile->Read( pBuf, nCom );
            if(nRead != nCom)
            {
                TRACE2("read rtn != required size![%d<-%d]\n",nRead,nCom);
                return FALSE;
            }
            
            dwSize = nUnCom;
            int nUnComRtn = uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nCom );
            
            if(Z_OK == nUnComRtn)
            {
                if(nReadCount != 0)
                {
                    TRACE1("ReadBreadthNetwork reload[%d] sucess!\n",nReadCount);
                }
                break;
            }
        
#ifdef _DEBUG
            if(Z_OK != nUnComRtn)
            {                          
                TRACE3("[error:nCom = %d;nUnCom = %d,BreadthID = %d]\n",nCom,nUnCom,(*pBNet)->m_nBreadthID);
                TRACE1("uncompress failed when ReadBreadthNetwork! uncompress code = %d\n",nUnComRtn);
            }
#endif
            
            if(nReadCountLimit-1 == nReadCount)
            {
                m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;
                return FALSE;
            }

            
        }     
//////////////////////////////////////////////////////////////////////////


		///�ж��ļ��İ汾��Ϣ
		DWORD dwStation = 0;
		Fread( pFBuf, dwSize, dwStation, szInfo, 3, 1 );
		if( swString(szInfo).IsEmpty() )
		{
			return FALSE;
		}
		if( swString(szInfo).Find( L"RUT" ) < 0 )
		{
			return FALSE;
		}
		int nVer = 0;
		Fread( pFBuf, dwSize, dwStation, &nVer, sizeof(int), 1 );
		if( nVer != 910 )
		{
			return FALSE;
		}

		////��ȡ��������
		Fread( pFBuf, dwSize, dwStation, &nRawBreadthBID, sizeof(int), 1 );
        ASSERT(nRawBreadthBID == (*pBNet)->m_nBMID);
		int nRutNum = 0;
		int nVtxNum = 0;
		Fread( pFBuf, dwSize, dwStation, &nRutNum, sizeof(unsigned short), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVtxNum, sizeof(unsigned short), 1 );
		Fread( pFBuf, dwSize, dwStation, &nRutNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVtxNum, sizeof(int), 1 );
		if( nVtxNum == 0 )
		{
			return FALSE;
		}

		int nRLNum = 0;
		int nVLNum = 0;
		int nRLNum1 = 0;	//�ڶ��ȼ�·��
		int nVLNum1 = 0;
		int nVOff = 0;
		Fread( pFBuf, dwSize, dwStation, &nRLNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVLNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nRLNum1, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVLNum1, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVOff, sizeof(int), 1 );
		///��ȡ·����Ϣ
		DEFINE_VARIABLE_USED_BY_READ_ROADSECTION_FEATURE;


		int nReadNum = nRutNum;
		if( byLFlag == 3 )
			nReadNum = nRLNum;
		else if( byLFlag == 2 )
			nReadNum = nRLNum+nRLNum1;

        if(nReadNum == 0)
        {
            m_nRutNum[nBreadthID] = nReadNum;
            return TRUE;
        }
		m_nRutNum[nBreadthID] = nReadNum;
        m_pRutList[nBreadthID] = (CGIS_NetworkRut**)m_pMemMngOfBuddyData->newmalloc(m_nRutNum[nBreadthID]*sizeof(CGIS_NetworkRut*));
        if(!(m_pRutList[nBreadthID]))
        {
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_OUT_OF_MEMORY));
        }
        
		memset( m_pRutList[nBreadthID], 0, sizeof(CGIS_NetworkRut*)*m_nRutNum[nBreadthID] );
		for( i=0;	i<nReadNum;	i++ )
		{
			READ_ROADSECTION_FEATURE;

			CGIS_NetworkRut *pNRut = new CGIS_NetworkRut;
			pNRut->SetRutInfo( nRutID, byDirection, byType, byCharge, nLaneNum, bySpeed, 
					nRutLevel, byRutType, fRutLength, nVtxStartID, nVtxEndID, nBreadthID );
            pNRut->SetLimitInfo(pLimitInfo);
    
			m_pRutList[nBreadthID][nRutID] = pNRut;
		}
		
		//��ȡ�ڵ���Ϣ
		unsigned short	nVtxID = 0;		//�ڵ�ID
		ST_GEO_PTXY		ptPosition;		//�ڵ�����
		BYTE			byEdge = 0;		//�Ƿ���ͼ����Ե:0-����,1-��
		int				nABID = 0;		//�ڽ�ͼ����
		unsigned short	nARID = 0;		//����Rut_id
		BYTE			byRNum = 0;		//�����ӵ�·����
		unsigned short	pVRList[11];		//�����ӵ�·��ID
		unsigned short	pRTList[11];		//��ˮƽ����ļн�����
		BYTE			pRLList[11];		//·������ͼ��ID
		int		nVBID = 0;				//�ڵ�����ͼ��ID
		unsigned short nPos = 0;
		nReadNum = nVtxNum;
		if( byLFlag == 3 )
		{
			dwStation = nVOff;
			nReadNum = nVLNum;
		}
		else if( byLFlag == 2 )
		{
			dwStation = nVOff;
			nReadNum = nVLNum+nVLNum1;
		}

        if(nReadNum == 0)
        {
		    m_nVtxNum[nBreadthID] = nReadNum;
			ClearBNetwork( *pBNet );
            return FALSE;
        }
		m_nVtxNum[nBreadthID] = nReadNum;
		m_pVtxList[nBreadthID] = (CGIS_NetworkVtx**)m_pMemMngOfBuddyData->newmalloc(m_nVtxNum[nBreadthID]*sizeof(CGIS_NetworkVtx*));
        if(!(m_pVtxList[nBreadthID]))
        {
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_OUT_OF_MEMORY));
        }

		memset( m_pVtxList[nBreadthID], 0, sizeof(CGIS_NetworkVtx*)*m_nVtxNum[nBreadthID] );
//		ST_GEO_RECT rt = CalculateBMapRect( (*pBNet)->m_nBMID );
		for( i=0;	i<nReadNum;	i++ )
		{
			Fread( pFBuf, dwSize, dwStation, &nVtxID, sizeof(unsigned short), 1 );
			if( nVtxID > nReadNum )
			{
                TRACE0("vtx's nVtxID is wrong!(> vtx's num)\n");
				m_byErrCode = nErrorCode =CGIS_NetworkException::ERROR_FILE_IO;;
				ClearBNetwork( *pBNet );
				return FALSE;
			}
			Fread( pFBuf, dwSize, dwStation, &ptPosition, sizeof(ST_GEO_PTXY), 1 );
//			Fread( pFBuf, dwSize, dwStation, &nPos, sizeof(unsigned short), 1 );
//			ptPosition.dx = rt.dLeft + nPos/100000.0;
//			Fread( pFBuf, dwSize, dwStation, &nPos, sizeof(unsigned short), 1 );
//			ptPosition.dy = rt.dTop + nPos/100000.0;
			Fread( pFBuf, dwSize, dwStation, &byEdge, sizeof(BYTE), 1 );
			if( byEdge == 1 )
			{
				Fread( pFBuf, dwSize, dwStation, &nABID, sizeof(int), 1 );
				Fread( pFBuf, dwSize, dwStation, &nARID, sizeof(unsigned short), 1 );
			}
			Fread( pFBuf, dwSize, dwStation, &byRNum, sizeof(BYTE), 1 );
			if( byRNum > 10 || byRNum == 0 )
			{
                TRACE0("vtx's rutnum is wrong!(> 10 ||  == 0)\n");
				m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;;
				ClearBNetwork( *pBNet );
				return FALSE;
			}
			ASSERT( byRNum > 0 );
			Fread( pFBuf, dwSize, dwStation, pVRList, sizeof(unsigned short)*byRNum, 1 );
			Fread( pFBuf, dwSize, dwStation, pRTList, sizeof(unsigned short)*byRNum, 1 );
			Fread( pFBuf, dwSize, dwStation, pRLList, sizeof(BYTE)*byRNum, 1 );
			CGIS_NetworkVtx *pNVtx = new CGIS_NetworkVtx;
			pNVtx->SetVtxInfo( nVtxID, ptPosition, nBreadthID );
			pNVtx->SetVEInfo( byEdge, nABID, nARID );
			pNVtx->SetVRutInfo( byRNum, pVRList, pRTList, pRLList );
			m_pVtxList[nBreadthID][nVtxID] = pNVtx;
		}

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        
		if( m_nSLBNum >= m_nMaxSLBNum && m_byCFlag == 1 )
		{
			ClearNWBreadth( m_pSLBID, m_nSLBNum%m_nMaxSLBNum );
		}
		if( m_nELBNum >= m_nMaxELBNum && m_byCFlag == 2 )
		{
			ClearNWBreadth( m_pELBID, m_nELBNum%m_nMaxELBNum );
		}
        
		{
			if( m_byCFlag == 1 )
			{
				m_pSLBID[m_nSLBNum%m_nMaxSLBNum] = (*pBNet)->m_nBreadthID;
				m_nSLBNum++;					//�Ѽ���ͼ������
			}
			else if( m_byCFlag == 2 )
			{
				m_pELBID[m_nELBNum%m_nMaxELBNum] = (*pBNet)->m_nBreadthID;
				m_nELBNum++;					//�Ѽ���ͼ������
			}
		}

		(*pBNet)->m_nLoadCounter++;
	}
	CATCH( CMemoryException, e )
	{

        TRACE0("catched CMemoryException\n");
        m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;
        
		ClearBNetwork( *pBNet );
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
        TRACE0("catched CFileException\n");
		m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;

		ClearBNetwork( *pBNet );
		if(INVALID_HANDLE_VALUE != gHNFile)
			CloseHandle( gHNFile );
		gHNFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	AND_CATCH( CGIS_NetworkException, e )
	{
		TRACE0("catched CGIS_NetworkException\n");
		m_byErrCode = nErrorCode = e->GetErrorCode();
		ClearBNetwork( *pBNet );
		return FALSE;
	}
	AND_CATCH( CUserException, e )
	{
		TRACE0("catched CUserException\n");
		m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
		ClearBNetwork( *pBNet );
		return FALSE;
	}
	END_CATCH

        
	return TRUE;
}

BOOL CGIS_MapBreadthNetwork::ReadBreadthNetworkRoadSectionByIDArray(
    CGIS_BreadthMap **pBNet, 
    BYTE byRoadSectionNum,
    unsigned short* pArrayOfRoadSectionID,
    int& nErrorCode)
{
    ASSERT((*pBNet)->m_nLoadCounter);
	if( gHNFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.Rut", m_pMap->GetMapInfo()->GetMapFilePath() );
		gHNFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHNFile)
			return FALSE;
	}

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    int nLoadedRoadSectionNum = 0;
	TRY
	{
//		TRACE( _T("BLID:%d,%d\n"),(*pBNet)->m_nBreadthID,(*pBNet)->m_nBMID );
		CFile file(gHNFile);
        CFile* pFile = &file;
		pFile->SeekToBegin();
		char szInfo[32];
		pFile->Read( szInfo, 9 );
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		pFile->Read( &nMNum, sizeof(int) );
		int i;
		for( i=0;	i<nMNum;	i++ )
		{
			pFile->Read( &nMID, sizeof(int) );
			pFile->Read( &nMOff, sizeof(int) );
			if( nMID == m_pMap->GetMapInfo()->GetMapID() )
				break;
		}
		int nRawBreadthBID = 0;
		int dwCSize = 0;
		int nBOff = nMOff + 13 + ((*pBNet)->m_nBreadthID)*12;
		pFile->Seek( nBOff, CFile::begin );
		pFile->Read( &nRawBreadthBID, sizeof(int) );
        ASSERT(nRawBreadthBID == (*pBNet)->m_nBMID);
		pFile->Read( &nBOff, sizeof(int) );
		pFile->Read( &dwCSize, sizeof(int) );
		pFile->Seek( nMOff+nBOff, CFile::begin );

		int nUnCom = 5;
		pFile->Read( &nUnCom, sizeof(int) );
		int nCom = dwCSize - sizeof( int );
		if( nUnCom > nCom*8 )
			return FALSE;
        
        char* pFBuf = m_pBufOfBeforeCompress;
        char* pBuf = m_pBufOfAfterCompress;
          
//////////////////////////////////////////////////////////////////////////
        DWORD dwSize = 0;
        DWORD dwDataPos = pFile->GetPosition();
        int nReadCountLimit = 2;//��������
        for(int nReadCount = 0; nReadCount < 2; nReadCount++)
        {
            pFile->Seek(dwDataPos,CFile::begin);    
                 
            UINT nRead = pFile->Read( pBuf, nCom );
            if(nRead != nCom)
            {
                TRACE2("read rtn != required size![%d<-%d]\n",nRead,nCom);
                return FALSE;
            }
            
            dwSize = nUnCom;
            int nUnComRtn = uncompress( (unsigned char*)pFBuf, &dwSize, (unsigned char*)pBuf, nCom );
            
            if(Z_OK == nUnComRtn)
            {
                if(nReadCount != 0)
                {
                    TRACE1("ReadBreadthNetwork reload[%d] sucess!\n",nReadCount);
                }
                break;
            }
        
#ifdef _DEBUG
            if(Z_OK != nUnComRtn)
            {                          
                TRACE3("[error:nCom = %d;nUnCom = %d,BreadthID = %d]\n",nCom,nUnCom,(*pBNet)->m_nBreadthID);
                TRACE1("uncompress failed when ReadBreadthNetwork! uncompress code = %d\n",nUnComRtn);
            }
#endif
            
            if(nReadCountLimit-1 == nReadCount)
            {
                m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;
                return FALSE;
            }

            
        }     
//////////////////////////////////////////////////////////////////////////


		///�ж��ļ��İ汾��Ϣ
		DWORD dwStation = 0;
		Fread( pFBuf, dwSize, dwStation, szInfo, 3, 1 );
		if( swString(szInfo).IsEmpty() )
		{
			return FALSE;
		}
		if( swString(szInfo).Find( L"RUT" ) < 0 )
		{
			return FALSE;
		}
		int nVer = 0;
		Fread( pFBuf, dwSize, dwStation, &nVer, sizeof(int), 1 );
		if( nVer != 910 )
		{
			return FALSE;
		}

		////��ȡ��������
		Fread( pFBuf, dwSize, dwStation, &nRawBreadthBID, sizeof(int), 1 );
        ASSERT(nRawBreadthBID == (*pBNet)->m_nBMID);
		int nRutNum = 0;
		int nVtxNum = 0;
		Fread( pFBuf, dwSize, dwStation, &nRutNum, sizeof(unsigned short), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVtxNum, sizeof(unsigned short), 1 );
		Fread( pFBuf, dwSize, dwStation, &nRutNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVtxNum, sizeof(int), 1 );
		if( nVtxNum == 0 )
		{
			return FALSE;
		}

		int nRLNum = 0;
		int nVLNum = 0;
		int nRLNum1 = 0;	//�ڶ��ȼ�·��
		int nVLNum1 = 0;
		int nVOff = 0;
		Fread( pFBuf, dwSize, dwStation, &nRLNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVLNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nRLNum1, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVLNum1, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVOff, sizeof(int), 1 );
		///��ȡ·����Ϣ
        DEFINE_VARIABLE_USED_BY_READ_ROADSECTION_FEATURE;
		int nReadNum = nRutNum;
        if(nReadNum == 0)
        {
            return FALSE;
        }
        
        ASSERT(m_nRutNum[nBreadthID] != 0 && m_nRutNum[nBreadthID] <= nReadNum);
        ASSERT(m_pRutList[nBreadthID]);
        unsigned short nOldRutNum = m_nRutNum[nBreadthID];
        CGIS_NetworkRut** pOldRutList  = m_pRutList[nBreadthID];
        
        CGIS_NetworkRut** pNewRutList = (CGIS_NetworkRut**)m_pMemMngOfBuddyData->newmalloc(nReadNum*sizeof(CGIS_NetworkRut*));
        if(!(pNewRutList))
        {
            THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        }
        memset(pNewRutList,0,nReadNum*sizeof(CGIS_NetworkRut*));
        memcpy(pNewRutList,pOldRutList,nOldRutNum*sizeof(CGIS_NetworkRut*));
        m_pMemMngOfBuddyData->newfree(pOldRutList);
		m_nRutNum[nBreadthID] = nReadNum;
        m_pRutList[nBreadthID] = pNewRutList;
        
		for( i=0;	i<nReadNum;	i++ )
		{
            READ_ROADSECTION_FEATURE;
            for(int i = 0; i < byRoadSectionNum; i++)
            {
                if(nRutID == pArrayOfRoadSectionID[i])
                {
                    if(!m_pRutList[nBreadthID][nRutID])
                    {
                        CGIS_NetworkRut *pNRut = new CGIS_NetworkRut;
                        
                        pNRut->SetRutInfo( nRutID, byDirection, byType, byCharge, nLaneNum, bySpeed, 
                            nRutLevel, byRutType, fRutLength, nVtxStartID, nVtxEndID, nBreadthID );
                        pNRut->SetLimitInfo( pLimitInfo );
                        m_pRutList[nBreadthID][nRutID] = pNRut;                       
                    }
                    nLoadedRoadSectionNum++;
                    break;
                }
            }
            if(nLoadedRoadSectionNum == byRoadSectionNum)
                break;

		}
		
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////       
	}
	CATCH( CMemoryException, e )
	{

        TRACE0("catched CMemoryException\n");
        m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;
        
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
        TRACE0("catched CFileException\n");
		m_byErrCode = nErrorCode = CGIS_NetworkException::ERROR_FILE_IO;

		if(INVALID_HANDLE_VALUE != gHNFile)
			CloseHandle( gHNFile );
		gHNFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	AND_CATCH( CGIS_NetworkException, e )
	{
		TRACE0("catched CGIS_NetworkException\n");
		m_byErrCode = nErrorCode = e->GetErrorCode();
		return FALSE;
	}
	END_CATCH

        
	return TRUE;
}


//��ö�ӦID�ķַ�·��
CGIS_BreadthMap *CGIS_MapBreadthNetwork::GetBreadthMapOfNetwork( int nNID )
{
//	CGIS_BreadthMap *pBNet = NULL;
//	return m_pMap->GetBreadthMapList()[nNID];
	return m_pMap->GetBreadthMapByID2( nNID );
//	for( int i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
//	{
//		pBNet = m_pMap->GetBreadthMapList()[i];
//		if( pBNet )
//		{
//			if( pBNet->m_nBreadthID == nNID )
//				return pBNet;
//		}
//	}
	return NULL;
}
CGIS_BreadthMap *CGIS_MapBreadthNetwork::GetBreadthMapOfNetwork2( int nBID )
{
	return m_pMap->GetBreadthMapByID( nBID );
}
///�õ��¸��ڵ�
CGIS_NetworkVtx* CGIS_MapBreadthNetwork::GetNextRutVtx( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut, BOOL bHead )
{
	CGIS_NetworkVtx* pRtnVtx = NULL;
	// ��㵽�յ�
	if( pRut->GetRutStartVtxID() == pVtx->GetVtxID() )
	{
		if( pRut->GetRutDirection() == 0 )
			pRtnVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutEndVtxID() );
		else if( pRut->GetRutDirection() == 1 && bHead )
			pRtnVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutEndVtxID() );
		else if( pRut->GetRutDirection() == 2 && !bHead )
			pRtnVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutEndVtxID() );
	}
	else 
	{
		// �յ㵽���
		if( pRut->GetRutDirection() == 0 )
			pRtnVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutStartVtxID() );
		else if( pRut->GetRutDirection() == 1 && !bHead )
			pRtnVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutStartVtxID() );
		else if( pRut->GetRutDirection() == 2 && bHead )
			pRtnVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutStartVtxID() );
	}
	return pRtnVtx;
}
///���ӵ�˫�п����Ƿ���Ե���·�ε���һ���ڵ�
BOOL CGIS_MapBreadthNetwork::CouldReachAnotherVertexInRoadSection( CGIS_NetworkVtx *pVtx, CGIS_NetworkRut *pRut )
{
    ASSERT(pVtx);
    ASSERT(pRut);
    ASSERT(pVtx->GetVBreadthID() == pRut->GetRBreadthID());
    
    if( pRut->GetRutDirection() == 0 )
        return TRUE;

    if(pRut->GetRutStartVtxID() == pVtx->GetVtxID())
    {
        if(pRut->GetRutDirection() == 1)
            return TRUE;
    }
    
    if(pRut->GetRutEndVtxID() == pVtx->GetVtxID())
    {
        if(pRut->GetRutDirection() == 2)
            return TRUE;
    }

    return FALSE;
}
CGIS_NetworkVtx* CGIS_MapBreadthNetwork::GetNextRutVtx2( 
    CGIS_NetworkVtx *pVtx,
    CGIS_NetworkRut *pRut,
    BOOL bHead )
{
	CGIS_NetworkVtx *pSVtx = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutStartVtxID() );
	CGIS_NetworkVtx *pEVtx = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutEndVtxID() );
	if( pSVtx == NULL || pEVtx == NULL )
	{
		ASSERT( pSVtx && pEVtx );
		return NULL;
	}
	if( pSVtx->GetVtxEdge() == 1 && pEVtx->GetVtxEdge() == 1 )
	{
		CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork(pVtx->GetVBreadthID());
		if( pBMap )
		{
			if( pBMap->m_nBMID == pSVtx->GetVAbutBID() && pVtx->GetVRutList()[0] == pSVtx->GetVAbutRID() )
				return GetNextRutVtx( pSVtx, pRut, bHead );
			else
				return GetNextRutVtx( pEVtx, pRut, bHead );

		}
		else
			ASSERT( FALSE );
	}
	else if( pSVtx->GetVtxEdge() == 1 )
	{
		return GetNextRutVtx( pSVtx, pRut, bHead );
	}
	else if( pEVtx->GetVtxEdge() == 1 )
	{
		return GetNextRutVtx( pEVtx, pRut, bHead );
	}
	else
		ASSERT( FALSE );
	return NULL;
}

int CGIS_MapBreadthNetwork::GetWPProjRut( CGIS_WayPoint *pWayPoint, IGIS_Map *pMap, CGIS_FeatureRut **pRList, CGIS_BreadthMap **pBMaps, ST_GEO_PTXY *pPPtList, int nRNum, ST_GEO_PTXY ptEnd,int& nErrorCode )
{
	CGIS_NetworkRut *pNRut = NULL;
	CGIS_NetworkVtx *pNVtx = NULL;
	CGIS_BreadthMap *pBMap = NULL;
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	for( int i=0;	i<nRNum;	i++ )
	{
		if( pRList[i] )
		{
			pBMap = pBMaps[i];
			if( pBMap == NULL )
				continue;
            int nReadErrorCode = 0;
			if( !ReadBreadthNetwork( &pBMap, m_LoadNetworkFlag,nReadErrorCode))
            {
                nErrorCode = nReadErrorCode;
                return -1;
            }
			pNRut = GetNetworkRut( pBMap->m_nBreadthID, pRList[i]->GetRutID() );
			if( pNRut == NULL )
				continue;
			ASSERT( pNRut );
			if( pNRut->GetRutDirection() != 3 )
			{
				pWayPoint->SetWPProjPosition( pPPtList[i], eMapType );
				pWayPoint->SetWPRutID( pRList[i]->GetRutID(), eMapType );
				pWayPoint->SetWPRutNID( pBMaps[i]->m_nBreadthID, eMapType );
				pWayPoint->SetWPMapID( pMap->GetMapInfo()->GetMapID(), eMapType );
                pWayPoint->SetWPRutLevel(pNRut->GetType(),eMapType);
				CGIS_BreadthMap *pBMap = pBMaps[i];
				if( pBMap == NULL )
					continue;
                int nReadErrorCode = 0;
				if(!ReadBreadthNetwork( &pBMap, m_LoadNetworkFlag ,nReadErrorCode))
					continue;
				CGIS_NetworkVtx *pVtxStart = GetNetworkVtx( pNRut->GetRBreadthID(),pNRut->GetRutStartVtxID() );
				CGIS_NetworkVtx *pVtxEnd = GetNetworkVtx( pNRut->GetRBreadthID(),pNRut->GetRutEndVtxID() );
				if( pVtxStart == NULL || pVtxEnd == NULL )
					continue;
				if( ((pVtxStart->GetVRutNum()==1&&pVtxStart->GetVtxEdge()!=1) || 
					(pVtxEnd->GetVRutNum()==1&&pVtxEnd->GetVtxEdge()!=1) ) && pNRut->GetRutDirection()!=0 )
					continue;
				int nVtxID = CalcDist(ptEnd.dx, ptEnd.dy, pVtxStart->GetVtxPosition().dx, pVtxStart->GetVtxPosition().dy) > 
						CalcDist(ptEnd.dx, ptEnd.dy, pVtxEnd->GetVtxPosition().dx, pVtxEnd->GetVtxPosition().dy)?
						pVtxEnd->GetVtxID():pVtxStart->GetVtxID();
				pWayPoint->SetWPVtxID( nVtxID, eMapType );
				swString strWPRutName;
				CharToUnicode( pRList[i]->GetAnnotation() , &strWPRutName );
				pWayPoint->SetWPRutName( strWPRutName );
				unsigned short nPPos = 0;
				for( int j=0;	j<pRList[i]->GetPtNum()-1;	j++ )
				{
					nPPos = j;
					if( PointOnLine( pPPtList[i], pRList[i]->GetPtList()[j], pRList[i]->GetPtList()[j+1] ) )
						break;
				}
				pWayPoint->SetWPRutPtList( pRList[i]->GetPtList(), pRList[i]->GetPtNum(), nPPos, eMapType );
				return i;
			}
		}
	}
	return -1;
}
int CGIS_MapBreadthNetwork::FullWayPointData2( 
    CGIS_WayPoint *pWayPoint, 
    CGIS_GeoMap *pMap, 
    CGIS_NetworkPathInfo *pPPInfo, 
    ST_GEO_PTXY ptEnd,
    int& nErrorCode)
{
	CGIS_FeatureRut *pFRut = pPPInfo->GetCFRut();
	if( pFRut == NULL )
		return FullWayPointData( pWayPoint, pMap, ptEnd,nErrorCode );
	CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork( pFRut->m_nBreadthID );
	if( pBMap == NULL )
		return 0;
    int nReadErrorCode = 0;
	if( !ReadBreadthNetwork( &pBMap,1, nReadErrorCode))
    {
        return 0;
    }
	CGIS_NetworkRut *pNRut = GetNetworkRut( pBMap->m_nBreadthID, pFRut->GetRutID() );
	if( pNRut == NULL )
		return 0;
	CGIS_NetworkVtx *pStartNode = GetNetworkVtx( pBMap->m_nBreadthID, pNRut->GetRutStartVtxID() );
	CGIS_NetworkVtx *pEndNode = GetNetworkVtx( pBMap->m_nBreadthID, pNRut->GetRutEndVtxID() );
	if( pStartNode==NULL || pEndNode==NULL )
		return 0;
	if( pNRut->GetRutDirection() == 1 || pNRut->GetRutDirection() == 2 )
	{
		if( pNRut->GetRutDirection() == 1 )
			pWayPoint->SetWPVtxID( pEndNode->GetVtxID(), pMap->GetMapInfo()->GetMapType() );
		else
			pWayPoint->SetWPVtxID( pStartNode->GetVtxID(), pMap->GetMapInfo()->GetMapType() );
	}
	else
	{
		if( pPPInfo->GetCFDType() == 0 )
		{
			double dbDis1 = CalcDist(pStartNode->GetVtxPosition().dx, pStartNode->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy );
			double dbDis2 = CalcDist(pEndNode->GetVtxPosition().dx, pEndNode->GetVtxPosition().dy, ptEnd.dx, ptEnd.dy );
			if( dbDis1 < dbDis2 )
				pWayPoint->SetWPVtxID(pStartNode->GetVtxID(), pMap->GetMapInfo()->GetMapType() );
			else
				pWayPoint->SetWPVtxID(pEndNode->GetVtxID(), pMap->GetMapInfo()->GetMapType() );
		}
		else if( pPPInfo->GetCFDType() == 1 )
		{
			double dbDis1 = CalcDist(pStartNode->GetVtxPosition().dx, pStartNode->GetVtxPosition().dy, 
				pFRut->GetPtList()[pFRut->GetPtNum()-1].dx, pFRut->GetPtList()[pFRut->GetPtNum()-1].dy );
			double dbDis2 = CalcDist(pEndNode->GetVtxPosition().dx, pEndNode->GetVtxPosition().dy, 
				pFRut->GetPtList()[pFRut->GetPtNum()-1].dx, pFRut->GetPtList()[pFRut->GetPtNum()-1].dy );
			if( dbDis1 < dbDis2 )
				pWayPoint->SetWPVtxID(pStartNode->GetVtxID(), pMap->GetMapInfo()->GetMapType() );
			else
				pWayPoint->SetWPVtxID(pEndNode->GetVtxID(), pMap->GetMapInfo()->GetMapType() );
		}
		else if( pPPInfo->GetCFDType() == 2 )
		{
			double dbDis1 = CalcDist(pStartNode->GetVtxPosition().dx, pStartNode->GetVtxPosition().dy, 
				pFRut->GetPtList()[0].dx, pFRut->GetPtList()[0].dy );
			double dbDis2 = CalcDist(pEndNode->GetVtxPosition().dx, pEndNode->GetVtxPosition().dy, 
				pFRut->GetPtList()[0].dx, pFRut->GetPtList()[0].dy );
			if( dbDis1 < dbDis2 )
				pWayPoint->SetWPVtxID(pStartNode->GetVtxID(), pMap->GetMapInfo()->GetMapType() );
			else
				pWayPoint->SetWPVtxID(pEndNode->GetVtxID(), pMap->GetMapInfo()->GetMapType() );
		}
	}
	swString strFRut;
	CharToUnicode( pFRut->GetAnnotation() , &strFRut );
	pWayPoint->SetWPRutName( strFRut );
	int nPos = 0;
	double dbDis = 1000000000;
	for( int i=0;	i< pFRut->GetPtNum()-1;	i++ )
	{
		if( fabs(pFRut->GetPtList()[i].dx-pFRut->GetPtList()[i+1].dx)>=FLOAT_ZERO || fabs(pFRut->GetPtList()[i].dy-pFRut->GetPtList()[i+1].dy)>=FLOAT_ZERO )
		{
			double dbTem = 0.0;
			ST_GEO_PTXY ptTem = DisPointToLine( pWayPoint->GetWPPosition(), pFRut->GetPtList()[i], pFRut->GetPtList()[i+1] );
			BOOL bOnLine = PointOnLine(ptTem,pFRut->GetPtList()[i], pFRut->GetPtList()[i+1]);
			if(bOnLine)
				dbTem = CalcDist( ptTem.dx, ptTem.dy, pWayPoint->GetWPPosition().dx, pWayPoint->GetWPPosition().dy );
			else
			{
				double dis1 = CalcDist(pWayPoint->GetWPPosition().dx, pWayPoint->GetWPPosition().dy,pFRut->GetPtList()[i].dx,pFRut->GetPtList()[i].dy);
				double dis2 = CalcDist(pWayPoint->GetWPPosition().dx, pWayPoint->GetWPPosition().dy,pFRut->GetPtList()[i+1].dx,pFRut->GetPtList()[i+1].dy);
				if( dis1 < dis2 )
				{
					dbTem = CalcDist( pWayPoint->GetWPPosition().dx, pWayPoint->GetWPPosition().dy, pFRut->GetPtList()[i].dx, pFRut->GetPtList()[i].dy );
					ptTem = pFRut->GetPtList()[i];
				}
				else 
				{
					dbTem = CalcDist( pWayPoint->GetWPPosition().dx, pWayPoint->GetWPPosition().dy, pFRut->GetPtList()[i+1].dx, pFRut->GetPtList()[i+1].dy );
					ptTem = pFRut->GetPtList()[i+1];
				}
			}
			if( dbDis > dbTem )
			{
				dbDis = dbTem;
				pWayPoint->SetWPProjPosition( ptTem, pMap->GetMapInfo()->GetMapType() );
				nPos = i;
			}
		}
	}
	pWayPoint->SetWPRutID( pFRut->GetRutID(), pMap->GetMapInfo()->GetMapType() );
	pWayPoint->SetWPRutNID( pFRut->m_nBreadthID, pMap->GetMapInfo()->GetMapType() );
	pWayPoint->SetWPMapID( pMap->GetMapInfo()->GetMapID(), pMap->GetMapInfo()->GetMapType() );
	pWayPoint->SetWPRutPtList( pFRut->GetPtList(), pFRut->GetPtNum(), nPos, pMap->GetMapInfo()->GetMapType() );
    pWayPoint->SetWPRutLevel(pNRut->GetType(),pMap->GetMapInfo()->GetMapType() );

    return 1;
}
/**
 * @brief ���;��������
 *
 * @param 
 * @return
*/
BOOL CGIS_MapBreadthNetwork::FullWayPointData( 
    CGIS_WayPoint *pWayPoint,
    CGIS_GeoMap *pMap, 
    ST_GEO_PTXY ptEnd,
    int& nErrorCode)
{
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	if( pWayPoint->GetWPRutPtNum(eMapType) >= 2 )
	{
		CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork( pWayPoint->GetWPRutNID(eMapType) );
		if( pBMap == NULL )
			return FALSE;
        int nReadErrorCode = 0;
		if( !ReadBreadthNetwork( &pBMap, m_LoadNetworkFlag,nReadErrorCode ))
        {
            nErrorCode = nReadErrorCode;
            return FALSE;
        }

		return TRUE;
	}

    if(!pMap->CalWayPointRut( pWayPoint, ptEnd ,nErrorCode))
    {
        return FALSE;
    }
    
	return TRUE;
}
int CGIS_MapBreadthNetwork::Clear( int nVal )
{
	ClearNetwork();
	return nVal;
}

/**
 * @brief �������ڽڵ�,�˺������׳��쳣
 *
 * @param 
 * @return
*/
CGIS_NetworkVtx *CGIS_MapBreadthNetwork::GetVAbutVtx( CGIS_NetworkVtx *pVtx )
{
	if( pVtx == NULL )
		return NULL;
	CGIS_NetworkVtx *pVAbut = NULL;
	if( pVtx->GetVtxEdge() == 1 )
	{
		CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork2( pVtx->GetVAbutBID() );
		if( pBMap == NULL )
			return pVtx;

        int nReadErrorCode = 0;
		if( !ReadBreadthNetwork( &pBMap, m_LoadNetworkFlag, nReadErrorCode ))
		{
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,(CGIS_NetworkException::enErrorCode)nReadErrorCode));
		}

        
		CGIS_NetworkRut *pNERut = GetNetworkRut( pBMap->m_nBreadthID, pVtx->GetVAbutRID() );
		if( pNERut )
		{
			CGIS_NetworkVtx *pSVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNERut->GetRutStartVtxID() );
			CGIS_NetworkVtx *pEVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNERut->GetRutEndVtxID() );
			if( pSVtx && pEVtx )
			{
				if( pSVtx->GetVtxEdge() == 1 && pEVtx->GetVtxEdge() == 1 )
				{
					CGIS_BreadthMap *pBTemp = GetBreadthMapOfNetwork(pVtx->GetVBreadthID());
					if( pBTemp )
					{
						if( pBTemp->m_nBMID == pSVtx->GetVAbutBID() && pVtx->GetVRutList()[0] == pSVtx->GetVAbutRID() )
							pVAbut = pSVtx;
						else
							pVAbut = pEVtx;
					}
					else
						ASSERT( FALSE );
				}
				else if( pSVtx->GetVtxEdge() == 1 )
					pVAbut = pSVtx;
				else if( pEVtx->GetVtxEdge() == 1 )
					pVAbut = pEVtx;
			}
		}
	}

	if( pVAbut == NULL )
		pVAbut = pVtx;

    ASSERT(pVAbut);
	return pVAbut;
}
/**
 * @brief ����;���������ڵ�
 *
 * @param 
 * @return
 * @note ���￼����·�εĵ�˫����Ϣ.
*/
BOOL CGIS_MapBreadthNetwork::GetRealVtx( 
    CGIS_WayPoint *pWPOne, 
    CGIS_WayPoint *pWPTwo,
    CGIS_GeoMap *pMap, 
    CGIS_NetworkVtx **pStartVtx, 
    CGIS_NetworkVtx **pEndVtx )
{
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	CGIS_NetworkRut *pRut = GetNetworkRut( pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType) );
	if( pRut == NULL )
		return FALSE;

    unsigned char nDirection = pRut->GetRutDirection();
    switch(nDirection)
    {
    case 0://˫���򰴾���ȡ�ڵ�
        {
            CGIS_NetworkVtx *pVStart    = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutStartVtxID() );
            CGIS_NetworkVtx *pVEnd      = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutEndVtxID() );
            ASSERT( pVEnd && pVStart );
            if( pVStart == NULL || pVEnd == NULL )
                return FALSE;
            if( CalcDist(pVStart->GetVtxPosition().dx,pVStart->GetVtxPosition().dy, pWPTwo->GetWPProjPosition(eMapType).dx,pWPTwo->GetWPProjPosition(eMapType).dy) > 
                CalcDist(pVEnd->GetVtxPosition().dx,pVEnd->GetVtxPosition().dy, pWPTwo->GetWPProjPosition(eMapType).dx,pWPTwo->GetWPProjPosition(eMapType).dy) )
            {
                if( pVStart->GetVRutNum() <= 1 && pVEnd->GetVRutNum() <= 1 )
                    *pStartVtx = pVEnd;
                else if( pVEnd->GetVRutNum() <= 1 )
                    *pStartVtx = pVStart;
                else
                    *pStartVtx = pVEnd;
            }
            else
            {
                if( pVStart->GetVRutNum() <= 1 && pVEnd->GetVRutNum() <= 1 )
                    *pStartVtx = pVStart;
                else if( pVStart->GetVRutNum() <= 1 )
                    *pStartVtx = pVEnd;
                else
                    *pStartVtx = pVStart;
            }
        }
        break;
    case 1://ȡβ�ڵ�Ϊ��ʼ�ڵ�
        *pStartVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutEndVtxID() );
        break;
    case 2://ȡ�׽ڵ�Ϊ��ʼ�ڵ�
		*pStartVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutStartVtxID() );
        break;
    default:
        ASSERT(FALSE);
        break;  
    }

	pWPOne->SetWPVtxID( (*pStartVtx)->GetVtxID(), eMapType );
    //////////////////////////////////////////////////////////////////////////
    //����Ŀ��ڵ�
	pRut = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) );
	if( pRut == NULL )
		return FALSE;
    
    nDirection = pRut->GetRutDirection();
    switch(nDirection)
    {
    case 0://˫���򰴾���ȡ�ڵ�
        {
            CGIS_NetworkVtx *pVStart = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutStartVtxID() );
            CGIS_NetworkVtx *pVEnd = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutEndVtxID() );
            ASSERT( pVEnd && pVStart );
            if( pVStart == NULL || pVEnd == NULL )
                return FALSE;
            if( CalcDist(pVStart->GetVtxPosition().dx,pVStart->GetVtxPosition().dy, pWPOne->GetWPProjPosition(eMapType).dx,pWPOne->GetWPProjPosition(eMapType).dy) > 
                CalcDist(pVEnd->GetVtxPosition().dx,pVEnd->GetVtxPosition().dy, pWPOne->GetWPProjPosition(eMapType).dx,pWPOne->GetWPProjPosition(eMapType).dy) )
            {
                if( pVStart->GetVRutNum() <= 1 && pVEnd->GetVRutNum() <= 1 )
                    *pEndVtx = pVEnd;
                else if( pVEnd->GetVRutNum() <= 1 )
                    *pEndVtx = pVStart;
                else
                    *pEndVtx = pVEnd;
            }
            else
            {
                if( pVStart->GetVRutNum() <= 1 && pVEnd->GetVRutNum() <= 1 )
                    *pEndVtx = pVStart;
                else if( pVStart->GetVRutNum() <= 1 )
                    *pEndVtx = pVEnd;
                else
                    *pEndVtx = pVStart;
            }
        }
        break;
    case 1://ȡβ�ڵ�Ϊ��ʼ�ڵ�
        *pEndVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutStartVtxID() );
        break;
    case 2://ȡ�׽ڵ�Ϊ��ʼ�ڵ�
		*pEndVtx = GetNetworkVtx( pRut->GetRBreadthID(),pRut->GetRutEndVtxID() );
        break;
    default:
        ASSERT(FALSE);
        break;  
    }
	pWPTwo->SetWPVtxID( (*pEndVtx)->GetVtxID(), eMapType );
    //////////////////////////////////////////////////////////////////////////
    
	return TRUE;
}
void CGIS_MapBreadthNetwork::ExtendNormalNode(
	CGIS_NetworkNode *pParentNode,
    CPtrList *pOpenList, 
	CGIS_NetworkVtx *pEndVtx, 
    CGIS_NetworkPathInfo *pPathInfo, 
    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup, 
    BYTE nRLevel, 
    BOOL bFromHeadFlag,
    BOOL bSearchStartNodeSign)
{
    	////��������·�ε����ȴ���
	int nChild = 0;
	int nParent = 0;
	int i;
	for( i=0;	i<pParentNode->m_pVtx->GetVRutNum();	i++ )
	{
		if( pParentNode->m_pVtx->GetVRutList()[i] == pParentNode->m_pRut->GetRutID() )
		{
			nParent++;
			break;
		}
		nParent++;
	}
    CGIS_NetworkRut* pRut = NULL;
    CGIS_NetworkVtx* pVtx = NULL;
	for( i=0;	i<pParentNode->m_pVtx->GetVRutNum();	i++ )
	{
		nChild++;
		pRut = GetNetworkRut( pParentNode->m_pVtx->GetVBreadthID(), pParentNode->m_pVtx->GetVRutList()[i] );
		if( pRut == NULL )
			continue;
		if( pParentNode->m_pRut->GetRutID() == pRut->GetRutID() && bSearchStartNodeSign == FALSE )
			continue;
		if( pRut->GetRutLevel() < nRLevel )
			continue;
		pVtx = GetNextRutVtx( pParentNode->m_pVtx, pRut, bFromHeadFlag );
		if( pVtx == NULL )
		{
			ASSERT( pVtx == NULL );
			continue;
		}
        if(pLimitInfoGroup)
        {
            BOOL bLimit = FALSE;
            if(bFromHeadFlag)
            {
                if(pParentNode->m_g > 1800)
                {
                    bLimit = pLimitInfoGroup->IfAbsoluteLimit(pRut->GetRutID());
                }
                else
                {
                    bLimit = pLimitInfoGroup->IfLimit(pRut->GetRutID(),CGIS_NetworkStatus::m_nStartCalculateTime+pParentNode->m_g/600);
                }
            }
            else
            {
                bLimit = pLimitInfoGroup->IfAbsoluteLimit(pRut->GetRutID());
            }
            
            if( bLimit )
                continue;
        }
		CGIS_NetworkNode* pChildNode = new CGIS_NetworkNode;

		pChildNode->m_pParent = pParentNode;
		pChildNode->m_pRut = pRut;
		pChildNode->m_pVtx = pVtx;
		int nAngle = 0;
		if( bFromHeadFlag )
			nAngle = pParentNode->m_pVtx->GetVRTurnList()[nParent-1]-pParentNode->m_pVtx->GetVRTurnList()[nChild-1];
		else
			nAngle = pParentNode->m_pVtx->GetVRTurnList()[nChild-1]-pParentNode->m_pVtx->GetVRTurnList()[nParent-1];
		BYTE byAngle = 1;
		int nAng = abs( abs(nAngle)-180 );
		//ֱ��
		if( nAng < 8 )
			byAngle = 1;
		else if( nAng < 50 )
			byAngle = 2;
		else if( nAng < 120 )
			byAngle = 3;
		else if( nAng < 160 )
			byAngle = 4;
		else
			byAngle = 5;
			
		CalculateNetworkNode( pChildNode, pOpenList, pPathInfo, pEndVtx->GetVtxPosition(), byAngle, bFromHeadFlag );
    }

}
void CGIS_MapBreadthNetwork::ExtendEdgeNode(
	CGIS_NetworkNode *pParentNode,
    CPtrList *pOpenList, 
	CGIS_NetworkVtx *pEndVtx, 
    CGIS_NetworkPathInfo *pPathInfo, 
    BYTE nRLevel, 
    BOOL bFromHeadFlag)
{
    ASSERT(pParentNode);
    ASSERT(pParentNode->m_pVtx);
    ASSERT(pParentNode->m_pVtx->GetVtxEdge());

    
    CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork2( pParentNode->m_pVtx->GetVAbutBID() );
    if( pBMap == NULL )
    {     
        return;
        //THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    
    int nReadErrorCode = 0;
    if( !ReadBreadthNetwork( &pBMap, m_LoadNetworkFlag ,nReadErrorCode))
    {
        return;
        //THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,(CGIS_NetworkException::enErrorCode)nReadErrorCode));
    }
    
    
    CGIS_NetworkRut* pRut = GetNetworkRut( pBMap->m_nBreadthID, pParentNode->m_pVtx->GetVAbutRID() );
    if( pRut == NULL )
    {

        return;
        //THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    CGIS_NetworkVtx *pSVtx = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutStartVtxID() );
    CGIS_NetworkVtx *pEVtx = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutEndVtxID() );
    if( pSVtx == NULL || pEVtx == NULL )
    {
        return;
        //THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    
    
    
    if( pSVtx->GetVtxEdge() == 1 || pEVtx->GetVtxEdge() == 1 )
    {
        ASSERT( (fabs(pSVtx->GetVtxPosition().dx-pParentNode->m_pVtx->GetVtxPosition().dx)<FLOAT_ZERO &&
            fabs(pSVtx->GetVtxPosition().dy-pParentNode->m_pVtx->GetVtxPosition().dy)<FLOAT_ZERO ) ||
            ( fabs(pEVtx->GetVtxPosition().dx-pParentNode->m_pVtx->GetVtxPosition().dx)<FLOAT_ZERO &&
            fabs(pEVtx->GetVtxPosition().dy-pParentNode->m_pVtx->GetVtxPosition().dy)<FLOAT_ZERO ) );
    }
    else
        ASSERT( FALSE );
    
    
    CGIS_NetworkVtx* pVtx = GetNextRutVtx2( pParentNode->m_pVtx, pRut, bFromHeadFlag );
    if( pVtx == NULL )
    {
        return;
        //THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    
    if( pRut->GetRutLevel() < nRLevel )
        return;
    
    CGIS_NetworkNode* pChildNode = new CGIS_NetworkNode;
    pChildNode->m_pParent = pParentNode;
    pChildNode->m_pRut = pRut;
    pChildNode->m_pVtx = pVtx;      
    CalculateNetworkNode( pChildNode, pOpenList, pPathInfo, pEndVtx->GetVtxPosition(), 1, bFromHeadFlag );
    
    
    if( pSVtx->GetVtxEdge() == 1 )
    {
        if( bFromHeadFlag )
            pSVtx->GetVtxSInfo()->byStatusOfNormalSearch = 1;
        else
            pSVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
    }
    if( pEVtx->GetVtxEdge() == 1 )
    {
        if( bFromHeadFlag )
            pEVtx->GetVtxSInfo()->byStatusOfNormalSearch = 1;
        else
            pEVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
    }		
}

/**
 * @brief ��Ŀ���̽·
 *
 * @note ע��,�˺������׳��쳣
 * @return TRUE-�ҵ�Ŀ��� FALSE-δ�ҵ�Ŀ���
*/
BOOL CGIS_MapBreadthNetwork::CalculateOptimalPathRut12( 
    CPtrList *pOpenList, 
    CPtrList *pCloseList, 
    unsigned short *pVIDs,
    int *pVBIDs, 
    unsigned short &nVNum, 
	CGIS_NetworkVtx *pEndVtx, 
    CGIS_NetworkPathInfo *pPathInfo, 
    BYTE nRLevel, 
    BOOL bFromHeadFlag,
    BOOL bSearchStartNodeSign )
{
	ASSERT( pOpenList->GetCount() > 0 );
	if( pOpenList->GetCount() <= 0 )
		return FALSE;

	CGIS_NetworkNode *pParentNode = (CGIS_NetworkNode*)pOpenList->GetHead();
    pParentNode->m_pVtx->GetVtxSInfo()->posOHead = pParentNode->m_pVtx->GetVtxSInfo()->posOEnd = NULL;
    pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 2;

    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup = NULL;
    GET_LIMIT_INFO_V3(pParentNode,bFromHeadFlag,pLimitInfoGroup);


    CGIS_NetworkNode** pNodeArray = (CGIS_NetworkNode**)pVIDs;
    ASSERT(pVIDs);
	for( int i=0;	i<nVNum;	i++ )
	{
        CGIS_NetworkVtx* pDestVtx = pNodeArray[i]->m_pVtx;
        //pDestVtx = GetVAbutVtx(pDestVtx);
        //ASSERT(pDestVtx);
        if(pDestVtx)
        {
            if(pParentNode->m_pVtx->GetVtxEdge() == 1)
            {
                if(pDestVtx->GetVtxEdge() == 1)
                {
                    if(pParentNode->m_pVtx->GetVAbutRID() == pNodeArray[i]->m_pRut->GetRutID() 
                        && 
                        pParentNode->m_pVtx->GetVAbutBID() == pDestVtx->GetVBreadthID())
                    {
                        nVNum = i;
                        return TRUE;
                    }
                    else
                    {
                        pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
                    }
                }
            }
            if( pParentNode->m_pVtx == pDestVtx )
            {
                //int nVtxID = pVIDs[i];
                //////////////////////////////////////////////////////////////////////////
                //ȡ������Ϣ.
                
                BOOL bLimit = FALSE;
                {
                    unsigned short nRoadSectionID = pNodeArray[i]->m_pRut->GetRutID();
                    GET_NORMAL_ROADSECTION_ID(pNodeArray[i],nRoadSectionID);
                    if(pLimitInfoGroup)
                    {
                        bLimit = pLimitInfoGroup->IfAbsoluteLimit(nRoadSectionID);
                    }
                }           
                if(!bLimit)
                {
                    nVNum = i;
                    return TRUE;
                    
                }
                else
                {
                    pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
                }
                
                //////////////////////////////////////////////////////////////////////////
            }
        }
	}
	pOpenList->RemoveHead( );
	if( pParentNode->m_pRut )
	{
		if( pParentNode->m_pRut->GetRutType() == 4 )
			pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
	}
	pCloseList->AddTail( pParentNode );
    //////////////////////////////////////////////////////////////////////////

	CGIS_NetworkRut *pRut = NULL;
	CGIS_NetworkVtx *pVtx = NULL;
	CGIS_NetworkNode *pChildNode = NULL;
	CGIS_NetworkNode *pOldNode = NULL;

	///����ڵ�Ϊ�߽�ڵ�����
	if( pParentNode->m_pVtx->GetVtxEdge() == 1 )
	{
        ExtendEdgeNode(pParentNode,pOpenList,pEndVtx,pPathInfo,nRLevel,bFromHeadFlag);
	}
    else
    {
        ExtendNormalNode(pParentNode,pOpenList,pEndVtx,pPathInfo,pLimitInfoGroup,nRLevel,bFromHeadFlag,bSearchStartNodeSign);
			
	}
	return FALSE;
}
///��������·��
BOOL CGIS_MapBreadthNetwork::CalculateOptimalPathRut( 
    CPtrList *pOpenList, 
    CPtrList *pCloseList, 
    CGIS_NetworkVtx *pMVtx,     //�ж�����Ľڵ����
    CGIS_NetworkVtx *pEndVtx,   //��λ�ǲ����Ѿ��ҵ��Ľڵ����
    CGIS_NetworkPathInfo *pPathInfo,
    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup,
    BYTE nRLevel, 
    BOOL bFromHeadFlag, 
    BOOL bSearchStartNodeSign )
{
	ASSERT( pOpenList->GetCount() > 0 );
	if( pOpenList->GetCount() <= 0 )
		return FALSE;
    
	CGIS_NetworkNode *pParentNode = (CGIS_NetworkNode*)pOpenList->GetHead();
    pParentNode->m_pVtx->GetVtxSInfo()->posOHead = pParentNode->m_pVtx->GetVtxSInfo()->posOEnd = NULL;
    pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 2;
   
    //DUMPSTATE(pParentNode);

    if( pParentNode->m_pVtx == pMVtx )
	{ 
        //return TRUE;
        //////////////////////////////////////////////////////////////////////////
        //070512��Ӵ˴���,�������ҵ�Ŀ���ʱ�ж��Ƿ�Ŀ��·���Ƿ�����.
        if(pParentNode->m_pVtx->GetVtxEdge() == 1)
        {
            return TRUE;
        }
        else
        {
            ASSERT(m_pDestRut);
            BOOL bLimit = FALSE;
            if(pLimitInfoGroup)
            {
                bLimit = pLimitInfoGroup->IfAbsoluteLimit(m_pDestRut->GetRutID());   
            }
            if(!bLimit)
            {
                return TRUE;
            }
            else
            {
                pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
            }
        }
        //////////////////////////////////////////////////////////////////////////
	}

    //////////////////////////////////////////////////////////////////////////
    //this code for debug network code
#ifdef _DEBUG_VIEW_CALCULATE_PATH_PROCESS
    CGeoView* pView = GetMapView ();
    ASSERT(pView);
    {
        CDC* pDC = pView->GetDC();
        {          
            ST_GEO_PTXY* pPt = &pParentNode->m_pVtx->GetVtxPosition();
            int  nPosX = 0;
            int  nPosY = 0;
            pView->XyMapToView( pPt->dx, pPt->dy, nPosX, nPosY );
            
            pDC->Ellipse(nPosX - 8, nPosY - 8, nPosX + 8, nPosY + 8);          
        }    
        pView->ReleaseDC(pDC);
    }
    Sleep(500);
#endif    
    //////////////////////////////////////////////////////////////////////////
    

    pOpenList->RemoveHead( );
    if( pParentNode->m_pRut )
    {
        if( pParentNode->m_pRut->GetRutType() == 4 )
            pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pParentNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
    }
    
    pCloseList->AddTail( pParentNode );


	CGIS_NetworkRut *pRut = NULL;
	CGIS_NetworkVtx *pVtx = NULL;
	CGIS_NetworkNode *pChildNode = NULL;
	CGIS_NetworkNode *pOldNode = NULL;
	
	///����ڵ�Ϊ�߽�ڵ�����
	if( pParentNode->m_pVtx->GetVtxEdge() == 1 )
	{
		CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork2( pParentNode->m_pVtx->GetVAbutBID() );
		if( pBMap == NULL )
			return FALSE;

        int nReadErrorCode = 0;
		if( !ReadBreadthNetwork( &pBMap, m_LoadNetworkFlag, nReadErrorCode ))
        {
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_OUT_OF_MEMORY));            
        }
		pRut = GetNetworkRut( pBMap->m_nBreadthID, pParentNode->m_pVtx->GetVAbutRID() );
		if( pRut == NULL )
			return FALSE;
		CGIS_NetworkVtx *pSVtx = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutStartVtxID() );
		CGIS_NetworkVtx *pEVtx = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutEndVtxID() );
//		ASSERT( pSVtx && pEVtx );
		if( pSVtx == NULL || pEVtx == NULL )
			return FALSE;
		if( pSVtx->GetVtxEdge() == 1 || pEVtx->GetVtxEdge() == 1 )
		{
			ASSERT( (fabs(pSVtx->GetVtxPosition().dx-pParentNode->m_pVtx->GetVtxPosition().dx)<FLOAT_ZERO &&
				fabs(pSVtx->GetVtxPosition().dy-pParentNode->m_pVtx->GetVtxPosition().dy)<FLOAT_ZERO ) ||
				( fabs(pEVtx->GetVtxPosition().dx-pParentNode->m_pVtx->GetVtxPosition().dx)<FLOAT_ZERO &&
				fabs(pEVtx->GetVtxPosition().dy-pParentNode->m_pVtx->GetVtxPosition().dy)<FLOAT_ZERO ) );
		}
		else
			ASSERT( FALSE );
		pVtx = GetNextRutVtx2( pParentNode->m_pVtx, pRut, bFromHeadFlag );
		if( pVtx == NULL )
		{
            TRACE0("ASSERT( pVtx )failed!\n");
			//ASSERT( pVtx );
			return FALSE;
		}
		if( pRut->GetRutLevel() < nRLevel )
			return FALSE;
		pChildNode = new CGIS_NetworkNode;
		if( pChildNode == NULL )
			return FALSE;
		pChildNode->m_pParent = pParentNode;
		pChildNode->m_pRut = pRut;
		pChildNode->m_pVtx = pVtx;
		CalculateNetworkNode( pChildNode, pOpenList, pPathInfo, pEndVtx->GetVtxPosition(), 1, bFromHeadFlag );


		if( pSVtx->GetVtxEdge() == 1 )
		{
			if( bFromHeadFlag )
				pSVtx->GetVtxSInfo()->byStatusOfNormalSearch = 1;
			else
				pSVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
		}
		if( pEVtx->GetVtxEdge() == 1 )
		{
			if( bFromHeadFlag )
				pEVtx->GetVtxSInfo()->byStatusOfNormalSearch = 1;
			else
				pEVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;
		}
		return FALSE;
	}
	////��������·�ε����ȴ���
	int nChild = 0;
	int nParent = 0;
	int i;
	for( i=0;	i<pParentNode->m_pVtx->GetVRutNum();	i++ )
	{
		if( pParentNode->m_pVtx->GetVRutList()[i] == pParentNode->m_pRut->GetRutID() )
		{
			nParent++;
			break;
		}
		nParent++;
	}
	for( i=0;	i<pParentNode->m_pVtx->GetVRutNum();	i++ )
	{
		nChild++;
		pRut = GetNetworkRut( pParentNode->m_pVtx->GetVBreadthID(), pParentNode->m_pVtx->GetVRutList()[i] );
		if( pRut == NULL )
			continue;
		if( pParentNode->m_pRut->GetRutID() == pRut->GetRutID() && bSearchStartNodeSign == FALSE )
			continue;
		if( pRut->GetRutLevel() < nRLevel )
			continue;
		pVtx = GetNextRutVtx( pParentNode->m_pVtx, pRut, bFromHeadFlag );
		if( pVtx == NULL )
			continue;
        if(pLimitInfoGroup)
        {
            BOOL bLimit = FALSE;
            if(bFromHeadFlag)
            {
                if(pParentNode->m_g > 1800)
                {
                    bLimit = pLimitInfoGroup->IfAbsoluteLimit(pRut->GetRutID());
                }
                else
                {
                    bLimit = pLimitInfoGroup->IfLimit(pRut->GetRutID(),CGIS_NetworkStatus::m_nStartCalculateTime+pParentNode->m_g/600);
                }
            }
            else
            {
                bLimit = pLimitInfoGroup->IfAbsoluteLimit(pRut->GetRutID());
            }
            
            if( bLimit )
                continue;
        }
        //////////////////////////////////////////////////////////////////////////
        if(pRut->GetRutType() == 5)
        {
            pParentNode->m_bHaveJCT = TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        
		if( m_byRLevel == 4 )//��ǰ�������͹��ܼ���Ϊ4
		{
            //��ǰ·��Ϊ���ڸ���
            if( pParentNode->m_pVtx->GetVRLList()[i] == 11 )
            {
				if( pRut->GetRutType() != 4 && pRut->GetRutType() != 1)// && pRut->GetRutType() != 0)
                    pParentNode->m_nLRNum ++;
                
            }

		}
		else if( m_byRLevel == 3 )//��ǰ�������͹��ܼ���Ϊ3
		{
            //ͼ��11Ϊ����//ͼ��10Ϊ���������ڸ���//9Ϊ�������ɵ�
			if( pParentNode->m_pVtx->GetVRLList()[i] == 10 || pParentNode->m_pVtx->GetVRLList()[i] == 11 )
			{

				if( pRut->GetRutType() != 4 && pRut->GetRutType() != 1)// && pRut->GetRutType() != 0)
                     pParentNode->m_nLRNum ++;
			}
		}
        else if(m_byRLevel == 0)
        {
            if(pRut->GetRutLevel() >= 1) //if( pParentNode->m_pVtx->GetVRutList()[i] == 5)
            {
               if( pRut->GetRutType() != 4)// && pRut->GetRutType() != 1)
                   pParentNode->m_nLRNum++;
            }
        }
		pChildNode = new CGIS_NetworkNode;
		if( pChildNode == NULL )
			return FALSE;
		pChildNode->m_pParent = pParentNode;
		pChildNode->m_pRut = pRut;
		pChildNode->m_pVtx = pVtx;
		int nAngle = 0;
		if( bFromHeadFlag )
			nAngle = pParentNode->m_pVtx->GetVRTurnList()[nParent-1]-pParentNode->m_pVtx->GetVRTurnList()[nChild-1];
		else
			nAngle = pParentNode->m_pVtx->GetVRTurnList()[nChild-1]-pParentNode->m_pVtx->GetVRTurnList()[nParent-1];
		if( bSearchStartNodeSign )
		{
			if( pParentNode->m_pRut->GetRutID() == pRut->GetRutID() && pParentNode->m_pRut->GetRBreadthID() == pRut->GetRBreadthID() )
				nAngle = 180;
		}
		BYTE byAngle = 1;

		int nAng = abs( abs(nAngle)-180 );
		//ֱ��
		if( nAng < 8 )
			byAngle = 1;
		else if( nAng < 50 )
			byAngle = 2;
		else if( nAng < 120 )
			byAngle = 3;
		else if( nAng < 160 )
			byAngle = 4;
		else
			byAngle = 5;
			
		CalculateNetworkNode( pChildNode, pOpenList, pPathInfo, pEndVtx->GetVtxPosition(), byAngle, bFromHeadFlag );

	}
	return FALSE;
}

///�����ʼwaypoint·����Ϣ
BOOL CGIS_MapBreadthNetwork::FullWPStartPath(
    CGIS_WayPoint*      pWPOne, 
    CGIS_NetworkVtx*    pVtx,       //����·�εĽڵ�
    CGIS_NetworkRut*    pRut,       //·��
    CGIS_GeoMap*        pMap, 
    BOOL&               bRutSame, 
    CPtrList*           pPathPointList,
    POSITION&           pos )
{
	CGIS_NetworkVtx *pVTemp = pVtx;
    //�ڵ��·�β���ͬһ��·��,��ôȡ���ڽڵ�
	if( pVtx->GetVBreadthID() != pRut->GetRBreadthID() )
	{
		CGIS_NetworkVtx *pVNext = GetNextRutVtx2(pVtx,pRut);
		if( pVNext )
		{
			if( pVNext->GetVtxID() == pRut->GetRutStartVtxID() )
				pVTemp = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutEndVtxID() );
			else
				pVTemp = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutStartVtxID() );
		}
		else
			pVTemp = pVtx;
	}

	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	if( pWPOne->GetWPType() == EN_WAYPOINTTYPE_ORIGIN )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPOne->GetWPProjPosition(eMapType) );
		pPPMark->SetWayPoint( pWPOne );
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_START );
		CGIS_NetworkRut *pNRut = GetNetworkRut( pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType) );
        ASSERT(pNRut);
		if( pNRut == NULL )
			return FALSE;
        
		pMap->FullVRutInfo( pNRut, pPPMark->GetVRutInfo() );//���ݱ�·��������һ��pp���rut������Ϣ
		pos = pPathPointList->AddTail( pPPMark );
	}



	bRutSame = FALSE;
	BOOL bOrder = FALSE;//��ʾȡ������˳��������.�����·�εĽڵ�����ʼ�ڵ���Ϊ˳�����Ϊ����
	if( pRut->GetRutID() == pWPOne->GetWPRutID(eMapType) && pRut->GetRBreadthID() == pWPOne->GetWPRutNID(eMapType))
	{
		bRutSame = TRUE;
		if( pVTemp->GetVtxID() == pRut->GetRutStartVtxID() )
			bOrder = TRUE;
		else 
			bOrder = FALSE;
	}
	else
	{
		bRutSame = FALSE;
		CGIS_NetworkRut *pNRut = GetNetworkRut( pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType) );
		if( pNRut == NULL )
			return FALSE;

        //����ڵ�IDΪ·�ε���ʼID �� �˽ڵ��·����ͬһ��ͼ���Ļ���˳�����
		if( pVTemp->GetVtxID() == pNRut->GetRutStartVtxID() && pVTemp->GetVBreadthID() == pNRut->GetRBreadthID())
        { 
			bOrder = TRUE;
        }
		else
		{
			CGIS_NetworkVtx *pNVS = GetNetworkVtx(pNRut->GetRBreadthID(), pNRut->GetRutStartVtxID());
			CGIS_NetworkVtx *pNVE = GetNetworkVtx(pNRut->GetRBreadthID(), pNRut->GetRutEndVtxID());
			if( pNVS && pNVE )
			{
				if( CalcDist(pVTemp->GetVtxPosition().dx, pVTemp->GetVtxPosition().dy, pNVS->GetVtxPosition().dx, pNVS->GetVtxPosition().dy) > 
					CalcDist(pVTemp->GetVtxPosition().dx, pVTemp->GetVtxPosition().dy, pNVE->GetVtxPosition().dx, pNVE->GetVtxPosition().dy) )
					bOrder = FALSE;
				else
					bOrder = TRUE;
			}
		}
	}


	///���waypoint���·���ϵ�����
	ST_GEO_PTXY *pPtList = pWPOne->GetWPRutPtList(eMapType);
	unsigned short nPtNum = pWPOne->GetWPRutPtNum(eMapType);
	unsigned short nPPos = pWPOne->GetWPProjPos(eMapType);
	if( pPtList == NULL )
		return FALSE;
	if( nPtNum <= 2 )
		return TRUE;
	if( (bRutSame && bOrder) || (bRutSame==FALSE && bOrder==FALSE) )
	{
		for( int i=nPPos+1;	i<nPtNum-1;	i++ )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	else
	{
		for( int i=nPPos;	i>0;	i-- )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	return TRUE;
}
///�����ʼwaypoint·����Ϣ
BOOL CGIS_MapBreadthNetwork::FullWPStartPathV2(
    CGIS_WayPoint*      pWPOne, 
    CGIS_NetworkVtx*    pVtx,
    CGIS_NetworkRut*    pRut,
    CGIS_GeoMap*        pMap, 
    BOOL&               bRutSame, 
    CPtrList*           pPathPointList,
    POSITION&           pos )
{
    ASSERT(pRut == NULL);//û��·��
	CGIS_NetworkVtx *pVTemp = pVtx;


	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	if( pWPOne->GetWPType() == EN_WAYPOINTTYPE_ORIGIN )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPOne->GetWPProjPosition(eMapType) );
		pPPMark->SetWayPoint( pWPOne );
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_START );
		CGIS_NetworkRut *pNRut = GetNetworkRut( pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType) );
        ASSERT(pNRut);
		if( pNRut == NULL )
			return FALSE;
        
		pMap->FullVRutInfo( pNRut, pPPMark->GetVRutInfo() );//���ݱ�·��������һ��pp���rut������Ϣ
		pos = pPathPointList->AddTail( pPPMark );
	}

    BOOL bOrder = FALSE;
	{
		bRutSame = FALSE;
		CGIS_NetworkRut *pNRut = GetNetworkRut( pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType) );
		if( pNRut == NULL )
			return FALSE;

        //����ڵ�IDΪ·�ε���ʼID �� �˽ڵ��·����ͬһ��ͼ���Ļ���˳�����
		if( pVTemp->GetVtxID() == pNRut->GetRutStartVtxID() && pVTemp->GetVBreadthID() == pNRut->GetRBreadthID())
        { 
            bOrder = TRUE;
        }
		else
		{
			CGIS_NetworkVtx *pNVS = GetNetworkVtx(pNRut->GetRBreadthID(), pNRut->GetRutStartVtxID());
			CGIS_NetworkVtx *pNVE = GetNetworkVtx(pNRut->GetRBreadthID(), pNRut->GetRutEndVtxID());
			if( pNVS && pNVE )
			{
				if( CalcDist(pVTemp->GetVtxPosition().dx, pVTemp->GetVtxPosition().dy, pNVS->GetVtxPosition().dx, pNVS->GetVtxPosition().dy) > 
					CalcDist(pVTemp->GetVtxPosition().dx, pVTemp->GetVtxPosition().dy, pNVE->GetVtxPosition().dx, pNVE->GetVtxPosition().dy) )
					bOrder = FALSE;
				else
					bOrder = TRUE;
			}
		}
	}
	///���waypoint���·���ϵ�����
	ST_GEO_PTXY *pPtList = pWPOne->GetWPRutPtList(eMapType);
	unsigned short nPtNum = pWPOne->GetWPRutPtNum(eMapType);
	unsigned short nPPos = pWPOne->GetWPProjPos(eMapType);
	if( pPtList == NULL )
		return FALSE;
	if( nPtNum <= 2 )
		return TRUE;
	if( (bRutSame && bOrder) || (bRutSame==FALSE && bOrder==FALSE) )
	{
		for( int i=nPPos+1;	i<nPtNum-1;	i++ )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	else
	{
		for( int i=nPPos;	i>0;	i-- )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	return TRUE;
}///�����ֹwaypoint·����Ϣ
BOOL CGIS_MapBreadthNetwork::FullWPEndPath_DELETE( 
    CGIS_WayPoint *pWPTwo, 
    CGIS_NetworkVtx *pVtx, 
    CGIS_NetworkRut *pRut, 
    int nRutID,
    CGIS_GeoMap *pMap,
    CPtrList *pPathPointList,
    POSITION &pos )
{
	////β��waypoint�Ĵ���
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	BOOL bRutSame = FALSE, bOrder = FALSE;
	if( nRutID == pWPTwo->GetWPRutID(eMapType) )
	{
		bRutSame = TRUE;
		if( pVtx->GetVtxID() == pRut->GetRutStartVtxID() )
			bOrder = TRUE;
		else 
			bOrder = FALSE;
	}
	else
	{
		bRutSame = FALSE;
		CGIS_NetworkRut *pNRut = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) );
		if( pNRut == NULL )
			return FALSE;
		if( pVtx->GetVtxID() == pNRut->GetRutStartVtxID() )
			bOrder = TRUE;
		else
			bOrder = FALSE;
	}
	///���waypoint���·���ϵ�����
	ST_GEO_PTXY *pPtList = pWPTwo->GetWPRutPtList(eMapType);
	unsigned short nPtNum = pWPTwo->GetWPRutPtNum(eMapType);
	unsigned short nPPos = pWPTwo->GetWPProjPos(eMapType);
	if( pPtList == NULL )
		return FALSE;
	if( bRutSame )
	{
		unsigned short nCount = 0;
		if( nPtNum <= 2 )
			nCount = 1;
		else 
		{
			if( bOrder )
				nCount = nPPos;
			else
				nCount = nPtNum-nPPos;
		}
		POSITION ps = NULL;
		for( int i=0;	i< nCount;	i++ )
		{
			ps = pos;
			CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPathPointList->GetAt( pos );
			if( pPPoint )
				delete pPPoint;
			pPathPointList->GetPrev( pos );
			pPathPointList->RemoveAt( ps );
		}
	}
	else
	{
		if( nPtNum <=2 )
			return TRUE;
		if( bOrder )
		{
			for( int i=0;	i<nPPos;	i++ )
			{
				CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
				pPathPt->SetPathpointPosition( pPtList[i] );
				pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
				pos = pPathPointList->InsertAfter( pos, pPathPt );
			}
		}
		else 
		{
			for( int i=nPtNum-1;	i>nPPos;	i-- )
			{
				CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
				pPathPt->SetPathpointPosition( pPtList[i] );
				pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
				pos = pPathPointList->InsertAfter( pos, pPathPt );
			}
		}
	}
	return TRUE;
}
///�����ֹwaypoint·����Ϣ
BOOL CGIS_MapBreadthNetwork::FullWPEndPathV2(
    CGIS_WayPoint *pWPTwo, 
    CGIS_NetworkVtx *pVtx, 
    CGIS_GeoMap *pMap, 
    CPtrList *pPathPointList,
    POSITION &pos )
{
	////β��waypoint�Ĵ���
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	BOOL bOrder = FALSE;
	CGIS_NetworkRut *pNRut = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) );
	if( pNRut == NULL )
		return FALSE;
	if( pVtx->GetVtxEdge() == 1 )
	{
		if( pVtx->GetVBreadthID() != pNRut->GetRBreadthID() )
		{
			CGIS_NetworkVtx *pVStart = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutStartVtxID() );
			if( pVStart )
			{
				if( fabs(pVStart->GetVtxPosition().dx-pVtx->GetVtxPosition().dx)<=FLOAT_ZERO && 
					fabs(pVStart->GetVtxPosition().dy-pVtx->GetVtxPosition().dy)<=FLOAT_ZERO )
					bOrder = TRUE;
				else
					bOrder = FALSE;
			}
		}
		else
		{
			if( pVtx->GetVtxID() == pNRut->GetRutStartVtxID() )
				bOrder = TRUE;
			else
				bOrder = FALSE;
		}
	}
	else
	{
		if( pVtx->GetVtxID() == pNRut->GetRutStartVtxID() )
			bOrder = TRUE;
		else
			bOrder = FALSE;
	}
	///���waypoint���·���ϵ�����
	ST_GEO_PTXY *pPtList = pWPTwo->GetWPRutPtList(eMapType);
	unsigned short nPtNum = pWPTwo->GetWPRutPtNum(eMapType);
	unsigned short nPPos = pWPTwo->GetWPProjPos(eMapType);
	if( nPtNum <= 2 )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(eMapType) );
		if( pWPTwo->GetWPType() == EN_WAYPOINTTYPE_DESTINATION )
			pPPMark->SetPathpointType( EN_PATHPOINTTYPE_END );
		pPPMark->SetWayPoint( pWPTwo );
		pMap->FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) ), pPPMark->GetVRutInfo() );
		pos = pPathPointList->AddTail( pPPMark );
		return TRUE;
	}
	if( bOrder )
	{
		for( int i=0;	i<=nPPos;	i++ )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	else
	{
		for( int i=nPtNum-1;	i>nPPos;	i-- )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
	pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(eMapType) );
	if( pWPTwo->GetWPType() == EN_WAYPOINTTYPE_DESTINATION )
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_END );
	pPPMark->SetWayPoint( pWPTwo );
	pMap->FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) ), pPPMark->GetVRutInfo() );
	pos = pPathPointList->AddTail( pPPMark );
	return TRUE;
}
///�����ֹwaypoint·����Ϣ
BOOL CGIS_MapBreadthNetwork::FullWPEndPathV3(
    CGIS_WayPoint *pWPTwo, 
    CGIS_NetworkVtx *pVtx, 
    CGIS_GeoMap *pMap, 
    CPtrList *pPathPointList,
    POSITION &pos )
{
	////β��waypoint�Ĵ���
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	BOOL bOrder = FALSE;
	CGIS_NetworkRut *pNRut = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) );
	if( pNRut == NULL )
		return FALSE;
	if( pVtx->GetVtxEdge() == 1 )
	{
		if( pVtx->GetVBreadthID() != pNRut->GetRBreadthID() )
		{
			CGIS_NetworkVtx *pVStart = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutStartVtxID() );
			if( pVStart )
			{
				if( fabs(pVStart->GetVtxPosition().dx-pVtx->GetVtxPosition().dx)<=FLOAT_ZERO && 
					fabs(pVStart->GetVtxPosition().dy-pVtx->GetVtxPosition().dy)<=FLOAT_ZERO )
					bOrder = TRUE;
				else
					bOrder = FALSE;
			}
		}
		else
		{
			if( pVtx->GetVtxID() == pNRut->GetRutStartVtxID() )
				bOrder = TRUE;
			else
				bOrder = FALSE;
		}
	}
	else
	{
		if( pVtx->GetVtxID() == pNRut->GetRutStartVtxID() )
			bOrder = TRUE;
		else
			bOrder = FALSE;
	}
	///���waypoint���·���ϵ�����
	ST_GEO_PTXY *pPtList = pWPTwo->GetWPRutPtList(eMapType);
	unsigned short nPtNum = pWPTwo->GetWPRutPtNum(eMapType);
	unsigned short nPPos = pWPTwo->GetWPProjPos(eMapType);
	if( nPtNum <= 2 )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(eMapType) );
		if( pWPTwo->GetWPType() == EN_WAYPOINTTYPE_DESTINATION )
			pPPMark->SetPathpointType( EN_PATHPOINTTYPE_END );
		pPPMark->SetWayPoint( pWPTwo );
		pMap->FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) ), pPPMark->GetVRutInfo() );
		pos = pPathPointList->AddTail( pPPMark );
		return TRUE;
	}
	if( bOrder )
	{
		for( int i=1;	i<nPPos;	i++ )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	else
	{
		for( int i=nPtNum-2;	i>nPPos;	i-- )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
	pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(eMapType) );
	if( pWPTwo->GetWPType() == EN_WAYPOINTTYPE_DESTINATION )
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_END );
	pPPMark->SetWayPoint( pWPTwo );
	pMap->FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) ), pPPMark->GetVRutInfo() );
	pos = pPathPointList->AddTail( pPPMark );
	return TRUE;
}


///���·�ߵ���waypoint��ͬһ·�ε����
BOOL CGIS_MapBreadthNetwork::FullWPOneAndWPTwoOnSameRut( CGIS_WayPoint *pWPOne, CGIS_WayPoint *pWPTwo, CGIS_GeoMap *pMap, CPtrList *pPathPointList, POSITION &pos )
{
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	if( pWPOne->GetWPType() == EN_WAYPOINTTYPE_ORIGIN )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPOne->GetWPProjPosition(eMapType) );
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_START );
		pPPMark->SetWayPoint( pWPOne );
		pMap->FullVRutInfo( GetNetworkRut( pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType) ), pPPMark->GetVRutInfo() );
		pos = pPathPointList->AddTail( pPPMark );
	}

    
	ST_GEO_PTXY *pPtList = pWPOne->GetWPRutPtList(eMapType);
	unsigned short nPOne = pWPOne->GetWPProjPos(eMapType);
	unsigned short nPTwo = pWPTwo->GetWPProjPos(eMapType);
	if( nPOne < nPTwo )
	{
		for( int i=nPOne+1; i<= nPTwo;	i++ )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	else
	{
		for( int i=nPOne; i> nPTwo;	i-- )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
	pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(eMapType) );
	if( pWPTwo->GetWPType() == EN_WAYPOINTTYPE_DESTINATION )
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_END );
	pPPMark->SetWayPoint( pWPTwo );
	pMap->FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) ), pPPMark->GetVRutInfo() );
	pos = pPathPointList->AddTail( pPPMark );
	return TRUE;
}
//��䳤�������ʱ��ʼ�˺���ֹ�˵ĺ�����Ϣ��
int CGIS_MapBreadthNetwork::FullPathHeadTailData( 
    CGIS_GeoMap *pMap, 
    CGIS_WayPoint *pWPOne, 
    CGIS_WayPoint *pWPTwo,
    CGIS_NetworkNode *pStartNode,
    CGIS_NetworkNode *pNodeEnd,
    CPtrList* pPathMid,
    CPtrList* pPathPointList)
{
    //////////////////////////////////////////////////////////////////////////   
    CSpaceMemoryLock aLock;
    TRACK_FUNCTION("FullPathHeadTailData");
    //////////////////////////////////////////////////////////////////////////
    TRY
    {

    //////////////////////////////////////////////////////////////////////////
    WATCH("FullPathHeadTailData");
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkRut* pHeadDetailRoadSectionInMidLeg = NULL;
    CGIS_NetworkRut* pTailDetailRoadSectionInMidLeg = NULL;
    CPtrList pathHead;
    CPtrList pathTail;
    CPtrList *pPHead = &pathHead; 
    CPtrList *pPTail = &pathTail;
    //�������·�ζ�дһ������
    //ͷ�����
    CPtrList RutList,VtxList;
    CGIS_NetworkNode *pNode = pStartNode;
	while( pNode )
	{
        //DUMPSTATE(pNode);
		ASSERT( pNode->m_pRut && pNode->m_pVtx );
		RutList.AddHead(pNode->m_pRut);
		VtxList.AddHead(pNode->m_pVtx);
		pNode = pNode->m_pParent;
	}
    //��״:   --->o--->o--->o......--->o
	if( RutList.GetCount() > 0 )
    {
        RutList.RemoveHead();
    }
    //ȥ��·�ε�ͷ���������״: o--->o--->o......--->o

    
#ifdef _ANALYSE_FULLPATHDATA
    {
        TRACE2("--Start:RutListSize[%d],VtxListSize[%d]\n",RutList.GetCount(),VtxList.GetCount());
        POSITION posOfRoadSection = RutList.GetHeadPosition();
        POSITION posOfVertex = VtxList.GetHeadPosition();
        while(posOfRoadSection)
        {
            CGIS_NetworkRut* pRoadSection   = (CGIS_NetworkRut*)RutList.GetNext(posOfRoadSection);
            CGIS_NetworkVtx* pVertex        = (CGIS_NetworkVtx*)VtxList.GetNext(posOfVertex);
            TRACE2("\tRut%.4d->@Vtx%.4d\n",pRoadSection->GetRutID(),pVertex->GetVtxID());
            TRACE_SLEEP_BY_TIMES(10);
        }
        TRACE0("--------\n");
        Sleep(100);
    }
#endif

    //////////////////////////////////////////////////////////////////////////
	///����·�ߵ�����
    {
        if(RutList.GetCount() >= 2)
        {
            POSITION psHead = RutList.GetHeadPosition();
            CGIS_NetworkRut *pRoadSection = (CGIS_NetworkRut*)RutList.GetNext( psHead );
            CGIS_NetworkRut *pRStart = NULL;
            if( psHead )
            {
                CGIS_NetworkRut *pRStart = (CGIS_NetworkRut*)RutList.GetAt( psHead );
                if( pRoadSection->GetRutID() == pRStart->GetRutID() 
                    && 
                    pRoadSection->GetRBreadthID() == pRStart->GetRBreadthID() )
                {
                    ASSERT(FALSE);
                    VtxList.RemoveHead();
                    RutList.RemoveHead();
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //��һ���ڵ�����ϸ��ٵĽڵ�Ļ�ֱ������������һ��.
    if(RutList.GetCount() == 0)
    {
        BOOL bRut = FALSE;
        POSITION pos = NULL;
        FullWPStartPathV2(pWPOne, (CGIS_NetworkVtx*)VtxList.GetHead(), NULL, pMap, bRut, pPHead, pos);
    }
    else
    {            
        ////�ж���ʼĿ���
        
        POSITION pos = NULL;
        CGIS_NetworkVtx* pVtx = (CGIS_NetworkVtx*)VtxList.GetHead( );
        CGIS_NetworkRut* pRoadSection = (CGIS_NetworkRut*)RutList.GetHead( );
        BOOL bRut = FALSE;
        FullWPStartPath( pWPOne, pVtx, pRoadSection, pMap, bRut, pPHead, pos );
        if( bRut )
        {
            VtxList.RemoveHead();
            RutList.RemoveHead();
        }
        
        EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
        CGIS_NetworkRut *pRoadSectionPrevious = GetNetworkRut(pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType));
        //List�������ŵ�.���->...->�ϸ��ٵ�
        /*
        ע��,���һ�������ϸ��ٵ�,Ӧ����ȫ��·������NODE������������
        */
        POSITION psRut = RutList.GetHeadPosition();
        POSITION psVtx = VtxList.GetHeadPosition();
        
        POSITION psVPos = VtxList.GetHeadPosition();
        pMap->GetBVRuts( &VtxList, psVPos, pWPOne->GetWPRutID(eMapType) );
        


        while( psRut )
        {
            //ע��, pRoadSectionPrevious-> pvtx ->pRoadSection
            pRoadSection = (CGIS_NetworkRut*)RutList.GetNext( psRut );
            pVtx = (CGIS_NetworkVtx*)VtxList.GetNext( psVtx );
            
            
            //������һͼ���Ŀռ�����
            if( pVtx->GetVtxEdge() == 1 )
                pMap->GetBVRuts( &VtxList, psVPos );
            
            //���ݿռ�������亽��
            pMap->GetRutData(
                pVtx,
                pRoadSectionPrevious,
                pRoadSection, 
                pWPTwo->GetWPRutNID(eMapType), 
                pWPTwo->GetWPRutID(eMapType), pPHead, pos
                );
            
            pRoadSectionPrevious = pRoadSection;
        }
        
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    /*
    ע��,��β�����ʱ,��һ�����¸��ٵĵ�,�������ȫ��·�����Ѿ���������NODE,�ص���ͼֻҪ����next·�ε�
    �ռ����ݾͿ�����.
    */
    //β�����

#ifdef _ANALYSE_FULLPATHDATA
    TRACE0("full tail data......\n");
#endif
    {
        RutList.RemoveAll();
        VtxList.RemoveAll();
        CGIS_NetworkNode* pNode = pNodeEnd;
        while( pNode )
        {
            //DUMPSTATE(pNode);
            if( pNode->m_pVtx->GetVtxEdge() == 1 )
            {
                CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork2( pNode->m_pVtx->GetVAbutBID() );
                ASSERT(pBMap);
                if( pBMap )
                {
                    CGIS_NetworkRut *pNRut = GetNetworkRut( pBMap->m_nBreadthID, pNode->m_pVtx->GetVAbutRID() );
                    //ȡ��·��
                    if( pNRut )
                    {
                        //��Ӱ��·�μ���·������
                        RutList.AddTail( pNode->m_pRut );
                        CGIS_NetworkVtx *pNSVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNRut->GetRutStartVtxID() );
                        CGIS_NetworkVtx *pNEVtx = GetNetworkVtx( pBMap->m_nBreadthID, pNRut->GetRutEndVtxID() );
                        ASSERT( pNSVtx && pNEVtx );

                        
                        if( pStartNode == NULL || pNodeEnd == NULL )
                        {
                            VtxList.AddTail( pNode->m_pVtx );
                            pNode = pNode->m_pParent;
                            continue;
                        }
                        

                        //��Ӱ�ӽڵ����ڵ�����
                        if( pNSVtx->GetVtxEdge() == 1 || pNEVtx->GetVtxEdge() == 1 )
                        {
                            if( pNSVtx->GetVtxEdge() == 1 &&
                                (fabs(pNSVtx->GetVtxPosition().dx-pNode->m_pVtx->GetVtxPosition().dx)<FLOAT_ZERO &&
                                fabs(pNSVtx->GetVtxPosition().dy-pNode->m_pVtx->GetVtxPosition().dy)<FLOAT_ZERO ) )
                                VtxList.AddTail( pNSVtx );
                            else if( pNEVtx->GetVtxEdge() == 1 &&
                                (fabs(pNEVtx->GetVtxPosition().dx-pNode->m_pVtx->GetVtxPosition().dx)<FLOAT_ZERO &&
                                fabs(pNEVtx->GetVtxPosition().dy-pNode->m_pVtx->GetVtxPosition().dy)<FLOAT_ZERO ) )
                                VtxList.AddTail( pNEVtx );
                            else 
                                ASSERT( FALSE );
                        }
                        else
                            ASSERT( FALSE );
                        pNode = pNode->m_pParent;
                        continue;
                    }
                }
            }//�Ǳ�Ե�ڵ��node����ִ������Ĵ���
            
            RutList.AddTail( pNode->m_pRut );
            VtxList.AddTail( pNode->m_pVtx );
            pNode = pNode->m_pParent;
        }

        //��״:   o<---......o<---o<---o<---

        RutList.RemoveTail( );
        //ȥ��·�ε�β���������״: o<---......o<---o<---o

#ifdef _ANALYSE_FULLPATHDATA
    {
        TRACE2("--End  :RutListSize[%d],VtxListSize[%d]\n",RutList.GetCount(),VtxList.GetCount());
        POSITION posOfRoadSection = RutList.GetHeadPosition();
        POSITION posOfVertex = VtxList.GetHeadPosition();
        while(posOfRoadSection)
        {
            CGIS_NetworkRut* pRoadSection   = (CGIS_NetworkRut*)RutList.GetNext(posOfRoadSection);
            CGIS_NetworkVtx* pVertex        = (CGIS_NetworkVtx*)VtxList.GetNext(posOfVertex);
            TRACE2("\tRut%.4d->@Vtx%.4d\n",pRoadSection->GetRutID(),pVertex->GetVtxID());
            TRACE_SLEEP_BY_TIMES(10);
        }
        TRACE0("--------\n");
        Sleep(100);
    }
#endif


        //////////////////////////////////////////////////////////////////////////
        ///����·�ߵ�����
        {
            if(RutList.GetCount() >= 2)
            {
                POSITION psHead = RutList.GetTailPosition();
                CGIS_NetworkRut* pRoadSection = (CGIS_NetworkRut*)RutList.GetPrev( psHead );
                if( psHead )
                {
                    CGIS_NetworkRut* pRStart = (CGIS_NetworkRut*)RutList.GetAt( psHead );
                    if( pRoadSection->GetRutID() == pRStart->GetRutID() &&
                        pRoadSection->GetRBreadthID() == pRStart->GetRBreadthID() )
                    {
                        //ASSERT(FALSE);
                        VtxList.RemoveTail();
                        RutList.RemoveTail();
                    }
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        
    //////////////////////////////////////////////////////////////////////////
    //��Ӵ˴��봦������յ�·�����ۻص����.
    {
        CGIS_NetworkRut* pRut = (CGIS_NetworkRut*)RutList.GetTail();
        if( pRut )
        {
            if( pRut->GetRutID() == pWPTwo->GetWPRutID(pMap->GetMapInfo()->GetMapType())
                &&
                pRut->GetRBreadthID() == pWPTwo->GetWPRutNID(pMap->GetMapInfo()->GetMapType()))
            {
                VtxList.RemoveTail();
                RutList.RemoveTail();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////

        POSITION psRut = RutList.GetHeadPosition();
        POSITION psVtx = VtxList.GetHeadPosition();
        POSITION psVPos = VtxList.GetHeadPosition();
        POSITION pos = NULL;
        
        CGIS_NetworkVtx* pVtx = NULL;
        CGIS_NetworkRut* pRoadSection = NULL;
        CGIS_NetworkRut *pRoadSectionPrevious = NULL;



        EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
        pMap->GetBVRuts( &VtxList, psVPos, pWPOne->GetWPRutID(eMapType) );

        //��һ����ֻ��ռ�����,������NODE.
        /*��ΪNODE�������洴������.�������治Ҫ�ٴ���.���ǿռ����ݱ������������.
        */
        if(psRut)
        {
            pRoadSection = (CGIS_NetworkRut*)RutList.GetNext( psRut );
            pVtx = (CGIS_NetworkVtx*)VtxList.GetNext( psVtx );
            pMap->GetRutDataV2(
                pVtx, pRoadSectionPrevious, pRoadSection, 
                pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType), 
                pPTail, pos );
            pRoadSectionPrevious = pRoadSection;
        }
        
        while( psRut )
        {
            pRoadSection = (CGIS_NetworkRut*)RutList.GetNext( psRut );
            pVtx = (CGIS_NetworkVtx*)VtxList.GetNext( psVtx );
            if( pVtx->GetVtxEdge() == 1 )
                pMap->GetBVRuts( &VtxList, psVPos );
            pMap->GetRutData(
                pVtx, pRoadSectionPrevious, pRoadSection, 
                pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType), 
                pPTail, pos );
            pRoadSectionPrevious = pRoadSection;
        }

        pVtx = (CGIS_NetworkVtx*)VtxList.GetNext( psVtx );
        if(RutList.GetCount()>0)
        {
            
            pMap->FullEVNode( 
                pWPTwo, 
                pVtx,
                pRoadSectionPrevious, 
                GetNetworkRut(pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType)),
                pPTail,
                pos );
        }
        FullWPEndPathV3( pWPTwo, pVtx, pMap, pPTail, pos );
    }
    
    //////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
            {
                //////////////////////////////////////////////////////////////////////////
                //�������¸��ٵĵ�û���ظ����
                ST_GEO_PTXY pt[2];
                pt[0] = ((CGIS_Pathpoint*) pPathMid->GetHead())->GetPathpointPosition();
                pt[1] = ((CGIS_Pathpoint*) pPathMid->GetTail())->GetPathpointPosition();
                
                
                //                TRACE2("�ϸ�������:%f,%f\n",pt[0].dx,pt[0].dy);
                //                TRACE2("�¸�������:%f,%f\n",pt[1].dx,pt[1].dy);
                
            }
#endif
            
#ifdef _DEBUG
            {
                //////////////////////////////////////////////////////////////////////////
                //�������¸��ٵĵ�û���ظ����
                ST_GEO_PTXY pt[2];
                pt[0] = ((CGIS_Pathpoint*) pPathMid->GetHead())->GetPathpointPosition();
                pt[1] = ((CGIS_Pathpoint*) pathHead.GetTail())->GetPathpointPosition();
                //ASSERT( fabs(pt[0].dx - pt[1].dx) > FLOAT_DETA_XY || fabs(pt[0].dy - pt[1].dy) > FLOAT_DETA_XY);
                pt[0] = ((CGIS_Pathpoint*) pPathMid->GetTail())->GetPathpointPosition();
                pt[1] = ((CGIS_Pathpoint*) pathTail.GetHead())->GetPathpointPosition();
                //ASSERT( fabs(pt[0].dx - pt[1].dx) > FLOAT_DETA_XY || fabs(pt[0].dy - pt[1].dy) > FLOAT_DETA_XY);
                if(0)
                {
                    TRACE2("\t%f,%f\n",pt[0].dx,pt[0].dy);
                    TRACE2("\t%f,%f\n",pt[1].dx,pt[1].dy);
                }
            }
#endif
            
{
    
    
    CGIS_Pathpoint *pPPoint = NULL;
    POSITION pos = pathHead.GetHeadPosition();
    while(pos)
    {
        pPPoint = (CGIS_Pathpoint*)pathHead.GetNext(pos);
        if( pPPoint ==NULL )
            continue;
        pPathPointList->AddTail(pPPoint);
    }
    pos = pPathMid->GetHeadPosition();
    while(pos)
    {
        pPPoint = (CGIS_Pathpoint*)pPathMid->GetNext(pos);
        if( pPPoint ==NULL )
            continue;
        pPathPointList->AddTail(pPPoint);
    }
    pos = pathTail.GetHeadPosition();
    while(pos)
    {
        pPPoint = (CGIS_Pathpoint*)pathTail.GetNext(pos);
        if( pPPoint ==NULL )
            continue;
        pPathPointList->AddTail(pPPoint);
    }
}
    }
    CATCH(CException,e)
    {
#ifdef _WIN32_WCE
#ifdef _AFX_OLD_EXCEPTIONS
        aLock.Unlock();
#endif
#endif
        
        THROW_LAST();
    }
    END_CATCH
        
return 1;
}

///���·�ߵ���waypoint��ͬһ�ڵ�����
BOOL CGIS_MapBreadthNetwork::FullWPOneAndWPTwoOnSameVtx( 
    CGIS_WayPoint *pWPOne, 
    CGIS_WayPoint *pWPTwo, 
    CGIS_GeoMap *pMap, 
    CPtrList *pPathPointList, 
    POSITION &pos )
{
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	CGIS_NetworkVtx* pVtx                   = GetNetworkVtx( pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPVtxID(eMapType));
	CGIS_NetworkRut* pRoadSectionPrevious   = GetNetworkRut( pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType) );
	CGIS_NetworkRut* pRoadSectionNext       = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) );
	ASSERT( pRoadSectionPrevious && pRoadSectionNext && pVtx );
	if( !pRoadSectionPrevious  || !pRoadSectionNext || !pVtx )
		return FALSE;
    //////////////////////////////////////////////////////////////////////////


	if( pWPOne->GetWPType() == EN_WAYPOINTTYPE_ORIGIN )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPOne->GetWPProjPosition(eMapType) );
		pPPMark->SetWayPoint( pWPOne );
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_START );
		pMap->FullVRutInfo( pRoadSectionPrevious, pPPMark->GetVRutInfo() );
		pos = pPathPointList->AddTail( pPPMark );
	}
	BOOL bOrder = FALSE;
	if( pRoadSectionPrevious->GetRutStartVtxID() == pWPOne->GetWPVtxID(eMapType) )
		bOrder = FALSE;
	else
		bOrder = TRUE;
	///���waypoint���·���ϵ�����
	ST_GEO_PTXY *pPtList = pWPOne->GetWPRutPtList(eMapType);
	unsigned short nPtNum = pWPOne->GetWPRutPtNum(eMapType);
	unsigned short nPPos = pWPOne->GetWPProjPos(eMapType);
	if( nPtNum > 2 )
	{
		if( bOrder )
		{
			for( int i=nPPos+1;	i<nPtNum-1;	i++ )
			{
				CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
				pPathPt->SetPathpointPosition( pPtList[i] );
				pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
				pos = pPathPointList->InsertAfter( pos, pPathPt );
			}
		}
		else
		{
			for( int i=nPPos;	i>0;	i-- )
			{
				CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
				pPathPt->SetPathpointPosition( pPtList[i] );
				pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
				pos = pPathPointList->InsertAfter( pos, pPathPt );
			}
		}
	}
	if( pVtx )
	{
		CPtrList VList;
		VList.AddTail( pVtx );
		POSITION psVList = VList.GetHeadPosition();
		pMap->GetBVRuts( &VList, psVList, pWPOne->GetWPRutID(eMapType) );
		pMap->FullEVNode( pWPTwo, pVtx, pRoadSectionPrevious, pRoadSectionNext, pPathPointList, pos );
	}
	////β��waypoint�Ĵ���
	if( pRoadSectionNext->GetRutStartVtxID() == pWPTwo->GetWPVtxID(eMapType) )
		bOrder = TRUE;
	else
		bOrder = FALSE;
	///���waypoint���·���ϵ�����
	pPtList = pWPTwo->GetWPRutPtList(eMapType);
	nPtNum = pWPTwo->GetWPRutPtNum(eMapType);
	nPPos = pWPTwo->GetWPProjPos(eMapType);
	if( nPtNum <= 2 )
	{
		CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
		pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(eMapType) );
		if( pWPTwo->GetWPType() == EN_WAYPOINTTYPE_DESTINATION )
			pPPMark->SetPathpointType( EN_PATHPOINTTYPE_END );
		pPPMark->SetWayPoint( pWPTwo );
		pMap->FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) ), pPPMark->GetVRutInfo() );
		pos = pPathPointList->AddTail( pPPMark );
		return TRUE;
	}
	if( bOrder )
	{
		for( int i=0;	i<=nPPos;	i++ )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	else
	{
		for( int i=nPtNum-1;	i>nPPos;	i-- )
		{
			CGIS_Pathpoint *pPathPt = new CGIS_Pathpoint;
			pPathPt->SetPathpointPosition( pPtList[i] );
			pPathPt->SetPathpointType( EN_PATHPOINTTYPE_INFLEXION );
			pos = pPathPointList->InsertAfter( pos, pPathPt );
		}
	}
	CGIS_PathpointMark *pPPMark = new CGIS_PathpointMark;
	pPPMark->SetPathpointPosition( pWPTwo->GetWPProjPosition(eMapType) );
	if( pWPTwo->GetWPType() == EN_WAYPOINTTYPE_DESTINATION )
		pPPMark->SetPathpointType( EN_PATHPOINTTYPE_END );
	pPPMark->SetWayPoint( pWPTwo );
	pMap->FullVRutInfo( GetNetworkRut( pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType) ), pPPMark->GetVRutInfo() );
	pos = pPathPointList->AddTail( pPPMark );
	return TRUE;
}


/**
 * @brief ���·��pathleg����
 *
 * @param 
 * @return
*/
BOOL CGIS_MapBreadthNetwork::FullPathLegData(
    CGIS_WayPoint *pWPOne, 
    CGIS_WayPoint *pWPTwo,
    CPtrList *pRutList,
    CPtrList *pVtxList,
    CGIS_GeoMap *pMap, 
    CPtrList *pPathPointList )
{
    //////////////////////////////////////////////////////////////////////////
    //��դ��
    ASSERT(pWPOne);
    ASSERT(pWPTwo);
    ASSERT(pRutList);
    ASSERT(pVtxList);
    ASSERT(pMap);
    ASSERT(pPathPointList);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////   
    CSpaceMemoryLock aLock;
    TRACK_FUNCTION("FullPathLegData");
    //////////////////////////////////////////////////////////////////////////
    TRY
    {
    //////////////////////////////////////////////////////////////////////////
    WATCH("FullPathLegData");
    //////////////////////////////////////////////////////////////////////////
    
#ifdef _ANALYSE_PATH_TURN
        DUMPRUTLIST(pRutList);
        DUMPVTXLIST(pVtxList);
#endif    
    

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
	//��Ŀ�������·��Ϊͬһ��·��
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
	POSITION pos = pPathPointList->GetTailPosition();
	if(
        pWPOne->GetWPRutID(eMapType) == pWPTwo->GetWPRutID(eMapType)
        && 
		pWPOne->GetWPRutNID(eMapType) == pWPTwo->GetWPRutNID(eMapType)
      )
	{
        CGIS_NetworkRut *pRut = GetNetworkRut(pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType));
        ASSERT(pRut);
        
        if( pRut)
        {
            switch(pRut->GetRutDirection())
            {
            case 0://˫��ͨ��
                {
                    return FullWPOneAndWPTwoOnSameRut( pWPOne, pWPTwo, pMap, pPathPointList, pos );
                }
                break;
            case 1://����ͨ��
                {
                    if( pWPOne->GetWPProjPos(eMapType) == pWPTwo->GetWPProjPos(eMapType) )
                    {
                        double dbDis1 = CalcDist(pWPOne->GetWPProjPosition(eMapType).dx, pWPOne->GetWPProjPosition(eMapType).dy,
                            pWPOne->GetWPRutPtList(eMapType)[pWPOne->GetWPRutPtNum(eMapType)-1].dx, pWPOne->GetWPRutPtList(eMapType)[pWPOne->GetWPRutPtNum(eMapType)-1].dy );
                        double dbDis2 = CalcDist(pWPTwo->GetWPProjPosition(eMapType).dx, pWPTwo->GetWPProjPosition(eMapType).dy,
                            pWPOne->GetWPRutPtList(eMapType)[pWPOne->GetWPRutPtNum(eMapType)-1].dx, pWPOne->GetWPRutPtList(eMapType)[pWPOne->GetWPRutPtNum(eMapType)-1].dy );
                        //˳�У���ֻ�����ͬһ·���ڿռ�����
                        if( dbDis1 > dbDis2 )
                            return FullWPOneAndWPTwoOnSameRut( pWPOne, pWPTwo, pMap, pPathPointList, pos );
                    }
                    else if( pWPOne->GetWPProjPos(eMapType) < pWPTwo->GetWPProjPos(eMapType) )
                        return FullWPOneAndWPTwoOnSameRut( pWPOne, pWPTwo, pMap, pPathPointList, pos );
                }
                break;//����ͨ��
            case 2:
                {
                    if( pWPOne->GetWPProjPos(eMapType) == pWPTwo->GetWPProjPos(eMapType) )
                    {
                        double dbDis1 = CalcDist(pWPOne->GetWPProjPosition(eMapType).dx, pWPOne->GetWPProjPosition(eMapType).dy,
                            pWPOne->GetWPRutPtList(eMapType)[0].dx, pWPOne->GetWPRutPtList(eMapType)[0].dy );
                        double dbDis2 = CalcDist(pWPTwo->GetWPProjPosition(eMapType).dx, pWPTwo->GetWPProjPosition(eMapType).dy,
                            pWPOne->GetWPRutPtList(eMapType)[0].dx, pWPOne->GetWPRutPtList(eMapType)[0].dy );
                        if( dbDis1 > dbDis2 )
                            return FullWPOneAndWPTwoOnSameRut( pWPOne, pWPTwo, pMap, pPathPointList, pos );
                    }
                    else if( pWPOne->GetWPProjPos(eMapType) > pWPTwo->GetWPProjPos(eMapType) )
                        return FullWPOneAndWPTwoOnSameRut( pWPOne, pWPTwo, pMap, pPathPointList, pos );
                }
                break;
            default:
                break;                
            }
        }
			
	}


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
  	//��Ŀ��������ڵ�·����(��ѽڵ���ͬһ��)�Ĵ���
	if( pRutList->GetCount() == 0 )
    {
        return FullWPOneAndWPTwoOnSameVtx( pWPOne, pWPTwo, pMap, pPathPointList, pos );
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //��������,�����յ㲻��ͬһ·����,Ҳ��������·����
	///����·�ߵ�����
	ASSERT( pRutList->GetCount() >= 1 );


    //-------------------------------------------------------------
    //��δ������������ͷ��/β���۷������
	CGIS_NetworkVtx *pVtx = NULL;
	CGIS_NetworkRut *pRut = NULL;
	POSITION psHead = pRutList->GetHeadPosition();
	pRut = (CGIS_NetworkRut*)pRutList->GetNext( psHead );
	CGIS_NetworkRut *pRStart = NULL;
	if( psHead )
	{
		CGIS_NetworkRut *pRStart = (CGIS_NetworkRut*)pRutList->GetAt( psHead );
		if( pRut->GetRutID() == pRStart->GetRutID() && 
			pRut->GetRBreadthID() == pRStart->GetRBreadthID() )
		{
            TRACE0("���ʱ��㴦�����ظ�·��\n");
			pVtxList->RemoveHead();
			pRutList->RemoveHead();
		}
	}
	psHead = pRutList->GetTailPosition();
	pRut = (CGIS_NetworkRut*)pRutList->GetPrev( psHead );
	if( psHead )
	{
		pRStart = (CGIS_NetworkRut*)pRutList->GetAt( psHead );
		if( pRut->GetRutID() == pRStart->GetRutID() &&
			pRut->GetRBreadthID() == pRStart->GetRBreadthID() )
		{
            TRACE0("���ʱ�յ㴦�����ظ�·��\n");
			pVtxList->RemoveTail();
			pRutList->RemoveTail();
		}
	}
    //-------------------------------------------------------------
    

    //////////////////////////////////////////////////////////////////////////
	////�ж���ʼĿ���
	pVtx = (CGIS_NetworkVtx*)pVtxList->GetHead( );
	pRut = (CGIS_NetworkRut*)pRutList->GetHead( );


    ///�����ʼwaypoint·����Ϣ
	BOOL bRut = FALSE;
	FullWPStartPath( pWPOne, pVtx, pRut, pMap, bRut, pPathPointList, pos );//�����һ���������������ΪMARK,��������������ʼ·�ε�ͶӰ�㵽��ʼ�ڵ���һ��.
	if( bRut )//��һ��·������ʼ·�εĻ����������ۻ�,����Ҫ�ѵ�һ���ڵ�͵�һ��·��ȥ��
	{
		pVtxList->RemoveHead();
		pRutList->RemoveHead();
	}

    
	CGIS_NetworkRut *pRoadSectionPrevious = GetNetworkRut(pWPOne->GetWPRutNID(eMapType),pWPOne->GetWPRutID(eMapType));
	POSITION psVPos = pVtxList->GetHeadPosition();
	//���������ۻغ��ٿ�,���Ŀ�ĵؾ�������·����,�����ֱ����䴦��
	if( pRutList->IsEmpty() )
	{
		pMap->GetBVRuts( pVtxList, psVPos, pWPOne->GetWPRutID(eMapType) );
		pVtx = (CGIS_NetworkVtx*)pVtxList->GetHead( );
		pMap->FullEVNode( pWPTwo, pVtx, pRoadSectionPrevious, GetNetworkRut(pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType)),
							pPathPointList, pos );

        return FullWPEndPathV2( pWPTwo, pVtx, pMap, pPathPointList, pos );
	}

    //����β���ۻ�
	pRut = (CGIS_NetworkRut*)pRutList->GetTail();
	if( pRut )
	{
		if( pRut->GetRutID() == pWPTwo->GetWPRutID(eMapType) 
            && 
            pRut->GetRBreadthID() == pWPTwo->GetWPRutNID(eMapType) )
		{
			pVtxList->RemoveTail();
			pRutList->RemoveTail();
		}
	}


    //////////////////////////////////////////////////////////////////////////
    //����Ĵ���ȥ�������ж����·��
    //////////////////////////////////////////////////////////////////////////
    //

    
    CPtrList aNodeList;
    POSITION posOfNodeList = aNodeList.GetHeadPosition();
    POSITION psRut = pRutList->GetHeadPosition();
	POSITION psVtx = pVtxList->GetHeadPosition();
	psVPos = pVtxList->GetHeadPosition();
	pMap->GetBVRuts( pVtxList, psVPos, pWPOne->GetWPRutID(eMapType) );
	while( psRut )
	{
		pRut = (CGIS_NetworkRut*)pRutList->GetNext( psRut );
		pVtx = (CGIS_NetworkVtx*)pVtxList->GetNext( psVtx );
        
        //------------------
        //���ؿռ����ݱ���
		if( pVtx->GetVtxEdge() == 1 )
        {
            //ȡ������ͬһͼ���ڵ����·�εļ���
			pMap->GetBVRuts( pVtxList, psVPos );
        }
        //��ȡ���ݵ�������.
		pMap->GetRutDataV3( 
            pVtx, pRoadSectionPrevious, pRut, 
            pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType), 
            pPathPointList, pos ,&aNodeList,posOfNodeList);
        //------------------

        pRoadSectionPrevious = pRut;
	}
    
    CGIS_NetworkPath* pPath = m_pNetworkRoute->GetNetworkPath();
    ASSERT(pPath);
    pPath->GetNodeList()->AddTail(&aNodeList);
	pVtx = (CGIS_NetworkVtx*)pVtxList->GetTail();
	//�յ�VTX��ͼ�������
	if( pVtx->GetVtxEdge() == 1 )
	{
		BOOL bFind = FALSE;
		CGIS_BreadthMap *pBMap = GetBreadthMapOfNetwork( pVtx->GetVBreadthID() );
		if( pBMap )
		{
			if( pVtx->GetVBreadthID() != pWPTwo->GetWPRutNID(eMapType) )
			{
				CGIS_NetworkRut *pRTemp = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pVtx->GetVAbutRID() );
				if( pRTemp )
				{
					CGIS_NetworkVtx *pVTemp = GetNetworkVtx( pWPTwo->GetWPRutNID(eMapType), pRTemp->GetRutStartVtxID() );
					if( pVTemp )
					{
						if( pVTemp->GetVtxEdge() == 1 )
						{
							if( pVTemp->GetVAbutBID() == pBMap->m_nBMID && pVTemp->GetVAbutRID() == pRoadSectionPrevious->GetRutID())
							{
								pVtx = pVTemp;
								bFind = TRUE;
							}
						}
					}
					if( !bFind )
					{
						pVTemp = GetNetworkVtx( pWPTwo->GetWPRutNID(eMapType), pRTemp->GetRutEndVtxID() );
						if( pVTemp )
						{
							if( pVTemp->GetVtxEdge() == 1 )
							{
								if( pVTemp->GetVAbutBID() == pBMap->m_nBMID && pVTemp->GetVAbutRID() == pRoadSectionPrevious->GetRutID())
								{
									pVtx = pVTemp;
									bFind = TRUE;
								}
							}
						}
					}
				}
			}
		}
	}
	//�������һ���ڵ�Ŀռ��������
	pMap->FullEVNode( pWPTwo, pVtx, pRoadSectionPrevious, GetNetworkRut(pWPTwo->GetWPRutNID(eMapType),pWPTwo->GetWPRutID(eMapType)),
						pPathPointList, pos );
	
	FullWPEndPathV2( pWPTwo, pVtx, pMap, pPathPointList, pos );

    }
    CATCH(CException,e)
    {
#ifdef _WIN32_WCE
#ifdef _AFX_OLD_EXCEPTIONS
        aLock.Unlock();
#endif
#endif
        THROW_LAST();
    }
    END_CATCH



	return TRUE;
}

int IfBetter(CGIS_NetworkNode* a,CGIS_NetworkNode* b)
{

    return b->m_f > a->m_f ;
}


/**
 * @brief �ҳ�С�����ڵ��㷨
 *
 * @param 
 * @return BOOL-�Ƿ��ҵ�С���ĳ���
*/
/**
 * @brief ��С�����㷨
*/
BOOL CGIS_MapBreadthNetwork::SearchExitOfResidentialArea( 
        CPtrList *              pOpenList,      //Ӧ���Ѿ���һ������չ�Ľڵ�
        CPtrList *              pCloseList,
        CGIS_NetworkVtx *       pDestVertex,       //Ŀ��ڵ�
        CGIS_NetworkPathInfo *  pPathInfo,  
        BYTE                    nRLevel,        //���ڴ˵ȼ���·�β���չ
        BOOL                    bHead,          //�������һ��Ƿ�������
        BOOL                    &bFind,         //[out]�Ƿ��ҵ�Ŀ���
        stuTacticOfSearchPathHead* pTactic)     //����
{
    ASSERT(pOpenList);
    ASSERT(pCloseList);
  
    //////////////////////////////////////////////////////////////////////////
	int nResult = 0;
	ASSERT( pOpenList->GetCount() > 0 );

    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup = NULL;
	CGIS_NetworkNode *pNode = (CGIS_NetworkNode*)pOpenList->GetHead();
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
	CGIS_NetworkNode *pNextNode = pNode;
	CGIS_NetworkNode *pNTem = NULL;
	BOOL bSearchStartNodeSign = TRUE;
    
    //////////////////////////////////////////////////////////////////////////
    BOOL bFindAppropriateNode = FALSE;
    if(pNextNode->m_nLRNum >= 1)
    {
        bFindAppropriateNode = TRUE; 
    }

	while( 
        (pNode->m_pRut->GetRutLevel() < nRLevel ||  //��ǰnode֮·�εĹ��ܼ���С���޶��������.
        pNode->m_pRut->GetRutType() == 4 ||         //��ǰ·����
        pNode->m_pRut->GetRutType() == 1 ||
		pNode->m_pVtx->GetVtxEdge() == 1)
        && 
        !bFindAppropriateNode
        && 
        !bFind )
	{
        
        if( m_byErrCode )
        {
            switch(m_byErrCode)
            {
            case 1:
                nResult = CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
                break;
            case 2:
                nResult = CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
                break;
            case 3:
                nResult = -7;
                break;
            default:
                ASSERT(FALSE);
                break;
            }
        
			break;
		}
        
        if(pTactic->EnableLimitInfo)
        {          
            if( pNode )
            {
                GET_LIMIT_INFO_V3(pNode,bHead,pLimitInfoGroup);
            }
        }

        //��open���Ƴ�һ���ڵ�ȥ����.
        BOOL bRtn = CalculateOptimalPathRut(
            pOpenList,
            pCloseList, 
            pDestVertex, 
            pDestVertex,
            pPathInfo,
            pLimitInfoGroup,
            0,
            bHead,
            bSearchStartNodeSign );

		if( bRtn)
        {
            ASSERT(!bFind);
			bFind = TRUE;
        }


        //////////////////
		bSearchStartNodeSign = FALSE;
		if( !pOpenList->IsEmpty() )
		{
            //open���յĻ����õ�ǰ���ŵ�
            //��һ���ڵ�Ϊ��ǰ���ŵ�ĸ�NODE
            //
			pNode = (CGIS_NetworkNode*)pOpenList->GetHead();
			if( pNode->m_pParent )
				pNextNode = pNode->m_pParent;
			else
				pNextNode = pNode;
            
            if(pNextNode->m_nLRNum >= 1)
            {
                bFindAppropriateNode = TRUE;                    
            }
            
        }
        else 
        {
			break;
        }
	}


	POSITION ps = pOpenList->GetHeadPosition();
	while( ps )
	{
		pNode = (CGIS_NetworkNode*)pOpenList->GetNext( ps );
		if( pNode )
		{
			pNode->m_pVtx->GetVtxSInfo()->posOHead = pNode->m_pVtx->GetVtxSInfo()->posOEnd = NULL;
			pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 0;
		}
	}
	ps = pCloseList->GetHeadPosition();
	while( ps )
	{
		pNode = (CGIS_NetworkNode*)pCloseList->GetNext( ps );
		if( pNode )
		{
			pNode->m_pVtx->GetVtxSInfo()->posOHead = pNode->m_pVtx->GetVtxSInfo()->posOEnd = NULL;
			pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 0;
		}
	}

	if( bFindAppropriateNode)
    {
        nResult = 1;
    }
	return nResult;
}


void CGIS_MapBreadthNetwork::ResetNodeStateInNodeList   ( CPtrList *pNodeList )
{
	CGIS_NetworkNode *pNode = NULL;
	POSITION pos = pNodeList->GetHeadPosition();
	while( pos )
	{
		pNode = (CGIS_NetworkNode*)pNodeList->GetNext( pos );
        ASSERT(pNode);
		if( pNode )
		{
            //pNode->m_nLRNum = 0;
			pNode->m_pVtx->ResetSortInfo();
		}
	}
	pNodeList->RemoveAll();
}

void CGIS_MapBreadthNetwork::ClearNWNList( CPtrList *pNList )
{
	CGIS_NetworkNode *pNode = NULL;
	POSITION pos = pNList->GetHeadPosition();
	while( pos )
	{
		pNode = (CGIS_NetworkNode*)pNList->GetNext( pos );
		if( pNode )
		{
			pNode->m_pVtx->GetVtxSInfo()->posOHead = pNode->m_pVtx->GetVtxSInfo()->posOEnd = NULL;
			if( pNode->m_pVtx->GetVtxEdge() == 1 )
				pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;//��Ϊ1��Ϊ�˱�����Щ�ڵ㲻�ں�߱�ɾ��
			else
		        pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 0;

            pNode->m_pVtx->ResetSortInfo();
			delete pNode; 
			pNode = NULL;
		}
	}
	pNList->RemoveAll();
}
/**
 * @brief ȡ��һ��NODE�������Ϣ
 *
 * @param 
 * @return
*/
void CGIS_MapBreadthNetwork::GetNWNodeRVInfo(
    CGIS_NetworkNode *pNode,
    CPtrList *pCloseNodeList,
    CPtrList *pRoadSectionList,
    CPtrList *pVertexList )
{
	CGIS_NetworkRut *pRut = NULL;
	CGIS_NetworkVtx *pVtx = NULL;
	CGIS_NetworkVtx *pVTem = NULL;
	CGIS_NetworkNode *pNTem = pNode;
 
    short nVRNum = 0;
	BOOL bFFlag = TRUE;
	POSITION pos = NULL;
	while( pNTem )
	{
        //-------------------------------------------------------------
        //�������ڵ���CLOSE����,�ȴӸñ�������
		pos = pCloseNodeList->Find( pNTem );
		if( pos )
			pCloseNodeList->RemoveAt( pos );
        //-------------------------------------------------------------
		pRut = pNTem->m_pRut;
		pVtx = pNTem->m_pVtx;
		if( pVtx && pRut )
		{
			pRoadSectionList->AddTail( pRut );
			pVTem = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutStartVtxID() );
			if( pVTem )
			{
				if( pVTem != pVtx && pVTem->GetVtxEdge() == 1 )
					pVertexList->AddTail( pVTem );
			}
			pVTem = GetNetworkVtx( pRut->GetRBreadthID(), pRut->GetRutEndVtxID() );
			if( pVTem )
			{
				if( pVTem != pVtx && pVTem->GetVtxEdge() == 1 )
					pVertexList->AddTail( pVTem );
			}
			pVertexList->AddTail( pVtx );
            
			nVRNum = pVtx->GetVRutNum();
			ASSERT( nVRNum > 0 );
			CGIS_NetworkRut *pNRut = NULL;
			BYTE byRAType = 1;
			for( int i=0;	i<nVRNum;	i++ )
			{
				if( pVtx->GetVRutList()[i] == pRut->GetRutID() )
					continue;
				else
				{
					pNRut = GetNetworkRut( pVtx->GetVBreadthID(), pVtx->GetVRutList()[i] );
					if( pNRut )
					{
						if( pNRut->GetRutType() == 4 )
							byRAType = 2;
						else if( pNRut->GetRutType() == 0 )
							byRAType = 3;
					}
				}
			}
			pNTem = pNTem->m_pParent;
			if( pNTem )
			{
				if( pNTem->m_pRut && pNTem->m_pVtx )
				{
					if( bFFlag )
						FullPathNodeRuts( pRoadSectionList, pVertexList, pVtx, pRut, pNTem->m_pRut, byRAType );
					if( pNTem->m_pRut->GetRutType() == 0 || pNTem->m_pRut->GetRutType() == 4 )
						bFFlag = FALSE;
					else 
						bFFlag = TRUE;
				}
			}
		}
		else
			ASSERT( FALSE );
	}
}
void CGIS_MapBreadthNetwork::ClearNWHead( 
    CGIS_NetworkNode *pStartNode, 
    CGIS_NetworkNode *pEndNode,
    CPtrList *pSONList, 
    CPtrList *pSCNList, 
    CPtrList *pEONList,
    CPtrList *pECNList )
{
	CPtrList RList, VList;
	GetNWNodeRVInfo( pStartNode, pSCNList, &RList, &VList );
	GetNWNodeRVInfo( pEndNode, pECNList, &RList, &VList );
	ClearNWNList( pSONList );
	ClearNWNList( pSCNList );
	ClearNWNList( pEONList );
	ClearNWNList( pECNList );
	CGIS_NetworkRut *pNRut = NULL;
	CGIS_NetworkVtx *pNVtx = NULL;
	CGIS_NetworkVtx *pNVTem = NULL;
	CGIS_BreadthMap *pBMap = NULL;
	int i;
	for( i=0;	i<m_pMap->GetBreadthMapNum();	i++ )
	{
		pBMap = m_pMap->GetBreadthMapByID2( i );
		if( pBMap )
		{
			if( pBMap->m_nLoadCounter == 0 )
				continue;
			for( int j =0;	j<m_nRutNum[pBMap->m_nBreadthID]; j++ )
			{
				pNRut = GetNetworkRut(pBMap->m_nBreadthID,j);
				if( pNRut )
				{
					if( pNRut->GetRutLevel() >= m_byRLevel )
					{
						RList.AddTail( pNRut );
						pNVTem = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutStartVtxID() );
						if( pNVTem )
						{
							VList.AddTail( pNVTem );
						}
						else
                        {
							TRACE0("�ڵ���ɾ��!(ԭ��:��������ɾ����·�����ô˽��)\n");
                        }
						pNVTem = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutEndVtxID() );
						if( pNVTem )
						{
							VList.AddTail( pNVTem );
						}
						else
                        {
							TRACE0("�ڵ���ɾ��!(ԭ��:��������ɾ����·�����ô˽��)\n");
                        }
					}
                    else
                    {
                        BOOL bReverse = FALSE;
						pNVTem = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutStartVtxID() );
                        if(pNVTem && pNVTem->GetVtxEdge() != 1)
                        {
                            int nCount = pNVTem->GetVRutNum();
                            for(int i = 0; i < nCount; i++)
                            {
                                CGIS_NetworkRut* pRoadSection = GetNetworkRut(pBMap->m_nBreadthID,pNVTem->GetVRutList()[i]);
                                if(pRoadSection)
                                {
                                    if(pRoadSection->GetRutType() == 0)
                                    {
                                        bReverse = TRUE;
                                        break;
                                    }
                                }
                            }
                        }
						pNVTem = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutEndVtxID() );
                        if(pNVTem && pNVTem->GetVtxEdge() != 1)
                        {
                            int nCount = pNVTem->GetVRutNum();
                            for(int i = 0; i < nCount; i++)
                            {
                                CGIS_NetworkRut* pRoadSection = GetNetworkRut(pBMap->m_nBreadthID,pNVTem->GetVRutList()[i]);
                                if(pRoadSection)
                                {
                                    if(pRoadSection->GetRutType() == 0)
                                    {
                                        bReverse = TRUE;
                                        break;
                                    }
                                }
                            }
                        }
                        if(bReverse)
                        {
                            RList.AddTail(pNRut);
                            pNVTem = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutStartVtxID() );
                            if( pNVTem )
                            {
                                VList.AddTail( pNVTem );
                            }
                            else
                            {
                                TRACE0("�ڵ���ɾ��!(ԭ��:��������ɾ����·�����ô˽��)\n");
                            }
                            pNVTem = GetNetworkVtx( pNRut->GetRBreadthID(), pNRut->GetRutEndVtxID() );
                            if( pNVTem )
                            {
                                VList.AddTail( pNVTem );
                            }
                            else
                            {
                                TRACE0("�ڵ���ɾ��!(ԭ��:��������ɾ����·�����ô˽��)\n");
                            }
                        }
                    }
				}
			}
		}
	}

    //////////////////////////////////////////////////////////////////////////
    //��·�κͽڵ��Ӧ����Ա��ں��ɾ��
	POSITION pos = RList.GetHeadPosition();
	while( pos )
	{
		pNRut = (CGIS_NetworkRut*)RList.GetNext(pos);
		if( pNRut )
			m_pRutList[pNRut->GetRBreadthID()][pNRut->GetRutID()] = NULL;
	}
	pos = VList.GetHeadPosition();
	while( pos )
	{
		pNVtx = (CGIS_NetworkVtx*)VList.GetNext(pos);
		if( pNVtx )
		{
			m_pVtxList[pNVtx->GetVBreadthID()][pNVtx->GetVtxID()] = NULL;
			pNVtx->GetVtxSInfo()->byStatusOfNormalSearch = pNVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;//�ڵ���Ϊopen״̬
		}
	}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //���д��ڵ�·��ȫ��ȥ��,���д��ڵĽڵ����δʹ�ù���ȫɾ��
	for( i = 0;	i < m_pMap->GetBreadthMapNum();	i++ )
	{
		pBMap = m_pMap->GetBreadthMapByID2( i );
        ASSERT(pBMap);
		if( pBMap )
		{
			if( pBMap->m_nLoadCounter == 0 )
				continue;
			int j;
			for( j =0;	j<m_nRutNum[pBMap->m_nBreadthID]; j++ )
			{
				pNRut = GetNetworkRut(pBMap->m_nBreadthID,j);
				if( pNRut )
				{
					delete pNRut;
                    pNRut = NULL;
				}
				m_pRutList[pBMap->m_nBreadthID][j] = NULL;
			}
			for( j=0;	j<m_nVtxNum[pBMap->m_nBreadthID];	j++ )
			{
				pNVtx = m_pVtxList[pBMap->m_nBreadthID][j];
				if( pNVtx )
				{
					if( pNVtx->GetVtxSInfo()->byStatusOfNormalSearch != 0 || pNVtx->GetVtxSInfo()->byStatusOfReverseSearch != 0 )
						VList.AddTail( pNVtx );
					else
					{
						delete pNVtx;
                        pNVtx = NULL;
					}
				}
				m_pVtxList[pBMap->m_nBreadthID][j] = NULL;
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //�ѱ�����·�κͽڵ�ָ���ȥ
	pos = RList.GetHeadPosition();
	while( pos )
	{
		pNRut = (CGIS_NetworkRut*)RList.GetNext(pos);
		if( pNRut )
			m_pRutList[pNRut->GetRBreadthID()][pNRut->GetRutID()] = pNRut;
	}
	pos = VList.GetHeadPosition();
	while( pos )
	{
		pNVtx = (CGIS_NetworkVtx*)VList.GetNext(pos);
		if( pNVtx )
			m_pVtxList[pNVtx->GetVBreadthID()][pNVtx->GetVtxID()] = pNVtx;
	}
    //////////////////////////////////////////////////////////////////////////
}
/**
 * @brief 
 *
 * @param 
 * @return
*/
CGIS_NetworkNode* CGIS_MapBreadthNetwork::GetHeadNode(
    CPtrList *pOpenNodeList, 
    CPtrList *pCloseNodeList,
    BYTE byRoadSectionLevel,
    BYTE byValidRoadSectionCount )
{
	POSITION pos = NULL;
	CGIS_NetworkNode *pNode = (CGIS_NetworkNode*)pOpenNodeList->GetHead();
	if( pNode )
	{
		if( pNode->m_pParent )
		{
			if( 
                (pNode->m_pParent->m_pRut->GetRutLevel() >= byRoadSectionLevel && pNode->m_pParent->m_pVtx->GetVtxEdge() != 1)
                || 
				(pNode->m_pParent->m_nLRNum >= byValidRoadSectionCount && pNode->m_pParent->m_pRut->GetRutType() != 4) 
              )
			{
                //�����node��������ôȡ��nodeΪ��ʼ�ڵ�
				pNode = pNode->m_pParent;
				pos = pCloseNodeList->Find( pNode );
				if( pos )
					pCloseNodeList->RemoveAt( pos );
			}
			else
				pOpenNodeList->RemoveHead();
		}
		else
        {
            pOpenNodeList->RemoveHead();
        }
	}

    
	return pNode;
}
void CGIS_MapBreadthNetwork::AddNWNode( CGIS_NetworkNode *pNode, CPtrList *pCNList )
{
	CGIS_NetworkNode *pNTem = pNode->m_pParent;
	while( pNTem )
	{
		pCNList->AddTail(pNTem);
		pNTem = pNTem->m_pParent;
	}
}

void CGIS_MapBreadthNetwork::CalculateRouteBuf( 
     ST_GEO_PTXY ptStart, 
     ST_GEO_PTXY ptEnd, 
     ST_GEO_PTXY pRBuf[], 
     int &nRBNum )
{
	m_nMaxSLBNum = m_nMaxELBNum = NAV_MAX_LBNNUM;
	float dDis = CalcDist( ptStart.dx, ptStart.dy, ptEnd.dx, ptEnd.dy );

	ST_GEO_PTXY pt1;
    ST_GEO_PTXY pt2;

	ST_GEO_PTXY pPtArea[7];
	pt1.dx = 104.0f;
	pt1.dy = 38.16f;
	pt2.dx = 117.0f;
	pt2.dy = 38.16f;
	double dbDis1 = 0.0f;
	double dbDis2 = 0.0f;
	double dbDis = (fabs(ptStart.dx-ptEnd.dx)+fabs(ptStart.dy-ptEnd.dy))*53.483;
	if( dDis < 556150 )
	{
		ST_GEO_RECT rtRout;
		ST_GEO_PTXY pPtList[2];
		pPtList[0] = ptStart;
		pPtList[1] = ptEnd;
		CalcRect( pPtList, 2, rtRout );
		double dDetax, dDetay;
		dDetax = 1.5+fabs(ptStart.dy-ptEnd.dy)/12.98;
		dDetay = 1.5+fabs(ptStart.dx-ptEnd.dx)/12.98;
		if( (rtRout.dTop > pt1.dy && rtRout.dBottom > pt1.dy) && (rtRout.dLeft < pt1.dx && rtRout.dRight > pt1.dx ) )
		{
			dbDis1 = (fabs(ptStart.dx-pt1.dx)+fabs(ptStart.dy-pt1.dy))*53.483;
			dbDis2 = (fabs(pt1.dx-ptEnd.dx)+fabs(pt1.dy-ptEnd.dy))*53.483;
			rtRout.dLeft -= dDetax;
			rtRout.dRight += dDetax;
			rtRout.dTop -= (rtRout.dTop-pt1.dy+dDetay);
			rtRout.dBottom += dDetay;
		}
		else if( (rtRout.dTop < pt2.dy && rtRout.dBottom > pt2.dy) && (rtRout.dLeft > pt2.dx && rtRout.dRight > pt2.dx ) )
		{
			dbDis1 = (fabs(ptStart.dx-pt2.dx)+fabs(ptStart.dy-pt2.dy))*53.483;
			dbDis2 = (fabs(pt2.dx-ptEnd.dx)+fabs(pt2.dy-ptEnd.dy))*53.483;
			rtRout.dLeft -= (rtRout.dLeft-pt2.dx+dDetax+0.6);
			rtRout.dRight += dDetax;
			rtRout.dTop -= dDetay;
			rtRout.dBottom += dDetay;
		}
		else
		{
			rtRout.dLeft -= dDetax;
			rtRout.dRight += dDetax;
			rtRout.dTop -= dDetay;
			rtRout.dBottom += dDetay;
		}
		nRBNum = 5;
		pRBuf[0].dx = pRBuf[4].dx = pRBuf[3].dx = rtRout.dLeft;
		pRBuf[0].dy = pRBuf[4].dy = pRBuf[1].dy = rtRout.dTop;
		pRBuf[1].dx = pRBuf[2].dx = rtRout.dRight;
		pRBuf[2].dy = pRBuf[3].dy = rtRout.dBottom;
		if( dbDis1 > dbDis && dbDis2 > dbDis )
		{
			if( dbDis1 > dbDis2 )
				m_dbMaxDis = dbDis1+75.636;
			else
				m_dbMaxDis = dbDis2+75.636;
		}
		else if( dbDis1 >dbDis || dbDis2>dbDis )
		{
			if( dbDis1 > dbDis )
				m_dbMaxDis = dbDis1+75.636;
			else
				m_dbMaxDis = dbDis2+75.636;
		}
		else 
			m_dbMaxDis = dbDis+75.636;
		return;
	}
	float dBMin = 1.8+444920.1/dDis;
	if( dBMin > 4.01 )
		dBMin = 4.0;
	float dBMax = 4.2 + 889840.1/dDis;
	if( dBMax > 6.1 )
		dBMax = 6.0;
	nRBNum = 0;
	pPtArea[0].dx = pPtArea[6].dx = 109.0;
	pPtArea[0].dy = pPtArea[6].dy = 18.00;
	pPtArea[1].dx = pPtArea[0].dx;
	pPtArea[1].dy = 40.8f;
	pPtArea[2].dx = 122.10f;
	pPtArea[2].dy = 47.82f;
	pPtArea[3].dx = pPtArea[2].dx;
	pPtArea[3].dy = 53.00;
	pPtArea[4].dx = 136.00;
	pPtArea[4].dy = pPtArea[3].dy;
	pPtArea[5].dx = pPtArea[4].dx;
	pPtArea[5].dy = pPtArea[0].dy;
	
	short nCont = 0;
	double dx = 0.0, dy = 0.0;
	short nPtNum = 0;
	ST_GEO_PTXY pPtList[5];
	BOOL bIPStart = PointInPoly( ptStart, pPtArea, 7 );
	BOOL bIPEnd = PointInPoly( ptEnd, pPtArea, 7 );
	ST_GEO_PTXY ptLeft, ptTop;
	ptLeft.dx = 15.0;
	ptLeft.dy = pt1.dy;
	ptTop.dx = pt2.dx;
	ptTop.dy = 10.0;
	BOOL bCross = LineToLine(ptStart, ptEnd, ptLeft, pt1)||LineToLine(ptStart, ptEnd, ptTop, pt2);
	pPtList[nPtNum] = ptStart;
	nPtNum++;
	if( (ptStart.dy > pt1.dy) && (ptEnd.dy > pt1.dy) )
	{
		if( (ptStart.dx<pt1.dx) && (ptEnd.dx>pt1.dx) )
		{
			pPtList[nPtNum] = pt1;
			nPtNum++;
			if( bIPEnd )
			{
				for( int i=0;	i<3;	i++ )
				{
					if( LineToLine(pt1, ptEnd, pPtArea[i], pPtArea[i+1], dx, dy) )
						nCont++;
				}
				if( nCont > 0 )
				{
					pPtList[nPtNum].dx = dx;
					pPtList[nPtNum].dy = dy;
					nPtNum++;
					nCont = -nPtNum;
				}
			}
		}
		else if( (ptStart.dx>pt1.dx) && (ptEnd.dx<pt1.dx) )
		{
			if( bIPStart )
			{
				for( int i=0;	i<3;	i++ )
				{
					if( LineToLine(ptStart, pt1, pPtArea[i], pPtArea[i+1], dx, dy) )
						nCont++;
				}
				if( nCont > 0 )
				{
					pPtList[nPtNum].dx = dx;
					pPtList[nPtNum].dy = dy;
					nPtNum++;
					nCont = nPtNum;
				}
			}
			pPtList[nPtNum] = pt1;
			nPtNum++;
		}
		else
		{
			if( (bIPEnd && !bIPStart) || (!bIPEnd && bIPStart) )
			{
				for( int i=0;	i<3;	i++ )
				{
					if( LineToLine(ptStart, ptEnd, pPtArea[i], pPtArea[i+1], dx, dy) )
						nCont++;
				}
				if( nCont > 0 )
				{
					pPtList[nPtNum].dx = dx;
					pPtList[nPtNum].dy = dy;
					nPtNum++;
					if( bIPStart )
						nCont = nPtNum;
					else 
						nCont = -nPtNum;
				}
			}
		}
	}
	else if( (ptStart.dx > pt2.dx) && (ptEnd.dx > pt2.dx) )
	{
		if( (ptStart.dy<pt2.dy) && (ptEnd.dy>pt2.dy) )
		{
			pPtList[nPtNum] = pt2;
			nPtNum++;
		}
		else if( (ptStart.dy>pt2.dy) && (ptEnd.dy<pt2.dy) )
		{
			pPtList[nPtNum] = pt2;
			nPtNum++;
		}
		else
		{
			if( (bIPEnd && !bIPStart) || (!bIPEnd && bIPStart) )
			{
				for( int i=0;	i<3;	i++ )
				{
					if( LineToLine(ptStart, ptEnd, pPtArea[i], pPtArea[i+1], dx, dy) )
						nCont++;
				}
				if( nCont > 0 )
				{
					pPtList[nPtNum].dx = dx;
					pPtList[nPtNum].dy = dy;
					nPtNum++;
					if( bIPStart )
						nCont = nPtNum;
					else 
						nCont = -nPtNum;
				}
			}
		}
	}
	else if( ( ( ( ((ptStart.dx>pt1.dx)&&(ptEnd.dx<pt1.dx))||((ptStart.dx<pt1.dx)&&(ptEnd.dx>pt1.dx)) ) && 
		( ((ptStart.dy<pt1.dy)&&(ptEnd.dy>pt1.dy))||((ptStart.dy>pt1.dy)&&(ptEnd.dy<pt1.dy))) ) || 
		( ( ((ptStart.dx>pt2.dx)&&(ptEnd.dx<pt2.dx))||((ptStart.dx<pt2.dx)&&(ptEnd.dx>pt2.dx)) ) && 
		( ((ptStart.dy<pt2.dy)&&(ptEnd.dy>pt2.dy))||((ptStart.dy>pt2.dy)&&(ptEnd.dy<pt2.dy)) ) ) ) && 
		!( ( ( ((ptStart.dx>pt1.dx)&&(ptEnd.dx<pt1.dx))||((ptStart.dx<pt1.dx)&&(ptEnd.dx>pt1.dx)) ) && 
		( ((ptStart.dy<pt1.dy)&&(ptEnd.dy>pt1.dy))||((ptStart.dy>pt1.dy)&&(ptEnd.dy<pt1.dy)) ) ) && 
		( ( ((ptStart.dx>pt2.dx)&&(ptEnd.dx<pt2.dx))||((ptStart.dx<pt2.dx)&&(ptEnd.dx>pt2.dx)) ) && 
		( ((ptStart.dy<pt2.dy)&&(ptEnd.dy>pt2.dy))||((ptStart.dy>pt2.dy)&&(ptEnd.dy<pt2.dy)))) ) && !bCross )
	{
		double dbArea1 = CalcArea( ptStart, ptEnd, pt1 );
		double dbArea2 = CalcArea( ptStart, ptEnd, pt2 );
		if( (dbArea1>0.0 &&dbArea2>0.0) || (dbArea1<0.0 &&dbArea2<0.0) )
		{
			if( fabs(dbArea1)>fabs(dbArea2) )
			{
				if( (bIPStart && !bIPEnd) || (!bIPStart && bIPEnd) )
				{
					if( bIPStart )
					{
						pPtList[nPtNum] = pt2;
						nPtNum++;
						for( int i=0;	i<3;	i++ )
						{
							if( LineToLine(pt2, ptEnd, pPtArea[i], pPtArea[i+1], dx, dy) )
								nCont++;
						}
						if( nCont > 0 )
						{
							pPtList[nPtNum].dx = dx;
							pPtList[nPtNum].dy = dy;
							nPtNum++;
							nCont = nPtNum;
						}
					}
					else if( bIPEnd )
					{
						for( int i=0;	i<3;	i++ )
						{
							if( LineToLine(pt2, ptEnd, pPtArea[i], pPtArea[i+1], dx, dy) )
								nCont++;
						}
						if( nCont > 0 )
						{
							pPtList[nPtNum].dx = dx;
							pPtList[nPtNum].dy = dy;
							nPtNum++;
							pPtList[nPtNum] = pt2;
							nPtNum++;
							nCont = -nPtNum;
						}
						else
						{
							pPtList[nPtNum] = pt2;
							nPtNum++;
						}
					}
				}
				else
				{
					pPtList[nPtNum] = pt2;
					nPtNum++;
				}
			}
			else
			{
				if( (bIPStart && !bIPEnd) || (!bIPStart && bIPEnd) )
				{
					if( bIPStart )
					{
						for( int i=0;	i<3;	i++ )
						{
							if( LineToLine(ptStart, pt1, pPtArea[i], pPtArea[i+1], dx, dy) )
								nCont++;
						}
						if( nCont > 0 )
						{
							pPtList[nPtNum].dx = dx;
							pPtList[nPtNum].dy = dy;
							nPtNum++;
							nCont = nPtNum;
						}
						pPtList[nPtNum] = pt2;
						nPtNum++;
					}
					else if( bIPEnd )
					{
						pPtList[nPtNum] = pt1;
						nPtNum++;
						for( int i=0;	i<3;	i++ )
						{
							if( LineToLine(pt1, ptEnd, pPtArea[i], pPtArea[i+1], dx, dy) )
								nCont++;
						}
						if( nCont > 0 )
						{
							pPtList[nPtNum].dx = dx;
							pPtList[nPtNum].dy = dy;
							nPtNum++;
							nCont = -nPtNum;
						}
					}
				}
				else
				{
					pPtList[nPtNum] = pt1;
					nPtNum++;
				}
			}
		}
		else
		{
			if( (bIPStart && !bIPEnd) || (!bIPStart && bIPEnd) )
			{
				for( int i=0;	i<3;	i++ )
				{
					if( LineToLine(ptStart, ptEnd, pPtArea[i], pPtArea[i+1], dx, dy) )
						nCont++;
				}
				if( nCont > 0 )
				{
					pPtList[nPtNum].dx = dx;
					pPtList[nPtNum].dy = dy;
					nPtNum++;
					if( bIPStart )
						nCont = nPtNum;
					else
						nCont = -nPtNum;
				}
			}
		}
	}
	else 
	{
		if( (bIPEnd && !bIPStart) || (!bIPEnd && bIPStart) )
		{
			for( int i=0;	i<3;	i++ )
			{
				if( LineToLine(ptStart, ptEnd, pPtArea[i], pPtArea[i+1], dx, dy) )
					nCont++;
			}
			if( nCont > 0 )
			{
				pPtList[nPtNum].dx = dx;
				pPtList[nPtNum].dy = dy;
				nPtNum++;
				if( bIPStart )
					nCont = nPtNum;
				else 
					nCont = -nPtNum;
			}
		}
	}
	pPtList[nPtNum] = ptEnd;
	nPtNum++;
	if( nCont != 0 )
	{
		ST_GEO_PTXY pPtBuf1[1024];
		int nPtBuf1 = 0;
		ST_GEO_PTXY pPtBuf2[1024];
		int nPtBuf2 = 0;
		ST_GEO_PTXY *pPtList1 = NULL;
		if( nCont > 0 )
		{
			pPtList1 = pPtList;
			CalcLineBuff( dBMin, pPtList1, nCont, pPtBuf1, nPtBuf1 );
			pPtList1 = pPtList+(nCont-1);
			CalcLineBuff( dBMax, pPtList1, nPtNum-nCont+1, pPtBuf2, nPtBuf2 );
		}
		else if( nCont < 0 )
		{
			pPtList1 = pPtList;
			CalcLineBuff( dBMax, pPtList1, -nCont, pPtBuf1, nPtBuf1 );
			pPtList1 = pPtList+(-nCont-1);
			CalcLineBuff( dBMin, pPtList1, nPtNum+nCont+1, pPtBuf2, nPtBuf2 );
		}
		int nNum = 3;
		if( nPtBuf1 == 10 )
		{
			if( nCont > 0 )
				nNum = 4;
			else 
				nNum = 5;
		}
		else if( nPtBuf1 == 11 || nPtBuf1 == 9 )
			nNum = 4;
		int i;
		for( i=0; i<nNum;	i++ )
		{
			pRBuf[nRBNum] = pPtBuf1[i];
			nRBNum++;
		}
		for( i=1;	i<nPtBuf2-1;	i++ )
		{
			pRBuf[nRBNum] = pPtBuf2[i];
			nRBNum++;
		}
		for( i=nNum+1;	i<nPtBuf1;	i++ )
		{
			pRBuf[nRBNum] = pPtBuf1[i];
			nRBNum++;
		}
	}
	else
	{
		if( bIPStart && bIPEnd )
		{
			if( nPtNum > 2 )
			{
				double db1 = CalcDist(pPtList[0].dx, pPtList[0].dy, pPtList[1].dx, pPtList[1].dy);
				double db2 = CalcDist(pPtList[2].dx, pPtList[2].dy, pPtList[1].dx, pPtList[1].dy);
				double db = db1>db2?db2:db1;
				if( db < dBMin*111.23*1000 )
				{
					pPtList[1] = pPtList[2];
					nPtNum = 2;
				}
			}
			CalcLineBuff( dBMin, pPtList, nPtNum, pRBuf, nRBNum );
		}
		else 
		{
			if( nPtNum > 2 )
			{
				double db1 = CalcDist(pPtList[0].dx, pPtList[0].dy, pPtList[1].dx, pPtList[1].dy);
				double db2 = CalcDist(pPtList[2].dx, pPtList[2].dy, pPtList[1].dx, pPtList[1].dy);
				double db = db1>db2?db2:db1;
				if( db < dBMax*111.23*1000 )
				{
					pPtList[1] = pPtList[2];
					nPtNum = 2;
				}
			}
			CalcLineBuff( dBMax, pPtList, nPtNum, pRBuf, nRBNum );
		}
	}
	m_dbMaxDis = 0.0;
	for( int i=1;	i<nPtNum;	i++ )
		m_dbMaxDis += (fabs(pPtList[i-1].dx-pPtList[i].dx)+fabs(pPtList[i-1].dy-pPtList[i].dy))*53.483;
	m_dbMaxDis += 75.636;
	if( bIPStart && bIPEnd )
		m_nMaxSLBNum = m_nMaxELBNum = 448;
	else if( bIPStart )
		m_nMaxSLBNum = 512;
	else if( bIPEnd )
		m_nMaxELBNum = 512;
}


BOOL CGIS_MapBreadthNetwork::ReadBreadthNetworkV2( 
    CGIS_BreadthMap **pBNet, BYTE byLFlag,
    int& nLayer0RoadSectionMax, int& nLayer1RoadSectionMax,int& nLayer2RoadSectionMax,
    int& nLayer0VertexMax, int& nLayer1VertexMax,int& nLayer2VertexMax)
{
	
	if( (*pBNet)->m_nLoadCounter != 0 )
	{
		(*pBNet)->m_nLoadCounter++;
		return TRUE;
	}
	if( gHNFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sAutomap.Rut", m_pMap->GetMapInfo()->GetMapFilePath() );
		gHNFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gHNFile)
			return FALSE;
	}

	ST_GEO_RECT rtRect = m_pMap->GetBreadthMapRect( *pBNet );
	if( !RectInPoly(rtRect,m_pRBuf, m_nRBNum) )
		return FALSE;
   

	TRY
	{
//		TRACE( _T("BLID:%d,%d\n"),(*pBNet)->m_nBreadthID,(*pBNet)->m_nBMID );
		CFile file(gHNFile);
        CFile* pFile = &file;
		pFile->SeekToBegin();
		char szInfo[32];
		pFile->Read( szInfo, 9 );
		int nMNum = 0;
		int nMID = 0;
		int nMOff = 0;
		pFile->Read( &nMNum, sizeof(int) );
		int i;
		for( i=0;	i<nMNum;	i++ )
		{
			pFile->Read( &nMID, sizeof(int) );
			pFile->Read( &nMOff, sizeof(int) );
			if( nMID == m_pMap->GetMapInfo()->GetMapID() )
				break;
		}
		int nRawBreadthBID = 0;
		int dwCSize = 0;
		int nBOff = nMOff + 13 + ((*pBNet)->m_nBreadthID)*12;
		pFile->Seek( nBOff, CFile::begin );
		pFile->Read( &nRawBreadthBID, sizeof(int) );
        ASSERT(nRawBreadthBID == (*pBNet)->m_nBMID);
		pFile->Read( &nBOff, sizeof(int) );
		pFile->Read( &dwCSize, sizeof(int) );
		pFile->Seek( nMOff+nBOff, CFile::begin );

		int nUnCom = 5;
		pFile->Read( &nUnCom, sizeof(int) );
		int nCom = dwCSize - sizeof( int );
		if( nUnCom > nCom*8 )
			return FALSE;
        
        char* pFBuf = m_pBufOfBeforeCompress;
        char* pBuf = m_pBufOfAfterCompress;
                    
//////////////////////////////////////////////////////////////////////////
        DWORD dwSize = 0;
        DWORD dwDataPos = pFile->GetPosition();
        int nReadCountLimit = 2;//��������
        for(int nReadCount = 0; nReadCount < 2; nReadCount++)
        {
            pFile->Seek(dwDataPos,CFile::begin);    
                 
            UINT nRead = pFile->Read( pBuf , nCom );
            if(nRead != nCom)
            {
                TRACE2("read rtn != required size![%d<-%d]\n",nRead,nCom);
                return FALSE;
            }
            
            dwSize = nUnCom;
            int nUnComRtn = uncompress( (unsigned char*)pFBuf , &dwSize, (unsigned char*)pBuf , nCom );
            
            if(Z_OK == nUnComRtn)
            {
                if(nReadCount != 0)
                {
                    TRACE1("ReadBreadthNetwork reload[%d] sucess!\n",nReadCount);
                }
                break;
            }
        
#ifdef _DEBUG
            if(Z_OK != nUnComRtn)
            {
                TRACE3("[error:nCom = %d;nUnCom = %d,BreadthID = %d]\n",nCom,nUnCom,(*pBNet)->m_nBreadthID);
                TRACE1("uncompress failed when ReadBreadthNetwork! uncompress code = %d\n",nUnComRtn);
            }
#endif
            
            if(nReadCountLimit-1 == nReadCount)
            {
                m_byErrCode = 1;
                return FALSE;
            }


            
        }     
//////////////////////////////////////////////////////////////////////////


		///�ж��ļ��İ汾��Ϣ
		DWORD dwStation = 0;
		Fread( pFBuf , dwSize, dwStation, szInfo, 3, 1 );
		if( swString(szInfo).IsEmpty() )
		{
			return FALSE;
		}
		if( swString(szInfo).Find( L"RUT" ) < 0 )
		{
			return FALSE;
		}
		int nVer = 0;
		Fread( pFBuf , dwSize, dwStation, &nVer, sizeof(int), 1 );
		if( nVer != 910 )
		{
			return FALSE;
		}

		////��ȡ��������
		Fread( pFBuf , dwSize, dwStation, &nRawBreadthBID, sizeof(int), 1 );
        ASSERT(nRawBreadthBID == (*pBNet)->m_nBMID);
		int nRutNum = 0;
		int nVtxNum = 0;
		Fread( pFBuf , dwSize, dwStation, &nRutNum, sizeof(unsigned short), 1 );
		Fread( pFBuf , dwSize, dwStation, &nVtxNum, sizeof(unsigned short), 1 );
		Fread( pFBuf , dwSize, dwStation, &nRutNum, sizeof(int), 1 );
		Fread( pFBuf , dwSize, dwStation, &nVtxNum, sizeof(int), 1 );
		if( nVtxNum == 0 )
		{
			return FALSE;
		}

		int nRLNum = 0;
		int nVLNum = 0;
		int nRLNum1 = 0;	//�ڶ��ȼ�·��
		int nVLNum1 = 0;
		int nVOff = 0;
		Fread( pFBuf, dwSize, dwStation, &nRLNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVLNum, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nRLNum1, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVLNum1, sizeof(int), 1 );
		Fread( pFBuf, dwSize, dwStation, &nVOff, sizeof(int), 1 );
		///��ȡ·����Ϣ
		DEFINE_VARIABLE_USED_BY_READ_ROADSECTION_FEATURE;

		int nReadNum = nRutNum;
		if( byLFlag == 3 )
			nReadNum = nRLNum;
		else if( byLFlag == 2 )
			nReadNum = nRLNum+nRLNum1;

        nLayer0RoadSectionMax = nRLNum;
        nLayer1RoadSectionMax = nRLNum + nRLNum1;
        nLayer2RoadSectionMax = nRutNum;
        nLayer0VertexMax = nVLNum;
        nLayer1VertexMax = nVLNum + nVLNum1;
        nLayer2VertexMax = nVtxNum;
        if(nReadNum == 0)
        {
            return FALSE;
        }
		m_nRutNum[nBreadthID] = nReadNum;
        m_pRutList[nBreadthID] = (CGIS_NetworkRut**)m_pMemMngOfBuddyData->newmalloc(m_nRutNum[nBreadthID]*sizeof(CGIS_NetworkRut*));
        if(!(m_pRutList[nBreadthID]))
        {
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_OUT_OF_MEMORY));
        }
        int nErrorCode;
		memset( m_pRutList[nBreadthID], 0, sizeof(CGIS_NetworkRut*)*m_nRutNum[nBreadthID] );
		for( i=0;	i<nReadNum;	i++ )
		{
			READ_ROADSECTION_FEATURE;
			CGIS_NetworkRut *pNRut = new CGIS_NetworkRut;
			pNRut->SetRutInfo( nRutID, byDirection, byType, byCharge, nLaneNum, bySpeed, 
					nRutLevel, byRutType, fRutLength, nVtxStartID, nVtxEndID, nBreadthID );
			pNRut->SetLimitInfo( pLimitInfo);
			m_pRutList[nBreadthID][nRutID] = pNRut;
		}
		
		//��ȡ�ڵ���Ϣ
        DEFINE_VARIABLE_USED_BY_READ_VERTEX_FEATURE;
        nReadNum = nVtxNum;
		if( byLFlag == 3 )
		{
			dwStation = nVOff;
			nReadNum = nVLNum;
		}
		else if( byLFlag == 2 )
		{
			dwStation = nVOff;
			nReadNum = nVLNum+nVLNum1;
		}

        if(nReadNum == 0)
        {
			ClearBNetwork( *pBNet );
            return FALSE;
        }
		m_nVtxNum[nBreadthID] = nReadNum;
		m_pVtxList[nBreadthID] = (CGIS_NetworkVtx**)m_pMemMngOfBuddyData->newmalloc(m_nVtxNum[nBreadthID]*sizeof(CGIS_NetworkVtx*));
        if(!(m_pVtxList[nBreadthID]))
        {
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_OUT_OF_MEMORY));
        }

		memset( m_pVtxList[nBreadthID], 0, sizeof(CGIS_NetworkVtx*)*m_nVtxNum[nBreadthID] );
//		ST_GEO_RECT rt = CalculateBMapRect( (*pBNet)->m_nBMID );
		for( i=0;	i<nReadNum;	i++ )
		{
            READ_VERTEX_FEATURE;
			CGIS_NetworkVtx *pNVtx = new CGIS_NetworkVtx;
			pNVtx->SetVtxInfo( nVtxID, ptPosition, nBreadthID );
			pNVtx->SetVEInfo( byEdge, nABID, nARID );
			pNVtx->SetVRutInfo( byRNum, pVRList, pRTList, pRLList );
			m_pVtxList[nBreadthID][nVtxID] = pNVtx;
		}

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        
		if( m_nSLBNum >= m_nMaxSLBNum && m_byCFlag == 1 )
		{
			ClearNWBreadth( m_pSLBID, m_nSLBNum%m_nMaxSLBNum );
		}
		if( m_nELBNum >= m_nMaxELBNum && m_byCFlag == 2 )
		{
			ClearNWBreadth( m_pELBID, m_nELBNum%m_nMaxELBNum );
		}
        
		{
			if( m_byCFlag == 1 )
			{
				m_pSLBID[m_nSLBNum%m_nMaxSLBNum] = (*pBNet)->m_nBreadthID;
				m_nSLBNum++;					//�Ѽ���ͼ������
			}
			else if( m_byCFlag == 2 )
			{
				m_pELBID[m_nELBNum%m_nMaxELBNum] = (*pBNet)->m_nBreadthID;
				m_nELBNum++;					//�Ѽ���ͼ������
			}
		}

		(*pBNet)->m_nLoadCounter++;
	}
	CATCH( CMemoryException, e )
	{

        TRACE0("catched CMemoryException\n");
        m_byErrCode = CGIS_NetworkException::ERROR_FILE_IO;
        
		ClearBNetwork( *pBNet );
		return FALSE;
	}
	AND_CATCH( CFileException, e )
	{
        TRACE0("catched CFileException\n");
        m_byErrCode = CGIS_NetworkException::ERROR_FILE_IO;

		ClearBNetwork( *pBNet );
		if(INVALID_HANDLE_VALUE != gHNFile)
			CloseHandle( gHNFile );
		gHNFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	AND_CATCH( CGIS_NetworkException, e )
	{
		TRACE0("catched CGIS_NetworkException\n");
		m_byErrCode = e->GetErrorCode();
		ClearBNetwork( *pBNet );
		return FALSE;
	}
	AND_CATCH( CUserException, e )
	{
		TRACE0("catched CUserException\n");
		m_byErrCode = CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
		ClearBNetwork( *pBNet );
		return FALSE;
	}
	END_CATCH

        
	return TRUE;
}

/**
* @brief ���㵼��·��
*
* @param byCType-(0.������·�߹滮 1.ƫ��ʱ��·�߹滮 2.��·ʱ·�߹滮),
*       ����byCTypeΪ1.2���������Ҫ���복�������
*              
* @return  1.����ɹ�
*          0.·��������ж��     
*         -1.����û��ƥ���·��         
*         -2.·����ͨ     
*         -3.�ļ�I/O�쳣     
*         -4.�ڴ治��
*         -5.���߹���,��������.
*         -6.������������
*         -7.���㱻�ⲿ��ֹ
*/	
int CGIS_MapBreadthNetwork::CalculateNavigatePathActual( CPtrList* WayPointList,CPtrList* pPathpointlist )
{
    ASSERT(WayPointList);
    ASSERT(pPathpointlist);
    
    TRACK_FUNCTION("********************************************");
	if( m_pNetworkRoute == NULL )
        return Clear( CGIS_NetworkException::ERROR_CONDITION_NOT_ENOUGH );

	//////////////////////////////////////////////////////////////////////////
	//tanying 20080808 add this code 
	CGIS_NetworkPath* pPath = m_pNetworkRoute->GetNetworkPath();
	if(pPath)
	{
		CPtrList* pNodeList = pPath->GetNodeList();
		if(pNodeList)
		{
			POSITION pos = pNodeList->GetHeadPosition();
			while(pos)
			{
				CGIS_PathpointNodeV2* pNode = (CGIS_PathpointNodeV2*)pNodeList->GetNext(pos);
				ASSERT(pNode);
				delete pNode;
			}
			pNodeList->RemoveAll();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	
    //////////////////////////////////////////////////////////////////////////
    //��ʼ����Ա������״̬
	m_byErrCode = 0;
	m_LoadNetworkFlag = 1;
	m_byCFlag = 1;
	POSITION pos = WayPointList->GetHeadPosition();
	CGIS_WayPoint *pWPOne = (CGIS_WayPoint*)WayPointList->GetNext(pos);
	if( pWPOne == NULL )
		return Clear( -1 );

	TRACE0("����������.\n");
	m_nRBNum = 5;
	m_pRBuf[0].dx = m_pRBuf[4].dx = m_pRBuf[3].dx = 70.00f;
	m_pRBuf[0].dy = m_pRBuf[4].dy = m_pRBuf[1].dy = 15.00f;
	m_pRBuf[1].dx = m_pRBuf[2].dx = 137.20f;
	m_pRBuf[2].dy = m_pRBuf[3].dy = 55.50f;
	memset( m_pSLBID, 0, sizeof(int)*m_nMaxSLBNum );
	m_nSLBNum = 0;					//�Ѽ���ͼ������
	memset( m_pELBID, 0, sizeof(int)*m_nMaxELBNum );
	m_nELBNum = 0;					//�Ѽ���ͼ������
	m_byClear = 0;

	double dbTotalDistance = 0.0;
	short nWPNum = 1;
	CGIS_WayPoint *pWPTwo = (CGIS_WayPoint*)WayPointList->GetAt(pos);
	if( pWPTwo == NULL )
		return Clear( -1 );


   
    int nErrorCode = 0;
	if( !FullWayPointData2(pWPOne, m_pMap, m_pNetworkRoute->GetNetworkPathInfo(), pWPTwo->GetWPPosition(),nErrorCode) )
	{
        TRACE0("����������ʧ��!\n");
        return Clear( nErrorCode );
	}

	while( pos )
	{

		TRACE0("****���;�е�����.\n");

		m_eCalculateScene = SCENE_NORMAL;//���ó���Ϊ��ͨ,��߻���ݾ������������
		m_byRLevel = 4;
		m_LoadNetworkFlag = 1;
		m_byCFlag = 2;
		m_byClear = 0;
		m_nRBNum = 5;
		m_pRBuf[0].dx = m_pRBuf[4].dx = m_pRBuf[3].dx = 70.00f;
		m_pRBuf[0].dy = m_pRBuf[4].dy = m_pRBuf[1].dy = 15.00f;
		m_pRBuf[1].dx = m_pRBuf[2].dx = 137.20f;
		m_pRBuf[2].dy = m_pRBuf[3].dy = 55.50f;
		pWPTwo = (CGIS_WayPoint*)WayPointList->GetNext(pos);


        int nErrorCode = 0;
		if( !FullWayPointData(pWPTwo,m_pMap,pWPOne->GetWPPosition(),nErrorCode))
		{
            TRACE0("���;�е�����ʧ��!\n");
            return Clear( nErrorCode );
        }

		TRACE0("���㺽��BUF.\n");
		m_nRBNum = 0;
		CalculateRouteBuf( 
            pWPOne->GetWPPosition(),
            pWPTwo->GetWPPosition(),
            m_pRBuf,
            m_nRBNum );
		double dbDis = CalcDist(
            pWPOne->GetWPPosition().dx,pWPOne->GetWPPosition().dy, 
			pWPTwo->GetWPPosition().dx,pWPTwo->GetWPPosition().dy
            );
 
		//////////////////////////////////////////////////////////////////////////
        TRACE0("----��ǰ������Ϣ----\n");
		TRACE2("\t���:[ %f,%f ]\n",pWPOne->GetWPPosition().dx,pWPOne->GetWPPosition().dy);
		TRACE2("\t�յ�:[ %f,%f ]\n",pWPTwo->GetWPPosition().dx,pWPTwo->GetWPPosition().dy);
		TRACE1("\t����:[ %.2f km]\n",dbDis/1000);
		TRACE0("--------------------\n");
        //////////////////////////////////////////////////////////////////////////
        DBG2LOG0("----Current Calculate Info----\r");
		DBG2LOG2("\tStart:[ %f,%f ]\r",pWPOne->GetWPPosition().dx,pWPOne->GetWPPosition().dy);
		DBG2LOG2("\tEnd  :[ %f,%f ]\r",pWPTwo->GetWPPosition().dx,pWPTwo->GetWPPosition().dy);
		DBG2LOG1("\tDist :[ %.2f km]\r",dbDis/1000);
		DBG2LOG0("--------------------\r");
        //////////////////////////////////////////////////////////////////////////

        if( pos )
		{
			CGIS_WayPoint *pWPoint = (CGIS_WayPoint*)WayPointList->GetAt(pos);
			if( pWPoint )
			{
				double dbLen = CalcDist(pWPTwo->GetWPPosition().dx,pWPTwo->GetWPPosition().dy, 
						pWPoint->GetWPPosition().dx,pWPoint->GetWPPosition().dy);
				if( dbDis > 300000.1 || dbLen > 300000.1 || dbTotalDistance > 300000.1 )
					dbTotalDistance += 500000.0;
			}
		}
         
        
		BOOL bCalculateSuccess = FALSE;

        TRY
        {
            WATCH("��ǰ����·������");
            CGIS_NetworkPathInfo* pPathInfo =  m_pNetworkRoute->GetNetworkPathInfo();
            ASSERT(pPathInfo);
            
            //////////////////////////////////////////////////////////////////////////
            stuTacticOfSearchPathHead* pTactic = &m_stTacticOfSearchPathHead;
            CPtrList aOpenListForHeadSearchExitOfResidentialArea;
            CPtrList aClosListForHeadSearchExitOfResidentialArea;
            CPtrList aOpenListForTailSearchExitOfResidentialArea;
            CPtrList aClosListForTailSearchExitOfResidentialArea;
            
            CPtrList aOpenListForSearchHeadNodeUpToTrunk;
            CPtrList aClosListForSearchHeadNodeUpToTrunk;
            CPtrList aOpenListForSearchTailNodeUpToTrunk;
            CPtrList aClosListForSearchTailNodeUpToTrunk;
            
            CPtrList aOpenListForSearchMidNormal;
            CPtrList aClosListForSearchMidNormal;
            CPtrList aOpenListForSearchMidReverse;
            CPtrList aClosListForSearchMidReverse;
            
            pTactic->pOpenListForHeadSearchExitOfResidentialArea= &aOpenListForHeadSearchExitOfResidentialArea;
            pTactic->pClosListForHeadSearchExitOfResidentialArea= &aClosListForHeadSearchExitOfResidentialArea;
            pTactic->pOpenListForTailSearchExitOfResidentialArea= &aOpenListForTailSearchExitOfResidentialArea;
            pTactic->pClosListForTailSearchExitOfResidentialArea= &aClosListForTailSearchExitOfResidentialArea;
            
            pTactic->pOpenListForSearchHeadNodeUpToTrunk= &aOpenListForSearchHeadNodeUpToTrunk;
            pTactic->pClosListForSearchHeadNodeUpToTrunk= &aClosListForSearchHeadNodeUpToTrunk;
            pTactic->pOpenListForSearchTailNodeUpToTrunk= &aOpenListForSearchTailNodeUpToTrunk;
            pTactic->pClosListForSearchTailNodeUpToTrunk= &aClosListForSearchTailNodeUpToTrunk;
            
            pTactic->pOpenListForSearchMidNormal                       = &aOpenListForSearchMidNormal;
            pTactic->pClosListForSearchMidNormal                       = &aClosListForSearchMidNormal; 
            pTactic->pOpenListForSearchMidReverse                      = &aOpenListForSearchMidReverse;
            pTactic->pClosListForSearchMidReverse                      = &aClosListForSearchMidReverse; 

//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pOpenListForHeadSearchExitOfResidentialArea);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pClosListForHeadSearchExitOfResidentialArea);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pOpenListForTailSearchExitOfResidentialArea);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pClosListForTailSearchExitOfResidentialArea);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pOpenListForSearchHeadNodeUpToTrunk);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pClosListForSearchHeadNodeUpToTrunk);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pOpenListForSearchTailNodeUpToTrunk);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pClosListForSearchTailNodeUpToTrunk);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pOpenListForSearchMidNormal);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pClosListForSearchMidNormal);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pOpenListForSearchMidReverse);
//            CLEARNODELISTANDRESETVERTEXSTATE(pTactic->pClosListForSearchMidReverse);
//            //////////////////////////////////////////////////////////////////////////
            //��δ��뱣֤���ɵ���Դ���Եõ��ͷ�,��Ҫ�����ɵ�nodeռ�õĿռ�
            CLocalNodeListMng aLocalNodeListMng0(pTactic->pOpenListForHeadSearchExitOfResidentialArea);
            CLocalNodeListMng aLocalNodeListMng1(pTactic->pClosListForHeadSearchExitOfResidentialArea);
            CLocalNodeListMng aLocalNodeListMng2(pTactic->pOpenListForTailSearchExitOfResidentialArea);
            CLocalNodeListMng aLocalNodeListMng3(pTactic->pClosListForTailSearchExitOfResidentialArea);
            CLocalNodeListMng aLocalNodeListMng4(pTactic->pOpenListForSearchTailNodeUpToTrunk);
            CLocalNodeListMng aLocalNodeListMng5(pTactic->pClosListForSearchTailNodeUpToTrunk);
            CLocalNodeListMng aLocalNodeListMng6(pTactic->pOpenListForSearchMidNormal);
            CLocalNodeListMng aLocalNodeListMng7(pTactic->pClosListForSearchMidNormal);
            CLocalNodeListMng aLocalNodeListMng8(pTactic->pOpenListForSearchMidReverse);
            CLocalNodeListMng aLocalNodeListMng9(pTactic->pClosListForSearchMidReverse);
            //////////////////////////////////////////////////////////////////////////

            if(  dbDis < 2000.1 )
            {
                TRACE0("----��ǰ�������----\n");
                TRACE0("\t��������յ�ֱ�ӽ���A*�㷨\n");
                TRACE0("----------------\n");

                bCalculateSuccess = CalculateNavigatePathShortLeg (pWPOne,pWPTwo, pPathInfo,	m_pMap, pPathpointlist);
            }
            else if( dbDis < 30000.1)
            {
                TRACE0("----��ǰ�������----\n");
                TRACE0("\t�������յ����˫��A*��������.\n");
                TRACE0("----------------\n");
                
                bCalculateSuccess = CalculateNavigatePathMidLeg    (pWPOne,pWPTwo, pPathInfo,	m_pMap, pPathpointlist);
            }
            else if( dbDis < 300000.1 )
            {
                TRACE0("----��ǰ�������----\n");
                TRACE0("\t�������¸ߵȼ�·���Ľڵ�,˫��A*��������[3]\n");
                TRACE0("----------------\n");
                
                bCalculateSuccess = CalculateNavigatePathLongLeg    (pWPOne,pWPTwo, pPathInfo,	m_pMap, pPathpointlist);
            }
            else if( dbDis < 800000.1 )
            {
                TRACE0("----��ǰ�������----\n");
                TRACE0("\t�������¸ߵȼ�·���Ľڵ�,˫��A*��������[4]\n");
                TRACE0("----------------\n");

                bCalculateSuccess = CalculateNavigatePathMidLongLeg (pWPOne,pWPTwo, pPathInfo,	m_pMap, pPathpointlist);
            }
            else
            {
                TRACE0("----��ǰ�������----\n");
                TRACE0("\t�������¸ߵȼ�·���Ľڵ�,˫��A*��������[4]\n");
                TRACE0("----------------\n");
                bCalculateSuccess = CalculateNavigatePathBigLongLeg (pWPOne,pWPTwo, pPathInfo,	m_pMap, pPathpointlist);
            } 
            
        }
        CATCH(CUserException,e)
        {
            return Clear(CGIS_NetworkException::ERROR_OUT_OF_MEMORY);
        }
        AND_CATCH(CMemoryException,e)
        {          
            return Clear(CGIS_NetworkException::ERROR_OUT_OF_MEMORY);
        }
        AND_CATCH(CGIS_NetworkException,e)
        {
            return Clear(e->GetErrorCode());
        }
        END_CATCH
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        //���ִ�е�����һ���Ǽ���ɹ���
        ASSERT(bCalculateSuccess == TRUE);
        TRACE0("��ǰ�������ɹ�.\n");
        DBG2SCR0("��ǰ�������ɹ�.\n");
        DBG2LOG0("Current Calculate Success.\n");
        
        
        //////////////////////////////////////////////////////////////////////////
        //
		pWPOne = pWPTwo;
		nWPNum++;
		dbTotalDistance += dbDis;
		if( dbTotalDistance > 8000000.1 )
			return Clear( -2 );

		if( m_eCalculateScene == SCENE_FAR 
            ||
            nWPNum > 5
            || 
            dbTotalDistance > 500000.1 
            || 
            m_byClear == 1 )
		{
			//TRACE0("����ж������,ж��ȫ��·������.\n");
            //DBG2SCR0("����ж������,ж��ȫ��·������.\n");
            
			memset( m_pSLBID, 0, sizeof(int)*m_nMaxSLBNum );
			m_nSLBNum = 0;					//�Ѽ���ͼ������
			memset( m_pELBID, 0, sizeof(int)*m_nMaxELBNum );
			m_nELBNum = 0;					//�Ѽ���ͼ������
            
			m_eCalculateScene = SCENE_NONE;
			m_byRLevel = 4;
			m_LoadNetworkFlag = 1;
			m_byCFlag = 1;
            
			nWPNum = 0;
			dbTotalDistance = 0.0;
			ClearNetwork();
			if( pos )
			{
				pWPTwo->ClearWPRutInfo();
				m_nRBNum = 5;
				m_pRBuf[0].dx = m_pRBuf[4].dx = m_pRBuf[3].dx = 70.00f;
				m_pRBuf[0].dy = m_pRBuf[4].dy = m_pRBuf[1].dy = 15.00f;
				m_pRBuf[1].dx = m_pRBuf[2].dx = 137.20f;
				m_pRBuf[2].dy = m_pRBuf[3].dy = 55.50f;
                
                int nErrorCode = 0;
				if( !FullWayPointData(pWPTwo,m_pMap,pWPOne->GetWPPosition(),nErrorCode))
                {
                    TRACE0("ж��֮������;�е�����ʧ��!\n");
                    return Clear( nErrorCode );
                }
            }
		}
	}

	return Clear( 1 );
}

/**
 * @brief ��V2�Ļ����ϸ����˽ӿڵķ���ֵ�����ش�����Ĺ���
 * ���ڴ˰汾�������˽�ֹ��ԽС��������.
 * @param 
 * @return
*/
CGIS_MapBreadthNetwork::enBoolValue CGIS_MapBreadthNetwork::CalculateNavigatePathShortLeg(
    CGIS_WayPoint *pWPOne, 
    CGIS_WayPoint *pWPTwo, 
    CGIS_NetworkPathInfo *pPathInfo, 
    CGIS_GeoMap *pMap, 
    CPtrList *pPathPointList )
{
    //////////////////////////////////////////////////////////////////////////
    //դ��
	ASSERT(pWPOne);
	ASSERT(pWPTwo);
	ASSERT(pPathInfo);
	ASSERT(pMap);
	ASSERT(pPathPointList);

    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkNode* pStartNode    = NULL;
    CGIS_NetworkNode* pEndNode      = NULL;
    stuTacticOfSearchPathHead* pTactic = &m_stTacticOfSearchPathHead;
    
step1:
	BOOL bFind = FALSE;

    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ�ڵ��Ŀ��ڵ�
	CGIS_NetworkVtx *pStartVtx = NULL;
	CGIS_NetworkVtx *pEndVtx = NULL;
	GetRealVtx( pWPOne, pWPTwo, pMap, &pStartVtx, &pEndVtx );
	if( pStartVtx==NULL || pEndVtx==NULL )
    {
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_OUT_OF_MEMORY));
    }
    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ·�κ���ֹ·��
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    CGIS_NetworkRut* pStartRoadSection = GetNetworkRut( pWPOne->GetWPRutNID(eMapType), pWPOne->GetWPRutID(eMapType) );
    CGIS_NetworkRut* pEndRoadSection = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType) );
    ASSERT(pStartRoadSection);
    ASSERT(pEndRoadSection);
	if( !pStartRoadSection || !pEndRoadSection )
	{
		//������ж�زŻ���ز�������
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
	}
    //////////////////////////////////////////////////////////////////////////
    //������ʼNODE����ֹNODE
	pStartNode = new CGIS_NetworkNode;
	pStartNode->m_pRut = pStartRoadSection;
	pStartNode->m_pVtx = pStartVtx;
    pEndNode = new CGIS_NetworkNode;
	pEndNode->m_pRut = pEndRoadSection;
	pEndNode->m_pVtx = pEndVtx;
    //////////////////////////////////////////////////////////////////////////

      
    //////////////////////////////////////////////////////////////////////////
    //----�����յ���С���ڵĴ���----
    if(!LeaveResidentialArea(
        pTactic->pOpenListForHeadSearchExitOfResidentialArea,
        pTactic->pClosListForHeadSearchExitOfResidentialArea,
        pMap,pPathInfo,pStartNode,pEndVtx,pWPTwo,TRUE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    if(!LeaveResidentialArea(
        pTactic->pOpenListForTailSearchExitOfResidentialArea,
        pTactic->pClosListForTailSearchExitOfResidentialArea,
        pMap,pPathInfo,pEndNode,pStartVtx,pWPOne,FALSE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    //////////////////////////////////////////////////////////////////////////
   
    //////////////////////////////////////////////////////////////////////////
    //��Դ����
    CalculateNavigatePath_SingleSourceSearch(pMap,pPathInfo,pStartNode,pEndNode,pWPOne,pWPTwo,bFind,pTactic);
    
    //////////////////////////////////////////////////////////////////////////
    ///����·�β����ռ�����
Label_FullPathData:
    ASSERT(bFind);
    if( bFind )
    {
        CPtrList RutList, VtxList;
        ASSERT( pStartNode && pEndNode );
        DOUBLE_SRC_SEARCH_BACKDATE_PATH(&RutList,&VtxList,pStartNode,pEndNode);
        FullPathLegData( pWPOne, pWPTwo, &RutList, &VtxList, pMap, pPathPointList );
    }
                

    
//    if( !bFind )
//        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_UNDEFINE));
    
    return VALUE_TRUE;
}

CGIS_MapBreadthNetwork::enBoolValue CGIS_MapBreadthNetwork::CalculateNavigatePathMidLeg( 
    CGIS_WayPoint *pWPOne, 
    CGIS_WayPoint *pWPTwo,
    CGIS_NetworkPathInfo *pPathInfo,
    CGIS_GeoMap *pMap,
    CPtrList *pPathPointList )
{
    //////////////////////////////////////////////////////////////////////////
    //դ��
    ASSERT(pWPOne);
    ASSERT(pWPTwo);
    ASSERT(pPathInfo);
    ASSERT(pMap);
    ASSERT(pPathPointList);
    
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkNode* pStartNode    = NULL;
    CGIS_NetworkNode* pEndNode      = NULL;
    stuTacticOfSearchPathHead* pTactic = &m_stTacticOfSearchPathHead;
    
    
    //////////////////////////////////////////////////////////////////////////
    CPtrList aHeadForbidNodeList;
    CPtrList aTailForbidNodeList;
    pTactic->stParaForSearchUpNode.pHeadForbidNodeList = &aHeadForbidNodeList;
    pTactic->stParaForSearchUpNode.pTailForbidNodeList = &aTailForbidNodeList;
    //////////////////////////////////////////////////////////////////////////
    
step1:
    BOOL bFind = FALSE;
    
    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ�ڵ��Ŀ��ڵ�
    CGIS_NetworkVtx *pStartVtx = NULL;
    CGIS_NetworkVtx *pEndVtx = NULL;
    GetRealVtx( pWPOne, pWPTwo, pMap, &pStartVtx, &pEndVtx );
    if( pStartVtx==NULL || pEndVtx==NULL )
    {
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ·�κ���ֹ·��
    EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    CGIS_NetworkRut* pStartRoadSection = GetNetworkRut( pWPOne->GetWPRutNID(eMapType), pWPOne->GetWPRutID(eMapType) );
    CGIS_NetworkRut* pEndRoadSection = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType) );
    ASSERT(pStartRoadSection);
    ASSERT(pEndRoadSection);
    if( !pStartRoadSection || !pEndRoadSection )
    {
        //������ж�زŻ���ز�������
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    //////////////////////////////////////////////////////////////////////////
    //������ʼNODE����ֹNODE
    pStartNode = new CGIS_NetworkNode;
    pStartNode->m_pRut = pStartRoadSection;
    pStartNode->m_pVtx = pStartVtx;
    pEndNode = new CGIS_NetworkNode;
    pEndNode->m_pRut = pEndRoadSection;
    pEndNode->m_pVtx = pEndVtx;
    //////////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////////
    //----�����յ���С���ڵĴ���----
    if(!LeaveResidentialArea(
        pTactic->pOpenListForHeadSearchExitOfResidentialArea,
        pTactic->pClosListForHeadSearchExitOfResidentialArea,
        pMap,pPathInfo,pStartNode,pEndVtx,pWPTwo,TRUE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    if(!LeaveResidentialArea(
        pTactic->pOpenListForTailSearchExitOfResidentialArea,
        pTactic->pClosListForTailSearchExitOfResidentialArea,
        pMap,pPathInfo,pEndNode,pStartVtx,pWPOne,FALSE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    //////////////////////////////////////////////////////////////////////////
     
    
    //-------------------------------------------------------------
    //�趨�����
    pStartVtx = GetVAbutVtx( pStartVtx );
    if( pStartVtx == NULL )
    {
        delete pStartNode;
        delete pEndNode;
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    
    pEndVtx = GetVAbutVtx( pEndVtx );
    if( pEndVtx == NULL )
    {
        delete pStartNode;
        delete pEndNode;
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    //-------------------------------------------------------------
    
    
    {
        
        CPtrList* pSOList = pTactic->pOpenListForSearchMidNormal;
        CPtrList* pSCList = pTactic->pClosListForSearchMidNormal;
        CPtrList* pEOList = pTactic->pOpenListForSearchMidReverse;
        CPtrList* pECList = pTactic->pClosListForSearchMidReverse;
          
        TRY
        {            
            pSOList->AddTail( pStartNode );
            pEOList->AddTail( pEndNode );
            
            BYTE byRLevel = 1;
            CGIS_NetworkNode *pNode = NULL;
            bFind = CalculateOptimalPathMid(
                pSOList, 
                pSCList,
                pEOList,
                pECList,
                pPathInfo,
                byRLevel, 
                &pStartNode,
                &pEndNode,
                &m_stTacticOfSearchPathHead,
                TRUE );
            
            ASSERT(bFind);
            
        }
        CATCH(CGIS_NetworkException,e)//�����쳣
        {          
            THROW_LAST(e);
        }
        END_CATCH
    }
        
    //////////////////////////////////////////////////////////////////////////
    ///����·�β����ռ�����
Label_FullPathData:
    ASSERT(bFind);
    if( bFind )
    {
        CPtrList RutList, VtxList;
        ASSERT( pStartNode && pEndNode );
        DOUBLE_SRC_SEARCH_BACKDATE_PATH(&RutList,&VtxList,pStartNode,pEndNode);        
        FullPathLegData( pWPOne, pWPTwo, &RutList, &VtxList, pMap, pPathPointList );
    }
    //////////////////////////////////////////////////////////////////////////
    
    
    //ClearInvalNetwork( m_pSLBID, m_nSLBNum, m_nMaxSLBNum, m_pELBID, m_nELBNum, m_nMaxELBNum );      
	return VALUE_TRUE;
}

/**
 * @brief 
 *
 * @param 
 * @return 1-��ʾ�ҵ� ����ֵ-��ʾ����
*/
CGIS_MapBreadthNetwork::enBoolValue CGIS_MapBreadthNetwork::CalculateNavigatePathLongLeg( 
    CGIS_WayPoint *pWPOne, 
    CGIS_WayPoint *pWPTwo, 
    CGIS_NetworkPathInfo *pPathInfo, 
    CGIS_GeoMap *pMap, 
    CPtrList *pPathPointList )
{

    //////////////////////////////////////////////////////////////////////////
    //դ��
	ASSERT(pWPOne);
	ASSERT(pWPTwo);
	ASSERT(pPathInfo);
	ASSERT(pMap);
	ASSERT(pPathPointList);

    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkNode* pStartNode    = NULL;
    CGIS_NetworkNode* pEndNode      = NULL;
    stuTacticOfSearchPathHead* pTactic = &m_stTacticOfSearchPathHead;

    //////////////////////////////////////////////////////////////////////////
    CPtrList aHeadForbidNodeList;
    CPtrList aTailForbidNodeList;
    pTactic->stParaForSearchUpNode.pHeadForbidNodeList = &aHeadForbidNodeList;
    pTactic->stParaForSearchUpNode.pTailForbidNodeList = &aTailForbidNodeList;
    //////////////////////////////////////////////////////////////////////////

step1:
	BOOL bFind = FALSE;

    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ�ڵ��Ŀ��ڵ�
	CGIS_NetworkVtx *pStartVtx = NULL;
	CGIS_NetworkVtx *pEndVtx = NULL;
	GetRealVtx( pWPOne, pWPTwo, pMap, &pStartVtx, &pEndVtx );
	if( pStartVtx==NULL || pEndVtx==NULL )
    {
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ·�κ���ֹ·��
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    CGIS_NetworkRut* pStartRoadSection = GetNetworkRut( pWPOne->GetWPRutNID(eMapType), pWPOne->GetWPRutID(eMapType) );
    CGIS_NetworkRut* pEndRoadSection = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType) );
    ASSERT(pStartRoadSection);
    ASSERT(pEndRoadSection);
	if( !pStartRoadSection || !pEndRoadSection )
	{
		//������ж�زŻ���ز�������
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
	}
    //////////////////////////////////////////////////////////////////////////
    //������ʼNODE����ֹNODE
	pStartNode = new CGIS_NetworkNode;
	pStartNode->m_pRut = pStartRoadSection;
	pStartNode->m_pVtx = pStartVtx;
    pEndNode = new CGIS_NetworkNode;
	pEndNode->m_pRut = pEndRoadSection;
	pEndNode->m_pVtx = pEndVtx;
    //////////////////////////////////////////////////////////////////////////

      
    //////////////////////////////////////////////////////////////////////////
    //----�����յ���С���ڵĴ���----
    if(!LeaveResidentialArea(
        pTactic->pOpenListForHeadSearchExitOfResidentialArea,
        pTactic->pClosListForHeadSearchExitOfResidentialArea,
        pMap,pPathInfo,pStartNode,pEndVtx,pWPTwo,TRUE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    if(!LeaveResidentialArea(
        pTactic->pOpenListForTailSearchExitOfResidentialArea,
        pTactic->pClosListForTailSearchExitOfResidentialArea,
        pMap,pPathInfo,pEndNode,pStartVtx,pWPOne,FALSE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    //----��ȡ��ͷ���м���Ĳ���----
    pTactic->nRLevel[0] = 3;
    pTactic->nRLevel[1] = 3;
    pTactic->nRLevel[2] = 3;
    pTactic->nLoadNetworkFlag[0] = 1; 
    pTactic->nLoadNetworkFlag[1] = 1; 
    pTactic->nLoadNetworkFlag[2] = 2; 
    pTactic->stParaForSearchUpNode.nCountOfLinkedHignRoadSection = 1;
    pTactic->stParaForSearchUpNode.nCountOfAppropriateNodeUpperLimit = 16;
    pTactic->stParaForSearchUpNode.nCountOfAppropriateNodeLowerLimit = 10;
    pTactic->eCalculateSceneForSearchHead = SCENE_NORMAL;
    pTactic->eCalculateSceneForSearchMid = SCENE_NORMAL;
    CalculateNavigatePath_DoubleSourceSearch(pMap,pPathInfo,pStartNode,pEndNode,pWPOne,pWPTwo,bFind,pTactic);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    ///����·�β����ռ�����
Label_FullPathData:
    ASSERT(bFind);
    if( bFind )
    {
        CPtrList RutList, VtxList;
        ASSERT( pStartNode && pEndNode );
        DOUBLE_SRC_SEARCH_BACKDATE_PATH(&RutList,&VtxList,pStartNode,pEndNode);        
        FullPathLegData( pWPOne, pWPTwo, &RutList, &VtxList, pMap, pPathPointList );
    }
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    CLEARNODELISTANDRESETVERTEXSTATE(&aHeadForbidNodeList);
    CLEARNODELISTANDRESETVERTEXSTATE(&aTailForbidNodeList);
    //////////////////////////////////////////////////////////////////////////
    
//	if(!bFind )
//    {
//        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
//    }
	return VALUE_TRUE;
}

CGIS_MapBreadthNetwork::enBoolValue CGIS_MapBreadthNetwork::CalculateNavigatePathMidLongLeg( 
    CGIS_WayPoint *pWPOne, 
    CGIS_WayPoint *pWPTwo,
    CGIS_NetworkPathInfo *pPathInfo, 
    CGIS_GeoMap *pMap, 
    CPtrList *pPathPointList )
{

    //////////////////////////////////////////////////////////////////////////
    //դ��
	ASSERT(pWPOne);
	ASSERT(pWPTwo);
	ASSERT(pPathInfo);
	ASSERT(pMap);
	ASSERT(pPathPointList);

    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkNode* pStartNode    = NULL;
    CGIS_NetworkNode* pEndNode      = NULL;
    stuTacticOfSearchPathHead* pTactic = &m_stTacticOfSearchPathHead;
	

    //////////////////////////////////////////////////////////////////////////
    CPtrList aHeadForbidNodeList;
    CPtrList aTailForbidNodeList;
    pTactic->stParaForSearchUpNode.pHeadForbidNodeList = &aHeadForbidNodeList;
    pTactic->stParaForSearchUpNode.pTailForbidNodeList = &aTailForbidNodeList;
    //////////////////////////////////////////////////////////////////////////

step1:
	BOOL bFind = FALSE;

    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ�ڵ��Ŀ��ڵ�
	CGIS_NetworkVtx *pStartVtx = NULL;
	CGIS_NetworkVtx *pEndVtx = NULL;
	GetRealVtx( pWPOne, pWPTwo, pMap, &pStartVtx, &pEndVtx );
	if( pStartVtx==NULL || pEndVtx==NULL )
    {
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ·�κ���ֹ·��
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    CGIS_NetworkRut* pStartRoadSection = GetNetworkRut( pWPOne->GetWPRutNID(eMapType), pWPOne->GetWPRutID(eMapType) );
    CGIS_NetworkRut* pEndRoadSection = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType) );
    ASSERT(pStartRoadSection);
    ASSERT(pEndRoadSection);
	if( !pStartRoadSection || !pEndRoadSection )
	{
		//������ж�زŻ���ز�������
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
	}
    //////////////////////////////////////////////////////////////////////////
    //������ʼNODE����ֹNODE
	pStartNode = new CGIS_NetworkNode;
	pStartNode->m_pRut = pStartRoadSection;
	pStartNode->m_pVtx = pStartVtx;
    pEndNode = new CGIS_NetworkNode;
	pEndNode->m_pRut = pEndRoadSection;
	pEndNode->m_pVtx = pEndVtx;
    //////////////////////////////////////////////////////////////////////////

      
    //////////////////////////////////////////////////////////////////////////
    //----�����յ���С���ڵĴ���----
    if(!LeaveResidentialArea(
        pTactic->pOpenListForHeadSearchExitOfResidentialArea,
        pTactic->pClosListForHeadSearchExitOfResidentialArea,
        pMap,pPathInfo,pStartNode,pEndVtx,pWPTwo,TRUE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    if(!LeaveResidentialArea(
        pTactic->pOpenListForTailSearchExitOfResidentialArea,
        pTactic->pClosListForTailSearchExitOfResidentialArea,
        pMap,pPathInfo,pEndNode,pStartVtx,pWPOne,FALSE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    //----��ȡ��ͷ���м���Ĳ���----
    pTactic->nRLevel[0] = 4;
    pTactic->nRLevel[1] = 4;
    pTactic->nRLevel[2] = 4;
    pTactic->nLoadNetworkFlag[0] = 1; 
    pTactic->nLoadNetworkFlag[1] = 1; 
    pTactic->nLoadNetworkFlag[2] = 3; 
    pTactic->stParaForSearchUpNode.nCountOfLinkedHignRoadSection = 2;
    pTactic->stParaForSearchUpNode.nCountOfAppropriateNodeUpperLimit = 4;
    pTactic->stParaForSearchUpNode.nCountOfAppropriateNodeLowerLimit = 0;  
    pTactic->eCalculateSceneForSearchHead = SCENE_NORMAL;
    pTactic->eCalculateSceneForSearchMid = SCENE_FAR;
    CalculateNavigatePath_DoubleSourceSearch(pMap,pPathInfo,pStartNode,pEndNode,pWPOne,pWPTwo,bFind,pTactic);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    ///����·�β����ռ�����
Label_FullPathData:
    ASSERT(bFind);
    if( bFind )
    {
        CPtrList RutList, VtxList;
        ASSERT( pStartNode && pEndNode );
        DOUBLE_SRC_SEARCH_BACKDATE_PATH(&RutList,&VtxList,pStartNode,pEndNode);        
        FullPathLegData( pWPOne, pWPTwo, &RutList, &VtxList, pMap, pPathPointList );
    }
    //////////////////////////////////////////////////////////////////////////



    //////////////////////////////////////////////////////////////////////////
    //�������NODE���������нڵ��״̬����������һ�β���ʹ��
    CLEARNODELISTANDRESETVERTEXSTATE(&aHeadForbidNodeList);
    CLEARNODELISTANDRESETVERTEXSTATE(&aTailForbidNodeList);
    //////////////////////////////////////////////////////////////////////////
    
//	if(!bFind )
//    {
//        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
//    }
	return VALUE_TRUE;
}

CGIS_MapBreadthNetwork::enBoolValue CGIS_MapBreadthNetwork::CalculateNavigatePathBigLongLeg(
         CGIS_WayPoint *pWPOne, 
         CGIS_WayPoint *pWPTwo, 
         CGIS_NetworkPathInfo *pPathInfo, 
         CGIS_GeoMap *pMap,
         CPtrList *pPathPointList)
{
    //////////////////////////////////////////////////////////////////////////
    //դ��
	ASSERT(pWPOne);
	ASSERT(pWPTwo);
	ASSERT(pPathInfo);
	ASSERT(pMap);
	ASSERT(pPathPointList);

    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkNode* pStartNode    = NULL;
    CGIS_NetworkNode* pEndNode      = NULL;
    stuTacticOfSearchPathHead* pTactic = &m_stTacticOfSearchPathHead;
	

    //////////////////////////////////////////////////////////////////////////
    CPtrList aHeadForbidNodeList;
    CPtrList aTailForbidNodeList;
    pTactic->stParaForSearchUpNode.pHeadForbidNodeList = &aHeadForbidNodeList;
    pTactic->stParaForSearchUpNode.pTailForbidNodeList = &aTailForbidNodeList;
    //////////////////////////////////////////////////////////////////////////

step1:
	BOOL bFind = FALSE;

    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ�ڵ��Ŀ��ڵ�
	CGIS_NetworkVtx *pStartVtx = NULL;
	CGIS_NetworkVtx *pEndVtx = NULL;
	GetRealVtx( pWPOne, pWPTwo, pMap, &pStartVtx, &pEndVtx );
	if( pStartVtx==NULL || pEndVtx==NULL )
    {
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    //////////////////////////////////////////////////////////////////////////
    //ȡ����ʼ·�κ���ֹ·��
	EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    CGIS_NetworkRut* pStartRoadSection = GetNetworkRut( pWPOne->GetWPRutNID(eMapType), pWPOne->GetWPRutID(eMapType) );
    CGIS_NetworkRut* pEndRoadSection = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType) );
    ASSERT(pStartRoadSection);
    ASSERT(pEndRoadSection);
	if( !pStartRoadSection || !pEndRoadSection )
	{
		//������ж�زŻ���ز�������
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
	}
    //////////////////////////////////////////////////////////////////////////
    //������ʼNODE����ֹNODE
	pStartNode = new CGIS_NetworkNode;
	pStartNode->m_pRut = pStartRoadSection;
	pStartNode->m_pVtx = pStartVtx;
    pEndNode = new CGIS_NetworkNode;
	pEndNode->m_pRut = pEndRoadSection;
	pEndNode->m_pVtx = pEndVtx;
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    //----�����յ���С���ڵĴ���----
    if(!LeaveResidentialArea(
        pTactic->pOpenListForHeadSearchExitOfResidentialArea,
        pTactic->pClosListForHeadSearchExitOfResidentialArea,
        pMap,pPathInfo,pStartNode,pEndVtx,pWPTwo,TRUE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    if(!LeaveResidentialArea(
        pTactic->pOpenListForTailSearchExitOfResidentialArea,
        pTactic->pClosListForTailSearchExitOfResidentialArea,
        pMap,pPathInfo,pEndNode,pStartVtx,pWPOne,FALSE,bFind))
    {
        if(bFind)
            goto Label_FullPathData;
    }
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    //----��ȡ��ͷ���м���Ĳ���----
    {
        CPtrList pathMid;
        TRY
        {            
            pTactic->nRLevel[0] = 4;
            pTactic->nRLevel[1] = 4;
            pTactic->nRLevel[2] = 4;
            pTactic->nLoadNetworkFlag[0] = 1; 
            pTactic->nLoadNetworkFlag[1] = 1; 
            pTactic->nLoadNetworkFlag[2] = 3; 
            pTactic->stParaForSearchUpNode.nCountOfLinkedHignRoadSection = 2;
            pTactic->stParaForSearchUpNode.nCountOfAppropriateNodeUpperLimit = 4;
            pTactic->stParaForSearchUpNode.nCountOfAppropriateNodeLowerLimit = 0;
            pTactic->eCalculateSceneForSearchHead = SCENE_NORMAL;
            pTactic->eCalculateSceneForSearchMid = SCENE_FAR;

             BOOL bFindInTrunk = FALSE;
            CalculateNavigatePath_DoubleSourceSearchInTrunkNetworkV2(pMap,pPathInfo,pStartNode,pEndNode,pWPOne,pWPTwo,bFind,pTactic,&pathMid,bFindInTrunk);

            if( bFind )
            {
                if(bFindInTrunk)
                {
                    FullPathHeadTailData( pMap, pWPOne, pWPTwo, pStartNode, pEndNode, &pathMid, pPathPointList );
                    goto Label_Exit;
                }
                else
                {
                    goto Label_FullPathData;
                }
            }
        }
        CATCH(CGIS_NetworkException,e)
        {
            THROW_LAST();
        }
        END_CATCH

    }
    //////////////////////////////////////////////////////////////////////////
    ///����·�β����ռ�����
Label_FullPathData:
    ASSERT(bFind);
    if( bFind )
    {
        CPtrList RutList, VtxList;
        ASSERT( pStartNode && pEndNode );
        DOUBLE_SRC_SEARCH_BACKDATE_PATH(&RutList,&VtxList,pStartNode,pEndNode);        
        FullPathLegData( pWPOne, pWPTwo, &RutList, &VtxList, pMap, pPathPointList );
    }
    //////////////////////////////////////////////////////////////////////////

Label_Exit:
    //////////////////////////////////////////////////////////////////////////
    //�������NODE���������нڵ��״̬����������һ�β���ʹ��
    CLEARNODELISTANDRESETVERTEXSTATE(&aHeadForbidNodeList);
    CLEARNODELISTANDRESETVERTEXSTATE(&aTailForbidNodeList);
    //////////////////////////////////////////////////////////////////////////
   
//	if(!bFind )
//    {
//        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
//    }
	return VALUE_TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/**
 * @brief �ϸߵȼ�·�����㷨,��ȷ�˷���ֵ������
 * ����TRUE��ʾ�ҵ��˸����������ϸ��ٵĵ� ����FALSE��ʾû�ҵ������������ϸ��ٵĵ�.
*/

CGIS_MapBreadthNetwork::enBoolValue CGIS_MapBreadthNetwork::CalculateOptimalPathHead( 
        CPtrList *              pOpenList, 
        CPtrList *              pCloseList,
        CGIS_NetworkVtx *       pVtx,
        CGIS_NetworkPathInfo *  pPathInfo,
        BYTE                    nRLevel,   //��·����͹��ܼ���
        BOOL                    bHead,     //�������һ��Ƿ�������
        BOOL                    &bFind,   //�Ƿ��ҵ�Ŀ���
        stuTacticOfSearchPathHead* pTactic)
{
	int nResult = 0;
	ASSERT( pOpenList->GetCount() > 0 );

    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup = NULL;
	CGIS_NetworkNode *pNode = (CGIS_NetworkNode*)pOpenList->GetHead();
    //////////////////////////////////////////////////////////////////////////
     DWORD dwDistOfStart2Dest = CalcDist(pNode->m_pVtx->GetVtxPosition().dx,pNode->m_pVtx->GetVtxPosition().dy,
        pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy)*1.0;
    int nFindCount = 0;

    //////////////////////////////////////////////////////////////////////////
   ST_GEO_PTXY ptProject;
    ST_GEO_PTXY ptLineStart = pNode->m_pVtx->GetVtxPosition();
    ST_GEO_PTXY ptLineEnd = pVtx->GetVtxPosition();
    //////////////////////////////////////////////////////////////////////////
    ST_GEO_PTXY PtList[2];
    PtList[0] = ptLineStart;
    PtList[1] = ptLineEnd;
        
    ST_GEO_PTXY pPtBuf[1024];
    int nPtBuf = 0;
    ST_GEO_PTXY *pPtList = PtList;
    int nCont = 2;
    if( nCont > 0 )
    {
        double dbWidth = fabs(pNode->m_pVtx->GetVtxPosition().dx - pVtx->GetVtxPosition().dx) + 
            fabs(pNode->m_pVtx->GetVtxPosition().dy - pVtx->GetVtxPosition().dy);
        CalcLineBuff( dbWidth/3.3, pPtList, nCont, pPtBuf, nPtBuf );
    }

    //////////////////////////////////////////////////////////////////////////
    
	CGIS_NetworkNode *pNextNode = pNode;
	CGIS_NetworkNode *pNTem = NULL;
	BOOL bSearchStartNodeSign = TRUE;
    
    //////////////////////////////////////////////////////////////////////////
    BOOL bFindAppropriateNode = FALSE;

	while( 
        (pNode->m_pRut->GetRutLevel() < nRLevel ||  //��ǰnode֮·�εĹ��ܼ���С���޶��������.
        pNode->m_pRut->GetRutType() == 4 ||         //��ǰ·����
        pNode->m_pRut->GetRutType() == 1 ||
		pNode->m_pVtx->GetVtxEdge() == 1 ||
        !bFindAppropriateNode )
        &&
        !bFind )
	{
#ifdef _ANALYSE_CALCULATE_PATH_PROCESS
//        DUMPSTATE(pNode);
//        if(pNode->m_pRut->GetRutLevel() < nRLevel)
//        {
//            TRACE2("��ǰ·�εĵȼ�����:%d<%d\n",pNode->m_pRut->GetRutLevel(), nRLevel);
//        } 
//        else if(pNode->m_pRut->GetRutType() == 4)
//        {
//            TRACE0("��ǰ·���ǽ���·��·��\n");
//        }
//        else if(pNode->m_pRut->GetRutType() == 1)
//        {
//            TRACE0("��ǰ·����δ�ɼ�·��\n");
//        }
//        else if(pNode->m_pVtx->GetVtxEdge() == 1)
//        {
//            TRACE0("��ǰ�ڵ��Ǳ߽�ڵ�\n");
//        }
//        else if(!bFindAppropriateNode)
//        {
//            TRACE0("��ǰ�ڵ��Ǳ߽�ڵ�\n");
//        }
//        else
//        {
//            ASSERT(FALSE);
//        }
        
#endif   
            
        if(CGIS_NetworkException::ERROR_NONE != m_byErrCode )
        {    
            nResult = m_byErrCode;
            break;           
        }
		if( pNode )
		{
            GET_LIMIT_INFO_V3(pNode,bHead,pLimitInfoGroup);
        }
        //��open���Ƴ�һ���ڵ�ȥ����.
        BOOL bRtn = CalculateOptimalPathRut(
            pOpenList,
            pCloseList, 
            pVtx, 
            pVtx,
            pPathInfo,
            pLimitInfoGroup, 
            1,
            bHead,
            bSearchStartNodeSign );

		if( bRtn)
        {
            ASSERT(!bFind);
			bFind = TRUE;
        }

        //////////////////
		bSearchStartNodeSign = FALSE;
		if( !pOpenList->IsEmpty() )
		{
            //open���յĻ����õ�ǰ���ŵ�
            //��һ���ڵ�Ϊ��ǰ���ŵ�ĸ�NODE
            //
			pNode = (CGIS_NetworkNode*)pOpenList->GetHead();
			if( pNode->m_pParent )
				pNextNode = pNode->m_pParent;
			else
				pNextNode = pNode;
                        
            if(pNextNode->m_nLRNum >= pTactic->stParaForSearchUpNode.nCountOfLinkedHignRoadSection && !pNextNode->m_bHaveJCT)
    {
        //////////////////////////////////////////////////////////////////////////
        //������NODE���ѱ�������NODE,��ô������
        BOOL bForbid = FALSE;
        {                   
            
            CPtrList* pForbidNodeList = NULL;
            if(bHead)
            {
                        pForbidNodeList = pTactic->stParaForSearchUpNode.pHeadForbidNodeList;
            }
            else
            {
                        pForbidNodeList = pTactic->stParaForSearchUpNode.pTailForbidNodeList;
            }
            if(pForbidNodeList)
            {
                POSITION pos = pForbidNodeList->GetHeadPosition();
                while(pos)
                {
                    CGIS_NetworkNode* pForbidNode = (CGIS_NetworkNode*)pForbidNodeList->GetNext(pos);
                    if(pNextNode->m_pRut == pForbidNode->m_pRut && pNextNode->m_pVtx == pForbidNode->m_pVtx)
                    {
                        bForbid = TRUE;                                
                        TRACE0("Node������.\n"); 
                        DUMPSTATE(pNextNode);
                        break;
                    }
                }
            }
            
        }
                if(bForbid)
                    continue;
        //////////////////////////////////////////////////////////////////////////
                
                if(pTactic->stParaForSearchUpNode.bProjectOnLine )
        {
                    if(0 != DisPointToLine(pNextNode->m_pVtx->GetVtxPosition(),ptLineStart,ptLineEnd,ptProject))
            {
                        continue;
                    }
                }
                //////////////////////////////////////////////////////////////////////////
                DWORD dwDist = CalcDist(
                    pNextNode->m_pVtx->GetVtxPosition().dx,
                    pNextNode->m_pVtx->GetVtxPosition().dy,
                    pVtx->GetVtxPosition().dx
                    ,pVtx->GetVtxPosition().dy);
                if(dwDist > dwDistOfStart2Dest )
                    continue;
                //////////////////////////////////////////////////////////////////////////
                nFindCount++;
                if(nFindCount > pTactic->stParaForSearchUpNode.nCountOfAppropriateNodeUpperLimit)
                {
                    break;
                }  
                //////////////////////////////////////////////////////////////////////////
                if(!PointInPoly(pNextNode->m_pVtx->GetVtxPosition(),pPtBuf,nPtBuf))
                    continue;
                
                    bFindAppropriateNode = TRUE; 
                break;
                }
            }
            else
            {
			break;
        }
	}
    
#ifdef _DEBUG
    if(bFindAppropriateNode)
    {
        TRACE0("�ҵ����������Ľ���ߵȼ�·����Node\n");
        CGIS_NetworkNode* pAppropriateNode = (CGIS_NetworkNode*)pOpenList->GetHead();
        pAppropriateNode = pAppropriateNode->m_pParent;
        DUMPSTATE(pAppropriateNode);
        TRACE0("----------------\n");
    }
    else
    {
        TRACE0("δ�ҵ����������Ľ���ߵȼ�·����Node\n");
            }

#endif	
    return bFindAppropriateNode?VALUE_TRUE:VALUE_FALSE;
        }


/**
 * @brief �ϸߵȼ�·�����㷨,��ȷ�˷���ֵ������
 * ����TRUE��ʾ�ҵ��˸����������ϸ��ٵĵ� ����FALSE��ʾû�ҵ������������ϸ��ٵĵ�.
*/

CGIS_MapBreadthNetwork::enBoolValue CGIS_MapBreadthNetwork::CalculateOptimalPathHeadForUpToTrunk( 
        CPtrList *              pOpenList, 
        CPtrList *              pCloseList,
        CGIS_NetworkVtx *       pVtx,
        CGIS_NetworkPathInfo *  pPathInfo,
        BYTE                    nRLevel,   //��·����͹��ܼ���
        BOOL                    bHead,     //�������һ��Ƿ�������
        BOOL                    &bFind,   //�Ƿ��ҵ�Ŀ���
        stuTacticOfSearchPathHead* pTactic)
{
    ASSERT(m_pTrunkNetwork);
    if(!m_pTrunkNetwork)
        return VALUE_FALSE;
    
	int nResult = 0;
	ASSERT( pOpenList->GetCount() > 0 );
	unsigned short *pLRList = NULL;
	BYTE byLRNum = 0;
    CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup = NULL;
	CGIS_NetworkNode *pNode = (CGIS_NetworkNode*)pOpenList->GetHead();

//    DWORD dwDistOfStart2Dest = CalcDist(pNode->m_pVtx->GetVtxPosition().dx,pNode->m_pVtx->GetVtxPosition().dy,
//        pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy)*0.8;
    int nFindCount = 0;

    //////////////////////////////////////////////////////////////////////////
    ST_GEO_PTXY ptProject;
    ST_GEO_PTXY ptLineStart = pNode->m_pVtx->GetVtxPosition();
    ST_GEO_PTXY ptLineEnd = pVtx->GetVtxPosition();
    //////////////////////////////////////////////////////////////////////////
    ST_GEO_PTXY PtList[2];
    PtList[0] = ptLineStart;
    PtList[1] = ptLineEnd;
        
    ST_GEO_PTXY pPtBuf[1024];
    int nPtBuf = 0;
    ST_GEO_PTXY *pPtList = PtList;
    int nCont = 2;
    if( nCont > 0 )
    {
        double dbWidth = fabs(pNode->m_pVtx->GetVtxPosition().dx - pVtx->GetVtxPosition().dx) + 
            fabs(pNode->m_pVtx->GetVtxPosition().dy - pVtx->GetVtxPosition().dy);
        CalcLineBuff( dbWidth/4, pPtList, nCont, pPtBuf, nPtBuf );
    }

    //////////////////////////////////////////////////////////////////////////
    
	CGIS_NetworkNode *pNextNode = pNode;
	CGIS_NetworkNode *pNTem = NULL;
	BOOL bSearchStartNodeSign = TRUE;
    
    //////////////////////////////////////////////////////////////////////////
    BOOL bFindAppropriateNode = FALSE;
	while( 
        (pNode->m_pRut->GetRutLevel() < nRLevel ||  //��ǰnode֮·�εĹ��ܼ���С���޶��������.
        pNode->m_pRut->GetRutType() == 4 ||         //��ǰ·����
        pNode->m_pRut->GetRutType() == 1 ||
		pNode->m_pVtx->GetVtxEdge() == 1 ||
        !bFindAppropriateNode )
        &&
        !bFind )
	{
#ifdef _ANALYSE_CALCULATE_PATH_PROCESS
//        DUMPSTATE(pNode);
//        if(pNode->m_pRut->GetRutLevel() < nRLevel)
//        {
//            TRACE2("��ǰ·�εĵȼ�����:%d<%d\n",pNode->m_pRut->GetRutLevel(), nRLevel);
//        } 
//        else if(pNode->m_pRut->GetRutType() == 4)
//        {
//            TRACE0("��ǰ·���ǽ���·��·��\n");
//        }
//        else if(pNode->m_pRut->GetRutType() == 1)
//        {
//            TRACE0("��ǰ·����δ�ɼ�·��\n");
//        }
//        else if(pNode->m_pVtx->GetVtxEdge() == 1)
//        {
//            TRACE0("��ǰ�ڵ��Ǳ߽�ڵ�\n");
//        }
//        else if(!bFindAppropriateNode)
//        {
//            TRACE0("��ǰ�ڵ��Ǳ߽�ڵ�\n");
//        }
//        else
//        {
//            ASSERT(FALSE);
//        }
        
#endif   
        
        if(CGIS_NetworkException::ERROR_NONE != m_byErrCode )
        {    
            nResult = m_byErrCode;
            break;           
        }
		if( pNode )
		{
            GET_LIMIT_INFO_V3(pNode,bHead,pLimitInfoGroup);
        }
        //��open���Ƴ�һ���ڵ�ȥ����.
        BOOL bRtn = CalculateOptimalPathRut(
            pOpenList,
            pCloseList, 
            pVtx, 
            pVtx,
            pPathInfo,
            pLimitInfoGroup, 
            1,
            bHead,
            bSearchStartNodeSign );

		if( bRtn)
        {
            ASSERT(!bFind);
			bFind = TRUE;
        }

        //////////////////
		bSearchStartNodeSign = FALSE;
		if( !pOpenList->IsEmpty() )
		{
            //open���յĻ����õ�ǰ���ŵ�
            //��һ���ڵ�Ϊ��ǰ���ŵ�ĸ�NODE
            //
			pNode = (CGIS_NetworkNode*)pOpenList->GetHead();
			if( pNode->m_pParent )
				pNextNode = pNode->m_pParent;
			else
				pNextNode = pNode;
            
            if(pNextNode->m_nLRNum >= pTactic->stParaForSearchUpNode.nCountOfLinkedHignRoadSection && !pNextNode->m_bHaveJCT)
            {
                //////////////////////////////////////////////////////////////////////////
                //������NODE���ѱ�������NODE,��ô������
                BOOL bForbid = FALSE;
                {                   
                    
                    CPtrList* pForbidNodeList = NULL;
                    if(bHead)
                    {
                        pForbidNodeList = pTactic->stParaForSearchUpNode.pHeadForbidNodeList;
                    }
                    else
                    {
                        pForbidNodeList = pTactic->stParaForSearchUpNode.pTailForbidNodeList;
                    }
                    if(pForbidNodeList)
                    {
                        POSITION pos = pForbidNodeList->GetHeadPosition();
                        while(pos)
                        {
                            CGIS_NetworkNode* pForbidNode = (CGIS_NetworkNode*)pForbidNodeList->GetNext(pos);
                            if(pNextNode->m_pRut == pForbidNode->m_pRut && pNextNode->m_pVtx == pForbidNode->m_pVtx)
                            {
                                bForbid = TRUE;                                
                                TRACE0("Node������.\n"); 
                                DUMPSTATE(pNextNode);
                                break;
                            }
                        }
                    }
                    
                }
                if(bForbid)
                    continue;
                //////////////////////////////////////////////////////////////////////////
                if(pTactic->stParaForSearchUpNode.bProjectOnLine)
                {
                    if(0 != DisPointToLine(pNextNode->m_pVtx->GetVtxPosition(),ptLineStart,ptLineEnd,ptProject))
                        continue;
                }
                //////////////////////////////////////////////////////////////////////////               
//                DWORD dwDist = CalcDist(pNextNode->m_pVtx->GetVtxPosition().dx,pNextNode->m_pVtx->GetVtxPosition().dy,
//                    pVtx->GetVtxPosition().dx,pVtx->GetVtxPosition().dy);
//                if(dwDist > dwDistOfStart2Dest)
//                    continue;
                //////////////////////////////////////////////////////////////////////////
                nFindCount++;               
                if(nFindCount > pTactic->stParaForSearchUpNode.nCountOfAppropriateNodeUpperLimit)
                    {
                    break;
                        }
                //////////////////////////////////////////////////////////////////////////
                
                if(!PointInPoly(pNextNode->m_pVtx->GetVtxPosition(),pPtBuf,nPtBuf))
                    continue;
                //////////////////////////////////////////////////////////////////////////
                CGIS_MBBNetwork::enErrorCode eErrorCode;
                if(!m_pTrunkNetwork->IfUpNode(pNextNode->m_pVtx->GetVBreadthID(),pNextNode->m_pVtx->GetVtxID(),&eErrorCode))
                    continue;

                        bFindAppropriateNode = TRUE; 
                break;
            }
		}
		else 
        {
			break;
        }
	}
    
#ifdef _DEBUG
    if(bFindAppropriateNode)
    {
        TRACE0("�ҵ����������Ľ���ߵȼ�·����Node\n");
        CGIS_NetworkNode* pAppropriateNode = (CGIS_NetworkNode*)pOpenList->GetHead();
        pAppropriateNode = pAppropriateNode->m_pParent;
        DUMPSTATE(pAppropriateNode);
        TRACE0("----------------\n");
    }
    else
    {
        TRACE0("δ�ҵ����������Ľ���ߵȼ�·����Node\n");
    }

#endif	
    return bFindAppropriateNode?VALUE_TRUE:VALUE_FALSE;
}


/**
* @brief     ���㺽�����˵��м�·��
* @note �˺������׳� CGIS_NetworkException/CUserException
* @param 
* @return TRUE-�ҵ�Ŀ���.FALSE-Ŀǰ�����ܷ��ش�ֵ
* @exception ����δ�ҵ�Ŀ���������׳��쳣����ʽ����δ�ҵ�Ŀ����ԭ��
*/
BOOL CGIS_MapBreadthNetwork::CalculateOptimalPathMid(
    CPtrList *pSOList,
    CPtrList *pSCList, 
    CPtrList *pEOList, 
    CPtrList *pECList, 
    CGIS_NetworkPathInfo *pPathInfo,
    BYTE nRLevel,
    CGIS_NetworkNode **pOutStartNode,
    CGIS_NetworkNode **pOutEndNode,
    stuTacticOfSearchPathHead* pTactic,     //����
    BOOL bSearchStartNodeSign )
{
    ASSERT(pSOList);
    ASSERT(pEOList);
    ASSERT(!pSOList->IsEmpty());
    ASSERT(!pEOList->IsEmpty());
    
    
    //////////////////////////////////////////////////////////////////////////
    BOOL bFindDest = FALSE;
    
    
    CGIS_NetworkNode *pStartNode    = (CGIS_NetworkNode*)pSOList->GetHead();
    CGIS_NetworkNode *pEndNode      = (CGIS_NetworkNode*)pEOList->GetHead();
    ASSERT( pStartNode && pEndNode );
    
    
    int nResult = 0;
    int nHeadExtendCount = 0;
    int nTailExtendCount = 0;
    
    //������Ϣ����
    BYTE bySLRNum = 0;
    BYTE byELRNum = 0;
    
    BOOL bStart = FALSE;
    POSITION ps = NULL;
    CGIS_NetworkNode *pNode = NULL;
    const unsigned short nVNum = 500;

	CGIS_NetworkNode* pHeadNodeArray[nVNum];
	CGIS_NetworkNode* pTailNodeArray[nVNum];
    //����Ŀ���
    unsigned short nCount = 1;
    unsigned short nNum = 0;
    
    
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkVtx *pStartVtx = pStartNode->m_pVtx;
    ASSERT(pStartVtx);
    pStartVtx = GetVAbutVtx( pStartVtx );
    
    CGIS_NetworkVtx *pEndVtx = pEndNode->m_pVtx;
    ASSERT(pEndVtx);
    pEndVtx = GetVAbutVtx( pEndVtx );
    //////////////////////////////////////////////////////////////////////////
    
    
    //////////////////////////////////////////////////////////////////////////
    pHeadNodeArray[0] = pStartNode;
    pTailNodeArray[0] = pEndNode;
    //////////////////////////////////////////////////////////////////////////
    m_byCFlag = 1;
    
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkVtx *pMVtx = NULL;
    TRY
    {
#ifdef _ANALYSE_CALCULATE_PATH_PROCESS
        int nDebugCount = 0;
            DBG2LOG2("EO:%d EC:%d\n",pEOList->GetCount(),pECList->GetCount());
//            pEndNode->LogState();
            DBG2LOG2("SO:%d SC:%d\n",pSOList->GetCount(),pSCList->GetCount());
//            pStartNode->LogState();
#endif      
        while( 
            !pSOList->IsEmpty() && 
            !pEOList->IsEmpty() && 
            !bFindDest )
        {
#ifdef _ANALYSE_CALCULATE_PATH_PROCESS
            DBG2LOG1("nDebugCount:%d\n",nDebugCount);
#endif            
            
            pEndNode = (CGIS_NetworkNode*)pEOList->GetHead();
            nNum = nCount>nVNum?nVNum:nCount;

            if( CalculateOptimalPathRut12(
                pSOList, 
                pSCList, 
                (unsigned short*)pTailNodeArray,
                NULL,         
                nNum,
                pEndVtx,
                pPathInfo, 
                nRLevel,
                TRUE,
                bSearchStartNodeSign ) )
            {
                if( pECList->GetCount() == 0 )
                {
                    bStart = TRUE;
                    bFindDest = TRUE;
                    *pOutEndNode = (CGIS_NetworkNode*)pEOList->GetHead();
                    *pOutStartNode = (CGIS_NetworkNode*)pSOList->GetHead();
                    break;
                }
                pNode = pTailNodeArray[nNum];
                ASSERT(pNode);
                
                bStart = TRUE;
                bFindDest = TRUE;
                *pOutEndNode = pNode;
                *pOutStartNode = (CGIS_NetworkNode*)pSOList->GetHead();
                
                DUMPNODESTATE("���򽻻�ɹ�:",pNode);
                break;
            }
            
            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            m_byCFlag = 2;
            pMVtx = pEndNode->m_pVtx;
            pMVtx = GetVAbutVtx( pMVtx );
            if( pMVtx == NULL )
                break;
            pTailNodeArray[nCount%nVNum] = pEndNode;
            if( pSOList->IsEmpty() )
                break;
            
            pStartNode = (CGIS_NetworkNode*)pSOList->GetHead();

            if( CalculateOptimalPathRut12(
                pEOList,
                pECList,
                (unsigned short*)pHeadNodeArray,
                NULL,              
                nNum,
                pStartVtx, 
                pPathInfo, 
                nRLevel, 
                FALSE, 
                bSearchStartNodeSign ) )
            {
                pNode = pHeadNodeArray[nNum];
                ASSERT(pNode);
                bStart = FALSE;
                bFindDest = TRUE;
                *pOutStartNode = pNode;
                *pOutEndNode = (CGIS_NetworkNode*)pEOList->GetHead();

                DUMPNODESTATE("���򽻻�ɹ�:",pNode);

                break;
            }
            
            m_byCFlag = 1;
            pMVtx = pStartNode->m_pVtx;
            pMVtx = GetVAbutVtx( pMVtx );
            if( pMVtx == NULL )
                break;

            
            pHeadNodeArray[nCount%nVNum] = pStartNode;
            nCount++;
            bSearchStartNodeSign = FALSE;
#ifdef _ANALYSE_CALCULATE_PATH_PROCESS
        int nDebugCount = 0;
            DBG2LOG2("EO:%d EC:%d\n",pEOList->GetCount(),pECList->GetCount());
//            pEndNode->LogState();
            DBG2LOG2("SO:%d SC:%d\n",pSOList->GetCount(),pSCList->GetCount());
//            pStartNode->LogState();
#endif



#ifdef _ANALYSE_CALCULATE_PATH_PROCESS
            nDebugCount++;
#endif
        }

        if(!bFindDest)
        {
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_NETWORK_DISCONNECT));
        }
    }
    CATCH(CGIS_NetworkException,e)
    {
        THROW_LAST(e);
    }
    END_CATCH
        
    return TRUE;
}
BOOL CGIS_MapBreadthNetwork::LeaveResidentialArea(
    CPtrList*   pOpenList, 
    CPtrList*   pCloseList,
    CGIS_GeoMap* pMap ,
    CGIS_NetworkPathInfo* pPathInfo ,
    CGIS_NetworkNode*& pStartNode,
    CGIS_NetworkVtx* pEndVtx,
    CGIS_WayPoint* pWPT,
    BOOL bHead,
    BOOL& bFindDest 
    )
{
    ASSERT(pOpenList);
    ASSERT(pCloseList);
    ASSERT(pOpenList->IsEmpty());
    ASSERT(pOpenList->IsEmpty());

    EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    //////////////////////////////////////////////////////////////////////////
    //----�����յ���С���ڵĴ���----
    if(pStartNode->m_pRut->GetRutLevel() != 0)
        return TRUE;

    pEndVtx = GetVAbutVtx(pEndVtx);
    ASSERT(pEndVtx);
    if(!pEndVtx)
    {
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_OUT_OF_MEMORY));    
    }

    {
        TRACE0("��ǰ����С����,��Ѱ��С������.\n");
        pOpenList->AddTail( pStartNode );
        m_pDestRut = GetNetworkRut( pWPT->GetWPRutNID(eMapType), pWPT->GetWPRutID(eMapType) );
        m_byRLevel = 0;
        if(!SearchExitOfResidentialArea(pOpenList,pCloseList,pEndVtx,pPathInfo,1,bHead,bFindDest,&m_stTacticOfSearchPathHead))
        {
            if(bFindDest)
            {
                pStartNode = (CGIS_NetworkNode*)pOpenList->GetHead();
                return FALSE;
            }
            else //����������ȥ��
            {
                TRACE0("�������С������ʧ��.\n");
                THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_NETWORK_DISCONNECT));
            }
        }
        else //�ҵ���С������
        {
            pStartNode = GetHeadNode(pOpenList,pCloseList,1,1);  
            DUMPNODESTATE("�ҵ��ĵ�ǰ��С������:",pStartNode);
            
            ResetNodeStateInNodeList( pOpenList );
            ResetNodeStateInNodeList( pCloseList );
            pStartNode->m_nLRNum = 0;

            return TRUE;
        }
    }
}

BOOL CGIS_MapBreadthNetwork::CalculateNavigatePath_SingleSourceSearch(
    CGIS_GeoMap* pMap , 
    CGIS_NetworkPathInfo* pPathInfo ,
    CGIS_NetworkNode*& pOriginStartNode,
    CGIS_NetworkNode*& pOriginEndNode,                                                        
    CGIS_WayPoint* pWPOne,
    CGIS_WayPoint* pWPTwo,
    BOOL& bFind,
    stuTacticOfSearchPathHead* pTactic    )
{
    bFind = FALSE;
    CGIS_NetworkNode* pStartNode = new CGIS_NetworkNode;
    pStartNode->m_pRut     = pOriginStartNode->m_pRut;
    pStartNode->m_pVtx     = pOriginStartNode->m_pVtx;
    pStartNode->m_pParent  = pOriginStartNode->m_pParent;
    CGIS_NetworkNode* pEndNode = new CGIS_NetworkNode;
    pEndNode->m_pRut     = pOriginEndNode->m_pRut;
    pEndNode->m_pVtx     = pOriginEndNode->m_pVtx;
    pEndNode->m_pParent  = pOriginEndNode->m_pParent;

     //-------------------------------------------------------------
    //�趨�����    
    CGIS_NetworkVtx* pEndVtx = pEndNode->m_pVtx;
    pEndVtx = GetVAbutVtx( pEndVtx );
    if( pEndVtx == NULL )
    {
        delete pStartNode;
        delete pEndNode;
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }
    //-------------------------------------------------------------

    

    EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    //////////////////////////////////////////////////////////////////////////
    BOOL bSearchStartNodeSign       = TRUE;
    CGIS_NetworkVtx* pDestVertex    = NULL;   
    unsigned int nErrorCode = 0;
    CGIS_NetworkNode* pNode         = NULL;           
    
    CPtrList *pOpenList     = pTactic->pOpenListForSearchMidNormal;
    CPtrList *pCloseList    = pTactic->pClosListForSearchMidNormal;
    
    m_eCalculateScene = SCENE_SHORT;
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //-------------------------̽·��ʼ---------------------------------------
    TRY
    {      
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //��Դ̽·
        pOpenList->AddTail(pStartNode );
        pDestVertex = pEndVtx;
        m_pDestRut = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType) );
        
        while( !pOpenList->IsEmpty() )
        {       
            //-------------------------------------------------------------
            //ȡ���й��� 
            CGIS_NetworkRut::stuRoadSectionLimitInfoGroup* pLimitInfoGroup = NULL;
            {
                pNode = (CGIS_NetworkNode*)pOpenList->GetHead();
                if( pNode )
                {
                    GET_LIMIT_INFO_V3(pNode,TRUE,pLimitInfoGroup);
                }
            }
            if( CalculateOptimalPathRut( 
                pOpenList, 
                pCloseList,
                pDestVertex, 
                pDestVertex,
                pPathInfo, 
                pLimitInfoGroup,
                1, 
                TRUE, 
                bSearchStartNodeSign ) )
            {
                bFind = TRUE;
                pOriginStartNode = (CGIS_NetworkNode*)pOpenList->GetHead();
                break;
            }
            bSearchStartNodeSign = FALSE;
        }

        if(!bFind)
        {
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_NETWORK_DISCONNECT));
        }
     }
    CATCH(CGIS_NetworkException,e)
    {
        THROW_LAST();
    }
    END_CATCH       

    if(bFind)
        return TRUE;

    return FALSE;
}


BOOL CGIS_MapBreadthNetwork::CalculateNavigatePath_DoubleSourceSearch(
    CGIS_GeoMap* pMap , 
    CGIS_NetworkPathInfo* pPathInfo ,
    CGIS_NetworkNode*& pOriginStartNode,
    CGIS_NetworkNode*& pOriginEndNode,                                                        
    CGIS_WayPoint* pWPOne,
    CGIS_WayPoint* pWPTwo,
    BOOL& bFind,
    stuTacticOfSearchPathHead* pTactic    )
{
    CPtrList* pSOList = pTactic->pOpenListForSearchHeadNodeUpToTrunk;
    CPtrList* pSCList = pTactic->pClosListForSearchHeadNodeUpToTrunk;
    CPtrList* pEOList = pTactic->pOpenListForSearchTailNodeUpToTrunk;
    CPtrList* pECList = pTactic->pClosListForSearchTailNodeUpToTrunk;
    CPtrList* pSOList1 = pTactic->pOpenListForSearchMidNormal;
    CPtrList* pSCList1 = pTactic->pClosListForSearchMidNormal;
    CPtrList* pEOList1 = pTactic->pOpenListForSearchMidReverse;
    CPtrList* pECList1 = pTactic->pClosListForSearchMidReverse;

    ClearNWNList(pTactic->stParaForSearchUpNode.pHeadForbidNodeList);
    ClearNWNList(pTactic->stParaForSearchUpNode.pTailForbidNodeList);
    int nRepeatCount = 0;
    CGIS_NetworkNode* pStartNode = NULL;
    CGIS_NetworkNode* pEndNode = NULL;
step1:
    if(nRepeatCount > 4 && bFind)
    {
        ASSERT(pStartNode);
        ASSERT(pEndNode);
        
        pOriginStartNode = pStartNode;
        pOriginEndNode = pEndNode;
        
        return TRUE;    
    }
    ClearNWNList(   pSOList );
    ClearNWNList(   pSCList );
    ClearNWNList(   pEOList );
    ClearNWNList(   pECList );
    ClearNWNList(   pSOList1);
    ClearNWNList(   pSCList1);
    ClearNWNList(   pEOList1);
    ClearNWNList(   pECList1);
    
    bFind = FALSE;
    pStartNode = new CGIS_NetworkNode;
    pStartNode->m_pRut     = pOriginStartNode->m_pRut;
    pStartNode->m_pVtx     = pOriginStartNode->m_pVtx;
    pStartNode->m_pParent  = pOriginStartNode->m_pParent;
    pEndNode = new CGIS_NetworkNode;
    pEndNode->m_pRut     = pOriginEndNode->m_pRut;
    pEndNode->m_pVtx     = pOriginEndNode->m_pVtx;
    pEndNode->m_pParent  = pOriginEndNode->m_pParent;
    
    EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    //////////////////////////////////////////////////////////////////////////
    //----��ȡ��ͷ���м���Ĳ���----


    BOOL bFindHeadNode = FALSE;
    BOOL bFindTailNode = FALSE;


    pSOList->AddTail( pStartNode );
    pEOList->AddTail( pEndNode );
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkVtx* pEndVtx = pEndNode->m_pVtx;
    pEndVtx = GetVAbutVtx(pEndVtx);
    ASSERT(pEndVtx);
    if( pEndVtx == NULL )
    {
        ClearNWNList( pSOList );
        ClearNWNList( pEOList );
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }


    m_eCalculateScene = pTactic->eCalculateSceneForSearchHead;
    BYTE byRLevel = pTactic->nRLevel[0];
    m_byRLevel = pTactic->nRLevel[0];
    //-------------------------------------------------------------
    //���÷���Ϊ��������Ŀ��·��Ϊ��2���ڵ�·��
    m_byCFlag = 1;
    m_pDestRut = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType) );
    //-------------------------------------------------------------

    bFindHeadNode = CalculateOptimalPathHead(
                                             pSOList, 
                                             pSCList, 
                                             pEndVtx,
                                             pPathInfo,
                                             byRLevel, 
                                             TRUE,
                                             bFind,
                                             pTactic);

    if(bFind)
    {
        TRACE0("�����ϸߵȼ�·��ʱ�ҵ�Ŀ��,����ɹ�!");
        pOriginStartNode = (CGIS_NetworkNode*)pSOList->GetHead();
        return TRUE;
    }
    
    {
        //////////////////////////////////////////////////////////////////////////
        //20070718����ж��ϸ��ٵĵ��Ƿ��ڸ���·�����Ŀ���֮��Ĵ���
//        if(bFindHeadNode)
//        {
//            CGIS_NetworkNode* pHeadUpNode = PeekHeadNode( pSOList, pSCList, pTactic->nRLevel[2] );
//            ASSERT(pHeadUpNode);
//            //FindHighNetworkEnterPoint(pHeadUpNode)
//            {
//                CGIS_NetworkNode* pNode = pHeadUpNode;
//                CGIS_NetworkRut* pRoadSectionPrevious = pNode->m_pRut;
//                CGIS_NetworkRut* pRoadSectionNext = NULL;
//                pNode = pNode->m_pParent;
//                while(pNode)
//                {
//                    pRoadSectionNext = pNode->m_pRut;
//
//                    if(
//                        (pRoadSectionPrevious->GetType() == 5 || pRoadSectionPrevious->GetRutType() == 3)
//                        &&
//                        (pRoadSectionNext->GetType() != 5 || pRoadSectionNext->GetRutType() != 3)
//                        )
//                    {
//                        ST_GEO_PTXY ptLineStart = pNode->m_pVtx->GetVtxPosition();
//                        ST_GEO_PTXY ptLineEnd = pEndVtx->GetVtxPosition();
//                        ST_GEO_PTXY ptProject;
//                        if(0 == DisPointToLine(pHeadUpNode->m_pVtx->GetVtxPosition(),ptLineStart,ptLineEnd,ptProject))
//                        {
//                             
//                        }
//                        else
//                        {
//                            CGIS_NetworkNode* pHeadForbidNode = new CGIS_NetworkNode;
//                            pHeadForbidNode->m_pRut = pHeadUpNode->m_pRut;
//                            pHeadForbidNode->m_pVtx = pHeadUpNode->m_pVtx;
//                            pTactic->stParaForSearchUpNode.pHeadForbidNodeList->AddTail(pHeadForbidNode);
//                            
//                            
//                            //�����ͷ���ظ��ҵĻ���Ҫ��β�����ۻ��Ľ��㶼���
//                            ClearNWNList(pTactic->stParaForSearchUpNode.pTailForbidNodeList);
//                            pTactic->stParaForSearchUpNode.pTailForbidNodeList->RemoveAll();
//                            goto step1;                                        
//
//                        }
//
//                    }
//                    pRoadSectionPrevious = pRoadSectionNext;
//                    pNode = pNode->m_pParent;
//                    
//                }
//            }
//        }


        
        //////////////////////////////////////////////////////////////////////////
        
    }

    ASSERT(!bFind);
    {
        if( pSOList->GetCount() == 0 )
        {
            ClearNWNList( pSCList );
            ClearNWNList( pEOList );
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_NETWORK_DISCONNECT));
        }
        
        //��ʱ�����ҵ����ϸ��ٵ�,Ҳ����û���ҵ�.
        
        //ȡ��OPEN���е����ŵ���Ϊ��ʼ�ڵ�.
        TRACE0("��β���ϸߵȼ�·��ʹ�õķ���Node\n");
        DUMPSTATE((CGIS_NetworkNode*)pSOList->GetHead());
        CGIS_NetworkVtx* pStartVtx = ((CGIS_NetworkNode*)pSOList->GetHead())->m_pVtx;
        pStartVtx = GetVAbutVtx( pStartVtx );
        if( pStartVtx == NULL )
        {
            ClearNWNList( pSOList );
            ClearNWNList( pSCList );
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_UNDEFINE));
        }
        
        byRLevel = pTactic->nRLevel[1];
        m_byRLevel = pTactic->nRLevel[1];
        m_byCFlag = 2;//��������
        //////////////////////////////////////////////////////////////////////////
        m_pDestRut = GetNetworkRut( pWPOne->GetWPRutNID(eMapType), pWPOne->GetWPRutID(eMapType) );
        //////////////////////////////////////////////////////////////////////////
        bFindTailNode = CalculateOptimalPathHead( 
            pEOList, 
            pECList,
            pStartVtx, 
            pPathInfo,
            byRLevel, 
            FALSE,
            bFind ,
            pTactic);      
    }
    //////////////////////////////////////////////////////////////////////////
    if( bFind )
    {
        TRACE0("�����ϸߵȼ�·��ʱ�ҵ�Ŀ��,����ɹ�!");
        pOriginStartNode  = (CGIS_NetworkNode*)pSOList->GetHead();
        pOriginEndNode    = (CGIS_NetworkNode*)pEOList->GetHead();
        return TRUE;
    }
     
    ASSERT(!bFind);
    
    if( pSOList->GetCount() <= 0 || pEOList->GetCount() <= 0 )
    {
        //����Ϊ0����·���Ҷ���
        ClearNWNList( pSOList );
        ClearNWNList( pSCList );
        ClearNWNList( pEOList );
        ClearNWNList( pECList );
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_NETWORK_DISCONNECT));
    }
    
    
    byRLevel            = pTactic->nRLevel[2];
    m_LoadNetworkFlag   = pTactic->nLoadNetworkFlag[2];
    pStartNode  = GetHeadNode( pSOList, pSCList, pTactic->nRLevel[2] );
    pEndNode    = GetHeadNode( pEOList, pECList, pTactic->nRLevel[2] );
    
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkNode* pHeadUpNode = NULL;
    CGIS_NetworkNode* pTailUpNode = NULL;
    if(bFindHeadNode)
    {
        pHeadUpNode = pStartNode;
        ASSERT(pHeadUpNode);
    }
    if(bFindTailNode)
    {
        pTailUpNode = pEndNode;
        ASSERT(pTailUpNode);
    }
    //////////////////////////////////////////////////////////////////////////
    
    TRACE0("�ϸ���NODE\n");
    DUMPSTATE(pStartNode);
    TRACE0("�¸���NODE\n");
    DUMPSTATE(pEndNode);
    ASSERT( pStartNode && pEndNode );

    ClearNWHead( pStartNode, pEndNode, pSOList,pSCList,pEOList,pECList );
    
    

    TRACE0("��ʼ˫�򽻻�\n");
    
    pSOList1->AddTail( pStartNode );
    pEOList1->AddTail( pEndNode );
    //AddNWNode( pStartNode, pSCList1 );
    //AddNWNode( pEndNode, pECList1 );
    m_byClear = 1;
    m_eCalculateScene = pTactic->eCalculateSceneForSearchMid;
   

        bFind = CalculateOptimalPathMid(
            pSOList1,
            pSCList1,
            pEOList1,
            pECList1,
            pPathInfo,
            byRLevel,
            &pStartNode,
            &pEndNode,
            pTactic
            );
        
        ASSERT(bFind);
        
        if(bFind)
        {
            //////////////////////////////////////////////////////////////////////////
            switch((bFindHeadNode<<4)|bFindTailNode)
            {
            case 0x00://��������²�����.��Ϊ�޷���ͷ��.
                break;
            case 0x10://ͷ������.
                {
                    TRACE0("��������\n");
                    if(IfRouteHaveRepeatNode(pHeadUpNode,pStartNode,pHeadUpNode))
                    {
                        CGIS_NetworkNode* pHeadForbidNode = new CGIS_NetworkNode;
                        pHeadForbidNode->m_pRut = pHeadUpNode->m_pRut;
                        pHeadForbidNode->m_pVtx = pHeadUpNode->m_pVtx;
                        pTactic->stParaForSearchUpNode.pHeadForbidNodeList->AddTail(pHeadForbidNode);
                                                
                        
                        //�����ͷ���ظ��ҵĻ���Ҫ��β�����ۻ��Ľ��㶼���
                        ClearNWNList(pTactic->stParaForSearchUpNode.pTailForbidNodeList);
                        pTactic->stParaForSearchUpNode.pTailForbidNodeList->RemoveAll();
                        goto step1;                                        
                    }
                }
                break;
            case 0x01://β������
                {
                    TRACE0("��������\n");
                    if(IfRouteHaveRepeatNode(pTailUpNode,pEndNode,pTailUpNode))
                    {
                        CGIS_NetworkNode* pTailForbidNode = new CGIS_NetworkNode;
                        pTailForbidNode->m_pRut = pTailUpNode->m_pRut;
                        pTailForbidNode->m_pVtx = pTailUpNode->m_pVtx;
                        pTactic->stParaForSearchUpNode.pTailForbidNodeList->AddTail(pTailForbidNode);
                        

                        
                        //����Ǵ�β���ظ��ҵĻ���Ҫ��ͷ�����ۻ��Ľ��㶼���
                        ClearNWNList(pTactic->stParaForSearchUpNode.pHeadForbidNodeList);                        
                        pTactic->stParaForSearchUpNode.pHeadForbidNodeList->RemoveAll();
                        goto step1;                        
                    }
                }
                break;
            case 0x11:
                {
                    TRACE0("��������\n");
                    BOOL bHeadRepeat = IfRouteHaveRepeatNode(pHeadUpNode,pStartNode,pHeadUpNode);
                    TRACE0("��������\n");
                    BOOL bTailRepeat = IfRouteHaveRepeatNode(pTailUpNode,pEndNode,pTailUpNode);
                    if(bHeadRepeat && !bTailRepeat) 
                    {
                        CGIS_NetworkNode* pHeadForbidNode = new CGIS_NetworkNode;
                        pHeadForbidNode->m_pRut = pHeadUpNode->m_pRut;
                        pHeadForbidNode->m_pVtx = pHeadUpNode->m_pVtx;
                        pHeadForbidNode->m_pParent = pHeadUpNode->m_pParent;
                        pTactic->stParaForSearchUpNode.pHeadForbidNodeList->AddTail(pHeadForbidNode);
                        
                        
      
                        
                        //�����ͷ���ظ��ҵĻ���Ҫ��β�����ۻ��Ľ��㶼���
                        ClearNWNList(pTactic->stParaForSearchUpNode.pTailForbidNodeList);
                        pTactic->stParaForSearchUpNode.pTailForbidNodeList->RemoveAll();
                        goto step1;                                        
                    }
                    if(bTailRepeat && !bHeadRepeat)
                    {
                        CGIS_NetworkNode* pTailForbidNode = new CGIS_NetworkNode;
                        pTailForbidNode->m_pRut = pTailUpNode->m_pRut;
                        pTailForbidNode->m_pVtx = pTailUpNode->m_pVtx;
                        pTactic->stParaForSearchUpNode.pTailForbidNodeList->AddTail(pTailForbidNode);
                        

                        
                        //����Ǵ�β���ظ��ҵĻ���Ҫ��ͷ�����ۻ��Ľ��㶼���
                        ClearNWNList(pTactic->stParaForSearchUpNode.pHeadForbidNodeList);                        
                        pTactic->stParaForSearchUpNode.pHeadForbidNodeList->RemoveAll();
                        goto step1;                        
                    }
                    
                    if(bHeadRepeat && bTailRepeat)
                    {
                        //��������ݲ�����,��δ�뵽���ʵĲ���.
                        DBG2LOG0("��ע��,�˴ι滮����ͷ����β���������ظ�·��!");
                        //ASSERT(FALSE);                      
                    }                   
                }
                break;
            default:
                ASSERT(FALSE);
                break;
                }      
                

                pOriginStartNode = pStartNode;
                pOriginEndNode = pEndNode;

                return TRUE;
            }
        
           
            
}
BOOL CGIS_MapBreadthNetwork::CalculateNavigatePath_DoubleSourceSearchInTrunkNetwork_DEL(
    CGIS_GeoMap* pMap , 
    CGIS_NetworkPathInfo* pPathInfo ,
    CGIS_NetworkNode*& pOriginStartNode,
    CGIS_NetworkNode*& pOriginEndNode,                                                        
    CGIS_WayPoint* pWPOne,
    CGIS_WayPoint* pWPTwo,
    BOOL& bFind,
    stuTacticOfSearchPathHead* pTactic,
    CPtrList* pPathMid)
{
    CPtrList* pSOList = pTactic->pOpenListForSearchHeadNodeUpToTrunk;
    CPtrList* pSCList = pTactic->pClosListForSearchHeadNodeUpToTrunk;
    CPtrList* pEOList = pTactic->pOpenListForSearchTailNodeUpToTrunk;
    CPtrList* pECList = pTactic->pClosListForSearchTailNodeUpToTrunk;
    CPtrList* pSOList1 = pTactic->pOpenListForSearchMidNormal;
    CPtrList* pSCList1 = pTactic->pClosListForSearchMidNormal;
    CPtrList* pEOList1 = pTactic->pOpenListForSearchMidReverse;
    CPtrList* pECList1 = pTactic->pClosListForSearchMidReverse;
    
    ClearNWNList(pTactic->stParaForSearchUpNode.pHeadForbidNodeList);
    ClearNWNList(pTactic->stParaForSearchUpNode.pTailForbidNodeList);

step1:
    ClearNWNList(   pSOList );
    ClearNWNList(   pSCList );
    ClearNWNList(   pEOList );
    ClearNWNList(   pECList );
    ClearNWNList(   pSOList1);
    ClearNWNList(   pSCList1);
    ClearNWNList(   pEOList1);
    ClearNWNList(   pECList1);

    bFind = FALSE;
    CGIS_NetworkNode* pStartNode = new CGIS_NetworkNode;
    pStartNode->m_pRut     = pOriginStartNode->m_pRut;
    pStartNode->m_pVtx     = pOriginStartNode->m_pVtx;
    pStartNode->m_pParent  = pOriginStartNode->m_pParent;
    CGIS_NetworkNode* pEndNode = new CGIS_NetworkNode;
    pEndNode->m_pRut     = pOriginEndNode->m_pRut;
    pEndNode->m_pVtx     = pOriginEndNode->m_pVtx;
    pEndNode->m_pParent  = pOriginEndNode->m_pParent;
    
    EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    //////////////////////////////////////////////////////////////////////////
    //----��ȡ��ͷ���м���Ĳ���----


    BOOL bFindHeadNode = FALSE;
    BOOL bFindTailNode = FALSE;


    pSOList->AddTail( pStartNode );
    pEOList->AddTail( pEndNode );
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkVtx* pEndVtx = pEndNode->m_pVtx;
    pEndVtx = GetVAbutVtx(pEndVtx);
    ASSERT(pEndVtx);
    if( pEndVtx == NULL )
    {
        ClearNWNList( pSOList );
        ClearNWNList( pEOList );
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }


    BYTE byRLevel = pTactic->nRLevel[0];
    m_byRLevel = pTactic->nRLevel[0];
    //-------------------------------------------------------------
    //���÷���Ϊ��������Ŀ��·��Ϊ��2���ڵ�·��
    m_byCFlag = 1;
    m_pDestRut = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType) );
    //-------------------------------------------------------------

    bFindHeadNode = CalculateOptimalPathHead(
                                             pSOList, 
                                             pSCList, 
                                             pEndVtx,
                                             pPathInfo,
                                             byRLevel, 
                                             TRUE,
                                             bFind,
                                             pTactic);

    if(bFind)
    {
        pOriginStartNode = (CGIS_NetworkNode*)pSOList->GetHead();
        return TRUE;
    }
    

    ASSERT(!bFind);
    {
        if( pSOList->GetCount() == 0 )
        {
            ClearNWNList( pSCList );
            ClearNWNList( pEOList );
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_NETWORK_DISCONNECT));
        }
        
        //��ʱ�����ҵ����ϸ��ٵ�,Ҳ����û���ҵ�.
        
        //ȡ��OPEN���е����ŵ���Ϊ��ʼ�ڵ�.
        TRACE0("��β���ϸߵȼ�·��ʹ�õķ���Node\n");
        DUMPSTATE((CGIS_NetworkNode*)pSOList->GetHead());
        CGIS_NetworkVtx* pStartVtx = ((CGIS_NetworkNode*)pSOList->GetHead())->m_pVtx;
        pStartVtx = GetVAbutVtx( pStartVtx );
        if( pStartVtx == NULL )
        {
            ClearNWNList( pSOList );
            ClearNWNList( pSCList );
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_UNDEFINE));
        }
        
        byRLevel = pTactic->nRLevel[1];
        m_byRLevel = pTactic->nRLevel[1];
        m_byCFlag = 2;//��������
        //////////////////////////////////////////////////////////////////////////
        m_pDestRut = GetNetworkRut( pWPOne->GetWPRutNID(eMapType), pWPOne->GetWPRutID(eMapType) );
        //////////////////////////////////////////////////////////////////////////
        bFindTailNode = CalculateOptimalPathHead( 
            pEOList, 
            pECList,
            pStartVtx, 
            pPathInfo,
            byRLevel, 
            FALSE,
            bFind ,
            pTactic);      
    }
    //////////////////////////////////////////////////////////////////////////
    if( bFind )
    {
        pOriginStartNode  = (CGIS_NetworkNode*)pSOList->GetHead();
        pOriginEndNode    = (CGIS_NetworkNode*)pEOList->GetHead();
        return TRUE;
    }
     
    ASSERT(!bFind);
    
    if( pSOList->GetCount() <= 0 || pEOList->GetCount() <= 0 )
    {
        //����Ϊ0����·���Ҷ���
        ClearNWNList( pSOList );
        ClearNWNList( pSCList );
        ClearNWNList( pEOList );
        ClearNWNList( pECList );
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_NETWORK_DISCONNECT));
    }
    
    
    byRLevel            = pTactic->nRLevel[2];
    m_LoadNetworkFlag   = pTactic->nLoadNetworkFlag[2];
    pStartNode = GetHeadNode( pSOList, pSCList, pTactic->nRLevel[2] );
    pEndNode = GetHeadNode( pEOList, pECList, pTactic->nRLevel[2] );
    
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkNode* pHeadUpNode = NULL;
    CGIS_NetworkNode* pTailUpNode = NULL;
    if(bFindHeadNode)
    {
        pHeadUpNode = pStartNode;
        ASSERT(pHeadUpNode);
    }
    if(bFindTailNode)
    {
        pTailUpNode = pEndNode;
        ASSERT(pTailUpNode);
    }
    //////////////////////////////////////////////////////////////////////////
    
    TRACE0("�ϸ���NODE\n");
    DUMPSTATE(pStartNode);
    TRACE0("�¸���NODE\n");
    DUMPSTATE(pEndNode);
    ASSERT( pStartNode && pEndNode );
    ClearNWHead( pStartNode, pEndNode, pSOList, pSCList, pEOList, pECList );
    

    m_byClear = 1;
    //////////////////////////////////////////////////////////////////////////
    //��ȫ��·���Ͻ�������
    
    if(!m_pTrunkNetwork)
    {
        CGIS_MBBNetwork* pObjAddr = (CGIS_MBBNetwork*)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_MBBNetwork));
        if(pObjAddr)
        { 
            m_pTrunkNetwork = new (pObjAddr) CGIS_MBBNetwork;  
            m_pTrunkNetwork->SetGeoMap( pMap );
            m_pTrunkNetwork->SetUnCompressBuffer(
                m_pBufOfBeforeCompress,
                m_pBufOfAfterCompress,
                sizeof(m_pBufOfBeforeCompress),
                sizeof(m_pBufOfAfterCompress));               
        }
    }
    TRACE0("----------------\n");
    CGIS_MBBNetwork::enErrorCode eErrorCode = CGIS_MBBNetwork::ERROR_NONE;
    BOOL bRtn = FALSE;
    ASSERT(m_pTrunkNetwork);
    
    if(m_pTrunkNetwork)
    {
#ifdef _DEBUG
#ifndef _WIN32_WCE 
        m_pTrunkNetwork->SetMapViewSrc(GetMapView());
#endif
#endif
        bRtn = m_pTrunkNetwork->CalculateOptimalPathMid( 
            pStartNode, 
            pEndNode,
            pPathInfo,
            pPathMid,
            &eErrorCode);
    }
    if(bRtn)
    {   
        pOriginStartNode = pStartNode;
        pOriginEndNode = pEndNode;
        
        bFind = TRUE;
    }
    
    //////////////////////////////////////////////////////////////////////////
 	ClearInvalNetwork( m_pSLBID, m_nSLBNum, m_nMaxSLBNum, m_pELBID, m_nELBNum, m_nMaxELBNum );

	if( !bFind )
    {
        CGIS_NetworkException::enErrorCode eExceptionErrorCode = CGIS_NetworkException::ERROR_UNDEFINE;
        switch(eErrorCode)
        {
            case CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY:
                TRACE0("out of memory when calculate in trunk network.\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
                break;
            case CGIS_MBBNetwork::ERROR_NETWORK_DISCONNECT:
                TRACE0("trunk network disconnect.\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_TRUNK_NETWORK_DISCONNECT;
                break;
            case CGIS_MBBNetwork::ERROR_FILE_IO:
                TRACE0("trunk network disconnect.\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_FILE_IO;
                break;
            case CGIS_MBBNetwork::ERROR_STOP_BY_OUTSIDE:
                TRACE0("can't find the trunk vertex by detailmap vertex!\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_STOP_BY_OUTSIDE;
                break;
                
            case CGIS_MBBNetwork::ERROR_INVALID_START_VERTEX:
                TRACE0("can't find the trunk vertex by detailmap vertex!\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_INVALID_START_VERTEX_IN_TRUNK_NETWORK;
                break;
            case CGIS_MBBNetwork::ERROR_INVALID_END_VERTEX:                
                TRACE0("can't find the trunk vertex by detailmap vertex!\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_INVALID_END_VERTEX_IN_TRUNK_NETWORK;
                break;
            default:
                ASSERT(FALSE);
                break;
        }
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,eExceptionErrorCode));
    }
	return VALUE_TRUE;          
}
/**
 * @brief �ڴ˰汾��ʵ��
 *
 * @param 
 * @return
*/
BOOL CGIS_MapBreadthNetwork::CalculateNavigatePath_DoubleSourceSearchInTrunkNetworkV2(
    CGIS_GeoMap* pMap , 
    CGIS_NetworkPathInfo* pPathInfo ,
    CGIS_NetworkNode*& pOriginStartNode,
    CGIS_NetworkNode*& pOriginEndNode,                                                        
    CGIS_WayPoint* pWPOne,
    CGIS_WayPoint* pWPTwo,
    BOOL& bFind,
    stuTacticOfSearchPathHead* pTactic,
    CPtrList* pPathMid,
    BOOL& bFindInTrunk)
{
    CPtrList* pSOList = pTactic->pOpenListForSearchHeadNodeUpToTrunk;
    CPtrList* pSCList = pTactic->pClosListForSearchHeadNodeUpToTrunk;
    CPtrList* pEOList = pTactic->pOpenListForSearchTailNodeUpToTrunk;
    CPtrList* pECList = pTactic->pClosListForSearchTailNodeUpToTrunk;
    CPtrList* pSOList1 = pTactic->pOpenListForSearchMidNormal;
    CPtrList* pSCList1 = pTactic->pClosListForSearchMidNormal;
    CPtrList* pEOList1 = pTactic->pOpenListForSearchMidReverse;
    CPtrList* pECList1 = pTactic->pClosListForSearchMidReverse;
    
    ClearNWNList(pTactic->stParaForSearchUpNode.pHeadForbidNodeList);
    ClearNWNList(pTactic->stParaForSearchUpNode.pTailForbidNodeList);


    if(!m_pTrunkNetwork)
    {
        CGIS_MBBNetwork* pObjAddr = (CGIS_MBBNetwork*)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_MBBNetwork));
        if(pObjAddr)
        { 
            m_pTrunkNetwork = new (pObjAddr) CGIS_MBBNetwork;  
            m_pTrunkNetwork->SetGeoMap( pMap );
            m_pTrunkNetwork->SetUnCompressBuffer(
                m_pBufOfBeforeCompress,
                m_pBufOfAfterCompress,
                sizeof(m_pBufOfBeforeCompress),
                sizeof(m_pBufOfAfterCompress));               
        }
    }
step1:
    ClearNWNList(   pSOList );
    ClearNWNList(   pSCList );
    ClearNWNList(   pEOList );
    ClearNWNList(   pECList );
    ClearNWNList(   pSOList1);
    ClearNWNList(   pSCList1);
    ClearNWNList(   pEOList1);
    ClearNWNList(   pECList1);

    bFind = FALSE;
    CGIS_NetworkNode* pStartNode = new CGIS_NetworkNode;
    pStartNode->m_pRut     = pOriginStartNode->m_pRut;
    pStartNode->m_pVtx     = pOriginStartNode->m_pVtx;
    pStartNode->m_pParent  = pOriginStartNode->m_pParent;
    CGIS_NetworkNode* pEndNode = new CGIS_NetworkNode;
    pEndNode->m_pRut     = pOriginEndNode->m_pRut;
    pEndNode->m_pVtx     = pOriginEndNode->m_pVtx;
    pEndNode->m_pParent  = pOriginEndNode->m_pParent;
    
    EnMapType eMapType = pMap->GetMapInfo()->GetMapType();
    //////////////////////////////////////////////////////////////////////////
    //----��ȡ��ͷ���м���Ĳ���----


    BOOL bFindHeadNode = FALSE;
    BOOL bFindTailNode = FALSE;


    pSOList->AddTail( pStartNode );
    pEOList->AddTail( pEndNode );
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkVtx* pEndVtx = pEndNode->m_pVtx;
    pEndVtx = GetVAbutVtx(pEndVtx);
    ASSERT(pEndVtx);
    if( pEndVtx == NULL )
    {
        ClearNWNList( pSOList );
        ClearNWNList( pEOList );
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_DATA_ERROR));
    }


    BYTE byRLevel = pTactic->nRLevel[0];
    m_byRLevel = pTactic->nRLevel[0];
    //-------------------------------------------------------------
    //���÷���Ϊ��������Ŀ��·��Ϊ��2���ڵ�·��
    m_byCFlag = 1;
    m_pDestRut = GetNetworkRut( pWPTwo->GetWPRutNID(eMapType), pWPTwo->GetWPRutID(eMapType) );
    //-------------------------------------------------------------

    bFindHeadNode = CalculateOptimalPathHeadForUpToTrunk(
                                             pSOList, 
                                             pSCList, 
                                             pEndVtx,
                                             pPathInfo,
                                             byRLevel, 
                                             TRUE,
                                             bFind,
                                             pTactic);

    if(bFind)
    {
        pOriginStartNode = (CGIS_NetworkNode*)pSOList->GetHead();
        return TRUE;
    }
    

    ASSERT(!bFind);
    {
        if( pSOList->GetCount() == 0 )
        {
            ClearNWNList( pSCList );
            ClearNWNList( pEOList );
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_NETWORK_DISCONNECT));
        }
        
        //��ʱ�����ҵ����ϸ��ٵ�,Ҳ����û���ҵ�.
        
        //ȡ��OPEN���е����ŵ���Ϊ��ʼ�ڵ�.
        TRACE0("��β���ϸߵȼ�·��ʹ�õķ���Node\n");
        DUMPSTATE((CGIS_NetworkNode*)pSOList->GetHead());
        CGIS_NetworkVtx* pStartVtx = ((CGIS_NetworkNode*)pSOList->GetHead())->m_pVtx;
        pStartVtx = GetVAbutVtx( pStartVtx );
        if( pStartVtx == NULL )
        {
            ClearNWNList( pSOList );
            ClearNWNList( pSCList );
            THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_UNDEFINE));
        }
        
        byRLevel = pTactic->nRLevel[1];
        m_byRLevel = pTactic->nRLevel[1];
        m_byCFlag = 2;//��������
        //////////////////////////////////////////////////////////////////////////
        m_pDestRut = GetNetworkRut( pWPOne->GetWPRutNID(eMapType), pWPOne->GetWPRutID(eMapType) );
        //////////////////////////////////////////////////////////////////////////
        bFindTailNode = CalculateOptimalPathHeadForUpToTrunk( 
            pEOList, 
            pECList,
            pStartVtx, 
            pPathInfo,
            byRLevel, 
            FALSE,
            bFind ,
            pTactic);      
    }
    //////////////////////////////////////////////////////////////////////////
    if( bFind )
    {
        pOriginStartNode  = (CGIS_NetworkNode*)pSOList->GetHead();
        pOriginEndNode    = (CGIS_NetworkNode*)pEOList->GetHead();
        return TRUE;
    }
     
    ASSERT(!bFind);
    
    if( pSOList->GetCount() <= 0 || pEOList->GetCount() <= 0 )
    {
        //����Ϊ0����·���Ҷ���
        ClearNWNList( pSOList );
        ClearNWNList( pSCList );
        ClearNWNList( pEOList );
        ClearNWNList( pECList );
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,CGIS_NetworkException::ERROR_NETWORK_DISCONNECT));
    }
    
    
    byRLevel            = pTactic->nRLevel[2];
    m_LoadNetworkFlag   = pTactic->nLoadNetworkFlag[2];
    pStartNode = GetHeadNode( pSOList, pSCList, pTactic->nRLevel[2] );
    pEndNode = GetHeadNode( pEOList, pECList, pTactic->nRLevel[2] );
    
    //////////////////////////////////////////////////////////////////////////
    CGIS_NetworkNode* pHeadUpNode = NULL;
    CGIS_NetworkNode* pTailUpNode = NULL;
    if(bFindHeadNode)
    {
        pHeadUpNode = pStartNode;
        ASSERT(pHeadUpNode);
    }
    if(bFindTailNode)
    {
        pTailUpNode = pEndNode;
        ASSERT(pTailUpNode);
    }
    //////////////////////////////////////////////////////////////////////////
    
    TRACE0("�ϸ���NODE\n");
    DUMPSTATE(pStartNode);
    TRACE0("�¸���NODE\n");
    DUMPSTATE(pEndNode);
    ASSERT( pStartNode && pEndNode );
    ClearNWHead( pStartNode, pEndNode, pSOList, pSCList, pEOList, pECList );
    

    TRACE0("��ʼ��ȫ��·����˫�򽻻�\n");
    bFindInTrunk = TRUE;
    m_byClear = 1;
    //////////////////////////////////////////////////////////////////////////
    //��ȫ��·���Ͻ�������
    
    if(!m_pTrunkNetwork)
    {
        CGIS_MBBNetwork* pObjAddr = (CGIS_MBBNetwork*)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_MBBNetwork));
        if(pObjAddr)
        { 
            m_pTrunkNetwork = new (pObjAddr) CGIS_MBBNetwork;  
            m_pTrunkNetwork->SetGeoMap( pMap );
            m_pTrunkNetwork->SetUnCompressBuffer(
                m_pBufOfBeforeCompress,
                m_pBufOfAfterCompress,
                sizeof(m_pBufOfBeforeCompress),
                sizeof(m_pBufOfAfterCompress));               
        }
    }
    TRACE0("----------------\n");
    CGIS_MBBNetwork::enErrorCode eErrorCode = CGIS_MBBNetwork::ERROR_NONE;
    BOOL bRtn = FALSE;
    ASSERT(m_pTrunkNetwork);
    
    if(m_pTrunkNetwork)
    {
#ifdef _DEBUG
#ifndef _WIN32_WCE 
        m_pTrunkNetwork->SetMapViewSrc(GetMapView());
#endif
#endif
        bRtn = m_pTrunkNetwork->CalculateOptimalPathMid( 
            pStartNode, 
            pEndNode,
            pPathInfo,
            pPathMid,
            &eErrorCode);
    }
    if(bRtn)
    {   
        pOriginStartNode = pStartNode;
        pOriginEndNode = pEndNode;
        
        bFind = TRUE;
    }
    
    //////////////////////////////////////////////////////////////////////////
 	ClearInvalNetwork( m_pSLBID, m_nSLBNum, m_nMaxSLBNum, m_pELBID, m_nELBNum, m_nMaxELBNum );

	if( !bFind )
    {
        CGIS_NetworkException::enErrorCode eExceptionErrorCode = CGIS_NetworkException::ERROR_UNDEFINE;
        switch(eErrorCode)
        {
            case CGIS_MBBNetwork::ERROR_OUT_OF_MEMORY:
                TRACE0("out of memory when calculate in trunk network.\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
                break;
            case CGIS_MBBNetwork::ERROR_NETWORK_DISCONNECT:
                TRACE0("trunk network disconnect.\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_TRUNK_NETWORK_DISCONNECT;
                break;
            case CGIS_MBBNetwork::ERROR_FILE_IO:
                TRACE0("trunk network disconnect.\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_FILE_IO;
                break;
            case CGIS_MBBNetwork::ERROR_STOP_BY_OUTSIDE:
                TRACE0("can't find the trunk vertex by detailmap vertex!\n");
                eExceptionErrorCode = CGIS_NetworkException::ERROR_STOP_BY_OUTSIDE;
                break;              
            case CGIS_MBBNetwork::ERROR_INVALID_START_VERTEX:
            case CGIS_MBBNetwork::ERROR_INVALID_END_VERTEX:                
                //TRACE0("can't find the trunk vertex by detailmap vertex!\n");
                //eExceptionErrorCode = CGIS_NetworkException::ERROR_INVALID_END_VERTEX_IN_TRUNK_NETWORK;
                {
                    pSOList1->AddTail( pStartNode );
                    pEOList1->AddTail( pEndNode );
                    //AddNWNode( pStartNode, pSCList1 );
                    //AddNWNode( pEndNode, pECList1 );
                    m_byClear = 1;
                    m_eCalculateScene = pTactic->eCalculateSceneForSearchMid;
                    
                    bFindInTrunk = FALSE;
                    TRACE0("ȫ��·���Ͻ���ʧ��,�ص���ϸ·�����н���\n");
                    bFind = CalculateOptimalPathMid(
                        pSOList1,
                        pSCList1,
                        pEOList1,
                        pECList1,
                        pPathInfo,
                        byRLevel,
                        &pStartNode,
                        &pEndNode,
                        pTactic
                        );
                    
                    ASSERT(bFind);
                    
                    if(bFind)
                    {
                        //////////////////////////////////////////////////////////////////////////
                        switch((bFindHeadNode<<4)|bFindTailNode)
                        {
                        case 0x00://��������²�����.��Ϊ�޷���ͷ��.
                            break;
                        case 0x10://ͷ������.
                            break;
                        case 0x01://β������
                            break;
                        case 0x11:
                            break;
                        default:
                            ASSERT(FALSE);
                            break;
                        }      
                        
                        pOriginStartNode = pStartNode;
                        pOriginEndNode = pEndNode;
                        
                        return TRUE;
                    }
                    
                }
                break;
            default:
                ASSERT(FALSE);
                break;
        }
        THROW(new CGIS_NetworkException(TRUE,CGIS_NetworkException::MOUDLE_CALCULATE_PATH,eExceptionErrorCode));
    }
	return VALUE_TRUE;          
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//����������ֲ�������Դ��,������֤ÿ������ļ������֮�����ɵ�NODE����ȫ����ж�ص�,
//��Ϊ�����ж�ظɾ��Ļ�,���ܻ�Ӱ�쵽�����ĵ�ļ���.
CLocalNodeListMng::CLocalNodeListMng(CPtrList* pList)
{
    ASSERT(pList);
    m_pList = pList;
}

CLocalNodeListMng::~CLocalNodeListMng()
{
    CPtrList* pNList  = m_pList;
	CGIS_NetworkNode *pNode = NULL;
	POSITION pos = pNList->GetHeadPosition();
	while( pos )
	{
		pNode = (CGIS_NetworkNode*)pNList->GetNext( pos );
		if( pNode )
		{
			pNode->m_pVtx->GetVtxSInfo()->posOHead = pNode->m_pVtx->GetVtxSInfo()->posOEnd = NULL;
			if( pNode->m_pVtx->GetVtxEdge() == 1 )
				pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 1;//��Ϊ1��Ϊ�˱�����Щ�ڵ㲻�ں�߱�ɾ��
			else
		        pNode->m_pVtx->GetVtxSInfo()->byStatusOfNormalSearch = pNode->m_pVtx->GetVtxSInfo()->byStatusOfReverseSearch = 0;
			delete pNode; 
			pNode = NULL;
		}
	}
	pNList->RemoveAll();
}
//////////////////////////////////////////////////////////////////////////
