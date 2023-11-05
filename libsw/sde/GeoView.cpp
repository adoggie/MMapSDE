
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "GeoView.h"
#include "GIS_MapInfo.h"
#include "GIS_GeoMap.h"
#include "GIS_BreadthMap.h"
#include "GIS_Pathpoint.h"
#include "GIS_WayPoint.h"
#include "GIS_LayerManager.h"
#include "GIS_QueryByShape.h"

#include "FileDataStruct/V7/GIS_NetworkRoadSectionV7.h"
#include "FileDataStruct/V7/GIS_NetworkVertexV7.h"

#include "SDEControler.h"
#include "GIS_MapDataManager.h"
#include "MemoryDispatchForSpaceDataV2.h"
//////////////////////////////////////////////////////////////////////////
#include "./DebugFunctionLib/DebugFun.h"
#include "./Zlib/zlib.h"
#include "SDELib.h"
#include "swBase.h"

extern CSDELibApp theApp;

// added by scott 2008.10.07

int pow(int x,int y){
	return pow( double(x),double(y) );
}
int fabs(int x)	{
	return fabs( float(x));
}
float sqrt(int x){
	return sqrt( float(x));
}

#ifdef _WIN32_WCE
BOOL wce_PolyPolygon(
													HDC hdc,                  // handle to DC
													CONST POINT *lpPoints,    // array of vertices
													CONST INT *lpPolyCounts,  // array of count of vertices
													int nCount                // count of polygons
													){
	int n ;
	POINT* pt = (POINT*)lpPoints;
	for(n=0;n<nCount;n++){
		Polygon(hdc,pt, *(lpPolyCounts+n));
		pt += *(lpPolyCounts+n);
	}
	return TRUE;
}


BOOL wce_PolyPolyline(
									HDC hdc,                      // handle to device context
									CONST POINT *lppt,            // array of points
									CONST DWORD *lpdwPolyPoints,  // array of values
									DWORD cCount                  // number of entries in values array
								){
	int n ;
	POINT* pt = (POINT*)lppt;
	for(n=0;n<cCount;n++){
		Polyline(hdc,pt, *(lpdwPolyPoints+n));
		pt += *(lpdwPolyPoints+n);
	}
	return TRUE;
}

#endif
//-- end add
//////////////////////////////////////////////////////////////////////////
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
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static CEvent *g_pMatchEvent = NULL;
/*
*/
#ifdef _WIN32_WCE 
static int g_nFeatureClipType[100000*5];
static POINT g_ptList[20000*5];
static int g_pPart[1000*5];
static ST_GEO_PTXY g_ptNum[10000*5]; // scott
#else
static int g_nFeatureClipType[100000];
static POINT g_ptList[20000];
static int g_pPart[1000];
static ST_GEO_PTXY g_ptNum[10000];

#endif
///////////////////////////////////////////////////////////////////////////////
// CGeoView
void ThreadDrawMap( void *pParam )
{
	CGeoView *pView = (CGeoView*)pParam;
	if( pView )
		pView->DrawMap();
}
CGeoView::CGeoView()
{
	HINSTANCE hOldInstance;
#ifdef _WIN32_WCE
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#else
	// removed by scott 
//	hOldInstance = AfxGetResourceHandle();
//	AfxSetResourceHandle(theApp.m_hInstance);    //theApp是DLL的App
	
	AfxSetResourceHandle((HINSTANCE)GetModuleHandle(L"sdelib.dll"));
	hOldInstance = AfxGetResourceHandle();
#endif

	m_bClearBuf = FALSE;

	CSDEControler* pControl = CSDEControler::GetSDEControl();
	ASSERT( pControl );
	CGIS_MapDataManager* pMapManager = NULL;
	if( pControl )
		pMapManager = (CGIS_MapDataManager*)pControl->GetMapManager();
	ASSERT( pMapManager );

	if( pMapManager )
		pMapManager->AttechView( this );
	//{{AFX_DATA_INIT(CGeoView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	X_OFF = Y_OFF = 8;
	DC_BOTTOM	= GetSystemMetrics(SM_CYSCREEN) ;	// 绘图区域：底
//	DC_BOTTOM	= 234;	// 绘图区域：底
	DC_LEFT		= 0;	// 绘图区域：左
	DC_TOP		= 0;
	DC_RIGHT	= GetSystemMetrics(SM_CXSCREEN) ;
//	DC_RIGHT	= 480;
	
	// 缺省背景色为白色
	pBufDC		= NULL;
	pBufBMP		= NULL;
	m_pOldBufBMP = NULL;

	pBackDC		= NULL;
	pBackBMP	= NULL;
	m_pOldBackBMP = NULL;

	m_pdcMoveTemp = NULL;
	m_pmapMoveTemp= NULL;
	m_pOldMoveTemp = NULL;

	m_pPureBackDC = NULL;
	m_pPureBackBmp = NULL;
	m_pOldPureBackBmp = NULL;

	pCurrentDC = NULL;

	m_pApartBackDC = NULL;
	m_pApartBackBmp = NULL;
	m_pOldApartBackBmp = NULL;
	m_colBack = RGB(220,255,255);
	m_BackBrush.nForeColor = m_colBack;
	m_BackPen.nColor = m_colBack;
	m_BackPen.nStyle = PS_SOLID;
	m_BackPen.nWidth = 1;

	// 缺省DC为BufDC
//	pCurrentDC	= pBufDC;
	m_bIsVehMiddle = FALSE;
	m_OldScaleLevel = -1;
	
//	bMapDrawed	= FALSE;
	m_pSpRutLayerInfo = NULL;
	m_bDown		= FALSE;
	m_MapSet = new CPtrList;
	m_DrawPtList = new CPtrList;

	ImageList.Create(IDB_OBJICON, 16, 0, RGB(255, 0, 255));
#ifdef _WIN32_WCE
	m_CarImageList.Create(IDB_CARICON, 40, 0, RGB(253, 2, 252));
#else
	{
		
		
		m_CarImageList.Create( 40 , 40 , ILC_COLORDDB|ILC_MASK , 0 , 4 );
		CBitmap pBitmap;
		pBitmap.LoadBitmap( IDB_CARICON );
		m_CarImageList.Add( &pBitmap , RGB(253, 2, 252) );
		
	}
#endif
	pAnnoList	= new CPtrList;
	m_hDrawMap = NULL;
	m_LayerDrawState = NULL;
	m_LayerCount = 0;
	st_level.EnLevel = EN_LEVEL_DEGREE;
	st_level.nDecimal = 4;
	m_Angle = 0;
	m_ScaleCount = 13;
	m_CurPt.dx = m_CurPt.dy = 10e5;
	m_dbScaleValue = new double[m_ScaleCount];
	m_dbScaleValue[0] = 1;
	m_dbScaleValue[1] = 2;
	m_dbScaleValue[2] = 4;
	m_dbScaleValue[3] = 8;//
	m_dbScaleValue[4] = 15;
	m_dbScaleValue[5] = 30;
	m_dbScaleValue[6] = 60;
	m_dbScaleValue[7] = 120;
	m_dbScaleValue[8] = 300;
	m_dbScaleValue[9] = 600;
	m_dbScaleValue[10] = 1200;
	m_dbScaleValue[11] = 2000;
	m_dbScaleValue[12] = 5000;
//	m_dbScaleValue[13] = 10000;
//	m_dbScaleValue[14] = 30000;
//	m_dbScaleValue[15] = 60000;
	m_bDayShow = TRUE;
	m_ScaleCurLevel = 1;
	m_Nav = FALSE;
//	m_MatchDis = 70;
	m_pNetworkRoute = NULL;
	m_PathNodePos = NULL;
	m_PathPtPos = NULL;
	m_PathNearNodePos = NULL;
//	m_LastRoatID = -1;
	m_LastRoat = NULL;
	m_Nav = FALSE;
	m_nPreMBLNum = 0;
	m_nPreBNum = 0;
	
	m_nCurMBLNum = 0;
	m_bLFlag = TRUE;
	m_bDFlag = TRUE;
	m_bMoveMap = FALSE;
	m_pPtList = NULL;
	m_bShowVehSymbol = TRUE;
	m_hVehPen = NULL;
	m_hVehBrs =NULL;
	m_hClePen = NULL;
	m_nRadius = 0;
	m_nVehAngle = 90;
	m_fHightScale = 0;
	m_bReDrawCar = FALSE;
	m_FlaghIcon = AfxGetApp()->LoadIcon(IDI_ICON_FLAG);
	m_MidFlagIcon = AfxGetApp()->LoadIcon( IDI_ICON_MIDFLAG );


	m_bDrawNetWorkRoute = TRUE;
	m_bHighway = FALSE;
	m_ptMatch.dx = 181;
	m_ptMatch.dy = 91;
	m_b3D = FALSE;
	m_nLeaveCount = 0;
	m_bDrawByLayer = TRUE;
	m_bNav = FALSE;
	m_posTurnList = NULL;
	m_pOldFont = NULL;
	m_bShowTip = FALSE;
	m_bShowFocusRut = FALSE;
	m_bShowMapCompass = TRUE;
	m_pSpecialRut = NULL;
	m_bShowSpecalRut = FALSE;
	m_ptMapCompass.x = 5;
	m_ptMapCompass.y = 85;

	m_CompassImageList.Create(IDB_COMPASS, 42, 0, RGB(255, 0, 255)); 
	m_strFindName = L"";
	m_strFindName.Empty();
	m_pFeatureTip = NULL;
	plotRect.left = plotRect.right = plotRect.bottom = plotRect.top = 0;
	m_MaxPlotRect.left = m_MaxPlotRect.right = m_MaxPlotRect.bottom = m_MaxPlotRect.top = 0;
	m_nCarRunDirect = 0 ;

	// 增加对成员变量的初始化 20070326
	dNewScale = 0;
	::memset(&drawRect, 0, sizeof(ST_GEO_RECT));
	::memset(&NewDrawRect, 0, sizeof(ST_GEO_RECT));
	::memset(&RotateRect, 0, sizeof(ST_GEO_RECT));
	m_nW = 0;
	m_nH = 0;
	dScale = 0;
	m_bMapSeclude = FALSE;
	m_dbAngle = 0;
	for (int i=0; i<MLBNUM_MAX; i++)
	{
		m_pPreLList[i] = NULL;
		m_pPreBIDs[i] = NULL;
		m_pCurLList[i] = NULL;
		m_pCurBIDs[i] = 0;
		m_pPreBIDList[i] = 0;
	}
	m_sinRotate = 0;
	m_cosRotate = 0;
	nAdjustx = 0;
	nAdjusty = 0;
	m_TipFeaType = EN_OBJ_ALL;
	::memset(&m_ptTipXy, 0, sizeof(ST_GEO_PTXY));


	if( !g_pMatchEvent )
	{
		g_pMatchEvent = new CEvent;
		g_pMatchEvent->SetEvent();
	}
#ifndef _WIN32_WCE
	
	AfxSetResourceHandle(hOldInstance);
#endif
	//路口扩大图默认参数
	m_ExpandMapCfg.colBack = RGB(204,204,153);
	m_ExpandMapCfg.colMidLine = RGB(255,255,255);
	m_ExpandMapCfg.colRoad = RGB(102,102,102);
	m_ExpandMapCfg.colRut = RGB(0,0,204);
	m_ExpandMapCfg.nLength = 180;
	m_ExpandMapCfg.nWidth = 196;

	m_pDCCross = NULL;
	m_pCrossBmp = NULL;
	m_pCrossBmpOld = NULL;
	m_CrossPos = NULL;

//	m_pDCCrossWithoutCar = NULL;
//	m_pCrossBmpWithoutCar = NULL;
//	m_pCrossBmpWithoutCarOld = NULL;
	m_dxMoveError = m_dyMoveError = 0;
}

CGeoView::~CGeoView()
{
	ImageList.Detach();
	m_CarImageList.Detach();
	if( pBufDC )
	{
		pBufDC->DeleteDC();
		delete pBufDC;
		pBufDC = NULL;
	}
	if( pBufBMP )
	{
		pBufBMP->DeleteObject();
		pBufBMP->Detach();
		delete pBufBMP;
		pBufBMP		= NULL;
	}

	if( pBackDC )
	{
		pBackDC->DeleteDC();
		delete pBackDC;
		pBackDC = NULL;
	}
	if( pBackBMP )
	{
		pBackBMP->DeleteObject();
		pBackBMP->Detach();
		delete pBackBMP;
		pBackBMP	= NULL;
	}

	if( m_pdcMoveTemp )
	{
		m_pdcMoveTemp->DeleteDC();
		delete m_pdcMoveTemp;
		m_pdcMoveTemp = NULL;
	}
	if( m_pmapMoveTemp )
	{
		m_pmapMoveTemp->DeleteObject();
		m_pmapMoveTemp->Detach();
		delete m_pmapMoveTemp;
	}


	if( m_pPureBackDC)
	{
		m_pPureBackDC->DeleteDC();
		delete m_pPureBackDC;
		m_pPureBackDC = NULL;
	}
	if( m_pPureBackBmp )
	{
		m_pPureBackBmp->DeleteObject();
		m_pPureBackBmp->Detach();
		delete m_pPureBackBmp;
		m_pPureBackBmp = NULL;
	}

	if( m_pApartBackDC )
	{
		m_pApartBackDC->DeleteDC();
		delete m_pApartBackDC;
		m_pApartBackDC = NULL;
	}
	if( m_pApartBackBmp )
	{
		m_pApartBackBmp->DeleteObject();
		m_pApartBackBmp->Detach();
		delete m_pApartBackBmp;
		m_pApartBackBmp = NULL;
	}

	if( m_pDCCross )
	{
		m_pDCCross->DeleteDC();
		delete m_pDCCross;
		m_pDCCross = NULL;
	}
	if( m_pCrossBmp )
	{
		m_pCrossBmp->DeleteObject();
		m_pCrossBmp->Detach();
		delete m_pCrossBmp;
		m_pCrossBmp = NULL;
	}

//	if( m_pDCCrossWithoutCar )
//	{
//		m_pDCCrossWithoutCar->DeleteDC();
//		delete m_pDCCrossWithoutCar;
//		m_pDCCrossWithoutCar = NULL;
//	}
//	if( m_pCrossBmpWithoutCar )
//	{
//		m_pCrossBmpWithoutCar->DeleteObject();
//		m_pCrossBmpWithoutCar->Detach();
//		delete m_pCrossBmpWithoutCar;
//		m_pCrossBmpWithoutCar = NULL;
//	}
	
	ClearAnnoList();
	ClearDrawPOI();
	delete pAnnoList;
	pAnnoList = NULL;
	if(m_MapSet != NULL)
	{
		m_MapSet->RemoveAll();
		delete m_MapSet;
		m_MapSet = NULL;
	}
//	if( m_pNetworkRoute )
//	{
//		delete m_pNetworkRoute;
//		m_pNetworkRoute = NULL;
//	}
	if(m_LastRoat)
	{
		delete m_LastRoat;
		m_LastRoat = NULL;
	}
	if(m_dbScaleValue != NULL)
	{
		delete[] m_dbScaleValue;
		m_dbScaleValue = NULL;
	}
	if( m_pPtList!=NULL )
	{
		delete[] m_pPtList;
		m_pPtList = NULL;
	}
	
	if ( m_bmpForBmp.GetSafeHandle() != NULL )
		m_bmpForBmp.DeleteObject();

	if ( m_hVehPen != NULL )
	{
		::DeleteObject( m_hVehPen );
		m_hVehPen = NULL;
	}

	if ( m_hVehBrs != NULL )
	{
		::DeleteObject( m_hVehBrs );
		m_hVehBrs = NULL;
	}

	if ( m_hClePen != NULL )
	{
		::DeleteObject( m_hClePen );
		m_hClePen = NULL;
	}
	if( m_pFeatureTip )
	{
		delete m_pFeatureTip;
		m_pFeatureTip = NULL;
	}

	// 增加部分指针释放内存操作 20070327
	if (m_LayerDrawState != NULL)
	{
		delete[]m_LayerDrawState;
		m_LayerDrawState = NULL;
	}
	if (m_DrawPtList != NULL)
	{
		m_DrawPtList->RemoveAll();
		delete m_DrawPtList;
		m_DrawPtList = NULL;
	}
	if (m_pSpecialRut != NULL)
	{
		delete m_pSpecialRut;
		m_pSpecialRut = NULL;
	}
	if( g_pMatchEvent )
	{
		delete g_pMatchEvent;
		g_pMatchEvent = NULL;
	}
}	
#ifdef _CVIEW
BEGIN_MESSAGE_MAP(CGeoView, CView)
#else
BEGIN_MESSAGE_MAP(CGeoView, CWnd)
#endif
	//{{AFX_MSG_MAP(CGeoView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 调整设置

void CGeoView::AdjustBuf()
{
	static bool bHaveBuf = false;
	if (!bHaveBuf)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		bHaveBuf = true;
	}
	RECT rtView;
	GetClientRect(&rtView);
	BOOL bChangeBMP = FALSE;
	if( rtView.right > m_MaxPlotRect.right
		|| rtView.bottom > m_MaxPlotRect.bottom 
		|| rtView.left < m_MaxPlotRect.left
		|| rtView.top < m_MaxPlotRect.top )
	{
		m_MaxPlotRect.left = min(rtView.left,m_MaxPlotRect.left) ;
		m_MaxPlotRect.top = min(rtView.top,m_MaxPlotRect.top)  ;
		m_MaxPlotRect.right = max(rtView.right,m_MaxPlotRect.right)  ;
		m_MaxPlotRect.bottom = max(rtView.bottom,m_MaxPlotRect.bottom) ;
		if( m_MaxPlotRect.right < 300 )
			m_MaxPlotRect.right += ((m_MaxPlotRect.right-m_MaxPlotRect.left)+10) ;
		else
			m_MaxPlotRect.right += 30;
		m_MaxPlotRect.bottom += 50;
		bChangeBMP = TRUE;
	}
	plotRect = rtView;
	CPoint ptTemp;
	ptTemp.x = ( plotRect.right + plotRect.left ) / 2;
	ptTemp.y = ( plotRect.bottom + plotRect.top ) / 2;

	SetRotateCenter( ptTemp );//默认为中心点是旋转点
	SetZoomCenter( ptTemp ); //缩放中心点
	CClientDC dc(this);
	if( !pBufDC )
	{
		pBufDC = new CDC();
		pBufDC->CreateCompatibleDC(&dc);
	}
	if( pBufBMP && bChangeBMP )
	{
		pBufDC->SelectObject(m_pOldBufBMP);
		pBufBMP->DeleteObject();
		delete pBufBMP;
		pBufBMP = NULL;
	}
	if( !pBufBMP ) //缓冲位图内存
	{
		pBufBMP = new CBitmap();
		BOOL bBuf = pBufBMP->CreateCompatibleBitmap(&dc, (m_MaxPlotRect.right - m_MaxPlotRect.left),
									(m_MaxPlotRect.bottom - m_MaxPlotRect.top));
		
		m_pOldBufBMP = pBufDC->SelectObject( pBufBMP );
	}

	
	if( !pBackDC )
	{
		pBackDC = new CDC();
		pBackDC->CreateCompatibleDC(&dc);
	}

	if( pBackBMP && bChangeBMP )
	{
		pBackDC->SelectObject( m_pOldBackBMP );
		pBackBMP->DeleteObject();
		delete pBackBMP;
		pBackBMP = NULL;
	}
	if( !pBackBMP )
	{
		pBackBMP = new CBitmap();
		BOOL bBack = pBackBMP->CreateCompatibleBitmap(&dc, (m_MaxPlotRect.right - m_MaxPlotRect.left),
									 (m_MaxPlotRect.bottom - m_MaxPlotRect.top));
		m_pOldBackBMP = pBackDC->SelectObject( pBackBMP );
	}
	
	pCurrentDC	= pBufDC;

	if( !m_pdcMoveTemp )
	{
		m_pdcMoveTemp = new CDC();
		m_pdcMoveTemp->CreateCompatibleDC(&dc);
	}


	if( m_pmapMoveTemp && bChangeBMP )
	{
		m_pdcMoveTemp->SelectObject( m_pOldMoveTemp );
		m_pmapMoveTemp->DeleteObject();
		delete m_pmapMoveTemp;
		m_pmapMoveTemp = NULL;
	}
	if( !m_pmapMoveTemp )
	{
		m_pmapMoveTemp = new CBitmap();
		m_pmapMoveTemp->CreateCompatibleBitmap(&dc, (m_MaxPlotRect.right - m_MaxPlotRect.left),
									 (m_MaxPlotRect.bottom - m_MaxPlotRect.top));
		m_pOldMoveTemp = m_pdcMoveTemp->SelectObject( m_pmapMoveTemp );
	}

	
	if( !m_pPureBackDC )
	{
		m_pPureBackDC = new CDC();
		m_pPureBackDC->CreateCompatibleDC(&dc);
	}

	if( m_pPureBackBmp && bChangeBMP )
	{
		m_pPureBackDC->SelectObject( m_pOldPureBackBmp );
		m_pPureBackBmp->DeleteObject();
		delete m_pPureBackBmp;
		m_pPureBackBmp = NULL;
	}
	if( !m_pPureBackBmp )
	{
		m_pPureBackBmp = new CBitmap();
		m_pPureBackBmp->CreateCompatibleBitmap(&dc, (m_MaxPlotRect.right - m_MaxPlotRect.left),
									 (m_MaxPlotRect.bottom - m_MaxPlotRect.top) );
		m_pOldPureBackBmp = m_pPureBackDC->SelectObject( m_pPureBackBmp );
	}
	if( !m_pApartBackDC )
	{
		m_pApartBackDC = new CDC();
		m_pApartBackDC->CreateCompatibleDC(&dc);
	}

	if( m_pApartBackBmp && bChangeBMP )
	{
		m_pApartBackDC->SelectObject( m_pOldApartBackBmp );
		m_pApartBackBmp->DeleteObject();
		delete m_pApartBackBmp;
		m_pApartBackBmp = NULL;
	}
	if( !m_pApartBackBmp )
	{
		m_pApartBackBmp = new CBitmap();
		m_pApartBackBmp->CreateCompatibleBitmap(&dc, (m_MaxPlotRect.right - m_MaxPlotRect.left),
									 (m_MaxPlotRect.bottom - m_MaxPlotRect.top) );
		m_pOldApartBackBmp = m_pApartBackDC->SelectObject( m_pApartBackBmp );
	}
	if( bChangeBMP )
	{
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(LOGFONT));
		
		wcscpy(logFont.lfFaceName, L"宋体");
		logFont.lfCharSet	= DEFAULT_CHARSET;
		logFont.lfWeight = FW_SEMIBOLD;
		logFont.lfHeight	= GetDeviceCaps(pCurrentDC->m_hDC, LOGPIXELSY) * 9 / 50;
	//	logFont.lfHeight	= GetDeviceCaps(pCurrentDC->m_hDC, LOGPIXELSY) * 9 / 72;
		m_font.DeleteObject();
		m_font.Detach();
		m_font.CreateFontIndirect(&logFont);

		if( m_pOldFont )
			pCurrentDC->SelectObject( m_pOldFont );
		m_pOldFont = pCurrentDC->SelectObject(&m_font);
		pBackDC->SelectObject(&m_font);


		CSize s = pCurrentDC->GetTextExtent(L"中");
		m_nH	= s.cy;
		m_nW	= s.cx;
	}

	if( !m_pDCCross )
	{
		m_pDCCross = new CDC();
		m_pDCCross->CreateCompatibleDC(&dc);
	}
	if( !m_pCrossBmp )
	{
		m_pCrossBmp = new CBitmap();
		m_pCrossBmp->CreateCompatibleBitmap(&dc, m_ExpandMapCfg.nWidth,
									 m_ExpandMapCfg.nLength );
		m_pCrossBmpOld = m_pDCCross->SelectObject( m_pCrossBmp );
	}

//	if( !m_pDCCrossWithoutCar )
//	{
//		m_pDCCrossWithoutCar = new CDC();
//		m_pDCCrossWithoutCar->CreateCompatibleDC(&dc);
//	}
//	if( !m_pCrossBmpWithoutCar )
//	{
//		m_pCrossBmpWithoutCar = new CBitmap();
//		m_pCrossBmpWithoutCar->CreateCompatibleBitmap(&dc, m_ExpandMapCfg.nWidth,
//									 m_ExpandMapCfg.nLength );
//		m_pCrossBmpWithoutCarOld = m_pDCCrossWithoutCar->SelectObject( m_pCrossBmpWithoutCar );
//	}
}

///////////////////////////////////////////////////////////////////////////////
// 计算比例尺

void CGeoView::CalcScale()
{

	double	sx, sy;
	float	fx, fy;
	
	fx = (NewDrawRect.dRight - NewDrawRect.dLeft) / 2;
	fy = (NewDrawRect.dBottom - NewDrawRect.dTop) / 2;
	int nW,nH;
	nW = plotRect.right - plotRect.left;
	nH = plotRect.bottom - plotRect.top;
	sx = (NewDrawRect.dRight - NewDrawRect.dLeft) / nW;
	sy = (NewDrawRect.dBottom - NewDrawRect.dTop) / nH;

	if(fx == 0 || fy == 0)
	{
		memcpy(&NewDrawRect, &drawRect, sizeof(ST_GEO_RECT));
		return;
	}

	if(sx < sy)
	{
		dNewScale = 1.0 / sy;
	}
	else
	{
		dNewScale = 1.0 / sx;
	}

	fx = (NewDrawRect.dRight - NewDrawRect.dLeft) / 2;
	fy = (NewDrawRect.dBottom - NewDrawRect.dTop) / 2;
	if(fx == 0 || fy == 0)
	{
		memcpy(&NewDrawRect, &drawRect, sizeof(ST_GEO_RECT));
		dNewScale = dScale;
	}
	else
	{
		NewDrawRect.dRight = (float)(NewDrawRect.dLeft + nW / dNewScale);
		NewDrawRect.dBottom = (float)(NewDrawRect.dTop + nH / dNewScale);
	}
	if(m_Angle == 0)
	{
		RotateRect = NewDrawRect;
	}

}

///////////////////////////////////////////////////////////////////////////////
// 初始化窗口
void CGeoView::SetScale(double dx, double dy,UINT ScaleID)
{
	dScale = dNewScale = TurnScale(ScaleID);
	m_bMapSeclude = MAPSECLUDE_SCALE>dScale? TRUE:FALSE;
//	double nW = (plotRect.right - plotRect.left) /2;
//	double nH = (plotRect.bottom - plotRect.top) /2;
//	NewDrawRect.dRight = (float)(dx + (nW / dNewScale) );
//	NewDrawRect.dLeft = (float)(dx - (nW / dNewScale) );
//	NewDrawRect.dBottom = (float)(dy+ (nH / dNewScale) );
//	NewDrawRect.dTop = (float)(dy - (nH / dNewScale) );
	double dbR = plotRect.right - m_ptZoomCenter.x;
	double dbL = plotRect.right - dbR;
	double dbB = plotRect.bottom - m_ptZoomCenter.y;
	double dbT = plotRect.bottom - dbB;
	NewDrawRect.dRight = (float)(dx + (dbR / dNewScale) );
	NewDrawRect.dLeft = (float)(dx - (dbL / dNewScale) );
	NewDrawRect.dBottom = (float)(dy+ (dbT / dNewScale) );
	NewDrawRect.dTop = (float)(dy - (dbB / dNewScale) );

	drawRect.dLeft = drawRect.dRight = drawRect.dTop = drawRect.dBottom = 10e+5;
	nAdjustx = (plotRect.right -(RotateRect.dRight - RotateRect.dLeft) * dScale) / 2.0
			    + ( ( (RotateRect.dRight+RotateRect.dLeft)/2.0 - (NewDrawRect.dRight+NewDrawRect.dLeft)/2.0 )*dScale ) ;
	nAdjusty = (plotRect.bottom - (RotateRect.dBottom - RotateRect.dTop) * dScale) / 2.0
			    - ( ( (RotateRect.dBottom+RotateRect.dTop)/2.0 - (NewDrawRect.dBottom+NewDrawRect.dTop)/2.0 )*dScale ) ;
	if(m_Angle == 0)
	{
		RotateRect = NewDrawRect;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 刷新
void CGeoView::Refresh()
{
	InvalidateRect(&plotRect, FALSE);
}
void CGeoView::SetBackColor( COLORREF colBack )
{
	m_colBack = colBack;

	m_BackBrush.nForeColor = m_colBack;
	m_BackPen.nColor = m_colBack;
	m_BackPen.nStyle = PS_SOLID;
	m_BackPen.nWidth = 1;
}

///////////////////////////////////////////////////////////////////////////////
// 从地图到屏幕坐标的转换函数
void CGeoView::XyMapToView(double dx, double dy, int& nx, int& ny)	
{
	float tx,ty;
	tx = nAdjustx + (dx - RotateRect.dLeft) * dScale; // dscale 计算是表示1度需要多少个屏幕坐标点
	ty = (RotateRect.dBottom - dy) * dScale + nAdjusty;
	if(m_Angle != 0)
	{
		//地图转角度
		POINT ptCenter;
		ptCenter = m_ptRotateCenter;
//		ptCenter.y = ptCenter.y / 2;
//		ptCenter.x = plotRect.right / 2;
//		ptCenter.y = plotRect.bottom / 2;
		RotatePlot(tx,ty,ptCenter);
 	}
	nx = (int)(tx);
	ny = (int)(ty);
}

///////////////////////////////////////////////////////////////////////////////
// 从屏幕到地图坐标的转换函数
void CGeoView::XyViewToMap(int nx, int ny, double& dx, double& dy)
{
	float tx = nx;
	float ty = ny;
	if(m_Angle != 0)
	{
		//地图转角度
		POINT ptCenter;
		ptCenter = m_ptRotateCenter;
//		ptCenter.y = ptCenter.y / 2;
//		ptCenter.x = plotRect.right / 2;
//		ptCenter.y = plotRect.bottom / 2;
		UnRotatePlot(tx,ty,ptCenter);
	}
   dx = float(tx - plotRect.left) / dNewScale + NewDrawRect.dLeft; // 左侧的地图经纬度坐标
   dy = NewDrawRect.dBottom - float(ty - plotRect.top) / dNewScale;
}

///////////////////////////////////////////////////////////////////////////////
// 以一点为中心放大屏幕显示
void CGeoView::ZoomInAt(CPoint pt)
{
	if(m_ScaleCurLevel < m_ScaleCount) // 13 levels
	{
		double dx,dy;
		XyViewToMap(pt.x , pt.y , dx, dy);
		m_ScaleCurLevel++;
		dScale = dNewScale = TurnScale(m_ScaleCurLevel);
//		double nW = (plotRect.right - plotRect.left) /2;
//		double nH = (plotRect.bottom - plotRect.top) /2;
//		NewDrawRect.dRight = (float)(dx + (nW / dNewScale));
//		NewDrawRect.dLeft = (float)(dx - (nW / dNewScale));
//		NewDrawRect.dBottom = (float)(dy+ (nH / dNewScale));
//		NewDrawRect.dTop = (float)(dy - (nH / dNewScale));
		double dbR = plotRect.right - m_ptZoomCenter.x;
		double dbL = plotRect.right - dbR;
		double dbB = plotRect.bottom - m_ptZoomCenter.y;
		double dbT = plotRect.bottom - dbB;
		NewDrawRect.dRight = (float)(dx + (dbR / dNewScale) );
		NewDrawRect.dLeft = (float)(dx - (dbL / dNewScale) );
		NewDrawRect.dBottom = (float)(dy+ (dbT / dNewScale) );
		NewDrawRect.dTop = (float)(dy - (dbB / dNewScale) );

		if(m_Angle != 0)
			RotateRect =  MaxNewRect(plotRect);
		else
			RotateRect = NewDrawRect;

	}

}

///////////////////////////////////////////////////////////////////////////////
// 以一点为中心缩小屏幕显示
void CGeoView::ZoomOutAt(CPoint pt)
{
	if(m_ScaleCurLevel > 1)
	{
		double dx,dy;
		XyViewToMap(pt.x , pt.y , dx, dy);
		m_ScaleCurLevel--;
		dScale = dNewScale = TurnScale(m_ScaleCurLevel);
//		double nW = (plotRect.right - plotRect.left) /2;
//		double nH = (plotRect.bottom - plotRect.top) /2;
//		NewDrawRect.dRight = (float)(dx + (nW / dNewScale));
//		NewDrawRect.dLeft = (float)(dx - (nW / dNewScale));
//		NewDrawRect.dBottom = (float)(dy+ (nH / dNewScale));
//		NewDrawRect.dTop = (float)(dy - (nH / dNewScale));
		double dbR = plotRect.right - m_ptZoomCenter.x;
		double dbL = plotRect.right - dbR;
		double dbB = plotRect.bottom - m_ptZoomCenter.y;
		double dbT = plotRect.bottom - dbB;
		NewDrawRect.dRight = (float)(dx + (dbR / dNewScale) );
		NewDrawRect.dLeft = (float)(dx - (dbL / dNewScale) );
		NewDrawRect.dBottom = (float)(dy+ (dbT / dNewScale) );
		NewDrawRect.dTop = (float)(dy - (dbB / dNewScale) );

		if(m_Angle != 0)
			RotateRect =  MaxNewRect(plotRect);
		else
			RotateRect = NewDrawRect;
	}

}

///////////////////////////////////////////////////////////////////////////////
// 将一点设为屏幕中心显示
void CGeoView::CenterinPoint(CPoint pt)
{
	double dx1, dy1, dx2, dy2;

	XyViewToMap(int(plotRect.left + (plotRect.right - plotRect.left) / 2), 
				int(plotRect.top + (plotRect.bottom - plotRect.top) / 2), dx1, dy1);
	XyViewToMap(pt.x, pt.y, dx2, dy2);	
	
	NewDrawRect.dLeft	= (float)(NewDrawRect.dLeft		+ dx2	- dx1);
	NewDrawRect.dTop	= (float)(NewDrawRect.dTop		+ dy2	- dy1);
	NewDrawRect.dRight	= (float)(NewDrawRect.dRight	+ dx2	- dx1);
	NewDrawRect.dBottom = (float)(NewDrawRect.dBottom	+dy2	- dy1);

	NormalizeRect(NewDrawRect);
}

///////////////////////////////////////////////////////////////////////////////
// 将绘制区域居中显示
void CGeoView::CenterinRect(ST_GEO_RECT &rect)
{
	double sx, sy;
	
	
	sx = (rect.dRight - rect.dLeft) / (plotRect.right - plotRect.left);
	sy = (rect.dBottom - rect.dTop) / (plotRect.bottom - plotRect.top);

	if (sx < sy)
	{
		dNewScale = 1.0 / sy;
	}
	else
	{
		dNewScale = 1.0 / sx;
	}
	
	ST_GEO_PTXY pt;
	pt.dx = rect.dLeft + (rect.dRight -rect.dLeft) / 2;
	pt.dy = rect.dTop + (rect.dBottom-rect.dTop) / 2;
	NewDrawRect.dLeft	= (float)(pt.dx - (plotRect.right - plotRect.left) / dNewScale / 2);
	NewDrawRect.dRight  = (float)(pt.dx + (plotRect.right - plotRect.left) / dNewScale / 2);
	NewDrawRect.dTop	= (float)(pt.dy - (plotRect.bottom - plotRect.top) / dNewScale / 2);
	NewDrawRect.dBottom = (float)(pt.dy + (plotRect.bottom - plotRect.top) / dNewScale / 2);
}

///////////////////////////////////////////////////////////////////////////////
// 移动
void CGeoView::DragMove(CPoint pt)
{
	if(m_bMoveMap)
		return;
	double dx1, dy1, dx2, dy2;
	XyViewToMap(m_startPt.x, m_startPt.y, dx1, dy1);


	XyViewToMap(pt.x, pt.y, dx2, dy2);	
	m_positionBMP.x = pt.x - m_startPt.x;
	m_positionBMP.y = pt.y - m_startPt.y;
	m_bMoveMap = TRUE;
	
	NewDrawRect.dLeft	= (float)(NewDrawRect.dLeft		+ dx1 - dx2);
	NewDrawRect.dTop	= (float)(NewDrawRect.dTop		+ dy1 - dy2);
	NewDrawRect.dRight	= (float)(NewDrawRect.dRight	+ dx1 - dx2);
	NewDrawRect.dBottom = (float)(NewDrawRect.dBottom	+ dy1 - dy2);
	NormalizeRect(NewDrawRect);
	if(m_Angle != 0)
	{
		RotateRect.dLeft	= (float)(RotateRect.dLeft		+ dx1 - dx2);
		RotateRect.dTop	= (float)(RotateRect.dTop		+ dy1 - dy2);
		RotateRect.dRight	= (float)(RotateRect.dRight	+ dx1 - dx2);
		RotateRect.dBottom = (float)(RotateRect.dBottom	+ dy1 - dy2);
		NormalizeRect(RotateRect);
	}
	if(m_Angle == 0)
	{
		RotateRect = NewDrawRect;
	}

}

///////////////////////////////////////////////////////////////////////////////
// 窗口方式放大
void CGeoView::ZoomInWindow(int nx1, int ny1, int nx2, int ny2)
{
	double sx, sy;
	double dx1, dy1, dx2, dy2, dx, dy;

	sx = NewDrawRect.dRight - NewDrawRect.dLeft;
	sy = NewDrawRect.dBottom - NewDrawRect.dTop;

	// 防止为零
	if(fabs(sx) < 10E-300 || fabs(sy)<10E-300)
	{
		return;
	}

	// 如果水平或垂直方向没有像素位移，则返回
	if(nx1 == nx2 || ny1 == ny2)
	{
		return;
	}

	XyViewToMap(nx1, ny1, dx1, dy1);
	XyViewToMap(nx2, ny2, dx2, dy2);

	if(dx1 > dx2)
	{
		dx	= dx1;
		dx1 = dx2;
		dx2 = dx;
	}

	if(dy1 > dy2)
	{
       dy	= dy1;
	   dy1	= dy2;
	   dy2	= dy;
	}

	NewDrawRect.dLeft	= (float)dx1;
	NewDrawRect.dTop	= (float)dy1;
    NewDrawRect.dRight	= (float)dx2;
    NewDrawRect.dBottom = (float)dy2;
   
	CalcScale();
}

///////////////////////////////////////////////////////////////////////////////
// 按键弹起
void CGeoView::ButtonUp(CPoint pt)
{
	if(m_MapSet == NULL)
		return;
	if( m_bDown )
	{	
		m_bDown = FALSE;
		if( (abs(pt.x - m_startPt.x)<5) && (abs(pt.y - m_startPt.y)<5) )
		{
			Refresh();
			return;
		}
		DragMove(pt);//移动过多导致认为是拖动操作
		Refresh();
		m_startPt = pt;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 按键按下
void CGeoView::ButtonDown(CPoint pt)
{
	if(m_MapSet == NULL)
	{
		return;
	}
/*	DWORD nThreadId = 0;
	HANDLE hDrawState = (HANDLE)::CreateThread(NULL,0,&SetDrawStateTpro,(LPVOID)this,0,&nThreadId );
	SetThreadPriority( hDrawState, THREAD_PRIORITY_HIGHEST );
*/	m_startPt = m_endPt = pt;
	m_bDown = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 按键移动
void CGeoView::MouseMove(CPoint pt)
{
	if(m_MapSet == NULL)
	{
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 绘制一矩形框
void CGeoView::DrawRange()
{
	return;
	if (m_startPt != m_endPt)
	{
		swRect focusRect(m_startPt, m_endPt);
		focusRect.NormalizeRect();

		CDC *pDC = GetDC();
		pDC->SetROP2(R2_NOTXORPEN);
		pDC->MoveTo(m_startPt);
		pDC->LineTo(m_endPt.x, m_startPt.y);
		pDC->LineTo(m_endPt);
		pDC->LineTo(m_startPt.x, m_endPt.y);
		pDC->LineTo(m_startPt);

		ReleaseDC(pDC);
	}
}

///////////////////////////////////////////////////////////////////////////////
// 为控制点添加注释信息
void CGeoView::AddAnnotate(ST_GEO_PTXY* pPtList, int nPtNum, swString pText , BYTE byLevel,BOOL bSameDirect )
{
	swRect rect;
	int nx, ny;
	ST_GEO_ANNO* pAnno;

	if(nPtNum == 1)
	{
		//剔除重名情况
		POSITION pos = pAnnoList->GetHeadPosition();
		BOOL bSameRecord = FALSE;
		while(pos != NULL)
		{
			pAnno = (ST_GEO_ANNO*)pAnnoList->GetNext( pos );
			if( pText == pAnno->szAnno )
			{
				bSameRecord = TRUE;
				break;
			}
		}
		if( bSameRecord )
			return;
		XyMapToView(pPtList->dx, pPtList->dy, nx, ny);
		// 绘制注释信息
		pCurrentDC->DrawText(pText, &rect, DT_CALCRECT);
		swRect r(nx, ny, nx + rect.Width(), ny + rect.Height());
		
		// This method moves a swRect object by the specified offsets, 
		// x units along the x-axis and y units along the y-axis
		r.OffsetRect(X_OFF, -(r.Height() + Y_OFF));
		if(CalcAnnoStation(r))
		{
			RECT *pr	= new RECT;
			pr->left	= r.left;
			pr->right	= r.right;
			pr->top		= r.top;
			pr->bottom	= r.bottom;
			pAnno		= new ST_GEO_ANNO;
			pAnno->pRect	= pr;
			pAnno->nRectNum = 1;
			pAnno->szAnno	= pText;
			pAnno->byLevel = 0;
			pAnnoList->AddTail(pAnno);
		}
	}
	else
	{
		// 为了避免和前面同名变量混淆，将变量名称更改
		// 原来：RECT rect[BUFFER_SIZE_32] 改为：rt
		// 线状注记
		RECT rt[BUFFER_SIZE_32];

//		//计算文字显示方向
//		BOOL bSame = TRUE;
//		int nAngle =( CalcAngle( pPtList[nPtNum-1].dx,pPtList[nPtNum-1].dy,pPtList[0].dx,pPtList[0].dy ) *180 / PI );
//		if( (nAngle < 45 && nAngle > -135) || (nAngle < 360 && nAngle > 225) )
//			bSame = TRUE;
//		else
//			bSame = FALSE;

		//需删除的名称位置
		POSITION posSet[50];
		int nPos = 0;
		// 计算字与字之间是否重叠
		int i;
		for(i = 0; i < nPtNum; i++)
		{
			int j = i;
			if( bSameDirect == FALSE )
				j = nPtNum - i - 1 ;
			XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);
			rt[i].left	= nx;
			rt[i].top		= ny;
			rt[i].right	= nx + m_nW;
			rt[i].bottom	= ny + m_nH;

//			if(!IsAnnotate(rt[i]))
//			{
//				return;
//			}
//////////////////////////////////////////////////////////////////////////
			POSITION pos = pAnnoList->GetHeadPosition();

			while(pos != NULL)
			{
				ST_GEO_ANNO *pAnno;
				swRect rIn;
				
				POSITION posOld = pos;
				pAnno = (ST_GEO_ANNO *)pAnnoList->GetNext(pos);
				int k;
				for(k = 0; k < pAnno->nRectNum; k++)
				{
					rIn.SetRect(pAnno->pRect[k].left - 10, pAnno->pRect[k].top - 10, 
								pAnno->pRect[k].right + 10, pAnno->pRect[k].bottom + 10);
					
					if(IntersectRect(&rIn, &(rt[i])))
					{
						if( pAnno->byLevel < byLevel )
						{
							BOOL bHaveDel = FALSE;
							for( int k = 0 ; k < nPos ; k++ )
							{
								if( posOld == posSet[k] )
								{
									bHaveDel = TRUE;
									break;
								}
							}
							if( !bHaveDel )
							{
								posSet[nPos] = posOld;
								nPos ++;
							}
							break;
						}
						else
							return ;
					}
				}
			}

//	return TRUE;
	//////////////////////////////////////////////////////////////////////////

			if(i > 0 && IntersectRect(rt + i, rt + i - 1))
			{
				return;
			}
		}
		for( i = 0 ; i < nPos ; i++ )
		{
			pAnno = (ST_GEO_ANNO*)pAnnoList->GetAt( posSet[i] );
			if( pAnno )
			{
				if( pAnno->pRect )
				{
					delete[] pAnno->pRect;
					pAnno->pRect = NULL;
				}
				delete pAnno;
				pAnno = NULL;
			}
			pAnnoList->RemoveAt( posSet[i] );
		}

		RECT *pRect = new RECT[nPtNum];
		memcpy(pRect, rt, nPtNum * sizeof(RECT));
		pAnno = new ST_GEO_ANNO;
		pAnno->pRect	= pRect;
		pAnno->nRectNum = nPtNum;
		pAnno->szAnno	= pText;
		pAnnoList->AddTail(pAnno);
	}
}



///////////////////////////////////////////////////////////////////////////////
// 计算添加注释信息的位置
BOOL CGeoView::CalcAnnoStation(RECT &rect)
{
	int nW = int((rect.right - rect.left) / 2);
	int nH = int((rect.bottom - rect.top) / 2);

	swRect rIn;
	
	// 九个方向遍历，0为右上，顺时针依次旋转，7为正上方，8为覆盖文字位置
	for (int i = 0; i < 8; i ++)
	{
		if(IsAnnotate(rect))
		{
			return TRUE;
		}

		switch(i)
		{
			case 0 :
			case 1 : 
				rect.top += nH + Y_OFF;
				rect.bottom += nH + Y_OFF;
				break;
			case 2 :
			case 3 :
				rect.left -= nW + X_OFF;
				rect.right -= nW + X_OFF;
				break;
			case 4 :
			case 5 : 
				rect.top -= nH + Y_OFF;
				rect.bottom -= nH + Y_OFF;
				break;
			case 6 : 
			case 7 : 
				rect.left += nW + X_OFF;
				rect.right += nW + X_OFF;
				break;
		}
	}
	
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// 判断指定区域内是否有注释信息
BOOL CGeoView::IsAnnotate(RECT rect)
{
	POSITION pos = pAnnoList->GetHeadPosition();

	while(pos != NULL)
	{
		ST_GEO_ANNO *pAnno;
		swRect rIn;
		
		pAnno = (ST_GEO_ANNO *)pAnnoList->GetNext(pos);
		for(int k = 0; k < pAnno->nRectNum; k++)
		{
			rIn.SetRect(pAnno->pRect[k].left - 10, pAnno->pRect[k].top - 10, 
					    pAnno->pRect[k].right + 10, pAnno->pRect[k].bottom + 10);
			
			if(IntersectRect(&rIn, &rect))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 清除注释信息列表
void CGeoView::ClearAnnoList()
{
	ST_GEO_ANNO* pAnno;
	POSITION pos = pAnnoList->GetHeadPosition();
	while(pos != NULL)
	{
		POSITION ps = pos;
		pAnno = (ST_GEO_ANNO*)pAnnoList->GetNext(pos);
		if(m_bMoveMap)
		{
			BOOL bRemove = TRUE;//是否需要删除该注释
			for(int i = 0 ; i < pAnno->nRectNum ; i++)
			{
				(pAnno->pRect)[i].left += m_positionBMP.x;
				(pAnno->pRect)[i].right += m_positionBMP.x;
				(pAnno->pRect)[i].top += m_positionBMP.y;
				(pAnno->pRect)[i].bottom += m_positionBMP.y;

				
				if( IntersectRect( &(pAnno->pRect[i]),&plotRect ) )
					bRemove = FALSE;
			}
			if(bRemove)
			{
				delete [] pAnno->pRect;
				delete pAnno;
				pAnnoList->RemoveAt(ps);
			}
		}
		else
		{
			delete [] pAnno->pRect;
			delete pAnno;
		}
	}

	if(!m_bMoveMap)
		pAnnoList->RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////
// 准备画笔
void CGeoView::PreparePen(ST_GEO_PEN pen)
{
	m_pen.Detach();
	m_pen.CreatePen(pen.nStyle, pen.nWidth, pen.nColor);
	m_pOldPen = (CPen*)pCurrentDC->SelectObject(&m_pen);
}

///////////////////////////////////////////////////////////////////////////////
// 准备画刷
void CGeoView::PrepareBrush(ST_GEO_BRUSH brush)
{
	m_brush.Detach();
	m_brush.CreateSolidBrush(brush.nForeColor);
	m_pOldBrush = (CBrush*)pCurrentDC->SelectObject(&m_brush);
}

///////////////////////////////////////////////////////////////////////////////
// 释放画笔
void CGeoView::ReleasePen()
{
	pCurrentDC->SelectObject(m_pOldPen);
	m_pen.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////
// 释放画刷
void CGeoView::ReleaseBrush()
{
	pCurrentDC->SelectObject(m_pOldBrush);
	m_brush.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////
// 绘制地图
void CGeoView::DrawMap()
{
//	DWORD dwTime = GetTickCount();
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    //////////////////////////////////////////////////////////////////////////
    CSpaceMemoryLock aLock;    

    //////////////////////////////////////////////////////////////////////////

	if( m_ScaleCurLevel < 5 )
		SetbDrawByLayer( FALSE );
	if( m_ScaleCurLevel < 3 )
		m_bMoveMap = FALSE;

//取消直接绘制  scott 2008.11.27
	//CDC *pDC = GetDC();
	if( !m_bReDrawCar )
	{
		if( !m_bMoveMap )
			m_dxMoveError = m_dyMoveError = 0;
		if(m_Angle != 0)
			RotateRect = MaxNewRect(plotRect); //旋转之后的矩形大小

		pCurrentDC->SetBkMode(TRANSPARENT);
		PrepareBrush(m_BackBrush);
		PreparePen(m_BackPen);
		pCurrentDC->Rectangle(swRect(plotRect));
		ReleaseBrush();
		ReleasePen();

		m_ApartBrush.Detach();
		m_ApartBrush.CreateSolidBrush( m_BackBrush.nForeColor );
		m_ApartPen.Detach();
		m_ApartPen.CreatePen( m_BackPen.nStyle , m_BackPen.nWidth , m_BackPen.nColor );
		m_pOldApartBrush = m_pApartBackDC->SelectObject(&m_ApartBrush);
		m_pOldApartPen = m_pApartBackDC->SelectObject(&m_ApartPen);
		m_pApartBackDC->Rectangle(swRect(plotRect));
		m_pApartBackDC->SelectObject(m_pOldApartBrush);
		m_pApartBackDC->SelectObject(m_pOldApartPen);
		m_ApartBrush.DeleteObject();
		m_ApartPen.DeleteObject();
		

		
		ClearAnnoList();
		ClearDrawPOI();

		ST_GEO_RECT rtDraw;
		ST_GEO_RECT rtOld = GetOldDrawRect( );
		if(m_Angle != 0)
			rtDraw = GetRotateRect();
		else
			rtDraw = GetCurrentDrawrect();

		if(m_bMoveMap)
		{
			//pDC->SetBkMode(TRANSPARENT);
			if( m_OldScaleLevel != GetCurrentScaleLevel()  || ( !m_bIsVehMiddle && m_bMoveMap && m_bDrawByLayer ) )
			{
				DrawMapCompass( pCurrentDC );
			//	pDC->BitBlt(0, 0, plotRect.right - plotRect.left, plotRect.bottom - plotRect.top, 
				//		  pCurrentDC, 0, 0, SRCCOPY);

				PrepareBrush(m_BackBrush);
				PreparePen(m_BackPen);
				pCurrentDC->Rectangle(swRect(plotRect));
				ReleaseBrush();
				ReleasePen();
			}
			m_pdcMoveTemp->BitBlt(0,0,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,m_pPureBackDC,0,0,SRCCOPY);
			DrawTurnSign( m_pdcMoveTemp ); //绘制转向折线
			if( IsPtInRect( m_CurPt.dx , m_CurPt.dy , rtOld.dLeft,rtOld.dTop,rtOld.dRight,rtOld.dBottom ) )
			{
				int  nPosX = 0;
				int  nPosY = 0;
				
				XyMapToView(m_CurPt.dx, m_CurPt.dy, nPosX, nPosY );
				nPosX -= m_positionBMP.x;
				nPosY -= m_positionBMP.y;
				DrawCurPt( m_pdcMoveTemp , nPosX , nPosY );
			}
			if( m_bDrawByLayer &&  (m_OldScaleLevel != GetCurrentScaleLevel()  || ( !m_bIsVehMiddle ) ) )
			{
				m_pApartBackDC->BitBlt(m_positionBMP.x,m_positionBMP.y,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,
				   m_pdcMoveTemp, 0, 0, SRCCOPY);
				DrawMapCompass( m_pApartBackDC );
				//pDC->BitBlt(0,0,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,
				//   m_pApartBackDC, 0, 0, SRCCOPY);
			}
			pCurrentDC->BitBlt(m_positionBMP.x,m_positionBMP.y,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,
				   m_pdcMoveTemp, 0, 0, SRCCOPY);
		}
		dScale = dNewScale;//一度暂用多少个像素
		nAdjustx = (plotRect.right -(RotateRect.dRight - RotateRect.dLeft) * dScale) / 2.0
			        + ( ( (RotateRect.dRight+RotateRect.dLeft)/2.0 - (NewDrawRect.dRight+NewDrawRect.dLeft)/2.0 )*dScale ) ;
		nAdjusty = (plotRect.bottom - (RotateRect.dBottom - RotateRect.dTop) * dScale) / 2.0
			        - ( ( (RotateRect.dBottom+RotateRect.dTop)/2.0 - (NewDrawRect.dBottom+NewDrawRect.dTop)/2.0 )*dScale ) ;
		if( m_MapSet->GetCount() > 0 )
		{
			CPtrList DrawMapSet;
			ReLoadMapSet( m_MapSet, rtOld, rtDraw, dScale, MAPSECLUDE_SCALE, m_bMapSeclude, DrawMapSet );
		}
		drawRect = NewDrawRect;
		if(m_Angle != 0)
			drawRect = RotateRect;

		if(m_bMoveMap)
			pCurrentDC->BitBlt(m_positionBMP.x,m_positionBMP.y,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,
								m_pPureBackDC, 0, 0, SRCCOPY);
		
		ST_GEO_PEN pen;
		pen.nColor = RGB( 128,64,64 );
		pen.nStyle = PS_SOLID;
		pen.nWidth = 2;
		ST_GEO_BRUSH brush;
		brush.nForeColor = GetBackColor();
		PreparePen( pen );
		PrepareBrush( brush );
		DrawPOI();
		ReleaseBrush();
		ReleasePen();
//
//		DrawNetworkRoute();
//		DrawAnnotate();

		m_pPureBackDC->BitBlt(0, 0, plotRect.right - plotRect.left, plotRect.bottom - plotRect.top, 
						pCurrentDC, 0, 0, SRCCOPY);
	}
	else
		pCurrentDC->BitBlt(0, 0, plotRect.right - plotRect.left, plotRect.bottom - plotRect.top, 
						m_pPureBackDC, 0, 0, SRCCOPY);


	DrawNetworkRoute();
	DrawAnnotate();


	int  nPosX = 0;
	int  nPosY = 0;
	
	pBackDC->BitBlt(0, 0, plotRect.right - plotRect.left, plotRect.bottom - plotRect.top, 
					pCurrentDC, 0, 0, SRCCOPY);
	
	XyMapToView( m_CurPt.dx, m_CurPt.dy, nPosX, nPosY );
	DrawTurnSign( pBackDC );
	DrawCurPt( pBackDC , nPosX , nPosY );
	DrawMapCompass( pBackDC );
	if( m_bShowTip )
		ShowCursorFind( pBackDC );
	if( m_bShowFocusRut )
		ShowCursorFindRut( pBackDC );
	if( m_bShowSpecalRut )
		ShowSpecialRut( pBackDC );
	if( m_FocusPtSet.GetCount() > 0 )
		ShowFocusPOI( pBackDC );
	m_bShowTip = FALSE;
	m_bShowFocusRut = FALSE;

	m_bMoveMap = FALSE;


	SetbDrawByLayer( TRUE );
//	ReleaseDC( pDC );

}


///////////////////////////////////////////////////////////////////////////////
// 绘制图形点
void CGeoView::DrawPoint(int nx, int ny,short SymbolID)
{
	POINT pt;
	pt.x = nx - 8;
	pt.y = ny - 8;
	//04.11.11
	if(SymbolID >= 0)
	{ 
		ImageList.Draw(pCurrentDC, SymbolID, pt, ILD_TRANSPARENT);
	}
	else
	{
		pCurrentDC->Ellipse(nx - 6, ny - 6, nx + 6, ny + 6);
	}
}

///////////////////////////////////////////////////////////////////////////////
// 绘制线
void CGeoView::DrawLine(ST_GEO_PTXY* pPtList, int nPtNum)
{
	if( !pPtList || nPtNum <= 1)
		return;
	POINT *pPt = NULL; 
	int nx, ny;
	int nxEnd,nyEnd;
	int j = 0;
	int nxLast, nyLast;
	

	int nCurrentScale = GetCurrentScale();
	int nFilter = 4;
	if( nCurrentScale > 100 )
		nFilter = 6;
	double dbXOld = pPtList[j].dx ;
	double dbYOld = pPtList[j].dy ;
	int nRealNum = 1;
	XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);
	XyMapToView(pPtList[nPtNum-1].dx, pPtList[nPtNum-1].dy, nxEnd, nyEnd );
	pPt = new POINT[nPtNum];
	pPt[0].x = nx;
	pPt[0].y = ny;
	if( nCurrentScale > 100 )
	{
		if( nx == nxEnd && ny == nyEnd )
		{
			delete[] pPt;
			return;
		}
		if( abs(nxEnd-nx)<nFilter && abs(nyEnd-ny)<nFilter )
		{
			pPt[0].x = nx;
			pPt[0].y = ny;
			pPt[1].x = nxEnd;
			pPt[1].y = nyEnd;
			pCurrentDC->Polyline( pPt,2 );
			delete[] pPt;
			return;
		}
	}
	j++;
	nxLast = nx;
	nyLast = ny;

	int nXDis , nYDis;
	for(; j < nPtNum; j++)
	{
		if( j < nPtNum-1 )
		{
			nXDis = ( pPtList[j].dx - dbXOld ) * dNewScale ;
			if( abs(nXDis) < nFilter )
			{
				nYDis = ( dbYOld - pPtList[j].dy ) * dNewScale ;
				if( abs(nYDis) < nFilter )
					continue;
			}
		}
		if( j == nPtNum-1)
		{
			nx = nxEnd;
			ny = nyEnd;
		}
		else
			XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);
		nxLast = nx;
		nyLast = ny;

		dbXOld = pPtList[j].dx;
		dbYOld = pPtList[j].dy;
		pPt[nRealNum].x = nx;
		pPt[nRealNum].y = ny;
		nRealNum ++ ;
	}
	
	pCurrentDC->Polyline( pPt,nRealNum );
	delete[] pPt;
	pPt = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 绘制线
void CGeoView::DrawLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum, int nType)
{
	switch(nType)
	{
	case 6:
		DrawRailLine(pPtList, nPtNum, pPart, nPartNum);
		break;
	case 7:
		DrawDoubleLine(pPtList, nPtNum, pPart, nPartNum);
		break;
	default:
		DrawPartLine(pPtList, nPtNum, pPart, nPartNum);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 绘制多义线
void CGeoView::DrawPolyLine(ST_GEO_RECT *pRect)
{
	POINT pt[5];

	int nx,ny;
	XyMapToView(pRect->dLeft, pRect->dTop, nx, ny);
	pt[0].x = nx;
	pt[0].y = ny;
	XyMapToView(pRect->dRight, pRect->dTop, nx, ny);
	pt[1].x = nx;
	pt[1].y = ny;
	XyMapToView(pRect->dRight, pRect->dBottom, nx, ny);
	pt[2].x = nx;
	pt[2].y = ny;
	XyMapToView(pRect->dLeft, pRect->dBottom, nx, ny);
	pt[3].x = nx;
	pt[3].y = ny;
	pt[4].x = pt[0].x;
	pt[4].y = pt[0].y;

	pCurrentDC->Polyline(pt, 5);
}

///////////////////////////////////////////////////////////////////////////////
// 绘制多边形
#ifdef _WIN32_WCE
static POINT g_PolyPoint[20000]; 
#else
static POINT g_PolyPoint[20000*5];  // scott 
#endif

void CGeoView::DrawPoly(ST_GEO_PTXY* pPtList, int nPtNum)
{
	if( !pPtList || nPtNum <= 2)
		return;
	POINT *pPt = NULL; 
	int nx, ny;
	int nxEnd,nyEnd;
	int j = 0;
	int nxLast, nyLast;
	

	double dbXOld = pPtList[j].dx ;
	double dbYOld = pPtList[j].dy ;
	int nRealNum = 1;
//	pPt = new POINT[nPtNum];
	pPt = g_PolyPoint;  //地图多边形的定点坐标转换为屏幕坐标
	XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);
	pPt[0].x = nx;
	pPt[0].y = ny;
	j++;
	nxLast = nx;
	nyLast = ny; //封闭定点，要回路过来

	int nXDis , nYDis;
	for(; j < nPtNum; j++)
	{
//		if( j < nPtNum-1 )
//		{
//			nXDis = ( pPtList[j].dx - dbXOld ) * dNewScale ;
//			if( abs(nXDis) == 0 )
//			{
//				nYDis = ( dbYOld - pPtList[j].dy ) * dNewScale ;
//				if( abs(nYDis) == 0 )
//					continue;
//			}
//		}
		XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);
		if( nx == nxLast && ny == nyLast )
			continue;
		nxLast = nx;
		nyLast = ny;

		dbXOld = pPtList[j].dx;
		dbYOld = pPtList[j].dy;
		pPt[nRealNum].x = nx;
		pPt[nRealNum].y = ny;
		nRealNum ++ ;
	}
	if( nRealNum > 2 )
		pCurrentDC->Polygon( pPt , nRealNum );
//	delete[] pPt;
//	pPt = NULL;
}
void CGeoView::DrawPoly(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum)
{
	if( !pPtList || nPtNum <= 2 )
		return;
	int nx, ny;

	POINT* pt = new POINT[nPtNum];

	for(int i = 0; i < nPtNum; i++)
	{
		XyMapToView(pPtList[i].dx, pPtList[i].dy, nx, ny);

		pt[i].x = nx;
		pt[i].y = ny;
//		nCount++ ;
	}
	
	if (nPartNum == 1)
	{
		pCurrentDC->Polygon(pt, nPtNum);
	}
	else
	{
		pCurrentDC->PolyPolygon(pt, pPart, nPartNum);
	}

	delete [] pt;
	pt = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 绘制注释
void CGeoView::DrawAnnotate()
{
	ST_GEO_ANNO* pAnno;
	POSITION pos = pAnnoList->GetHeadPosition();
	swString str;
	while(pos != NULL)
	{
		pAnno = (ST_GEO_ANNO*)pAnnoList->GetNext(pos);
		str = pAnno->szAnno;
//		CharToUnicode(pAnno->szAnno, &str);

		COLORREF ColText = pCurrentDC->GetTextColor();
		COLORREF ColOut = RGB(255,255,255);
		if(pAnno->nRectNum == 1)
		{
			pCurrentDC->SetTextColor( ColOut );
			RECT rtTemp = *(pAnno->pRect);
			rtTemp.left = pAnno->pRect->left++ ;
			rtTemp.right = pAnno->pRect->right++ ;
			pCurrentDC->DrawText( str , &rtTemp  , DT_SINGLELINE | DT_NOCLIP );

			rtTemp = *(pAnno->pRect);
			rtTemp.left = pAnno->pRect->left-- ;
			rtTemp.right = pAnno->pRect->right-- ;
			pCurrentDC->DrawText( str , &rtTemp  , DT_SINGLELINE | DT_NOCLIP );

			rtTemp = *(pAnno->pRect);
			rtTemp.top = pAnno->pRect->top++  ;
			rtTemp.bottom = pAnno->pRect->bottom++ ;
			pCurrentDC->DrawText( str , &rtTemp  , DT_SINGLELINE | DT_NOCLIP );

			rtTemp = *(pAnno->pRect);
			rtTemp.top = pAnno->pRect->top--  ;
			rtTemp.bottom = pAnno->pRect->bottom-- ;
			pCurrentDC->DrawText( str , &rtTemp  , DT_SINGLELINE | DT_NOCLIP );

			pCurrentDC->SetTextColor(ColText);
			pCurrentDC->DrawText( str , pAnno->pRect  , DT_SINGLELINE | DT_NOCLIP );
			//以上代码绘制文本效果，带有阴影注释
//			DrawOutlineForChars(pCurrentDC,pAnno->pRect->left,pAnno->pRect->top,str,ColOut,ColText );

//			pCurrentDC->DrawText(str, pAnno->pRect, DT_SINGLELINE | DT_NOCLIP);
		}
		else //单独绘制每一个字符到不同的矩形区域
		{
			TCHAR tch;
			for(int i = 0; i < pAnno->nRectNum; i++)
			{
				tch = str.GetAt(i);
				pCurrentDC->SetTextColor( ColOut );
				RECT rtTemp = *( pAnno->pRect+i);
				rtTemp.left = ( pAnno->pRect+i)->left++ ;
				rtTemp.right = ( pAnno->pRect+i)->right++ ;
				pCurrentDC->DrawText( &tch , 1,&rtTemp  , DT_SINGLELINE | DT_NOCLIP );

				rtTemp = *( pAnno->pRect+i);
				rtTemp.left = ( pAnno->pRect+i)->left-- ;
				rtTemp.right = ( pAnno->pRect+i)->right-- ;
				pCurrentDC->DrawText( &tch , 1,&rtTemp  , DT_SINGLELINE | DT_NOCLIP );

				rtTemp = *( pAnno->pRect+i);
				rtTemp.top = ( pAnno->pRect+i)->top++  ;
				rtTemp.bottom = ( pAnno->pRect+i)->bottom++ ;
				pCurrentDC->DrawText( &tch , 1,&rtTemp  , DT_SINGLELINE | DT_NOCLIP );

				rtTemp = *( pAnno->pRect+i);
				rtTemp.top = ( pAnno->pRect+i)->top--  ;
				rtTemp.bottom = ( pAnno->pRect+i)->bottom-- ;
				pCurrentDC->DrawText( &tch , 1,&rtTemp  , DT_SINGLELINE | DT_NOCLIP );

				pCurrentDC->SetTextColor(ColText);
				pCurrentDC->DrawText( &tch , 1, pAnno->pRect+i  , DT_SINGLELINE | DT_NOCLIP );
//				pCurrentDC->DrawText(&tch, 1, pAnno->pRect+i, DT_SINGLELINE | DT_NOCLIP);
//				DrawOutlineForChars( pCurrentDC,(pAnno->pRect+i)->left,(pAnno->pRect+i)->top,(swString)tch,ColOut,ColText );
			}
		}
	}

//	ClearAnnoList();
}

///////////////////////////////////////////////////////////////////////////////
// 绘制标示
void CGeoView::DrawFlag(double dx, double dy, int nNo)
{
	int nx,ny;
	XyMapToView(dx, dy, nx, ny);
	CPoint pt;
	pt.x = nx;
	pt.y = ny - 16;

//	HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICON_FLAG);
//	pCurrentDC->DrawIcon(pt,hIcon);
	HICON Icon ;
	if( nNo == 0 )
		Icon = m_FlaghIcon;
	else
		Icon = m_MidFlagIcon;
	if( Icon && IsPtInScreen(pt.x,pt.y,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom) )
		pCurrentDC->DrawIcon(pt,Icon);
}

///////////////////////////////////////////////////////////////////////////////
// 绘制当前点
void CGeoView::DrawCurPt( CDC* pDC , int nPosX, int nPosY)
{
	if ( pDC == NULL ) return;

//	int  nPosX = 0;
//	int  nPosY = 0;
//	
//	XyMapToView(m_CurPt.dx, m_CurPt.dy, nPosX, nPosY );

	if ( m_bShowVehSymbol ) //绘制车辆
	{
		CPoint ptDraw;
		ptDraw.x = nPosX - 20;
		ptDraw.y = nPosY - 20; 
		int nAngleID = (m_nVehAngle / 15.0 + 0.5 );
		if( nAngleID > 23 || nAngleID < 0 )
			nAngleID = 0;
		
		m_CarImageList.Draw( pDC, nAngleID, ptDraw, ILD_TRANSPARENT );

//		double sina = sin((360-m_nVehAngle) * PI / 180.0);
//		double cosa = cos((360-m_nVehAngle) * PI / 180.0);
//
//		short  nVehW = 8;
//		short  nVehH = 10;
//
//		POINT  ptVeh[3];
//		ZeroMemory(&ptVeh[0], sizeof(POINT)*3);
//	
//		ptVeh[0].x =  (LONG)((-nVehW) * cosa + (nVehH) * sina);
//		ptVeh[0].y =  (LONG)(-(-nVehW) * sina + (nVehH) * cosa);
//		ptVeh[1].x =  (LONG)((0) * cosa + (-nVehH ) * sina);
//		ptVeh[1].y =  (LONG)(-(0) * sina + (-nVehH ) * cosa);
//		ptVeh[2].x =  (LONG)((nVehW) * cosa + (nVehH) * sina);
//		ptVeh[2].y =  (LONG)(-(nVehW) * sina + (nVehH) * cosa);
//
//		for(int i = 0; i < 3; i++ )
//		{
//			ptVeh[i].x += nPosX;
//			ptVeh[i].y += nPosY;
//		}
//
//		if ( m_hVehPen == NULL )
//			m_hVehPen = ::CreatePen( PS_SOLID, 0, RGB(0,0,255) );
//
//		if ( m_hVehBrs == NULL )
//			m_hVehBrs = ::CreateSolidBrush( RGB(0, 0, 255) );
//
//		HPEN    hOldPen = (HPEN)::SelectObject(hDC, m_hVehPen );
//		HBRUSH  hOldBsh = (HBRUSH)::SelectObject(hDC, m_hVehBrs );
//		::Polygon(hDC, &ptVeh[0], 3 );
//
////		int nRadius = 0.5 + CMainControl::GetMainControl()->m_VehDrvStat.GetSysPrecision();
//		int nRadius = 0.5 + m_nRadius;
//		nRadius = (nRadius) / (this->GetCurrentScale()); 
//		if ( nRadius > 10 ) 
//		{			
//			if ( m_hClePen == NULL )
//				m_hClePen = ::CreatePen(PS_SOLID,1,RGB(255, 0, 0));
//			
//			::SelectObject(hDC, m_hClePen );
//			::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH) );
//			
//			::Ellipse(hDC, nPosX - nRadius ,nPosY - nRadius * m_fHightScale, 
//				nPosX + nRadius, nPosY + nRadius * m_fHightScale );
//			
//			::SelectObject(hDC, hOldPen );
//			::SelectObject(hDC, hOldBsh );
//		}
	}else
	{
		
		if ( m_bmpForBmp.GetSafeHandle() == NULL )
			m_bmpForBmp.LoadBitmap( IDB_SYMBOL_UNKNOW );
		
		BITMAP  bm;
		m_bmpForBmp.GetBitmap(&bm);
		
#ifdef _WIN32_WCE		
		::TransparentImage(pDC->GetSafeHdc(), nPosX-bm.bmWidth/2,nPosY-bm.bmHeight/2,
			               bm.bmWidth, bm.bmHeight, 
						   m_bmpForBmp.GetSafeHandle(),
						   0,0,bm.bmWidth,bm.bmHeight,RGB(255,0,255) );
#else
		//ASSERT(FALSE);//TO BE ADD
#endif
	}
}

///////////////////////////////////////////////////////////////////////////////
// 绘制间断线
void CGeoView::DrawPartLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum)
{
	int nx, ny;
	int j = 0;
	int nxLast, nyLast;
	int nPart;

	for(int i = 0; i < nPartNum; i++)
	{
		XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);

		pCurrentDC->MoveTo(nx, ny);
		nxLast = nx;
		nyLast = ny;
		j++;
		nPart = pPart[i]-1;
		
		while(nPart > 0)
		{
			XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);


			if((nx != nxLast) || (ny != nyLast))
			{
				pCurrentDC->LineTo(nx, ny);
				nxLast = nx;
				nyLast = ny;
			}
			
			j++;
			nPart--;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// 绘制铁路线
//黑的出线，中间是白色的间隔线
void CGeoView::DrawRailLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum)
{
	CPen *pOldPen, pPen;
	pPen.CreatePen(PS_SOLID, 3, RGB(100, 100, 100));
	pOldPen = (CPen*)pCurrentDC->SelectObject(&pPen);
	DrawPartLine(pPtList,nPtNum,pPart,nPartNum);
	pCurrentDC->SelectObject(pOldPen);
	pPen.DeleteObject();

	pPen.CreatePen(PS_DASH,1,RGB(255,255,255));
	pCurrentDC->SelectObject(&pPen);
	DrawPartLine(pPtList,nPtNum,pPart,nPartNum);

	pCurrentDC->SelectObject(pOldPen);
	pPen.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////
// 绘制双线
//简单的方法在3宽度中间绘制一条白线
void CGeoView::DrawDoubleLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum)
{
	CPen *pOldPen, pPen;
	pPen.CreatePen(PS_SOLID, 3, RGB(100, 100, 100));

	pOldPen = (CPen*)pCurrentDC->SelectObject(&pPen);
	DrawPartLine(pPtList, nPtNum, pPart, nPartNum);
	pCurrentDC->SelectObject(pOldPen);
	pPen.DeleteObject();

	pPen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	pCurrentDC->SelectObject(&pPen);
	DrawPartLine(pPtList, nPtNum, pPart, nPartNum);

	pCurrentDC->SelectObject(pOldPen);
	pPen.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////
// 绘制选定层数据

void CGeoView::DrawSel()
{
}

///////////////////////////////////////////////////////////////////////////////
// 绘制选定线路
void CGeoView::DrawFindRut()
{
}

///////////////////////////////////////////////////////////////////////////////
void CGeoView::DrawNomalLine(double dx1, double dy1, double dx2, double dy2, DWORD dwR, DWORD dwG, DWORD dwB)
{				
	int nx1, ny1,nx2,ny2;
	CPen pen, *pOldPen;
	
	XyMapToView(dx1, dy1, nx1, ny1);
	XyMapToView(dx2, dy2, nx2, ny2);

	pen.CreatePen(PS_SOLID, 4, RGB(dwR, dwG, dwB));
	pOldPen = (CPen*)pCurrentDC->SelectObject(&pen);
	pCurrentDC->MoveTo(nx1, ny1);
	pCurrentDC->LineTo(nx2, ny2);
	pCurrentDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

void CGeoView::RotatePlot(float &X1, float &Y1, POINT RotateBasePt)
{
	float x = X1 - RotateBasePt.x;
	float y = Y1 - RotateBasePt.y;
	X1 = RotateBasePt.x + x*m_cosRotate - y*m_sinRotate;
	Y1 = RotateBasePt.y + x*m_sinRotate + y*m_cosRotate;
}

void CGeoView::UnRotatePlot(float &X1, float &Y1, POINT RotateBasePt)
{
	float x = X1 - RotateBasePt.x;
	float y = Y1 - RotateBasePt.y;
	X1 = RotateBasePt.x + x*m_cosRotate + y*m_sinRotate;
	Y1 = RotateBasePt.y - x*m_sinRotate + y*m_cosRotate;
}

// 修正NewDrawRect 可视区域的地理经纬度边界
void CGeoView::MoveToCenter(double dX, double dY)
{
	if(m_bMoveMap)
		return;
	int nx,ny;
//	XyMapToView(dX,dY,nx,ny);


	//////////////////////////////////////////////////////////////////////////
	float tx,ty;
	tx = nAdjustx + (dX - RotateRect.dLeft) * dScale;
	ty = (RotateRect.dBottom - dY) * dScale + nAdjusty;
	if(m_Angle != 0)
	{
		//地图转角度
		POINT ptCenter;
		ptCenter = m_ptRotateCenter;
		RotatePlot(tx,ty,ptCenter);
 	}
	//////////////////////////////////////////////////////////////////////////
	float dbTempx = (plotRect.right/2 - tx); //中心点与要设置的中心点的x的偏差
	float dbTempy = (plotRect.bottom/2 - ty);
	int nTempx = (dbTempx+m_dxMoveError);
	int nTempy = (dbTempy+m_dyMoveError);
	m_dxMoveError = (dbTempx+m_dxMoveError) - nTempx ;
	m_dyMoveError = (dbTempy+m_dyMoveError) - nTempy ;


	double dx1, dy1, dx2,dy2;
	XyViewToMap(plotRect.right / 2,plotRect.bottom / 2,dx1,dy1);

	//根据与中心点的偏差，计算新的地理矩形
	NewDrawRect.dLeft	= (float)(NewDrawRect.dLeft		+ dX - dx1);
	NewDrawRect.dTop	= (float)(NewDrawRect.dTop		+ dY - dy1);
	NewDrawRect.dRight	= (float)(NewDrawRect.dRight	+ dX - dx1);
	NewDrawRect.dBottom = (float)(NewDrawRect.dBottom	+ dY - dy1); //调整可视地图区域，根据当前设置点与中心点的偏移
	NormalizeRect(NewDrawRect); //修正矩形

	if(m_Angle != 0)
	{
		RotateRect.dLeft	= (float)(RotateRect.dLeft	+ dX - dx1);
		RotateRect.dTop	= (float)(RotateRect.dTop		+ dY - dy1);
		RotateRect.dRight	= (float)(RotateRect.dRight	+ dX - dx1);
		RotateRect.dBottom = (float)(RotateRect.dBottom	+ dY - dy1);
		NormalizeRect(RotateRect);
	}
	if(m_Angle == 0)
	{
		RotateRect = NewDrawRect;
	}
	m_positionBMP.x = nTempx ;
	m_positionBMP.y = nTempy ;


	m_bMoveMap = TRUE;
}

BOOL CGeoView::Update(IGIS_Message* pMessage)
{
	if(pMessage != NULL)
	{
		Refresh();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
double CGeoView::GetCurrentScale()
{
//	return m_ScaleCurLevel;
	return m_dbScaleValue[m_ScaleCurLevel - 1];
}
UINT CGeoView::GetCurrentScaleLevel()
{
	return m_ScaleCurLevel;
}
ST_GEO_RECT CGeoView::GetCurrentDrawrect()
{
	return NewDrawRect;
}
CPtrList* CGeoView::GetMapSet()
{
	return m_MapSet;
}
RECT CGeoView::GetViewPlot()
{
	return plotRect;
}
void CGeoView::LoadMap(swString* szFileName)
{
}
CDC* CGeoView::GetBackDC()
{
	if(pBackDC != NULL)
	{
		return pBackDC;
	}
	else
	{
		return NULL;
	}
}
CDC *CGeoView::GetBufDC()
{
	if( pBufDC != NULL )
		return pBufDC;
	return NULL;
}
CDC *CGeoView::GetCurrentDC()
{
	if( pCurrentDC != NULL )
		return pCurrentDC;
	return NULL;
}
CBitmap* CGeoView::GetBackBMP()
{
	if(pBackBMP != NULL)
	{
		return pBackBMP;
	}
	else
	{
		return NULL;
	}
}
void CGeoView::SetCenterPt(CPoint pt)
{
	m_CenterPt = pt;
}
ST_GEO_PTXY	CGeoView::GetCurPt()
{
	return m_CurPt;   
}
void CGeoView::SetCurPt(ST_GEO_PTXY pt)
{
//	ST_GEO_RECT rtChina;
//	rtChina.dLeft = 70;
//	rtChina.dRight = 137.419;
//	rtChina.dTop = 0;
//	rtChina.dBottom = 55.3626;
//	if( !IsPtInRect( pt.dx , pt.dy , rtChina.dLeft , rtChina.dTop ,
//		rtChina.dRight , rtChina.dBottom ) )
//		return;
	m_CurPt = pt;
}

BOOL CGeoView::SetMap(IGIS_Map* pMap)
{
	if(m_MapSet != NULL && pMap != NULL)
	{
		POSITION pos = m_MapSet->GetHeadPosition();
		IGIS_Map *pTem = NULL;
		while( pos )
		{
			pTem = (IGIS_Map*)m_MapSet->GetNext(pos);
			if( pMap == pTem )
				return FALSE;
		}
//		SetLayerDrawState(pMap->GetMapInfo()->m_nLayerNum);
		m_MapSet->AddTail(pMap);
		m_RichPOIView.SetRichPOIFilePath( pMap->GetMapInfo()->GetMapFilePath() );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
ST_GEO_RECT CGeoView::GetOldDrawRect()
{
	return drawRect;
}
BOOL CGeoView::ReleaseMap(IGIS_Map* pMap) 
{
	if(m_MapSet != NULL && pMap != NULL)
	{
		POSITION pos = m_MapSet->GetHeadPosition();
		POSITION ps = NULL;
		IGIS_Map* pTempMap;
		while(pos)
		{
			ps = pos;
			pTempMap = (IGIS_Map*)(m_MapSet->GetNext(pos));
			if(pTempMap == pMap)
			{
				m_MapSet->RemoveAt(ps);
				return TRUE;
			}
		}
		return FALSE;

	}
	else
	{
		return FALSE;
	}
}

//查找在指定可视范围内的地理图层,这里不区分图副下的图层，只考虑这25层图层
void CGeoView::GetMSLayer( CGIS_GeoMap *pMap, double dbScale, CGIS_MapLayer **pLList, int& nCount )
{
	ASSERT( pMap );
	nCount = 0;
	POSITION pos = pMap->m_LayerManagerSet->GetHeadPosition();
	CGIS_LayerManager *pLManager = NULL;
	while( pos )
	{ //每个地图拥有多个图层管理器
	//图层管理器也就是一个图层容器，这里是map<int,layer*>
		pLManager = (CGIS_LayerManager*)pMap->m_LayerManagerSet->GetNext(pos);
		if( pLManager )
		{
			int nLNum = pLManager->GetLayerListMap()->GetCount();
			if( nLNum < 25 )
				nLNum = 25; //?
			IGIS_Layer *pLayer = NULL;
			//第一图层不加载
			for( int i=0;	i<nLNum;	i++ )
			{
				if( pMap->GetMapInfo()->GetMapID() == MAPID_MAP_DETAIL 
					&&	i == 0 )
					continue;
				pLayer = NULL;
				pLManager->GetLayerListMap()->Lookup( i, pLayer );
				if( pLayer )
				{
					if( pLayer->GetLayerInfo()->GetLayerState() &&
						dbScale > pLayer->GetLayerInfo()->GetMinGRScale() &&
						dbScale < pLayer->GetLayerInfo()->GetMaxGRScale() ) //判别图层是否当前可见
					{
						pLList[nCount] = (CGIS_MapLayer*)pLayer; //返回在当前缩放比例下的图层
						nCount++;
					}
				}
			}
		}
	}
}
void CGeoView::DrawPoint( CGIS_FeaturePoint *pPoint, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno )
{
	BOOL bInOld =  IsPtInRect(pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, rtOld.dLeft, 
				  rtOld.dTop, rtOld.dRight, rtOld.dBottom); //落到了无效的区域内
	if(m_bMoveMap && bInOld && m_Angle == 0)
		return;
	if( (IsPtInRect(pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, rtDraw.dLeft, 
		  rtDraw.dTop, rtDraw.dRight, rtDraw.dBottom)) )
	{
		int nx,ny;
		XyMapToView( pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, nx, ny );
//		if( IsPtInScreen( nx, ny, rtPlot.left, rtPlot.right, rtPlot.top, rtPlot.bottom ) )
		{
//			DrawPoint( pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, pPoint->GetIConID() );
			ST_PT_DRAW *ptDraw = new ST_PT_DRAW;
			ptDraw->nIconID = pPoint->GetIConID();
			ptDraw->nx = nx;
			ptDraw->ny = ny;
			if( !AddDrawPOI( ptDraw ) )
			{
				delete ptDraw;
				ptDraw = NULL;
			}
			if(bAnno && pPoint->GetAnnotation()!=NULL )
			{
				swString str;
				CharToUnicode( pPoint->GetAnnotation() , &str );
				AddAnnotate( pPoint->GetPtList(), 1, str );
				//Annotate(pView, m_pListObj + i);
			}
		}
	}
}
void CGeoView::DrawLine( CGIS_FeatureLine *pLine, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno, BYTE nStyle )
{
	int nClipType = GeoRectIntersect( &rtDraw, &pLine->GetRectObj() );
	if(nClipType == 0)
		return;
	if( m_bMoveMap && GeoRectIntersect( &rtOld,&pLine->GetRectObj() ) ==2 && m_Angle == 0)
		return; //rtOld可能是之前的无效的视窗地理区域，因为包含此feature所以不用绘制

	BOOL bState = FALSE;
	if(nClipType == 2 )
	{
		bState = TRUE;
	}
//	else
//	{
//		if( pLine->GetPartNum() > 1 )
//			bState = TRUE;
//		else
//			bState = FALSE;
//	}
	if( bState ) // 2   rect include feature object
	{	//绘制由点构成的线段
		DrawLine( pLine->GetPtList(), pLine->GetPtNum(), pLine->GetPart(),
						pLine->GetPartNum(), nStyle);
		
		if(bAnno && pLine->GetAnnotation()!=NULL )
		{
//			if( nStyle == 0 )
//			{
				ST_GEO_PTXY pt; //线段中心点
				pt.dx = (pLine->GetRectObj().dRight+pLine->GetRectObj().dLeft)/2.0;
				pt.dy = (pLine->GetRectObj().dBottom+pLine->GetRectObj().dTop)/2.0;

				swString str;
				CharToUnicode( pLine->GetAnnotation() , &str );
				AddAnnotate( &pt, 1, str );
//			}
//			else
//			{
//				int nStrNum = swString(pLine->GetAnnotation()).GetLength();
//
//				if( nStrNum <= pLine->GetPtNum() )
//				{
//					ST_GEO_PTXY *pPt = new ST_GEO_PTXY[nStrNum];
//					BOOL bAdd = GetPointsOnLine( pLine->GetPtList(), pLine->GetPtNum(), nStrNum, pPt) ;
//					if( bAdd )
//					{
//						AddAnnotate( pPt, nStrNum, pLine->GetAnnotation() );
//					}
//					delete [] pPt;
//					pPt = NULL;
//				}	
//			}
		}
	}
	else // 1,3相交或者feature包含地理区域
	{
		if( pLine->GetPartNum() == 1 )
		{
			int nNum;
			ST_GEO_RECT rtTemp;
			double dbClip = 10 / dNewScale;
			rtTemp.dBottom = rtDraw.dBottom + dbClip;
			rtTemp.dTop = rtDraw.dTop - dbClip;
			rtTemp.dLeft = rtDraw.dLeft - dbClip;
			rtTemp.dRight = rtDraw.dRight + dbClip;
			ST_GEO_PTXY* pPtList = GetClipRgn( pLine->GetPtList(), pLine->GetPtNum(), 
									rtTemp, &nNum, pLine->GetObjType() );
			if(nNum > 0 && pPtList )
			{
				DrawLine( pPtList, nNum, &nNum, 1, nStyle );
				if(bAnno && pLine->GetAnnotation()!=NULL )
				{
//					if( nStyle == 0 )
//					{
						ST_GEO_PTXY pt;
						pt.dx = (pLine->GetRectObj().dRight+pLine->GetRectObj().dLeft)/2.0;
						pt.dy = (pLine->GetRectObj().dBottom+pLine->GetRectObj().dTop)/2.0;
						swString str;
						CharToUnicode( pLine->GetAnnotation() , &str );
						AddAnnotate( &pt, 1, str );
//					}
//					else
//					{
//						int nStrNum = swString(pLine->GetAnnotation()).GetLength();
//
//						if( nStrNum <= nNum )
//						{
//							ST_GEO_PTXY *pPt = new ST_GEO_PTXY[nStrNum];
//							BOOL bAdd = GetPointsOnLine( pPtList, nNum, nStrNum, pPt) ;
//							if( bAdd )
//							{
//								AddAnnotate( pPt, nStrNum, pLine->GetAnnotation() );
//							}
//							delete [] pPt;
//							pPt = NULL;
//						}	
//					}
				}
			}
//			delete []pPtList;
		}
		else //多段
		{
			
			ST_GEO_RECT rtTemp;
			ST_GEO_PTXY *ptTemp;
			int nPtStart = 0;
			for( int i = 0 ; i < pLine->GetPartNum() ; i ++ )
			{
				if( i>0 )
					nPtStart += pLine->GetPart()[i-1];
				ptTemp = &(pLine->GetPtList()[nPtStart]);
				::CalcRect(ptTemp,pLine->GetPart()[i],rtTemp);
				int nClipMul = GeoRectIntersect( &rtDraw, &rtTemp );
				if( nClipMul == 2 )
				{
					DrawLine( ptTemp , pLine->GetPart()[i] );
				}
				else if( nClipMul != 0 )
				{
					int nNum;
					
					// 为了不和前面的同名变量发生混淆，将变量名更改
					// 原来：ST_GEO_RECT rtTemp 改为：rtNewTemp
					ST_GEO_RECT rtNewTemp;
					double dbClip = 10 / dNewScale;
					rtNewTemp.dBottom = rtDraw.dBottom + dbClip;
					rtNewTemp.dTop = rtDraw.dTop - dbClip;
					rtNewTemp.dLeft = rtDraw.dLeft - dbClip;
					rtNewTemp.dRight = rtDraw.dRight + dbClip;
					ST_GEO_PTXY* pPtList = GetClipRgn( ptTemp, pLine->GetPart()[i], 
									rtNewTemp, &nNum, pLine->GetObjType() );
					if( pPtList )
					{
						DrawLine( ptTemp , nNum );
//						delete [] pPtList;
					}
				}
					
			}
		}
	}
}
void CGeoView::DrawWaterPoly( CGIS_FeaturePoly *pPoly, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno )
{
	int nClipType = GeoRectIntersect( &rtDraw, &pPoly->GetRectObj() );
	if(nClipType == 0)
		return;
	if(m_bMoveMap && GeoRectIntersect( &rtOld,&pPoly->GetRectObj() ) ==2  && m_Angle == 0)
		return;

	//控制水系的显示密度
	double dx = pPoly->GetRectObj().dRight - pPoly->GetRectObj().dLeft;
	double dy = pPoly->GetRectObj().dBottom - pPoly->GetRectObj().dTop;
	//水系的显示范围 1：无论何种比例尺均显示 2：比例尺小于7时才显示 3：比例尺小于5时才显示
	int nWaterLevel = 1 ;
	if( dx < 0.01 && dy < 0.01 )
		nWaterLevel = 3;
	else if( dx < 0.05 && dy < 0.05 )
		nWaterLevel = 2;

	if( nWaterLevel == 3 && m_ScaleCurLevel > 5)
		return;
	else if( nWaterLevel == 2 && m_ScaleCurLevel > 7 )
		return;
	BOOL bState = FALSE;
	if(nClipType == 2 )
		bState = TRUE;
//	else
//	{
//		if( pPoly->GetPartNum() > 1 )
//			bState = TRUE;
//		else
//			bState = FALSE;
//	}
	
	if( bState )
	{
		DrawPoly( pPoly->GetPtList(), pPoly->GetPtNum(), 
						pPoly->GetPart(), pPoly->GetPartNum() );
		if(bAnno && pPoly->GetAnnotation()!=NULL )
		{
			swString str;
			CharToUnicode( pPoly->GetAnnotation() , &str );
			AddAnnotate( &pPoly->GetCenterPt(), 1, str );
		}
	}
	else //相交或者feature包含显示区域
	{
//		if( pPoly->GetPtNum() <= 5 )
//			ASSERT(FALSE);
		if( pPoly->GetPartNum() == 1 )
		{
			int nNum;
			ST_GEO_RECT rtTemp;
			double dbClip = 10 / dNewScale;
			rtTemp.dBottom = rtDraw.dBottom + dbClip;
			rtTemp.dTop = rtDraw.dTop - dbClip;
			rtTemp.dLeft = rtDraw.dLeft - dbClip;
			rtTemp.dRight = rtDraw.dRight + dbClip;

			ST_GEO_PTXY* pPtList = GetClipRgn( pPoly->GetPtList(), pPoly->GetPtNum(), 
							rtTemp, &nNum, pPoly->GetObjType() );
			if(nNum > 0 && pPtList )
			{
	//			if( nNum <= 3 )
	//				ASSERT(FALSE);
				DrawPoly(pPtList, nNum, &nNum,1);
				if(bAnno && pPoly->GetAnnotation()!=NULL )
				{
	//				CGIS_FeaturePoly poly(EN_GOTYPE_POLY);
	//				poly.SetPtNum(nNum);
	//				poly.SetPtList(pPtList);
	//				poly.SetAnnotation(pPoly->GetAnnotation());
	//				::CalcRect(pPtList,nNum,poly.GetRectObj());
	//				CalcCenter(&poly);
					swString str;
					CharToUnicode( pPoly->GetAnnotation() , &str );
					AddAnnotate( &pPoly->GetCenterPt(), 1, str );
				}
//				delete [] pPtList;
			}
		}
		else
		{
			ST_GEO_RECT rtTemp;
			ST_GEO_PTXY *ptTemp;
			int nPtStart = 0;
			for( int i = 0 ; i < pPoly->GetPartNum() ; i ++ )
			{
				if( i>0 )
					nPtStart += pPoly->GetPart()[i-1];
				ptTemp = &(pPoly->GetPtList()[nPtStart]);
				::CalcRect(ptTemp,pPoly->GetPart()[i],rtTemp);
				int nClipMul = GeoRectIntersect( &rtDraw, &rtTemp );
				if( nClipMul == 2 )
				{
					DrawPoly(ptTemp, pPoly->GetPart()[i], NULL,1);
				}
				else if( nClipMul != 0 )
				{
					int nNum;

					// 为了不和前面的同名变量发生混淆，将变量名更改
					// 原来：ST_GEO_RECT rtTemp 改为：rtNewTemp
					ST_GEO_RECT rtNewTemp;
					double dbClip = 10 / dNewScale;
					rtNewTemp.dBottom = rtDraw.dBottom + dbClip;
					rtNewTemp.dTop = rtDraw.dTop - dbClip;
					rtNewTemp.dLeft = rtDraw.dLeft - dbClip;
					rtNewTemp.dRight = rtDraw.dRight + dbClip;
					ST_GEO_PTXY* pPtList = GetClipRgn( ptTemp, pPoly->GetPart()[i], 
									rtNewTemp, &nNum, pPoly->GetObjType() );
					if( pPtList )
					{
						DrawPoly(pPtList, nNum, NULL,1);
//						delete [] pPtList;
					}
				}
					
			}
		}
	}
}
void CGeoView::DrawPoly( CGIS_FeaturePoly *pPoly, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno )
{
	int nClipType = GeoRectIntersect( &rtDraw, &pPoly->GetRectObj() );
	if(nClipType == 0)
		return;
	if(m_bMoveMap && GeoRectIntersect( &rtOld,&pPoly->GetRectObj() ) ==2  && m_Angle == 0)
		return;
	BOOL bState = FALSE;
	if(nClipType == 2 )
		bState = TRUE;
//	else
//	{
//		if( pPoly->GetPartNum() > 1 )
//			bState = TRUE;
//		else
//			bState = FALSE;
//	}
	
	if( bState )
	{
		DrawPoly( pPoly->GetPtList(), pPoly->GetPtNum(), 
						pPoly->GetPart(), pPoly->GetPartNum() );
		if(bAnno && pPoly->GetAnnotation()!=NULL && pPoly->GetPartNum() > 1 )
		{
			swString str;
			CharToUnicode( pPoly->GetAnnotation() , &str );
			AddAnnotate( &pPoly->GetCenterPt(), 1, str );
		}
	}
	else
	{
//		if( pPoly->GetPtNum() <= 5 )
//			ASSERT(FALSE);
		if( pPoly->GetPartNum() == 1 )
		{
			int nNum;
			ST_GEO_RECT rtTemp;
			double dbClip = 10 / dNewScale;
			rtTemp.dBottom = rtDraw.dBottom + dbClip;
			rtTemp.dTop = rtDraw.dTop - dbClip;
			rtTemp.dLeft = rtDraw.dLeft - dbClip;
			rtTemp.dRight = rtDraw.dRight + dbClip;

			ST_GEO_PTXY* pPtList = GetClipRgn( pPoly->GetPtList(), pPoly->GetPtNum(), 
							rtTemp, &nNum, pPoly->GetObjType() );
			if(nNum > 0 && pPtList )
			{
	//			if( nNum <= 3 )
	//				ASSERT(FALSE);
				DrawPoly(pPtList, nNum, &nNum,1);
				if(bAnno && pPoly->GetAnnotation()!=NULL )
				{
	//				CGIS_FeaturePoly poly(EN_GOTYPE_POLY);
	//				poly.SetPtNum(nNum);
	//				poly.SetPtList(pPtList);
	//				poly.SetAnnotation(pPoly->GetAnnotation());
	//				::CalcRect(pPtList,nNum,poly.GetRectObj());
	//				CalcCenter(&poly);
					swString str;
					CharToUnicode( pPoly->GetAnnotation() , &str );
					AddAnnotate( &pPoly->GetCenterPt(), 1, str );
				}
//				delete [] pPtList;
			}
		}
		else
		{
			ST_GEO_RECT rtTemp;
			ST_GEO_PTXY *ptTemp;
			int nPtStart = 0;
			for( int i = 0 ; i < pPoly->GetPartNum() ; i ++ )
			{
				if( i>0 )
					nPtStart += pPoly->GetPart()[i-1];
				ptTemp = &(pPoly->GetPtList()[nPtStart]);
				::CalcRect(ptTemp,pPoly->GetPart()[i],rtTemp);
				int nClipMul = GeoRectIntersect( &rtDraw, &rtTemp );
				if( nClipMul == 2 )
				{
					DrawPoly(ptTemp, pPoly->GetPart()[i], NULL,1);
				}
				else if( nClipMul != 0 )
				{
					int nNum;

					// 为了不和前面的同名变量发生混淆，将变量名更改
					// 原来：ST_GEO_RECT rtTemp 改为：rtNewTemp
					ST_GEO_RECT rtNewTemp;
					double dbClip = 10 / dNewScale;
					rtNewTemp.dBottom = rtDraw.dBottom + dbClip;
					rtNewTemp.dTop = rtDraw.dTop - dbClip;
					rtNewTemp.dLeft = rtDraw.dLeft - dbClip;
					rtNewTemp.dRight = rtDraw.dRight + dbClip;
					ST_GEO_PTXY* pPtList = GetClipRgn( ptTemp, pPoly->GetPart()[i], 
									rtNewTemp, &nNum, pPoly->GetObjType() );
					if( pPtList )
					{
						DrawPoly(pPtList, nNum, NULL,1);
//						delete [] pPtList;
					}
				}
					
			}
		}
	}
}
void CGeoView::DrawRut( CGIS_FeatureRut *pRut, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno ,ST_GEO_PEN pen)
{
	
	int nClipType = GeoRectIntersect(&rtDraw, &pRut->GetRectObj());
	if(nClipType == 0)	// 没有交集
		return;
	if( m_bMoveMap && GeoRectIntersect( &rtOld,&pRut->GetRectObj() ) == 2  && m_Angle == 0)
		return;
	

	int nNum = 0;
	ST_GEO_PTXY* pPtList = NULL;
	if( bAnno && pRut->GetAnnotation()!=NULL )
	{
		swString str;
		CharToUnicode( pRut->GetAnnotation() , &str );
		int nStrNum = str.GetLength();
		swRect rect;
		int nDX = abs( (pRut->GetPtList()[0].dx - pRut->GetPtList()[pRut->GetPtNum() - 1].dx) * dNewScale );
		int nDY = abs( (pRut->GetPtList()[0].dy - pRut->GetPtList()[pRut->GetPtNum() - 1].dy) * dNewScale );
		int nLength = nDY + nDX;
		if( nLength > (m_nH * nStrNum)  )
		{
			ST_GEO_PTXY *pPt = new ST_GEO_PTXY[nStrNum];
			BOOL bAdd = GetPointsOnLine(pRut->GetPtList(), pRut->GetPtNum(), nStrNum, pPt);
			if( bAdd )
			{
				AddAnnotate(pPt, nStrNum, str);
			}
			delete [] pPt;
		}
	}
	if(nClipType == 2)
	{
		DrawLine( pRut->GetPtList(), pRut->GetPtNum() );
	}
	else
	{
		ST_GEO_RECT rtTemp;
			double dbClip = 10 / dNewScale;
			rtTemp.dBottom = rtDraw.dBottom + dbClip;
			rtTemp.dTop = rtDraw.dTop - dbClip;
			rtTemp.dLeft = rtDraw.dLeft - dbClip;
			rtTemp.dRight = rtDraw.dRight + dbClip;


		pPtList = GetClipRgn(pRut->GetPtList(), pRut->GetPtNum(), 
										  rtTemp, &nNum, EN_GOTYPE_RUT);
		if(nNum > 0 && pPtList )
		{
			DrawLine(pPtList, nNum);
//			delete[] pPtList;
		}
	}
	
}

void CGeoView::ReLoadMapSet( CPtrList *pMapList, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, double dbSeclude, BOOL &bMapSeclude, CPtrList &DrawMapSet )
{
	//是否需要清空缓充图幅记录
	if( m_bClearBuf ) //每次加载图块数据之后将被缓冲，以便加速下次加载的速度
	{
		m_nPreMBLNum = 0 ;				
		m_nPreBNum = 0 ;	
		ClearBufData( FALSE );
	}
	BOOL bLFlag = m_bLFlag;
	/*当前定义450为DETAIL和NATION地图的分解比例尺，也就是每个屏幕像素表示450米单位
	*/
	BOOL bSeclude = dbSeclude>dbScale? TRUE:FALSE; //这里的scale表示1度用多少个像素表示,越小表示地图比例越大，地图被缩放的越小
	/////////////////根据条件更新状态

	//上次即没有卸载，也没有加载完成的更新
	if( m_bDFlag == FALSE )
	{
//		ResetEvent( m_hLEvent );
		int i;
		for( i=0;	i<m_nPreMBLNum;	i++ )
			m_pPreLList[i]->DecreaseLCount( m_pPreBIDs[i] );
		m_nPreMBLNum = 0;
		for( i=0;	i<m_nCurMBLNum;	i++ )
		{
			m_pPreLList[i] = m_pCurLList[i];
			m_pPreBIDs[i] = m_pCurBIDs[i];
		}
		m_nPreMBLNum = m_nCurMBLNum;
		m_nCurMBLNum = 0;
		m_bDFlag = TRUE;
//		SetEvent( m_hLEvent );
	}
	///上次卸载了，但是没有加载完全
	else if( bLFlag == FALSE )
	{
//		ResetEvent( m_hLEvent );
		for( int i=0;	i<m_nCurMBLNum;	i++ )
		{
			m_pPreLList[i] = m_pCurLList[i];
			m_pPreBIDs[i] = m_pCurBIDs[i];
		}
		m_nPreMBLNum = m_nCurMBLNum;
		m_nCurMBLNum = 0;
//		SetEvent( m_hLEvent );
	}
	m_bDFlag = FALSE;
	///判断是否需要卸载先前加载的数据
	if( bSeclude != bMapSeclude ) //这次与前一次不一致
//	if( (bSeclude != bMapSeclude)	|| (GeoRectIntersect(&rtOld,&rtDraw)==0) )
	{
//		ResetEvent( m_hLEvent );
		for( int i=0;	i<m_nPreMBLNum;	i++ )
			m_pPreLList[i]->DecreaseLCount( m_pPreBIDs[i] );
		m_nCurMBLNum = m_nPreMBLNum = 0;
		m_bDFlag = TRUE;
		m_nPreBNum = 0;
//		SetEvent( m_hLEvent );
	}
	bMapSeclude = bSeclude;
	m_bLFlag = FALSE;
	IGIS_Map *pIMap = NULL;
	CGIS_GeoMap *pMap = NULL;
	POSITION pos = pMapList->GetHeadPosition();
	ST_GEO_RECT rtXNew;
	ST_GEO_RECT rtYNew;
	rtDraw.dLeft -= 0.001;
	rtDraw.dRight += 0.001;
	rtDraw.dBottom += 0.001;
	rtDraw.dTop -= 0.001;
	if(  m_bMoveMap )
	{//为了优化加载速度，当拖动地图时，有
	//一部分矩形将无效，比如往右上角方向拖动，将产生一个左下角的新的矩形区域
		if( m_positionBMP.x > 0 ) 
		{
			rtXNew.dLeft = rtDraw.dLeft;
			rtXNew.dRight = rtOld.dLeft;
		}
		else
		{
			rtXNew.dLeft = rtOld.dRight;
			rtXNew.dRight = rtDraw.dRight;
		}
		if( m_positionBMP.y > 0 )
		{
			rtYNew.dTop = rtOld.dBottom;
			rtYNew.dBottom =rtDraw.dBottom ;
		}
		else
		{
			rtYNew.dTop = rtDraw.dTop;
			rtYNew.dBottom =rtOld.dTop ;
		}
		rtXNew.dTop = rtDraw.dTop;
		rtXNew.dBottom = rtDraw.dBottom;
		rtYNew.dLeft = rtDraw.dLeft;
		rtYNew.dRight = rtDraw.dRight;//这里 又撤销了之前的相差区域的检测
	}
	int nCurBMIDs[500];
	memset( nCurBMIDs, 0, sizeof(int)*500);
	int nCurBNum = 0;
	while( pos )
	{//遍历每一张地图,一共才两张地图 nation 和detail/city
		pIMap = (IGIS_Map*)pMapList->GetNext(pos);
		//在两种地图显示模式下，一种是国家行政区域图，一种是省市级地图
		//过滤图层
		if( pIMap == NULL )
			continue;
		if( pIMap->GetMapInfo()->GetMapID() == MAPID_UM_NAVI )//UM_NAVI 用户地图，目前并未使用
			continue;
		if( bSeclude )//如果到了地图分界的时候必须判别当前是否是国家级别地图
		{
			if( pIMap->GetMapInfo()->GetMapID() != MAPID_MAP_NATION ) //国家级别地图
				continue;
		}
		else //到这里必须是详细地图了
		{
			if( pIMap->GetMapInfo()->GetMapID() == MAPID_MAP_NATION )
				continue;
		}
		if( GeoRectIntersect(&pIMap->GetMapInfo()->GetMapRect(), &rtDraw) == 0 )//地图与当前可视地理范围是否相交
			continue;
		pMap = (CGIS_GeoMap*)pIMap;
		CGIS_MapLayer *pBLList[LAYER_PT_END+1];
		int nCount = 0;
		//查找需要显示的图层
		GetMSLayer( pMap, dbScale, (CGIS_MapLayer**)(pBLList), nCount );//找到可见的图层
		//需要重绘的图幅
	//	CPtrList ListBMap;
	std::list<CGIS_BreadthMap*>  ListBMap;
        unsigned int pErrorCode = 0 ; //加载错误返回
        int nReLoadCount = 0;          //重加载次数
		int nReloadIn = -1;			//某图幅LoadBLData内部已经加载过了
		//需绘制的RICHPOI的集合
		CPtrList* pListRichPOI = NULL;
		if( pMap->GetMapInfo()->GetMapType() == EN_MAPTYPE_NATION ) //全国一级地图
		{
			for( int i=0;	i<pMap->GetBreadthMapNum();	i++ ) //将所有网格进行区域相交检测
			{
//				CGIS_BreadthMap *pBMap = pMap->GetBreadthMapList()[i];
				//获取地图的网格块
				CGIS_BreadthMap *pBMap = pMap->GetBreadthMapByID2( i );
				if( pBMap )
				{ //网格区块的地理矩形区域
					ST_GEO_RECT rt = pMap->GetBreadthMapRect(pBMap);
					if( GeoRectIntersect(&rtDraw,&rt) != 0 )
//					if( GeoRectIntersect(&rtDraw,&pBMap->m_rtBreadth) != 0 )
					{
                        pErrorCode = 0;
						if( nReloadIn != i )
						{ //网格区块地图与当前显示区域相交
							if( !pMap->LoadBLData( pBMap, pBLList, nCount , &pErrorCode ) )
 								return;
						}
                        if( pErrorCode == 2 && nReLoadCount < 3 )
                        {
							nReloadIn = i;
                            i = -1;
                            nReLoadCount++ ;
							m_nCurMBLNum = 0;
							nCurBNum = 0;
                            continue;
                        }
						//将每一个网格块上出现的图层加入链表,最多1024个网格块图层
						for( int n=0;	n<nCount;	n++ )
						{
//							pBLList[n]->IncreaseLCount( pBMap->m_nBreadthID );
							m_pCurLList[m_nCurMBLNum] = pBLList[n];
							m_pCurBIDs[m_nCurMBLNum] = pBMap->m_nBreadthID;
							m_nCurMBLNum++;
						}
						nCurBMIDs[nCurBNum] = pBMap->m_nBreadthID;
						nCurBNum++;
						if( m_bMoveMap && m_Angle == 0)
						{
							if( GeoRectIntersect(&rtXNew,&rt) == 0 
								 && GeoRectIntersect(&rtYNew,&rt) == 0)
//							if( GeoRectIntersect(&rtXNew,&pBMap->m_rtBreadth) == 0 
//								 && GeoRectIntersect(&rtYNew,&pBMap->m_rtBreadth) == 0)
								continue;
						}
						//ListBMap.AddTail( pBMap ); //网格地图块加入链表
						ListBMap.push_back(pBMap);
					}
				}
			}
		}
		else //省份和城市地图
		{
			int nBMIDs[500];
			memset( nBMIDs, 0, sizeof(int)*500);
			int nBNum = 0;
			CalculateBMapIDInRect(rtDraw, nBMIDs, nBNum ); //查询指定地图区域内数据区块的编号列表
            ASSERT( nBNum < 500 );
			//加载RICHPOI数据
			pListRichPOI = m_RichPOIView.GetRichPOISet( nBMIDs , nBNum );
			for( int i=0;	i<nBNum;	i++ )
			{
                pErrorCode = 0;
				CGIS_BreadthMap *pBMap = pMap->GetBreadthMapByID( nBMIDs[i] );
				if( pBMap )
				{
					ST_GEO_RECT rt = CalculateBMapRect( nBMIDs[i] );
//					DWORD dwTime = GetTickCount();
					if( nReloadIn != i )
					{ //一个网格块由多个图层数据构成
						if( !pMap->LoadBLData( pBMap, pBLList, nCount , &pErrorCode ) )
							return;
					}
		                    if( pErrorCode == 2 && nReLoadCount < 3 )
		                    {
								nReloadIn = i;
		                        i = -1;
		                        nReLoadCount++ ;
								m_nCurMBLNum = 0;
								nCurBNum = 0;
		                        continue;
		                    }
//					TRACE( L"加载时间：%d \n" , GetTickCount() - dwTime );
					//将所有在区块内的图层放入线性数组中
					for( int n=0;	n<nCount;	n++ )
					{
//						pBLList[n]->IncreaseLCount( pBMap->m_nBreadthID );
						m_pCurLList[m_nCurMBLNum] = pBLList[n];
						m_pCurBIDs[m_nCurMBLNum] = pBMap->m_nBreadthID;
						m_nCurMBLNum++;
					}
					nCurBMIDs[nCurBNum] = pBMap->m_nBreadthID;
					nCurBNum ++;
					if( m_bMoveMap && m_Angle == 0) //地图移动中，但没有旋转角度
					{
						if( GeoRectIntersect(&rtXNew,&rt) == 0 
							 && GeoRectIntersect(&rtYNew,&rt) == 0)
							continue;
					}					
					ListBMap.push_back(pBMap);
				}
			}

		}
		//显示区域计算地图区域，筛选网格块，从网格块中找出图层列表
		//开始处理图层
		//分面、线、路、点顺序
		//CPtrList pPolyList,pLineList,pRutList,pPtList;
		std::list<CGIS_MapLayer*> pPolyList,pLineList,pRutList,pPtList;
		CGIS_MapLayer *pLayer = NULL;
		for( int nNum = 0 ; nNum < nCount ; nNum++ )
		{
			pLayer = pBLList[nNum];//处理每个图层
			if( !pLayer )
				continue;
			int nLayerID = pLayer->GetLayerInfo()->GetLayerID();
			if( pMap->GetMapInfo()->GetMapType() == EN_MAPTYPE_NATION ){ //在国家一级地图中，0是线性，1为多边形，其他都为点
				if( nLayerID == 0 ) //在国家地图中,点，线，面都分被存储在不同的图层
					pLineList.push_back(pLayer);
				else if( nLayerID == 1 )					
					pPolyList.push_back(pLayer);
				else
					pPtList.push_back(pLayer);
			}
			else //CITY layers
			{
				if( nLayerID < 4 )  //<4  地表					
					pPolyList.push_back(pLayer);
				else if( nLayerID < 5 ) // == 4   //道路,好像是铁路的定义				
					pLineList.push_back(pLayer);
				else if( nLayerID < 12 )	
					pRutList.push_back(pLayer);
				else
					pPtList.push_back(pLayer);
			}
		}
		//画面
		std::list<CGIS_BreadthMap*>::iterator itrB;
		//POSITION posMap = ListBMap.GetHeadPosition();
		CGIS_BreadthMap *pBMap = NULL;
		CDC *pDC = NULL;
		//当前区域计算出的地图网格块绘制
		//while( posMap ) //迭代地图网格
		//{
		for(itrB=ListBMap.begin();itrB!=ListBMap.end();itrB++){
			pBMap = *itrB;
			std::list<CGIS_MapLayer*>::iterator itr;
			for(itr=pPolyList.begin();itr!=pPolyList.end();itr++){
				DrawPolyLayer(rtOld, rtDraw, dbScale,pBMap->m_nBreadthID,*itr);
			}
			if(m_bMoveMap)
			{
				pCurrentDC->BitBlt(m_positionBMP.x,m_positionBMP.y,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,
									m_pdcMoveTemp, 0, 0, SRCCOPY);
			}
			if( m_bDrawByLayer &&  (m_OldScaleLevel != GetCurrentScaleLevel()  || ( !m_bIsVehMiddle && m_bMoveMap ) ) ){
#ifdef _WIN32_WCE
                //加这个宏是因为发现此段代码在WIN32 pc机上运行很慢,原因待查.下同.
				pDC = GetDC();
				m_pApartBackDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
						pCurrentDC, 0,0, SRCCOPY );
				DrawMapCompass( m_pApartBackDC );
				pDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
						m_pApartBackDC, 0,0, SRCCOPY );
				ReleaseDC( pDC );
#endif
			}
			//画线
			for(itr=pLineList.begin();itr!=pLineList.end();itr++){				
				DrawLineLayer(rtOld, rtDraw, dbScale,pBMap->m_nBreadthID,*itr);
			}	
			if(m_bMoveMap)
			{
				pCurrentDC->BitBlt(m_positionBMP.x,m_positionBMP.y,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,
									m_pdcMoveTemp, 0, 0, SRCCOPY);
			}
			if( m_bDrawByLayer &&  (m_OldScaleLevel != GetCurrentScaleLevel()  || ( !m_bIsVehMiddle && m_bMoveMap ) ) )
			{
#ifdef _WIN32_WCE
				pDC = GetDC();
				m_pApartBackDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
						pCurrentDC, 0,0, SRCCOPY );
				DrawMapCompass( m_pApartBackDC );
				pDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
						m_pApartBackDC, 0,0, SRCCOPY );
				ReleaseDC( pDC );
#endif
			}

			//画道路
			for(itr=pRutList.begin();itr!=pRutList.end();itr++){
				DrawRutLayer(rtOld, rtDraw, dbScale,pBMap->m_nBreadthID,*itr);
			}
			if(m_bMoveMap)
			{
				pCurrentDC->BitBlt(m_positionBMP.x,m_positionBMP.y,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,
									m_pdcMoveTemp, 0, 0, SRCCOPY);
			}
			if( m_bDrawByLayer &&  (m_OldScaleLevel != GetCurrentScaleLevel()  || ( !m_bIsVehMiddle && m_bMoveMap ) ) )
			{
#ifdef _WIN32_WCE
				pDC = GetDC();
				m_pApartBackDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
						pCurrentDC, 0,0, SRCCOPY );
				DrawMapCompass( m_pApartBackDC );
				pDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
						m_pApartBackDC, 0,0, SRCCOPY );
				ReleaseDC( pDC );
#endif
			}
			for(itr=pPtList.begin();itr!=pPtList.end();itr++){
				DrawPointLayer(rtOld, rtDraw, dbScale,pBMap->m_nBreadthID,*itr);
			}
			if(m_bMoveMap)
			{
				pCurrentDC->BitBlt(m_positionBMP.x,m_positionBMP.y,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,
									m_pdcMoveTemp, 0, 0, SRCCOPY);
			}
			if( m_bDrawByLayer &&  (m_OldScaleLevel != GetCurrentScaleLevel()  || ( !m_bIsVehMiddle && m_bMoveMap ) ) )
			{
#ifdef _WIN32_WCE
				pDC = GetDC();
				m_pApartBackDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
						pCurrentDC, 0,0, SRCCOPY );
				DrawMapCompass( m_pApartBackDC );
				pDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
						m_pApartBackDC, 0,0, SRCCOPY );
				ReleaseDC( pDC );
#endif
			}
		}
		//绘制RICHPOI
		DrawRichPOI(pListRichPOI,rtOld, rtDraw);
		if(m_bMoveMap)
		{
			pCurrentDC->BitBlt(m_positionBMP.x,m_positionBMP.y,plotRect.right - plotRect.left, plotRect.bottom - plotRect.top,
								m_pdcMoveTemp, 0, 0, SRCCOPY);
		}
		if( m_bDrawByLayer &&  (m_OldScaleLevel != GetCurrentScaleLevel()  || ( !m_bIsVehMiddle && m_bMoveMap ) ) )
		{
#ifdef _WIN32_WCE
			pDC = GetDC();
			m_pApartBackDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
					pCurrentDC, 0,0, SRCCOPY );
			DrawMapCompass( m_pApartBackDC );
			pDC->BitBlt( 0,0, plotRect.right-plotRect.left, plotRect.bottom-plotRect.top,
					m_pApartBackDC, 0,0, SRCCOPY );
			ReleaseDC( pDC );
#endif
		}
	}
	if( m_bDFlag == FALSE )
	{
		int i = 0;
		//超过四个图幅，则重新加载
		//以下步骤将当前数据缓存起来以备下次重复使用
		if( m_nPreBNum > 4 || nCurBNum > 4 )
		{
			for( i=0;	i<m_nPreMBLNum;	i++ )
				m_pPreLList[i]->DecreaseLCount( m_pPreBIDs[i] );
			for( i=0;	i<m_nCurMBLNum;	i++ )
			{
				m_pPreLList[i] = m_pCurLList[i];
				m_pPreBIDs[i] = m_pCurBIDs[i];
			}
			m_nPreMBLNum = m_nCurMBLNum;

			for( i = 0 ; i < nCurBNum ; i++ )
				m_pPreBIDList[i] = nCurBMIDs[i];
			m_nPreBNum = nCurBNum;
		}
		//剔除重复加载的图层，若两者之和超过四个图幅，则剔除应该被删除的图层
		else
		{
			//需要保存在内存中的图幅集
			int nKeepB[4];
			int nKeepNum = 0;
			nKeepNum += nCurBNum;
			for( i = 0 ; i < nCurBNum ; i++ )
				nKeepB[i] = nCurBMIDs[i];
			for( i = 0 ; i < m_nPreBNum ; i++ )
			{
				if( nKeepNum == 4 )
					break;
				int nPre = m_pPreBIDList[i];
				BOOL bAdd = TRUE;
				for( int j = 0 ; j < nCurBNum ; j++ )
				{
					int nCur = nCurBMIDs[j];
					if( nPre == nCur )
					{
						bAdd = FALSE;
						break;
					}
				}
				if( bAdd )
				{
					nKeepB[nKeepNum] = nPre;
					nKeepNum++;
				}
			}
			//修改历史图幅记录
			for( i = 0 ; i < nKeepNum ; i++ )
				m_pPreBIDList[i] = nKeepB[i];
			m_nPreBNum = nKeepNum;
			//临时历史计数器
			CGIS_MapLayer *TempPreList[MLBNUM_MAX];
			int TempPreIDs[MLBNUM_MAX];
			memset( TempPreIDs, 0, sizeof(int)*MLBNUM_MAX);
			int TempPreNum = 0;
			for( i=0;	i<m_nPreMBLNum;	i++ )
			{
				CGIS_MapLayer* pPreTemp = m_pPreLList[i];
				int nPreTemp = m_pPreBIDs[i];
				BOOL bAdd = TRUE;
				int j;
				for( j = 0 ; j < m_nCurMBLNum ; j++ )
				{
					CGIS_MapLayer* pCurTemp = m_pCurLList[j];
					int nCurTemp = m_pCurBIDs[j];
					if( pPreTemp == pCurTemp && nCurTemp == nPreTemp )
					{
						bAdd = FALSE;
						break;
					}
				}
				if( bAdd )
				{
					for( j = 0 ; j < nKeepNum ; j++ )
					{
						int nKeep = nKeepB[j];
						if( nPreTemp == nKeep )
						{
							bAdd = TRUE;
							break;
						}
						else
							bAdd = FALSE;
					}
				}
				if( bAdd )
				{
					TempPreList[TempPreNum] = pPreTemp;
					TempPreIDs[TempPreNum] = nPreTemp;
					TempPreNum++;
				}
				else
					pPreTemp->DecreaseLCount( nPreTemp );

			}
			for( i=0;	i<m_nCurMBLNum;	i++ )
			{
				TempPreList[i+TempPreNum] = m_pCurLList[i];
				TempPreIDs[i+TempPreNum] = m_pCurBIDs[i];
			}
			TempPreNum += m_nCurMBLNum;
			//确定历史计数器
			
			for( i=0;	i<TempPreNum;	i++ )
			{
				m_pPreLList[i] = TempPreList[i];
				m_pPreBIDs[i] = TempPreIDs[i];
			}
			m_nPreMBLNum = TempPreNum;
		}
		m_bDFlag = TRUE;
	}
	else
	{
		int i;
		for(i=0;	i<m_nCurMBLNum;	i++ )
		{
			m_pPreLList[i] = m_pCurLList[i];
			m_pPreBIDs[i] = m_pCurBIDs[i];
		}
		m_nPreMBLNum = m_nCurMBLNum;
		for( i = 0 ; i < nCurBNum ; i++ )
			m_pPreBIDList[i] = nCurBMIDs[i];
		m_nPreBNum = nCurBNum;
	}
	m_nCurMBLNum = 0;
	m_bLFlag = TRUE;
}

void CGeoView::DrawLonLatLine(ST_GEO_RECT rect,CDC* pDC)
{
	CPen *pOldPen , nPen;
	nPen.CreatePen(PS_DASH,1,RGB(0,0,0));
	pOldPen = (CPen*)(pDC->SelectObject(&nPen));
	pDC->SetBkMode(TRANSPARENT);
	CList <float,float>DrawA(16);
//	CArray<float,float> DrawA;
//	DrawA.SetSize(16);
//	CArray<float,float> DrawLat(16);
	if(dScale >= 60000.0)
	{
		int nx,ny;
		double nLonTemp = (rect.dLeft * 1000+0.5);
		while(nLonTemp <= (rect.dRight * 1000+0.5))
		{
			XyMapToView(nLonTemp/1000.0,rect.dTop,nx,ny);
			pDC->MoveTo(nx,ny);
			XyMapToView(nLonTemp/1000.0,rect.dBottom,nx,ny);
			pDC->LineTo(nx,ny);
			DrawA.AddTail((float)(nLonTemp/1000.0));
			nLonTemp++;
		}
		int lonCount = DrawA.GetCount();
		float latDis = 0;
		if(lonCount > 0)
		{
			latDis = (rect.dBottom - rect.dTop) / lonCount;
		}
		for(int i = 0 ; i < DrawA.GetCount() ; i++)
		{
			XyMapToView(DrawA.GetAt(DrawA.FindIndex(i)),rect.dBottom-i*latDis-latDis/2.0,nx,ny);
			swString strTemp = GetTLonLat(DrawA.GetAt(DrawA.FindIndex(i)),st_level);
			CSize s = pCurrentDC->GetTextExtent(strTemp);
			pDC->DrawText(strTemp,swRect(nx-s.cx/2,ny-s.cy/2,nx+s.cx/2,ny+s.cy/2),DT_WORDBREAK | DT_CENTER| DT_VCENTER |DT_NOCLIP);
		}
		DrawA.RemoveAll();
		double nLatTemp = (rect.dTop * 1000+0.5);
		while(nLatTemp <= (rect.dBottom * 1000+0.5))
		{
			XyMapToView(rect.dLeft,nLatTemp/1000.0,nx,ny);
			pDC->MoveTo(nx,ny);
			XyMapToView(rect.dRight,nLatTemp/1000.0,nx,ny);
			pDC->LineTo(nx,ny);
			DrawA.AddHead((float)(nLatTemp/1000.0));
			nLatTemp++;
		}
		
		int latCount = DrawA.GetCount();
		float lonDis = 0;
		if(latCount > 0)
		{
			lonDis= (rect.dRight - rect.dLeft) / latCount;
		}
		for(int j = 0 ; j < DrawA.GetCount() ; j++)
		{
			XyMapToView(rect.dLeft+j*lonDis+lonDis/2.0,DrawA.GetAt(DrawA.FindIndex(j)),nx,ny);
			swString strTemp = GetTLonLat(DrawA.GetAt(DrawA.FindIndex(j)),st_level);
			CSize s = pCurrentDC->GetTextExtent(strTemp);
			pDC->DrawText(strTemp,swRect(nx-s.cx/2,ny-s.cy,nx+s.cx/2,ny),DT_WORDBREAK | DT_CENTER| DT_VCENTER |DT_NOCLIP);
		}
		DrawA.RemoveAll();
	}
	else
	{
		int nLonD,nLatD;
		nLonD = int((rect.dRight - rect.dLeft)*1000+0.5);
		nLatD = int((rect.dBottom - rect.dTop)*1000+0.5);
		int nLev = 1;
		int nEnd;
		nEnd = nLonD / nLev;
		while(nEnd > 3)
		{
			nLev++;
			nEnd = nLonD / nLev;
		}
		int nx,ny;
		int nLonTemp = (int)(rect.dLeft * 1000+0.5);
		while(nLonTemp <= (int)(rect.dRight * 1000+0.5))
		{
			XyMapToView(nLonTemp/1000.0,rect.dTop,nx,ny);
			pDC->MoveTo(nx,ny);
			XyMapToView(nLonTemp/1000.0,rect.dBottom,nx,ny);
			pDC->LineTo(nx,ny);
			DrawA.AddTail((float)(nLonTemp/1000.0));
			nLonTemp+=nLev;
		}

		int lonCount = DrawA.GetCount();
		float latDis = 0;
		if(lonCount > 0)
		{
			latDis = (rect.dBottom - rect.dTop) / lonCount;
		}
		for(int i = 0 ; i < DrawA.GetCount() ; i++)
		{
			XyMapToView(DrawA.GetAt(DrawA.FindIndex(i)),rect.dBottom-i*latDis-latDis/2.0,nx,ny);
			swString strTemp = GetTLonLat(DrawA.GetAt(DrawA.FindIndex(i)),st_level);
			CSize s = pCurrentDC->GetTextExtent(strTemp);
			pDC->DrawText(strTemp,swRect(nx-s.cx/2,ny-s.cy/2,nx+s.cx/2,ny+s.cy/2),DT_WORDBREAK | DT_CENTER| DT_VCENTER |DT_NOCLIP);
		}
		DrawA.RemoveAll();

		nLev = 1;
		nEnd = nLatD / nLev;
		while(nEnd > 3)
		{
			nLev++;
			nEnd = nLatD / nLev;
		}
		int nLatTemp = (int)(rect.dTop * 1000+0.5);
		while(nLatTemp <= (int)(rect.dBottom * 1000+0.5))
		{
			XyMapToView(rect.dLeft,nLatTemp/1000.0,nx,ny);
			pDC->MoveTo(nx,ny);
			XyMapToView(rect.dRight,nLatTemp/1000.0,nx,ny);
			pDC->LineTo(nx,ny);
			DrawA.AddHead((float)(nLatTemp/1000.0));
			nLatTemp+=nLev;
		}
		int latCount = DrawA.GetCount();
		float lonDis = 0;
		if(latCount > 0)
		{
			lonDis= (rect.dRight - rect.dLeft) / latCount;
		}
		for(int j = 0 ; j < DrawA.GetCount() ; j++)
		{
			XyMapToView(rect.dLeft+j*lonDis+lonDis/2.0,DrawA.GetAt(DrawA.FindIndex(j)),nx,ny);
			swString strTemp = GetTLonLat(DrawA.GetAt(DrawA.FindIndex(j)),st_level);
			CSize s = pCurrentDC->GetTextExtent(strTemp);
			pDC->DrawText(strTemp,swRect(nx-s.cx/2,ny-s.cy,nx+s.cx/2,ny),DT_WORDBREAK | DT_CENTER| DT_VCENTER |DT_NOCLIP);
		}
		DrawA.RemoveAll();
	}


	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();
}
swString CGeoView::GetTLonLat(float latlon,ST_LonLatLinelevel st_Drawlevel)
{
	
	swString strTaget,strTemp;
	strTaget=L"";
	int level;
	if(st_Drawlevel.nDecimal <= 5 && st_Drawlevel.nDecimal >= 0)
	{
		level = (int)pow(10,st_Drawlevel.nDecimal);
	}
	else
	{
		level = (int)pow(10,5);
	}
	if(st_Drawlevel.EnLevel == EN_LEVEL_DEGREE)
	{
		strTaget.Format(L"%d.",(int)latlon);
		strTemp.Format(L"%d°",(int)((latlon - (int)latlon)*level+0.5));
		strTaget+=strTemp;
	}
	if(st_Drawlevel.EnLevel == EN_LEVEL_MINUTE)
	{
		strTaget.Format(L"%d°",(int)latlon);
		double minute = (latlon - (int)latlon)*60+0.5;
		strTemp.Format(L"%d.",(int)minute);
		strTaget+=strTemp;
		strTemp.Format(L"%d′",(int)((minute-(int)minute)*level+0.5));
		strTaget+=strTemp;
	}
	if(st_Drawlevel.EnLevel == EN_LEVEL_SECOND)
	{
		strTaget.Format(L"%d°",(int)latlon);
		double minute = (latlon - (int)latlon)*60+0.5;
		strTemp.Format(L"%d′",(int)minute);
		strTaget+=strTemp;
		double second = (minute - (int)minute)*60+0.5;
		strTemp.Format(L"%d.",(int)second);
		strTaget+=strTemp;
		strTemp.Format(L"%d″",(int)((second-(int)second)*level+0.5));
		strTaget+=strTemp;
	}
	
	return strTaget;
	
}
void CGeoView::SetLayerDrawState(UINT nCount)
{
	if(m_LayerDrawState == NULL)
	{
		m_LayerDrawState = new BOOL[nCount];
		m_LayerCount = nCount;
	}
	else if(nCount > m_LayerCount)
	{
		BOOL* temp = new BOOL[nCount];
		for(unsigned short i=0 ; i<m_LayerCount ; i++)
		{
			temp[i] = m_LayerDrawState[i];
		}
		delete[] m_LayerDrawState;
		m_LayerDrawState = temp;
		m_LayerCount = nCount;
	}
}
BOOL CGeoView::SetDrawState(UINT nLayID,BOOL nState)
{
	if(m_LayerDrawState == NULL || nLayID > m_LayerCount)
	{
		return FALSE;
	}
	m_LayerDrawState[nLayID-1] = nState;
	return TRUE;

}
BOOL CGeoView::GetDrawState(UINT nLayID)
{
	if(m_LayerDrawState == NULL || nLayID > m_LayerCount)
	{
		return FALSE;
	}
	return m_LayerDrawState[nLayID-1];
}
BOOL CGeoView::SetLonLatLevel(ST_LonLatLinelevel st_Drawlevel)
{
	st_level = st_Drawlevel;
	return TRUE;
}
ST_LonLatLinelevel CGeoView::GetLonLatLevel()
{
	return st_level;
}
void CGeoView::SetAngle(UINT nAng)
{
	m_Angle = nAng % 360;
	m_sinRotate = sin(m_Angle*PI/180.0);
	m_cosRotate = cos(m_Angle*PI/180.0);
}
ST_GEO_RECT CGeoView::MaxNewRect(RECT rect)
{
	ST_GEO_RECT DrawRect;
	double dx1,dy1,dx2,dy2,dx3,dy3,dx4,dy4;
	XyViewToMap(rect.left,rect.top,dx1,dy1);
	XyViewToMap(rect.right,rect.bottom,dx2,dy2);
	XyViewToMap(rect.right,rect.top,dx3,dy3);
	XyViewToMap(rect.left,rect.bottom,dx4,dy4);
	DrawRect.dLeft = (float)min(min(dx1,dx2),min(dx3,dx4));
	DrawRect.dRight = (float)max(max(dx1,dx2),max(dx3,dx4));
	DrawRect.dTop = (float)min(min(dy1,dy2),min(dy3,dy4));
	DrawRect.dBottom = (float)max(max(dy1,dy2),max(dy3,dy4));
	return DrawRect;
}
ST_GEO_RECT CGeoView::GetRotateRect()
{
	return RotateRect;
}
double CGeoView::TurnScale(UINT scalelevel)
{
	double tempSource;
	if(scalelevel <= m_ScaleCount)
	{
		m_ScaleCurLevel = scalelevel;
	}
	tempSource = m_dbScaleValue[m_ScaleCurLevel-1]; //定义了1个像素表示多少米 
	return (1852*60/tempSource);//一度用几个像素来表示

}
UINT CGeoView::GetAngle()
{
	return m_Angle;
}
void CGeoView::SetScale(double* scalev,UINT scalec)
{
	if(scalev != NULL)
	{
		if(m_dbScaleValue != NULL)
		{
			delete[] m_dbScaleValue;
			m_dbScaleValue = NULL;
		}
		m_dbScaleValue = new double[scalec];
		for(UINT i = 0 ; i < scalec ; i++)
		{
			m_dbScaleValue[i] = scalev[i];
		}
		// 去掉下面一行，由于上面刚分配内存，如果再指向其它地方，导致内存泄漏
//		m_dbScaleValue = scalev;
		m_ScaleCount = scalec;
	}
}
void CGeoView::MatchRoad( ST_GEO_PTXY& pt , int nSpeed , int nTime , EnMathState enState )
{
    ASSERT(FALSE);
    
	if( enState == EN_MATCH_NORMAL )
	{
		POSITION pos = m_MapSet->GetHeadPosition();
		CGIS_GeoMap *pMap = NULL;
		while(pos)
		{
			pMap = (CGIS_GeoMap*)m_MapSet->GetNext(pos);
			unsigned short nMapID = pMap->GetMapInfo()->GetMapID();
			if( nMapID == MAPID_MAP_NATION || nMapID == MAPID_UM_NAVI )
				continue;
			if( IsPtInRect(pt.dx,pt.dy,pMap->GetMapInfo()->GetMapRect().dLeft,pMap->GetMapInfo()->GetMapRect().dTop,
					pMap->GetMapInfo()->GetMapRect().dRight,pMap->GetMapInfo()->GetMapRect().dBottom) )
				break;

		}
		if( pMap && pMap->GetCarDirect() != 0 && m_LastRoat )
		{
			int nRunLen = nSpeed * nTime / 1000 ;
			ST_GEO_PTXY ptNextNode;
			
			for( int i = 0 ; i < m_LastRoat->GetPtNum()-1 ; i++ )
			{
				BOOL bOnLine = PointOnLine( pt,m_LastRoat->GetPtList()[i],m_LastRoat->GetPtList()[i+1]);
				if( bOnLine )
				{
					//是否已经到达节点
					BOOL bGetNode = FALSE;
					if( pMap->GetCarDirect() == 1 )
						ptNextNode = m_LastRoat->GetPtList()[i+1];
					else
						ptNextNode = m_LastRoat->GetPtList()[i];
					double dbRoadLen = CalcDist( pt.dx,pt.dy,ptNextNode.dx,ptNextNode.dy);
					int nCount = i;
					ST_GEO_PTXY ptStart = pt;
					ST_GEO_PTXY ptEnd = ptNextNode;
					while( nRunLen > dbRoadLen )
					{
						nRunLen -= dbRoadLen;
						if( pMap->GetCarDirect() == 1 )
						{
							if( nCount == m_LastRoat->GetPtNum()-2 )
							{
								bGetNode = TRUE;
								pt = m_LastRoat->GetPtList()[m_LastRoat->GetPtNum()-1];
								break;
							}
							dbRoadLen = CalcDist( m_LastRoat->GetPtList()[nCount+1].dx,m_LastRoat->GetPtList()[nCount+1].dy,
								                   m_LastRoat->GetPtList()[nCount+2].dx,m_LastRoat->GetPtList()[nCount+2].dy);
							ptStart = m_LastRoat->GetPtList()[nCount+1];
							ptEnd = m_LastRoat->GetPtList()[nCount+2];
							nCount++;
						}
						else
						{
							if( nCount == 0 )
							{
								bGetNode = TRUE;
								pt = m_LastRoat->GetPtList()[0];
								break;
							}
							dbRoadLen = CalcDist( m_LastRoat->GetPtList()[nCount].dx,m_LastRoat->GetPtList()[nCount].dy,
								                   m_LastRoat->GetPtList()[nCount-1].dx,m_LastRoat->GetPtList()[nCount-1].dy);
							ptStart = m_LastRoat->GetPtList()[nCount];
							ptEnd = m_LastRoat->GetPtList()[nCount-1];

							nCount--;
						}
					}
					if( !bGetNode && dbRoadLen != 0)
					{
						double dx,dy;
						GetPointArc( ptStart.dx, ptStart.dy, ptEnd.dx,ptEnd.dy,
						nRunLen / dbRoadLen , dx,dy );
						pt.dx = dx;
						pt.dy = dy;
					}
					break;
				}
			}
		}
	}
	else if( enState == EN_MATCH_REALNAV && m_pNetworkRoute && m_PathNodePos )
	{
		int nRunLen = nSpeed * nTime / 1000;
		CPtrList *pPathPointList = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();
		CGIS_Pathpoint *pPathPoint1 = NULL;
		CGIS_Pathpoint *pPathPoint2 = NULL;
		POSITION pos = m_PathNodePos;
		while(pos)
		{
			pPathPoint1 = (CGIS_Pathpoint*)pPathPointList->GetNext(pos);
			if(!pos)
				break;
			pPathPoint2 = (CGIS_Pathpoint*)pPathPointList->GetAt(pos);
			EnPathpointType enType = pPathPoint2->GetPathpointType();
			BOOL bOnLine = PointOnLine(pt,pPathPoint1->GetPathpointPosition(),pPathPoint2->GetPathpointPosition());
			if(bOnLine)
			{
				double dbRoadLen = CalcDist( pt.dx,pt.dy,
									pPathPoint2->GetPathpointPosition().dx , pPathPoint2->GetPathpointPosition().dy);
				BOOL bGetNode = FALSE;
				ST_GEO_PTXY ptStart = pt;
				ST_GEO_PTXY ptEnd = pPathPoint2->GetPathpointPosition();
				while( nRunLen > dbRoadLen )
				{
					nRunLen -= dbRoadLen;
					POSITION posMem = pos;
					CGIS_Pathpoint* pPathPtTemp = (CGIS_Pathpoint*)pPathPointList->GetNext( posMem );
					if( !posMem )
					{
						bGetNode = TRUE;
						pt = pPathPtTemp->GetPathpointPosition();
						break;
					}
					CGIS_Pathpoint* pPathPtTemp1 = (CGIS_Pathpoint*)pPathPointList->GetAt( posMem );
					dbRoadLen = CalcDist( pPathPtTemp->GetPathpointPosition().dx,pPathPtTemp->GetPathpointPosition().dy,
								           pPathPtTemp1->GetPathpointPosition().dx,pPathPtTemp1->GetPathpointPosition().dy);
					ptStart = pPathPtTemp->GetPathpointPosition();
					ptEnd = pPathPtTemp1->GetPathpointPosition();
				}
				if( !bGetNode && dbRoadLen != 0 )
				{
					double dx,dy;
					GetPointArc( ptStart.dx, ptStart.dy, ptEnd.dx,ptEnd.dy,
					nRunLen / dbRoadLen , dx,dy );
					pt.dx = dx;
					pt.dy = dy;
				}
				break;
			}
		}
	}
}
EnMatchRoad CGeoView::MatchRoad(ST_GEO_PTXY pt,ST_GEO_PTXY &Rpt,EnMathState enState)
{

//	Rpt = pt;
//	return EN_MATCH_WELL;
	if(enState == EN_MATCH_SIMULATENAV)
	{
		Rpt = pt;
		return EN_MATCH_WELL;
	}
	CPtrList *pPathPointList = NULL;
	if( enState == EN_MATCH_REALNAV && m_pNetworkRoute != NULL )
	{
		if( m_pNetworkRoute->GetNetworkPath() )
			pPathPointList = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();
	}

	int MatchDis;
	if( m_bHighway )
		MatchDis = MATCH_HIGHWAY_NOMAL;
	else
		MatchDis = MATCH_CITYROAD_NORMAL;
//	double mindis = MatchDis;
	double mindis = 1000000;
	if(enState == EN_MATCH_REALNAV)
	{
		if( pPathPointList != NULL )
		{
			POSITION posTemp;
			ST_GEO_PTXY ptRTemp = {0, 0};
			if(m_PathNodePos == NULL)//寻找匹配的航线段
			{
				BOOL bFirstMatch = ( m_PathNearNodePos == NULL );
				int nFindPtNum = 0;
				int nFindPreNum = 0;
				POSITION pos;
				//如果是第一次匹配，则遍历所有航线点
				if( bFirstMatch )
					pos = pPathPointList->GetHeadPosition();
				//若不是第一次匹配，则从最近节点开始寻找(前20个NODE，后300个PATHPOINT)
				else
				{
					CGIS_Pathpoint *pPpt = NULL;
					pos = m_PathNearNodePos;
					int nNodeNum = 0;
					while( pos )
					{
						nFindPreNum++;
						POSITION posre = pos;
						pPpt = (CGIS_Pathpoint*)pPathPointList->GetPrev(pos);
						if(pPpt->GetPathpointType() == EN_PATHPOINTTYPE_NODE 
							|| pPpt->GetPathpointType() == EN_PATHPOINTTYPE_START 
							|| pPpt->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE)
						{
							nNodeNum ++;
							if( nNodeNum >= 20 )
							{
								pos = posre;
								break;
							}
						}
						if( !pos )
						{
							pos = pPathPointList->GetHeadPosition();
							break;
						}
					}
				}
				posTemp = pos;
				CGIS_Pathpoint *pPathPoint1;
				CGIS_Pathpoint *pPathPoint2;
				while(pos)
				{
					nFindPtNum ++;
					POSITION posre = pos;
					pPathPoint1 = (CGIS_Pathpoint*)pPathPointList->GetNext(pos);
					if(pPathPoint1->GetPathpointType() == EN_PATHPOINTTYPE_NODE 
						|| pPathPoint1->GetPathpointType() == EN_PATHPOINTTYPE_START 
						|| pPathPoint1->GetPathpointType() == EN_PATHPOINTTYPE_INTERMEDIATE)
						posTemp = posre;
					if(!pos)
						break;
					pPathPoint2 = (CGIS_Pathpoint*)pPathPointList->GetAt(pos);
					ST_GEO_PTXY temppt = DisPointToLine(pt,pPathPoint1->GetPathpointPosition(),pPathPoint2->GetPathpointPosition());
					BOOL bOnLine = PointOnLine(temppt,pPathPoint1->GetPathpointPosition(),pPathPoint2->GetPathpointPosition());
					if(bOnLine)
					{
						double dbTemp = CalcDist(pt.dx,pt.dy,temppt.dx,temppt.dy);
						if(dbTemp  < mindis)
						{
							mindis = dbTemp;
							m_PathNodePos = posTemp;
							m_PathPtPos = posre;
							m_PathNearNodePos = posTemp;
							ptRTemp = temppt;
						}
					}
					else
					{
						double dis1 = CalcDist(pt.dx,pt.dy,pPathPoint1->GetPathpointPosition().dx,pPathPoint1->GetPathpointPosition().dy);
						double dis2 = CalcDist(pt.dx,pt.dy,pPathPoint2->GetPathpointPosition().dx,pPathPoint2->GetPathpointPosition().dy);
						if( dis1 < dis2 && dis1 < mindis)
						{
							mindis = dis1; 
							ptRTemp = pPathPoint1->GetPathpointPosition();
							m_PathPtPos = posre;
							m_PathNodePos = posTemp;
							m_PathNearNodePos = posTemp;
						}
						if(dis1 > dis2 && dis2 < mindis)
						{
							mindis = dis2;
							ptRTemp = pPathPoint2->GetPathpointPosition();
							m_PathPtPos = posre;
							m_PathNodePos = posTemp;
							m_PathNearNodePos = posTemp;
						}
					}
					if(mindis <= MatchDis)
					{
						Rpt = ptRTemp;
						m_ptMatch = Rpt;
						//判断最后一次匹配是否在高速上
						CGIS_Pathpoint *pTempPoint = (CGIS_Pathpoint*)pPathPointList->GetAt( m_PathNodePos );
						if( pTempPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
						{
							BYTE nType = ( (CGIS_PathpointNode*)pTempPoint )->GetRType() ;
							if(  nType == 0 )
								m_bHighway = TRUE;
							else
								m_bHighway = FALSE;
						}
						if( m_LastRoat )
						{
							delete m_LastRoat;
							m_LastRoat = NULL;
						}
						m_nLeaveCount = 0 ;
						return EN_MATCH_WELL;
					}
					if( !bFirstMatch && nFindPtNum >= (nFindPreNum+200) )
						break;
				}
				m_PathNodePos = NULL;
				m_PathPtPos = NULL;
			}
			else//从最近道路开始匹配
			{
				//若下面30个节点都无法匹配到，则说明已经偏离航线。
				int nFindNextNodeNum = 0;
				POSITION pos = m_PathNodePos;
				posTemp = m_PathNodePos;
				double Premindis = 100000;
				ST_GEO_PTXY PreptRTemp = {0, 0};

				CGIS_Pathpoint *pPathPoint1 = NULL;
				CGIS_Pathpoint *pPathPoint2 = NULL;
				//是否已经匹配过适当点了
				BOOL bHaveMatch = FALSE;
				//匹配是否应该结束
				BOOL bMatchOver = FALSE;
				//是否在匹配顺序后点
				BOOL bAfterPtPos = FALSE;
				while(pos)
				{
					if( m_bHighway )
						MatchDis = MATCH_HIGHWAY_NOMAL;
					else
						MatchDis = MATCH_CITYROAD_NORMAL;
					POSITION posre = pos;
					if( posre == m_PathPtPos )
						bAfterPtPos = TRUE;
					double APmindis ;
					if( bAfterPtPos )
						APmindis = mindis;
					else
						APmindis = Premindis;
					pPathPoint1 = (CGIS_Pathpoint*)pPathPointList->GetNext(pos);
					if(!pos)
					{
						m_PathNodePos = NULL;
						m_PathPtPos = NULL;
						posTemp = NULL;
						break;
					}
					pPathPoint2 = (CGIS_Pathpoint*)pPathPointList->GetAt(pos);
					EnPathpointType enType1 = pPathPoint2->GetPathpointType();
					EnPathpointType enType2 = pPathPoint1->GetPathpointType();
					ST_GEO_PTXY temppt = DisPointToLine(pt,pPathPoint1->GetPathpointPosition(),pPathPoint2->GetPathpointPosition());
					BOOL bOnLine = PointOnLine(temppt,pPathPoint1->GetPathpointPosition(),pPathPoint2->GetPathpointPosition());
					if(bOnLine)
					{
						double dbDis = CalcDist(pt.dx,pt.dy,temppt.dx,temppt.dy);
						if( dbDis < APmindis )
						{
							//若在节点附近，则跳变阀值变小
							if( enType1 == EN_PATHPOINTTYPE_NODE && enType2 != EN_PATHPOINTTYPE_START ) 
							{
								int nNodeDis = CalcDist( pPathPoint2->GetPathpointPosition().dx , pPathPoint2->GetPathpointPosition().dy , pt.dx , pt.dy );
								if( nNodeDis < MatchDis )
								{
									if( m_bHighway )
										MatchDis = MATCH_HIGHWAY_VTX;
									else
										MatchDis = MATCH_CITYROAD_VTX;
								}
							}
							if( bAfterPtPos )
							{
								mindis = dbDis;
								ptRTemp = temppt;
								bHaveMatch = TRUE;
								m_PathPtPos = posre;
							}
							else
							{
								Premindis = dbDis;
								PreptRTemp = temppt;
							}
						}
						else if( bHaveMatch )
							bMatchOver = TRUE;
					}
					else
					{
						double dis1 = CalcDist(pt.dx,pt.dy,pPathPoint1->GetPathpointPosition().dx,pPathPoint1->GetPathpointPosition().dy);
						double dis2 = CalcDist(pt.dx,pt.dy,pPathPoint2->GetPathpointPosition().dx,pPathPoint2->GetPathpointPosition().dy);
						if( dis1 < dis2 && dis1 < APmindis)
						{
							//若在节点附近，则跳变阀值变小
							if( enType1 == EN_PATHPOINTTYPE_NODE && enType2 != EN_PATHPOINTTYPE_START ) 
							{
								int nNodeDis = CalcDist( pPathPoint2->GetPathpointPosition().dx , pPathPoint2->GetPathpointPosition().dy , pt.dx , pt.dy );
								if( nNodeDis < MatchDis )
								{
									if( m_bHighway )
										MatchDis = MATCH_HIGHWAY_VTX;
									else
										MatchDis = MATCH_CITYROAD_VTX;
								}
							}
							if( bAfterPtPos )
							{
								mindis = dis1; 
								ptRTemp = pPathPoint1->GetPathpointPosition();
								bHaveMatch = TRUE;
								m_PathPtPos = posre;
							}
							else
							{
								Premindis = dis1;
								PreptRTemp = pPathPoint1->GetPathpointPosition();
							}
						}
						else if(dis1 > dis2 && dis2 < APmindis)
						{
							//若在节点附近，则跳变阀值变小
							if( enType1 == EN_PATHPOINTTYPE_NODE && enType2 != EN_PATHPOINTTYPE_START ) 
							{
								int nNodeDis = CalcDist( pPathPoint2->GetPathpointPosition().dx , pPathPoint2->GetPathpointPosition().dy , pt.dx , pt.dy );
								if( nNodeDis < MatchDis )
								{
									if( m_bHighway )
										MatchDis = MATCH_HIGHWAY_VTX;
									else
										MatchDis = MATCH_CITYROAD_VTX;
								}
							}
							if( bAfterPtPos )
							{
								mindis = dis2;
								ptRTemp = pPathPoint2->GetPathpointPosition();
								bHaveMatch = TRUE;
								m_PathPtPos = posre;
							}
							else
							{
								Premindis = dis2;
								PreptRTemp = pPathPoint2->GetPathpointPosition();
							}
						}
						else if( bHaveMatch )
							bMatchOver = TRUE ;
					}
					BOOL bEndOfPart = (enType1 == EN_PATHPOINTTYPE_NODE||enType1 == EN_PATHPOINTTYPE_END||enType1 == EN_PATHPOINTTYPE_INTERMEDIATE);
//					if( ( bMatchOver || enType1 == EN_PATHPOINTTYPE_NODE  ||  enType1 == EN_PATHPOINTTYPE_END || enType1 == EN_PATHPOINTTYPE_INTERMEDIATE ) 
//						&& ( mindis <= MatchDis || Premindis <= MatchDis ) )
					if( (bMatchOver&&mindis<=MatchDis) ||	( bEndOfPart&&Premindis <= MatchDis&&nFindNextNodeNum>0)  
						|| (bEndOfPart&&mindis<=MatchDis) )
					{
						if( mindis <= MatchDis )
							Rpt = ptRTemp;
						else
							Rpt = PreptRTemp;
						m_ptMatch = Rpt;
						//判断最后一次匹配是否在高速上
						CGIS_Pathpoint *pTempPoint = (CGIS_Pathpoint*)pPathPointList->GetAt( m_PathNodePos );
						if( pTempPoint->GetPathpointType() == EN_PATHPOINTTYPE_NODE )
						{
							BYTE nType = ( (CGIS_PathpointNode*)pTempPoint )->GetRType() ;
							if(  nType == 0 )
								m_bHighway = TRUE;
							else
								m_bHighway = FALSE;
						}
//						if( (fabs(Rpt.dx-pPathPoint2->GetPathpointPosition().dx)<=FLOAT_ZERO &&
//							fabs(Rpt.dy-pPathPoint2->GetPathpointPosition().dy)<=FLOAT_ZERO) )
						//下一点是否为路口
						BOOL bNode = (enType1 == EN_PATHPOINTTYPE_NODE);
						//离路口20米左右就算过了该路口,这样就能解决U-TURN的问题,这样也就不会有跳变问题
						if( bNode && mindis<=MatchDis )
						{
							if( CalcDist( Rpt.dx,Rpt.dy,pPathPoint2->GetPathpointPosition().dx,pPathPoint2->GetPathpointPosition().dy) < PASS_VTX )
							{
								posTemp = pos;
								m_PathPtPos = posTemp;
							}
						}

						m_PathNodePos = posTemp;
						m_PathNearNodePos = posTemp;
						if( m_LastRoat )
						{
							delete m_LastRoat;
							m_LastRoat = NULL;
						}
						m_nLeaveCount = 0 ;
						return EN_MATCH_WELL;
					}
					if(enType1 == EN_PATHPOINTTYPE_NODE  ||  enType1 == EN_PATHPOINTTYPE_END || enType1 == EN_PATHPOINTTYPE_INTERMEDIATE)
					{
						if(!pos)
						{
							m_PathNodePos = NULL;
							m_PathPtPos = NULL;
							posTemp = NULL;
						}
						else
						{
							posTemp = pos;
							nFindNextNodeNum++;
							if( nFindNextNodeNum >= 30 )
							{
								m_PathNodePos = NULL;
								m_PathPtPos = NULL;
								posTemp = NULL;
								break;
							}
						}
					}
							
				}
			}
		}
	}
	POSITION pos = m_MapSet->GetHeadPosition();
	CGIS_GeoMap *pMap = NULL;
//	CGIS_GeoMap *pTempMap = NULL;
//	BOOL bInGlobeMap = GetCurrentScale() > 1852*60 / MAPSECLUDE_SCALE;
	while(pos)
	{
		pMap = (CGIS_GeoMap*)m_MapSet->GetNext(pos);
		unsigned short nMapID = pMap->GetMapInfo()->GetMapID();
		if( nMapID == MAPID_MAP_NATION || nMapID == MAPID_UM_NAVI )
			continue;
		if( IsPtInRect(pt.dx,pt.dy,pMap->GetMapInfo()->GetMapRect().dLeft,pMap->GetMapInfo()->GetMapRect().dTop,
				pMap->GetMapInfo()->GetMapRect().dRight,pMap->GetMapInfo()->GetMapRect().dBottom) )
			break;

	}
	if(!pMap)
	{
		Rpt = pt;
		return EN_MATCH_NULL;
	}
	//偏航距离
	if( enState == EN_MATCH_REALNAV )
	{
		if( m_ptMatch.dx == 181 && m_ptMatch.dy == 91 )
			mindis = 1000000;
		else
			mindis = CalcDis( pt.dx , pt.dy , m_ptMatch.dx , m_ptMatch.dy );
	}

	WaitForSingleObject( g_pMatchEvent->m_hObject , INFINITE );
	g_pMatchEvent->ResetEvent();
    EnMatchRoad eMatchRoad = pMap->MatchRoad( pt,Rpt,enState,&m_LastRoat,mindis,m_bHighway,m_ptMatch,m_nLeaveCount );
	g_pMatchEvent->SetEvent();
	m_nCarRunDirect = pMap->GetCarDirect();
	m_nCarRunNextPt = pMap->GetNextRutPt();
	return eMatchRoad;
}
int CGeoView::GetWrongDirect(  )
{
	int nLastRoadID = -1 ;
	if( m_LastRoat )
		nLastRoadID = m_LastRoat->GetRutID();
	int nStartRoadID = -1 ;
	CGIS_WayPoint *pTempWayPoint = NULL;
	if( m_pNetworkRoute )
		pTempWayPoint = (CGIS_WayPoint*)( m_pNetworkRoute->GetWayPointList()->GetHead() );
	if( pTempWayPoint )
	nStartRoadID = pTempWayPoint->GetWPRutID(EN_MAPTYPE_CITY);
	if( nLastRoadID != -1 && nStartRoadID != -1 )
	{
		if( nLastRoadID == nStartRoadID )
		{
			POSITION pos = m_MapSet->GetHeadPosition();
			CGIS_GeoMap *pMap = NULL;
			while(pos)
			{
				pMap = (CGIS_GeoMap*)m_MapSet->GetNext(pos);
				unsigned short nMapID = pMap->GetMapInfo()->GetMapID();
				if( nMapID == MAPID_MAP_NATION || nMapID == MAPID_UM_NAVI )
					continue;
				if( IsPtInRect(m_LastRoat->GetPtList()[0].dx,m_LastRoat->GetPtList()[0].dy,pMap->GetMapInfo()->GetMapRect().dLeft,pMap->GetMapInfo()->GetMapRect().dTop,
						pMap->GetMapInfo()->GetMapRect().dRight,pMap->GetMapInfo()->GetMapRect().dBottom) )
					break;
			}
			if( !pMap )
				return 0;
			else
			{
				if( pMap->GetCarDirect() == 0 )
					return 1;
				else
				{
					int nCarRunDirect;
					ST_GEO_PTXY ptRTemp;
					CPtrList *pPathPointList = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();
					CGIS_Pathpoint *pNextPathPt = NULL ;

					// 由于前面已经定义了同名的变量，其作用域在此也有效，为了避免混淆，
					// 更改变量名称。原来：POSITION pos 改为：posPath
					POSITION posPath = pPathPointList->GetHeadPosition();
					if( posPath )
						pPathPointList->GetNext(posPath);
					if( posPath )
						pNextPathPt = (CGIS_Pathpoint*)pPathPointList->GetNext( posPath );
					if( !pNextPathPt )
						return 0;
					ptRTemp = pNextPathPt->GetPathpointPosition();
					double dbAng = CalcAngle( ptRTemp.dx , ptRTemp.dy , pTempWayPoint->GetWPPosition().dx , pTempWayPoint->GetWPPosition().dy );
					double dbAngRoadEnd = CalcAngle( ptRTemp.dx , ptRTemp.dy ,
						m_LastRoat->GetPtList()[m_LastRoat->GetPtNum() - 1].dx,m_LastRoat->GetPtList()[m_LastRoat->GetPtNum() - 1].dy );
					double dbAngRoadStart = CalcAngle( ptRTemp.dx , ptRTemp.dy ,m_LastRoat->GetPtList()[0].dx,m_LastRoat->GetPtList()[0].dy );
					int nAngAbsEnd = abs( dbAng * 180.0 / PI - dbAngRoadEnd * 180.0 / PI );
					int nAngAbsStart = abs( dbAng * 180.0 / PI - dbAngRoadStart * 180.0 / PI );
					if( nAngAbsStart > 180 )
						nAngAbsStart = 360 - nAngAbsStart;
					if( nAngAbsEnd > 180 )
						nAngAbsEnd = 360 - nAngAbsEnd;
					if( nAngAbsStart > nAngAbsEnd  )
						nCarRunDirect = 2;//尾－》头
					else
						nCarRunDirect = 1;//头－》尾
					if( nCarRunDirect == pMap->GetCarDirect() )
						return 1;
					else
						return 2;
				}
			}
		}
		else
			return 3;
	}
	else
		return 0;
}
double CGeoView::CalcDis( double dx1,double dy1,double dx2,double dy2)
{
	return CalcDist(dx1,dy1,dx2,dy2);
}
double CGeoView::CalcAng( double dx1,double dy1,double dx2,double dy2)
{
	return CalcAngle(dx1,dy1,dx2,dy2);
}
void  CGeoView::GetPointArc(double dx1, double dy1, double dx2, double dy2, 
					  double dRatio, double& xx, double& yy)
{
	GetPointOnArc(dx1,dy1,dx2,dy2,dRatio,xx,yy);
}
void   CGeoView::SetNetworkRoute( CGIS_NetworkRoute* pNetworkRoute )
{
	m_pNetworkRoute = pNetworkRoute;
	m_CrossPos = NULL;
	if( m_pPtList != NULL )
	{
		delete[] m_pPtList;
		m_pPtList = NULL;
	}
	if(m_pNetworkRoute!= NULL)
	{
		CPtrList *pPathList = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();
		if( !pPathList  || pPathList->GetCount() == 0)
		{
			return;
		}
		m_pPtList = new ST_GEO_PTXY[ pPathList->GetCount() ];
		
		POSITION pos = pPathList->GetHeadPosition();
		int nCount = 0;
		CGIS_Pathpoint *pPathpoint= NULL;
		while( pos )
		{
			pPathpoint = (CGIS_Pathpoint*)pPathList->GetNext( pos );
			if( pPathpoint )
			{
				m_pPtList[nCount] = pPathpoint->GetPathpointPosition();
				nCount++;
			}
		}
	}
}
CGIS_NetworkRoute*	CGeoView::GetNetworkRoute()
{
	return m_pNetworkRoute;
}
void CGeoView::SetPlotRect(RECT rect)
{
	plotRect = rect;
}
double CGeoView::GetScale(int scalelevel)
{
	if(scalelevel < 1)
	{
		return -1;
	}
	if(scalelevel > (int)m_ScaleCount)
	{
		return 0;
	}
	return m_dbScaleValue[scalelevel-1];
}
int CGeoView::GetScaleCount()
{
	return m_ScaleCount;
}
void CGeoView::ClearMathState( BOOL bNaveOut )
{
	m_PathNodePos = NULL;
	m_PathPtPos = NULL;
	m_PathNearNodePos = NULL;
	m_nLeaveCount = 0;
	m_ptMatch.dx = 181;
	m_ptMatch.dy = 91;
	if( !bNaveOut )
		m_LastRoat = NULL;
}
void CGeoView::DrawUserLine(ST_GEO_PTXY* pPtList, int nPtNum,ST_GEO_PEN userPen)
{
	int nx, ny;
	int j = 0;
	int nxLast, nyLast;

	CPen pen;
	CPen *pOldPen;
	pen.CreatePen(userPen.nStyle, userPen.nWidth, userPen.nColor);
	pOldPen = (CPen*)pCurrentDC->SelectObject(&pen);

	POINT* pPt = new POINT[nPtNum];
	XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);
	pPt[0].x = nx;
	pPt[0].y = ny;
	nxLast = nx;
	nyLast = ny;
	j++;
	int nCount = 1;
	for(; j < nPtNum; j++)
	{
		XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);

		if(j<nPtNum-1)
		{
			if( abs(nx - nxLast)<4 && abs(ny - nyLast)<4 )
				continue;
		}
		pPt[nCount].x = nx;
		pPt[nCount].y = ny;
		nxLast = nx;
		nyLast = ny;
		nCount++;
	}
	pCurrentDC->Polyline( pPt , nCount );
	pCurrentDC->SelectObject(pOldPen);
	pen.DeleteObject();
	delete[] pPt;
}
void CGeoView::DrawNetworkRoute()
{
	if( m_bDrawNetWorkRoute && m_pNetworkRoute )
	{
		CPtrList *pPathList = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();
		if( !pPathList  || pPathList->GetCount() == 0)
		{
			return;
		}
		ST_GEO_PEN pen;
		pen.nColor = RGB(255,0,255);
//		pen.nColor = RGB(220,30,30);
//		pen.nColor = RGB(255,0,0);
		pen.nWidth = 8;
		pen.nStyle = PS_SOLID;

		
		ST_GEO_RECT rtTemp;
		CalcRect(m_pPtList,pPathList->GetCount() ,rtTemp);
		ST_GEO_RECT rtDraw ;
		if(GetAngle() != 0 )
			rtDraw = GetRotateRect();
		else
			rtDraw = GetCurrentDrawrect();
		int nClipMul = GeoRectIntersect( &rtDraw, &rtTemp );
		if( nClipMul == 2 )
			DrawUserLine( m_pPtList, pPathList->GetCount(),pen);
		else if( nClipMul != 0 )
		{
			BOOL bClip = TRUE;
			int nNum;
			ST_GEO_RECT rtTemp1;
			double dbX ,dbY;
			dbX = ( rtDraw.dRight - rtDraw.dLeft ) / 10.0;
			dbY = ( rtDraw.dBottom - rtDraw.dTop ) /10.0;
			rtTemp1.dBottom = rtDraw.dBottom + dbY ;
			rtTemp1.dTop = rtDraw.dTop - dbY;
			rtTemp1.dLeft = rtDraw.dLeft - dbX;
			rtTemp1.dRight = rtDraw.dRight + dbY;
			ST_GEO_PTXY* pPtList = NULL;
			if( pPathList->GetCount() < 35000)
				pPtList = GetClipRgn( m_pPtList, pPathList->GetCount(), 
									rtTemp1, &nNum, EN_GOTYPE_LINE );
			else
			{
				ST_GEO_RECT rect = rtTemp1;
				bClip = FALSE;
				int i, nSide;			
				double dx, dy;

				ST_GEO_PTXY *pTempList = m_pPtList, *pRtnList;
				pRtnList = NULL;

				double dx1, dy1, dx2, dy2;
				BOOL bAccepted, bLastAccepted;
				
				// 判断第一点是否是求出来的，1：初始值，2：原来链表值，3：求出的值
				int nFirstPointCalc;
				int nRtnNo;

				nNum = pPathList->GetCount();
				for(nSide = 1; nSide <= 4; nSide++)
				{
					int nPtNum = nNum * 2;
					if( nPtNum > 5000 )
						nPtNum = nNum + 1000;
					pRtnList = NULL;
					TRY
					{
						pRtnList	= new ST_GEO_PTXY[nPtNum];
					}
					CATCH( CMemoryException, e )
					{
						if( pTempList )
							delete[] pTempList;
						nNum = 0;
						return ;
					}
					END_CATCH
					if( !pRtnList )
					{
						if( pTempList )
							delete[] pTempList;
						nNum = 0;
						return ;
					}
					bAccepted	= bLastAccepted = TRUE;
					nRtnNo		= 0;

					nFirstPointCalc = 1;
					for(i = 0; i < nNum; i++)
					{
         				// 判断点是否该被取得
						switch (nSide)
						{
						case 1 :
							bAccepted = pTempList[i].dx >= rect.dLeft;
							dx1 = rect.dLeft;
							dy1 = rect.dTop;
							dx2 = rect.dLeft;
							dy2 = rect.dBottom;
							break;
						case 2 : 
							bAccepted = pTempList[i].dy >= rect.dTop;
							dx1 = rect.dLeft;
							dy1 = rect.dTop;
							dx2 = rect.dRight;
							dy2 = rect.dTop;
							break;
						case 3 : 
							bAccepted = pTempList[i].dx <= rect.dRight;
							dx1 = rect.dRight;
							dy1 = rect.dTop;
							dx2 = rect.dRight;
							dy2 = rect.dBottom;
							break;
						case 4 :
							bAccepted = pTempList[i].dy <= rect.dBottom;
							dx1 = rect.dLeft;
							dy1 = rect.dBottom;
							dx2 = rect.dRight;
							dy2 = rect.dBottom;
							break;
						}

						// 在范围内
						if(bAccepted)
						{
							if(!bLastAccepted)
							{
								if(GetIntersectedPoint(dx1, dy1, dx2, dy2, pTempList[i].dx,
									pTempList[i].dy, pTempList[i-1].dx, pTempList[i-1].dy, dx, dy, 2))
								{
									pRtnList[nRtnNo].dx = (float)dx;
									pRtnList[nRtnNo].dy = (float)dy;
									nRtnNo++;
								}
							}

							pRtnList[nRtnNo].dx = pTempList[i].dx;
							pRtnList[nRtnNo].dy = pTempList[i].dy;
							nRtnNo++;
							if(1 == nFirstPointCalc)
							{
								nFirstPointCalc = 2;
							}
						}
						else
						{
							if(1 == nFirstPointCalc)
							{
								nFirstPointCalc = 3;
							}

							if(i > 0)
							{
								if(bLastAccepted)
								{
									if(GetIntersectedPoint(dx1, dy1, dx2, dy2, pTempList[i].dx,
										pTempList[i].dy, pTempList[i-1].dx, pTempList[i-1].dy, dx, dy, 2))
									{
										pRtnList[nRtnNo].dx = (float)dx;
										pRtnList[nRtnNo].dy = (float)dy;
										nRtnNo++;
									}
								}
							}
						}

						bLastAccepted = bAccepted;
					}
					if(pTempList != m_pPtList)
					{
						delete [] pTempList;
						pTempList = NULL;
					}

					pTempList	= pRtnList;
					nNum	= nRtnNo;
				}
				pPtList = pRtnList;
			}

			if( pPtList && nNum > 1 )
				DrawUserLine( pPtList , nNum , pen );
			if( pPtList && !bClip)
			{
				delete[] pPtList;
				pPtList = NULL;
			}
		}




		CPtrList *pWPList = m_pNetworkRoute->GetWayPointList();
		CGIS_WayPoint *pWayPoint = NULL;
		POSITION pos = pWPList->GetHeadPosition();
		ST_GEO_PTXY WPPts[2];
		while(pos)
		{
			pWayPoint = (CGIS_WayPoint*)pWPList->GetNext(pos);
			if( pWayPoint )
			{
				WPPts[0] = pWayPoint->GetWPPosition();
				if( pWayPoint->GetWPMapID() == 1 )
					WPPts[1] = pWayPoint->GetWPProjPosition(EN_MAPTYPE_NATION);
				else
					WPPts[1] = pWayPoint->GetWPProjPosition(EN_MAPTYPE_CITY);
				DrawUserLine( WPPts, 2,pen );
			}
		}

		POSITION ps = m_pNetworkRoute->GetWayPointList()->GetHeadPosition();
		CGIS_WayPoint* pTempWP;
		int i = 1;
		while(ps)
		{
			pTempWP = (CGIS_WayPoint*)m_pNetworkRoute->GetWayPointList()->GetNext(ps);
			if( i == 1 )
			{
				i++;
				continue;
			}
			if( ps )
				DrawFlag(pTempWP->GetWPPosition().dx,pTempWP->GetWPPosition().dy,1);
			else
				DrawFlag(pTempWP->GetWPPosition().dx,pTempWP->GetWPPosition().dy,0);

			i++;
		}
	}
}
COLORREF CGeoView::GetBackColor()
{
	return m_colBack;
}
	//设置白天黑夜模式
void CGeoView::SetbDayShow( BOOL bDayShow )
{
	m_bDayShow = bDayShow;
	if( pCurrentDC->GetSafeHdc() )
	{
		pCurrentDC->SetTextColor( RGB(0,0,0) );
//		if( bDayShow )
//			pCurrentDC->SetTextColor( RGB(0,0,0) );
//		else
//			pCurrentDC->SetTextColor( RGB(200,120,200) );
	}
}
	//取得白天黑夜模式
BOOL CGeoView::GetbDayShow()
{
	return m_bDayShow;
}
void  CGeoView::DrawOutlineForChars(CDC* pDC, int x, int y,
										   swString& strContext,
										   COLORREF ExCol, COLORREF InCol )
{
	swRect rt(0,0,0,0);
	pDC->DrawText(strContext, &rt, DT_CALCRECT);

	CDC  ExDC, InDC;
	CBitmap *pOldExBmp,ExBmp,*pOldInBmp, InBmp;

	LOGFONT logFont,ExlogFont,InlogFont;
	CFont *pCurFont = pDC->GetCurrentFont();
	CFont *pOldExFont,ExFont,*pOldInFont,InFont ;
	pCurFont->GetLogFont(&logFont);
	ExlogFont = InlogFont = logFont;
	ExFont.Detach();
	InFont.Detach();
	ExFont.CreateFontIndirect(&ExlogFont);
	InFont.CreateFontIndirect(&InlogFont);

	ExDC.CreateCompatibleDC(pDC);
	ExBmp.CreateCompatibleBitmap(pDC, rt.Width(), rt.Height());
	pOldExBmp = ExDC.SelectObject(&ExBmp);
	pOldExFont = ExDC.SelectObject(&ExFont);
	ExDC.SetBkColor(RGB(255,0,255));
	ExDC.SetTextColor( ExCol );
	ExDC.DrawText(strContext, &rt, DT_NOCLIP);
	
	InDC.CreateCompatibleDC(pDC);
	InBmp.CreateCompatibleBitmap(pDC, rt.Width(), rt.Height());
	pOldInBmp = InDC.SelectObject(&InBmp);
	pOldInFont = InDC.SelectObject(&InFont);
	InDC.SetBkColor(RGB(255,0,255));
	InDC.SetTextColor( InCol );
	InDC.DrawText(strContext, &rt, DT_NOCLIP);
#ifdef _WIN32_WCE
	TransparentImage( pDC->GetSafeHdc(), x - 1, y, rt.Width(), rt.Height(),
		ExDC.GetSafeHdc(), 0, 0, rt.Width(), rt.Height() , RGB(255,0,255) );
	TransparentImage( pDC->GetSafeHdc(), x + 1, y, rt.Width(), rt.Height(),
		ExDC.GetSafeHdc(), 0, 0, rt.Width(), rt.Height() , RGB(255,0,255));
	TransparentImage( pDC->GetSafeHdc(), x , y - 1, rt.Width(), rt.Height(),
		ExDC.GetSafeHdc(), 0, 0, rt.Width(), rt.Height() , RGB(255,0,255));
	TransparentImage( pDC->GetSafeHdc(), x, y + 1, rt.Width(), rt.Height(),
		ExDC.GetSafeHdc(), 0, 0, rt.Width(), rt.Height() , RGB(255,0,255) );
	
	TransparentImage( pDC->GetSafeHdc(), x, y , rt.Width(), rt.Height(),
			InDC.GetSafeHdc(), 0, 0, rt.Width(), rt.Height(), RGB(255,0,255) );
#else
	ASSERT(FALSE);	//to be add
#endif

	ExDC.SelectObject( pOldExBmp );
	ExDC.SelectObject( pOldExFont );
	InDC.SelectObject( pOldInBmp );
	InDC.SelectObject( pOldInFont );
	InDC.DeleteDC();
	ExDC.DeleteDC();
	ExBmp.DeleteObject();
	InBmp.DeleteObject();
}
void CGeoView::DrawRutLayer( ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer ){
	if(!pLayer){
		return;
	}
	BOOL bAnno = ( (dbScale<pLayer->GetLayerInfo()->GetMaxARScale()) && (dbScale>pLayer->GetLayerInfo()->GetMinARScale()) ) 
					|| (pLayer->GetLayerInfo()->GetMaxARScale() == -1);

	CPtrList *pObjList = NULL;
	pLayer->GetObjectListMap()->Lookup( nBID, pObjList );
	if( !pObjList )
		return;
	POSITION pos = pObjList->GetHeadPosition();
	CGIS_FeatureRut *pObj = NULL;
	//绘制列表
	CPtrList listDraw;
	//地物个数
	int nDrawObj = 0 ;
	//最多绘制的点的个数
	int nDrawPtMax = 0;
	//申请点的最大内存
	POINT *ptList = NULL;
	//地物的PART数
	int *pPart = NULL;
	while( pos )
	{
		pObj = (CGIS_FeatureRut*)pObjList->GetNext( pos );
		if( !pObj )
			continue;

		int nClipType = GeoRectIntersect(&rtDraw, &pObj->GetRectObj());
		if(nClipType == 0)	// 没有交集
			continue;
		if( m_bMoveMap && GeoRectIntersect( &rtOld,&pObj->GetRectObj() ) == 2  && m_Angle == 0)
			continue;
		if( !pObj->GetPtList() || pObj->GetPtNum() <=1 )
			continue;
		listDraw.AddTail( pObj );
		nDrawObj++;
		nDrawPtMax += pObj->GetPtNum();
	}
	if( nDrawObj == 0 )
		return;
//	ptList = new POINT[nDrawPtMax];
//	pPart = new int[nDrawObj];
	ptList = g_ptList;
	pPart = g_pPart;
	pos = listDraw.GetHeadPosition();
	int nCount = 0;
	int nPtCount = 0;
	int nPtLastCount = 0;
	CGIS_LayerInfo *pLayInfo = pLayer->GetLayerInfo();
	while( pos )
	{
		nPtLastCount = nPtCount;
		pObj = (CGIS_FeatureRut*)listDraw.GetNext( pos );
	
		int nx, ny;
		int nxEnd,nyEnd;
		int nxLast, nyLast;
		ST_GEO_PTXY* pPtList = pObj->GetPtList();
		int nPtNum = pObj->GetPtNum();
		

		int nCurrentScale = GetCurrentScale();
		int nFilter = 4;
		if( nCurrentScale > 100 )
			nFilter = 8;
		double dbXOld = pPtList[0].dx ;
		double dbYOld = pPtList[0].dy ;
		XyMapToView(pPtList[0].dx, pPtList[0].dy, nx, ny);
		XyMapToView(pPtList[nPtNum-1].dx, pPtList[nPtNum-1].dy, nxEnd, nyEnd );
		if( pObj->GetAnnotation()!=NULL )
		{
			swString str;
			CharToUnicode( pObj->GetAnnotation() , &str );
			int nStrNum = str.GetLength();
//			int nStrNum = swString(pObj->GetAnnotation()).GetLength();
			int nLength = abs( (nxEnd - nx) + (nyEnd - ny) );
			if( nLength > (m_nH * nStrNum)  )
			{
//				ST_GEO_PTXY *pPt = new ST_GEO_PTXY[nStrNum];
				ST_GEO_PTXY *pPt = g_ptNum;
				BOOL bAdd = GetPointsOnLine(pObj->GetPtList(), pObj->GetPtNum(), nStrNum, pPt);
				if( bAdd )
				{
					//计算文字显示方向
					BOOL bSame = ReadDirect( nx , ny , nxEnd , nyEnd );
					AddAnnotate( pPt, nStrNum, str , (pLayInfo->GetLayerID()-4) ,bSame );
				}
//				delete [] pPt;
			}
		}
		if( nCurrentScale > 100 )
		{
			if( nx == nxEnd && ny == nyEnd )
				continue;
			if( abs(nxEnd-nx)<nFilter && abs(nyEnd-ny)<nFilter )
			{
				ptList[nPtCount].x = nx;
				ptList[nPtCount].y = ny;
				ptList[nPtCount+1].x = nxEnd;
				ptList[nPtCount+1].y = nyEnd;
				nPtCount+=2;
//				FilterDrawRut( ptList , nPtCount , nPtCount-nPtLastCount,pPart , nCount );
				pPart[nCount] = nPtCount - nPtLastCount;
				nCount++;
				continue;
			}
		}
		ptList[nPtCount].x = nx;
		ptList[nPtCount].y = ny;
		nPtCount++;
		nxLast = nx;
		nyLast = ny;

		int nXDis , nYDis;
		for( int j = 1 ; j < nPtNum; j++ ) 
		{
			if( j < nPtNum-1 )
			{
				nXDis = ( pPtList[j].dx - dbXOld ) * dNewScale ;
				if( abs(nXDis) < nFilter )
				{
					nYDis = ( dbYOld - pPtList[j].dy ) * dNewScale ;
					if( abs(nYDis) < nFilter )
						continue;
				}
			}
			if( j == nPtNum-1)
			{
				nx = nxEnd;
				ny = nyEnd;
			}
			else
				XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);
			nxLast = nx;
			nyLast = ny;

			dbXOld = pPtList[j].dx;
			dbYOld = pPtList[j].dy;
			ptList[nPtCount].x = nx;
			ptList[nPtCount].y = ny;
			nPtCount++;
		}
//		FilterDrawRut( ptList , nPtCount , nPtCount-nPtLastCount,pPart , nCount );
		pPart[nCount] = nPtCount - nPtLastCount;
		nCount++;
	}
	ST_GEO_PEN penTemp;
	
	ST_GEO_PEN pen = pLayInfo->GetPen();
	int nWidth;
	if( m_ScaleCurLevel < 5 )
	{
		if( m_ScaleCurLevel == 4 || m_ScaleCurLevel == 3 )
			nWidth = pen.nWidth  ;
		else
			nWidth = (4 - m_ScaleCurLevel)*2 + pen.nWidth;
		ST_GEO_BRUSH brushTemp = pLayInfo->GetBrush();
		int R = GetRValue( brushTemp.nForeColor ) ;
		int G = GetGValue( brushTemp.nForeColor ) ;
		int B = GetBValue( brushTemp.nForeColor ) ;
		penTemp.nColor = RGB( R,G,B );
		penTemp.nWidth = nWidth;
		penTemp.nStyle = PS_SOLID;
		PreparePen( penTemp );
		if( nCount == 1 )
			pCurrentDC->Polyline( ptList , pPart[0] );
		else
#ifdef _WIN32_WCE
			wce_PolyPolyline( pCurrentDC->GetSafeHdc() , ptList , (unsigned long*)pPart , nCount );
			//pCurrentDC->PolyPolyline( pCurrentDC->GetSafeHdc() , ptList , (unsigned long*)pPart , nCount );
#else
			pCurrentDC->PolyPolyline( ptList , (unsigned long*)pPart , nCount );
	//			ASSERT(FALSE);
#endif
		ReleasePen();
	}
	nWidth = pen.nWidth ;
	if( m_ScaleCurLevel < 3)
		nWidth = (4 - m_ScaleCurLevel)*2 + pen.nWidth;
	penTemp.nColor = pen.nColor;
	if( nWidth > 2 )
		penTemp.nWidth = nWidth-2;
	else
		penTemp.nWidth = nWidth;
	penTemp.nStyle = PS_SOLID;
	PreparePen( penTemp );
	if( nCount == 1 )
		pCurrentDC->Polyline( ptList , pPart[0] );
	else
	{
#ifdef _WIN32_WCE
		{
		wce_PolyPolyline( pCurrentDC->GetSafeHdc() , ptList , (unsigned long*)pPart , nCount );
		}
#else
		{
			pCurrentDC->PolyPolyline( ptList , (unsigned long*)pPart , nCount );
		}	
#endif
	}
	ReleasePen();

//	delete[] pPart;
//	delete[] ptList;
}
void CGeoView::DrawPolyLayer( ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer )
{
	if(!pLayer){
		return;
	}
	BOOL bAnno = ( (dbScale<pLayer->GetLayerInfo()->GetMaxARScale()) && (dbScale>pLayer->GetLayerInfo()->GetMinARScale()) ) 
					|| (pLayer->GetLayerInfo()->GetMaxARScale() == -1);

	CPtrList *pObjList = NULL;
	pLayer->GetObjectListMap()->Lookup( nBID, pObjList );
	if( !pObjList )
		return;
	POSITION pos = pObjList->GetHeadPosition();
	CGIS_FeaturePoly *pObj = NULL;
	//绘制列表
	CPtrList listDraw;
	//地物个数
	int nDrawObj = 0 ;
	//最多绘制的点的个数
	int nDrawPtMax = 0;
	//申请点的最大内存
	POINT *ptList = NULL;
	//地物的PART数
	int *pPart = NULL;
	//每个地物的裁剪返回值
//	int *nFeatureClipType = new int[pObjList->GetCount()];
	int *nFeatureClipType = g_nFeatureClipType;
	//最多的地物个数，包括分PART的地物
	int nAllPolyCount = 0;

	BOOL bSeclude = MAPSECLUDE_SCALE>dbScale? TRUE:FALSE;
	//a.过滤绘制对象
	while( pos )
	{
		pObj = (CGIS_FeaturePoly*)pObjList->GetNext( pos );
		if( !pObj )
			continue;
		//判别绘制区域与几何对象是否相交
		int nClipType = GeoRectIntersect(&rtDraw, &pObj->GetRectObj());
		if(nClipType == 0)	// 没有交集
			continue;
		if( m_bMoveMap && GeoRectIntersect( &rtOld,&pObj->GetRectObj() ) == 2  && m_Angle == 0)
			continue;
		if( !pObj->GetPtList() || pObj->GetPtNum() <=2 )  //显示的点数太少，不用绘制
			continue;
		//根据显示比例和地物的显示级别确定是否显示某个地物
		if( pObj->GetPolyShowClass() == 1  &&  m_ScaleCurLevel > 4 )
			continue;
		if( pObj->GetPolyShowClass() == 2  &&  m_ScaleCurLevel > 6 )
			continue;
		if( pObj->GetPolyShowClass() < 7 && m_ScaleCurLevel > 12 )
			continue;
		listDraw.AddTail( pObj );
		nFeatureClipType[nDrawObj] = nClipType;
		nDrawObj++;
		nAllPolyCount += pObj->GetPartNum();
		nDrawPtMax += pObj->GetPtNum();
	}
	if( nDrawObj == 0 )
	{
//		delete[] nFeatureClipType;
		return;
	}
	CGIS_LayerInfo *pLayInfo = pLayer->GetLayerInfo();
	ST_GEO_PEN pen1 = pLayInfo->GetPen();
	pen1.nStyle = PS_NULL;
	PreparePen( pen1 );
	PrepareBrush( pLayInfo->GetBrush() );

//	ptList = new POINT[nDrawPtMax];
	ptList = g_ptList;
//	pPart = new int[nAllPolyCount];
	pPart = g_pPart;
	pos = listDraw.GetHeadPosition();
	int nCount = 0;
	int nPtCount = 0;
	int nPtLastCount = 0;
	int nObjCount = 0;
	//b.准备开始绘制
	while( pos )
	{
		nObjCount ++;
		nPtLastCount = nPtCount;
		pObj = (CGIS_FeaturePoly*)listDraw.GetNext( pos );//过滤之后的实际绘制对象

		if( pObj->GetPartNum() == 1 ) //单个多边形组成的对象
		{
			int nx, ny;
			int nxEnd,nyEnd;
			int nxLast, nyLast;
			ST_GEO_PTXY* pPtList; 
			int nPtNum;
			nPtNum = pObj->GetPtNum();
			pPtList = pObj->GetPtList(); 
			//进行裁剪,单独绘制
			if( nFeatureClipType[nObjCount-1] != 2 )
			{
				int nNum;
				ST_GEO_RECT rtTemp;
				double dbClip = 10 / dNewScale;
				rtTemp.dBottom = rtDraw.dBottom + dbClip;
				rtTemp.dTop = rtDraw.dTop - dbClip;
				rtTemp.dLeft = rtDraw.dLeft - dbClip;
				rtTemp.dRight = rtDraw.dRight + dbClip;
				//仅仅绘制在指定区域内的多边形的点
				ST_GEO_PTXY* pTempPtList = GetClipRgn( pPtList, nPtNum, 
										rtTemp, &nNum, EN_GOTYPE_POLY );
				if(nNum > 2 && pTempPtList )
				{
					DrawPoly( pTempPtList , nNum ); //绘制多边形
//					delete[] pTempPtList;
				}
				continue;
			}
			double dbXOld = pPtList[0].dx ;
			double dbYOld = pPtList[0].dy ;

			XyMapToView(pPtList[0].dx, pPtList[0].dy, nx, ny);
			
			ptList[nPtCount].x = nx;
			ptList[nPtCount].y = ny;
			nPtCount++;
			nxLast = nx;
			nyLast = ny;

			int nXDis , nYDis;
			for( int j = 1 ; j < nPtNum; j++ ) 
			{
//				if( j < nPtNum-1 )
//				{
//					nXDis = ( pPtList[j].dx - dbXOld ) * dNewScale ;
//					if( abs(nXDis) == 0 )
//					{
//						nYDis = ( dbYOld - pPtList[j].dy ) * dNewScale ;
//						if( abs(nYDis) == 0 )
//							continue;
//					}
//				}
				XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);
				if( nx == nxLast && ny == nyLast )
					continue;
				nxLast = nx;
				nyLast = ny;

				dbXOld = pPtList[j].dx;
				dbYOld = pPtList[j].dy;
				ptList[nPtCount].x = nx;
				ptList[nPtCount].y = ny;
				nPtCount++;
			}
	//		FilterDrawRut( ptList , nPtCount , nPtCount-nPtLastCount,pPart , nCount );
			//如果只有两个点，则不显示
			if( (nPtCount - nPtLastCount) <= 2 )
				nPtCount = nPtLastCount;
			else
			{
				pPart[nCount] = nPtCount - nPtLastCount;
				nCount++;
			}
		}
		//分每一个PART进行处理
		else
		{
			int nPtStart = 0;
			ST_GEO_PTXY* pPtList; 
			int nPtNum;
			ST_GEO_PTXY* pAllPtList = new ST_GEO_PTXY[pObj->GetPtNum() + pObj->GetPartNum() ];
			
			// 为了避免混淆，不要定义同名变量，因此下面更改变量名称
			// 原来：int nCount 改为：nRecCount
			int nRecCount = 0;
	
			ST_GEO_PTXY* pPartOne = new ST_GEO_PTXY[pObj->GetPartNum()];
			int nDrawPartNum = 0;
			int nNum;
			ST_GEO_PTXY *pTempPtList;
			int k;
			for( k = 0 ; k < pObj->GetPartNum() ; k++ )
			{
				if( k>0 )
					nPtStart += pObj->GetPart()[k-1];
				nPtNum = pObj->GetPart()[k];
				pPtList = &(pObj->GetPtList()[nPtStart]);
				if( nFeatureClipType[nObjCount-1] != 2 )
				{
					ST_GEO_RECT rtTemp;
					::CalcRect(pPtList,nPtNum,rtTemp);
					int nClipMul = GeoRectIntersect( &rtDraw, &rtTemp );
					if( nClipMul == 0 )
						continue;
					double dbClip = 10 / dNewScale;
					rtTemp.dBottom = rtDraw.dBottom + dbClip;
					rtTemp.dTop = rtDraw.dTop - dbClip;
					rtTemp.dLeft = rtDraw.dLeft - dbClip;
					rtTemp.dRight = rtDraw.dRight + dbClip;
					pTempPtList = GetClipRgn( pPtList, nPtNum, 
											rtTemp, &nNum, EN_GOTYPE_POLY );
					if( pTempPtList )
					{
						for( int kk = 0 ; kk < nNum ; kk++ )
						{
							pAllPtList[nRecCount] = pTempPtList[kk];
							nRecCount++;
						}
						if( nNum > 0 )
						{
							pPartOne[nDrawPartNum] = pTempPtList[0];
							nDrawPartNum++;
						}
//						delete[] pTempPtList;
					}
				}
				else
				{
					pTempPtList = pPtList;
					nNum = nPtNum;
					for( int kk = 0 ; kk < nNum ; kk++ )
					{
						pAllPtList[nRecCount] = pTempPtList[kk];
						nRecCount++;
					}
					pPartOne[nDrawPartNum] = pTempPtList[0];
					nDrawPartNum++;
				}
			}
			nPtStart = 0;
			for( k = nDrawPartNum-2 ; k >=0 ; k-- )
			{
				pAllPtList[nRecCount] = pPartOne[k];
				nRecCount++;
			}
			if( nRecCount > 2 )
				DrawPoly( pAllPtList , nRecCount );
			delete[] pAllPtList;
			delete[] pPartOne;

		}
	}
	if( nCount == 1 )
		pCurrentDC->Polygon( ptList , pPart[0] );
	else if( nCount > 0  )
	{
#ifdef _WIN32_WCE
		wce_PolyPolygon( pCurrentDC->GetSafeHdc() , ptList , pPart , nCount );
#else
		pCurrentDC->PolyPolygon( ptList , pPart , nCount );
#endif
	}
	ReleasePen();
	ReleaseBrush();

//	delete[] pPart;
//	delete[] ptList;
//	delete[] nFeatureClipType;
}
void CGeoView::DrawLineLayer( ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer ){
	if(!pLayer){
		return;
	}
	BOOL bAnno = ( (dbScale<pLayer->GetLayerInfo()->GetMaxARScale()) && (dbScale>pLayer->GetLayerInfo()->GetMinARScale()) ) 
					|| (pLayer->GetLayerInfo()->GetMaxARScale() == -1);

	CPtrList *pObjList = NULL;
	pLayer->GetObjectListMap()->Lookup( nBID, pObjList );
	if( !pObjList )
		return;
	POSITION pos = pObjList->GetHeadPosition();
	CGIS_FeatureLine *pObj = NULL;
	//绘制列表
	CPtrList listDraw;
	//地物个数
	int nDrawObj = 0 ;
	//最多绘制的点的个数
	int nDrawPtMax = 0;
	//申请点的最大内存
	POINT *ptList = NULL;
	//地物的PART数
	int *pPart = NULL;
	//每个地物的裁剪返回值
	int *nFeatureClipType = g_nFeatureClipType;
//	int *nFeatureClipType = new int[pObjList->GetCount()];
	//最多的地物个数，包括分PART的地物
	int nAllPolyCount = 0;
	while( pos )
	{
		pObj = (CGIS_FeatureLine*)pObjList->GetNext( pos );
		if( !pObj )
			continue;

		int nClipType = GeoRectIntersect(&rtDraw, &pObj->GetRectObj());
		if(nClipType == 0)	// 没有交集
			continue;
		if( m_bMoveMap && GeoRectIntersect( &rtOld,&pObj->GetRectObj() ) == 2  && m_Angle == 0)
			continue;
		if( !pObj->GetPtList() || pObj->GetPtNum() <=1 )
			continue;
		listDraw.AddTail( pObj );
		nFeatureClipType[nDrawObj] = nClipType;
		nDrawObj++;
		nAllPolyCount += pObj->GetPartNum();
		nDrawPtMax += pObj->GetPtNum();
	}
	if( nDrawObj == 0 )
	{
//		delete[] nFeatureClipType;
		return;
	}
	CGIS_LayerInfo *pLayInfo = pLayer->GetLayerInfo();
	ST_GEO_PEN pen = pLayInfo->GetPen();
	PreparePen( pen );
	ptList = g_ptList;
	pPart = g_pPart;
//	ptList = new POINT[nDrawPtMax];
//	pPart = new int[nAllPolyCount];
	pos = listDraw.GetHeadPosition();
	int nCount = 0;
	int nPtCount = 0;
	int nPtLastCount = 0;
	int nObjCount = 0;
	while( pos )
	{
		nObjCount ++;
		pObj = (CGIS_FeatureLine*)listDraw.GetNext( pos );

		int nPtStart = 0;
		//分每一个PART进行处理
		for( int nPartNum = 0 ; nPartNum < pObj->GetPartNum() ; nPartNum++ )
		{
			nPtLastCount = nPtCount;
			if( nPartNum>0 )
				nPtStart += pObj->GetPart()[nPartNum-1];
			int nx, ny;
			int nxEnd,nyEnd;
			int nxLast, nyLast;
			ST_GEO_PTXY* pPtList; 
			int nPtNum;
			if( pObj->GetPartNum() == 1 )
			{
				nPtNum = pObj->GetPtNum();
				pPtList = pObj->GetPtList(); 
			}
			else
			{
				nPtNum = pObj->GetPart()[nPartNum];
				pPtList = &(pObj->GetPtList()[nPtStart]);
			}
			//进行裁剪,单独绘制
			if( nFeatureClipType[nObjCount-1] != 2 )
			{
				int nNum;
				ST_GEO_RECT rtTemp;
				if( pObj->GetPartNum() > 1 )
				{
					::CalcRect(pPtList,nPtNum,rtTemp);
					int nClipMul = GeoRectIntersect( &rtDraw, &rtTemp );
					if( nClipMul == 0 )
						continue;
				}
				double dbClip = 10 / dNewScale;
				rtTemp.dBottom = rtDraw.dBottom + dbClip;
				rtTemp.dTop = rtDraw.dTop - dbClip;
				rtTemp.dLeft = rtDraw.dLeft - dbClip;
				rtTemp.dRight = rtDraw.dRight + dbClip;
				ST_GEO_PTXY* pTempPtList = GetClipRgn( pPtList, nPtNum, 
										rtTemp, &nNum, EN_GOTYPE_LINE );
				if(nNum > 1 && pTempPtList )
				{
					DrawLine( pTempPtList , nNum );
//					delete[] pTempPtList;
				}
				continue;
			}

			

			int nCurrentScale = GetCurrentScale();
			int nFilter = 4;
			if( nCurrentScale > 100 )
				nFilter = 6;
			double dbXOld = pPtList[0].dx ;
			double dbYOld = pPtList[0].dy ;
			XyMapToView(pPtList[0].dx, pPtList[0].dy, nx, ny);
			XyMapToView(pPtList[nPtNum-1].dx, pPtList[nPtNum-1].dy, nxEnd, nyEnd );
			if( nCurrentScale > 100 )
			{
				if( nx == nxEnd && ny == nyEnd )
					continue;
				if( abs(nxEnd-nx)<nFilter && abs(nyEnd-ny)<nFilter )
				{
					ptList[nPtCount].x = nx;
					ptList[nPtCount].y = ny;
					ptList[nPtCount+1].x = nxEnd;
					ptList[nPtCount+1].y = nyEnd;
					nPtCount+=2;
	//				FilterDrawRut( ptList , nPtCount , nPtCount-nPtLastCount,pPart , nCount );
					pPart[nCount] = nPtCount - nPtLastCount;
					nCount++;
					continue;
				}
			}
			ptList[nPtCount].x = nx;
			ptList[nPtCount].y = ny;
			nPtCount++;
			nxLast = nx;
			nyLast = ny;

			int nXDis , nYDis;
			for( int j = 1 ; j < nPtNum; j++ ) 
			{
				if( j < nPtNum-1 )
				{
					nXDis = ( pPtList[j].dx - dbXOld ) * dNewScale ;
					if( abs(nXDis) < nFilter )
					{
						nYDis = ( dbYOld - pPtList[j].dy ) * dNewScale ;
						if( abs(nYDis) < nFilter )
							continue;
					}
				}
				if( j == nPtNum-1)
				{
					nx = nxEnd;
					ny = nyEnd;
				}
				else
					XyMapToView(pPtList[j].dx, pPtList[j].dy, nx, ny);
				nxLast = nx;
				nyLast = ny;

				dbXOld = pPtList[j].dx;
				dbYOld = pPtList[j].dy;
				ptList[nPtCount].x = nx;
				ptList[nPtCount].y = ny;
				nPtCount++;
			}
	//		FilterDrawRut( ptList , nPtCount , nPtCount-nPtLastCount,pPart , nCount );
			pPart[nCount] = nPtCount - nPtLastCount;
			nCount++;
		}
	}
	if( nCount == 1 )
		pCurrentDC->Polyline( ptList , pPart[0] );
	else
#ifdef _WIN32_WCE
		wce_PolyPolyline( pCurrentDC->GetSafeHdc() , ptList , (unsigned long*)pPart , nCount );
#else
		pCurrentDC->PolyPolyline( ptList , (unsigned long*)pPart , nCount );
//		ASSERT(FALSE);//to be add
#endif
	ReleasePen();

//	delete[] pPart;
//	delete[] ptList;
//	delete[] nFeatureClipType;
}
void CGeoView::DrawPointLayer(  ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer  )
{    // scale - 一度用多少个像素来表示
	BOOL bAnno = ( (dbScale<pLayer->GetLayerInfo()->GetMaxARScale()) && (dbScale>pLayer->GetLayerInfo()->GetMinARScale()) ) 
					|| (pLayer->GetLayerInfo()->GetMaxARScale() == -1);

	CPtrList *pObjList = NULL;
	pLayer->GetObjectListMap()->Lookup( nBID, pObjList );
	if( !pObjList )
		return;
	POSITION pos = pObjList->GetHeadPosition();
	CGIS_FeaturePoint *pPoint = NULL;
	int nLayerID = pLayer->GetLayerInfo()->GetLayerID();
	while( pos )
	{
		pPoint = (CGIS_FeaturePoint*)pObjList->GetNext( pos );
		//根据显示比例和地物的显示级别确定是否显示城镇点
		if( nLayerID == 24 )
		{
			if( pPoint->GetPointObjectType() > 4  &&  m_ScaleCurLevel > 5 )
				continue;
			else if( pPoint->GetPointObjectType() == 4  &&  m_ScaleCurLevel > 7 )
				continue;
			if( pPoint->GetPointObjectType() > 2 )
				pPoint->SetIconID( -1 );
		}
		BOOL bInOld =  IsPtInRect(pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, rtOld.dLeft, 
					  rtOld.dTop, rtOld.dRight, rtOld.dBottom);
		if(m_bMoveMap && bInOld && m_Angle == 0)
			continue;
		if( (IsPtInRect(pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, rtDraw.dLeft, 
			  rtDraw.dTop, rtDraw.dRight, rtDraw.dBottom)) )
		{
			int nx,ny;
			XyMapToView( pPoint->GetPtList()->dx, pPoint->GetPtList()->dy, nx, ny );
			ST_PT_DRAW *ptDraw = new ST_PT_DRAW;
			ptDraw->nIconID = pPoint->GetIConID();
			ptDraw->nx = nx;
			ptDraw->ny = ny;
			if( !AddDrawPOI( ptDraw ) )
			{
				delete ptDraw;
				ptDraw = NULL;
			}
			if(bAnno && pPoint->GetAnnotation()!=NULL 
				&& ((nLayerID>23 || nLayerID<6) || m_ScaleCurLevel == 1) )
			{
				swString str;
				CharToUnicode( pPoint->GetAnnotation() , &str );
				AddAnnotate( pPoint->GetPtList(), 1, str );
			}
		}
	}
}
void CGeoView::FilterDrawRut( POINT* ptList , int &nPtCount , int nNum , int* pPart , int &nCount )
{
	if( !ptList || !pPart )
		return;
	if( nCount == 0 )
	{
		pPart[nCount] = nNum;
		nCount++;
	}
	else
	{
		POINT nStart = ptList[nPtCount-nNum];
		POINT nEnd = ptList[nPtCount-1];
		POINT nTempStart,nTempEnd;
		int nTempCount = 0;
		//是否能与现有的道路相连
		BOOL bHave = FALSE;
		for( int i = 0 ; i < nCount ; i++ )
		{
			nTempStart = ptList[nTempCount];
			nTempCount += pPart[i];
			nTempEnd = ptList[nTempCount -1];
			//与某条路段相连有以下四种情况
			if( (nTempStart.x == nStart.x && nTempStart.y == nStart.y) 
				|| (nTempStart.x == nEnd.x && nTempStart.y == nEnd.y) 
				|| (nTempEnd.x == nStart.x && nTempEnd.y == nStart.y) 
				|| (nTempEnd.x == nEnd.x && nTempEnd.y == nEnd.y) )
			{
				bHave = TRUE;
				POINT* pTempPtList = new POINT[nNum];
				for( int j = 0 ; j < nNum ; j++ )
					pTempPtList[j] = ptList[nPtCount-nNum+j];
				if( (nTempStart.x == nStart.x && nTempStart.y == nStart.y) 
					|| (nTempStart.x == nEnd.x && nTempStart.y == nEnd.y) )
				{
					//移位
					int k;
					for( k = (nPtCount-nNum-1) ; k > (nTempCount-pPart[i]-1)  ; k-- )
						ptList[k+nNum-1] = ptList[k];
					if( nTempStart.x == nStart.x && nTempStart.y == nStart.y )
					{
						for( k = 0 ; k < nNum-1 ; k++)
							ptList[nTempCount-pPart[i]+k] = pTempPtList[nNum-k-1];
					}
					else
					{
						for( k = 0 ; k < nNum-1 ; k++)
							ptList[nTempCount-pPart[i]+k] = pTempPtList[k];
					}
				}
				else
				{
					//移位
					int k;
					for( k = (nPtCount-nNum-1) ; k > (nTempCount-1)  ; k-- )
						ptList[k+nNum-1] = ptList[k];
					if( nTempEnd.x == nStart.x && nTempEnd.y == nStart.y )
					{
						for( k = 0 ; k < nNum-1 ; k++)
							ptList[nTempCount+k] = pTempPtList[k+1];
					}
					else
					{
						for( k = 0 ; k < nNum-1 ; k++)
							ptList[nTempCount+k] = pTempPtList[nNum-k-2];
					}

				}
				delete[] pTempPtList;
			}
			if( bHave )
			{
				pPart[i] += (nNum-1);
				nPtCount--;
				break;
			}
		}
		if( !bHave )
		{
			pPart[nCount] = nNum;
			nCount++;
		}
	}
}
void  CGeoView::DrawSortLayer(ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer  )
{
	BYTE nStyle = 0;
	if( pLayer->GetLayerInfo()->GetLayerID() != 0 )
		nStyle = pLayer->GetLayerInfo()->GetPen().nStyle;
	BOOL bAnno = ( (dbScale<pLayer->GetLayerInfo()->GetMaxARScale()) && (dbScale>pLayer->GetLayerInfo()->GetMinARScale()) ) 
					|| (pLayer->GetLayerInfo()->GetMaxARScale() == -1);

	CPtrList *pObjList = NULL;
	pLayer->GetObjectListMap()->Lookup( nBID, pObjList );
	if( !pObjList )
		return;
//	PreparePen( pLayer->GetLayerInfo()->GetPen() );
//	PrepareBrush( pLayer->GetLayerInfo()->GetBrush() );
	POSITION pos = pObjList->GetHeadPosition();

	CGIS_Feature *pObj = NULL;
	while( pos )
	{
		pObj = (CGIS_Feature*)pObjList->GetNext( pos );
		if( pObj )
		{
			switch( pObj->GetObjType( ) )
			{
			case EN_GOTYPE_POINT:
				DrawPoint( (CGIS_FeaturePoint*)pObj, rtOld, rtDraw, dbScale, bAnno );
				break;
			case EN_GOTYPE_LINE:
				DrawLine( (CGIS_FeatureLine*)pObj, rtOld, rtDraw, dbScale, bAnno, nStyle );
				break;
			case EN_GOTYPE_POLY:
				if( pLayer->GetLayerInfo()->GetLayerID() == 3 )
					DrawWaterPoly( (CGIS_FeaturePoly*)pObj, rtOld, rtDraw, dbScale, bAnno );
				else
					DrawPoly( (CGIS_FeaturePoly*)pObj, rtOld, rtDraw, dbScale, bAnno );
				break;
			case EN_GOTYPE_RUT:
				DrawRut( (CGIS_FeatureRut*)pObj, rtOld, rtDraw, dbScale, bAnno ,pLayer->GetLayerInfo()->GetPen());
				break;
			default:
				ASSERT( FALSE );
				break;
			}
		}
	}
}
void  CGeoView::DrawBackRut( ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer )
{
	BYTE nStyle = 0;
	CPtrList *pObjList = NULL;
	pLayer->GetObjectListMap()->Lookup( nBID, pObjList );
	if( !pObjList )
		return;
	POSITION pos = pObjList->GetHeadPosition();
	CGIS_FeatureRut *pRut = NULL;
	while( pos )
	{
		pRut = (CGIS_FeatureRut*)pObjList->GetNext( pos );
		if( pRut )
		{
			int nClipType = GeoRectIntersect(&rtDraw, &pRut->GetRectObj());
			if(nClipType == 0)	// 没有交集
				continue;
			if( m_bMoveMap && GeoRectIntersect( &rtOld,&pRut->GetRectObj() ) == 2  && m_Angle == 0)
				continue;
			int nNum = 0;
			ST_GEO_PTXY* pPtList = NULL;
			if(nClipType == 2)
			{
				DrawLine( pRut->GetPtList(), pRut->GetPtNum() );
			}
			else
			{
				ST_GEO_RECT rtTemp;
				rtTemp.dBottom = rtDraw.dBottom + 0.002;
				rtTemp.dTop = rtDraw.dTop - 0.002;
				rtTemp.dLeft = rtDraw.dLeft - 0.002;
				rtTemp.dRight = rtDraw.dRight + 0.002;
				pPtList = GetClipRgn(pRut->GetPtList(), pRut->GetPtNum(), 
												  rtTemp, &nNum, EN_GOTYPE_RUT);
				if(nNum > 0 && pPtList )
				{
					DrawLine(pPtList, nNum);
//					delete[] pPtList;
				}
			}
		}
	}
}
void CGeoView::SetCarPar(	int nVehAngle,	BOOL bShowVehSymbol,int  nRadius,float  fHightScale)
{
	m_nVehAngle = nVehAngle;
	m_bShowVehSymbol = bShowVehSymbol;
	m_nRadius = nRadius;
	m_fHightScale = fHightScale;
}
void CGeoView::SetbReDrawCar( BOOL bRedrawCar )
{
	m_bReDrawCar = bRedrawCar;
}
void CGeoView::SetbDrawNetWorkRoute( BOOL bDrawNetWorkRoute )
{
	m_bDrawNetWorkRoute = bDrawNetWorkRoute;
}
BOOL  CGeoView::AddDrawPOI( ST_PT_DRAW *ptDraw )
{
	if( m_DrawPtList && ptDraw )
	{
		ST_PT_DRAW* pPtDraw = NULL;
		POSITION pos = m_DrawPtList->GetHeadPosition();
		while( pos )
		{
			pPtDraw = (ST_PT_DRAW*)m_DrawPtList->GetNext( pos );
			if( pPtDraw->nx == ptDraw->nx && pPtDraw->ny == ptDraw->ny )
				return FALSE;
		}
		m_DrawPtList->AddTail( ptDraw );
	}
	return TRUE;

}
void  CGeoView::ClearDrawPOI( )
{
	ST_PT_DRAW* pDrawPt;
	POSITION pos = m_DrawPtList->GetHeadPosition();
	while( pos )
	{
		POSITION ps = pos;
		pDrawPt = (ST_PT_DRAW*)m_DrawPtList->GetNext(pos);
		if(m_bMoveMap)
		{
			BOOL bRemove = TRUE;//是否需要删除该POI
			pDrawPt->nx += m_positionBMP.x;
			pDrawPt->ny += m_positionBMP.y;//待删除的poi在当前屏幕上则不删除
			if( IsPtInScreen( pDrawPt->nx,pDrawPt->ny,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom ) )
				bRemove = FALSE;
			if(bRemove)
			{
				delete pDrawPt;
				pDrawPt = NULL;
				m_DrawPtList->RemoveAt(ps);
			}
		}
		else
		{
			delete  pDrawPt;
			pDrawPt = NULL;
		}
	}

	if(!m_bMoveMap)
		m_DrawPtList->RemoveAll();
}
void  CGeoView::DrawPOI( )
{
	if( m_DrawPtList )
	{
		ST_PT_DRAW *ptDraw = NULL;
		POSITION pos = m_DrawPtList->GetTailPosition();
		while( pos )
		{
			ptDraw = ( ST_PT_DRAW* )m_DrawPtList->GetPrev( pos );
			if( ptDraw )
				DrawPoint( ptDraw->nx , ptDraw->ny , ptDraw->nIconID );
		}
	}
}
CGIS_FeatureRut *CGeoView::GetCurrentRoute()
{
	return m_LastRoat;
}
CGIS_FeatureRut *CGeoView::GetCurrentRouteCopy()
{
	WaitForSingleObject( g_pMatchEvent->m_hObject , INFINITE );
	g_pMatchEvent->ResetEvent();

	CGIS_FeatureRut* pRoadCoyp = NULL;
	if( m_LastRoat && m_LastRoat->GetRutID() != -1 )
		pRoadCoyp = (CGIS_FeatureRut*)(m_LastRoat->Clone());
	g_pMatchEvent->SetEvent();

	return pRoadCoyp;

}
void CGeoView::Setb3DView( BOOL b3D )
{
	m_b3D = b3D;
}
BOOL CGeoView::Getb3DView()
{
	return m_b3D;
}
BOOL CGeoView::GetbDrawByLayer()
{
	return m_bDrawByLayer;
}
void CGeoView::SetbDrawByLayer( BOOL bDrawByLayer )
{
	m_bDrawByLayer = bDrawByLayer;
}
CPoint CGeoView::GetRotateCenter()
{
	return m_ptRotateCenter;
}
void CGeoView::SetRotateCenter( CPoint ptRotateCenter )
{
	m_ptRotateCenter = ptRotateCenter;
}
CPoint CGeoView::GetZoomCenter()
{
	return m_ptZoomCenter;
}
void CGeoView::SetZoomCenter( CPoint ptZoomCenter )
{
	m_ptZoomCenter = ptZoomCenter;
}
void CGeoView::DrawTurnSign( CDC* pDC )
{
	if( !m_bNav && !m_posTurnList)
		return;
	if( !m_pNetworkRoute )
		return;
	if( m_ScaleCurLevel > 4 )
		return;
	CGIS_NetworkPath *pPath = m_pNetworkRoute->GetNetworkPath();
	if( !pPath )
		return;
	POSITION pos;
	if( m_posTurnList )
		pos = m_posTurnList;
	else
		pPath->GetTLPosition( pos );
	if( !pos )
		return;
	CPtrList* ptTurnList = pPath->GetPathTurnList();
	if( !ptTurnList )
		return;
	ST_TURN_INFO *pTurnInfo = (ST_TURN_INFO*)ptTurnList->GetAt( pos );
	if( !pTurnInfo )
		return;
	if( pTurnInfo->byTDir == 5 )
		return;
	POSITION posStart = pTurnInfo->psPos;
	POSITION posEnd = pTurnInfo->psEndMobilePos;
	CPtrList *ptList = pPath->GetPathpointList();
	if( posStart && ptList )
	{
		if( !posEnd )
			posEnd = posStart;
		CGIS_Pathpoint* ptStart = (CGIS_Pathpoint*)ptList->GetAt( posStart );
		CGIS_Pathpoint* ptEnd = (CGIS_Pathpoint*)ptList->GetAt( posEnd );
		if( ptStart && ptEnd )
		{
			POSITION posPreTemp = posStart;
			CGIS_Pathpoint* ptPreTemp1 = NULL;
			CGIS_Pathpoint* ptPreTemp = (CGIS_Pathpoint*)ptList->GetPrev( posPreTemp );

			//需要绘制的坐标点链表
			CPtrList listPtDraw;
			ST_GEO_PTXY *ptXY = new ST_GEO_PTXY;
			ptXY->dx = ptPreTemp->GetPathpointPosition().dx;
			ptXY->dy = ptPreTemp->GetPathpointPosition().dy;
			listPtDraw.AddTail( ptXY );

			int nPixDis = 0 ;
			int nRemainDis = 0;
			//机动点前需要绘制的点
			while( posPreTemp )
			{
				ptPreTemp1 = (CGIS_Pathpoint*)ptList->GetPrev( posPreTemp );
				nRemainDis = 25 - nPixDis;
				double dbOneDis = CalcDis( ptPreTemp->GetPathpointPosition().dx , ptPreTemp->GetPathpointPosition().dy ,
					 ptPreTemp1->GetPathpointPosition().dx , ptPreTemp1->GetPathpointPosition().dy) / m_dbScaleValue[m_ScaleCurLevel];
				nPixDis += dbOneDis;
				//控制长度为25个像素
				if(  nPixDis > 25  )
				{
//					double nTotalDis = GetLength( ptPreTemp->GetPathpointPosition().dx , ptPreTemp->GetPathpointPosition().dy ,
//						                            ptPreTemp1->GetPathpointPosition().dx,ptPreTemp1->GetPathpointPosition().dy );
					ptXY = new ST_GEO_PTXY;
					double dx = 0;
					double dy = 0 ;
					GetPointOnArc( ptPreTemp->GetPathpointPosition().dx , ptPreTemp->GetPathpointPosition().dy ,
						            ptPreTemp1->GetPathpointPosition().dx,ptPreTemp1->GetPathpointPosition().dy ,
									nRemainDis / dbOneDis ,dx ,dy );
					ptXY->dx = dx;
					ptXY->dy = dy;
					listPtDraw.AddTail( ptXY );
					break;
				}
				else
				{
					ptXY = new ST_GEO_PTXY;
					ptXY->dx = ptPreTemp1->GetPathpointPosition().dx;
					ptXY->dy = ptPreTemp1->GetPathpointPosition().dy;
					listPtDraw.AddTail( ptXY );
				}
				ptPreTemp = ptPreTemp1;
			
			}
			//机动点后需要绘制的点
			POSITION posAftTemp = posStart;
			CGIS_Pathpoint* ptAftTemp = NULL;
			CGIS_Pathpoint* ptAftTemp1 = NULL;
			if( posAftTemp != posEnd )
			{
				while( posAftTemp && posAftTemp != posEnd )
				{
					ptAftTemp = (CGIS_Pathpoint*)ptList->GetNext( posAftTemp );
					ptXY = new ST_GEO_PTXY;
					ptXY->dx = ptAftTemp->GetPathpointPosition().dx;
					ptXY->dy = ptAftTemp->GetPathpointPosition().dy;
					listPtDraw.AddHead( ptXY );
				}
				ptXY = new ST_GEO_PTXY;
				ptAftTemp = (CGIS_Pathpoint*)ptList->GetAt( posEnd );
				ptXY->dx = ptAftTemp->GetPathpointPosition().dx;
				ptXY->dy = ptAftTemp->GetPathpointPosition().dy;
				listPtDraw.AddHead( ptXY );
			}
			posAftTemp = posEnd;
			
			ptAftTemp = (CGIS_Pathpoint*)ptList->GetNext( posAftTemp );

			nPixDis = 0 ;
			nRemainDis = 0;
			//最后一个点的位置
			POSITION posDrawLast = NULL;
			int nLOng = 15;
			while( posAftTemp )
			{
				ptAftTemp1 = (CGIS_Pathpoint*)ptList->GetNext( posAftTemp );
				nRemainDis = nLOng - nPixDis;
				double dbOneDis = CalcDis( ptAftTemp->GetPathpointPosition().dx , ptAftTemp->GetPathpointPosition().dy ,
					 ptAftTemp1->GetPathpointPosition().dx , ptAftTemp1->GetPathpointPosition().dy) / m_dbScaleValue[m_ScaleCurLevel];

				if( nRemainDis < 6 )
				{
					if( dbOneDis < 6 )
						nLOng+=dbOneDis;
					else
						nRemainDis = 6;
				}
				nPixDis += dbOneDis ;

				//控制长度为15个像素
				if(  nPixDis > nLOng  )
				{
//					double nTotalDis = GetLength( ptAftTemp->GetPathpointPosition().dx , ptAftTemp->GetPathpointPosition().dy ,
//						                            ptAftTemp1->GetPathpointPosition().dx,ptAftTemp1->GetPathpointPosition().dy );
					ptXY = new ST_GEO_PTXY;
					double dx,dy;
					GetPointOnArc( ptAftTemp->GetPathpointPosition().dx , ptAftTemp->GetPathpointPosition().dy ,
						            ptAftTemp1->GetPathpointPosition().dx,ptAftTemp1->GetPathpointPosition().dy ,
									nRemainDis / dbOneDis ,dx ,dy );
					ptXY->dx = dx;
					ptXY->dy = dy;
					listPtDraw.AddHead( ptXY );
					posDrawLast = posAftTemp;
					break;
				}
				else
				{
					ptXY = new ST_GEO_PTXY;
					ptXY->dx = ptAftTemp1->GetPathpointPosition().dx;
					ptXY->dy = ptAftTemp1->GetPathpointPosition().dy;
					listPtDraw.AddHead( ptXY );
				}
				ptAftTemp = ptAftTemp1;
			}
			//三角形
			ST_GEO_PTXY *pt1 = NULL;
			POSITION posFind = listPtDraw.GetHeadPosition();
			ST_GEO_PTXY *pt = (ST_GEO_PTXY*)listPtDraw.GetNext( posFind );
			int nx,ny;
			XyMapToView( pt->dx,pt->dy,nx,ny );
			double nWayAngle = 0;
			while( posFind )
			{
				pt1 = (ST_GEO_PTXY*)listPtDraw.GetNext( posFind );
				if( fabs(pt->dx - pt1->dx) > FLOAT_ZERO || fabs(pt->dy - pt1->dy) > FLOAT_ZERO )
				{
					nWayAngle = CalcAng( pt1->dx,pt1->dy,pt->dx,pt->dy );
					break;
				}
			}
			nWayAngle =   5*PI / 2.0 + nWayAngle - m_Angle * PI / 180.0;
			double sina = sin( nWayAngle );
			double cosa = cos( nWayAngle );

			short  nVehW = 10;
			short  nVehH = 8;

			POINT*  ptPoly = new POINT[3];
		
			ptPoly[0].x =  (LONG)((-nVehW) * cosa + (nVehH) * sina);
			ptPoly[0].y =  (LONG)(-(-nVehW) * sina + (nVehH) * cosa);
			ptPoly[1].x =  (LONG)((0) * cosa + (-nVehH ) * sina);
			ptPoly[1].y =  (LONG)(-(0) * sina + (-nVehH ) * cosa);
			ptPoly[2].x =  (LONG)((nVehW) * cosa + (nVehH) * sina);
			ptPoly[2].y =  (LONG)(-(nVehW) * sina + (nVehH) * cosa);
			int i;
			for(i = 0; i < 3; i++ )
			{
				ptPoly[i].x += nx;
				ptPoly[i].y += ny;
			}
			//画线
			CPen pen( PS_SOLID , 8 , RGB(0,200,0));
			CPen *pOldPen = pDC->SelectObject( &pen );
			
			POSITION posLine = listPtDraw.GetHeadPosition();
			POINT* ptLine = new POINT[listPtDraw.GetCount()];
			ST_GEO_PTXY* ptMapLine = NULL;
			for( i = 0 ; i < listPtDraw.GetCount() ; i++ )
			{
				ptMapLine = (ST_GEO_PTXY*)listPtDraw.GetNext( posLine );
				XyMapToView( ptMapLine->dx , ptMapLine->dy , nx , ny );
				ptLine[i].x = nx;
				ptLine[i].y = ny;
			}
			pDC->Polyline( ptLine , listPtDraw.GetCount() );
			pDC->SelectObject( pOldPen );
			pen.DeleteObject();
			//画三角
			CPen pen1(PS_SOLID , 2 , RGB(0,200,0) );
			pOldPen = pDC->SelectObject( &pen1 );
			CBrush brush(RGB(0,0,0));
			CBrush *pOldBrush = pDC->SelectObject( &brush );

			pDC->Polygon( ptPoly , 3 );
			posLine = listPtDraw.GetHeadPosition();
			pDC->SelectObject( pOldBrush );
			pDC->SelectObject( pOldPen );
			pen1.DeleteObject();
			brush.DeleteObject();
			//////////////////////////////////////////////////////////////////////////
			//画线
			CPen pen2( PS_SOLID , 6 , RGB(255,255,255));
			CPen *pOldPen2 = pDC->SelectObject( &pen2 );
			pDC->Polyline( ptLine , listPtDraw.GetCount() );
			pDC->SelectObject( pOldPen2 );
			pen2.DeleteObject();
			//画三角
			CPen pen3(PS_SOLID , 0 , RGB(255,255,255) );
			pOldPen2 = pDC->SelectObject( &pen3 );
			CBrush brush1(RGB(255,255,255));
			CBrush *pOldBrush1 = pDC->SelectObject( &brush1 );

			pDC->Polygon( ptPoly , 3 );
			pDC->SelectObject( pOldBrush1 );
			pDC->SelectObject( pOldPen2 );
			pen3.DeleteObject();
			brush1.DeleteObject();


			//清理内存
			delete[] ptLine;
			delete[] ptPoly;
			while( posLine )
			{
				pt = (ST_GEO_PTXY*)listPtDraw.GetNext( posLine );
				delete pt;
				pt = NULL;
			}

		}
	}
}
void CGeoView::SetbNav( BOOL bNav )
{
	m_bNav = bNav;
}
BOOL CGeoView::GetbNav()
{
	return m_bNav;
}
POSITION CGeoView::GetTurnListPos( )
{
	return m_posTurnList;

}
void CGeoView::SetTurnListPos( POSITION posTurnList )
{
	m_posTurnList = posTurnList;
}
void CGeoView::ShowCursorFind( CDC* pShowDC )
{
	m_strFindName = L"";
	if( m_pFeatureTip )
	{
		delete m_pFeatureTip;
		m_pFeatureTip = NULL;
	}
	double dx ,dy ;
	XyViewToMap( m_TipPt.x , m_TipPt.y , dx , dy );
	m_ptTipXy.dx = dx;
	m_ptTipXy.dy = dy;
	if( dScale < MAPSECLUDE_SCALE )
		return ;
	POSITION pos = m_MapSet->GetHeadPosition();
	CGIS_GeoMap *pMap = NULL;
	while( pos )
	{
		pMap = (CGIS_GeoMap*)m_MapSet->GetNext( pos );
		if( pMap->GetMapInfo()->GetMapID() == 2 )
			break;
	}
	CGIS_QueryByShape *pQueryBySharp = (CGIS_QueryByShape*)pMap->GetTool( QUERY_BYSHARP );
	CGIS_MapLayer *pBLList[LAYER_PT_END+1];
	int nCount = 0;
	GetMSLayer( pMap, dScale, (CGIS_MapLayer**)(pBLList), nCount );
	CGIS_MapLayer* pLayer = NULL;
	int nWidth;
	ST_GEO_PTXY ptXy;
	ptXy.dx = dx;
	ptXy.dy = dy;
	int nxNum , nyNum ;
	CalculateBMapIDInfo(  ptXy.dx , ptXy.dy , nxNum , nyNum );
	int nBandID =  CalculateBMapID( nxNum , nyNum );
	CGIS_BreadthMap *pBMap = pMap->GetBreadthMapByID( nBandID );
	if( !pBMap )
		return;
	CPtrList *pObjList = NULL;
	CPtrList listReturn;
	double dbDis;
	//最终找到的地物
	CGIS_Feature *pFeatureFind = NULL;
	//最终显示的名称
	swString strFind;
	strFind.Empty();
	//是否找到
	BOOL bFindRut = FALSE;
	int i;
	for( i = nCount-1 ; i >= 0  ; i-- )
	{
		pLayer = pBLList[i];
		CGIS_LayerInfo* pLayerInfo =  pLayer->GetLayerInfo();
		//道路优先
		if( pLayerInfo->GetLayerID() > 11 )
			continue;
		ST_GEO_PEN pen = pLayerInfo->GetPen();
		//各个道路层的最近阀值
		if( m_ScaleCurLevel < 5 )
		{
			if( m_ScaleCurLevel == 4 || m_ScaleCurLevel == 3 )
				nWidth = pen.nWidth;
			else
				nWidth = (4 - m_ScaleCurLevel)*2 + pen.nWidth;
		}
		else
		{
			if( pen.nWidth > 2 )
				nWidth = pen.nWidth - 2 ;
			else
				nWidth = pen.nWidth;
			if( m_ScaleCurLevel < 3)
				nWidth = (4 - m_ScaleCurLevel)*2 + pen.nWidth;
		}
		dbDis = nWidth*m_dbScaleValue[m_ScaleCurLevel-1];
		pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pObjList );
		if( !pObjList )
			continue;
		pQueryBySharp->QueryByBuffer( listReturn , &ptXy , pObjList , dbDis , EN_OBJ_RUT );
        
		if( listReturn.GetCount() > 0 )
		{
			bFindRut = TRUE;
			pFeatureFind = (CGIS_Feature*)listReturn.GetHead();
			if( pFeatureFind && pFeatureFind->GetAnnoNum() != 0)
				CharToUnicode( pFeatureFind->GetAnnotation() , &strFind );
//				strFind = (swString)pFeatureFind->GetAnnotation();
			else
				strFind = L"路";
			m_pFeatureTip = ( (CGIS_FeatureRut*)pFeatureFind )->Clone();
//////////////////////////////////////////////////////////////////////////
#if 1
#ifdef _DEBUG
#ifndef _WIN32_WCE
            {
                CGIS_FeatureRut* pFeatureRoadSection = (CGIS_FeatureRut*)m_pFeatureTip;
                if(pFeatureRoadSection->GetRutID() == -1)
                {
                    strFind += _T("(禁行路段)");
                } 
                else
                {
                    //取得SDE控制器
                    CSDEControler* pSdeControl = CSDEControler::GetSDEControl();
                    ASSERT(pSdeControl);
                    //取得航线计算工具
                    IGIS_NetWork *pNetwork = (IGIS_NetWork*)pSdeControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );
                    ASSERT(pNetwork);
                    
                    int nRoadSectionLayer;
                    CGIS_NetworkRut* pNetworkRut = pNetwork->GetRoadSectionByBreadthIDAndRoadSectionID(pBMap->m_nBreadthID,((CGIS_FeatureRut*)m_pFeatureTip)->GetRutID(),nRoadSectionLayer);
                    ASSERT(pNetworkRut);
                    int nStartVertexLayer;
                    int nEndVertexLayer;
                    
                    swString strAngleStart;
                    CGIS_NetworkVtx* pVertexStart = pNetwork->GetVertexByBreadthIDAndVertexID(pBMap->m_nBreadthID,pNetworkRut->GetRutStartVtxID(),nStartVertexLayer);
					int i;
                    for(i = 0; i < pVertexStart->GetVRutNum(); i++)
                    {
                        swString strTemp;
                        strTemp.Format(_T("%d-%d,"),pVertexStart->GetVRutList()[i],pVertexStart->GetVRTurnList()[i]);
                        strAngleStart += strTemp;
                    }
                    swString strAngleEnd;
                    CGIS_NetworkVtx* pVertexEnd = pNetwork->GetVertexByBreadthIDAndVertexID(pBMap->m_nBreadthID,pNetworkRut->GetRutEndVtxID(),nEndVertexLayer);
                    for(i = 0; i < pVertexEnd->GetVRutNum(); i++)
                    {
                        swString strTemp;
                        strTemp.Format(_T("%d-%d,"),pVertexEnd->GetVRutList()[i],pVertexEnd->GetVRTurnList()[i]);
                        strAngleEnd += strTemp;
                    }
                    
                    swString strV7Start;
                    swString strV7End;
                    swString strV7AngleStart;
                    swString strV7AngleEnd;
//                    CGIS_NetworkVertexV7* pVertexV7Start = pNetwork->GetTrunkVertexByBreadthIDAndVertexID(pMap, pVertexStart->GetVBreadthID(),pVertexStart->GetVtxID());
//                    CGIS_NetworkVertexV7* pVertexV7End = pNetwork->GetTrunkVertexByBreadthIDAndVertexID(pMap,pVertexEnd->GetVBreadthID(),pVertexEnd->GetVtxID());
//                    {
//                        
//                        if(pVertexV7Start)
//                        {
//                            CGIS_NetworkVertexV7* pVertexV7 = pVertexV7Start;
//                            
//                            strV7Start.Format(_T("SVM(Y):ID[%d]RN[%d]LRN[%d]"),
//                                pVertexV7->m_stData.m_nID,
//                                pVertexV7->m_stData.m_nRoadSectionNum,
//                                pVertexV7->m_stData.m_nLowLayerRoadSectionNum);
//                            {
//                                for(int i = 0; i < pVertexV7->GetRoadSectionNum(); i++)
//                                {
//                                    swString strTemp;
//                                    strTemp.Format(_T("%d-%d,"),pVertexV7->GetRoadSectionList()[i],pVertexV7->GetRoadSectionTurnList()[i]);
//                                    strV7AngleStart += strTemp;
//                                }
//                                strV7AngleStart += _T("Low:");
//                                for( i = 0; i < pVertexV7->GetLowLayerRoadSectionNum() ; i++)
//                                {
//                                    swString strTemp;
//                                    strTemp.Format(_T("%d,"),pVertexV7->m_pLowLayerRoadSectionTurnList[i]);
//                                    strV7AngleStart += strTemp;
//                                }
//                            }
//                        }
//                        else
//                        {
//                            strV7Start.Format(_T("SVM(N)"));
//                        }
//                        if(pVertexV7End)
//                        {
//                            CGIS_NetworkVertexV7* pVertexV7 = pVertexV7End;
//                            
//                            strV7End.Format(_T("EVM(Y):ID[%d]RN[%d]LRN[%d]"),
//                                pVertexV7->m_stData.m_nID,
//                                pVertexV7->m_stData.m_nRoadSectionNum,
//                                pVertexV7->m_stData.m_nLowLayerRoadSectionNum);
//                            
//                            {
//                                for(int i = 0; i < pVertexV7->GetRoadSectionNum(); i++)
//                                {
//                                    swString strTemp;
//                                    strTemp.Format(_T("%d-%d,"),pVertexV7->GetRoadSectionList()[i],pVertexV7->GetRoadSectionTurnList()[i]);
//                                    strV7AngleEnd += strTemp;
//                                }
//                                strV7AngleEnd += _T("Low:");
//                                for( i = 0; i < pVertexV7->GetLowLayerRoadSectionNum() ; i++)
//                                {
//                                    swString strTemp;
//                                    strTemp.Format(_T("%d,"),pVertexV7->m_pLowLayerRoadSectionTurnList[i]);
//                                    strV7AngleEnd += strTemp;
//                                }
//                            }
//                        }
//                        else
//                        {
//                            strV7End.Format(_T("EVM(N)"));
//                        }
//                    }
//                    
                    
                    swString strLimit;
                    swString strReverseLimit ;
                    CGIS_NetworkRut::stuRoadSectionLimitInfo* pLimitInfo = pNetworkRut->GetLimitInfo();
                    if(pLimitInfo)
                    {
                         pLimitInfo->Format(TRUE,0,strLimit);
                         pLimitInfo->Format(FALSE,0,strReverseLimit);
                         
                    }

					swString strSignPost = _T("");
					{
						
						for(int i = 0; i < pFeatureRoadSection->GetSignInfoCount(); i++)
						{
							CGIS_FeatureRut::LPSIGNINFO pSign = &pFeatureRoadSection->GetSignInfoArray()[i];
							swString strText;
							CharToUnicode(pSign->pszName, &strText);
							swString strText2;
							strText2.Format(_T("[%d-%s]"),pSign->wOutRutID,strText);
							strSignPost += strText2;
						}
					}

                    swString strTemp;
                    strTemp.Format(
                        _T("\n ID:%d(%d)\n Lay:%d\n Lvl:%d\n Typ:%d\n Dir:%d\n Chg:%d\n Len:%.2fm\n S_Vtx:%d-eg%d\n E_Vtx:%d-eg%d\n Lmt:%s\n Rlmt:%s\n SVA[%s]\n EVA[%s]\n %s\n %s\n SVMA:%s\n EVMA:%s\n SignPost:%s"),
                        pNetworkRut->GetRutID(),
                        pBMap->m_nBreadthID,
                        nRoadSectionLayer,
                        pNetworkRut->GetType(),
                        pNetworkRut->GetRutType(),
                        pNetworkRut->GetRutDirection(),
                        pNetworkRut->GetRutCharge(),
                        pNetworkRut->GetRutLength(),
                        pNetworkRut->GetRutStartVtxID(),
                        pVertexStart->GetVtxEdge(),
                        pNetworkRut->GetRutEndVtxID(),
                        pVertexEnd->GetVtxEdge(),
                        strLimit,
                        strReverseLimit,
                        strAngleStart,
                        strAngleEnd,
                        strV7Start,
                        strV7End,
                        strV7AngleStart,
                        strV7AngleEnd,
						strSignPost);
                    strFind += strTemp;
                    
                    pNetworkRut->DestoryCloneObject();
                    pVertexStart->DestoryCloneObject();
                    pVertexEnd->DestoryCloneObject();
                    
//                    if(pVertexV7Start)
//                    {
//                        pVertexV7Start->DestoryCloneObject();
//                    }
//                    if(pVertexV7End)
//                    {
//                        pVertexV7End->DestoryCloneObject();
//                    }
                    
            }
            }
#endif
#endif
#endif
//////////////////////////////////////////////////////////////////////////
                
			m_TipFeaType = EN_OBJ_RUT;
			break;
		}
	}
	BOOL bFindPOI = FALSE;
	//查找POI
	if( !bFindRut )
	{
		for( i = nCount-1 ; i >= 0  ; i-- )
		{
			pLayer = pBLList[i];
			CGIS_LayerInfo* pLayerInfo =  pLayer->GetLayerInfo();
			//POI优先
			if( pLayerInfo->GetLayerID() < 12 )
				break;
			dbDis = 10*m_dbScaleValue[m_ScaleCurLevel-1];
			pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pObjList );
			if( !pObjList )
				continue;
			pQueryBySharp->QueryByBuffer( listReturn , &ptXy , pObjList , dbDis , EN_OBJ_PT );
			if( listReturn.GetCount() > 0 )
			{
				bFindPOI = TRUE;
				pos = listReturn.GetHeadPosition();
				ST_FEATURE_POINT *pTempPt = NULL;
				while( pos )
				{
					pTempPt = (ST_FEATURE_POINT*)listReturn.GetNext( pos );
					if( pTempPt )
						pFeatureFind = pTempPt->pFeature;
					if( pFeatureFind && pFeatureFind->GetAnnoNum() != 0 )
					{
						CharToUnicode( pFeatureFind->GetAnnotation() , &strFind );
//						strFind = (swString)pFeatureFind->GetAnnotation();
						m_pFeatureTip =( (CGIS_FeaturePoint*)pFeatureFind )->Clone();
						m_TipFeaType = EN_OBJ_PT;
						break;
					}
				}
				break;
			}
		}
	}
	m_strFindName = strFind;
	ReSetCursor();
	
	//若找到，则绘制
	if( !strFind.IsEmpty() )
	{
		swRect rect;
		pShowDC->DrawText(strFind, &rect, DT_CALCRECT);
		int nx = m_TipPt.x+16;
		int ny = m_TipPt.y-16;
		swRect r( nx, ny , nx + rect.Width(), ny + rect.Height());

		int nW = int((r.right - r.left) / 2);
		int nH = int((r.bottom - r.top) / 2);
	
		swRect rtTaget = r;
		BOOL bFindRight = FALSE;
		for (int i = 0; i < 8; i ++)
		{
			if( IsPtInScreen( rtTaget.left,rtTaget.top,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom)
				&& IsPtInScreen( rtTaget.left,rtTaget.bottom,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom)
				&& IsPtInScreen( rtTaget.right,rtTaget.top,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom)
				&& IsPtInScreen( rtTaget.right,rtTaget.bottom,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom)
				)
			{
				bFindRight = TRUE;
				break;
			}
			switch(i)
			{
				case 0 :
				case 1 : 
					rtTaget.top += (nH + Y_OFF);
					rtTaget.bottom += (nH + Y_OFF);
					break;
				case 2 :
				case 3 :
					rtTaget.left -= (nW + X_OFF);
					rtTaget.right -= (nW + X_OFF);
					break;
				case 4 :
				case 5 : 
					rtTaget.top -= (nH + Y_OFF);
					rtTaget.bottom -= (nH + Y_OFF);
					break;
				case 6 : 
				case 7 : 
					rtTaget.left += (nW + X_OFF);
					rtTaget.right += (nW + X_OFF);
					break;
			}
		}
		if( bFindRight )
			r = rtTaget;

//		ST_GEO_PEN pen;
//		pen.nWidth = 1;
//		pen.nColor = RGB( 0,0,0 );
//		pen.nStyle = PS_SOLID;
//		ST_GEO_BRUSH brush;
//		brush.nForeColor = RGB( 255,255,255 );
//		PreparePen( pen );
//		PrepareBrush( brush );

		CPen pPen,*pPenOld;
		pPen.CreatePen(PS_SOLID, 1, RGB( 0,0,0 ) );
		pPenOld = (CPen*)pShowDC->SelectObject(&pPen);

		CBrush pBrush,*pBrushOld;
		pBrush.CreateSolidBrush( RGB( 255,255,255 ) );
		pBrushOld = (CBrush*)pShowDC->SelectObject( &pBrush );



		pShowDC->Rectangle( r );
		pShowDC->SetTextColor( RGB( 0,0,0) );
		pShowDC->DrawText( strFind , r , DT_NOCLIP );
//		ReleaseBrush();
//		ReleasePen();

		pShowDC->SelectObject( pPenOld );
		pPen.DeleteObject();

		pShowDC->SelectObject( pBrushOld );
		pBrush.DeleteObject();
		
	}
	//清理内存
	pos = listReturn.GetHeadPosition();
	CGIS_FeatureRut *pRut;
	ST_FEATURE_POINT *pPOI;
	while( pos )
	{
		if( bFindRut )
		{
			pRut = (CGIS_FeatureRut*)listReturn.GetNext( pos );
			if( pRut )
			{
				delete pRut;
				pRut = NULL;
			}
		}
		else
		{
			pPOI = (ST_FEATURE_POINT*)listReturn.GetNext( pos );
			if( pPOI )
			{
				delete pPOI->pFeature;
				delete pPOI;
				pPOI = NULL;
			}
		}
	}
	listReturn.RemoveAll();

}
void CGeoView::ReSetCursor()
{
	double dx,dy;
	XyViewToMap(m_TipPt.x , m_TipPt.y , dx, dy);
	if( m_pFeatureTip )
	{
		ST_GEO_PTXY ptCur;
		ptCur.dx = dx;
		ptCur.dy = dy;
		if( m_TipFeaType == EN_OBJ_RUT )
		{
			double dbDis = 10*m_dbScaleValue[m_ScaleCurLevel-1];
			CGIS_FeatureRut *pFeatureRutTemp = (CGIS_FeatureRut*)m_pFeatureTip;
			ST_GEO_PTXY ptPre = {0, 0};
			double dbDisToTaget = 10000000;
			for( int i = 0 ; i < (pFeatureRutTemp->GetPtNum()-1) ; i++ )
			{
				ST_GEO_RECT TagetRect;
				double dis = dbDis / 1852.0 / 60.0;
				TagetRect.dLeft = ptCur.dx - dis;
				TagetRect.dRight = ptCur.dx + dis;
				TagetRect.dBottom = ptCur.dy + dis;
				TagetRect.dTop = ptCur.dy - dis;
				NormalizeRect(TagetRect);
				ST_GEO_PTXY ptTemp1,ptTemp2;
				ptTemp1 = pFeatureRutTemp->GetPtList()[i];
				ptTemp2 = pFeatureRutTemp->GetPtList()[i+1];
				if( fabs(ptTemp1.dx-ptTemp2.dx)>FLOAT_ZERO || fabs(ptTemp1.dy-ptTemp2.dy)>FLOAT_ZERO )
				{
					ST_GEO_RECT rtTemp;
					rtTemp.dLeft = min(ptTemp1.dx,ptTemp2.dx);
					rtTemp.dRight = max(ptTemp1.dx,ptTemp2.dx);
					rtTemp.dTop = min(ptTemp1.dy,ptTemp2.dy);
					rtTemp.dBottom = max(ptTemp1.dy,ptTemp2.dy);
					if( GeoRectIntersect( &rtTemp,&TagetRect ) == 0)
						continue;
				}
				ST_GEO_PTXY ptPreTemp;
				DisPointToLine( ptCur , ptTemp1 , ptTemp2 , ptPreTemp );
				double dbPtDis = CalcDis( ptCur.dx , ptCur.dy , ptPreTemp.dx , ptPreTemp.dy );
				if(  dbPtDis < dbDisToTaget)
				{
					ptPre = ptPreTemp;
					dbDisToTaget = dbPtDis;
				}
			}
			int nx , ny;
			XyMapToView( ptPre.dx , ptPre.dy , nx , ny );
			m_ptTipXy = ptPre;
			m_TipPt.x = nx;
			m_TipPt.y = ny;
		}
		else if( m_TipFeaType == EN_OBJ_PT )
		{
			int nx ,ny;
			XyMapToView( m_pFeatureTip->GetPtList()->dx , m_pFeatureTip->GetPtList()->dy,
				nx,ny);
			m_TipPt.x = nx;
			m_TipPt.y = ny;
			m_ptTipXy = *(m_pFeatureTip->GetPtList());
		}
	}
	else
	{
		m_ptTipXy.dx = dx;
		m_ptTipXy.dy = dy;
	}
}

void CGeoView::ShowFindTip( CPoint pPt )
{
	m_TipPt = pPt;
	m_bShowTip = TRUE;
}
CPoint CGeoView::GetFindTip( )
{
	return m_TipPt;
}
ST_GEO_PTXY CGeoView::GetFindXyTip()
{
	return m_ptTipXy;
}

void CGeoView::DrawMapCompass( CDC* pCompassDC )
{
	if( !m_bShowMapCompass )
		return;
	int nAngleID = ( m_Angle / 15.0 + 0.5 );
	if( nAngleID > 23 || nAngleID < 0 )
		nAngleID = 0;
	
	m_CompassImageList.Draw( pCompassDC, nAngleID, m_ptMapCompass, ILD_TRANSPARENT );

}
void CGeoView::SetbShowCompass( BOOL bShowCompass )
{
	m_bShowMapCompass = bShowCompass;
}
void CGeoView::SetCompassPs( CPoint psCompass )
{
	m_ptMapCompass = psCompass;
}
swString CGeoView::GetstrFind( )
{
	return m_strFindName;
}
void CGeoView::ShowCursorFindRut( CDC* pShowDC )
{
	m_FocusRut.strName.Empty();
	m_FocusRut.nMapID = 2;
	double dx ,dy ;
	XyViewToMap( m_TipPt.x , m_TipPt.y , dx , dy );
	m_ptTipXy.dx = dx;
	m_ptTipXy.dy = dy;
	if( dScale < MAPSECLUDE_SCALE )
		return ;
	if( m_pFeatureTip )
	{
		delete m_pFeatureTip;
		m_pFeatureTip = NULL;
	}
	POSITION pos = m_MapSet->GetHeadPosition();
	CGIS_GeoMap *pMap = NULL;
	while( pos )
	{
		pMap = (CGIS_GeoMap*)m_MapSet->GetNext( pos );
		if( pMap->GetMapInfo()->GetMapID() == 2 )
			break;
	}
	CGIS_QueryByShape *pQueryBySharp = (CGIS_QueryByShape*)pMap->GetTool( QUERY_BYSHARP );
	CGIS_MapLayer *pBLList[LAYER_PT_END+1];
	int nCount = 0;
	GetMSLayer( pMap, dScale, (CGIS_MapLayer**)(pBLList), nCount );
	CGIS_MapLayer* pLayer = NULL;
	int nWidth;
	ST_GEO_PTXY ptXy;
	ptXy.dx = dx;
	ptXy.dy = dy;
	int nxNum , nyNum ;
	CalculateBMapIDInfo(  ptXy.dx , ptXy.dy , nxNum , nyNum );
	int nBandID =  CalculateBMapID( nxNum , nyNum );
	CGIS_BreadthMap *pBMap = pMap->GetBreadthMapByID( nBandID );

	if( !pBMap )
		return;
	nBandID = pBMap->m_nBreadthID;
	CPtrList *pObjList = NULL;
	CPtrList listReturn;
	double dbDis;
	//最终找到的道路
	CGIS_FeatureRut *pFeatureFind = NULL;
	for( int i = nCount-1 ; i >= 0  ; i-- )
	{
		pLayer = pBLList[i];
		CGIS_LayerInfo* pLayerInfo =  pLayer->GetLayerInfo();
		//道路
		if( pLayerInfo->GetLayerID() > 11 )
			continue;
		ST_GEO_PEN pen = pLayerInfo->GetPen();
		//各个道路层的最近阀值
		if( m_ScaleCurLevel < 5 )
		{
			if( m_ScaleCurLevel == 4 || m_ScaleCurLevel == 3 )
				nWidth = pen.nWidth;
			else
				nWidth = (4 - m_ScaleCurLevel)*2 + pen.nWidth;
		}
		else
		{
			if( pen.nWidth > 2 )
				nWidth = pen.nWidth - 2 ;
			else
				nWidth = pen.nWidth;
			if( m_ScaleCurLevel < 3)
				nWidth = (4 - m_ScaleCurLevel)*2 + pen.nWidth;
		}
		dbDis = nWidth*m_dbScaleValue[m_ScaleCurLevel-1];
		pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pObjList );
		if( !pObjList )
			continue;
		pQueryBySharp->QueryByBuffer( listReturn , &ptXy , pObjList , dbDis , EN_OBJ_RUT );
		if( listReturn.GetCount() > 0 )
		{
			pFeatureFind = (CGIS_FeatureRut*)listReturn.GetHead();
			if( pFeatureFind && pFeatureFind->GetAnnoNum() != 0)
				CharToUnicode( pFeatureFind->GetAnnotation() , &m_FocusRut.strName );
//				m_FocusRut.strName = (swString)pFeatureFind->GetAnnotation();
			else
				m_FocusRut.strName = L"路";
			m_FocusRut.nBandID = nBandID;
			m_FocusRut.nRutID = pFeatureFind->GetRutID();

			m_pFeatureTip =( (CGIS_FeaturePoint*)pFeatureFind )->Clone();
			m_TipFeaType = EN_OBJ_RUT;
			break;
		}
	}
	//若找到，则绘制
	if( pFeatureFind )
	{
//		ST_GEO_PEN pen;
//		pen.nWidth = nWidth;
//		pen.nColor = RGB( 0,255,0 );
//		pen.nStyle = PS_SOLID;
//		PreparePen( pen );
		CPen pPen,*pPenOld;
		pPen.CreatePen(PS_SOLID, nWidth, RGB( 0,255,0 ) );
		pPenOld = (CPen*)pShowDC->SelectObject(&pPen);

		int nPtCount = pFeatureFind->GetPtNum();
		POINT *ptList = new POINT[nPtCount];
		for( int i = 0 ; i < nPtCount ; i++ )
		{
			int nx , ny;
			XyMapToView( pFeatureFind->GetPtList()[i].dx , pFeatureFind->GetPtList()[i].dy ,
							nx , ny );
			ptList[i].x = nx;
			ptList[i].y = ny;
		}
		pShowDC->Polyline( ptList , nPtCount );	
        
//////////////////////////////////////////////////////////////////////////
//根据lpx的建议,加入显示路段的通行方向的功能2007-4-18
#if 1
#ifdef _DEBUG
#ifndef _WIN32_WCE
        {
            CGIS_FeatureRut* pFeatureRoadSection = (CGIS_FeatureRut*)m_pFeatureTip;
            if( pFeatureRoadSection && pFeatureRoadSection->GetRutID() != -1)
            {
                //取得SDE控制器
                CSDEControler* pSdeControl = CSDEControler::GetSDEControl();
                ASSERT(pSdeControl);
                //取得航线计算工具
                IGIS_NetWork *pNetwork = (IGIS_NetWork*)pSdeControl->GetSDETool( EN_SDETOOLTYPE_NETWORK );
                ASSERT(pNetwork);
                
                int nRoadSectionLayer;
                CGIS_NetworkRut* pNetworkRut = pNetwork->GetRoadSectionByBreadthIDAndRoadSectionID(pBMap->m_nBreadthID,((CGIS_FeatureRut*)m_pFeatureTip)->GetRutID(),nRoadSectionLayer);
                ASSERT(pNetworkRut);
                int nStartVertexLayer;
                int nEndVertexLayer;               
                CGIS_NetworkVtx* pVertexStart = pNetwork->GetVertexByBreadthIDAndVertexID(pBMap->m_nBreadthID,pNetworkRut->GetRutStartVtxID(),nStartVertexLayer);
                CGIS_NetworkVtx* pVertexEnd = pNetwork->GetVertexByBreadthIDAndVertexID(pBMap->m_nBreadthID,pNetworkRut->GetRutEndVtxID(),nEndVertexLayer);
                
                ST_GEO_PTXY ptStart = pVertexStart->GetVtxPosition();
                ST_GEO_PTXY ptEnd = pVertexEnd->GetVtxPosition();
                ST_GEO_PTXY *pt     = NULL;
                ST_GEO_PTXY *pt1    = NULL;
                switch(pNetworkRut->GetRutDirection())
                {
                case 0:
                    break;
                case 1:
                    pt  =   &ptEnd;
                    pt1 =   &ptStart;
                    break;
                case 2:
                    pt = &ptStart;
                    pt1 = &ptEnd;
                    break;
                default:
                    break;
                }
                
                if(pt && pt1)
                {                    
                    if( fabs(pt->dx - pt1->dx) > FLOAT_ZERO || fabs(pt->dy - pt1->dy) > FLOAT_ZERO )
                    {
                        double nWayAngle = 0;
                        nWayAngle = CalcAng( pt1->dx,pt1->dy,pt->dx,pt->dy );
                        
                        
                        
                        nWayAngle =   5*PI / 2.0 + nWayAngle - m_Angle * PI / 180.0;
                        double sina = sin( nWayAngle );
                        double cosa = cos( nWayAngle );
                        
                        short  nVehW = 16;
                        short  nVehH = 10;
                        
                        POINT*  ptPoly = new POINT[3];
                        
                        ptPoly[0].x =  (LONG)((-nVehW) * cosa + (nVehH) * sina);
                        ptPoly[0].y =  (LONG)(-(-nVehW) * sina + (nVehH) * cosa);
                        ptPoly[1].x =  (LONG)((0) * cosa + (-nVehH ) * sina);
                        ptPoly[1].y =  (LONG)(-(0) * sina + (-nVehH ) * cosa);
                        ptPoly[2].x =  (LONG)((nVehW) * cosa + (nVehH) * sina);
                        ptPoly[2].y =  (LONG)(-(nVehW) * sina + (nVehH) * cosa);
                        
                        for(int i = 0; i < 3; i++ )
                        {
                            ptPoly[i].x += m_TipPt.x;
                            ptPoly[i].y += m_TipPt.y;
                        }
                        
                        pShowDC->Polyline( ptPoly , 3 );	
                    }
                }
            }
        }
#endif
#endif
#endif
//////////////////////////////////////////////////////////////////////////
//		ReleasePen();
		pShowDC->SelectObject(pPenOld);
		pPen.DeleteObject();

		delete[] ptList;

	}

	ReSetCursor();
	//清理内存
	pos = listReturn.GetHeadPosition();
	CGIS_FeatureRut *pRut;
	while( pos )
	{
		pRut = (CGIS_FeatureRut*)listReturn.GetNext( pos );
		if( pRut )
		{
			delete pRut;
			pRut = NULL;
		}
	}
	listReturn.RemoveAll();
}
void CGeoView::ShowFocusRut( CPoint pPt )
{
	m_TipPt = pPt;
	m_bShowFocusRut = TRUE;
}
BOOL CGeoView::GetFocusRut( ST_FOCUS_RUT &stFocusRut )
{
	stFocusRut = m_FocusRut;
	if( m_FocusRut.strName.IsEmpty() )
		return FALSE;
	return TRUE;
}
void CGeoView::StartSpecalRut( ST_FOCUS_RUT stSpecalRut )
{
	m_bShowSpecalRut = TRUE;
	if( m_pSpecialRut)
	{
		delete m_pSpecialRut;
		m_pSpecialRut = NULL;
	}
	//寻找符合条件道路的过程
	CGIS_GeoMap *pMap = NULL;
	POSITION pos = m_MapSet->GetHeadPosition();
	while( pos )
	{
		pMap = (CGIS_GeoMap*)m_MapSet->GetNext( pos );
		if( pMap->GetMapInfo()->GetMapID() == 2 )
			break;
	}
	
	CGIS_BreadthMap *pBMap = pMap->GetBreadthMapByID2( stSpecalRut.nBandID );
	if( !pBMap )
		return;
	CGIS_LayerManager *pLManager = NULL;
	pLManager = (CGIS_LayerManager*)pMap->m_LayerManagerSet->GetHead();
	CGIS_MapLayer *pLayer = NULL;
	CGIS_MapLayer *pBLList[LAYER_PT_END+1];

	CPtrList *pObjList = NULL;
	CGIS_FeatureRut *pFeatureRut = NULL;
	BOOL bFind = FALSE;
	
	for( int i = 5 ; i < 12   ; i++ )
	{
		IGIS_Layer* pTempLayer = NULL;
		pLManager->GetLayerListMap()->Lookup( i, pTempLayer );
		pLayer = (CGIS_MapLayer*)pTempLayer;
		pBLList[0] = pLayer;
		
		if( !pMap->LoadBLData( pBMap, pBLList, 1 ) )
			return ;
		pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID , pObjList );
		pos = pObjList->GetHeadPosition();
		CGIS_LayerInfo* pTempInfo = NULL;
		pTempInfo =  pLayer->GetLayerInfo();
		while( pos )
		{
			pFeatureRut = (CGIS_FeatureRut*)pObjList->GetNext( pos );
			if( pFeatureRut && pFeatureRut->GetRutID() == stSpecalRut.nRutID )
			{
				m_pSpecialRut = (CGIS_FeatureRut*)pFeatureRut->Clone();
				m_pSpRutLayerInfo = pTempInfo;
				bFind = TRUE;
				break;
			}
		}
		pLayer->DecreaseLCount( pBMap->m_nBreadthID );
		if( bFind )
			break;
	}

	//平移到该位置
	if( m_pSpecialRut )
	{
		double dx,dy;
		int nNum = m_pSpecialRut->GetPtNum() / 2;
		dx = m_pSpecialRut->GetPtList()[nNum].dx;
		dy = m_pSpecialRut->GetPtList()[nNum].dy;
		SetScale( dx,dy,1 );
		Refresh();
	}
}
void CGeoView::StopSpecalRut( )
{
	m_bShowSpecalRut = FALSE;
	if( m_pSpecialRut)
	{
		delete m_pSpecialRut;
		m_pSpecialRut = NULL;
	}
	
}
void CGeoView::ShowSpecialRut( CDC* pShowDC )
{
	if( !m_pSpecialRut || !m_pSpRutLayerInfo )
		return;
	if( dScale > m_pSpRutLayerInfo->GetMaxGRScale() ||
		dScale < m_pSpRutLayerInfo->GetMinGRScale() )
		return;
	int nWidth;
	ST_GEO_PEN pen = m_pSpRutLayerInfo->GetPen();
	if( m_ScaleCurLevel < 5 )
	{
		if( m_ScaleCurLevel == 4 || m_ScaleCurLevel == 3 )
			nWidth = pen.nWidth;
		else
			nWidth = (4 - m_ScaleCurLevel)*2 + pen.nWidth;
	}
	else
	{
		if( pen.nWidth > 2 )
			nWidth = pen.nWidth - 2 ;
		else
			nWidth = pen.nWidth;
		if( m_ScaleCurLevel < 3)
			nWidth = (4 - m_ScaleCurLevel)*2 + pen.nWidth;
	}
//	pen.nWidth = nWidth;
//	pen.nColor = RGB( 0,255,0 );
//	pen.nStyle = PS_SOLID;
//	PreparePen( pen );

	CPen pPen,*pPenOld;
	pPen.CreatePen(PS_SOLID, nWidth, RGB( 0,255,0 ) );
	pPenOld = (CPen*)pShowDC->SelectObject(&pPen);

	int nPtCount = m_pSpecialRut->GetPtNum();
	POINT *ptList = new POINT[nPtCount];
	for( int i = 0 ; i < nPtCount ; i++ )
	{
		int nx , ny;
		XyMapToView( m_pSpecialRut->GetPtList()[i].dx , m_pSpecialRut->GetPtList()[i].dy ,
						nx , ny );
		ptList[i].x = nx;
		ptList[i].y = ny;
	}
	pShowDC->Polyline( ptList , nPtCount );		
//	ReleasePen();
	pShowDC->SelectObject(pPenOld);
	pPen.DeleteObject();
	delete[] ptList;

	
}
void CGeoView::ClearBufData( BOOL bClearBuf )
{
	InterlockedExchange( &(m_bClearBuf) , bClearBuf );
}
int CGeoView::GetCarDirect()
{
	return m_nCarRunDirect;
}
short CGeoView::GetCarRunNextPt()
{
	return m_nCarRunNextPt;
}
int CGeoView::GetLocationCity( ST_GEO_PTXY ptCurrent )
{
	if( !m_MapSet )
		return 0;
	CGIS_GeoMap* pMap = NULL;
	POSITION pos = m_MapSet->GetHeadPosition();
	while( pos )
	{
		pMap = (CGIS_GeoMap*)m_MapSet->GetNext( pos );
		if( pMap && pMap->GetMapInfo()->GetMapID() == MAPID_MAP_DETAIL )
			break;
	}
	if( !pMap )
		return 0;

    CSpaceMemoryLock aLock;    

	int nBandID = CalculateBMapID( ptCurrent.dx , ptCurrent.dy );

	CGIS_BreadthMap *pBMap = pMap->GetBreadthMapByID( nBandID );
	if( pBMap )
	{
		ST_GEO_RECT rt = CalculateBMapRect( nBandID );
		if(  IsPtInRect(ptCurrent.dx , ptCurrent.dy , rt.dLeft, rt.dTop , rt.dRight , rt.dBottom ) )
		{
			CGIS_LayerManager *pLManager = NULL;
			pos = pMap->m_LayerManagerSet->GetHeadPosition();
			while( pos )
			{
				pLManager = (CGIS_LayerManager*)( pMap->m_LayerManagerSet->GetNext(pos) );
				if(pLManager != NULL)
				{
					POSITION pos1 = pLManager->GetLayerListMap()->GetStartPosition();
					IGIS_Layer *pLayer = NULL;	
					short    nKey;
					while(pos1 != NULL)
					{
						pLManager->GetLayerListMap()->GetNextAssoc( pos1, nKey, pLayer );
						if( pLayer )
						{
							int nLayerID = pLayer->GetLayerInfo()->GetLayerID();
							//道路图层
							if( nLayerID == 0 )
							{
								CGIS_MapLayer *pBLList[1];
								pBLList[0] = (CGIS_MapLayer*)pLayer;
								if( !pMap->LoadBLData( pBMap, pBLList, 1 ) )
									return 0;
								CPtrList *pOList = NULL;
								pLayer->GetObjectListMap()->Lookup( pBMap->m_nBreadthID, pOList );
								if( !pOList )
								{
									pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
									return 0;
								}
								POSITION posFeature = pOList->GetHeadPosition();
								CGIS_FeaturePoly* pPoly = NULL;
								while( posFeature )
								{
									pPoly = (CGIS_FeaturePoly*)pOList->GetNext( posFeature );
									if( pPoly->GetPolyShowClass() == 2 )
										continue;
									if( PointInPoly(  ptCurrent , pPoly ) )
									{
										int nCityID = pPoly->GetPolyID();
										pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
										return nCityID;
									}
								}
								pBLList[0]->DecreaseLCount( pBMap->m_nBreadthID );
								break;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
BOOL CGeoView::ReadDirect(int nStartx , int nStarty , int nEndx , int nEndy )
{
	int dx = (nEndx - nStartx);
	int dy = (nEndy - nStarty);
	if( dx > 0 && ( (dy>0) || (abs(dy) < dx) ) )
		return TRUE;
	else if( dx <= 0 && dy >0 && (dy>abs(dx)) )
		return TRUE;

	return FALSE;

}
void CGeoView::SetFocusPOI( ST_FOCUS_PT ptFocusInput )
{
	if( ptFocusInput.nSymbalID >=0 )
	{
		ST_FOCUS_PT* ptFocus = new ST_FOCUS_PT;
		ptFocus->ptXy = ptFocusInput.ptXy;
		ptFocus->nSymbalID = ptFocusInput.nSymbalID;
		ptFocus->strName = ptFocusInput.strName;
		m_FocusPtSet.AddTail( ptFocus );
	}

}
void CGeoView::DeleteFocusPOI()
{
	if( m_FocusPtSet.GetCount() > 0 )
	{
		POSITION pos = m_FocusPtSet.GetHeadPosition();
		ST_FOCUS_PT* ptFocus = NULL;
		while( pos )
		{
			ptFocus = (ST_FOCUS_PT*)m_FocusPtSet.GetNext(pos);
			if( ptFocus )
			{
				delete ptFocus;
				ptFocus = NULL;
			}
		}
		m_FocusPtSet.RemoveAll();
	}
}
void CGeoView::ShowFocusPOI(CDC* pShowDC)
{
	if( m_FocusPtSet.GetCount() > 0 )
	{
		pShowDC->SetBkMode(TRANSPARENT);
		POSITION pos = m_FocusPtSet.GetHeadPosition();
		ST_FOCUS_PT* ptFocus = NULL;
		int nx,ny;
		while( pos )
		{
			ptFocus = (ST_FOCUS_PT*)m_FocusPtSet.GetNext(pos);
			if( ptFocus )
			{
				if( ptFocus->strName.IsEmpty() )
					continue;
				POINT pt;
				XyMapToView( ptFocus->ptXy.dx , ptFocus->ptXy.dy , nx , ny );
				pt.x = nx - 8;
				pt.y = ny - 8;
				//04.11.11
				if(ptFocus->nSymbalID >= 0)
					ImageList.Draw(pShowDC, ptFocus->nSymbalID, pt, ILD_TRANSPARENT);
				swRect rect;
				pShowDC->DrawText(ptFocus->strName, &rect, DT_CALCRECT);
				nx += 16;
				ny -= 16;
				swRect r( nx, ny , nx + rect.Width(), ny + rect.Height());

				int nW = int((r.right - r.left) / 2);
				int nH = int((r.bottom - r.top) / 2);
			
				swRect rtTaget = r;
				BOOL bFindRight = FALSE;
				for (int i = 0; i < 8; i ++)
				{
					if( IsPtInScreen( rtTaget.left,rtTaget.top,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom)
						&& IsPtInScreen( rtTaget.left,rtTaget.bottom,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom)
						&& IsPtInScreen( rtTaget.right,rtTaget.top,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom)
						&& IsPtInScreen( rtTaget.right,rtTaget.bottom,plotRect.left,plotRect.right,plotRect.top,plotRect.bottom)
						)
					{
						bFindRight = TRUE;
						break;
					}
					switch(i)
					{
						case 0 :
						case 1 : 
							rtTaget.top += (nH + Y_OFF);
							rtTaget.bottom += (nH + Y_OFF);
							break;
						case 2 :
						case 3 :
							rtTaget.left -= (nW + X_OFF);
							rtTaget.right -= (nW + X_OFF);
							break;
						case 4 :
						case 5 : 
							rtTaget.top -= (nH + Y_OFF);
							rtTaget.bottom -= (nH + Y_OFF);
							break;
						case 6 : 
						case 7 : 
							rtTaget.left += (nW + X_OFF) ;
							rtTaget.right += (nW + X_OFF);
							break;
					}
				}
				if( bFindRight )
					r = rtTaget;

				CPen pPen,*pPenOld;
				pPen.CreatePen(PS_SOLID, 2, RGB( 0,0,0 ) );
				pPenOld = (CPen*)pShowDC->SelectObject(&pPen);

				CBrush pBrush,*pBrushOld;
				pBrush.CreateSolidBrush( RGB( 100,255,100 ) );
				pBrushOld = (CBrush*)pShowDC->SelectObject( &pBrush );



				swRect rtDraw;
				rtDraw.bottom = r.bottom+1;
				rtDraw.top = r.top-1;
				rtDraw.left = r.left-1;
				rtDraw.right = r.right +1;
				pShowDC->Rectangle( rtDraw );
				pShowDC->SetTextColor( RGB( 255,255,128) );
				pShowDC->DrawText( ptFocus->strName , r , DT_SINGLELINE|DT_NOCLIP );

				pShowDC->SelectObject( pPenOld );
				pPen.DeleteObject();

				pShowDC->SelectObject( pBrushOld );
				pBrush.DeleteObject();			
			}
		}
	}
}
void CGeoView::DrawRichPOI(CPtrList* pListRichPOi,ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw)
{
	if( !pListRichPOi )
		return;
	ST_RICHPOI_BANDSET* pRichPOIBand = NULL;
	RICHPOI_INFO* pRichPOIInfo = NULL;
	BOOL bInOld;
	POSITION pos = pListRichPOi->GetHeadPosition();
	while( pos )
	{
		pRichPOIBand = (ST_RICHPOI_BANDSET*)pListRichPOi->GetNext( pos );
		if( pRichPOIBand )
		{
			POSITION posInfo = pRichPOIBand->listRichPOI.GetHeadPosition();
			while( posInfo )
			{
				pRichPOIInfo = (RICHPOI_INFO*)(pRichPOIBand->listRichPOI.GetNext( posInfo ));
				if( pRichPOIInfo )
				{
					bInOld =  IsPtInRect(pRichPOIInfo->fLon,pRichPOIInfo->fLat, rtOld.dLeft, 
								  rtOld.dTop, rtOld.dRight, rtOld.dBottom);
					if(m_bMoveMap && bInOld && m_Angle == 0)
						continue;
					if( (IsPtInRect(pRichPOIInfo->fLon,pRichPOIInfo->fLat, rtDraw.dLeft, 
						  rtDraw.dTop, rtDraw.dRight, rtDraw.dBottom)) )
					{
						int nx,ny;
						XyMapToView( pRichPOIInfo->fLon,pRichPOIInfo->fLat, nx, ny );
						ST_PT_DRAW *ptDraw = new ST_PT_DRAW;
						ptDraw->nIconID = 12;
						ptDraw->nx = nx;
						ptDraw->ny = ny;
						if( m_DrawPtList && ptDraw )
							m_DrawPtList->AddHead( ptDraw );
						ST_GEO_PTXY pAnnXy;
						pAnnXy.dx = pRichPOIInfo->fLon;
						pAnnXy.dy = pRichPOIInfo->fLat;
						AddAnnotate( &pAnnXy, 1, pRichPOIInfo->strRichPoiName );
					}
					
				}

			}
		}
	}
}
void CGeoView::SetExpandCfg( ST_EXPAND_MAPSET ExpandSet )
{
	if( (ExpandSet.nWidth > m_ExpandMapCfg.nWidth) || (ExpandSet.nLength > m_ExpandMapCfg.nLength) )
	{
		if( m_pCrossBmp  )
		{
			CClientDC dc(this);
			m_pDCCross->SelectObject( m_pCrossBmpOld );
			m_pCrossBmp->DeleteObject();
			delete m_pCrossBmp;
			m_pCrossBmp = NULL;

			m_pCrossBmp = new CBitmap();
			m_pCrossBmp->CreateCompatibleBitmap(&dc, ExpandSet.nWidth,
										 ExpandSet.nLength );
			m_pCrossBmpOld = m_pDCCross->SelectObject( m_pCrossBmp );
		}
//		if( m_pCrossBmpWithoutCar  )
//		{
//			CClientDC dc(this);
//			m_pDCCrossWithoutCar->SelectObject( m_pCrossBmpWithoutCarOld );
//			m_pCrossBmpWithoutCar->DeleteObject();
//			delete m_pCrossBmpWithoutCar;
//			m_pCrossBmpWithoutCar = NULL;
//
//			m_pCrossBmpWithoutCar = new CBitmap();
//			m_pCrossBmpWithoutCar->CreateCompatibleBitmap(&dc, ExpandSet.nWidth,
//										 ExpandSet.nLength );
//			m_pCrossBmpWithoutCarOld = m_pDCCrossWithoutCar->SelectObject( m_pCrossBmpWithoutCar );
//		}
	}
		
	m_ExpandMapCfg = ExpandSet;

}
CDC* CGeoView::GetExpandMap( POSITION pos )
{
	if( !m_pNetworkRoute || !pos )
		return NULL;
	CPtrList *pPathList = m_pNetworkRoute->GetNetworkPath()->GetPathpointList();
	if( !pPathList  || pPathList->GetCount() == 0 )
		return NULL;

	CGIS_Pathpoint* pPathpoint= NULL;
	CGIS_PathpointNode* pPathpointNode = NULL;
	pPathpoint = (CGIS_Pathpoint*)pPathList->GetAt( pos );
	if( !pPathpoint|| pPathpoint->GetPathpointType() != EN_PATHPOINTTYPE_NODE )
		return NULL;
	pPathpointNode = (CGIS_PathpointNode*)pPathpoint;
	if( m_CrossPos == pos )
	{
		//绘制车辆位置
/*		m_pDCCross->BitBlt(0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength
			,m_pDCCrossWithoutCar,0,0,SRCCOPY );
		if( pPathpointNode->GetCrossStatus() == CGIS_PathpointNode::STATUS_GDI_DRAW )
		{
			CPoint ptDraw;
			int nx ,ny;
			ExpandMapToView( m_rtMap , m_dbExpandScale ,m_dbSinAngle ,
				m_dbConAngle , m_CurPt.dx , 
				 m_CurPt.dy , nx,ny );

			ptDraw.x = nx;
			ptDraw.y = ny;			
			ptDraw.x -= 20;
			ptDraw.y -= 20;
			m_CarImageList.Draw( m_pDCCross, 0, ptDraw, ILD_TRANSPARENT );
		}
		*/
		return m_pDCCross;
	}
	else
		m_CrossPos = pos;

	//路口扩大图
	if( pPathpointNode->GetCrossStatus() == CGIS_PathpointNode::STATUS_GDI_DRAW )
	{

		//填充背景
		CBrush brushBack( m_ExpandMapCfg.colBack );
		m_pDCCross->FillRect( &swRect(0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength ),&brushBack );
		brushBack.DeleteObject();

		//计算地图旋转的角度
		int nAngle;
		CPtrList DrawRoutePtList;
		POSITION posCurrent = pos;
		ST_GEO_PTXY ptCurrent;
		ST_GEO_PTXY ptPrev;
		double dbHeadup = 0.00005;
		ptCurrent = ((CGIS_Pathpoint*)pPathList->GetPrev(posCurrent))->GetPathpointPosition();
		ptPrev = ((CGIS_Pathpoint*)pPathList->GetAt(posCurrent))->GetPathpointPosition();
		while ( fabs(ptCurrent.dx - ptPrev.dx) < dbHeadup && 
				fabs(ptCurrent.dy - ptPrev.dy) < dbHeadup && 
				posCurrent != NULL )
		{
			ptPrev = ((CGIS_Pathpoint*)pPathList->GetPrev(posCurrent))->GetPathpointPosition();
		}
		nAngle = (int)(CalcAngle(ptCurrent.dx,ptCurrent.dy,ptPrev.dx, ptPrev.dy) * 180 / PI);
		nAngle = 360 - (int)( 450 - nAngle ) % 360;
		m_dbSinAngle = sin( PI*nAngle/180.0 );
		m_dbConAngle = cos( PI*nAngle/180.0 );
		
		//该路口扩大图所需绘制的经纬度范围
		m_rtMap = pPathpointNode->GetCRect();
		//经纬度和像素点的比例关系
		float fCenterx,fCentery;
		fCenterx = (m_rtMap.dRight + m_rtMap.dLeft) / 2;
		fCentery = (m_rtMap.dTop + m_rtMap.dBottom) / 2;
		double dbExpandx =m_ExpandMapCfg.nWidth / fabs(m_rtMap.dRight - m_rtMap.dLeft) ;
		double dbExpandy = m_ExpandMapCfg.nLength / fabs(m_rtMap.dBottom - m_rtMap.dTop) ;
		m_dbExpandScale = min( dbExpandy ,dbExpandx );
		if( dbExpandx > dbExpandy )
		{
			double dbtemp = dbExpandx/dbExpandy*(m_rtMap.dRight - m_rtMap.dLeft)/2;
			m_rtMap.dLeft = fCenterx - dbtemp;
			m_rtMap.dRight = fCenterx + dbtemp;
		}
		else
		{
			double dbtemp = dbExpandy/dbExpandx*(m_rtMap.dBottom - m_rtMap.dTop)/2;
			m_rtMap.dTop = fCentery - dbtemp;
			m_rtMap.dBottom = fCentery + dbtemp;
		}
		//计算所需绘制的航线段
		POSITION posDrawPt = m_CrossPos;
		CGIS_Pathpoint* pPathDrawPt = NULL;
		pPathDrawPt = (CGIS_Pathpoint*)pPathList->GetNext( posDrawPt );
		DrawRoutePtList.AddTail( pPathDrawPt );
		while(posDrawPt)
		{
			pPathDrawPt = (CGIS_Pathpoint*)pPathList->GetNext( posDrawPt );
			DrawRoutePtList.AddTail( pPathDrawPt );
			if( !IsPtInRect(pPathDrawPt->GetPathpointPosition().dx,pPathDrawPt->GetPathpointPosition().dy
				,m_rtMap.dLeft,m_rtMap.dTop,m_rtMap.dRight,m_rtMap.dBottom))
				break;
		}
		posDrawPt = m_CrossPos;
		pPathList->GetPrev( posDrawPt );
		while( posDrawPt )
		{
			pPathDrawPt = (CGIS_Pathpoint*)pPathList->GetPrev( posDrawPt );
			DrawRoutePtList.AddHead( pPathDrawPt );
			if( !IsPtInRect(pPathDrawPt->GetPathpointPosition().dx,pPathDrawPt->GetPathpointPosition().dy
				,m_rtMap.dLeft,m_rtMap.dTop,m_rtMap.dRight,m_rtMap.dBottom))
				break;
		}
		//所需绘制的路段集
		CPtrList* listRut = pPathpointNode->GetNodeRutList();
		POSITION posRut = listRut->GetHeadPosition();
		CGIS_NetworkNodeRut* pNetworkNodeRut = NULL;
		CPen penRut;
		penRut.CreatePen( PS_SOLID , 20 , m_ExpandMapCfg.colRoad );
		CPen* pOldPen = m_pDCCross->SelectObject( &penRut );
		int* pnRutNum = new int[listRut->GetCount()];
		//计算一共有几个绘制点
		int nPtAllNum = 0; 
		while( posRut )
		{
			pNetworkNodeRut = (CGIS_NetworkNodeRut*)listRut->GetNext( posRut );
			if( !pNetworkNodeRut )
				continue;
			nPtAllNum += pNetworkNodeRut->GetRutPtNum();
		}
		POINT* ptSet;
		ptSet = new POINT[nPtAllNum];
		posRut = listRut->GetHeadPosition();
		int nRutCount = 0;
		int nPtCount = 0;
		while( posRut )
		{
			pNetworkNodeRut = (CGIS_NetworkNodeRut*)listRut->GetNext( posRut );
			if( !pNetworkNodeRut )
				continue;
			int nPtNum = pNetworkNodeRut->GetRutPtNum();
			ST_GEO_PTXY* ptLatLonSet = pNetworkNodeRut->GetRutPtList();
			int nx,ny;
			for( int i = 0 ; i < nPtNum ; i++ )
			{
				ExpandMapToView( m_rtMap , m_dbExpandScale ,m_dbSinAngle ,
					m_dbConAngle , ptLatLonSet[i].dx , 
					 ptLatLonSet[i].dy , nx,ny );
				ptSet[nPtCount].x = nx;
				ptSet[nPtCount].y = ny;
				nPtCount++;
			}
			pnRutNum[nRutCount] = nPtNum;
			nRutCount++;
		}
		//绘制路段
#ifdef _WIN32_WCE
		wce_PolyPolyline( m_pDCCross->GetSafeHdc() , ptSet , (unsigned long*)pnRutNum , nRutCount );
#endif
		m_pDCCross->SelectObject( pOldPen );
		penRut.DeleteObject();
		//绘制中心虚线
		m_pDCCross->SetBkMode( TRANSPARENT );
		penRut.CreatePen( PS_DASH,1,m_ExpandMapCfg.colMidLine );
		pOldPen = m_pDCCross->SelectObject( &penRut );
#ifdef _WIN32_WCE
		wce_PolyPolyline( m_pDCCross->GetSafeHdc() , ptSet , (unsigned long*)pnRutNum , nRutCount );
#else
		m_pDCCross->PolyPolyline( ptSet , (unsigned long*)pnRutNum , nRutCount );
#endif
		m_pDCCross->SelectObject( pOldPen );
		penRut.DeleteObject();

		if( ptSet )
		{
			delete[] ptSet;
			ptSet = NULL;
		}
		if( pnRutNum )
		{
			delete[] pnRutNum;
			pnRutNum = NULL;
		}
		//绘制航线
		POINT* pRutPtset = new POINT[DrawRoutePtList.GetCount()];
		int nRutPtCount = 0;
		posDrawPt = DrawRoutePtList.GetHeadPosition();
		while( posDrawPt )
		{
			pPathDrawPt = (CGIS_Pathpoint*)DrawRoutePtList.GetNext( posDrawPt );
			if( !pPathDrawPt )
				continue;
			ST_GEO_PTXY ptLatLonSet = pPathDrawPt->GetPathpointPosition();
			int nx,ny;
			ExpandMapToView( m_rtMap , m_dbExpandScale ,m_dbSinAngle ,
				m_dbConAngle , ptLatLonSet.dx , 
				 ptLatLonSet.dy , nx,ny );
			pRutPtset[nRutPtCount].x = nx;
			pRutPtset[nRutPtCount].y = ny;
			nRutPtCount++;
		}
		penRut.CreatePen( PS_SOLID , 10 , m_ExpandMapCfg.colRut );
		pOldPen = m_pDCCross->SelectObject( &penRut );
		m_pDCCross->Polyline( pRutPtset , nRutPtCount );
		penRut.DeleteObject();
		m_pDCCross->SelectObject( pOldPen );
		//绘制本车图标
//		m_pDCCross->BitBlt(0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength
//			,m_pDCCross,0,0,SRCCOPY );
//		CPoint ptDraw;
//		int nx ,ny;
//		ExpandMapToView( m_rtMap , m_dbExpandScale ,m_dbSinAngle ,
//			m_dbConAngle , m_CurPt.dx , 
//			 m_CurPt.dy , nx,ny );
//
//		ptDraw.x = nx;
//		ptDraw.y = ny;			
//		ptDraw.x -= 20;
//		ptDraw.y -= 20;
//		m_CarImageList.Draw( m_pDCCross, 0, ptDraw, ILD_TRANSPARENT );
		if( pRutPtset )
		{
			delete[] pRutPtset;
			pRutPtset = NULL;
		}
		return m_pDCCross;
		
	}
	//虚拟图
	else if( pPathpointNode->GetCrossStatus() == CGIS_PathpointNode::STATUS_IMAGE )
	{

		AFX_MANAGE_STATE(AfxGetStaticModuleState());


		//加载
		DWORD dw2Dor3D;
		BYTE* byBmpBack = GetExpandImgByID( pPathpointNode->GetCrossImgID() , dw2Dor3D );
		BYTE* byArrow = GetExpandImgByID( pPathpointNode->GetCrossArrowID() ,dw2Dor3D );

		//绘制
		CDC *pDC = GetDC();

		//背景
		BITMAPFILEHEADER* BitMapfileHeader = (BITMAPFILEHEADER*)byBmpBack;
		BYTE* pMemKeepBack = new BYTE[BitMapfileHeader->bfSize-sizeof(BITMAPFILEHEADER)];
		memcpy( pMemKeepBack , byBmpBack+sizeof(BITMAPFILEHEADER) , BitMapfileHeader->bfSize-sizeof(BITMAPFILEHEADER) );
		BITMAPINFO* BitMapInfo = (BITMAPINFO*)pMemKeepBack;

		DWORD dwWidth = (DWORD)(BitMapInfo->bmiHeader.biWidth) ; 
		DWORD dwHeight = (DWORD)(BitMapInfo->bmiHeader.biHeight);

		SetDIBitsToDevice(m_pDCCross->GetSafeHdc(),0,0,dwWidth,
			dwHeight,0,0,0,dwHeight,pMemKeepBack+BitMapfileHeader->bfOffBits-sizeof(BITMAPFILEHEADER),
			BitMapInfo,DIB_RGB_COLORS);


		//引导箭头
		BitMapfileHeader = (BITMAPFILEHEADER*)byArrow;
		BYTE* pMemKeepArrow = new BYTE[BitMapfileHeader->bfSize-sizeof(BITMAPFILEHEADER)];
		memcpy( pMemKeepArrow , byArrow+sizeof(BITMAPFILEHEADER) , BitMapfileHeader->bfSize-sizeof(BITMAPFILEHEADER) );
		BitMapInfo = (BITMAPINFO*)pMemKeepArrow;

		dwWidth = (DWORD)(BitMapInfo->bmiHeader.biWidth) ; 
		dwHeight = (DWORD)(BitMapInfo->bmiHeader.biHeight);

		CDC dcMem;
		dcMem.CreateCompatibleDC( pDC );
		CBitmap BmpMem;
		BmpMem.CreateCompatibleBitmap( pDC , m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength );
		CBitmap*pOldBmpMem = (CBitmap*)(dcMem.SelectObject( &BmpMem ));

		SetDIBitsToDevice(dcMem.GetSafeHdc(),0,0,dwWidth,
			dwHeight,0,0,0,dwHeight,pMemKeepArrow+BitMapfileHeader->bfOffBits-sizeof(BITMAPFILEHEADER)
			,BitMapInfo,DIB_RGB_COLORS);

		//透明色
		COLORREF crTranse;
		if( dw2Dor3D == 2 )
			crTranse = RGB(152,185,0);
		else if( dw2Dor3D == 3 )
			crTranse = RGB( 192,192,192);

		//透明叠加
		#ifdef _WIN32_WCE
		{
			TransparentImage(m_pDCCross->GetSafeHdc(), 0, 0, m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength,
							dcMem.GetSafeHdc(), 0, 0, m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength , crTranse );
		}
		#else
		{
			CDC dcMask;
			dcMask.CreateCompatibleDC( pDC );
			CBitmap bmpMask;
			bmpMask.CreateBitmap(m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength,1,1,NULL);
			CBitmap* pOldMaskBitmap = dcMask.SelectObject( &bmpMask );
			dcMask.FillSolidRect( 0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength,crTranse );

			//实现方式1
			//////////////////////////////////////////////////////////////////////////

//				m_pDCCross->MaskBlt(0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength
//					,&dcMem,0,0,bmpMask,0,0,ROP_TRANS);
			//////////////////////////////////////////////////////////////////////////

			//实现方式2
			//////////////////////////////////////////////////////////////////////////
			dcMem.SetBkColor(crTranse);
			dcMask.BitBlt(0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength
				,&dcMem,0,0,SRCCOPY);
			m_pDCCross->BitBlt(0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength
				,&dcMem,0,0,SRCINVERT);
			m_pDCCross->BitBlt(0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength
				,&dcMask,0,0,SRCAND);
			m_pDCCross->BitBlt(0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength
				,&dcMem,0,0,SRCINVERT);
			//////////////////////////////////////////////////////////////////////////
			
			

			dcMask.SelectObject( pOldMaskBitmap );
			dcMask.DeleteDC();
			bmpMask.DeleteObject();
		}
		#endif


		//清理
		if( byBmpBack )
		{
			delete[] byBmpBack;
			byBmpBack = NULL;
		}
		if( byArrow )
		{
			delete[] byArrow;
			byArrow = NULL;
		}
		if( pMemKeepBack )
		{
			delete[] pMemKeepBack;
			pMemKeepBack = NULL;
		}
		if( pMemKeepArrow )
		{
			delete[] pMemKeepArrow;
			pMemKeepArrow = NULL;
		}
		dcMem.SelectObject( pOldBmpMem );
		dcMem.DeleteDC();
		BmpMem.DeleteObject();
		ReleaseDC( pDC );
//		m_pDCCross->BitBlt(0,0,m_ExpandMapCfg.nWidth,m_ExpandMapCfg.nLength
//			,m_pDCCross,0,0,SRCCOPY );
		return m_pDCCross;
	}
	//无路口扩大图
	else
	{
		return NULL;
	}
		
	return NULL;

}
static HANDLE gExpandImgFile = INVALID_HANDLE_VALUE;
BYTE* CGeoView::GetExpandImgByID( int nImgID ,DWORD& dw2Dor3D )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(!m_MapSet)
		return NULL;
	IGIS_Map *pMap = NULL;
	pMap = (IGIS_Map*)m_MapSet->GetHead();

	if( !pMap )
		return NULL;

	swString strMapPath = pMap->GetMapInfo()->GetMapFilePath();

	if( gExpandImgFile == INVALID_HANDLE_VALUE )
	{
		swString strFName;
		strFName.Format( L"%sautomap.pic", strMapPath );
		gExpandImgFile = CreateFile( strFName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(INVALID_HANDLE_VALUE == gExpandImgFile)
			return FALSE;
	}

	BYTE* lpBuf = NULL;
	BYTE* lpUpBuf = NULL;
	TRY
	{
		CFile file( gExpandImgFile);
		int nImgNum ;
		file.Read( &nImgNum , sizeof(int) );

		ST_IMGINDEX pImgIndex;
		file.Seek( sizeof(int)+sizeof(ST_IMGINDEX)*nImgID, CFile::begin );
		file.Read( &pImgIndex , sizeof(ST_IMGINDEX) );

		dw2Dor3D = pImgIndex.dw2Dor3D;

		file.Seek( pImgIndex.dwOffset , CFile::begin );
		lpBuf = new BYTE[pImgIndex.dwLen];
		file.Read( lpBuf , pImgIndex.dwLen );

		lpUpBuf = new BYTE[pImgIndex.dwUpCompLen];

		int nUnZipRet = uncompress(lpUpBuf, &pImgIndex.dwUpCompLen, lpBuf, pImgIndex.dwLen);
		if (nUnZipRet != Z_OK)
		{
			if( lpBuf )
			{
				delete[] lpBuf;
				lpBuf = NULL;
			}
			if( lpUpBuf )
			{
				delete[] lpUpBuf;
				lpUpBuf = NULL;
			}
			return NULL;
		}
		
		if( lpBuf )
		{
			delete[] lpBuf;
			lpBuf = NULL;
		}
		return lpUpBuf;
		
	}
	CATCH( CMemoryException, e )
	{
		if( lpBuf )
		{
			delete[] lpBuf;
			lpBuf = NULL;
		}
		if( lpUpBuf )
		{
			delete[] lpUpBuf;
			lpUpBuf = NULL;
		}
		return NULL;
	}
	AND_CATCH( CFileException, e )
	{
		if( lpBuf )
		{
			delete[] lpBuf;
			lpBuf = NULL;
		}
		if( lpUpBuf )
		{
			delete[] lpUpBuf;
			lpUpBuf = NULL;
		}
		return NULL;
	}
	AND_CATCH( CException, e )
	{
		if( lpBuf )
		{
			delete[] lpBuf;
			lpBuf = NULL;
		}
		if( lpUpBuf )
		{
			delete[] lpUpBuf;
			lpUpBuf = NULL;
		}
		return NULL;
	}
	END_CATCH	
	
	return NULL;
}
void CGeoView::ExpandMapToView( ST_GEO_RECT rtMap , double dbExpandScale 
		, double dbSin , double dbCos , float fx , float fy , int& nx , int& ny )
{
	float tx,ty;
	tx = (fx - rtMap.dLeft)*dbExpandScale;
	ty = (rtMap.dBottom - fy)*dbExpandScale;


	POINT ptCenter;
	ptCenter.x = m_ExpandMapCfg.nWidth/2;
	ptCenter.y = m_ExpandMapCfg.nLength/2;
	float x = tx - ptCenter.x;
	float y = ty - ptCenter.y;
	tx = ptCenter.x + x*dbCos - y*dbSin;
	ty = ptCenter.y + x*dbSin + y*dbCos;

	nx = (int)(tx+0.5);
	ny = (int)(ty+0.5);
}

