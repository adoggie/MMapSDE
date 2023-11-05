
#ifndef _MAP_SYMBOL_H
#define _MAP_SYMBOL_H

//#include "map_resource.h"
#include "gvms_cmc.h"
#include "map_map.h"
#include <vector>
#include <map>
#include <list>
#include <wx/wx.h>

#include <geos.h>
using namespace geos;//::geom;
//using namespace geos::io;
#include "ice/sts.h"

#include <boost/shared_ptr.hpp>
using namespace boost;

enum GeoMapSymbolType{
	GEOMAP_SYMBOL_NULL =0x00,
	GEOMAP_SYMBOL_VEHICLE = 0x01,		//车辆
	GEOMAP_SYMBOL_USER_POI,					//用户兴趣点
	GEOMAP_SYMBOL_BARRIER,					//电子围栏
	GEOMAP_SYMBOL_ROUTE,						//跟踪轨迹
	GEOMAP_SYMBOL_LAST = 0xffff			
};
class GeoMapLayerBase;
class GeoMapLayer_Vehicle;
class GeoMapLayer_UserPoi;
class GeoMapLayer_Barrier;

struct SysResIcon;
class GeoMapSymbolBase{
public:
	GeoMapSymbolBase();
	GeoMapSymbolType	getType();
	virtual void	setIcon(shared_ptr<SysResIcon>& icon);
	//virtual void	setText(wxString & text);
	virtual void	setLayer(GeoMapLayerBase* layer);
	virtual void	setGeoXY(double x,double y,bool & viewPtChanged);	//设置经纬度 
	virtual void  setViewXY(int x, int y);			//移动图标
	virtual void	draw();						//简单的叠加绘制
	virtual void	drawCompound();		//单独图标区域绘制，合成上下级图层并绘制,支持任意图层的symbol的移动
	virtual geom::Envelope		getEnvelope();			//图上边界矩形
	virtual geom::Coordinate	getViewXY();		//图上中心点
	virtual geom::Coordinate	getGeoXY();				//经纬度坐标
	
	
	friend class GeoMapLayerBase;
	friend class GeoMapLayer_Vehicle;
	friend class GeoMapLayer_UserPoi;
	friend class GeoMapLayer_Barrier;
		
	virtual bool	isIntersected(const ViewRectT & rc);		//是否与指定矩形有交集
	virtual bool	isGeoIntersected(const GeoRectT & rc);		//地理坐标是否相交
	GeoPointT		getGeoPoint();
	virtual void	setFocus(bool focus = true);		//图标被选择
	virtual bool focused();												//是否当前已经聚焦
	
	virtual wxString getText();
	virtual int		getObjectId(){	return 0;}
	virtual void init();		//创建symbol并执行初始化
	GeoMapLayerBase* getLayer();
	virtual bool	isIntersected(const ViewPointT& pt); //检测点是否在符号上
	virtual void	setVisible(bool visible=true);
	bool	getVisible();
	void  setTimestamp(int t){ _timestamp = t;}
	int		getTimestamp(){ return _timestamp;}
	void	setFont(const wxFont& font);
	wxFont& getFont();
	void setTextColor(const wxColour& color);
	wxColour getTextColor();
protected:
	
	virtual void	drawIcon();
	virtual void	drawText();
	void	drawFocus();
	virtual ViewRectT getIconRect();
	ViewRectT	getTextRect();
protected:
	GeoMapSymbolType	_type;
	GeoMapLayerBase*	_layer;	//渲染图层
	shared_ptr<SysResIcon>	_icon;
/*
	wxString					_name;		//名称
	unsigned int			_sid;
	unsigned int			_clientid;	//客户编号
	
	bool							_visible;
	geom::Coordinate	_geo_xy;
	bool							_focused;		//聚焦
	ViewPointT				_viewxy;
	*/
	geom::Coordinate	_geo_xy;
	bool		_visible;
	int			_timestamp;
	wxFont	_font;
	wxColour _textColor; //文本颜色
};

typedef std::list< shared_ptr<GeoMapSymbolBase> > GeoMapSymbolList;

class GeoMapSymbol_Vehicle:public GeoMapSymbolBase{
public:
	GeoMapSymbol_Vehicle();
	void	draw();						
	void	drawCompound();		
	void	drawText();
	geom::Envelope		getEnvelope();			//图上边界矩形
	friend class GeoMapLayer_Vehicle;
	wxString getText();
	int		getObjectId();
	void init();	
	void	setDirection(float angle);		//设置行车方向

	void	showTrackPath(bool show= true);	
	void  setTrackPathColor(wxColour& color);
	void	setTrackPathWidth(int width);		///<轨迹线宽度
	void	drawTrackPath();
	void	resetTrackPoints();							///<清除轨迹点
	void  addTrackPoint(const TrackGeoPointT& gpt);	///<添加轨迹点
	void	addTrackPoints(const std::list<TrackGeoPointT>& gpts); ///<添加多个轨迹点集
	void	setStatus(eVehicleStatusT vs);
	void	setCenter();	///<地图居中
private:
	bool	setIcon();
public:					
	gvms::clientVehicleT	_vechile;
	bool		_showTrackPath;					///<是否显示轨迹线
	wxColour	_trackPathColor;			///<轨迹线颜色
	int			_trackPathWidth;				///<轨迹线宽度
	std::list<TrackGeoPointT>	_geoPoints;	///<跟踪轨迹点集合
	wxCriticalSection		_points_mutex;
	float			_direction;	///<方向
	eVehicleStatusT	_status;
};

class GeoMapSymbol_UserPoi:public GeoMapSymbolBase{
public:
	GeoMapSymbol_UserPoi();
	void	draw();						
	void	drawCompound();			
	geom::Envelope		getEnvelope();
	friend class GeoMapLayer_UserPoi;
	void	setGeoXY(double x,double y,bool & viewPtChanged);	//设置经纬度 
	void  setViewXY(int x, int y);
	geom::Coordinate	getGeoXY();				//经纬度坐标
	int		getObjectId();
	void init();	
	wxString getText();
	void	drawIcon();		//强制重新加载icon，用于修改icon之后的update
public:
	unsigned int			_mask_bit;
	shared_ptr<gvms::clientPoiDataT>	_userpoi;
};

class GeoMapSymbol_Barrier:public GeoMapSymbolBase{
public:
	GeoMapSymbol_Barrier();
	void	draw();						
	void	drawCompound();		
	bool	isIntersected(const ViewRectT & rc);		//是否与指定矩形有交集
	bool	isIntersected(const ViewPointT& pt);		//是否点落在多边形内
	bool	isGeoIntersected(const GeoRectT & rc);		//地理坐标是否相交
	geom::Envelope		getEnvelope();			//图上边界矩形
	geom::Coordinate	getGeoXY();				//经纬度坐标
	int		getObjectId();
	void	init();	
	void	setGeoXY(double x,double y,bool & viewPtChanged);	//设置经纬度 
	void  setViewXY(int x, int y);
	ViewRectT getIconRect();
	shared_ptr<geom::Polygon>	getViewPolygon();
public:
	shared_ptr<geom::Polygon>		_geo_polygon;		//地理围栏
	//shared_ptr<geom::Polygon>		_view_polygon;		//视图多边形 
	shared_ptr<gvms::clientBarrierT>	_barrier;
};

class GeoMapSymbol_Route:public GeoMapSymbolBase{
public:
	GeoMapSymbol_Route();
	void	draw();						
	void	drawCompound();		
	bool	isIntersected(const ViewRectT & rc);		//是否与指定矩形有交集
	bool	isGeoIntersected(const GeoRectT & rc);	
	void init();	
	
};







#endif

