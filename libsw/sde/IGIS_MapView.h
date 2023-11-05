#ifndef _IGIS_MAPVIEW_H
#define _IGIS_MAPVIEW_H

#include "swBase.h"
#include "GeoGlobe.h"
#include "IGIS_Message.h"
class IGIS_Map;

class swEXPORT IGIS_MapView
{
public:
	IGIS_MapView();
	virtual ~IGIS_MapView();
public:
	virtual BOOL Update(IGIS_Message* pMessage)   = 0; //ˢ��
	virtual double GetCurrentScale() = 0;//�õ���ǰ������
	virtual UINT GetCurrentScaleLevel() = 0;//�õ���ǰ�����߼���
	virtual ST_GEO_RECT GetCurrentDrawrect() = 0 ;//�õ���ǰ���ӷ�Χ
	virtual ST_GEO_RECT GetRotateRect() = 0;
	virtual UINT GetAngle() = 0;
	virtual void PreparePen(ST_GEO_PEN pen) = 0;//׼������
	virtual void DrawLine(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum, int nType) = 0;
	virtual void AddAnnotate(ST_GEO_PTXY* pPtList, int nPtNum, swString pText, BYTE byLevel = 0 ,BOOL bSameDirect = TRUE) = 0;//��ע����Ϣ
	virtual void ReleasePen() = 0;
	virtual void DrawAnnotate() = 0;
	virtual void PrepareBrush(ST_GEO_BRUSH brush) = 0;
	virtual void ReleaseBrush() = 0;
	virtual void DrawPoly(ST_GEO_PTXY* pPtList, int nPtNum, int* pPart, int nPartNum) = 0;
	virtual void XyMapToView(double dx, double dy, int& nx, int& ny) = 0;	
//	//��ͼ����ת������������
//	virtual void XyMapToDraw(double dx,double dy,int &nx,int &ny)=0;
	virtual RECT GetViewPlot() = 0;
	virtual void DrawLine(ST_GEO_PTXY* pPtList, int nPtNum) = 0;
	virtual BOOL SetMap(IGIS_Map* pMap) = 0;
	virtual ST_GEO_RECT GetOldDrawRect() = 0;
	virtual BOOL ReleaseMap(IGIS_Map* pMap) = 0; 
	virtual BOOL SetDrawState(UINT nLayID,BOOL nState) = 0;
	virtual BOOL GetDrawState(UINT nLayID) = 0;
	virtual BOOL SetLonLatLevel(ST_LonLatLinelevel st_Drawlevel) = 0;
	virtual ST_LonLatLinelevel GetLonLatLevel() = 0;
	virtual double TurnScale(UINT scalelevel) = 0;
};

#endif 
