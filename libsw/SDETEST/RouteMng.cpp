// RouteMng.cpp: implementation of the CRouteMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sdetest.h"
#include "RouteMng.h"

#include "SDEControler.h"
#include "MemoryDispatchForSpaceDataV2.h"
#include "GIS_NetworkException.h"

#include "GeoView.h"
#include "GIS_NetWork.h"
#include "GIS_WayPoint.h"
#include "GIS_NetworkRoute.h"
#include "GIS_FeatureRoute.h"

#include "MainFrm.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRouteMng::CRouteMng()
{
    m_pRoute = NULL;
    m_pPathInfo = NULL;
}

CRouteMng::~CRouteMng()
{
    if(m_pRoute)
    {
        delete m_pRoute;
        m_pRoute = NULL;
    }

    if(m_pPathInfo)
    {
        delete m_pPathInfo;
        m_pPathInfo = NULL;
    }


    ClearResult();
}

BOOL CRouteMng::GetRoute(CGIS_FeatureRoute*& pRoute)
{
    if(!m_pRoute)
    {
        m_pRoute = new CGIS_FeatureRoute;
        ASSERT(m_pRoute);
    }

    pRoute = m_pRoute;
    return TRUE;
}

CGIS_FeatureRoute* CRouteMng::GetCurRoute()
{
	return m_pRoute;
}

BOOL CRouteMng::GetPathInfo(CGIS_NetworkPathInfo*& pPathInfo)
{
    if(!m_pPathInfo)
    {
        CString temp = GetProPathName(L"\\map\\voice.txt");
        m_pPathInfo = new CGIS_NetworkPathInfo( temp);
        m_pPathInfo->SetVehicleType(EN_VEHICLETYPE_CAR);
        m_pPathInfo->SetRoutePreference(EN_RUTPREF_FASTERTIME);
    }

    pPathInfo = m_pPathInfo;
    return TRUE;
}

BOOL CRouteMng::ResetSpaceMemory()
{
    //取得SDE控制器
	CSDEControler* pSdeControl = CSDEControler::GetSDEControl();
	ASSERT(pSdeControl);
#ifdef _DEBUG
	pSdeControl->ResetMemoryForSpace();
#endif
    
    return TRUE;
}


/**
 * @brief 
 *
 * @param nCalculateType 0-正常 1-偏航 2-绕路
 * @return
*/
BOOL CRouteMng::CalculateRoute(ST_GEO_PTXY* pPtArray,int nArraySize)
{
    CMainControl* pMainControl = AppGetMainCtrl();
    ASSERT(pMainControl);
    CRouteMng* pRouteMng = pMainControl->GetRouteMng();
    ASSERT(pRouteMng);

    //取得SDE控制器
	CSDEControler* pSdeControl = CSDEControler::GetSDEControl();
	ASSERT(pSdeControl);
	//取得航线计算工具
	IGIS_NetWork *pNetwork = (IGIS_NetWork*)pSdeControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );
	ASSERT(pNetwork);

    CPtrList aWptList;
	TRACE0("========计算航线========\n");
    CGIS_WayPoint * pWpt = NULL;
    for(int i = 0; i < nArraySize; i++)
    {
        pWpt = new CGIS_WayPoint(i);
        char* pSz = new char[6];
        sprintf(pSz,"Wpt%.2d",i);
        pWpt->SetWPAnnoation(pSz);
        pWpt->SetWPAnnoNum(5);
        pWpt->SetWPPosition( pPtArray[i]);
        
        if(i == 0)
        {
            pWpt->SetWPType(EN_WAYPOINTTYPE_ORIGIN);
        }
        else if(i == (nArraySize-1))
        {
            pWpt->SetWPType(EN_WAYPOINTTYPE_DESTINATION);
        }
        else
        {
            pWpt->SetWPType(EN_WAYPOINTTYPE_INTERMEDIATE);
        }

        //TRACE3("\t第%d点:[ %f,%f ]\n",i,pPtArray[i].dx,pPtArray[i].dy);
        aWptList.AddTail(pWpt);
    }

	CGIS_FeatureRoute * pRoute = NULL;
    VERIFY(pRouteMng->GetRoute(pRoute));
	CGIS_NetworkRoute * pNetworkRoute = pRoute->GetNetworkRoute();
    ASSERT(pNetworkRoute);

    {
        //装填航点
        CPtrList * pWptList = pNetworkRoute->GetWayPointList();
        ASSERT(pWptList);
        POSITION pos = pWptList->GetHeadPosition();
        while(pos)
        {
            CGIS_WayPoint* pWpt = (CGIS_WayPoint*)pWptList->GetNext(pos);
            ASSERT(pWpt);
            delete pWpt;
            pWpt = NULL;
        }
        pWptList->RemoveAll();
        pWptList->AddTail( &aWptList );
    }

	CGIS_NetworkPathInfo* pPathInfo = NULL;
    VERIFY(pRouteMng->GetPathInfo(pPathInfo));
	pNetworkRoute->SetNetworkPathInfo( pPathInfo );
	SYSTEMTIME locTime;
	GetLocalTime(&locTime);
	pPathInfo->SetStartCalculateTime(&locTime);

    {
        //删除原来的路径点
        CGIS_NetworkPath* pPath = pNetworkRoute->GetNetworkPath();
        ASSERT(pPath);
       
        CPtrList *pPPList = pPath->GetPathpointList();
        ASSERT( pPPList );
		TRACE1("删除PathPoint %d 个.\n",pPPList->GetCount());
        POSITION pos = pPPList->GetHeadPosition();
        int nIndexN = 0;
        while( pos )
        {
            CGIS_Pathpoint *pPPoint = (CGIS_Pathpoint*)pPPList->GetNext(pos);
            delete pPPoint;


            //TRACE1("nIndexN = %d.\n",nIndexN);
            nIndexN++;
        }
        pPPList->RemoveAll();
	}

	//开始计算航线
	pNetwork->SetNetworkRoute(pNetworkRoute);
#ifdef _DEBUG
    pNetwork->SetMapViewSrc((CGeoView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView());
#endif

	DWORD dwTickStart = GetTickCount();
    int nRtn = pNetwork->CalculateOptimalRoute( 0 );
    switch(nRtn)
    {  
    case 1:
        {
            TRACE0("==返回:1.计算成功\n");    
#ifdef _DEBUG 
     CGeoView* pView = (CGeoView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
         pView->SetNetworkRoute(pNetworkRoute);
 #endif   
        }
        break;
    case 0:
        TRACE0("==返回:0.引用的路网数据已卸载.\n");
        break;
   case -1:
        TRACE0("==返回:-1.附近没有匹配的路段\n");
        break;
    case -2:
        TRACE0("==返回:-2.路网不通\n");
        break;
    case -3:
        TRACE0("==返回:-3.文件I/O异常\n");
        break;
    case -4:
        TRACE0("==返回:-4.内存不足\n");
        break;
    case -5:
        TRACE0("==返回:-5.航线过长,放弃计算\n");
        break;
    case -6:
        TRACE0("==返回:-6.计算条件不足\n");
        break;
    case -7:
        TRACE0("==返回:-7.外部中止计算\n");
        break;
    case -8:
        TRACE0("==返回:-8.主干路网不通\n");
        break;
    default:
        ASSERT(FALSE);
        break;
    }

#ifdef ANALYSE_DATA
    unsigned long nMaxHeapUsedSize = pNetwork->GetMaxHeapUsedSize();
    TRACE2("内存峰值 %dmb(%d bytes).\n",(nMaxHeapUsedSize>>20)+1,nMaxHeapUsedSize);

    if(nMaxHeapUsedSize > m_nMaxHeapUsedSize)
    {
        m_nMaxHeapUsedSize = nMaxHeapUsedSize;
    }
#endif

	DWORD dwTickCount = GetTickCount() - dwTickStart;
	TRACE1("========计算航线用时 %d ms========\n",dwTickCount);


    //////////////////////////////////////////////////////////////////////////
    stuNaviSourceItem* pItem = new stuNaviSourceItem;
    {
        CPtrList* pList = pNetworkRoute->GetWayPointList();
        POSITION pos = pList->GetHeadPosition();
        while(pos)
        {
            CGIS_WayPoint* pWpt = (CGIS_WayPoint*)pList->GetNext(pos);
            pItem->arGeoPoint.push_back(pWpt->GetWPPosition());
        }    
        pItem->nResult = nRtn;
    }
    MapOfNaviSource* pContainer = &m_NaviSourceContainer;
    ArrayOfNaviSourceItem* pItemArray = NULL;
    if(!pContainer->Lookup(pItem->nResult,pItemArray))
    {
        (*pContainer)[pItem->nResult] = new ArrayOfNaviSourceItem;
    }
    pItemArray = (*pContainer)[pItem->nResult];
    ASSERT(pItemArray);
    pItemArray->push_back(pItem);
    //////////////////////////////////////////////////////////////////////////
    
    return 1;
}

BOOL CRouteMng::CalculateRoute_LeeWay(ST_GEO_PTXY* pPtArray,int nArraySize)
{
    ASSERT(pPtArray);
    ASSERT(nArraySize == 1);
    //////////////////////////////////////////////////////////////////////////
    CMainControl* pMainControl = AppGetMainCtrl();
    ASSERT(pMainControl);
    CRouteMng* pRouteMng = pMainControl->GetRouteMng();
    ASSERT(pRouteMng);
    //取得SDE控制器
	CSDEControler* pSdeControl = CSDEControler::GetSDEControl();
	ASSERT(pSdeControl);
	//取得航线计算工具
	IGIS_NetWork *pNetwork = (IGIS_NetWork*)pSdeControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );
	ASSERT(pNetwork);

	CGIS_FeatureRoute * pRoute = NULL;
    VERIFY(pRouteMng->GetRoute(pRoute));
	CGIS_NetworkRoute * pNetworkRoute = pRoute->GetNetworkRoute();
    ASSERT(pNetworkRoute);
    //////////////////////////////////////////////////////////////////////////

	TRACE0("========计算航线========\n");
    CGIS_WayPoint * pWpt = NULL;
    
    pWpt = new CGIS_WayPoint(pNetworkRoute->GetWPMaxID()+1);
    char* pSz = new char[12];
    sprintf(pSz,"LeeWayStart");
    pWpt->SetWPAnnoation(pSz);
    pWpt->SetWPAnnoNum(5);
    pWpt->SetWPPosition( pPtArray[0]);
    pWpt->SetWPType(EN_WAYPOINTTYPE_ORIGIN);
    
    //TRACE3("\t第%d点:[ %f,%f ]\n",i,pPtArray[i].dx,pPtArray[i].dy);
    


	CGIS_NetworkPathInfo* pPathInfo = NULL;
    VERIFY(pRouteMng->GetPathInfo(pPathInfo));
	pNetworkRoute->SetNetworkPathInfo( pPathInfo );
	SYSTEMTIME locTime;
	GetLocalTime(&locTime);
	pPathInfo->SetStartCalculateTime(&locTime);

	//开始计算航线
	pNetwork->SetNetworkRoute(pNetworkRoute);
#ifdef _DEBUG
    pNetwork->SetMapViewSrc((CGeoView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView());
#endif

	DWORD dwTickStart = GetTickCount();
    int nRtn = pNetwork->CalculateOptimalRoute( 0 );
    switch(nRtn)
    {  
    case 1:
        {
            TRACE0("==返回:1.计算成功\n");    
#ifdef _DEBUG 
     CGeoView* pView = (CGeoView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
         pView->SetNetworkRoute(pNetworkRoute);
 #endif   
        }
        break;
    case 0:
        TRACE0("==返回:0.引用的路网数据已卸载.\n");
        break;
   case -1:
        TRACE0("==返回:-1.附近没有匹配的路段\n");
        break;
    case -2:
        TRACE0("==返回:-2.路网不通\n");
        break;
    case -3:
        TRACE0("==返回:-3.文件I/O异常\n");
        break;
    case -4:
        TRACE0("==返回:-4.内存不足\n");
        break;
    case -5:
        TRACE0("==返回:-5.航线过长,放弃计算\n");
        break;
    case -6:
        TRACE0("==返回:-6.计算条件不足\n");
        break;
    case -7:
        TRACE0("==返回:-7.外部中止计算\n");
        break;
    case -8:
        TRACE0("==返回:-8.主干路网不通\n");
        break;
    default:
        ASSERT(FALSE);
        break;
    }

#ifdef ANALYSE_DATA
    unsigned long nMaxHeapUsedSize = pNetwork->GetMaxHeapUsedSize();
    TRACE2("内存峰值 %dmb(%d bytes).\n",(nMaxHeapUsedSize>>20)+1,nMaxHeapUsedSize);

    if(nMaxHeapUsedSize > m_nMaxHeapUsedSize)
    {
        m_nMaxHeapUsedSize = nMaxHeapUsedSize;
    }
#endif

	DWORD dwTickCount = GetTickCount() - dwTickStart;
	TRACE1("========计算航线用时 %d ms========\n",dwTickCount);


    //////////////////////////////////////////////////////////////////////////
    stuNaviSourceItem* pItem = new stuNaviSourceItem;
    {
        CPtrList* pList = pNetworkRoute->GetWayPointList();
        POSITION pos = pList->GetHeadPosition();
        while(pos)
        {
            CGIS_WayPoint* pWpt = (CGIS_WayPoint*)pList->GetNext(pos);
            pItem->arGeoPoint.push_back(pWpt->GetWPPosition());
        }    
        pItem->nResult = nRtn;
    }
    MapOfNaviSource* pContainer = &m_NaviSourceContainer;
    ArrayOfNaviSourceItem* pItemArray = NULL;
    if(!pContainer->Lookup(pItem->nResult,pItemArray))
    {
        (*pContainer)[pItem->nResult] = new ArrayOfNaviSourceItem;
    }
    pItemArray = (*pContainer)[pItem->nResult];
    ASSERT(pItemArray);
    pItemArray->push_back(pItem);
    //////////////////////////////////////////////////////////////////////////


    return FALSE;
}

void CRouteMng::ClearResult()
{
    MapOfNaviSource* pContainer = &m_NaviSourceContainer;
    POSITION pos = pContainer->GetStartPosition();
    int nKey = -1;
    ArrayOfNaviSourceItem* pSubContainer = NULL;
    while(pos)
    {
        pContainer->GetNextAssoc(pos,nKey,pSubContainer);

        if(pSubContainer)
        {
            ArrayOfNaviSourceItem::iterator iteratorOfNaviSourceItemArray;
            for(iteratorOfNaviSourceItemArray = pSubContainer->begin();
            iteratorOfNaviSourceItemArray != pSubContainer->end(); iteratorOfNaviSourceItemArray++)
            {           
                stuNaviSourceItem* pItem = *iteratorOfNaviSourceItemArray;
                ASSERT(pItem);
                delete pItem;
            }
            
            pSubContainer->clear();
            delete pSubContainer;
        }
    }

    pContainer->RemoveAll();

     
    m_nMaxHeapUsedSize = 0;
}


//////////////////////////////////////////////////////////////////////////

void CRouteMng::DumpResult()
{
    int nTotalCalculateCount = GetCountOfAllNaviSource();
    if(nTotalCalculateCount == 0)
        return;


    TRACE0("<<<<******************************************\n");
    TRACE0("CRouteMng infomation\n");
    TRACE2("最大内存开销                : %d bytes(%d mb).\n",m_nMaxHeapUsedSize,(m_nMaxHeapUsedSize>>20)+1);
    TRACE1("共计规划                    : %d.\n",nTotalCalculateCount);
    TRACE2("计算成功                    : %d(%%%.2f).\n",
        GetCountOfNaviSourceByCalculateResult(1),
        (GetCountOfNaviSourceByCalculateResult(1)*100.0f)/nTotalCalculateCount);
    TRACE2("引用的路网数据已卸载        : %d(%%%.2f).\n",
        GetCountOfNaviSourceByCalculateResult(0),
        (GetCountOfNaviSourceByCalculateResult(0)*100.0f)/nTotalCalculateCount);
    TRACE2("附近没有匹配的路段          : %d(%%%.2f).\n",
        GetCountOfNaviSourceByCalculateResult(-1),
        (GetCountOfNaviSourceByCalculateResult(-1)*100.0f)/nTotalCalculateCount);
    TRACE2("路网不通                    : %d(%%%.2f).\n",
        GetCountOfNaviSourceByCalculateResult(-2),
        (GetCountOfNaviSourceByCalculateResult(-2)*100.0f)/nTotalCalculateCount);
    TRACE2("文件I/O异常                 : %d(%%%.2f).\n",
        GetCountOfNaviSourceByCalculateResult(-3),
        (GetCountOfNaviSourceByCalculateResult(-3)*100.0f)/nTotalCalculateCount);
    TRACE2("内存不足                    : %d(%%%.2f).\n",
        GetCountOfNaviSourceByCalculateResult(-4),
        (GetCountOfNaviSourceByCalculateResult(-4)*100.0f)/nTotalCalculateCount);
    TRACE2("航线过长,放弃计算           : %d(%%%.2f).\n",
        GetCountOfNaviSourceByCalculateResult(-5),
        (GetCountOfNaviSourceByCalculateResult(-5)*100.0f)/nTotalCalculateCount);
    TRACE0("******************************************>>>>\n");


    TRACE0("******************************************\n");
    TRACE0("路网不通导致失败的用例:\n");
    DumpNaviSourceByCalculateResult(-2);
    TRACE0("******************************************\n\n");
    TRACE0("******************************************\n");
    TRACE0("内存不足导致失败的用例:\n");
    DumpNaviSourceByCalculateResult(-4);
    TRACE0("******************************************\n\n");

}
int CRouteMng::GetCountOfNaviSourceByCalculateResult(int nResult)
{
    ArrayOfNaviSourceItem* pContainer = NULL;
    if(!m_NaviSourceContainer.Lookup(nResult,pContainer)) 
        return 0;

    ASSERT(pContainer);
    return pContainer->size();
}
int CRouteMng::GetCountOfAllNaviSource()
{
    int nCount = 0;
    MapOfNaviSource* pContainer = &m_NaviSourceContainer;
    POSITION pos = pContainer->GetStartPosition();
    int nKey = -1;
    ArrayOfNaviSourceItem* pSubContainer = NULL;
    while(pos)
    {
        pContainer->GetNextAssoc(pos,nKey,pSubContainer);
        
        if(pSubContainer)
        {
            nCount += pSubContainer->size();
        }
    }
    
    return nCount;
}

void CRouteMng::DumpNaviSourceByCalculateResult(int nResult)
{
    ArrayOfNaviSourceItem* pContainer = NULL;
    if(!m_NaviSourceContainer.Lookup(nResult,pContainer))
        return;

    ASSERT(pContainer);
    ArrayOfNaviSourceItem::iterator iteratorOfNaviSource;
    for(iteratorOfNaviSource = pContainer->begin();
    iteratorOfNaviSource != pContainer->end(); iteratorOfNaviSource++)
    {
        stuNaviSourceItem* pItem = *iteratorOfNaviSource;
        
        if(pItem->nResult ==nResult)
        {
            ArrayOfGeoPoint* pContainer = &pItem->arGeoPoint;
            int nCount = pContainer->size();
            
            if(nCount == 2)
            {
                ArrayOfGeoPoint::iterator iteratorOfGeoPoint;
                for(iteratorOfGeoPoint = pContainer->begin();
                iteratorOfGeoPoint != pContainer->end(); iteratorOfGeoPoint++)
                {
                    ST_GEO_PTXY pt = *iteratorOfGeoPoint;
                    
                    TRACE2("%f\n%f\n",pt.dx,pt.dy);
                }
                TRACE0("同步调用:航线规划\n");
            }
            else if(nCount > 2)
            {
                TRACE1("%d\n",nCount);
                ArrayOfGeoPoint::iterator iteratorOfGeoPoint;
                for(iteratorOfGeoPoint = pContainer->begin();
                iteratorOfGeoPoint != pContainer->end(); iteratorOfGeoPoint++)
                {
                    ST_GEO_PTXY pt = *iteratorOfGeoPoint;
                    
                    TRACE2("%f\n%f\n",pt.dx,pt.dy);
                }
                TRACE0("同步调用:多点航线规划\n");
                
            }
            else
            {
                ASSERT(FALSE);
            }
        }
    }
}

bool CRouteMng::ReCaculRoute(ST_GEO_PTXY ptCur)
{
	CGIS_FeatureRoute * pRoute = NULL;
	pRoute = m_pRoute;
	if( NULL==pRoute ) return false;
	
	CGIS_NetworkRoute * pNWRoute = NULL;
	pNWRoute = pRoute->GetNetworkRoute();
	if( NULL==pNWRoute ) return false;
	CGIS_NetworkPathInfo *pInfo = pNWRoute->GetNetworkPathInfo();
	if (NULL == pInfo) return false;
	SYSTEMTIME locTime;
	GetLocalTime(&locTime);
	pInfo->SetStartCalculateTime(&locTime);
	
	//航线只有2个途径点
	CPtrList * pWList= pNWRoute->GetWayPointList();
	if( NULL==pWList || pWList->GetCount() <= 1 )
	{
		ASSERT( FALSE );
		return false;
	}
	
	CPtrList * pList = pNWRoute->GetNetworkPath()->GetPathpointList();
	if( NULL==pList )	return false;
	if( pList->GetCount()<=0 ) return false;
	
	POSITION psCur		=	NULL;
	POSITION psVoice	=	NULL;
	
	pNWRoute->GetNetworkPath()->GetVoicePosition( psCur, psVoice );
	if( NULL==psCur )	return false;
	
	//找到当前位置的下一个途径点
	int		nIndex = 0;	//中途点列表的索引
	bool	bFlag = true;
	POSITION ps	= pList->GetHeadPosition();
	
	while ( ps && bFlag )
	{
		if ( ps == psCur )	bFlag = false;
		CGIS_Pathpoint * pPPoint = (CGIS_Pathpoint *)pList->GetNext(ps);
		
		int nType = pPPoint->GetPathpointType();
		if ( nType == EN_PATHPOINTTYPE_START||
			nType == EN_PATHPOINTTYPE_INTERMEDIATE||
			nType == EN_PATHPOINTTYPE_END ) 
			
			nIndex++;		 
	}
	
	CGIS_WayPoint * pW = NULL;
	
	// 删除下一个中途点
	for(int i=0; i<nIndex; i++)
	{
		pW = (CGIS_WayPoint*)pWList->GetHead();
		delete pW;
		pW = NULL;
		pWList->RemoveHead();
	}
	
	//清空PathPointList
	POSITION pos = pList->GetHeadPosition();
	while ( pos )
	{
		CGIS_Pathpoint * pPPoint = (CGIS_Pathpoint *)pList->GetNext( pos );
		if ( pPPoint!=NULL )	delete pPPoint;
	}
	pList->RemoveAll();
	
	CGIS_WayPoint * pTmp = (CGIS_WayPoint *)pWList->GetHead();
	ST_GEO_PTXY ptTmp;
	ptTmp = pTmp->GetWPPosition();
	double dis = CalcDist(ptCur.dx,ptCur.dy,ptTmp.dx,ptTmp.dy);
	if(dis >= 5)
	{
		CGIS_WayPoint * pCurWPoint = new CGIS_WayPoint( pNWRoute->GetWPMaxID()+1);
		pCurWPoint->SetWPPosition( ptCur );
		char * szAnno = new char[5];
		strcpy(szAnno,"起点");
		szAnno[4] = '\0';
		pCurWPoint->SetWPAnnoation(szAnno);
		pCurWPoint->SetWPAnnoNum(5);
		pCurWPoint->SetWPType(EN_WAYPOINTTYPE_ORIGIN);
		pWList->AddHead(pCurWPoint);
	}
	else
	{
		pTmp->SetWPType(EN_WAYPOINTTYPE_ORIGIN);
	}
	
	CGIS_WayPoint * pEnd = (CGIS_WayPoint *)pWList->GetTail();
	pEnd->SetWPType(EN_WAYPOINTTYPE_DESTINATION);

	CSDEControler* pControl = CSDEControler::GetSDEControl();
	if( pControl == NULL )	
		return false;
	
	IGIS_NetWork *pNetwork = (IGIS_NetWork*)pControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );
	if( pNetwork == NULL  )
		return false;

	int nRes = pNetwork->CalculateOptimalRoute(0, (CGIS_WayPoint*)pWList->GetHead());
    
	switch(nRes)
    {  
    case 1:
        {
            TRACE0("==返回:1.计算成功\n");    
#ifdef _DEBUG 
     CGeoView* pView = (CGeoView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();
         pView->SetNetworkRoute(pNWRoute);
#endif
        }
        break;
    case 0:
        TRACE0("==返回:0.引用的路网数据已卸载.\n");
        break;
   case -1:
        TRACE0("==返回:-1.附近没有匹配的路段\n");
        break;
    case -2:
        TRACE0("==返回:-2.路网不通\n");
        break;
    case -3:
        TRACE0("==返回:-3.文件I/O异常\n");
        break;
    case -4:
        TRACE0("==返回:-4.内存不足\n");
        break;
    case -5:
        TRACE0("==返回:-5.航线过长,放弃计算\n");
        break;
    case -6:
        TRACE0("==返回:-6.计算条件不足\n");
        break;
    case -7:
        TRACE0("==返回:-7.外部中止计算\n");
        break;
    case -8:
        TRACE0("==返回:-8.主干路网不通\n");
        break;
    default:
        ASSERT(FALSE);
        break;
    }
	return true;	
}
//////////////////////////////////////////////////////////////////////////