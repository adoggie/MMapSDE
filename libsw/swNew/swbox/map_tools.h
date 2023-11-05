
#ifndef _MAP_TOOLS_H
#define _MAP_TOOLS_H

#include "map_map.h"
#include "map_symbol.h"

#include <wx/wx.h>
#include <vector>
#include <list>
#include <string>
#include <map>

#include <wx/wxsqlite3.h>
#include <boost/shared_ptr.hpp>
using namespace boost;

enum GeoMapToolTypeT{
	GMTT_NULL = -1,
	GMTT_PAN=0,
	GMTT_ZOOMIN,				//放大
	GMTT_ZOOMOUT,				//缩小
	GMTT_MEASURE_LINE,	//测距
	GMTT_SELECT,				//点选或者区域选择工具
	GMTT_EDITOR,				//地图对象编辑工具,poi地标支持拖动,barrier支持拖动和改变节点形状
	GMTT_NEW_POI,				//添加POI
	GMTT_NEW_BARRIER,		//添加围栏
};

/*
measure_line: 
返回: 
	DIST_POINTS	: stringline 的长度
	DIST_FREE;		最后一个点与当前鼠标点的距离
*/
class GeoMapToolBase;
struct GeoMapTool_Event{
	GeoMapToolTypeT	src;	//事件源类型
	std::map<wxString,wxString>		msg;	//事件传递消息内容
	wxMouseEvent * evt;
	GeoMapToolBase* tool;
};

class GeoMapToolClient{
public:
	GeoMapToolClient(){}
	virtual void OnMapToolEvent( GeoMapTool_Event& evt){}
};

//// GeoMapToolBase //////////////////

class GeoMapCanvas ;
class GeoMapCanvas;
class GeoMapToolBase{
public:
	GeoMapToolBase();
	virtual void	set(GeoMapCanvas*) ;	
	virtual void  onMouseEvent(wxMouseEvent & evt);
	GeoMapToolTypeT	getType();
	void	addClient( GeoMapToolClient*);
	void  delClient(GeoMapToolClient*);
	virtual void onMapRedraw(const ViewRectT& rect=ViewRectT());//当地图进行重绘的时候通知tool,用于tool绘制时由于地图刷新导致tool绘制行为失败
protected:
	void	init();
	virtual void	drawRect(const ViewRectT& rect,wxDC * dc,bool restore=false);			//绘制举行区域
	virtual void	restoreRect(const ViewRectT& rect);		//回复之前canvas区域场景
	GeoMapCanvas*		_canvas;
	shared_ptr<wxCursor>			_cursor;
	wxString				_cursorName;
	shared_ptr<ViewPointT>	_startPt;
	GeoMapToolTypeT				_type;

	std::list<GeoMapToolClient*>			_clients;
	wxCriticalSection								_cs_clients;
};

inline GeoMapToolTypeT	GeoMapToolBase::getType(){
	return _type;
}

//平移
class GeoMapTool_Pan:public GeoMapToolBase{
public:
	GeoMapTool_Pan();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
private:
	
};
//放大
class GeoMapTool_ZoomIn:public GeoMapToolBase{
public:
	GeoMapTool_ZoomIn();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
};
//缩小
class GeoMapTool_ZoomOut:public GeoMapToolBase{
public:
	GeoMapTool_ZoomOut();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
};

enum GeoMapToolSelectTypeT{
	GMTST_NULL = 0x00,
//	GMTST_MAP =0x01,
	GMTST_POI = 0x02,
	GMTST_VEHICLE =0x04,
	GMTST_BARRIER = 0x08
};

//指定区域选择多种类型
class GeoMapTool_Select:public GeoMapToolBase{
public:
	GeoMapTool_Select();
	void	set(GeoMapCanvas*);
	void	setSelectTypes(unsigned int );			//设置可选择内容,多类型组合
	unsigned int getSelectTypes();
	void  onMouseEvent(wxMouseEvent & evt);
private:
	unsigned int	selectTypes;		//选择对象类型集合
	GeoMapSymbolList			_cachedlist;
};

//测线距,多折线 stringline
class GeoMapTool_MeasureLine:public GeoMapToolBase{
public:
	GeoMapTool_MeasureLine();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
private:
	void	clearScene();
private:
	std::list<shared_ptr<ViewPointT> >	_ptlist;		//多折线顶点
	shared_ptr<ViewPointT>		_lastpt;
	
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////





#endif
