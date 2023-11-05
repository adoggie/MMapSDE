
#ifndef _MAP_LAYER_H
#define _MAP_LAYER_H

#include "map_symbol.h"
#include "map_map.h"

#include <wx/wx.h>

#include <vector>
#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
using namespace boost;

enum GeoMapLayerType{
	GEOMAP_LAYER_NULL =0x00,
	GEOMAP_LAYER_USER_POI =0x01,
	GEOMAP_LAYER_BARRIER = 0x02,
	//GEOMAP_LAYER_ROUTE = 0x04,
	GEOMAP_LAYER_VEHICLE = 0x04,
	//GEOMAP_LAYER_TRACK = 0x10,		//gps车辆实时跟踪图层，实现车辆和轨迹绘制
	GEOMAP_LAYER_LOWEST = GEOMAP_LAYER_USER_POI,
	GEOMAP_LAYER_TOPMOST = GEOMAP_LAYER_VEHICLE,
	GEOMAP_LAYER_LAST = 0xff
};

struct GeoMapLayerFontT{
	wxString	font;
	int				size;
};

enum GeoMapLayerTextAlignT{
	GEOMAP_TEXTALIGN_LEFT = 0x1,
	GEOMAP_TEXTALIGN_BOTTOM ,
	GEOMAP_TEXTALIGN_RIGHT ,
	GEOMAP_TEXTALIGN_TOP 
};

class GeoMapCanvas;
class GeoMapLayerBase{
public: 
	GeoMapLayerBase();
	//void   chainLayer(GeoMapLayerType top ,GeoMapLayerType bottom );
	GeoMapLayerBase* getTopLayer();
	GeoMapLayerBase* getBottomLayer();
	void		setCanvas( GeoMapCanvas*  canvas);
	GeoMapCanvas*		getCanvas();
	inline GeoMapLayerType		getType();
	virtual void draw();									//重新绘制
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
	virtual bool	textVisible();	//设置文本是否显示
	void	setFont(GeoMapLayerFontT& font);
	GeoMapLayerFontT& getFont();
	GeoMapSymbolList	getSymbols();
	GeoMapLayerTextAlignT	getTextAlign();
	void setTextAlign(GeoMapLayerTextAlignT align);
	virtual GeoMapSymbolList findSymbols(const ViewRectT& rc,GeoMapSymbolBase* ignore=NULL);//选取矩形框内对象
	virtual void lockSymbols(bool lock=true);
	virtual void	changeSize(int cx,int cy);
	wxDC*		getDrawDC();
	wxBitmap*	getDrawBitmap();
	virtual void  invalidViewRect(const ViewRectT& rect,GeoMapSymbolBase* ignoreDraw=NULL/*忽略绘制*/);
	virtual void setSymbolGeoXY(const GeoPointT& xy,int sid,bool& viewXyChanged/*是否改变了视图位置*/,int time=0);	//设置对象地理座标,设置完座标即可请求图层进行刷新
	virtual void clearSymbols();		//清除图层数据
	virtual void redraw();
	virtual shared_ptr<GeoMapSymbolBase>	getSymbol(const ViewPointT& pt); //获取指定xy上的符号对象
	void		setSymbolVisible(int sid,bool visible = true);	//设置显示和隐藏图层图标
	virtual void append(shared_ptr<GeoMapSymbolBase>& symbol);	//添加对象到指定的图层
	virtual shared_ptr<GeoMapSymbolBase>	getSymbol(int sid); //获取指定符号对象
	virtual shared_ptr<GeoMapSymbolBase> detachSymbol(int sid);	//删除符号
	virtual void attachSymbol(const shared_ptr<GeoMapSymbolBase> symbol); //添加符号
	
protected:
	//virtual GeoMapSymbolList getSymbolsInViewRect
	virtual void	before_drawSelf();
	virtual void  after_drawSelf();
protected:
	GeoMapSymbolList		_symbols;
	wxCriticalSection		_cs_symbols;

	GeoMapCanvas*				_canvas;
	bool						_visible;
	bool						_textVisible;
	GeoMapLayerType	_type;
	//GeoMapLayerType	_topLayer;
	//GeoMapLayerType	_bottomLayer;
	GeoMapLayerFontT	_font;
	GeoMapLayerTextAlignT	_textAlign;
	wxMemoryDC			_drawDC;
	shared_ptr<wxBitmap>	_drawBitmap;
};

class GeoMapLayer_Vehicle:public GeoMapLayerBase{
public:
	GeoMapLayer_Vehicle();
	void draw();
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
	GeoMapSymbolList findSymbols(const ViewRectT& rc); //区域查车
	void appendSymbols( std::list< shared_ptr<gvms::clientVehicleT> > & vehicles); //添加新的车辆到地图
	void	setDirection(float angle,int vid);		//设置行车方向
	bool	textVisible();

	void	showTrackPath(int vid,bool show);
	void	hideTrackPath();	///全部隐藏
	void	setTrackPathColor(int vid,wxColour& color);
	void	setTrackPathWidth(int vid,int width);
	void	before_drawSelf();

	void	setReplayMode(bool mode);
	bool  isReplayMode();
	void  setFocus(int vid) ; //设置车辆为焦点
private:
	bool		_replaying; //是否历史回放状态 
	wxFont	_fntFouse;	//焦点车辆文本显示属性
	int			_focuseVehicle;	//焦点车辆
};

class GeoMapLayer_UserPoi:public GeoMapLayerBase{
public: 
	GeoMapLayer_UserPoi();
	void draw();
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
	GeoMapSymbolList findSymbols(const ViewRectT& rc); 
	bool	textVisible();
private:
};

class GeoMapLayer_Barrier:public GeoMapLayerBase{
public:
	GeoMapLayer_Barrier();
	void draw();
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
	GeoMapSymbolList findSymbols(const ViewRectT& rc); 
	void	deleteBarriersByGroup(int bgId);		//删除指定围栏组的围栏
	void	before_drawSelf();
	void  after_drawSelf();
private:
	wxMemoryDC			_tempDC;
	wxBitmap				_tempBitmap;	
};

class GeoMapLayer_Route:public GeoMapLayerBase{
public:
	GeoMapLayer_Route();
	void draw();
	virtual void loadSymbols(GeoRectT& rect);
	virtual void setVisible(bool visible = true);
	virtual void drawRect(ViewRectT& rect);
	virtual void setTextVisible(bool visible = true);
private:
};

//////////////////////////////////////////////////////////////////////////
//车辆跟踪图层也可用于历史记录回放
//启动车辆跟踪时必须在车辆层将跟踪车辆隐藏
//轨迹跟踪数据显示属性
struct GeoMapTrackPathAttrT{
	wxPen	pen;
	GeoMapTrackPathAttrT(){
		pen.SetColour(wxT("RED"));
		pen.SetWidth(3);
	}
};

struct GeoMapTrackPointT{
	GeoPointT geopt;	//位置
	int				time;		//时间 1970 
};


typedef std::map< GeoMapLayerType, shared_ptr<GeoMapLayerBase> > GeoMapLayerList;



#endif
