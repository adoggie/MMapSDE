
#ifndef _GEO_VIEW_H
#define _GEO_VIEW_H

#include "swbase.h"
///////////////////////////////////////////////////////////////////////////////
#include "GeoGlobe.h"
#include "IGIS_MapView.h"
#include "IGIS_Map.h"
#include "GIS_MapInfo.h"
#include "GIS_NetworkRoute.h"
#include "GIS_FeaturePoint.h"
#include "GIS_FeatureLine.h"
#include "GIS_FeaturePoly.h"
#include "GIS_FeatureRut.h"
#include "RichPOIView.h"

const unsigned short MLBNUM_MAX = 1024;

class CGIS_MapLayer;
class  CGeoView;

#ifdef _CVIEW
	class swEXPORT CGeoView : public CView, public IGIS_MapView
#else
	class swEXPORT CGeoView : public CWnd, public IGIS_MapView
#endif
{
public:
	CGeoView();		// protected constructor used by dynamic creation
//	BOOL		bMapDrawed;				 // 绘制标示
// Attributes
protected:
	HANDLE	m_hDrawMap;
	CPen		*m_pOldPen, m_pen;        //原画笔和当前画笔
	CBrush		*m_pOldBrush, m_brush;    //原笔刷和当前笔刷
	CPen  *m_pOldApartPen , m_ApartPen;
	CBrush  *m_pOldApartBrush , m_ApartBrush;
	CFont		*m_pOldFont, m_font;      //原字体和当前字体
     int		X_OFF	;	             // x方向的注记偏移
	 int		Y_OFF	;	             // y方向的注记偏移
	 int		DC_BOTTOM;	             // 绘图区域：底
	 int		DC_LEFT	;	             // 绘图区域：左
	 int		DC_RIGHT;	             // 绘图区域：右
	 int		DC_TOP;	                 // 绘图区域：顶
//	BOOL		bMapDrawed;				 // 绘制标示
	double		dNewScale;               //更新后的比例尺

	ST_GEO_RECT	drawRect;			     // 当前地图可视范围
 	ST_GEO_RECT	NewDrawRect;	         //更新后的地图可视范围	
	ST_GEO_RECT RotateRect;
	
	CDC			*pBufDC;				// 缓冲DC
	CBitmap		*pBufBMP;				// 缓冲位图
	CBitmap     *m_pOldBufBMP;			
	CDC			*pCurrentDC;			// 当前DC
	CPtrList*   m_MapSet;               //所有与该VIEW对应的MAP集合
	CPoint		m_startPt, m_endPt;     //光标的起始和终止点
	BOOL		m_bDown;                //当前光标的状态

	CPtrList	*pAnnoList;				// 注记信息链表
	CImageList	ImageList;              //物标的图标链表
	int			m_nW;                   //字体的宽度
	int			m_nH;                   //字体的高度
	RECT		plotRect;				// 当前屏幕可视范围
	RECT		m_MaxPlotRect;			//最大屏幕可视范围
	CDC			*pBackDC;				// 整合DC，和前景一致
	CBitmap		*pBackBMP;              //背景画布
	CBitmap     *m_pOldBackBMP;
	double		dScale;					// 比例尺
	CPoint      m_CenterPt;             //地图中心的屏幕坐标
	ST_GEO_PTXY	m_CurPt;                //当前位置的经纬度
	BOOL*        m_LayerDrawState;         //图层是否显示的状态
	UINT        m_LayerCount;			//当前的最大图层数
	ST_LonLatLinelevel st_level;        //绘制经纬度的精度

	BOOL m_bMapSeclude;
	UINT m_Angle;                      //地图旋转的角度
	double m_dbAngle;				   //车头方向 
	double* m_dbScaleValue;
	UINT m_ScaleCount;
	UINT m_ScaleCurLevel;
	BOOL m_Nav;						  //当前是否为导航状态
//	UINT m_MatchDis;				  //道路匹配的阀值
	CGIS_NetworkRoute* m_pNetworkRoute ;
//	UINT m_PathNodeID;
	
//	UINT m_LastRoatID;
	CGIS_FeatureRut* m_LastRoat;
	//是否平移,车辆是否在导航状态下移动中
	BOOL m_bMoveMap;
	COLORREF m_colBack;
	ST_GEO_BRUSH m_BackBrush;
	ST_GEO_PEN   m_BackPen;
	int m_OldScaleLevel;
	BOOL         m_bIsVehMiddle;  
public: 
	//刷新
	virtual BOOL Update(IGIS_Message* pMessage); //刷新
	//返回当前比例尺
	virtual double GetCurrentScale();
	//得到当前比例尺级别
	virtual UINT GetCurrentScaleLevel();
	//返回保存的上个状态的可视范围
	virtual ST_GEO_RECT GetOldDrawRect();
	//删除VIEW对应的某张地图
	virtual BOOL ReleaseMap(IGIS_Map* pMap); 
	//返回当前VIEW对应的地图集合
	virtual CPtrList* GetMapSet();
	//返回当前地图可视范围
	virtual ST_GEO_RECT GetCurrentDrawrect();
	//返回屏幕可视范围
	virtual RECT GetViewPlot();
	// 从地图到屏幕坐标的转换函数
	virtual void XyMapToView(double dx, double dy, int& nx, int& ny);
	// 为控制点添加注释信息
	virtual void AddAnnotate(ST_GEO_PTXY* pPtList, int nPtNum, swString pText , BYTE byLevel = 0 ,BOOL bSameDirect = TRUE );
    //初始化画笔
	virtual void PreparePen(ST_GEO_PEN pen);
	//初始化笔刷
	virtual void PrepareBrush(ST_GEO_BRUSH brush);
	//删除画笔
	virtual void ReleasePen();
	//删除笔刷
	virtual void ReleaseBrush();
	//画线物标
	virtual void DrawLine(ST_GEO_PTXY* pPtList, int nPtNum);
	//画复杂线物标
	virtual void DrawLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum, int nType);
	//画多边形物标
	virtual void DrawPoly(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum);
	//画标注文字
	virtual void DrawAnnotate();
	//添加VIEW对应的MAP
	virtual BOOL SetMap(IGIS_Map* pMap);
	//设置各图层的显示属性
	virtual BOOL SetDrawState(UINT nLayID,BOOL nState);
	//返回某图层的显示属性
	virtual BOOL GetDrawState(UINT nLayID);
	//设置绘制经纬网格时的精度
	virtual BOOL SetLonLatLevel(ST_LonLatLinelevel st_Drawlevel);
	//返回当前经纬度网格的精度
	virtual ST_LonLatLinelevel GetLonLatLevel();
	//返回当前转角度后的绘制范围
	virtual ST_GEO_RECT GetRotateRect();
	//返回当前地图的角度
	virtual UINT GetAngle();
	//转换比例尺单位
	virtual double TurnScale(UINT scalelevel);

private:
	// 绘制图形点
	void DrawPolyLine(ST_GEO_RECT *pRect);
	//绘制普通连线
	void DrawNomalLine(double dx1,double dy1,double dx2,double dy2, 
		DWORD dwR = 255, DWORD dwG = 0, DWORD dwB = 0);
	// 绘制间断线
	void DrawPartLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum);
	// 绘制铁路线
	void DrawRailLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum);
	// 绘制双线
	void DrawDoubleLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum);
	// 将drawrect居中显示
	void CenterinRect(ST_GEO_RECT &rect);
	// 计算添加注释信息的位置
	BOOL CalcAnnoStation(RECT &rect);
	// 判断指定区域内是否有注释信息
	BOOL IsAnnotate(RECT rect);
	//绘制查找到的最优路径（空）
	void DrawFindRut();
	// 将当前gps位置居中
	void CenterinPoint(CPoint pt);
	// 清除注释信息列表
	void ClearAnnoList();

	void ReLoadMapSet( CPtrList *pMapList, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, 
			double dbScale, double dbSeclude, BOOL &bMapSeclude, CPtrList &DrawMapSet );

	//设置图形绘制开关
	void SetLayerDrawState(UINT nCount);
	//根据设置返回经纬网格的显示
	swString GetTLonLat(float latlon,ST_LonLatLinelevel st_Drawlevel);
	//能否和其它道路一起绘制
	void FilterDrawRut( POINT* ptList , int &nPtCount ,int nNum , int* pPart , int &nCount );
	//绘制指北针
	void DrawMapCompass( CDC* pCompassDC );
public:
	double CalcDis( double dx1,double dy1,double dx2,double dy2);
	double CalcAng( double dx1,double dy1,double dx2,double dy2);
	void	GetPointArc(double dx1, double dy1, double dx2, double dy2, 
					  double dRatio, double& xx, double& yy);
	
	void SetBackColor( COLORREF colBack );
	COLORREF GetBackColor();
	//绘制当前GPS定位标志
	void DrawCurPt( CDC* pDC , int nPosX, int nPosY);
	//刷新视图
	void Refresh();
	//初始化屏幕
	void SetScale(double dx, double dy,UINT ScaleID);
	//以某点缩小
	void ZoomInAt(CPoint pt);
	//以某点放大
	void ZoomOutAt(CPoint pt);
	//绘制地图
	void DrawMap();
	// 调整设置
	void AdjustBuf();
	// 从地图到屏幕坐标的转换函数	
	void XyViewToMap(int nx, int ny, double& dx, double& dy);
	// 窗口方式放大
	void ZoomInWindow(int nx1, int ny1, int nx2, int ny2);
	//以一点为中心显示
	void MoveToCenter(double dX,double dY);
	
	// 由于函数实现中并没有使用到Angle这个参数，所以决定去掉。20070329
	//地图反旋转
//	void UnRotatePlot(float &X1, float &Y1,POINT RotateBasePt,double Angle);
	void UnRotatePlot(float &X1, float &Y1,POINT RotateBasePt);
	//地图旋转
//	void RotatePlot(float &X1, float &Y1, POINT RotateBasePt, double Angle);
	void RotatePlot(float &X1, float &Y1, POINT RotateBasePt);


	//计算当前比例尺
	void CalcScale();
	//光标在屏幕上拖动时的处理
	void DragMove(CPoint pt);
	//光标左键释放
	void ButtonUp(CPoint pt);
	//光标左键按下
	void ButtonDown(CPoint pt);
	//鼠标移动时的处理
	void MouseMove(CPoint pt);
	//绘制选定层（空）
	void DrawSel();
	// 绘制一矩形框
	void DrawRange();
	//导入地图
	void LoadMap(swString* szFileName);
	//返回整合后的DC
	CDC* GetBackDC();
	CDC *GetBufDC();
	CDC *GetCurrentDC();
	//返回背景画布
	CBitmap* GetBackBMP();
	//设置地图中心的坐标
	void SetCenterPt(CPoint pt);
	//返回当前位置经纬度
	ST_GEO_PTXY	GetCurPt();
	//设置当前位置的经纬度
	void SetCurPt(ST_GEO_PTXY pt);
	//绘制经纬网格
	void DrawLonLatLine(ST_GEO_RECT rect,CDC* pDC);
	//设置地图旋转的角度
	void SetAngle(UINT nAng);
	//设置比例尺范围
	void SetScale(double* scalev,UINT scalec);
	//地图匹配
	EnMatchRoad MatchRoad(ST_GEO_PTXY pt,ST_GEO_PTXY &Rpt,EnMathState enState);
	//匹配差分
	void MatchRoad( ST_GEO_PTXY& pt , int nSpeed , int nTime , EnMathState enState );
	//判断是否出现在起点附近逆航线行驶
	//1:同一道路，但行驶方向并不与航线相反
	//2:同一道路，但行驶方向与航线相反
	//3:不同道路
	//0：不能确定
	int GetWrongDirect();
	//返回游离状态时所匹配的道路，若返回指针为空，则说明当前无可匹配的道路
	CGIS_FeatureRut *GetCurrentRoute();
	//返回游离状态时所匹配的道路的拷贝
	CGIS_FeatureRut *GetCurrentRouteCopy();
//	//设置道路匹配的阀值
//	void SetMatchDis(UINT dis);
	//绘制目的地 nNo == 0 :: 终点   nNo == 1 :: 中途点
	void DrawFlag(double dx, double dy, int nNo);
	//设置路径
	void   SetNetworkRoute( CGIS_NetworkRoute* pNetworkRoute );
	CGIS_NetworkRoute*	GetNetworkRoute();
	//设置地图屏幕大小
	void SetPlotRect(RECT rect);
	double GetScale(int scalelevel);
	int GetScaleCount();
	//参数：是否为了偏航而清理
	void ClearMathState( BOOL bNaveOut );
	void DrawUserLine(ST_GEO_PTXY* pPtList, int nPtNum,ST_GEO_PEN userPen);
	void DrawNetworkRoute();
	//设置白天黑夜模式
	void SetbDayShow( BOOL bDayShow );
	//取得白天黑夜模式
	BOOL GetbDayShow();
	//设置画车参数
	void SetCarPar(	int nVehAngle,
	BOOL bShowVehSymbol,
	int  nRadius,
	float  fHightScale);
	void SetbReDrawCar( BOOL bRedrawCar );
	void SetbDrawNetWorkRoute( BOOL bDrawNetWorkRoute );
	void Setb3DView( BOOL b3D );
	BOOL Getb3DView();
	BOOL GetbDrawByLayer();
	void SetbDrawByLayer( BOOL bDrawByLayer );
	CPoint GetRotateCenter();
	void SetRotateCenter( CPoint ptRotateCenter );
	CPoint GetZoomCenter();
	void SetZoomCenter( CPoint ptZoomCenter );
	//设置导航模式
	void SetbNav( BOOL bNav );
	//取得当前导航模式
	BOOL GetbNav();
	//取得当前绘制的TURNLIST位置
	POSITION GetTurnListPos( );
	//设置需要绘制的TURNLIST位置
	void SetTurnListPos( POSITION posTurnList );
	//显示光标所在位置的地物名称（道路或POI）
	void ShowFindTip( CPoint pPt );
	//是否高亮显示光标所在道路
	void ShowFocusRut( CPoint pPt );
	//返回所选道路的属性值
	BOOL GetFocusRut( ST_FOCUS_RUT &stFocusRut );
	//返回光标位置
	CPoint GetFindTip( );
	//返回光标所在位置的经纬度
	ST_GEO_PTXY GetFindXyTip();
	//强制显示符合条件的道路
	void StartSpecalRut( ST_FOCUS_RUT stSpecalRut );
	//停止绘制强制道路
	void StopSpecalRut( );
	//是否显示指北针
	void SetbShowCompass( BOOL bShowCompass );
	//显示指北针的位置
	void SetCompassPs( CPoint psCompass );
	//返回光标所在的名称
	swString GetstrFind( );
	//清理缓存数据桶
	void ClearBufData( BOOL bClearBuf );
	int GetCarDirect();
	short GetCarRunNextPt();
	//返回车辆位置所在的城市ID
	int GetLocationCity( ST_GEO_PTXY ptCurrent );
	//设置需要焦点显示的兴趣点
	void SetFocusPOI( ST_FOCUS_PT ptFocusInput );
	//取消显示焦点显示的兴趣点
	void DeleteFocusPOI();
	//设置路口扩大图参数
	void SetExpandCfg( ST_EXPAND_MAPSET ExpandSet );
	//返回路口扩大图
	CDC* GetExpandMap( POSITION pos );
	

	// Dialog Data
	//{{AFX_DATA(CGeoView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeoView)
	protected:
	//}}AFX_VIRTUAL
private:

	CPtrList m_DrawMapSet;

	
	CGIS_MapLayer *m_pPreLList[MLBNUM_MAX];		//加载数据的图层链表
	int m_pPreBIDs[MLBNUM_MAX];		//加载数据的图幅链表
	unsigned short m_nPreMBLNum;				//加载的数据数量
	CGIS_MapLayer *m_pCurLList[MLBNUM_MAX];		//加载数据的图层链表
	int m_pCurBIDs[MLBNUM_MAX];		//加载数据的图幅链表
	unsigned short m_nCurMBLNum;				//加载的数据数量
	int m_nPreBNum;					//当前加载的图幅个数
	int m_pPreBIDList[MLBNUM_MAX]; //当前加载的图幅ID列表

	BOOL		m_bLFlag;						//加载进度的标志
	BOOL		m_bDFlag;	
	HICON       m_FlaghIcon;
	HICON		m_MidFlagIcon;
	ST_GEO_PTXY *m_pPtList;
	BOOL m_bDayShow;							//白天还是黑夜的显示模式
	//卸载标志
	//转角度参数
	double m_sinRotate;
	double m_cosRotate;
	//贴图位置
	CPoint m_positionBMP;
	//贴图的误差
	float m_dxMoveError;
	float m_dyMoveError;
	//平移地图时临时DC
	CDC* m_pdcMoveTemp;
	CBitmap* m_pmapMoveTemp;
	CBitmap* m_pOldMoveTemp;
	//平移地图时纯的地图DC
	CDC* m_pPureBackDC;
	CBitmap* m_pPureBackBmp;
	CBitmap* m_pOldPureBackBmp;
	//与地图分割的DC
	CDC* m_pApartBackDC;
	CBitmap* m_pApartBackBmp;
	CBitmap* m_pOldApartBackBmp;
	float nAdjustx, nAdjusty;

	//绘制车位的参数
	int m_nVehAngle;
	BOOL m_bShowVehSymbol;
	HPEN m_hVehPen;
	HPEN m_hClePen;
	HBRUSH m_hVehBrs;
	int  m_nRadius;
	CBitmap      m_bmpForBmp;
	float  m_fHightScale;
	//是否只需要重绘车辆
	BOOL m_bReDrawCar;
	//是否绘制航线
	BOOL m_bDrawNetWorkRoute; 
	//最后一次匹配航线的位置(偏航后的偏航点)
	ST_GEO_PTXY m_ptMatch; 
	//最后一次匹配航线所在位置是否为高速公路
	BOOL m_bHighway ; 
	//所需绘制的POI集
	CPtrList *m_DrawPtList;
	//连续偏航且匹配在同一条道路上的计数器
	int m_nLeaveCount;
	//是否为3D显示
	BOOL m_b3D;
    //本车位置的图标
	CImageList	m_CarImageList;   
	//是否不用分层显示机制
	BOOL m_bDrawByLayer;
	//地图旋转中心
	CPoint m_ptRotateCenter;
	//地图缩放中心
	CPoint m_ptZoomCenter;
	//当前是否为导航模式
	BOOL m_bNav;
	//当前绘制的TURNLIST的位置
	POSITION m_posTurnList;
	//是否显示所选中地物名称
	BOOL m_bShowTip;
	//是否显示光标选中的道路
	BOOL m_bShowFocusRut;
	//显示TIP的中心点位置
	CPoint m_TipPt;
	//TIP所显示的名称
	swString m_strFindName;
	//光标选中的焦点道路的属性
	ST_FOCUS_RUT m_FocusRut;
	//指定强制绘制的道路（如规避道路）
	CGIS_FeatureRut* m_pSpecialRut;
	//是否显示强制绘制的道路
	BOOL m_bShowSpecalRut;
	//TIP所在的地物
	CGIS_Feature *m_pFeatureTip;
	//TIP所指地物的类型
	EnQueryObjType m_TipFeaType;
	//TIP所在位置的经纬度
	ST_GEO_PTXY m_ptTipXy;

	//是否显示指北针
	BOOL m_bShowMapCompass;
	//显示指北针的位置
	CPoint m_ptMapCompass;
    //指北针的图标
	CImageList	m_CompassImageList;   
	//最近匹配到的航线中节点的位置
	POSITION m_PathNodePos;
	//最近匹配到的航线点的位置
	POSITION m_PathPtPos;
	//不在航线上时，最后一次最靠近航线的节点的位置
	POSITION m_PathNearNodePos;
	//特殊道路的图层信息
	CGIS_LayerInfo* m_pSpRutLayerInfo;
	//是否需要清空缓冲图幅
	long m_bClearBuf;
	//车辆行驶方向1：路段头－》路段尾 2：路段尾－》路段头0：未确定
	int	 m_nCarRunDirect;
	//车辆行驶的下一个节点号
	short m_nCarRunNextPt;
	//所需焦点显示的兴趣点列表
	CPtrList m_FocusPtSet;
	//RichPOI显示加载模块
	CRichPOIView m_RichPOIView; // scott 2009.3.9
	//路口扩大图的参数
	ST_EXPAND_MAPSET m_ExpandMapCfg;
	//当前路口扩大图中路口在航线中的位置
	POSITION m_CrossPos;
	//当前已经绘制好的路口扩大图的位图
	CBitmap* m_pCrossBmp;
	CBitmap* m_pCrossBmpOld;
//	//当前已经绘制好的路口扩大图的位图(无车辆)
//	CBitmap* m_pCrossBmpWithoutCar;
//	CBitmap* m_pCrossBmpWithoutCarOld;
	//路口扩大图所对应的内存DC
	CDC* m_pDCCross;
//	//路口扩大图所对应的无车辆显示的DC
//	CDC* m_pDCCrossWithoutCar;
	//路口扩大图的范围
	ST_GEO_RECT m_rtMap;
	//路口扩大图的比例尺
	double m_dbExpandScale;
	//路口扩大图的角度正弦值
	double m_dbSinAngle;
	//路口扩大图的角度余弦值
	double m_dbConAngle;




	//////////////////////////////////////////////////////////////////////////


	

	void GetMSLayer( CGIS_GeoMap *pMap, double dbScale, CGIS_MapLayer **pLList, int& nCount );
	void DrawPoint( CGIS_FeaturePoint *pPoint, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
//	void DrawPoint( CGIS_FeaturePoint *pPoint, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
	void DrawLine( CGIS_FeatureLine *pLine, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno, BYTE nStyle );
//	void DrawLine( CGIS_FeatureLine *pLine, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno, BYTE nStyle );
	void DrawPoly( CGIS_FeaturePoly *pPoly, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
	void DrawPoly(ST_GEO_PTXY* pPtList, int nPtNum);
	void DrawPolyLayer( ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer );
	void DrawWaterPoly( CGIS_FeaturePoly *pPoly, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
//	void DrawPoly( CGIS_FeaturePoly *pPoly, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
	void DrawRut( CGIS_FeatureRut *pRut, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno,ST_GEO_PEN pen );
	void DrawRutLayer( ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer );
	void DrawLineLayer( ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer );
	void DrawPoint(int nx, int ny,short SymbolID);
	void DrawPointLayer(  ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer  );
//	void DrawRut( CGIS_FeatureRut *pRut, swRect rtPlot, ST_GEO_RECT rtDraw, double dbScale, BOOL bAnno );
	//根据旋转角度确定绘制范围
	ST_GEO_RECT MaxNewRect(RECT rect);
	void  DrawOutlineForChars(CDC* pDC, int x, int y, swString& strContext, COLORREF ExCol, COLORREF InCol );
	void  DrawSortLayer(ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer  );
	void  DrawBackRut( ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer);
	//增加绘制的POI
	BOOL  AddDrawPOI( ST_PT_DRAW *ptDraw );
	//清理绘制POI集
	void  ClearDrawPOI( );
	//绘制POI
	void  DrawPOI( );
	//在导航过程中绘制路口指示箭头
	void DrawTurnSign( CDC* pDC );
	//显示光标所在位置的地物名称
	void ShowCursorFind( CDC* pShowDC );
	//高亮显示光标所在道路
	void ShowCursorFindRut( CDC* pShowDC );
	//显示某特定的道路（如规避道路）
	void ShowSpecialRut( CDC* pShowDC );
	//校正光标位置
	void ReSetCursor( );
	//文字阅读方向是否与两点的方向一致
	BOOL ReadDirect(int nStartx , int nStarty , int nEndx , int nEndy );
	//绘制焦点兴趣点
	void ShowFocusPOI(CDC* pShowDC);
	//绘制RICHPOI
	void DrawRichPOI(CPtrList* pListRichPOi,ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw);
	//根据图片ID返回虚拟图位图资源
	BYTE* GetExpandImgByID( int nImgID ,DWORD& dw2Dor3D );
	//经纬度转换为路口扩大图中的像素位置
	void ExpandMapToView( ST_GEO_RECT rtMap , double dbExpandScale 
		, double dbSin , double dbCos , float fx , float fy , int& nx , int& ny );

struct ST_IMGINDEX
{
	DWORD	dwID;		//文件id
	DWORD	dw2Dor3D;	//2-2D模式图;3-3D模式图
	DWORD	dwOffset;	//偏移，从文件头开始
	DWORD	dwLen;		//压缩包长度
	DWORD	dwUpCompLen;//压缩前（解压后）长度
};
	
//	ST_GEO_RECT MinNewRect(RECT rect);
// Implementation
public:
	virtual ~CGeoView();

	// Generated message map functions
	//{{AFX_MSG(CGeoView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif 


