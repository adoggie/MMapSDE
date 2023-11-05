
#ifndef _MAP_VIEW_H
#define _MAP_VIEW_H

#include <afxwin.h>
#include <afxcoll.h>
#include <afxcmn.h>

#include "SDEControler.h"
#include "GeoView.h"
#include "GIS_NetWork.h"
#include "GIS_WayPoint.h"
#include "GIS_NetworkRoute.h"
#include "GIS_FeatureRoute.h"

#include "map_tools.h"
#include "map_map.h"




class GeoMapViewImpl:public CGeoView,public GeoMapView{
public:
	GeoMapViewImpl(GeoMapLib*	maplib);
	~GeoMapViewImpl();
	
	bool init(GeoMapViewConfigT& conf);	
	void	getBitmap(GeoMapBitmapT& bitmap);	
	void	zoomIn(int cx,int cy,int level = 1);
	void	zoomInRect(ViewRectT rect);	
	void	zoomOut(int cx,int cy,int level = 1);
	void	zoomOutRect(ViewRectT rect);
	void	xyV2M(int x,int y,GeoPointT& point);
//	void	xyM2V(const GeoPointT& point,int &x,int &y);
	void	xyM2V(double px,double py,ViewPointT& xy);	
	void	changeSize(int cx,int cy);
	GeoRectT getGeoRect();
	void			moveGeoCenterXY(GeoPointT xy);
	void setScale(double dx, double dy,int scalelevel);
	void getOffset(float &x, float &y);
protected:
	void draw();
	void OnSize( UINT, int, int );
	void OnPaint( );
	void SetDrawStatic();
	void SetMapColor(CGIS_LayerInfo *layer, COLORREF brCol, COLORREF penCol);
	void SetMapParticular();
	void GetDefaultLevel(int *nLevel);
//	DECLARE_DYNCREATE(GeoMapView)
	DECLARE_MESSAGE_MAP()
	static wxCriticalSection	_drawMutex;  //会话全局锁，防止绘制时共享数据区被破坏
private:
	int m_OldScaleLevel;
	int m_nAngleOld;
	
};




#endif

