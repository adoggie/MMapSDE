// CommandAction.cpp: implementation of the CCommandAction class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "../SDETest.h"
#include "../MainControl.h"
#include "../RouteMng.h"
#include "../MainFrm.h"
#include "GeoView.h"
#include "IGIS_Query2.h"
#include "../DlgCommonList.h"
#include "CommandAction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
#define TRACELOCALTIME {SYSTEMTIME st;GetLocalTime(&st);TRACE3("======当地时间:%.2d:%.2d:%.2d====================\n",st.wHour,st.wMinute,st.wSecond);}
//////////////////////////////////////////////////////////////////////////
static void CALLBACK UpdateProcForQueryProvince(IGIS_Query2::enUpdateAspect eAspect, DWORD dwPara);
static void CALLBACK UpdateProcForQueryCity(IGIS_Query2::enUpdateAspect eAspect, DWORD dwPara);
static void CALLBACK UpdateProcForQueryRoad(IGIS_Query2::enUpdateAspect eAspect, DWORD dwPara);
static void CALLBACK UpdateProcForQueryCrossRoad(IGIS_Query2::enUpdateAspect eAspect , DWORD dwPara);
static void CALLBACK UpdateProcForQueryIndexOfPOI(IGIS_Query2::enUpdateAspect eAspect, DWORD dwPara);
static void CALLBACK UpdateProcForQueryPOIByIndex(IGIS_Query2::enUpdateAspect eAspect, DWORD dwPara);
static void CALLBACK UpdateProcForQueryPOINearest(IGIS_Query2::enUpdateAspect eAspect, DWORD dwPara);
//////////////////////////////////////////////////////////////////////////
static int QueryPOILatLog(CStringArray& arString,double& dLat,double& dLog);
//////////////////////////////////////////////////////////////////////////
CProvinceObject* g_pProvinceObject = NULL;
CCityObject* g_pCityObject = NULL;
CRoadObject* g_pRoadObject = NULL;
CPtrList* g_pCrossRoadList = NULL;
CStIndexOfPOI* g_pIndexOfPOI = NULL;
CPtrList* g_pIndexOfPOIList = NULL;
CPtrList* g_pGisFeatureList = NULL;
CStGISFeature* g_pGisFeature = NULL;
//////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommandAction::CCommandAction()
{
	
}

CCommandAction::~CCommandAction()
{

}

int CCommandAction:: DumpResult(CStringArray& arString)
{
    AppGetMainCtrl()->GetRouteMng()->DumpResult();
    return 1;
}
int CCommandAction:: ClearResult(CStringArray& arString)
{
	AppGetMainCtrl()->GetRouteMng()->ClearResult();   
    return 1;
}


int CCommandAction:: MoveToPOI(CStringArray& arString)
{
    double dLat;
    double dLog;
    if(!QueryPOILatLog(arString,dLat,dLog))
        return 0;

    CGeoView* pView = (CGeoView*)((CMainFrame*) AfxGetMainWnd())->GetActiveView();
    pView->MoveToCenter(dLog,dLat);
    pView->Refresh();

       
    return 1;
}

int CCommandAction:: MoveToCrossRoad(CStringArray& arString)
{

	if( 3 != arString.GetSize())
		return 0;

    IGIS_Query2* pGIS_Query2 = AppGetMainCtrl()->GetQueryTool();
	ASSERT(pGIS_Query2 != NULL);
	
	IGIS_Query2::tagSearchCondition aSearchCondition;
	aSearchCondition.nCountryId = 86;
	aSearchCondition.strMatchText = arString[0];
	aSearchCondition.nConditionMask = IGIS_Query2::CONDITION_COUNTRY|IGIS_Query2::CONDITION_MATCH_TEXT;
	aSearchCondition.nResult = IGIS_Query2::OBJECT_PROVINCE;

    aSearchCondition.lpfnUpdateProc = UpdateProcForQueryProvince;
	VERIFY(pGIS_Query2->SetCondition( aSearchCondition ));
	
	pGIS_Query2->SearchStart();   
    
    while(!pGIS_Query2->SetCondition(aSearchCondition))
    {
        Sleep(100);
    }

    if(!g_pProvinceObject)
        return 0;

	aSearchCondition.nProvinceId = g_pProvinceObject->m_nId;
	aSearchCondition.strMatchText = arString[1];
    aSearchCondition.nConditionMask = IGIS_Query2::CONDITION_PROVINCE|IGIS_Query2::CONDITION_MATCH_TEXT;
	aSearchCondition.nResult = IGIS_Query2::OBJECT_CITY;

    aSearchCondition.lpfnUpdateProc = UpdateProcForQueryCity;
	VERIFY(pGIS_Query2->SetCondition( aSearchCondition ));
    
	pGIS_Query2->SearchStart();   
    
    while(!pGIS_Query2->SetCondition(aSearchCondition))
    {
        Sleep(100);
    }

    delete g_pProvinceObject;
    g_pProvinceObject = NULL;
    if(!g_pCityObject)
        return 0;
    
    aSearchCondition.nCityId = g_pCityObject->m_nId;
	aSearchCondition.strMatchText = arString[2];
    aSearchCondition.nConditionMask = IGIS_Query2::CONDITION_CITY|IGIS_Query2::CONDITION_MATCH_TEXT;
	aSearchCondition.nResult = IGIS_Query2::OBJECT_ROAD;

    aSearchCondition.lpfnUpdateProc = UpdateProcForQueryRoad;
	VERIFY(pGIS_Query2->SetCondition( aSearchCondition ));
    
	pGIS_Query2->SearchStart();   

    while(!pGIS_Query2->SetCondition(aSearchCondition))
    {
        Sleep(100);
    }

    delete g_pCityObject;
    g_pCityObject = NULL;
    if(!g_pRoadObject)
        return 0;
    

	aSearchCondition.strRoad = g_pRoadObject->m_strName;
    aSearchCondition.nConditionMask = IGIS_Query2::CONDITION_CITY|IGIS_Query2::CONDITION_ROAD;
	aSearchCondition.nResult = IGIS_Query2::OBJECT_CROSSROAD;

    aSearchCondition.lpfnUpdateProc = UpdateProcForQueryCrossRoad;
	VERIFY(pGIS_Query2->SetCondition( aSearchCondition ));
    
	pGIS_Query2->SearchStart();   

    while(!pGIS_Query2->SetCondition(aSearchCondition))
    {
        Sleep(100);
    }

    delete g_pCityObject;
    g_pCityObject = NULL;
    if(!g_pCrossRoadList)
        return 0;


    CRoadPointObject* pObject = (CRoadPointObject*)g_pCrossRoadList->GetHead();
    if(g_pCrossRoadList->GetCount() > 1)
    {
        CDlgCommonList aDlg(NULL,g_pCrossRoadList);
        if(IDOK == aDlg.DoModal())
        {
            int nItem = aDlg.GetSelect();
            pObject = (CRoadPointObject*)g_pCrossRoadList->GetAt(g_pCrossRoadList->FindIndex(nItem));
        }
    }
    
    CGeoView* pView = (CGeoView*)((CMainFrame*) AfxGetMainWnd())->GetActiveView();
    pView->MoveToCenter(pObject->m_dx,pObject->m_dy);
    pView->Refresh();

    POSITION pos = g_pCrossRoadList->GetHeadPosition();
    while(pos)
    {
        CQueryCommonObject* pObject = (CQueryCommonObject*)g_pCrossRoadList->GetNext(pos);
        ASSERT(pObject);
        delete pObject;
    }
    g_pCrossRoadList->RemoveAll();
    delete g_pCrossRoadList;
    g_pCrossRoadList = NULL;
    
    
    return 1;
}

int CCommandAction::DuoDianGuihuahangxian(CStringArray& arString)
{
	if( arString.GetSize() < 1)
		return 0;

    int nPtCount = -1;
  	_stscanf(arString[0],_T("%d"), &nPtCount);
    ASSERT(nPtCount > 0);
    ASSERT(nPtCount <= 10);

    if(arString.GetSize() != (nPtCount*2+1))
        return 0;

    ST_GEO_PTXY aPrArray[10];
    for(int i = 0; i < nPtCount; i++)
    {
	    _stscanf(arString[i*2+1],_T("%f"), &aPrArray[i].dx);
    	_stscanf(arString[i*2+2],_T("%f"), &aPrArray[i].dy);
    }

	AppGetMainCtrl()->GetRouteMng()->CalculateRoute(&aPrArray[0],nPtCount);
	return 1;
}

int CCommandAction::Guihuahangxian(CStringArray& arString)
{
	if( 4 != arString.GetSize())
		return 0;

    ST_GEO_PTXY aPrArray[6];
	_stscanf(arString[0],_T("%f"), &aPrArray[0].dx);
	_stscanf(arString[1],_T("%f"), &aPrArray[0].dy);
	_stscanf(arString[2],_T("%f"), &aPrArray[1].dx);
	_stscanf(arString[3],_T("%f"), &aPrArray[1].dy);

	AppGetMainCtrl()->GetRouteMng()->CalculateRoute(&aPrArray[0],2);

	return 1;
}

int CCommandAction::SetRoutePreference(CStringArray& arString)
{
	if( 1 != arString.GetSize())
		return 0;

    int nCalculatePreference = 0;
  	_stscanf(arString[0],_T("%d"), &nCalculatePreference);
    ASSERT(nCalculatePreference >= 0);
    ASSERT(nCalculatePreference < 4);
    
    CGIS_NetworkPathInfo* pPathInfo = NULL;
	AppGetMainCtrl()->GetRouteMng()->GetPathInfo(pPathInfo);
    pPathInfo->SetRoutePreference((tagENROUTEPREFERENCE)nCalculatePreference);

	return 1;
}

int CCommandAction::GuihuahangxianPOIToPOI(CStringArray& arString)
{
	if( 6 != arString.GetSize())
		return 0;

    ST_GEO_PTXY aPtArray[2];

    {
        CStringArray arStringTemp;
        arStringTemp.Add(arString[0]);
        arStringTemp.Add(arString[1]);
        arStringTemp.Add(arString[2]);
        double dLat;
        double dLog;
        if(!QueryPOILatLog(arStringTemp,dLat,dLog))
        {         
            ASSERT(FALSE);
            return 0;
        }

        aPtArray[0].dx = dLog;
        aPtArray[0].dy = dLat;
    }

    {
        CStringArray arStringTemp;
        arStringTemp.Add(arString[3]);
        arStringTemp.Add(arString[4]);
        arStringTemp.Add(arString[5]);
        double dLat;
        double dLog;
        if(!QueryPOILatLog(arStringTemp,dLat,dLog))
        {         
            ASSERT(FALSE);
            return 0;
        }

        aPtArray[1].dx = dLog;
        aPtArray[1].dy = dLat;
    }
	AppGetMainCtrl()->GetRouteMng()->CalculateRoute(&aPtArray[0],2);

    return 1;
}


int CCommandAction::CircleGuihuahangxian(CStringArray& arString)
{
	if( 5 != arString.GetSize())
		return 0;

    int nCycleCount = 0;
  	_stscanf(arString[0],_T("%d"), &nCycleCount);
 
    ST_GEO_PTXY aPrArray[2];
    {
        for(int i = 0; i < 2; i++)
        {
            _stscanf(arString[i*2+1],_T("%f"), &aPrArray[i].dx);
            _stscanf(arString[i*2+2],_T("%f"), &aPrArray[i].dy);
        }
    }

	for(int i = 0; i < nCycleCount; i++)
	{
		AppGetMainCtrl()->GetRouteMng()->CalculateRoute(&aPrArray[0],2);
	}

	return 1; 
}
int CCommandAction::ResetSpaceMemory(CStringArray& arString)
{
	if( 2 != arString.GetSize())
		return 0;

    int nCycleCount = 0;
  	_stscanf(arString[0],_T("%d"), &nCycleCount);
    DWORD dwDelayMilliseconds = 0;
  	_stscanf(arString[1],_T("%d"), &dwDelayMilliseconds);
 
	for(int i = 0; i < nCycleCount; i++)
	{
        Sleep(dwDelayMilliseconds);
		AppGetMainCtrl()->GetRouteMng()->ResetSpaceMemory();
	}

	return 1; 
}
static CGeoView* s_pView = NULL;
int CCommandAction::MoveToCenter(CStringArray& arString)
{
	if( 2 != arString.GetSize())
		return 0;

	
	ST_GEO_PTXY aPrArray[1];
	_stscanf(arString[0],_T("%f"), &aPrArray[0].dx);
	_stscanf(arString[1],_T("%f"), &aPrArray[0].dy);

	
	CGeoView* pView = (CGeoView*)((CMainFrame*) AfxGetMainWnd())->GetActiveView();
    s_pView = pView;
    pView->MoveToCenter(aPrArray[0].dx,aPrArray[0].dy);
    pView->Refresh();

	return 1; 
}

int CCommandAction::ZoomIn(CStringArray& arString)
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ZOOMIN,0);
    return 1;
}
int CCommandAction::ZoomOut(CStringArray& arString)
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ZOOMOUT,0);
    return 1;
}
/**
 * @brief 地图随机移动
 *
 * @param 
 * @return
*/
int CCommandAction::CircleRandomMove(CStringArray& arString)
{
	if( 2 != arString.GetSize())
		return 0;

    int nCycleCount = 0;
  	_stscanf(arString[0],_T("%d"), &nCycleCount);
    DWORD dwDelayMilliseconds = 0;
  	_stscanf(arString[1],_T("%d"), &dwDelayMilliseconds);


    //CGeoView* pView = (CGeoView*)((CMainFrame*) AfxGetMainWnd())->GetActiveView();
    CGeoView* pView = s_pView;
 
    CPoint aPt(100,100);
    double dx ,dy;

    for(int i = 0; i < nCycleCount; i++)
	{
        Sleep(dwDelayMilliseconds);

        switch(rand()%4)
        {
        case 0:
            aPt.x++;
            break;
        case 1:
            aPt.y++;
            break;
        case 2:
            aPt.x--;
            break;
        case 3:
            aPt.y--;
            break;
        default:
            ASSERT(FALSE);
            break;
        }
                
        pView->XyViewToMap( aPt.x, aPt.y , dx , dy );
        pView->MoveToCenter(dx,dy);
        pView->Refresh();
    }


	return 1; 
}
/**
 * @brief 循环匹配道路
 *
 * @param 
 * @return
*/
int CCommandAction::CircleMatchRoad(CStringArray& arString)
{

   	return 1; 
}
/**
 * @brief 循环偏航
 *
 * @param 
 * @return
*/
int CCommandAction::CircleLeeway(CStringArray& arString)
{
	if( 5 != arString.GetSize())
		return 0;

    int nCycleCount = 0;
  	_stscanf(arString[0],_T("%d"), &nCycleCount);
 
    ST_GEO_PTXY aPrArray[2];
    {
        for(int i = 0; i < 2; i++)
        {
            _stscanf(arString[i*2+1],_T("%f"), &aPrArray[i].dx);
            _stscanf(arString[i*2+2],_T("%f"), &aPrArray[i].dy);
        }
    }

	for(int i = 0; i < nCycleCount; i++)
	{
		AppGetMainCtrl()->GetRouteMng()->CalculateRoute(&aPrArray[0],2);
	}

	return 1; 
    return 1;
}

static void CALLBACK UpdateProcForQueryProvince(IGIS_Query2::enUpdateAspect eAspect , DWORD dwPara)
{
	switch( eAspect )
	{
	case IGIS_Query2::UPDATE_NEW_OBJECT:
		{
			CObject* pObj = (CObject*)dwPara;
			ASSERT(pObj && pObj->IsKindOf(RUNTIME_CLASS(CObject)));			
			
			
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CProvinceObject)));
			CProvinceObject * pObject = (CProvinceObject*)pObj;

            g_pProvinceObject = pObject;		
		}
		break;
	case IGIS_Query2::UPDATE_STOP:
		{

		}
		break;
	default:
		ASSERT( FALSE );
		break;
	}
}

static void CALLBACK UpdateProcForQueryCity(IGIS_Query2::enUpdateAspect eAspect , DWORD dwPara)
{
	switch( eAspect )
	{
	case IGIS_Query2::UPDATE_NEW_OBJECT:
		{
			CObject* pObj = (CObject*)dwPara;
			ASSERT(pObj && pObj->IsKindOf(RUNTIME_CLASS(CObject)));			
			
			
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CCityObject)));
			CCityObject * pObject = (CCityObject*)pObj;

            if(!g_pCityObject)
            {
                g_pCityObject = pObject;		
            }
			
		}
		break;
	case IGIS_Query2::UPDATE_STOP:
		{

		}
		break;
	default:
		ASSERT( FALSE );
		break;
	}
}

static void CALLBACK UpdateProcForQueryRoad(IGIS_Query2::enUpdateAspect eAspect , DWORD dwPara)
{
	switch( eAspect )
	{
	case IGIS_Query2::UPDATE_NEW_OBJECT:
		{
			CObject* pObj = (CObject*)dwPara;
			ASSERT(pObj && pObj->IsKindOf(RUNTIME_CLASS(CObject)));			
			
			
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CRoadObject)));
			CRoadObject * pObject = (CRoadObject*)pObj;
			
            g_pRoadObject = pObject;		
		}
		break;
	case IGIS_Query2::UPDATE_STOP:
		{

		}
		break;
	default:
		ASSERT( FALSE );
		break;
	}
}

static void CALLBACK UpdateProcForQueryIndexOfPOI(IGIS_Query2::enUpdateAspect eAspect , DWORD dwPara)
{
	switch( eAspect )
	{
	case IGIS_Query2::UPDATE_NEW_OBJECT:
		{
			CObject* pObj = (CObject*)dwPara;
			ASSERT(pObj && pObj->IsKindOf(RUNTIME_CLASS(CObject)));			
			
			
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CStIndexOfPOI)));
			CStIndexOfPOI * pObject = (CStIndexOfPOI*)pObj;
			
            g_pIndexOfPOI = pObject;		
		}
		break;
	case IGIS_Query2::UPDATE_STOP:
		{

		}
		break;
	default:
		ASSERT( FALSE );
		break;
	}
}

static void CALLBACK UpdateProcForQueryCrossRoad(IGIS_Query2::enUpdateAspect eAspect , DWORD dwPara)
{
	switch( eAspect )
	{
	case IGIS_Query2::UPDATE_NEW_OBJECT:
		{
			CObject* pObj = (CObject*)dwPara;
			ASSERT(pObj && pObj->IsKindOf(RUNTIME_CLASS(CObject)));			
			
			
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CRoadPointObject)));
			CRoadPointObject * pObject = (CRoadPointObject*)pObj;
			
            if(!g_pCrossRoadList)	
            {
                g_pCrossRoadList = new CPtrList;
            }

            g_pCrossRoadList->AddTail(pObject);
		}
		break;
	case IGIS_Query2::UPDATE_STOP:
		{

		}
		break;
	default:
		ASSERT( FALSE );
		break;
	}
}
static void CALLBACK UpdateProcForQueryPOIByIndex(IGIS_Query2::enUpdateAspect eAspect , DWORD dwPara)
{
	switch( eAspect )
	{
	case IGIS_Query2::UPDATE_NEW_OBJECT:
		{
			CObject* pObj = (CObject*)dwPara;
			ASSERT(pObj && pObj->IsKindOf(RUNTIME_CLASS(CObject)));			
			
			
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CStGISFeature)));
			CStGISFeature * pObject = (CStGISFeature*)pObj;
			
            g_pGisFeature = pObject;
		}
		break;
	case IGIS_Query2::UPDATE_STOP:
		{

		}
		break;
	default:
		ASSERT( FALSE );
		break;
	}
}
static void CALLBACK UpdateProcForQueryPOINearest(IGIS_Query2::enUpdateAspect eAspect , DWORD dwPara)
{
	switch( eAspect )
	{
	case IGIS_Query2::UPDATE_NEW_OBJECT:
		{
			CObject* pObj = (CObject*)dwPara;
			ASSERT(pObj && pObj->IsKindOf(RUNTIME_CLASS(CObject)));			
			
			
			ASSERT(pObj->IsKindOf(RUNTIME_CLASS(CStGISFeature)));
			CStGISFeature * pObject = (CStGISFeature*)pObj;
			
            if(!g_pGisFeatureList)	
            {
                g_pGisFeatureList = new CPtrList;
            }

            g_pGisFeatureList->AddTail(pObject);
		}
		break;
	case IGIS_Query2::UPDATE_STOP:
		{

		}
		break;
	default:
		ASSERT( FALSE );
		break;
	}
}

int QueryPOILatLog(CStringArray& arString,double& dLat,double& dLog)
{
	if( 3 != arString.GetSize())
		return 0;
    TRACE3("\n%s,%s,%s\n\n",arString[0],arString[1],arString[2]);
    
    IGIS_Query2* pGIS_Query2 = AppGetMainCtrl()->GetQueryTool();
	ASSERT(pGIS_Query2 != NULL);
	
	IGIS_Query2::tagSearchCondition aSearchCondition;
	aSearchCondition.nCountryId = 86;
	aSearchCondition.nProvinceId = 0;
	aSearchCondition.strMatchText = arString[0];
    aSearchCondition.nConditionMask = IGIS_Query2::CONDITION_PROVINCE|IGIS_Query2::CONDITION_MATCH_TEXT;
	aSearchCondition.nResult = IGIS_Query2::OBJECT_CITY;

    aSearchCondition.lpfnUpdateProc = UpdateProcForQueryCity;
	VERIFY(pGIS_Query2->SetCondition( aSearchCondition ));
    
	pGIS_Query2->SearchStart();   
    
    while(!pGIS_Query2->SetCondition(aSearchCondition))
    {
        Sleep(100);
    }

    delete g_pProvinceObject;
    g_pProvinceObject = NULL;
    if(!g_pCityObject)
        return 0;
    
    aSearchCondition.nCityId = g_pCityObject->m_nId;
	aSearchCondition.nBigClass = 24;
	aSearchCondition.strMatchText = arString[1];
    aSearchCondition.nConditionMask = IGIS_Query2::CONDITION_CITY|IGIS_Query2::CONDITION_BIGCLASS|IGIS_Query2::CONDITION_MATCH_TEXT;
	aSearchCondition.nResult = IGIS_Query2::OBJECT_INDEX_OF_POI;

    aSearchCondition.lpfnUpdateProc = UpdateProcForQueryIndexOfPOI;
	VERIFY(pGIS_Query2->SetCondition( aSearchCondition ));
    
	pGIS_Query2->SearchStart();   

    while(!pGIS_Query2->SetCondition(aSearchCondition))
    {
        Sleep(100);
    }

    delete g_pCityObject;
    g_pCityObject = NULL;
    if(!g_pIndexOfPOI)
        return 0;
    
	aSearchCondition.BandIDOfIndex  = g_pIndexOfPOI->m_BandID;
	aSearchCondition.LayerIDOfIndex = g_pIndexOfPOI->m_LayerID;
	aSearchCondition.ClassIDOfIndex = g_pIndexOfPOI->m_ClassID;
	aSearchCondition.strNameOfIndex = g_pIndexOfPOI->m_strName;
	aSearchCondition.nConditionMask = IGIS_Query2::CONDITION_INDEX_OF_POI;
	aSearchCondition.nResult = IGIS_Query2::OBJECT_POI_BY_INDEX;

	aSearchCondition.lpfnUpdateProc = UpdateProcForQueryPOIByIndex;
	VERIFY(pGIS_Query2->SetCondition( aSearchCondition )); 
    
	pGIS_Query2->SearchStart();   

    while(!pGIS_Query2->SetCondition(aSearchCondition))
    {
        Sleep(100);
    }

    delete g_pIndexOfPOI;
    g_pIndexOfPOI = NULL;
    if(!g_pGisFeature)
        return 0;


    //到这里已把中心点查好
    //////////////////////////////////////////////////////////////////////////
	aSearchCondition.strMatchText = arString[2];
	aSearchCondition.fLat = g_pGisFeature->m_dy;
	aSearchCondition.fLog = g_pGisFeature->m_dx;
	aSearchCondition.eSearchRangeBaseon = IGIS_Query2::RANGE_BASE_ON_POINT;
	aSearchCondition.nConditionMask = IGIS_Query2::CONDITION_LATLOG|IGIS_Query2::CONDITION_MATCH_TEXT;
	aSearchCondition.nResult = IGIS_Query2::OBJECT_NEAREST;

	aSearchCondition.lpfnUpdateProc = UpdateProcForQueryPOINearest;
	VERIFY(pGIS_Query2->SetCondition(aSearchCondition));

	pGIS_Query2->SearchStart();   

    while(!pGIS_Query2->SetCondition(aSearchCondition))
    {
        Sleep(100);
    }

    delete g_pGisFeature;
    g_pGisFeature = NULL;
    if(!g_pGisFeatureList)
        return 0;
    
    
    
    
    
    CStGISFeature* pObject = (CStGISFeature*)g_pGisFeatureList->GetHead();
    if(g_pGisFeatureList->GetCount() > 1)
    {
        CDlgCommonList aDlg(NULL,g_pGisFeatureList);
        if(IDOK == aDlg.DoModal())
        {
            int nItem = aDlg.GetSelect();
            pObject = (CStGISFeature*)g_pGisFeatureList->GetAt(g_pGisFeatureList->FindIndex(nItem));
        }
    }
    
    dLat = pObject->m_dy;
    dLog = pObject->m_dx;

    {    
        CPtrList* pPtrList = g_pGisFeatureList;
        POSITION pos = pPtrList->GetHeadPosition();
        while(pos)
        {
            CQueryCommonObject* pObject = (CQueryCommonObject*)pPtrList->GetNext(pos);
            ASSERT(pObject);
            delete pObject;
        }
        pPtrList->RemoveAll();
        delete pPtrList;
    }  
    g_pGisFeatureList = NULL;    

    return 1;
}