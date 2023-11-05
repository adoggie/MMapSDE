
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
//	BOOL		bMapDrawed;				 // ���Ʊ�ʾ
// Attributes
protected:
	HANDLE	m_hDrawMap;
	CPen		*m_pOldPen, m_pen;        //ԭ���ʺ͵�ǰ����
	CBrush		*m_pOldBrush, m_brush;    //ԭ��ˢ�͵�ǰ��ˢ
	CPen  *m_pOldApartPen , m_ApartPen;
	CBrush  *m_pOldApartBrush , m_ApartBrush;
	CFont		*m_pOldFont, m_font;      //ԭ����͵�ǰ����
     int		X_OFF	;	             // x�����ע��ƫ��
	 int		Y_OFF	;	             // y�����ע��ƫ��
	 int		DC_BOTTOM;	             // ��ͼ���򣺵�
	 int		DC_LEFT	;	             // ��ͼ������
	 int		DC_RIGHT;	             // ��ͼ������
	 int		DC_TOP;	                 // ��ͼ���򣺶�
//	BOOL		bMapDrawed;				 // ���Ʊ�ʾ
	double		dNewScale;               //���º�ı�����

	ST_GEO_RECT	drawRect;			     // ��ǰ��ͼ���ӷ�Χ
 	ST_GEO_RECT	NewDrawRect;	         //���º�ĵ�ͼ���ӷ�Χ	
	ST_GEO_RECT RotateRect;
	
	CDC			*pBufDC;				// ����DC
	CBitmap		*pBufBMP;				// ����λͼ
	CBitmap     *m_pOldBufBMP;			
	CDC			*pCurrentDC;			// ��ǰDC
	CPtrList*   m_MapSet;               //�������VIEW��Ӧ��MAP����
	CPoint		m_startPt, m_endPt;     //������ʼ����ֹ��
	BOOL		m_bDown;                //��ǰ����״̬

	CPtrList	*pAnnoList;				// ע����Ϣ����
	CImageList	ImageList;              //����ͼ������
	int			m_nW;                   //����Ŀ��
	int			m_nH;                   //����ĸ߶�
	RECT		plotRect;				// ��ǰ��Ļ���ӷ�Χ
	RECT		m_MaxPlotRect;			//�����Ļ���ӷ�Χ
	CDC			*pBackDC;				// ����DC����ǰ��һ��
	CBitmap		*pBackBMP;              //��������
	CBitmap     *m_pOldBackBMP;
	double		dScale;					// ������
	CPoint      m_CenterPt;             //��ͼ���ĵ���Ļ����
	ST_GEO_PTXY	m_CurPt;                //��ǰλ�õľ�γ��
	BOOL*        m_LayerDrawState;         //ͼ���Ƿ���ʾ��״̬
	UINT        m_LayerCount;			//��ǰ�����ͼ����
	ST_LonLatLinelevel st_level;        //���ƾ�γ�ȵľ���

	BOOL m_bMapSeclude;
	UINT m_Angle;                      //��ͼ��ת�ĽǶ�
	double m_dbAngle;				   //��ͷ���� 
	double* m_dbScaleValue;
	UINT m_ScaleCount;
	UINT m_ScaleCurLevel;
	BOOL m_Nav;						  //��ǰ�Ƿ�Ϊ����״̬
//	UINT m_MatchDis;				  //��·ƥ��ķ�ֵ
	CGIS_NetworkRoute* m_pNetworkRoute ;
//	UINT m_PathNodeID;
	
//	UINT m_LastRoatID;
	CGIS_FeatureRut* m_LastRoat;
	//�Ƿ�ƽ��,�����Ƿ��ڵ���״̬���ƶ���
	BOOL m_bMoveMap;
	COLORREF m_colBack;
	ST_GEO_BRUSH m_BackBrush;
	ST_GEO_PEN   m_BackPen;
	int m_OldScaleLevel;
	BOOL         m_bIsVehMiddle;  
public: 
	//ˢ��
	virtual BOOL Update(IGIS_Message* pMessage); //ˢ��
	//���ص�ǰ������
	virtual double GetCurrentScale();
	//�õ���ǰ�����߼���
	virtual UINT GetCurrentScaleLevel();
	//���ر�����ϸ�״̬�Ŀ��ӷ�Χ
	virtual ST_GEO_RECT GetOldDrawRect();
	//ɾ��VIEW��Ӧ��ĳ�ŵ�ͼ
	virtual BOOL ReleaseMap(IGIS_Map* pMap); 
	//���ص�ǰVIEW��Ӧ�ĵ�ͼ����
	virtual CPtrList* GetMapSet();
	//���ص�ǰ��ͼ���ӷ�Χ
	virtual ST_GEO_RECT GetCurrentDrawrect();
	//������Ļ���ӷ�Χ
	virtual RECT GetViewPlot();
	// �ӵ�ͼ����Ļ�����ת������
	virtual void XyMapToView(double dx, double dy, int& nx, int& ny);
	// Ϊ���Ƶ����ע����Ϣ
	virtual void AddAnnotate(ST_GEO_PTXY* pPtList, int nPtNum, swString pText , BYTE byLevel = 0 ,BOOL bSameDirect = TRUE );
    //��ʼ������
	virtual void PreparePen(ST_GEO_PEN pen);
	//��ʼ����ˢ
	virtual void PrepareBrush(ST_GEO_BRUSH brush);
	//ɾ������
	virtual void ReleasePen();
	//ɾ����ˢ
	virtual void ReleaseBrush();
	//�������
	virtual void DrawLine(ST_GEO_PTXY* pPtList, int nPtNum);
	//�����������
	virtual void DrawLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum, int nType);
	//����������
	virtual void DrawPoly(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum);
	//����ע����
	virtual void DrawAnnotate();
	//���VIEW��Ӧ��MAP
	virtual BOOL SetMap(IGIS_Map* pMap);
	//���ø�ͼ�����ʾ����
	virtual BOOL SetDrawState(UINT nLayID,BOOL nState);
	//����ĳͼ�����ʾ����
	virtual BOOL GetDrawState(UINT nLayID);
	//���û��ƾ�γ����ʱ�ľ���
	virtual BOOL SetLonLatLevel(ST_LonLatLinelevel st_Drawlevel);
	//���ص�ǰ��γ������ľ���
	virtual ST_LonLatLinelevel GetLonLatLevel();
	//���ص�ǰת�ǶȺ�Ļ��Ʒ�Χ
	virtual ST_GEO_RECT GetRotateRect();
	//���ص�ǰ��ͼ�ĽǶ�
	virtual UINT GetAngle();
	//ת�������ߵ�λ
	virtual double TurnScale(UINT scalelevel);

private:
	// ����ͼ�ε�
	void DrawPolyLine(ST_GEO_RECT *pRect);
	//������ͨ����
	void DrawNomalLine(double dx1,double dy1,double dx2,double dy2, 
		DWORD dwR = 255, DWORD dwG = 0, DWORD dwB = 0);
	// ���Ƽ����
	void DrawPartLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum);
	// ������·��
	void DrawRailLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum);
	// ����˫��
	void DrawDoubleLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum);
	// ��drawrect������ʾ
	void CenterinRect(ST_GEO_RECT &rect);
	// �������ע����Ϣ��λ��
	BOOL CalcAnnoStation(RECT &rect);
	// �ж�ָ���������Ƿ���ע����Ϣ
	BOOL IsAnnotate(RECT rect);
	//���Ʋ��ҵ�������·�����գ�
	void DrawFindRut();
	// ����ǰgpsλ�þ���
	void CenterinPoint(CPoint pt);
	// ���ע����Ϣ�б�
	void ClearAnnoList();

	void ReLoadMapSet( CPtrList *pMapList, ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, 
			double dbScale, double dbSeclude, BOOL &bMapSeclude, CPtrList &DrawMapSet );

	//����ͼ�λ��ƿ���
	void SetLayerDrawState(UINT nCount);
	//�������÷��ؾ�γ�������ʾ
	swString GetTLonLat(float latlon,ST_LonLatLinelevel st_Drawlevel);
	//�ܷ��������·һ�����
	void FilterDrawRut( POINT* ptList , int &nPtCount ,int nNum , int* pPart , int &nCount );
	//����ָ����
	void DrawMapCompass( CDC* pCompassDC );
public:
	double CalcDis( double dx1,double dy1,double dx2,double dy2);
	double CalcAng( double dx1,double dy1,double dx2,double dy2);
	void	GetPointArc(double dx1, double dy1, double dx2, double dy2, 
					  double dRatio, double& xx, double& yy);
	
	void SetBackColor( COLORREF colBack );
	COLORREF GetBackColor();
	//���Ƶ�ǰGPS��λ��־
	void DrawCurPt( CDC* pDC , int nPosX, int nPosY);
	//ˢ����ͼ
	void Refresh();
	//��ʼ����Ļ
	void SetScale(double dx, double dy,UINT ScaleID);
	//��ĳ����С
	void ZoomInAt(CPoint pt);
	//��ĳ��Ŵ�
	void ZoomOutAt(CPoint pt);
	//���Ƶ�ͼ
	void DrawMap();
	// ��������
	void AdjustBuf();
	// �ӵ�ͼ����Ļ�����ת������	
	void XyViewToMap(int nx, int ny, double& dx, double& dy);
	// ���ڷ�ʽ�Ŵ�
	void ZoomInWindow(int nx1, int ny1, int nx2, int ny2);
	//��һ��Ϊ������ʾ
	void MoveToCenter(double dX,double dY);
	
	// ���ں���ʵ���в�û��ʹ�õ�Angle������������Ծ���ȥ����20070329
	//��ͼ����ת
//	void UnRotatePlot(float &X1, float &Y1,POINT RotateBasePt,double Angle);
	void UnRotatePlot(float &X1, float &Y1,POINT RotateBasePt);
	//��ͼ��ת
//	void RotatePlot(float &X1, float &Y1, POINT RotateBasePt, double Angle);
	void RotatePlot(float &X1, float &Y1, POINT RotateBasePt);


	//���㵱ǰ������
	void CalcScale();
	//�������Ļ���϶�ʱ�Ĵ���
	void DragMove(CPoint pt);
	//�������ͷ�
	void ButtonUp(CPoint pt);
	//����������
	void ButtonDown(CPoint pt);
	//����ƶ�ʱ�Ĵ���
	void MouseMove(CPoint pt);
	//����ѡ���㣨�գ�
	void DrawSel();
	// ����һ���ο�
	void DrawRange();
	//�����ͼ
	void LoadMap(swString* szFileName);
	//�������Ϻ��DC
	CDC* GetBackDC();
	CDC *GetBufDC();
	CDC *GetCurrentDC();
	//���ر�������
	CBitmap* GetBackBMP();
	//���õ�ͼ���ĵ�����
	void SetCenterPt(CPoint pt);
	//���ص�ǰλ�þ�γ��
	ST_GEO_PTXY	GetCurPt();
	//���õ�ǰλ�õľ�γ��
	void SetCurPt(ST_GEO_PTXY pt);
	//���ƾ�γ����
	void DrawLonLatLine(ST_GEO_RECT rect,CDC* pDC);
	//���õ�ͼ��ת�ĽǶ�
	void SetAngle(UINT nAng);
	//���ñ����߷�Χ
	void SetScale(double* scalev,UINT scalec);
	//��ͼƥ��
	EnMatchRoad MatchRoad(ST_GEO_PTXY pt,ST_GEO_PTXY &Rpt,EnMathState enState);
	//ƥ����
	void MatchRoad( ST_GEO_PTXY& pt , int nSpeed , int nTime , EnMathState enState );
	//�ж��Ƿ��������㸽���溽����ʻ
	//1:ͬһ��·������ʻ���򲢲��뺽���෴
	//2:ͬһ��·������ʻ�����뺽���෴
	//3:��ͬ��·
	//0������ȷ��
	int GetWrongDirect();
	//��������״̬ʱ��ƥ��ĵ�·��������ָ��Ϊ�գ���˵����ǰ�޿�ƥ��ĵ�·
	CGIS_FeatureRut *GetCurrentRoute();
	//��������״̬ʱ��ƥ��ĵ�·�Ŀ���
	CGIS_FeatureRut *GetCurrentRouteCopy();
//	//���õ�·ƥ��ķ�ֵ
//	void SetMatchDis(UINT dis);
	//����Ŀ�ĵ� nNo == 0 :: �յ�   nNo == 1 :: ��;��
	void DrawFlag(double dx, double dy, int nNo);
	//����·��
	void   SetNetworkRoute( CGIS_NetworkRoute* pNetworkRoute );
	CGIS_NetworkRoute*	GetNetworkRoute();
	//���õ�ͼ��Ļ��С
	void SetPlotRect(RECT rect);
	double GetScale(int scalelevel);
	int GetScaleCount();
	//�������Ƿ�Ϊ��ƫ��������
	void ClearMathState( BOOL bNaveOut );
	void DrawUserLine(ST_GEO_PTXY* pPtList, int nPtNum,ST_GEO_PEN userPen);
	void DrawNetworkRoute();
	//���ð����ҹģʽ
	void SetbDayShow( BOOL bDayShow );
	//ȡ�ð����ҹģʽ
	BOOL GetbDayShow();
	//���û�������
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
	//���õ���ģʽ
	void SetbNav( BOOL bNav );
	//ȡ�õ�ǰ����ģʽ
	BOOL GetbNav();
	//ȡ�õ�ǰ���Ƶ�TURNLISTλ��
	POSITION GetTurnListPos( );
	//������Ҫ���Ƶ�TURNLISTλ��
	void SetTurnListPos( POSITION posTurnList );
	//��ʾ�������λ�õĵ������ƣ���·��POI��
	void ShowFindTip( CPoint pPt );
	//�Ƿ������ʾ������ڵ�·
	void ShowFocusRut( CPoint pPt );
	//������ѡ��·������ֵ
	BOOL GetFocusRut( ST_FOCUS_RUT &stFocusRut );
	//���ع��λ��
	CPoint GetFindTip( );
	//���ع������λ�õľ�γ��
	ST_GEO_PTXY GetFindXyTip();
	//ǿ����ʾ���������ĵ�·
	void StartSpecalRut( ST_FOCUS_RUT stSpecalRut );
	//ֹͣ����ǿ�Ƶ�·
	void StopSpecalRut( );
	//�Ƿ���ʾָ����
	void SetbShowCompass( BOOL bShowCompass );
	//��ʾָ�����λ��
	void SetCompassPs( CPoint psCompass );
	//���ع�����ڵ�����
	swString GetstrFind( );
	//����������Ͱ
	void ClearBufData( BOOL bClearBuf );
	int GetCarDirect();
	short GetCarRunNextPt();
	//���س���λ�����ڵĳ���ID
	int GetLocationCity( ST_GEO_PTXY ptCurrent );
	//������Ҫ������ʾ����Ȥ��
	void SetFocusPOI( ST_FOCUS_PT ptFocusInput );
	//ȡ����ʾ������ʾ����Ȥ��
	void DeleteFocusPOI();
	//����·������ͼ����
	void SetExpandCfg( ST_EXPAND_MAPSET ExpandSet );
	//����·������ͼ
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

	
	CGIS_MapLayer *m_pPreLList[MLBNUM_MAX];		//�������ݵ�ͼ������
	int m_pPreBIDs[MLBNUM_MAX];		//�������ݵ�ͼ������
	unsigned short m_nPreMBLNum;				//���ص���������
	CGIS_MapLayer *m_pCurLList[MLBNUM_MAX];		//�������ݵ�ͼ������
	int m_pCurBIDs[MLBNUM_MAX];		//�������ݵ�ͼ������
	unsigned short m_nCurMBLNum;				//���ص���������
	int m_nPreBNum;					//��ǰ���ص�ͼ������
	int m_pPreBIDList[MLBNUM_MAX]; //��ǰ���ص�ͼ��ID�б�

	BOOL		m_bLFlag;						//���ؽ��ȵı�־
	BOOL		m_bDFlag;	
	HICON       m_FlaghIcon;
	HICON		m_MidFlagIcon;
	ST_GEO_PTXY *m_pPtList;
	BOOL m_bDayShow;							//���컹�Ǻ�ҹ����ʾģʽ
	//ж�ر�־
	//ת�ǶȲ���
	double m_sinRotate;
	double m_cosRotate;
	//��ͼλ��
	CPoint m_positionBMP;
	//��ͼ�����
	float m_dxMoveError;
	float m_dyMoveError;
	//ƽ�Ƶ�ͼʱ��ʱDC
	CDC* m_pdcMoveTemp;
	CBitmap* m_pmapMoveTemp;
	CBitmap* m_pOldMoveTemp;
	//ƽ�Ƶ�ͼʱ���ĵ�ͼDC
	CDC* m_pPureBackDC;
	CBitmap* m_pPureBackBmp;
	CBitmap* m_pOldPureBackBmp;
	//���ͼ�ָ��DC
	CDC* m_pApartBackDC;
	CBitmap* m_pApartBackBmp;
	CBitmap* m_pOldApartBackBmp;
	float nAdjustx, nAdjusty;

	//���Ƴ�λ�Ĳ���
	int m_nVehAngle;
	BOOL m_bShowVehSymbol;
	HPEN m_hVehPen;
	HPEN m_hClePen;
	HBRUSH m_hVehBrs;
	int  m_nRadius;
	CBitmap      m_bmpForBmp;
	float  m_fHightScale;
	//�Ƿ�ֻ��Ҫ�ػ泵��
	BOOL m_bReDrawCar;
	//�Ƿ���ƺ���
	BOOL m_bDrawNetWorkRoute; 
	//���һ��ƥ�亽�ߵ�λ��(ƫ�����ƫ����)
	ST_GEO_PTXY m_ptMatch; 
	//���һ��ƥ�亽������λ���Ƿ�Ϊ���ٹ�·
	BOOL m_bHighway ; 
	//������Ƶ�POI��
	CPtrList *m_DrawPtList;
	//����ƫ����ƥ����ͬһ����·�ϵļ�����
	int m_nLeaveCount;
	//�Ƿ�Ϊ3D��ʾ
	BOOL m_b3D;
    //����λ�õ�ͼ��
	CImageList	m_CarImageList;   
	//�Ƿ��÷ֲ���ʾ����
	BOOL m_bDrawByLayer;
	//��ͼ��ת����
	CPoint m_ptRotateCenter;
	//��ͼ��������
	CPoint m_ptZoomCenter;
	//��ǰ�Ƿ�Ϊ����ģʽ
	BOOL m_bNav;
	//��ǰ���Ƶ�TURNLIST��λ��
	POSITION m_posTurnList;
	//�Ƿ���ʾ��ѡ�е�������
	BOOL m_bShowTip;
	//�Ƿ���ʾ���ѡ�еĵ�·
	BOOL m_bShowFocusRut;
	//��ʾTIP�����ĵ�λ��
	CPoint m_TipPt;
	//TIP����ʾ������
	swString m_strFindName;
	//���ѡ�еĽ����·������
	ST_FOCUS_RUT m_FocusRut;
	//ָ��ǿ�ƻ��Ƶĵ�·�����ܵ�·��
	CGIS_FeatureRut* m_pSpecialRut;
	//�Ƿ���ʾǿ�ƻ��Ƶĵ�·
	BOOL m_bShowSpecalRut;
	//TIP���ڵĵ���
	CGIS_Feature *m_pFeatureTip;
	//TIP��ָ���������
	EnQueryObjType m_TipFeaType;
	//TIP����λ�õľ�γ��
	ST_GEO_PTXY m_ptTipXy;

	//�Ƿ���ʾָ����
	BOOL m_bShowMapCompass;
	//��ʾָ�����λ��
	CPoint m_ptMapCompass;
    //ָ�����ͼ��
	CImageList	m_CompassImageList;   
	//���ƥ�䵽�ĺ����нڵ��λ��
	POSITION m_PathNodePos;
	//���ƥ�䵽�ĺ��ߵ��λ��
	POSITION m_PathPtPos;
	//���ں�����ʱ�����һ��������ߵĽڵ��λ��
	POSITION m_PathNearNodePos;
	//�����·��ͼ����Ϣ
	CGIS_LayerInfo* m_pSpRutLayerInfo;
	//�Ƿ���Ҫ��ջ���ͼ��
	long m_bClearBuf;
	//������ʻ����1��·��ͷ����·��β 2��·��β����·��ͷ0��δȷ��
	int	 m_nCarRunDirect;
	//������ʻ����һ���ڵ��
	short m_nCarRunNextPt;
	//���轹����ʾ����Ȥ���б�
	CPtrList m_FocusPtSet;
	//RichPOI��ʾ����ģ��
	CRichPOIView m_RichPOIView; // scott 2009.3.9
	//·������ͼ�Ĳ���
	ST_EXPAND_MAPSET m_ExpandMapCfg;
	//��ǰ·������ͼ��·���ں����е�λ��
	POSITION m_CrossPos;
	//��ǰ�Ѿ����ƺõ�·������ͼ��λͼ
	CBitmap* m_pCrossBmp;
	CBitmap* m_pCrossBmpOld;
//	//��ǰ�Ѿ����ƺõ�·������ͼ��λͼ(�޳���)
//	CBitmap* m_pCrossBmpWithoutCar;
//	CBitmap* m_pCrossBmpWithoutCarOld;
	//·������ͼ����Ӧ���ڴ�DC
	CDC* m_pDCCross;
//	//·������ͼ����Ӧ���޳�����ʾ��DC
//	CDC* m_pDCCrossWithoutCar;
	//·������ͼ�ķ�Χ
	ST_GEO_RECT m_rtMap;
	//·������ͼ�ı�����
	double m_dbExpandScale;
	//·������ͼ�ĽǶ�����ֵ
	double m_dbSinAngle;
	//·������ͼ�ĽǶ�����ֵ
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
	//������ת�Ƕ�ȷ�����Ʒ�Χ
	ST_GEO_RECT MaxNewRect(RECT rect);
	void  DrawOutlineForChars(CDC* pDC, int x, int y, swString& strContext, COLORREF ExCol, COLORREF InCol );
	void  DrawSortLayer(ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer  );
	void  DrawBackRut( ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw, double dbScale, int nBID,CGIS_MapLayer *pLayer);
	//���ӻ��Ƶ�POI
	BOOL  AddDrawPOI( ST_PT_DRAW *ptDraw );
	//�������POI��
	void  ClearDrawPOI( );
	//����POI
	void  DrawPOI( );
	//�ڵ��������л���·��ָʾ��ͷ
	void DrawTurnSign( CDC* pDC );
	//��ʾ�������λ�õĵ�������
	void ShowCursorFind( CDC* pShowDC );
	//������ʾ������ڵ�·
	void ShowCursorFindRut( CDC* pShowDC );
	//��ʾĳ�ض��ĵ�·�����ܵ�·��
	void ShowSpecialRut( CDC* pShowDC );
	//У�����λ��
	void ReSetCursor( );
	//�����Ķ������Ƿ�������ķ���һ��
	BOOL ReadDirect(int nStartx , int nStarty , int nEndx , int nEndy );
	//���ƽ�����Ȥ��
	void ShowFocusPOI(CDC* pShowDC);
	//����RICHPOI
	void DrawRichPOI(CPtrList* pListRichPOi,ST_GEO_RECT rtOld, ST_GEO_RECT rtDraw);
	//����ͼƬID��������ͼλͼ��Դ
	BYTE* GetExpandImgByID( int nImgID ,DWORD& dw2Dor3D );
	//��γ��ת��Ϊ·������ͼ�е�����λ��
	void ExpandMapToView( ST_GEO_RECT rtMap , double dbExpandScale 
		, double dbSin , double dbCos , float fx , float fy , int& nx , int& ny );

struct ST_IMGINDEX
{
	DWORD	dwID;		//�ļ�id
	DWORD	dw2Dor3D;	//2-2Dģʽͼ;3-3Dģʽͼ
	DWORD	dwOffset;	//ƫ�ƣ����ļ�ͷ��ʼ
	DWORD	dwLen;		//ѹ��������
	DWORD	dwUpCompLen;//ѹ��ǰ����ѹ�󣩳���
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


