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
 * @brief   调试函数库(只在调试版本中被定义)
 *
 * 日志文件功能
 * 调试输出到屏幕功能
 * 函数调用跟踪功能
 *
 *    
 * @note    如果不想在调试版本中使能这些宏中的一项或几项,应该将这些宏在local文
 *          件中重新定义为空,即打开下面相应的注释即可.       
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
 * @brief 计算最佳航线
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

	//地图管理器或者自身维护的航线为空则失败
	if( m_pMapManager == NULL )
		return 0;
	if( m_pNetworkRoute == NULL )
		return 0;


	//如果航线的点的数量小于2个则返回失败
	CPtrList *pWptList = m_pNetworkRoute->GetWayPointList();
	if( pWptList->GetCount() < 2 )
		return 0;

    TRACE0("<<<<******************************************\n");
	//////////////////////////////////////////////////////////////////////////
	//遍历所有点
	POSITION pos = pWptList->GetHeadPosition();
	CGIS_WayPoint *pWPPoint = NULL;
	while( pos )
	{
		pWPPoint = (CGIS_WayPoint*)pWptList->GetNext(pos);
		if( pWPPoint )
			pWPPoint->ClearWPRutInfo();
	}

	//////////////////////////////////////////////////////////////////////////
	//遍历路径所有点并删除
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
	//为什么是取得id为2的地图
	IGIS_Map *pMap = m_pMapManager->GetMapByID( 2 );
	int nCFlag = 0;
	if( pMap )
    {
          
        CPtrList* pWptList = m_pNetworkRoute->GetWayPointList();
        
        TRACE0("----航线规划条件检查--\n");
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
                    WATCH("路径计算");
                    WATCH2LOG("路径计算");
                    nCFlag = CalculateOptimalRouteInMap(
                        pMap,
                        pWptList,
                        m_pNetworkRoute->GetNetworkPath()->GetPathpointList());
                }
                
                MemoryClear();
                
                //-------------------------------------------------------------
                //如果是内存不足引发失败且首次并未全力分配内存,则全力分配后再次计算
                if(CGIS_NetworkException::ERROR_OUT_OF_MEMORY == nCFlag)
                {
                    if(dwActualSize < dwMaxSize)
                    {
                        TRACE0("内存全力分配,再次计算!\n");
                        WRITE2LOG0("内存全力分配,再次计算!\n");
                        
                        DWORD dwRequireSize = dwMaxSize;
                        if(MemoryInitialBySize(dwRequireSize,dwActualSize,dwMaxSize))
                        {              
                            
                            {
                                WATCH("路径计算");
                                WATCH2LOG("路径计算");
                                
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
                WATCH("机动计算");
                WATCH2LOG("机动计算");
                
                TRACE0("----开始进行机动计算\n");
                int nErrorCode = CGIS_NetworkException::ERROR_NONE;
                BOOL bSuccess = m_pNetworkRoute->GetNetworkPath()->CalcuatePathTurnList(&nErrorCode);
                if(!bSuccess)
                {
                    TRACE0("机动计算失败!\n");
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
///parameter: byWPType 1.表示为航线上的目的地 2.表示航点是在航线上找得一点建立的目标点
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


            //有找到目的地点则直接返回
			if( pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE ||
					pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_START ||
					pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_END )
			{
				pWPoint = ((CGIS_PathpointMark*)pPPoint)->GetWayPoint();
				byWPType = 1;
                TRACE0("直接以目的地为目标点.\n");
				break;
			}

            

            //以拐点构造目标点
			if( dbDis > dbMDis && pPPoint->GetPathpointType() == EN_PATHPOINTTYPE_INFLEXION )
			{
                //新建途经点
				pWPoint = new CGIS_WayPoint( m_pNetworkRoute->GetWPMaxID()+1 );
				char *szAnno = new char[5];
				memcpy( szAnno, "终点", 5 );
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
                TRACE0("以航线的点构造目标点.\n");
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
	//所需删除的途径点个数
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
    //删除当前已经过的所有途径点
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
//        TRACE0("删除目的地之前的所有途经点\n");
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
//        TRACE0("删除当前车辆经过的所有途经点\n");
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
     * @brief 路线规划
     *
     * @param byCType-(0.正常的路线规划 1.偏航时的路线规划 2.绕路时路线规划),
     *       对于byCType为1.2两种情况需要传入车辆的起点
     *              
     * @return  1.计算成功
     *          0.路网数据已卸载     
     *         -1.附近没有匹配的路段         
     *         -2.路网不通     
     *         -3.文件I/O异常     
     *         -4.内存不足
     *         -5.航线过长,放弃计算.
     *         -6.计算条件不足
     *         -7.计算被外部终止
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

    //取出途经点列表
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
            //构建一条新航线去计算(当前点到目标点)
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
                    //如果是构建出来的点的话去掉最后一个PathPoint点
					CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPathPointListNew->GetTail();
					pPathPointListNew->RemoveTail();
					delete pPPoint;
					delete pWPDest;
				}
                //////////////////////////////////////////////////////////////////////////
                /*程明添加下述代码.
                针对计算机动时会丢失途中点的BUG.
                    原因,在偏航和绕路的计算时会改变wpt内部数据,
                    修正方法.计算时要还原此数据.
                    */
				else if( byWPType==1 )
				{
                    //
					CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPathPointListNew->GetTail();
					if( pPPoint && enWPType == EN_WAYPOINTTYPE_INTERMEDIATE )
						pPPoint->SetPathpointType(EN_PATHPOINTTYPE_INTERMEDIATE);
				}
                //////////////////////////////////////////////////////////////////////////
				//把新航线的点添加到老航线的头部
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
                //计算失败
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
            TRACE0("构造偏航/绕路 目标点失败.\n");
        }
	}
	else
    {
        //正常计算
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
 * @brief 计算指定地图上的最佳航线
 *
 * @param pMap 地图
 * @param WPList 点列表
 * @param pDesPointList 目标点列表

 * @return BOOL
    2007-1-23日拟定了新的策略如下.
    以当前计算的总计直线距离求出最小内存需求dwMinMemorySize和最大内存需求dwMaxMemorySize,
    例如总距离<60km则dwMinMemorySize为4m,dwMaxMemorySize为12m
   
*/
int CGIS_NetWork::CalculateOptimalRouteInMap(IGIS_Map *pMap,CPtrList* pWptList,CPtrList* pDesPointList)
{

    int bRT  = 0;
     
    TRY
    { 
        //新建图幅网格对象
        ASSERT(m_pMemMngOfBuddyData);
        //CGIS_MapBreadthNetwork *pMapNetwork = new CGIS_MapBreadthNetwork;
        CGIS_MapBreadthNetwork *pObjAddr = (CGIS_MapBreadthNetwork *)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_MapBreadthNetwork));
        if(!pObjAddr)
            return CGIS_NetworkException::ERROR_OUT_OF_MEMORY;
        CGIS_MapBreadthNetwork *pMapNetwork = new (pObjAddr) CGIS_MapBreadthNetwork;
        
        m_pMapNetwork = pMapNetwork;
        //ASSERT(pMap->IsKindOf(RUNTIME_CLASS(CGIS_GeoMap)));
        
        //关联地图网格与地图
        pMapNetwork->SetGeoMap( (CGIS_GeoMap*)pMap );
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( pMapNetwork );
        //关联地图网格与网格航线
        pMapNetwork->SetNetworkRoute( m_pNetworkRoute );
        
        
        //////////////////////////////////////////////////////////////////////////
        //清空目标点(计算好的路径点)列表
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
        //重新计算目标点列表
        bRT = pMapNetwork->CalculateNavigatePath( pWptList,pDesPointList );
      
        //清理操作
		{
			CSpaceMemoryLock aLock;    
			((CGIS_GeoMap*)pMap)->ClearStorageBMap();//地图清理
		}
        pMapNetwork->ClearNetwork();//路网清理
        m_pMapNetwork->~IGIS_MapNetwork();
        m_pMemMngOfBuddyData->newfree(m_pMapNetwork);
        m_pMapNetwork = NULL;
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( NULL );
    }
    CATCH(CMemoryException,e)
    {
       TRACE0("路网规划中捕获内存异常!\n"); 
    }
    AND_CATCH(CFileException,e)
    {
       TRACE0("路网规划中捕获文件异常!\n"); 
    }
    END_CATCH

#ifdef ANALYSE_DATA
    m_dwMaxHeapUsedSize = m_pMemMngOfBuddyData->GetMaxHeapUsedSize();
    TRACE3("路径计算内存占用: [%d mb][%d kb][%d bytes].\n",m_dwMaxHeapUsedSize>>20,m_dwMaxHeapUsedSize>>10,m_dwMaxHeapUsedSize);    
    DBG2LOG3("路径计算内存占用: [%d mb][%d kb][%d bytes].\n",m_dwMaxHeapUsedSize>>20,m_dwMaxHeapUsedSize>>10,m_dwMaxHeapUsedSize);
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
//			TRACE0("Path对象发出停止导航计算的指令..\n");
//			pPath->CalculateStop();
//		}
//	}
//	
//	if(m_pMapNetwork)
//	{
//		TRACE0("路网对象发出停止导航计算的指令..\n");
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
    
    //单段距离大于此值不算
    if(dbMaxLegDist > 6000000.0f)
        return FALSE;

    //////////////////////////////////////////////////////////////////////////
    //上面都是错误的情况.
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
        TRACE0("航线过长,无法计算!\n");
        return FALSE;
    }
    
    //////////////////////////////////////////////////////////////////////////
    //分配资源
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    CMemoryDispatchForNetworkV2* pDispatch = pSdeControl->GetMemoryDispatchForNetwork();
    ASSERT(pDispatch);
    
    //分配所需要的空间
    if(!pDispatch->GetMemoryMngV3(dwMinNeedSize,dwMaxNeedSize,dwAllocateActualSize,dwAllocateMaxSize))
    {
        TRACE0("Can't get the enough memory for network.\n");
        return FALSE;
    }
    
    TRACE1("Reverse %dm memory for calculate route.\n",dwAllocateActualSize>>20);
    VERIFY(pDispatch->MemoryInitial());
    //////////////////////////////////////////////////////////////////////////
    //上面都是错误的情况.
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
    
    //分配所需要的空间
    if(!pDispatch->GetMemoryMngV3(dwRequireSize,dwRequireSize,dwAllocateActualSize,dwAllocateMaxSize))
    {
        TRACE0("Can't get the enough memory for network.\n");
        return FALSE;
    }

    TRACE1("Reverse %dm memory for calculate route.\n",dwAllocateActualSize>>20);
    VERIFY(pDispatch->MemoryInitial());
    //////////////////////////////////////////////////////////////////////////
    //上面都是错误的情况.
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
        //新建图幅网格对象
        ASSERT(m_pMemMngOfBuddyData);
        //CGIS_MapBreadthNetwork *pMapNetwork = new CGIS_MapBreadthNetwork;
        CGIS_MapBreadthNetwork *pObjAddr = (CGIS_MapBreadthNetwork *)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_MapBreadthNetwork));
        ASSERT(pObjAddr);
        CGIS_MapBreadthNetwork *pMapNetwork = new (pObjAddr) CGIS_MapBreadthNetwork;
        
        m_pMapNetwork = pMapNetwork;
        //ASSERT(pMap->IsKindOf(RUNTIME_CLASS(CGIS_GeoMap)));
        
        //关联地图网格与地图
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

        pMapNetwork->ClearNetwork();//路网清理
        m_pMapNetwork->~IGIS_MapNetwork();
        m_pMemMngOfBuddyData->newfree(m_pMapNetwork);
        m_pMapNetwork = NULL;
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( NULL );
    }
    CATCH(CMemoryException,e)
    {
       TRACE0("路网规划中捕获内存异常!\n"); 
    }
    AND_CATCH(CFileException,e)
    {
       TRACE0("路网规划中捕获文件异常!\n"); 
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
        //新建图幅网格对象
        ASSERT(m_pMemMngOfBuddyData);
        //CGIS_MapBreadthNetwork *pMapNetwork = new CGIS_MapBreadthNetwork;
        CGIS_MapBreadthNetwork *pObjAddr = (CGIS_MapBreadthNetwork *)m_pMemMngOfBuddyData->newmalloc(sizeof(CGIS_MapBreadthNetwork));
        ASSERT(pObjAddr);
        CGIS_MapBreadthNetwork *pMapNetwork = new (pObjAddr) CGIS_MapBreadthNetwork;
        
        m_pMapNetwork = pMapNetwork;
        //ASSERT(pMap->IsKindOf(RUNTIME_CLASS(CGIS_GeoMap)));
        
        //关联地图网格与地图
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

        pMapNetwork->ClearNetwork();//路网清理
        m_pMapNetwork->~IGIS_MapNetwork();
        m_pMemMngOfBuddyData->newfree(m_pMapNetwork);
        m_pMapNetwork = NULL;
        ((CGIS_GeoMap*)pMap)->SetMapNetWork( NULL );
    }
    CATCH(CMemoryException,e)
    {
       TRACE0("路网规划中捕获内存异常!\n"); 
    }
    AND_CATCH(CFileException,e)
    {
       TRACE0("路网规划中捕获文件异常!\n"); 
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
