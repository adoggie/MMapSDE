
/*
map_canvas.h
��ͼ����

ʹ�� map_canvas.h ���������ǰ�˰��� 
*/

#ifndef _MAP_CANVAS_H
#define _MAP_CANVAS_H
#include "map_view.h"
#include <wx/wx.h>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

#include <map>
#include <wx/wxsqlite3.h>
#include <boost/shared_ptr.hpp>
using namespace boost;

#include "map_symbol.h"
#include "map_layer.h"
#include "map_map.h"
#include "map_tools.h"
#include "map_user.h"
#include "maptool_poi.h"
#include "maptool_barrier.h"
#include "maptool_editor.h"
#include "map_plugin.h"

#include "map_scalebar.h"

#include <geos.h>
using namespace geos;

class GeoMapCanvas;
class GeoMapTool_Select;

struct MapSelectedInfoT{
	GeoMapSymbolList	symbols;	//ͼ��ѡ���symbols
	GeoMapSymbolType	symbolType;
};

class GeoMapCanvasClient{
public:
	GeoMapCanvasClient(){};
	virtual void OnMouseEvent(GeoMapCanvas* canvas,wxMouseEvent& evt){}
	virtual void OnSelectObject(GeoMapCanvas* canvas,MapSelectedInfoT& info){}	//ѡ���ͼ���ݶ���
};

typedef std::list< GeoMapCanvasClient* > GeoMapCanvasClientListT;

struct GeoMapCanvasConfigT{
	shared_ptr<wxSQLite3Database> db;	
};

class GeoMapCanvas:public wxPanel{
public:
	GeoMapCanvas(wxWindow* parent);
	GeoMapView*	getView();
	bool init(GeoMapCanvasConfigT& conf = GeoMapCanvasConfigT());
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& evt);
	void OnMouseEvent(wxMouseEvent& evt);
	void OnMouseWheel(wxMouseEvent& evt);
	void OnEraseBG(wxEraseEvent& evt);

	GeoRectT	getGeoRect();			//��ȡ��������
	ViewRectT	getViewRect();		//��ȡ��Ļ��ʾ����
	shared_ptr<GeoMapLayerBase> getLayer(GeoMapLayerType type);
	wxDC*		getDrawDC();
	ViewPointT	xy_Geo2View( GeoPointT xy);
	GeoPointT		xy_View2Geo( ViewPointT xy);
	GeoRectT rect_View2Geo(ViewRectT & vwrc);
	ViewRectT  rect_Geo2View( GeoRectT& georc);
	shared_ptr<wxSQLite3Database>	getDB();
	shared_ptr<GeoMapToolBase>&		getMapTool();
	ViewPointT	getViewCenterXY();
	GeoPointT		getGeoCenterXY();
	void	update(bool redrawView = true);
	void  updateRect(const ViewRectT& rc);	//ʹ�ñ���ˢ�¾ֲ�����
	void  setMapTool(GeoMapToolTypeT tool);	//���õ�ǰ����
	shared_ptr<GeoMapToolBase>	getMaptool(GeoMapToolTypeT tool);
	shared_ptr<GeoMapToolBase>   getCurrentMapTool();
	//���µ�����Ϊ����
	void addClient(GeoMapCanvasClient* c);
	void delClient(GeoMapCanvasClient* c);
	//������ͼ��֮��ĵ������
	double getGeoDistance( const ViewPointT& p1,const ViewPointT& p2);
	double getGeoDistance( const std::list<shared_ptr<ViewPointT> >& ptlist);
	
	void	scaleToViewRect(const ViewRectT& rc,bool zoomin=true);			//���ŵ�ͼ��ָ������ͼ����
	void	updateRect(const ViewRectT& rc,GeoMapSymbolBase* ignore=NULL); //���»��Ʋ����Զ���
	shared_ptr<GeoMapLayerBase> getTopmostLayer();
	shared_ptr<GeoMapLayerBase> getLowestLayer();
	
	void	setGeoScense(const mapGeoScenseT&);	//���õ�ͼ��ʾλ�� 
	GeoMapLayerList&	getLayers();

	friend class GeoMapTool_Select;
	GeoMapSymbolList  mapSelectViewPoint(const ViewPointT& pt,wxMouseEvent * evt,GeoMapTool_Select* ,bool multiple=false/*�Ƿ�ѡ���Ƕ�ѡ*/);	//
	GeoMapSymbolList  mapSelectViewRect(const ViewRectT& rc,wxMouseEvent * evt,GeoMapTool_Select*);		//ѡ���ͼ����Χ
	
	void prepareLayerData();
	shared_ptr<GeoMapSymbolBase> getLayerSymbol(const ViewPointT& pt); //��ȡ�������ͼ�����
	int		getScaleLevel();
	double getScale();		//��ȡ��ǰ��ͼ����ĵ���ɼ����ű���
	void addPlugin(GeoMapCanvasPlugin* p);
protected:
	//void drawLayers();
	
private:
	
	DECLARE_EVENT_TABLE()
private:
	GeoMapView*		_mapview;
	GeoMapCanvasConfigT	_conf;
	GeoMapLayerList			_layers;
	unsigned int		_layersMask;
	wxMemoryDC			_drawDC;
	shared_ptr<wxBitmap>	_drawBitmap;
	
	shared_ptr<GeoMapToolBase>	_maptool;

	shared_ptr<GeoMapTool_Pan>	_maptool_pan;
	shared_ptr<GeoMapTool_ZoomIn> _maptool_zoomin;
	shared_ptr<GeoMapTool_ZoomOut> _maptool_zoomout;
	shared_ptr<GeoMapTool_MeasureLine> _maptool_measure;
	shared_ptr<GeoMapTool_Select>		_maptool_select;
	shared_ptr<GeoMapTool_NewBarrier> _maptool_newBarrier;
	shared_ptr<GeoMapTool_NewPoi>		_maptool_newPoi;
	shared_ptr<GeoMapTool_ObjectEditor>	_maptool_objeditor;
	GeoMapCanvasClientListT			_clientList;
	wxCriticalSection						_cs_clients;

	std::vector<GeoMapCanvasPlugin*> _plugins;

	MapCanvasPlugin_ScaleBar	_scalebar;
};


#endif

