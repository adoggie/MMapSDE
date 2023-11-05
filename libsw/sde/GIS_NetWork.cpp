// GIS_NetWork.cpp: implementation of the CGIS_NetWork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemoryDispatchForSpaceDataV2.h"
#include "GIS_NetworkException.h"
#include "./DebugFunctionLib/Watch.h"
#include "GIS_NetworkStatus.h"

//////////////////////////////////////////////////////////////////////////
#include "../DebugFunctionLib/DebugFun.h"
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

//#define DBG2SCR0(sz)              
//#define DBG2SCR1(sz, p1)          
//#define DBG2SCR2(sz, p1, p2)      
//#define DBG2SCR3(sz, p1, p2, p3) 
//#define TRACK_FUNCTION(sz) 
//#define DBG2LOG0(sz)                
//#define DBG2LOG1(sz, p1)            
//#define DBG2LOG2(sz, p1, p2)        
//#define DBG2LOG3(sz, p1, p2, p3)    
//#define WATCH2LOG(sz)                
//////////////////////////////////////////////////////////////////////////
#include "GIS_NetWork.h"
#include "FileDataStruct/V7/GIS_NetworkRoadSectionV7.h"
#include "FileDataStruct/V7/GIS_NetworkVertexV7.h"
#include "GIS_MBBNetwork.h"
#include "GIS_MapBreadthNetwork.h"
#include "GIS_WayPoint.h"
#include "GIS_Pathpoint.h"
#include "GIS_PathpointMark.h"
#include "GIS_FeatureRoute.h"
#include "./MemoryMngLib/MemoryMngV3A.h"

#include "SDEControler.h"
#include "MemoryDispatchForNetworkV2.h"
#include "GIS_PathpointNodeV2.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetWork::CGIS_NetWork()
{
	m_pMapNetwork = NULL;
}

CGIS_NetWork::~CGIS_NetWork()
{
	m_pNetworkRoute = NULL;
	m_pMapManager = NULL;
}
/**
 * @brief ������Ѻ���
 *
 * @param 
 * @return
*/
int CGIS_NetWork::CalculateOptimalRouteActual()
{
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    CMemoryDispatchForNetworkV2* pDispatch = pSdeControl->GetMemoryDispatchForNetwork();
    ASSERT(pDispatch);
//////////////////////////////////////////////////////////////////////////

	//��ͼ��������������ά���ĺ���Ϊ����ʧ��
	if( m_pMapManager == NULL )
		return 0;
	if( m_pNetworkRoute == NULL )
		return 0;


	//������ߵĵ������С��2���򷵻�ʧ��
	CPtrList *pWptList = m_pNetworkRoute->GetWayPointList();
	if( pWptList->GetCount() < 2 )
		return 0;

    TRACE0("<<<<******************************************\n");
	//////////////////////////////////////////////////////////////////////////
	//�������е�
	POSITION pos = pWptList->GetHeadPosition();
	CGIS_WayPoint *pWPPoint = NULL;
	while( pos )
	{
		pWPPoint = (CGIS_WayPoint*)pWptList->GetNext(pos);
		if( pWPPoint )
			pWPPoint->ClearWPRutInfo();
	}

	//////////////////////////////////////////////////////////////////////////
	//����·�����е㲢ɾ��
	CGIS_NetworkPath* pPath = m_pNetworkRoute->GetNetworkPath();
	ASSERT(pPath);
	CPtrList *pPPList = pPath->GetPathpointList();
	if( pPPList )
	{
		pos = pPPList->GetHeadPosition();
		
		CGIS_Pathpoint *pPPoint = NULL;
		while( pos )
		{
			pPPoint = (CGIS_Pathpoint*)pPPList->GetNext(pos);
			if( pPPoint )
				delete pPPoint;
			pPPoint = NULL;
		}
		pPPList->RemoveAll();
	}
	//////////////////////////////////////////////////////////////////////////
	
	
	//////////////////////////////////////////////////////////////////////////
	//Ϊʲô��ȡ��idΪ2�ĵ�ͼ
	IGIS_Map *pMap = m_pMapManager->GetMapByID( 2 );
	int nCFlag = 0;
	if( pMap )
    {
          
        CPtrList* pWptList = m_pNetworkRoute->GetWayPointList();
        
        TRACE0("----���߹滮�������--\n");
        BOOL bRtn = CheckCalculateCondition(pWptList);
        TRACE0("--------------------\n");
        if(!bRtn)
        {
            nCFlag = -6;
        }
        else
        {          
            DWORD dwActualSize = 0;
            DWORD dwMaxSize = 0;
            if(MemoryInitialByDist(pWptList,dwActualSize,dwMaxSize))
            {
                
                {
                    WATCH("·������");
                    WATCH2LOG("·������");
                    nCFlag = CalculateOptimalRouteInMap(
                        pMap,
                        pWptList,
                        m_pNetworkRoute->GetNetworkPath()->GetPathpointList());
                }
                
                MemoryClear();
                
                //-------------------------------------------------------------
                //������ڴ治������ʧ�����״β�δȫ�������ڴ�,��ȫ��������ٴμ���
                if(CGIS_NetworkException::ERROR_OUT_OF_MEMORY == nCFlag)
                {
                    if(dwActualSize < dwMaxSize)
                    {
                        TRACE0("�ڴ�ȫ������,�ٴμ���!\n");
                        WRITE2LOG0("�ڴ�ȫ������,�ٴμ���!\n");
                        
                        DWORD dwRequireSize = dwMaxSize;
                        if(MemoryInitialBySize(dwRequireSize,dwActualSize,dwMaxSize))
                        {              
                            
                            {
                                WATCH("·������");
                                WATCH2LOG("·������");
                                
                                nCFlag = CalculateOptimalRouteInMap(pMap,
                                    pWptList,
                                    m_pNetworkRoute->GetNetworkPath()->GetPathpointList());
                            }
                            
                            MemoryClear();
                        }
                        else
                        {
                            WRITE2LOG0("MemoryInitialBySize Failed!\r");
                            nCFlag = CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
                        }
                    }
                }
            }
            else
            {
                WRITE2LOG0("MemoryInitialByDist Failed!\r");
                nCFlag = CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
            }
            
            
            TRACE1("calculate path rerutn value: %d.\n",nCFlag);
            DBG2SCR1("calculate path rerutn value: %d.\n",nCFlag);
            DBG2LOG1("calculate path rerutn value: %d.\r",nCFlag);

            TRACE1("path point number: %d.\n",m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetCount());
            DBG2LOG1("path point number: %d.\n",m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetCount());

            
            
            if( nCFlag == 1 )
            {
                DBG2LOG0("calcuate path turnlist.\n");
                WATCH("��������");
                WATCH2LOG("��������");
                
                TRACE0("----��ʼ���л�������\n");
                int nErrorCode = CGIS_NetworkException::ERROR_NONE;
                BOOL bSuccess = m_pNetworkRoute->GetNetworkPath()->CalcuatePathTurnList(&nErrorCode);
                if(!bSuccess)
                {
                    TRACE0("��������ʧ��!\n");
                    nCFlag = nErrorCode;
                }
                else
                {
                    
#ifdef _DEBUG_VIEW_TURNLIST
                    DUMPSTATE(m_pNetworkRoute->GetNetworkPath());
#endif
                }
                    
                TRACE0("--------\n");

            }
        }
    }
    TRACE0("******************************************>>>>\n");
	return nCFlag;
}
///parameter: byWPType 1.��ʾΪ�����ϵ�Ŀ�ĵ� 2.��ʾ�������ں������ҵ�һ�㽨����Ŀ���
CGIS_WayPoint *CGIS_NetWork::ConstructNearWaypoint( BYTE &byWPType )
{
	if( m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->GetCount() <= 0 )
		return NULL;
	CGIS_WayPoint *pWPoint = NULL;
	POSITION posCur = NULL;
	POSITION posVoice = NULL;

    
	m_pNetworkRoute->GetNetworkPath()->GetVoicePosition( posCur, posVoice );


	CPtrList *pPPList = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();
	POSITION psCur = posCur;
	if( psCur == pPPList->GetHeadPosition() )
		pPPList->GetNext(psCur);

    
	double dbDis = 0.0;
	double dbMDis = 75000.1;
	CGIS_Pathpoint *pPPoint = NULL;

    
    //////////////////////////////////////////////////////////////////////////
    //calculate the limit by route (exist the freeway or city freeway)
	POSITION pos = psCur;
	while( pos )
	{
		pPPoint = (CGIS_Pathpoint*)pPPList->GetAt(pos);
		if( pPPoint )
		{
			if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
			{
				if( ((CGIS_PathpointNode*)pPPoint)->GetRType() == 0 )
					dbMDis = 150000.1;
				if( ((CGIS_PathpointNode*)pPPoint)->GetRType() == 1 ||
					((CGIS_PathpointNode*)pPPoint)->GetRType() == 2 )
					dbMDis = 100000.1;
				break;
			}
		}
		pPPList->GetNext(pos);
	}
    //////////////////////////////////////////////////////////////////////////
    


    //////////////////////////////////////////////////////////////////////////
    //repeat add distance fo find the dest point
	pos = psCur;
	while( pos )
	{
		pPPoint = (CGIS_Pathpoint*)pPPList->GetAt(pos);
		if( pPPoint )
		{
			dbDis += pPPoint->GetDis2Next();


            //���ҵ�Ŀ�ĵص���ֱ�ӷ���
			if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE ||
					pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_START ||
					pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END )
			{
				pWPoint = ((CGIS_PathpointMark*)pPPoint)->GetWayPoint();
				byWPType = 1;
                TRACE0("ֱ����Ŀ�ĵ�ΪĿ���.\n");
				break;
			}

            

            //�Թյ㹹��Ŀ���
			if( dbDis > dbMDis && pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INFLEXION )
			{
                //�½�;����
				pWPoint = new CGIS_WayPoint( m_pNetworkRoute->GetWPMaxID()+1 );
				char *szAnno = new char[5];
				memcpy( szAnno, "�յ�", 5 );
				pWPoint->SetWPAnnoation( szAnno );
				pWPoint->SetWPAnnoNum( 5 );
				POSITION ps = pos;
				pPPList->GetNext(ps);

                CGIS_Pathpoint *pPPTemp = (CGIS_Pathpoint*)pPPList->GetAt(ps);
				ST_GEO_PTXY ptWPoint;
				ptWPoint.dx = (float)((pPPoint->GetPathpointPosition().dx + pPPTemp->GetPathpointPosition().dx)/2.0);
				ptWPoint.dy = (float)((pPPoint->GetPathpointPosition().dy + pPPTemp->GetPathpointPosition().dy)/2.0);
				pWPoint->SetWPPosition( ptWPoint );
				pWPoint->SetWPType( EN_WAYPOINTTYPE_DESTINATION );
				byWPType = 2;
                TRACE0("�Ժ��ߵĵ㹹��Ŀ���.\n");
				break;
			}
		}
		pPPList->GetNext(pos);
	}

    //////////////////////////////////////////////////////////////////////////
    {
        CPtrList* pNodeList = m_pNetworkRoute->GetNetworkPath()->GetNodeList();
        ASSERT(pNodeList);
        POSITION posSub = pos;
        POSITION posCurrent = NULL;

        while(posSub)
        {
            posCurrent = posSub;
            pPPoint = (CGIS_Pathpoint*)pPPList->GetPrev(posSub);
            if( pPPoint && pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE)
            {
                break;
            }
        }

        if(posCurrent)
        {
            POSITION posSub = pNodeList->GetHeadPosition();
            POSITION posOfNode = NULL;
            while(posSub)
            {
                posOfNode = posSub;
                CGIS_PathpointNodeV2* pNode = (CGIS_PathpointNodeV2*)pNodeList->GetNext(posSub);
                if(pNode && pNode->GetPosOfPathPointList() == posCurrent)
                {
                    break;
                }
            }

            while(posOfNode)
            {
                posCurrent = posOfNode;
                CGIS_PathpointNodeV2* pNode = (CGIS_PathpointNodeV2*)pNodeList->GetPrev(posOfNode);
                delete pNode;
                pNodeList->RemoveAt(posCurrent);
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////////
	POSITION psTemp = NULL;
	//����ɾ����;�������
	int nDeleteWPNumber = 0;
	while( pos )
	{
		psTemp = pos;
		pPPoint = (CGIS_Pathpoint*)pPPList->GetPrev(pos);
		if( pPPoint )
		{
			if( (pPPoint->GetPathpointType() != EN_PATHPOINTTYPE_INFLEXION )
				&& (pPPoint->GetPathpointType() != EN_PATHPOINTTYPE_NODE) )
			{
				nDeleteWPNumber++;
			}
			delete pPPoint;
		}
		pPPoint = NULL;
		pPPList->RemoveAt(psTemp);
	}
	if( byWPType == 1 && nDeleteWPNumber > 1 )
		nDeleteWPNumber--;
    //ɾ����ǰ�Ѿ���������;����
	CGIS_WayPoint *pWPTemp = NULL;
	CPtrList* wpList = m_pNetworkRoute->GetWayPointList();
	pos = wpList->GetHeadPosition();
	while( pos )
	{
		psTemp = pos;
		if( nDeleteWPNumber == 0 )
			break;
		pWPTemp = (CGIS_WayPoint*)wpList->GetNext( pos );
		if( pWPTemp )
		{
			delete pWPTemp;
			pWPTemp = NULL;
		}
		wpList->RemoveAt( psTemp );
		nDeleteWPNumber--;
	}
//	if( byWPType == 1 )
//	{
//        TRACE0("ɾ��Ŀ�ĵ�֮ǰ������;����\n");
//		pos = m_pNetworkRoute->GetWayPointList()->Find( pWPoint );
//		m_pNetworkRoute->GetWayPointList()->GetPrev(pos);
//		while( pos )
//		{
//			psTemp = pos;
//			pWPTemp = (CGIS_WayPoint*)m_pNetworkRoute->GetWayPointList()->GetPrev(pos);
//			if( pWPTemp )
//				delete pWPTemp;
//			pWPTemp = NULL;
//			m_pNetworkRoute->GetWayPointList()->RemoveAt(psTemp);
//		}
//	}
//	else if( byWPType == 2 )
//	{
//        TRACE0("ɾ����ǰ��������������;����\n");
//		pWPTemp = (CGIS_WayPoint*)m_pNetworkRoute->GetWayPointList()->GetHead();
//		if( pWPTemp )
//		{
//			m_pNetworkRoute->GetWayPointList()->RemoveHead();
//			delete pWPTemp;
//		}
//		pWPTemp = NULL;
//	}
	return pWPoint;
}

    /**
     * @brief ·�߹滮
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

int CGIS_NetWork::CalculateOptimalRoute( BYTE byCType, CGIS_WayPoint *pWptCurrent )
{
    CCalculateStatusLock aStatusLock;

    //////////////////////////////////////////////////////////////////////////
	ASSERT( m_pNetworkRoute );
    CGIS_NetworkPathInfo* pPathInfo = m_pNetworkRoute->GetNetworkPathInfo();
    ASSERT(pPathInfo);
    CGIS_NetworkStatus::m_nStartCalculateTime = pPathInfo->GetStartCalculateTime();
    ASSERT(CGIS_NetworkStatus::m_nStartCalculateTime != 255);
    //////////////////////////////////////////////////////////////////////////
    

	ASSERT( m_pNetworkRoute );
	ASSERT( m_pMapManager );

    //ȡ��;�����б�
	CPtrList *pWptList = m_pNetworkRoute->GetWayPointList();
	ASSERT( pWptList->GetCount() >= 2 );
	if( pWptList->GetCount() < 2 )
		return -6;

	int nResult = 0;
	if( byCType == 1 || byCType == 2)
	{
		BYTE byWPType = 0;
		CGIS_WayPoint *pWPDest = ConstructNearWaypoint( byWPType );
		if( pWPDest )
		{
			CGIS_NetworkRoute *pROld = m_pNetworkRoute;

            //////////////////////////////////////////////////////////////////////////
            //����һ���º���ȥ����(��ǰ�㵽Ŀ���)
			CGIS_NetworkRoute *pRoute = new CGIS_NetworkRoute();//pFRoute->GetNetworkRoute();
			EnWaypointType enWPType = pWPDest->GetWPType();
			pWPDest->SetWPType( EN_WAYPOINTTYPE_DESTINATION );
			pRoute->GetWayPointList()->AddTail(pWptCurrent);
			pRoute->GetWayPointList()->AddTail(pWPDest);
			pRoute->SetNetworkPathInfo( pROld->GetNetworkPathInfo());

            m_pNetworkRoute = pRoute;
			nResult = CalculateOptimalRouteActual( );
			if( nResult == 1 )
			{
				pWPDest->SetWPType( enWPType );
				pROld->GetWayPointList()->AddHead( pWptCurrent );
				CPtrList *pPathPointListOld = pROld->GetNetworkPath()->GetPathpointList();
				CPtrList *pPathPointListNew = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();

				if( byWPType == 2 )
				{
                    //����ǹ��������ĵ�Ļ�ȥ�����һ��PathPoint��
					CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPathPointListNew->GetTail();
					pPathPointListNew->RemoveTail();
					delete pPPoint;
					delete pWPDest;
				}
                //////////////////////////////////////////////////////////////////////////
                /*���������������.
                ��Լ������ʱ�ᶪʧ;�е��BUG.
                    ԭ��,��ƫ������·�ļ���ʱ��ı�wpt�ڲ�����,
                    ��������.����ʱҪ��ԭ������.
                    */
				else if( byWPType==1 )
				{
                    //
					CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPathPointListNew->GetTail();
					if( pPPoint && enWPType == EN_WAYPOINTTYPE_INTERMEDIATE )
						pPPoint->SetPathpointType(EN_PATHPOINTTYPE_INTERMEDIATE);
				}
                //////////////////////////////////////////////////////////////////////////
				//���º��ߵĵ���ӵ��Ϻ��ߵ�ͷ��
                POSITION pos = pPathPointListNew->GetTailPosition();
				CGIS_Pathpoint *pPPoint = NULL;
				while( pos )
				{
					pPPoint = (CGIS_Pathpoint*)pPathPointListNew->GetPrev(pos);
					if( pPPoint )
						pPathPointListOld->AddHead(pPPoint);
				}
                //////////////////////////////////////////////////////////////////////////
                CPtrList* pNodeListOld = pROld->GetNetworkPath()->GetNodeList();
                CPtrList* pNodeListNew = m_pNetworkRoute->GetNetworkPath()->GetNodeList();
                pNodeListOld->AddHead(pNodeListNew);
                pNodeListNew->RemoveAll();
                //////////////////////////////////////////////////////////////////////////
                
                
				m_pNetworkRoute->GetWayPointList()->RemoveAll();
				m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->RemoveAll();
				delete m_pNetworkRoute;
				m_pNetworkRoute = pROld;
				m_pNetworkRoute->GetNetworkPath()->SetCTFlag( FALSE );			
				return nResult;
			}
			else
			{
                //����ʧ��
				if( byWPType == 1 )
					m_pNetworkRoute->GetWayPointList()->RemoveTail();
			}
			delete m_pNetworkRoute;
            m_pNetworkRoute = NULL;
            //////////////////////////////////////////////////////////////////////////
            
			m_pNetworkRoute = pROld;
			return nResult;
		}
        else
        {
            TRACE0("����ƫ��/��· Ŀ���ʧ��.\n");
        }
	}
	else
    {
        //��������
        return CalculateOptimalRouteActual( );
    }

	return 0;
}
int CGIS_NetWork::CalculateOptimalRouteV2( CGIS_WayPoint *pWPoint )
{
	int nResult = 0;
	CGIS_NetworkRoute *pROld = m_pNetworkRoute;
	CGIS_NetworkRoute *pRoute = new CGIS_NetworkRoute();//pFRoute->GetNetworkRoute();
	CGIS_WayPoint *pWPDest = ClearNetworkRoute();
	if( pWPDest )
	{
		EnWaypointType enWPType = pWPDest->GetWPType();
		pWPDest->SetWPType( EN_WAYPOINTTYPE_DESTINATION );
		pRoute->GetWayPointList()->AddTail(pWPoint);
		pRoute->GetWayPointList()->AddTail(pWPDest);
		pRoute->SetNetworkPathInfo( pROld->GetNetworkPathInfo() );
		m_pNetworkRoute = pRoute;
		nResult = CalculateOptimalRouteActual( );
		if( nResult == 1 )
		{
			pWPDest->SetWPType( enWPType );
			pROld->GetWayPointList()->AddHead( pWPoint );
			CPtrList *pPPOList = pROld->GetNetworkPath()->GetPathpointList();
			CPtrList *pPPList = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();
			POSITION pos = pPPList->GetTailPosition();
			CGIS_Pathpoint *pPPoint = NULL;
			while( pos )
			{
				pPPoint = (CGIS_Pathpoint*)pPPList->GetPrev(pos);
				if( pPPoint )
					pPPOList->AddHead(pPPoint);
			}
			m_pNetworkRoute->GetWayPointList()->RemoveAll();
			m_pNetworkRoute->GetNetworkPath()->GetPathpointList()->RemoveAll();
			delete m_pNetworkRoute;
			m_pNetworkRoute = pROld;
			m_pNetworkRoute->GetNetworkPath()->SetCTFlag( FALSE );			
			return nResult;
		}
	}
	return nResult;
}
CGIS_WayPoint* CGIS_NetWork::ClearNetworkRoute( )
{
	CGIS_WayPoint *pWPoint = NULL;
	POSITION posCur = NULL;
	POSITION posVoice = NULL;
	m_pNetworkRoute->GetNetworkPath()->GetVoicePosition( posCur, posVoice );
	CPtrList *pPPList = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();
	POSITION pos = posCur;
	if( pos == pPPList->GetHeadPosition() )
		pPPList->GetNext(pos);
	CGIS_Pathpoint *pPPoint = NULL;
	while( pos )
	{
		pPPoint = (CGIS_Pathpoint*)pPPList->GetAt(pos);
		if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE ||
				pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_START ||
				pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END )
		{
			pWPoint = ((CGIS_PathpointMark*)pPPoint)->GetWayPoint();
			break;
		}
		pPPList->GetNext(pos);
	}
	POSITION psTemp = NULL;
	while( pos )
	{
		psTemp = pos;
		pPPoint = (CGIS_Pathpoint*)pPPList->GetPrev(pos);
		if( pPPoint )
			delete pPPoint;
		pPPoint = NULL;
		pPPList->RemoveAt(psTemp);
	}
	pos = m_pNetworkRoute->GetWayPointList()->Find( pWPoint );
	m_pNetworkRoute->GetWayPointList()->GetPrev(pos);
	CGIS_WayPoint *pWPTemp = NULL;
	while( pos )
	{
		psTemp = pos;
		pWPTemp = (CGIS_WayPoint*)m_pNetworkRoute->GetWayPointList()->GetPrev(pos);
		if( pWPTemp )
			delete pWPTemp;
		pWPTemp = NULL;
		m_pNetworkRoute->GetWayPointList()->RemoveAt(psTemp);
	}
	return pWPoint;
}

/**
 * @brief ����ָ����ͼ�ϵ���Ѻ���
 *
 * @param pMap ��ͼ
 * @param WPList ���б�
 * @param pDesPointList Ŀ����б�

 * @return BOOL
    2007-1-23���ⶨ���µĲ�������.
    �Ե�ǰ������ܼ�ֱ�߾��������С�ڴ�����dwMinMemorySize������ڴ�����dwMaxMemorySize,
    �����ܾ���<60km��dwMinMemorySizeΪ4m,dwMaxMemorySizeΪ12m
   
*/
int CGIS_NetWork::CalculateOptimalRouteInMap(IGIS_Map *pMap,CPtrList* pWptList,CPtrList* pDesPointList)
{

    int bRT  = 0;
     
    TRY
    { 
        //�½�ͼ���������
        ASSERT(m_pMemMngOfBuddyData);
        //CGIS_MapBreadthNetwork *pMapNetwork = new CGIS_MapBreadthNetwork;
        CGIS_MapBreadthNetwork *pObjAddr = (CGIS_MapBreadthNetwork *)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_MapBreadthNetwork));
        if(!pObjAddr)
            return CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
        CGIS_MapBreadthNetwork *pMapNetwork = new (pObjAddr) CGIS_MapBreadthNetwork;
        
        m_pMapNetwork = pMapNetwork;
        //ASSERT(pMap->IsKindOf(RUNTIME_CLASS(CGIS_GeoMap)));
        
        //������ͼ�������ͼ
        pMapNetwork->SetGeoMap( (CGIS_GeoMap*)pMap );
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( pMapNetwork );
        //������ͼ������������
        pMapNetwork->SetNetworkRoute( m_pNetworkRoute );
        
        
        //////////////////////////////////////////////////////////////////////////
        //���Ŀ���(����õ�·����)�б�
        POSITION pos = pDesPointList->GetHeadPosition();
        CGIS_Pathpoint *pPPoint;
        while( pos )
        {
            pPPoint = (CGIS_Pathpoint*)pDesPointList->GetNext( pos );
            if( pPPoint )
            {
                delete pPPoint;
                pPPoint = NULL;
            }
        }
        pDesPointList->RemoveAll();
        //////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	pMapNetwork->SetMapViewSrc(GetMapView());
#endif     
        //���¼���Ŀ����б�
        bRT = pMapNetwork->CalculateNavigatePath( pWptList,pDesPointList );
      
        //�������
		{
			CSpaceMemoryLock aLock;    
			((CGIS_GeoMap*)pMap)->ClearStorageBMap();//��ͼ����
		}
        pMapNetwork->ClearNetwork();//·������
        m_pMapNetwork->~IGIS_MapNetwork();
        m_pMemMngOfBuddyData->newfree(m_pMapNetwork);
        m_pMapNetwork = NULL;
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( NULL );
    }
    CATCH(CMemoryException,e)
    {
       TRACE0("·���滮�в����ڴ��쳣!\n"); 
    }
    AND_CATCH(CFileException,e)
    {
       TRACE0("·���滮�в����ļ��쳣!\n"); 
    }
    END_CATCH

#ifdef ANALYSE_DATA
    m_dwMaxHeapUsedSize = m_pMemMngOfBuddyData->GetMaxHeapUsedSize();
    TRACE3("·�������ڴ�ռ��: [%d mb][%d kb][%d bytes].\n",m_dwMaxHeapUsedSize>>20,m_dwMaxHeapUsedSize>>10,m_dwMaxHeapUsedSize);    
    DBG2LOG3("·�������ڴ�ռ��: [%d mb][%d kb][%d bytes].\n",m_dwMaxHeapUsedSize>>20,m_dwMaxHeapUsedSize>>10,m_dwMaxHeapUsedSize);
#endif
	return bRT;
}

//////////////////////////////////////////////////////////////////////////

int CGIS_NetWork::CalculateStop()
{
    if( 0 == CGIS_NetworkStatus::m_nCalculateStatus)
        return 0;

    ASSERT(CGIS_NetworkStatus::m_bCalculateStopSign == 0);
	InterlockedIncrement(&CGIS_NetworkStatus::m_bCalculateStopSign);
	return TRUE;
    
//    ASSERT(m_pNetworkRoute);
//    ASSERT(m_pNetworkRoute->GetNetworkPath());
//    if(!m_pMapNetwork)
//    {
//        Sleep(300);
//        if(!m_pMapNetwork)
//        {
//            return FALSE;
//        }
//    }
//    
//	if(m_pNetworkRoute)
//	{
//		CGIS_NetworkPath* pPath = m_pNetworkRoute->GetNetworkPath();
//		if(pPath)	
//		{
//			TRACE0("Path���󷢳�ֹͣ���������ָ��..\n");
//			pPath->CalculateStop();
//		}
//	}
//	
//	if(m_pMapNetwork)
//	{
//		TRACE0("·�����󷢳�ֹͣ���������ָ��..\n");
//		m_pMapNetwork->CalculateStop();
//	}
//
// 	return TRUE;
}
BOOL CGIS_NetWork::GetMemoryLimitByDistance(DoubleArray* pArrayOfDist,DWORD& dwMinSize,DWORD& dwMaxSize)
{
    if(pArrayOfDist->GetSize() < 1)
        return FALSE;


    double dbMaxLegDist = 0.0f;
    for(int i = 0; i< pArrayOfDist->GetSize(); i++)
    {
        if((*pArrayOfDist)[i] > dbMaxLegDist)
        {
            dbMaxLegDist = (*pArrayOfDist)[i];
        }
    }



    if( dbMaxLegDist < 60000.0f)
    {
        dwMinSize = 3*IMemoryMng::MB;
        dwMaxSize = 12*IMemoryMng::MB;
    }
    else if(dbMaxLegDist < 300000.0f)
    {
        dwMinSize = 12*IMemoryMng::MB;
        dwMaxSize = 12*IMemoryMng::MB;
    }
    else if(dbMaxLegDist < 800000.0f)
    {
        dwMinSize = 12*IMemoryMng::MB;
        dwMaxSize = 12*IMemoryMng::MB;
    }
    else 
    {
        dwMinSize = 12*IMemoryMng::MB;
        dwMaxSize = 12*IMemoryMng::MB;
    }
    
    return TRUE;
}

BOOL CGIS_NetWork::CheckCalculateCondition(CPtrList* pWapPointList)
{
    if(!pWapPointList)
        return FALSE;
    if(pWapPointList->GetCount() < 2)
        return FALSE;

    CGIS_WayPoint *pWayPointPrev = NULL;
    CGIS_WayPoint *pWayPointNext = NULL;
    ST_GEO_PTXY ptPrev;
    ST_GEO_PTXY ptNext;

    POSITION pos = pWapPointList->GetHeadPosition();
    pWayPointPrev = (CGIS_WayPoint*)pWapPointList->GetNext(pos);

    double dbTotalDist = 0.0f;
    double dbMaxLegDist = 0.0f;
    while(pos)
    {
        pWayPointNext = (CGIS_WayPoint*)pWapPointList->GetNext(pos);

        ptPrev = pWayPointPrev->GetWPPosition();
        ptNext = pWayPointNext->GetWPPosition();
        double dbDis = CalcDist(ptPrev.dx,ptPrev.dy, ptNext.dx,ptNext.dy);
        if(dbDis > dbMaxLegDist)
        {
            dbMaxLegDist = dbDis;
        }

        dbTotalDist += dbDis;
        pWayPointPrev = pWayPointNext;
    }
    
    TRACE1("Total Dist:[%.2f km]\n",dbTotalDist/1000);
    
    //���ξ�����ڴ�ֵ����
    if(dbMaxLegDist > 6000000.0f)
        return FALSE;

    //////////////////////////////////////////////////////////////////////////
    //���涼�Ǵ�������.
    TRACE0("Check condition success,continue calculate.\n");
    return TRUE;
}




BOOL CGIS_NetWork::MemoryInitialByDist(CPtrList* pWapPointList,DWORD& dwAllocateActualSize,DWORD& dwAllocateMaxSize)
{
    ASSERT(pWapPointList);
    ASSERT(pWapPointList->GetCount() >= 2);

    CGIS_WayPoint *pWayPointPrev = NULL;
    CGIS_WayPoint *pWayPointNext = NULL;
    ST_GEO_PTXY ptPrev;
    ST_GEO_PTXY ptNext;

    POSITION pos = pWapPointList->GetHeadPosition();
    pWayPointPrev = (CGIS_WayPoint*)pWapPointList->GetNext(pos);

    double dbTotalDis = 0;
    CArray<double,double> aArOfDist;
    while(pos)
    {
        pWayPointNext = (CGIS_WayPoint*)pWapPointList->GetNext(pos);

        ptPrev = pWayPointPrev->GetWPPosition();
        ptNext = pWayPointNext->GetWPPosition();
        double dbDis = CalcDist(ptPrev.dx,ptPrev.dy, ptNext.dx,ptNext.dy);
        aArOfDist.Add(dbDis);
        dbTotalDis += dbDis;
        pWayPointPrev = pWayPointNext;
    }
    
    DWORD dwMinNeedSize = 0;    
    DWORD dwMaxNeedSize = 0;
    if(!GetMemoryLimitByDistance(&aArOfDist,dwMinNeedSize,dwMaxNeedSize))
    {
        TRACE0("���߹���,�޷�����!\n");
        return FALSE;
    }
    
    //////////////////////////////////////////////////////////////////////////
    //������Դ
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    CMemoryDispatchForNetworkV2* pDispatch = pSdeControl->GetMemoryDispatchForNetwork();
    ASSERT(pDispatch);
    
    //��������Ҫ�Ŀռ�
    if(!pDispatch->GetMemoryMngV3(dwMinNeedSize,dwMaxNeedSize,dwAllocateActualSize,dwAllocateMaxSize))
    {
        TRACE0("Can't get the enough memory for network.\n");
        return FALSE;
    }
    
    TRACE1("Reverse %dm memory for calculate route.\n",dwAllocateActualSize>>20);
    VERIFY(pDispatch->MemoryInitial());
    //////////////////////////////////////////////////////////////////////////
    //���涼�Ǵ�������.
    TRACE0("Memroy init success,continue calculate.\n");
    return TRUE;
}
BOOL CGIS_NetWork::MemoryInitialBySize(DWORD dwRequireSize,DWORD& dwAllocateActualSize,DWORD& dwAllocateMaxSize)
{
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    CMemoryDispatchForNetworkV2* pDispatch = pSdeControl->GetMemoryDispatchForNetwork();
    ASSERT(pDispatch);
    //////////////////////////////////////////////////////////////////////////
    
    //��������Ҫ�Ŀռ�
    if(!pDispatch->GetMemoryMngV3(dwRequireSize,dwRequireSize,dwAllocateActualSize,dwAllocateMaxSize))
    {
        TRACE0("Can't get the enough memory for network.\n");
        return FALSE;
    }

    TRACE1("Reverse %dm memory for calculate route.\n",dwAllocateActualSize>>20);
    VERIFY(pDispatch->MemoryInitial());
    //////////////////////////////////////////////////////////////////////////
    //���涼�Ǵ�������.
    TRACE0("Memroy init success,continue calculate.\n");
    return TRUE;
}

BOOL CGIS_NetWork::MemoryClear()
{
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    CMemoryDispatchForNetworkV2* pDispatch = pSdeControl->GetMemoryDispatchForNetwork();
    ASSERT(pDispatch);
    //////////////////////////////////////////////////////////////////////////
    
    pDispatch->MemoryClear();
    pDispatch->ReleaseMemoryMngV3();

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
#ifndef _WIN32_WCE
CGIS_NetworkRut* CGIS_NetWork::GetRoadSectionByBreadthIDAndRoadSectionID(int nBreadthID,int nID,int& nLayer)
{
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    CMemoryDispatchForNetworkV2* pDispatch = pSdeControl->GetMemoryDispatchForNetwork();
    ASSERT(pDispatch);
//////////////////////////////////////////////////////////////////////////

    unsigned long nSize = 6*IMemoryMng::MB;
    MemoryInitialBySize( nSize,nSize,nSize);

    CGIS_NetworkRut* pRoadSectionRtn = NULL;
    IGIS_Map *pMap = m_pMapManager->GetMapByID( 2 );
    ASSERT(pMap);


    TRY
    { 
        //�½�ͼ���������
        ASSERT(m_pMemMngOfBuddyData);
        //CGIS_MapBreadthNetwork *pMapNetwork = new CGIS_MapBreadthNetwork;
        CGIS_MapBreadthNetwork *pObjAddr = (CGIS_MapBreadthNetwork *)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_MapBreadthNetwork));
        ASSERT(pObjAddr);
        CGIS_MapBreadthNetwork *pMapNetwork = new (pObjAddr) CGIS_MapBreadthNetwork;
        
        m_pMapNetwork = pMapNetwork;
        //ASSERT(pMap->IsKindOf(RUNTIME_CLASS(CGIS_GeoMap)));
        
        //������ͼ�������ͼ
        pMapNetwork->SetGeoMap( (CGIS_GeoMap*)pMap );
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( pMapNetwork );
        
        CGIS_BreadthMap* pBreadthMap = ((CGIS_GeoMap*)pMap)->GetBreadthMapByID2(nBreadthID);
        ASSERT(pBreadthMap);

        
        int nLayer0RoadSectionMax;
        int nLayer1RoadSectionMax;
        int nLayer2RoadSectionMax;
        int nLayer0VertexMax;
        int nLayer1VertexMax;
        int nLayer2VertexMax;
        pMapNetwork->ReadBreadthNetworkV2(&pBreadthMap,1,
            nLayer0RoadSectionMax,nLayer1RoadSectionMax,nLayer2RoadSectionMax,
            nLayer0VertexMax,nLayer1VertexMax,nLayer2VertexMax);
        CGIS_NetworkRut* pRoadSection = pMapNetwork->GetNetworkRut(nBreadthID,nID);
        if(pRoadSection)
        {
            if(pRoadSection->GetRutID() < nLayer0RoadSectionMax)
            {
                nLayer = 0;
            }
            else if(pRoadSection->GetRutID() < nLayer1RoadSectionMax)
            {
                nLayer = 1;
            }
            else if(pRoadSection->GetRutID() < nLayer2RoadSectionMax)
            {
                nLayer = 2;
            }
            else
            {
                ASSERT(FALSE);
            }
            pRoadSectionRtn = pRoadSection->CreateCloneObject();
        }

        pMapNetwork->ClearNetwork();//·������
        m_pMapNetwork->~IGIS_MapNetwork();
        m_pMemMngOfBuddyData->newfree(m_pMapNetwork);
        m_pMapNetwork = NULL;
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( NULL );
    }
    CATCH(CMemoryException,e)
    {
       TRACE0("·���滮�в����ڴ��쳣!\n"); 
    }
    AND_CATCH(CFileException,e)
    {
       TRACE0("·���滮�в����ļ��쳣!\n"); 
    }
    END_CATCH

    MemoryClear();
    return pRoadSectionRtn;

}
CGIS_NetworkVtx* CGIS_NetWork::GetVertexByBreadthIDAndVertexID(int nBreadthID,int nID,int& nLayer)
{
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    CMemoryDispatchForNetworkV2* pDispatch = pSdeControl->GetMemoryDispatchForNetwork();
    ASSERT(pDispatch);
//////////////////////////////////////////////////////////////////////////

    unsigned long nSize = 6*IMemoryMng::MB;
    MemoryInitialBySize( nSize,nSize,nSize);

    CGIS_NetworkVtx* pVertexRtn = NULL;
    IGIS_Map *pMap = m_pMapManager->GetMapByID( 2 );
    ASSERT(pMap);


    TRY
    { 
        //�½�ͼ���������
        ASSERT(m_pMemMngOfBuddyData);
        //CGIS_MapBreadthNetwork *pMapNetwork = new CGIS_MapBreadthNetwork;
        CGIS_MapBreadthNetwork *pObjAddr = (CGIS_MapBreadthNetwork *)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_MapBreadthNetwork));
        ASSERT(pObjAddr);
        CGIS_MapBreadthNetwork *pMapNetwork = new (pObjAddr) CGIS_MapBreadthNetwork;
        
        m_pMapNetwork = pMapNetwork;
        //ASSERT(pMap->IsKindOf(RUNTIME_CLASS(CGIS_GeoMap)));
        
        //������ͼ�������ͼ
        pMapNetwork->SetGeoMap( (CGIS_GeoMap*)pMap );
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( pMapNetwork );
        
        CGIS_BreadthMap* pBreadthMap = ((CGIS_GeoMap*)pMap)->GetBreadthMapByID2(nBreadthID);
        ASSERT(pBreadthMap);
        int nLayer0RoadSectionMax;
        int nLayer1RoadSectionMax;
        int nLayer2RoadSectionMax;
        int nLayer0VertexMax;
        int nLayer1VertexMax;
        int nLayer2VertexMax;
        pMapNetwork->ReadBreadthNetworkV2(&pBreadthMap,1,
            nLayer0RoadSectionMax,nLayer1RoadSectionMax,nLayer2RoadSectionMax,
            nLayer0VertexMax,nLayer1VertexMax,nLayer2VertexMax);
        CGIS_NetworkVtx* pVertex = pMapNetwork->GetNetworkVtx(nBreadthID,nID);
        if(pVertex)
        {
            if(pVertex->GetVtxID() < nLayer0VertexMax)
            {
                nLayer = 0;
            }
            else if(pVertex->GetVtxID() < nLayer1VertexMax)
            {
                nLayer = 1;
            }
            else if(pVertex->GetVtxID() < nLayer2VertexMax)
            {
                nLayer = 2;
            }
            else
            {
                ASSERT(FALSE);
            }
            pVertexRtn = pVertex->CreateCloneObject();
        }

        pMapNetwork->ClearNetwork();//·������
        m_pMapNetwork->~IGIS_MapNetwork();
        m_pMemMngOfBuddyData->newfree(m_pMapNetwork);
        m_pMapNetwork = NULL;
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( NULL );
    }
    CATCH(CMemoryException,e)
    {
       TRACE0("·���滮�в����ڴ��쳣!\n"); 
    }
    AND_CATCH(CFileException,e)
    {
       TRACE0("·���滮�в����ļ��쳣!\n"); 
    }
    END_CATCH

    MemoryClear();
    return pVertexRtn;

}

CGIS_NetworkRoadSectionV7* CGIS_NetWork::GetTrunkRoadSectionByBreadthIDAndRoadSectionID(int nBreadthID,int nID)
{
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    CMemoryDispatchForNetworkV2* pDispatch = pSdeControl->GetMemoryDispatchForNetwork();
    ASSERT(pDispatch);
//////////////////////////////////////////////////////////////////////////

    unsigned long nSize = 6*IMemoryMng::MB;
    MemoryInitialBySize( nSize,nSize,nSize);
    CGIS_NetworkRoadSectionV7* pFtrRtn = NULL;


    CGIS_MBBNetwork* pTrunkNetwork = new CGIS_MBBNetwork;
    char* pBeforeCompressBuffer = new char[600000];
    char* pAfterCompressBuffer = new char[600000];
    pTrunkNetwork->SetUnCompressBuffer(pBeforeCompressBuffer,pAfterCompressBuffer,600000,600000);
    CGIS_MBBNetwork::enErrorCode eErrorCode;
    pTrunkNetwork->InitialOperation(&eErrorCode);
    
    int nDetailID;
    CGIS_MBBNetwork::stuPairIDItem* pMergeIDItem;
    //-------------------------------------------------------------
    
    nDetailID = (nBreadthID<<15)|nID;
    pMergeIDItem = pTrunkNetwork->GetVertexMergeIDByDetailIDV2(nDetailID);
    if(pMergeIDItem)
    {       
        int nMGID = (pMergeIDItem->nDestID)>>15;
        pTrunkNetwork->LoadBreadthDataV2(nMGID);
        unsigned long nVertexID = pMergeIDItem->nDestID&0x7fff; 
        CGIS_NetworkRoadSectionV7 *pFtr = pTrunkNetwork->GetNetworkRut(nMGID, nVertexID);
        if(pFtr)
        {
            pFtrRtn = pFtr->CreateCloneObject();
        }
    }



    delete pTrunkNetwork;
    pTrunkNetwork = NULL;
    
    if(pBeforeCompressBuffer) 
    {
        delete pBeforeCompressBuffer;
        pBeforeCompressBuffer = NULL;
    }
    if(pAfterCompressBuffer) 
    {
        delete pAfterCompressBuffer;
        pAfterCompressBuffer = NULL;
    }
    
    MemoryClear();
    return pFtrRtn;

}
CGIS_NetworkVertexV7* CGIS_NetWork::GetTrunkVertexByBreadthIDAndVertexID(CGIS_GeoMap* pMap,int nBreadthID,int nID)
{
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    CMemoryDispatchForNetworkV2* pDispatch = pSdeControl->GetMemoryDispatchForNetwork();
    ASSERT(pDispatch);
//////////////////////////////////////////////////////////////////////////

    unsigned long nSize = 6*IMemoryMng::MB;
    MemoryInitialBySize( nSize,nSize,nSize);
    CGIS_NetworkVertexV7* pFtrRtn = NULL;


    CGIS_MBBNetwork* pTrunkNetwork = new CGIS_MBBNetwork;
	pTrunkNetwork->SetGeoMap( pMap );
    
    char* pBeforeCompressBuffer = new char[600000];
    char* pAfterCompressBuffer = new char[600000];
    pTrunkNetwork->SetUnCompressBuffer(pBeforeCompressBuffer,pAfterCompressBuffer,600000,600000);

    CGIS_MBBNetwork::enErrorCode eErrorCode;
    pTrunkNetwork->InitialOperation(&eErrorCode);
    
    int nDetailID;
    CGIS_MBBNetwork::stuPairIDItem* pMergeIDItem;
    //-------------------------------------------------------------
    
    nDetailID = (nBreadthID<<15)|nID;

    pMergeIDItem = pTrunkNetwork->GetVertexMergeIDByDetailIDV2(nDetailID);
    if(pMergeIDItem)
    {       
        int nMGID = (pMergeIDItem->nDestID)>>15;
        pTrunkNetwork->LoadBreadthDataV2(nMGID);
        unsigned long nVertexID = pMergeIDItem->nDestID&0x7fff; 
        CGIS_NetworkVertexV7 *pFtr = pTrunkNetwork->GetNetworkVtx(nMGID, nVertexID);
        if(pFtr)
        {
            pFtrRtn = pFtr->CreateCloneObject();
        }
    }

    delete pTrunkNetwork;
    pTrunkNetwork = NULL;

    if(pBeforeCompressBuffer) 
    {
        delete pBeforeCompressBuffer;
        pBeforeCompressBuffer = NULL;
    }
    if(pAfterCompressBuffer) 
    {
        delete pAfterCompressBuffer;
        pAfterCompressBuffer = NULL;
    }


    MemoryClear();
    return pFtrRtn;

}

#endif


//////////////////////////////////////////////////////////////////////////
CGIS_NetWork::CCalculateStatusLock::CCalculateStatusLock()
{
    CGIS_NetworkStatus::m_bCalculateStopSign = FALSE;
    CGIS_NetworkStatus::m_nCalculateStatus = 1;

//    SYSTEMTIME st;
//    GetLocalTime(&st);
//    CGIS_NetworkStatus::m_nStartCalculateTime = st.wHour*6+st.wMinute/10;
}
CGIS_NetWork::CCalculateStatusLock::~CCalculateStatusLock()
{
    CGIS_NetworkStatus::m_bCalculateStopSign = FALSE;
    CGIS_NetworkStatus::m_nCalculateStatus = 0;
    CGIS_NetworkStatus::m_nStartCalculateTime = 255;

}

//////////////////////////////////////////////////////////////////////////
