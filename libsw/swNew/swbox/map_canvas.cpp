
#include "map_canvas.h"
#include "sysconfig.h"


BEGIN_EVENT_TABLE(GeoMapCanvas, wxPanel)
	EVT_PAINT(GeoMapCanvas::OnPaint)
	EVT_ERASE_BACKGROUND(GeoMapCanvas::OnEraseBG)
	EVT_SIZE(GeoMapCanvas::OnSize) 
	EVT_MOUSE_EVENTS(GeoMapCanvas::OnMouseEvent)
	EVT_MOUSEWHEEL(GeoMapCanvas::OnMouseWheel)
END_EVENT_TABLE()


GeoMapCanvas::GeoMapCanvas(wxWindow* parent):wxPanel(parent){
	_mapview = NULL;
}

bool GeoMapCanvas::init(GeoMapCanvasConfigT& conf){
	_conf = conf;
	_mapview = GeoMapLib::instance().newMapView();
	GeoMapViewConfigT vc;
	vc.hWnd = GetHandle();
	_mapview->init(vc);
	if( !_conf.db.get()  ){
		_conf.db = gvmsSysConfig::instance().getDB(); 
	}
	/// -- setup map layers --	
	_layers[GEOMAP_LAYER_USER_POI] = shared_ptr<GeoMapLayer_UserPoi>(new GeoMapLayer_UserPoi());
	_layers[GEOMAP_LAYER_USER_POI]->setCanvas(this);
	//_layers[GEOMAP_LAYER_USER_POI]->chainLayer(GEOMAP_LAYER_ROUTE,GEOMAP_LAYER_NULL);
	_layers[GEOMAP_LAYER_BARRIER] = shared_ptr<GeoMapLayer_Barrier>(new GeoMapLayer_Barrier());
	_layers[GEOMAP_LAYER_BARRIER]->setCanvas(this);
	//_layers[GEOMAP_LAYER_BARRIER]->chainLayer(GEOMAP_LAYER_NULL,GEOMAP_LAYER_USER_POI);
	
	//_layers[GEOMAP_LAYER_ROUTE] = shared_ptr<GeoMapLayer_Route>(new GeoMapLayer_Route());
	//_layers[GEOMAP_LAYER_ROUTE]->setCanvas(this);
	//_layers[GEOMAP_LAYER_ROUTE]->chainLayer(GEOMAP_LAYER_VEHICLE,GEOMAP_LAYER_USER_POI);
	_layers[GEOMAP_LAYER_VEHICLE] = shared_ptr<GeoMapLayer_Vehicle>(new GeoMapLayer_Vehicle());
	_layers[GEOMAP_LAYER_VEHICLE]->setCanvas(this);

	//_layers[GEOMAP_LAYER_TRACK] = shared_ptr<GeoMapLayer_Track>(new GeoMapLayer_Track());
	//_layers[GEOMAP_LAYER_TRACK]->setCanvas(this);
	
	//_layers[GEOMAP_LAYER_VEHICLE]->chainLayer(GEOMAP_LAYER_BARRIER,GEOMAP_LAYER_ROUTE);


	_layersMask = GEOMAP_LAYER_VEHICLE|GEOMAP_LAYER_USER_POI|GEOMAP_LAYER_BARRIER;//|GEOMAP_LAYER_ROUTE;
	// create maptools
	_maptool_pan = shared_ptr<GeoMapTool_Pan>(new GeoMapTool_Pan);
	_maptool_zoomin = shared_ptr<GeoMapTool_ZoomIn>(new GeoMapTool_ZoomIn) ;
	_maptool_zoomout = shared_ptr<GeoMapTool_ZoomOut>(new GeoMapTool_ZoomOut) ;
	_maptool_measure = shared_ptr<GeoMapTool_MeasureLine>(new GeoMapTool_MeasureLine) ;
	_maptool_select = shared_ptr<GeoMapTool_Select>(new GeoMapTool_Select);
	
	_maptool_objeditor = shared_ptr<GeoMapTool_ObjectEditor>(new GeoMapTool_ObjectEditor);	
	_maptool_newBarrier = shared_ptr<GeoMapTool_NewBarrier>(new GeoMapTool_NewBarrier) ;
	_maptool_newPoi =		shared_ptr<GeoMapTool_NewPoi>(new GeoMapTool_NewPoi)		;
	//准备图层信息
	//prepareLayerData();

	addPlugin(&_scalebar);
	return true;
}

void GeoMapCanvas::prepareLayerData(){
	GeoMapLayerBase * layer;
	layer = this->getLowestLayer().get();
	//自下而上绘制每一图层
	while(true){
		layer->loadSymbols( getGeoRect());
		layer = layer->getTopLayer();
		if( layer == NULL){
			break;
		}
	}
}
GeoMapView*	GeoMapCanvas::getView(){
	return _mapview;
}

void GeoMapCanvas::OnPaint(wxPaintEvent& event){
	event.Skip(); //不调用将导致窗体无法关闭
	if( !_mapview ){
		return ;
	}
//	drawLayers();
//	wxClientDC dc(this);	
//	dc.Blit(0,0, GetClientRect().GetWidth(),GetClientRect().GetHeight(),&_drawDC,0,0);
	update(false);
/*
	GeoMapBitmapT data;
	_mapview->getBitmap(data);
	wxClientDC src;
	src.SetHDC( (HDC) data.hdc);
	_drawDC.Blit(0,0, GetClientRect().GetWidth(),GetClientRect().GetHeight(),&src,0,0);
	*/
	//通知tool
	if( getCurrentMapTool().get()){
		/*
		wxMouseEvent evt;
		evt.m_eventType = wxEVT_MOTION;
		getCurrentMapTool()->ProcessEvent(evt);
		*/
		///保留之后修改
	}
}

void	GeoMapCanvas::update(bool redrawView /* = true*/){
	if(redrawView){
		_mapview->draw();					//强制底图被重新绘制
		getLowestLayer()->draw();  //更新每一个地图层
	}
	wxDC * endDC = getTopmostLayer()->getDrawDC();
	wxClientDC dc(this);	
	dc.Blit(0,0, GetClientRect().GetWidth(),GetClientRect().GetHeight(),endDC,0,0);
	//////////////////////////////////////////////////////////////////////////
	size_t n;
	for(n=0;n<_plugins.size();n++){
		_plugins[n]->OnDraw(this,&dc);
	}
	//////////////////////////////////////////////////////////////////////////
	if( getCurrentMapTool().get()){
		getCurrentMapTool()->onMapRedraw();
	}
}

void GeoMapCanvas::OnSize(wxSizeEvent& evt){
	if(!_mapview){
		return;
	}
	if( evt.GetSize().GetX()<=0 || evt.GetSize().GetY() <= 0){
		return;
	}
	_mapview->changeSize(evt.GetSize().GetX(),evt.GetSize().GetY());
	GeoMapLayerBase *layer;
	layer = getLowestLayer().get();
	//通知每一图层当前视图区域已经改变
	while(layer){
		layer->changeSize(evt.GetSize().GetX(),evt.GetSize().GetY());
		layer = layer->getTopLayer();
	}
	//拷贝到显示设备
	update();
}

shared_ptr<wxSQLite3Database>	GeoMapCanvas::getDB(){
	return _conf.db;
}

GeoRectT	GeoMapCanvas::getGeoRect(){
	GeoRectT r;
	GeoPointT gxy1 = xy_View2Geo(ViewPointT(0,GetRect().GetHeight()));
	GeoPointT gxy2 = xy_View2Geo(ViewPointT(GetRect().GetWidth(),0));
	r.x = gxy1.x;
	r.y = gxy1.y;
	r.w = gxy2.x - r.x;
	r.h = gxy2.y - r.y;
	return r;
}

ViewRectT	GeoMapCanvas::getViewRect(){
	ViewRectT r;
	r.x = 0;
	r.y = 0;
	r.w = GetClientSize().GetWidth();
	r.h = GetClientSize().GetHeight();
	return r;
}

shared_ptr<GeoMapLayerBase> GeoMapCanvas::getLayer(GeoMapLayerType type){
	if( _layers.find( type ) != _layers.end()){
		return _layers[type];
	}
	return shared_ptr<GeoMapLayerBase>((GeoMapLayerBase*)NULL);
}

wxDC*		GeoMapCanvas::getDrawDC(){
	return &_drawDC;
}


ViewPointT	GeoMapCanvas::xy_Geo2View( GeoPointT xy){	
	ViewPointT vp;
	_mapview->xyM2V(xy.x,xy.y,vp);	
	return vp;
}

GeoPointT GeoMapCanvas::xy_View2Geo( ViewPointT vp){	
	GeoPointT gp;
	_mapview->xyV2M( (int)vp.x,(int)vp.y,gp);	
	return gp;
}


GeoRectT GeoMapCanvas::rect_View2Geo(ViewRectT & vwrc){
	GeoRectT r;
	return r;
}

ViewRectT  GeoMapCanvas::rect_Geo2View( GeoRectT& georc){
	ViewRectT r;
	return r;
}

shared_ptr<GeoMapToolBase>&		GeoMapCanvas::getMapTool(){
	return _maptool;
}

void GeoMapCanvas::OnMouseEvent(wxMouseEvent& evt){
	if( _maptool.get()){
		_maptool->onMouseEvent(evt);
	}
	evt.Skip();
	{
		wxCriticalSectionLocker l(_cs_clients);
		GeoMapCanvasClientListT::iterator itr;
		for( itr = _clientList.begin(); itr!=_clientList.end(); itr++){
			(*itr)->OnMouseEvent(this, evt );	// reflect to each client
		}		
	}
}

ViewPointT	GeoMapCanvas::getViewCenterXY(){
	return ViewPointT( getViewRect().w/2,getViewRect().h/2);
}

void  GeoMapCanvas::updateRect(const ViewRectT& rc){
	wxClientDC dc(this);	
	dc.Blit(rc.x,rc.y, rc.w,rc.h,&_drawDC,rc.x,rc.y);	
}


void  GeoMapCanvas::setMapTool(GeoMapToolTypeT tool){
	if( _maptool.get()){
		if( _maptool->getType() == tool){
			return ;
		}
		_maptool->set(NULL);
	}
	GeoMapToolBase* base;
	switch( tool ){
		case GMTT_PAN:
			_maptool = _maptool_pan;
			break;
		case GMTT_ZOOMIN:
			_maptool = _maptool_zoomin;
			break;
		case GMTT_ZOOMOUT:
			_maptool = _maptool_zoomout;
			break;
		case GMTT_MEASURE_LINE:
			_maptool = _maptool_measure;
			break;
		case GMTT_SELECT:
			_maptool = _maptool_select;
			break;
		case GMTT_EDITOR:	 //地图对象选择
			_maptool = _maptool_objeditor;
			break;
		case GMTT_NEW_POI:
			_maptool = _maptool_newPoi;
			break;
		case GMTT_NEW_BARRIER:
			_maptool = _maptool_newBarrier;
			break;
		default:
			_maptool.reset();
			return ;			
	}
	//_maptool = shared_ptr<GeoMapToolBase>( base);
	_maptool->set(this);
}

shared_ptr<GeoMapToolBase>   GeoMapCanvas::getCurrentMapTool(){
	return _maptool;
}

shared_ptr<GeoMapToolBase>	GeoMapCanvas::getMaptool(GeoMapToolTypeT type){
	shared_ptr<GeoMapToolBase> maptool;
	switch( type ){
		case GMTT_PAN:
			maptool = _maptool_pan;
			break;
		case GMTT_ZOOMIN:
			maptool = _maptool_zoomin;
			break;
		case GMTT_ZOOMOUT:
			maptool = _maptool_zoomout;
			break;
		case GMTT_MEASURE_LINE:
			maptool = _maptool_measure;
			break;
		case GMTT_SELECT:
			maptool = _maptool_select;
			break;
		case GMTT_EDITOR:	 // scott 2008.12.15 , tomorrow is birthday,yeah
			maptool = _maptool_objeditor;
			break;
		default:
			maptool.reset();
	}
	return maptool;
}


void GeoMapCanvas::addClient(GeoMapCanvasClient* c){
	wxCriticalSectionLocker l(_cs_clients);
	if( std::find(_clientList.begin(),_clientList.end(),c) == _clientList.end() ){
		_clientList.push_back(c);
	}
}

void GeoMapCanvas::delClient(GeoMapCanvasClient* c){
	wxCriticalSectionLocker l(_cs_clients);
	_clientList.remove(c);
}


double GeoMapCanvas::getGeoDistance( const ViewPointT& p1,const ViewPointT& p2){

	GeoPointT gp1,gp2;
	gp1 = xy_View2Geo(p1);
	gp2 = xy_View2Geo(p2);
	geom::GeometryFactory factory;
	shared_ptr<geom::Point> pp1( factory.createPoint( geom::Coordinate(gp1.x,gp1.y)));
	shared_ptr<geom::Point> pp2( factory.createPoint( geom::Coordinate(gp2.x,gp2.y)));
	return pp1->distance(pp2.get());
}

double GeoMapCanvas::getGeoDistance( const std::list<shared_ptr<ViewPointT> > & ptlist){
	shared_ptr<ViewPointT> pt;
	std::list<shared_ptr<ViewPointT> >::const_iterator itr;
	double dist=0;
	for(itr = ptlist.begin(); itr!=ptlist.end();itr++){
		if( pt.get()){
			dist+=getGeoDistance( *pt.get(), *(*itr).get());
		}
		pt = *itr;
	}
	return dist;
}

void	GeoMapCanvas::scaleToViewRect(const ViewRectT& rc,bool zoomin/*=true*/){
	GeoPointT gxy;
	ViewPointT vxy;
	vxy.x = rc.x+rc.w/2;
	vxy.y = rc.y+rc.h/2;
	gxy = xy_View2Geo(vxy);
	ViewRectT canvasRc = getViewRect();
	GeoPointT gp1,gp2;
	double wide; 
	//convert to km
	
//	wide *= 111.23;
	double scale ;
	int  scalelevel = -1;
	int n;
	if( zoomin){ // enlarge map
		wide= getGeoDistance( ViewPointT(rc.x,rc.y),ViewPointT(rc.x+rc.w,rc.y));
		scale = getViewRect().w /  wide ;
		scale = (1862*60)/scale; //一个像素表示多少米
		for(n=0;n< _mapview->getConfig().map_scale_list.size();n++){
			if( _mapview->getConfig().map_scale_list[n] >= scale ){
				scalelevel = n+1;
				break;
			}
		}
	}else{ ///视图缩小
		wide= getGeoDistance( ViewPointT(0,0),ViewPointT(GetSize().GetWidth()-1,GetSize().GetHeight()-1) );
		scale = rc.w /  wide ;
		scale = (1862*60)/scale;
		for(n=0;n< _mapview->getConfig().map_scale_list.size();n++){
			if( _mapview->getConfig().map_scale_list[n] >= scale ){
				scalelevel = n+1;
				break;
			}
		}
	}
	if( scalelevel != -1){
		_mapview->setScale( gxy.x,gxy.y, scalelevel);
		update(true);
	}
}

//遍历所有能出现在当前视图范围的对象，并将其转换为视图坐标，再加上以这个坐标为中心的envelope框
//最后进行相交检测
GeoMapSymbolList  
GeoMapCanvas::mapSelectViewPoint(const ViewPointT& pt,wxMouseEvent * evt,
									 GeoMapTool_Select* tool ,bool multiple/*=false是否单选还是多选*/){
	ViewRectT rc;
	rc.x = pt.x;
	rc.y = pt.y;
	rc.w = 3;
	rc.h = 3;
	GeoMapSymbolList list;
	list = mapSelectViewRect( rc,evt,tool);
	/*
	if( multiple == false){ // 单选
		if( list.size() ){
			GeoMapSymbolList rs;
			rs.push_back(*list.begin());
			list = rs;
		}		
	}*/
	return list;
}

/*
params:
rc - 选择矩形
evt - 鼠标事件,用于判别控制键的状态
tool - select 工具对象
returns:
	选择的对象列表
*/
GeoMapSymbolList GeoMapCanvas::mapSelectViewRect(const ViewRectT& rc,wxMouseEvent * evt,GeoMapTool_Select* tool){
	MapSelectedInfoT selectInfo;
	GeoMapSymbolList list;
	GeoMapSymbolList one;
	//GeoMapLayerList::iterator itlayer;
	
	if( tool->getSelectTypes() & GMTST_POI ){
		one =_layers[GEOMAP_LAYER_USER_POI]->findSymbols(rc);
		selectInfo.symbols.insert(selectInfo.symbols.end(),one.begin(),one.end());
	}
	if( tool->getSelectTypes() & GMTST_VEHICLE ){ //框中车辆
		one =_layers[GEOMAP_LAYER_VEHICLE]->findSymbols(rc);
		selectInfo.symbols.insert(selectInfo.symbols.end(),one.begin(),one.end());
	}
	if( tool->getSelectTypes() & GMTST_BARRIER ){
		one =_layers[GEOMAP_LAYER_BARRIER]->findSymbols(rc);
		selectInfo.symbols.insert(selectInfo.symbols.end(),one.begin(),one.end());
	}
	if( selectInfo.symbols.size()){
		wxCriticalSectionLocker l(_cs_clients);
		GeoMapCanvasClientListT::iterator itr;
		for(itr= _clientList.begin();itr!=_clientList.end();itr++){
			(*itr)->OnSelectObject(this,selectInfo);   //提交反馈用户
		}
	}
	list = selectInfo.symbols;
	return list;
}

void	GeoMapCanvas::updateRect(const ViewRectT& rc,GeoMapSymbolBase* ignore){
}

shared_ptr<GeoMapLayerBase> GeoMapCanvas::getTopmostLayer(){
	return getLayer(GEOMAP_LAYER_TOPMOST); 
}

shared_ptr<GeoMapLayerBase> GeoMapCanvas::getLowestLayer(){
	return getLayer(GEOMAP_LAYER_LOWEST);
}

//设置地图显示位置 
void	GeoMapCanvas::setGeoScense(const mapGeoScenseT& s){
	getView()->setScale( s.cxy.x,s.cxy.y, s.scale);
//	getView()->moveGeoCenterXY( s.cxy ); //新坐标点为中心点
//	getView()->getConfig().scale = s.scale;
	update();
}

GeoPointT		GeoMapCanvas::getGeoCenterXY(){	
	return xy_View2Geo(getViewCenterXY());
}

GeoMapLayerList&	GeoMapCanvas::getLayers(){
	return _layers;
}

shared_ptr<GeoMapSymbolBase> 
GeoMapCanvas::getLayerSymbol(const ViewPointT& pt){
	shared_ptr<GeoMapSymbolBase>  symbol = getLayer(GEOMAP_LAYER_USER_POI)->getSymbol(pt);
	if( symbol.get()){
		return symbol;
	}
	symbol = getLayer(GEOMAP_LAYER_BARRIER)->getSymbol(pt);
	if( symbol.get()){
		return symbol;
	}
	return shared_ptr<GeoMapSymbolBase> ();
}

//////////////////////////////////////////////////////////////////////////
//地图缩放级别
int		GeoMapCanvas::getScaleLevel(){
	return _mapview->getConfig().scale;
}

double GeoMapCanvas::getScale(){
	return _mapview->getConfig().map_scale_list[getScaleLevel()-1];
}

///地图缩放
void GeoMapCanvas::OnMouseWheel(wxMouseEvent& evt){
	//wxMessageBox(wxString::Format(wxT("%d,%d"),evt.GetWheelRotation(),evt.GetWheelDelta()));
	if(evt.GetWheelRotation()>0){
		getView()->zoomIn( evt.GetX(),evt.GetY());
	}else{
		getView()->zoomOut( evt.GetX(),evt.GetY());
	}
	update();
	Update();
}


void GeoMapCanvas::addPlugin(GeoMapCanvasPlugin* p){
	_plugins.push_back(p);
}

void GeoMapCanvas::OnEraseBG(wxEraseEvent& evt){
	evt.Skip(false);
}
