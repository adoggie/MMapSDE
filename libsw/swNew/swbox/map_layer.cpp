#include "main_window.h"
#include "map_canvas.h"
#include "map_layer.h"
#include "sysconfig.h"
#include "map_resource.h"
#include "map_help.h"
#include "repository.h"
#include "mainapp.h"

GeoMapLayerBase::GeoMapLayerBase(){
	 
	_visible = true;
	_textAlign	= GEOMAP_TEXTALIGN_BOTTOM;
}

/*
void   GeoMapLayerBase::chainLayer(GeoMapLayerType top ,GeoMapLayerType bottom ){
	_topLayer = top;
	_bottomLayer = bottom;
}
*/
GeoMapLayerBase* GeoMapLayerBase::getTopLayer(){
	return _canvas->getLayer( (GeoMapLayerType)(_type<<1)).get();
}

GeoMapLayerBase* GeoMapLayerBase::getBottomLayer(){
	return _canvas->getLayer( GeoMapLayerType(_type>>1) ).get();
}

void		GeoMapLayerBase::setCanvas( GeoMapCanvas*  canvas){
	_canvas = canvas;
}

GeoMapCanvas*		GeoMapLayerBase::getCanvas(){
	return _canvas;
}

inline 
GeoMapLayerType		GeoMapLayerBase::getType(){
	return _type;
}

void GeoMapLayerBase::draw(){
	if( !getBottomLayer()  ){
		GeoMapBitmapT data;
		_canvas->getView()->getBitmap(data);
		wxClientDC src;
		src.SetHDC( (HDC) data.hdc);
		_drawDC.Blit(0,0, _canvas->GetClientRect().GetWidth(),_canvas->GetClientRect().GetHeight(),&src,0,0);
	}else{
		//下一层的图像拷贝到当前图层为底图
		_drawDC.Blit(0,0, _canvas->GetClientRect().GetWidth(),_canvas->GetClientRect().GetHeight(),getBottomLayer()->getDrawDC(),0,0);
	}
	before_drawSelf();

	GeoMapLayerType type = _type;
	//绘制图标 
	//wxCriticalSectionLocker l(_cs_symbols);
	GeoRectT grc = _canvas->getGeoRect();
	GeoMapSymbolList::iterator itr;
	for( itr = _symbols.begin(); itr!=_symbols.end();itr++){
		if( (*itr)->isGeoIntersected( grc) ){
			(*itr)->draw();
		}
	}
	after_drawSelf();
	if( getTopLayer() ){
		getTopLayer()->draw();
	}
}

void	GeoMapLayerBase::before_drawSelf(){}
void  GeoMapLayerBase::after_drawSelf(){}

void GeoMapLayerBase::loadSymbols(GeoRectT& rect){
}

void GeoMapLayerBase::setVisible(bool visible /*= true*/){
	_visible = visible;
}


void GeoMapLayerBase::drawRect(ViewRectT& rect){
	GeoMapLayerBase * layer;
	layer = getBottomLayer();
	while( layer ){
		layer = layer->getBottomLayer();
	}
	while( layer ){
		layer->loadSymbols( _canvas->rect_View2Geo(rect) );
		layer->draw();
		layer = layer->getTopLayer();
	}
}

void GeoMapLayerBase::setTextVisible(bool visible /*= true*/){
	_textVisible = visible;
}

bool	GeoMapLayerBase::textVisible(){
	return _textVisible;
}

void	GeoMapLayerBase::setFont(GeoMapLayerFontT& font){
	_font = font;
}

GeoMapLayerFontT& GeoMapLayerBase::getFont(){
	return _font;
}

GeoMapSymbolList	GeoMapLayerBase::getSymbols(){
	return _symbols;
}

GeoMapLayerTextAlignT	GeoMapLayerBase::getTextAlign(){
	return _textAlign;
}

void GeoMapLayerBase::setTextAlign(GeoMapLayerTextAlignT align){
	_textAlign = align;
}

GeoMapSymbolList GeoMapLayerBase::findSymbols(const ViewRectT& rc,GeoMapSymbolBase* ignore /*=NULL*/){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList list;
	GeoMapSymbolList::iterator itr;
	for(itr=_symbols.begin();itr!=_symbols.end();itr++){
		if( (*itr)->isIntersected(rc) ){  //视图区域相交
			list.push_back(*itr);
		}
	}
	return list;
}

void GeoMapLayerBase::lockSymbols(bool lock /*=true*/){
	if( lock ){
		_cs_symbols.Enter();
	}else{
		_cs_symbols.Leave();
	}
}

void	GeoMapLayerBase::changeSize(int cx,int cy){
	_drawBitmap = shared_ptr<wxBitmap>(new wxBitmap);
	_drawBitmap->Create( cx,cy);
	_drawDC.SelectObject( *_drawBitmap.get() );
}

wxDC*		GeoMapLayerBase::getDrawDC(){
	return &_drawDC;
}	

wxBitmap*	GeoMapLayerBase::getDrawBitmap(){
	return _drawBitmap.get();
}
/*
无效图层区域
*/
void  GeoMapLayerBase::invalidViewRect(const ViewRectT& rect,GeoMapSymbolBase* ignoreDraw/*=NULL忽略绘制*/){
	/*
	//检索选择区域内的所有symbol，计算其view的最大envelope
	{
		wxCriticalSectionLocker l(_cs_symbols);
		GeoMapSymbolList list;
		GeoMapSymbolList::iterator itr;
		shared_ptr<GeoMapSymbolBase> symbol;
		GeoRectT geoRc = _canvas->getGeoRect();
		for(itr = _symbols.begin();itr!=_symbols.end();itr++){
			symbol = (*itr);
			if( symbol.get() != ignoreDraw){
				if( symbol->isGeoIntersected( geoRc ) ){ //在当前地图视野
					symbol->isIntersected(rect);					
				}
			}
		}
	}
	//请求上级layer也要绘制次区域
	//拷贝下级layer的区域位图,此位图区域为当前受影响的所有symbol的最大envelope
	_drawDC.Blit(rect.x,rect.y, rect.w,rect.h,getBottomLayer()->getDrawDC(),rect.x,rect.y);
	//上级图层强制重绘
	if( getTopLayer() ){ 
		getTopLayer()->invalidViewRect(rect,NULL);
	}else{//顶层了就直接绘制到显示设备
		wxClientDC dc(_canvas);
		dc.Blit( rect.x,rect.y,rect.w,rect.h,
	}*/
}

void GeoMapLayerBase::setSymbolGeoXY(const GeoPointT& xy,int sid, bool& viewXyChanged,int time /*gps数据采集时间*/){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		shared_ptr<GeoMapSymbolBase> & s = *itr;
		if( s->getObjectId() == sid){
			s->setGeoXY(xy.x,xy.y,viewXyChanged);
			s->setTimestamp(time);
			break;
		}
	}
}

shared_ptr<GeoMapSymbolBase>	GeoMapLayerBase::getSymbol(int sid){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		shared_ptr<GeoMapSymbolBase> & s = *itr;
		if( s->getObjectId() == sid){
			return s;
		}
	}
	return shared_ptr<GeoMapSymbolBase>();
}

void GeoMapLayerBase::redraw(){
	draw();
}

shared_ptr<GeoMapSymbolBase>	GeoMapLayerBase::getSymbol(const ViewPointT& pt){
	wxCriticalSectionLocker l(_cs_symbols); // haha ,should be protected 
	GeoMapSymbolList::iterator itr;
		for( itr=_symbols.begin();itr!=_symbols.end();itr++){
			int type = (*itr)->getType();
		if( (*itr)->isIntersected(pt)){
			return *itr;
		}
	}
	return shared_ptr<GeoMapSymbolBase>();
}

void		GeoMapLayerBase::setSymbolVisible(int sid,bool visible ){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr=_symbols.begin();itr!=_symbols.end();itr++){
		if( (*itr)->getObjectId() == sid ){
			(*itr)->setVisible(visible);
		}
	}
}

void GeoMapLayerBase::append(shared_ptr<GeoMapSymbolBase>& symbol){
	wxCriticalSectionLocker l(_cs_symbols);
	_symbols.push_back(symbol);
}


shared_ptr<GeoMapSymbolBase> GeoMapLayerBase::detachSymbol(int sid){
	shared_ptr<GeoMapSymbolBase> s;
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr=_symbols.begin();itr!=_symbols.end();itr++){
		if( (*itr)->getObjectId() == sid ){
			s = (*itr);
			s->setLayer(NULL);
			_symbols.erase(itr);
			break;
		}
	}
	return s;
}

void GeoMapLayerBase::attachSymbol(const shared_ptr<GeoMapSymbolBase> symbol){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr=_symbols.begin();itr!=_symbols.end();itr++){
		if( (*itr) == symbol){
			return;
		}
	}
	symbol->setLayer(this);
	_symbols.push_back(symbol);
}

void GeoMapLayerBase::clearSymbols(){
	wxCriticalSectionLocker l(_cs_symbols);
	_symbols.clear();
}

//////////////////////////////////////////////////////////////////////////
//--GeoMapLayer_Vehicle::----------------------------------//
GeoMapLayer_Vehicle::GeoMapLayer_Vehicle(){
	_type = GEOMAP_LAYER_VEHICLE;
	_replaying = false;
	_focuseVehicle = INVALID_VEHICLE_ID;
}

void GeoMapLayer_Vehicle::draw(){	 
	//1.先绘制轨迹线
	
	//1.绘制车辆图标
	GeoMapLayerBase::draw();	
}

void	GeoMapLayer_Vehicle::before_drawSelf(){
	//return;
	
	//wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		GeoMapSymbol_Vehicle*  s =(GeoMapSymbol_Vehicle*) (*itr).get();
		s->drawTrackPath();
	}
}

bool	GeoMapLayer_Vehicle::textVisible(){
	//return gvmsSysConfig::instance().getUserSettingValue(wxT("map.vehicle.text.visible"))==wxT("1");
	return true;
}
/*
读取车辆并显示到地图,全部加载数据
*/
void GeoMapLayer_Vehicle::loadSymbols(GeoRectT& rect){
/*
	wxCriticalSectionLocker l(_cs_symbols);
	_symbols.clear();	
	wxCriticalSectionLocker ll(DataRepository::instance().vechiles.mutex);
	
	std::list< shared_ptr<gvms::clientVehicleT> >::iterator itr;
	for(itr = DataRepository::instance().vechiles.list.begin(); itr!=DataRepository::instance().vechiles.list.end();itr++){
		shared_ptr<GeoMapSymbol_Vehicle> symbol ( new GeoMapSymbol_Vehicle);
		symbol->_vechile = *itr;
		_symbols.push_back( symbol );
	}
*/
}

void GeoMapLayer_Vehicle::setVisible(bool visible /*= true*/){
}
void GeoMapLayer_Vehicle::drawRect(ViewRectT& rect){
}
void GeoMapLayer_Vehicle::setTextVisible(bool visible /*= true*/){
}

GeoMapSymbolList GeoMapLayer_Vehicle::findSymbols(const ViewRectT& rc){
	//std::vector<shared_ptr<GeoMapSymbolBase> > list;
	//return list;
	return GeoMapLayerBase::findSymbols(rc);
}

void GeoMapLayer_Vehicle::appendSymbols( std::list< shared_ptr<gvms::clientVehicleT> > & vehicles){
	wxCriticalSectionLocker l(_cs_symbols);
	std::list< shared_ptr<gvms::clientVehicleT> >::iterator itr;
	for(itr = vehicles.begin(); itr!=vehicles.end();itr++){
		shared_ptr<GeoMapSymbol_Vehicle> symbol ( new GeoMapSymbol_Vehicle);
		symbol->_vechile = *(*itr).get();
		symbol->setLayer(this);
		symbol->init();
		_symbols.push_back( symbol );
	}
}

void	GeoMapLayer_Vehicle::setDirection(float angle,int vid){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		GeoMapSymbol_Vehicle*  s =(GeoMapSymbol_Vehicle*) (*itr).get();
		if( s->getObjectId() == vid){
			s->setDirection(angle);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void	GeoMapLayer_Vehicle::showTrackPath(int vid,bool show){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		GeoMapSymbol_Vehicle*  s =(GeoMapSymbol_Vehicle*) (*itr).get();
		if( s->getObjectId() == vid){
			s->showTrackPath(show);
			break;
		}
	}
}

void	GeoMapLayer_Vehicle::hideTrackPath(){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		GeoMapSymbol_Vehicle*  s =(GeoMapSymbol_Vehicle*) (*itr).get();
		s->showTrackPath(false);
	}
}

void	GeoMapLayer_Vehicle::setTrackPathColor(int vid,wxColour& color){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		GeoMapSymbol_Vehicle*  s =(GeoMapSymbol_Vehicle*) (*itr).get();
		if( s->getObjectId() == vid){
			s->setTrackPathColor(color);
			break;
		}
	}
}

void	GeoMapLayer_Vehicle::setTrackPathWidth(int vid,int width){
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		GeoMapSymbol_Vehicle*  s =(GeoMapSymbol_Vehicle*) (*itr).get();
		if( s->getObjectId() == vid){
			s->setTrackPathWidth(width);
			break;
		}
	}
}

void	GeoMapLayer_Vehicle::setReplayMode(bool mode){
	_replaying  = mode;
}

bool  GeoMapLayer_Vehicle::isReplayMode(){
	return _replaying ;
}

/*
1.聚焦车辆，将图标置于队列末端，然后强制地图重新绘制
2.改变车辆文本颜色
调用者必须强制刷行地图画板
*/
void  GeoMapLayer_Vehicle::setFocus(int vid){
	_focuseVehicle = vid;
	
	wxCriticalSectionLocker l(_cs_symbols);
	GeoMapSymbolList::iterator itr;
	//wxFont font;
	//font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		GeoMapSymbol_Vehicle*  s =(GeoMapSymbol_Vehicle*) (*itr).get();
		s->setTextColor(*wxBLACK);
		if( s->getObjectId() == vid){ //找到车辆，将其删除，然后置入尾部
			s->setTextColor(*wxRED);  //焦点车辆颜色	
		}
	}

	for(itr= _symbols.begin();itr!=_symbols.end();itr++){
		GeoMapSymbol_Vehicle*  s =(GeoMapSymbol_Vehicle*) (*itr).get();
		if( s->getObjectId() == vid){ //找到车辆，将其删除，然后置入尾部
			shared_ptr<GeoMapSymbolBase> symbol;
			symbol = *itr;
			_symbols.erase(itr);
			_symbols.push_back(symbol); // append backend
			//this->draw();  //递归绘制数据，
			break;
		}
	}


}

//--GeoMapLayer_UserPoi::----------------------------------//
GeoMapLayer_UserPoi::GeoMapLayer_UserPoi(){
	_type = GEOMAP_LAYER_USER_POI;
}

bool	GeoMapLayer_UserPoi::textVisible(){
	//return gvmsSysConfig::instance().getUserSettingValue(wxT("map.poi.text.visible"))==wxT("1");
	return true;
}
void GeoMapLayer_UserPoi::draw(){
	GeoMapLayerBase::draw();	
}

void GeoMapLayer_UserPoi::loadSymbols(GeoRectT& rect){	
	wxCriticalSectionLocker l(_cs_symbols);

	unsigned int mask = 0;
	int scale ;
	scale = _canvas->getView()->getConfig().scale;
	/*
	//判别用户地图显示级别
	gvmsMapUserLevelList ulist = gvmsSysConfig::instance().getMapUserLevels();
	for( gvmsMapUserLevelList::iterator itr = ulist.begin() ;itr!=ulist.end() ; itr++){
		if( (*itr).scale_start <= scale && (*itr).scale_end >= scale ){
			mask = (*itr).mask_bit;
			break;
		}
	}*/
	_symbols.clear();
	//-- to select poi
	wxCriticalSectionLocker ll(DataRepository::instance().pois.mutex);
	std::list< shared_ptr<gvms::clientPoiDataT> >::iterator itr;
	for(itr = DataRepository::instance().pois.list.begin(); itr!=DataRepository::instance().pois.list.end();itr++){
		shared_ptr<GeoMapSymbol_UserPoi> symbol ( new GeoMapSymbol_UserPoi);
		symbol->_userpoi = *itr;
		symbol->setLayer(this);
		symbol->init();
		_symbols.push_back( symbol );
	}
}
void GeoMapLayer_UserPoi::setVisible(bool visible /*= true*/){
	GeoMapLayerBase::setVisible(visible);
}
void GeoMapLayer_UserPoi::drawRect(ViewRectT& rect){
}
void GeoMapLayer_UserPoi::setTextVisible(bool visible /*= true*/){
	GeoMapLayerBase::setTextVisible(visible);
}

GeoMapSymbolList GeoMapLayer_UserPoi::findSymbols(const ViewRectT& rc){
	return GeoMapLayerBase::findSymbols(rc);
}
//--GeoMapLayer_Barrier::----------------------------------//
GeoMapLayer_Barrier::GeoMapLayer_Barrier(){
	_type = GEOMAP_LAYER_BARRIER;
}
void GeoMapLayer_Barrier::draw(){
	
	GeoMapLayerBase::draw();
	
}
//围栏加载 
void GeoMapLayer_Barrier::loadSymbols(GeoRectT& rect){
	wxCriticalSectionLocker l(_cs_symbols);
	_symbols.clear();	
	//-- to select poi
	wxCriticalSectionLocker ll(DataRepository::instance().barriers.mutex);
	std::list< shared_ptr<gvms::clientBarrierT> >::iterator itr;
	for(itr = DataRepository::instance().barriers.list.begin(); itr!=DataRepository::instance().barriers.list.end();itr++){
		shared_ptr<GeoMapSymbol_Barrier> symbol ( new GeoMapSymbol_Barrier);
		symbol->_barrier = *itr;
		symbol->setLayer(this);
		_symbols.push_back( symbol );
	}
}

void GeoMapLayer_Barrier::setVisible(bool visible /*= true*/){
}
void GeoMapLayer_Barrier::drawRect(ViewRectT& rect){
}
void GeoMapLayer_Barrier::setTextVisible(bool visible /*= true*/){
}

GeoMapSymbolList GeoMapLayer_Barrier::findSymbols(const ViewRectT& rc){
	return GeoMapLayerBase::findSymbols(rc);
}

//删除围栏
void	GeoMapLayer_Barrier::deleteBarriersByGroup(int bgId){
	
}

void	GeoMapLayer_Barrier::before_drawSelf(){
	ViewRectT rc; 
	rc.x = rc.y = 0;
	rc.w = _canvas->GetSize().GetWidth();
	rc.h = _canvas->GetSize().GetHeight();
	_tempBitmap.Create(rc.w,rc.h);
	_tempDC.SelectObject( _tempBitmap);
	_tempDC.SetPen(*wxRED_PEN);
	_tempDC.SetBrush(*wxRED_BRUSH);
	_tempDC.Blit(0,0,rc.w,rc.h,&_drawDC,0,0);
}

void  GeoMapLayer_Barrier::after_drawSelf(){
	ViewRectT rc; 
	rc.x = rc.y = 0;
	rc.w = _canvas->GetSize().GetWidth();
	rc.h = _canvas->GetSize().GetHeight();
	_drawDC.Blit(0,0,rc.w,rc.h,&_tempDC,0,0,wxAND);
}

