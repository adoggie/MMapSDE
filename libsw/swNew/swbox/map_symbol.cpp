#include "map_canvas.h"
#include "map_symbol.h"
#include "map_help.h"
#include "map_layer.h"
#include "map_resource.h"
#include "repository.h"
#include "sysconfig.h"
#include "util.h"


GeoMapSymbolBase::GeoMapSymbolBase(){
	_type = GEOMAP_SYMBOL_NULL;
//	_visible = false;  //
	//_focused = false;
	_font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
	_textColor = *wxBLACK;
}

GeoMapSymbolType	GeoMapSymbolBase::getType(){
	return _type;
}

void	GeoMapSymbolBase::setIcon(shared_ptr<SysResIcon>& icon){
	_icon = icon;
}

/*
void	GeoMapSymbolBase::setText(wxString & text){
	_text = text;
}*/

void	GeoMapSymbolBase::setLayer(GeoMapLayerBase* layer){
	_layer = layer;
}

void	GeoMapSymbolBase::setGeoXY(double x,double y,bool & viewPtChanged/*显示坐标是否修改*/){
	geom::Coordinate viewxy = getViewXY();
	_geo_xy.x = x;
	_geo_xy.y = y;
	geom::Coordinate viewxy2 = getViewXY();
	if( viewxy !=viewxy2){
		ViewRectT rc = _layer->getCanvas()->getViewRect();
		if( viewxy.x >=rc.x && viewxy.x < rc.x+rc.w && viewxy.y>=rc.y && viewxy.y< rc.y+rc.h){
			viewPtChanged = true;   //设置前后视图点已经改变且坐标在当前视图内
		}
	}
	 
	/*
	GeoRectT curGeoRect = _layer->getCanvas()->getGeoRect();
	if( x >=curGeoRect.x && x < curGeoRect.x+curGeoRect.w && y>=curGeoRect.y && y< curGeoRect.y+curGeoRect.h){
	}else{
		return;	//out of sight
	}
	ViewPointT pt = GeoMap_Coordinate2Point<int>(_geo_xy);
	if( pt == _viewxy ){
		return;
	}

	hide();
	_viewxy = pt;
	draw();*/
}

void  GeoMapSymbolBase::setViewXY(int x, int y){
	/*
	if( _viewxy.x == x && _viewxy.y == y){
		return ;
	}
	//hide();
	_viewxy.x = x;
	_viewxy.y = y;
	GeoPointT gpt = _layer->getCanvas()->xy_View2Geo( ViewPointT(x,y));
	_geo_xy = GeoMap_Point2Coordinate<double>( gpt);
	*/
	//draw();
	GeoPointT gpt = _layer->getCanvas()->xy_View2Geo( ViewPointT(x,y));
	_geo_xy = GeoMap_Point2Coordinate<double>( gpt);
}

void	GeoMapSymbolBase::draw(){
	if( !getVisible()){
		return;
	}
	if( getGeoXY().x == 0 || getGeoXY().y == 0){
		return;
	}
	drawIcon();
	if( _layer->textVisible()){
		drawText();
	}
	//drawFocus();
}
//清除当前图标占据的envelope区域,强制其他图层和其它相交图标的重绘
void	GeoMapSymbolBase::drawCompound(){
}

geom::Envelope GeoMapSymbolBase::getEnvelope(){	
	geom::Envelope e;
	ViewRectT rc =  getIconRect();
	e.init(rc.x,rc.x+rc.w,rc.y,rc.y+rc.h);
	geom::Envelope t;
	if( !_layer->textVisible() ){
		return e;
	}
	//计算文本宽度 
	rc = getTextRect();
	t.init( rc.x,rc.x+rc.w,rc.y,rc.y+rc.h);	
	//合并icon和text区域 
	e.expandToInclude( &t);
	return e;
}

geom::Coordinate	GeoMapSymbolBase::getViewXY(){
	return	GeoMap_Point2Coordinate<int>(
						_layer->getCanvas()->xy_Geo2View( GeoMap_Coordinate2Point<double>(getGeoXY()) )
					);	
}

geom::Coordinate	GeoMapSymbolBase::getGeoXY(){
	return _geo_xy;
}

void	GeoMapSymbolBase::setFont(const wxFont& font){
	_font = font;
}

wxFont& GeoMapSymbolBase::getFont(){
	return _font;
}

void GeoMapSymbolBase::setTextColor(const wxColour& color){
	_textColor = color;
}

wxColour GeoMapSymbolBase::getTextColor(){
	return _textColor;
}

void	GeoMapSymbolBase::drawText(){
	ViewRectT rc;
	if( getText() == wxT("")){
		return;
	}
	rc = getTextRect();
	wxDC * dc = _layer->getDrawDC();
	dc->SetFont(_font);
	dc->SetBackgroundMode(wxSOLID );
	dc->SetTextBackground(*wxWHITE);
	dc->SetTextForeground(_textColor);
	dc->DrawText( getText(), rc.x,rc.y);
}

void	GeoMapSymbolBase::drawIcon(){
	if( !_icon.get()  ){
		return;
	}
	wxDC * dc = _layer->getDrawDC();
	ViewRectT rc;
	rc = getIconRect();
	dc->DrawBitmap( _icon->bitmap, rc.x,rc.y,true);
	//dc->DrawRectangle(0,0,rc.w,rc.h);
	//wxLogMessage( wxString::Format(wxT("draw icon : %d,%d,%d,%d"),rc.x,rc.y,rc.w,rc.h));
}

 
ViewRectT GeoMapSymbolBase::getIconRect(){
	ViewPointT xy ;
	geom::Coordinate c = getViewXY();
	xy.x = (int)c.x;
	xy.y = (int)c.y;
	ViewRectT rc;
	rc.x = xy.x - _icon->bitmap.GetWidth()/2;
	rc.y = xy.y - _icon->bitmap.GetHeight()/2;
	rc.w = _icon->bitmap.GetWidth();
	rc.h = _icon->bitmap.GetHeight();
	return rc;
}

ViewRectT	GeoMapSymbolBase::getTextRect(){
	ViewRectT rc;
	geom::Coordinate c = getViewXY();
	wxDC * dc = _layer->getDrawDC();
	wxSize ext = dc->GetTextExtent( getText() );
	if( _layer->getTextAlign() == GEOMAP_TEXTALIGN_BOTTOM ){
		rc.x = (int)c.x - ext.GetWidth()/2;
		//rc.y = (int)c.y - ext.GetHeight()/2;
		rc.y = (int)c.y + getIconRect().h/2 + 10;
		rc.w = ext.GetWidth();
		rc.h = ext.GetHeight();
	}
	return rc;
}

//符号绘制对象区域是否与指定的视图区域相交
//检测图标矩形与判别矩形是否有交集
bool	GeoMapSymbolBase::isIntersected(const ViewRectT & rc){
	shared_ptr<geom::Polygon> ring1 = GeoMap_CreatePolygon<>(rc);
	shared_ptr<geom::Polygon> ring2 = GeoMap_CreatePolygon<>(getIconRect());
	//std::string s1 =ring1->toString();
	//std::string s2 =ring2->toString();

	return ring1->intersects(ring2.get());//||ring1->within(ring2.get());
}
// 对象地理坐标是否存在此区域内
bool	GeoMapSymbolBase::isGeoIntersected(const GeoRectT & rect){
	geom::Coordinate c = getGeoXY();
	if( c.x > rect.x && c.x < rect.x + rect.w && c.y > rect.y && c.y < rect.y + rect.h ){ 
		// point land in rectangle
		return true;
	}
	return false;
}

GeoPointT		GeoMapSymbolBase::getGeoPoint(){
	return GeoMap_Coordinate2Point<double>( getGeoXY() );
}


void	GeoMapSymbolBase::setFocus(bool focus /*= true*/){
	//_focused = focus;
	//draw();
}

bool GeoMapSymbolBase::focused(){
	return false;//_focused;
}


void	GeoMapSymbolBase::drawFocus(){
	/*
	if( _focused){ //在icon内绘制线框
			wxPen   pen(*wxRED,1);
			wxClientDC dc(_layer->getCanvas());
			dc.SetPen(pen);
			ViewRectT irc = getIconRect();
			dc.DrawLine(irc.x,irc.y,irc.x+irc.w-1,irc.y);
			dc.DrawLine(irc.x+irc.w-1,irc.y,irc.x+irc.w-1,irc.y+irc.h-1);
			dc.DrawLine(irc.x+irc.w-1,irc.y+irc.h-1,irc.x,irc.y+irc.h-1);
			dc.DrawLine(irc.x,irc.y+irc.h-1,irc.x,irc.y);
	}
	*/
}

wxString GeoMapSymbolBase::getText(){
	return wxT("");
}

void GeoMapSymbolBase::init(){

}

GeoMapLayerBase* GeoMapSymbolBase::getLayer(){
	return _layer;
}

bool	GeoMapSymbolBase::isIntersected(const ViewPointT& pt){
	ViewRectT rc = getIconRect();
	if( pt.x >= rc.x && pt.x <= rc.x+rc.w-1 && pt.y>=rc.y && pt.y<=rc.y+rc.h-1){
		return true;
	}
	return false;
}

void	GeoMapSymbolBase::setVisible(bool visible){
	_visible = visible;
}

bool	GeoMapSymbolBase::getVisible(){
	return _visible;
}


//////////////////////////////////////////////////////////////////////////
//-- GeoMapSymbol_Vehicle --------------------------------------------//
GeoMapSymbol_Vehicle::GeoMapSymbol_Vehicle(){
	_type = GEOMAP_SYMBOL_VEHICLE;
	_showTrackPath = false;
	_trackPathWidth = 3;
	_trackPathColor = *wxBLUE;
	_direction = 0;
	_status = VS_MOVING;
}

void	GeoMapSymbol_Vehicle::draw(){
	GeoMapSymbolBase::draw();
}

void	GeoMapSymbol_Vehicle::setCenter(){
	GeoPointT gpt = GeoMap_Coordinate2Point<double>(_geo_xy);
	if( !util_GeoXyValid(gpt.x,gpt.y)){
		if( _geoPoints.size()){
			gpt = (*_geoPoints.begin()).point;
		}		
	}
	_layer->getCanvas()->getView()->moveGeoCenterXY(gpt);
	_layer->getCanvas()->update();	
	_layer->getCanvas()->Update();
}

void	GeoMapSymbol_Vehicle::drawCompound(){
}

geom::Envelope GeoMapSymbol_Vehicle::getEnvelope(){
	return GeoMapSymbolBase::getEnvelope();
}

void	GeoMapSymbol_Vehicle::drawText(){
	GeoMapSymbolBase::drawText();
}

wxString GeoMapSymbol_Vehicle::getText(){
	return wxString::FromUTF8(_vechile.brand.c_str());
}

int		GeoMapSymbol_Vehicle::getObjectId(){
	return _vechile.sid;
}

void GeoMapSymbol_Vehicle::init(){
	//采用默认图标 
	setIcon();
	setDirection(0);
}


bool GeoMapSymbol_Vehicle::setIcon(){
	wxString s;
	if( _status == VS_STOP){
		s = wxT("blue"); //蓝色表示停止
	}else if( _status ==  VS_ALARM){
		s = wxT("red");		//红色报警
	}else{
		s = wxT("normal");// wxString::Format(wxT("vehicleType.%d.icon.%d")
	}
	GeoMapSymbolBase::setIcon(SysResMgr::instance().getVehicleArrowIcon(GVMS_ANGLE(_direction),s));
	/*
	s = wxString::Format(wxT("vehicleType.%d.icon.%d"),_vechile.type,(int)_status);
	shared_ptr< SysResIcon> resicon = SysResMgr::instance().getIcon(s);
	if( !resicon.get()){
		return false;
	}
	///根据角度进行旋转
	wxImage image = resicon->bitmap.ConvertToImage();
	wxPoint cpt ;
	cpt.x = resicon->bitmap.GetWidth()/2;
	cpt.y = resicon->bitmap.GetHeight()/2;
	image.Rotate(_direction,cpt);
	shared_ptr<SysResIcon> icon(new SysResIcon);
	icon->bitmap = wxBitmap(image);
	GeoMapSymbolBase::setIcon(icon);*/

	return true;
}

//设置行车方向
void	GeoMapSymbol_Vehicle::setDirection(float angle){
	if( _direction ==  angle){
		return;
	}
	_direction = angle;
	//if( gvmsSysConfig::instance().getLocalGlobalSettingValue(wxT("vehicle.icon.default")) ==  wxT("1")){
	//	GeoMapSymbolBase::setIcon(SysResMgr::instance().getVehicleArrowIcon(GVMS_ANGLE(_direction)));
	//}else{ //非默认的行车图标
	setIcon();
	//}
}

///设置行车状态 
void	GeoMapSymbol_Vehicle::setStatus(eVehicleStatusT vs){
	_status = vs;
	//if( gvmsSysConfig::instance().getLocalGlobalSettingValue(wxT("vehicle.icon.default")) ==  wxT("1")){
	//	return; //默认图标不暂时不区分图标状态颜色
	//}
	setIcon();
}

void	GeoMapSymbol_Vehicle::showTrackPath(bool show){
	_showTrackPath = show;
	if(!show){
		resetTrackPoints();
	}
}

void  GeoMapSymbol_Vehicle::setTrackPathColor(wxColour& color){
	_trackPathColor = color;
}

void	GeoMapSymbol_Vehicle::setTrackPathWidth(int width){
	_trackPathWidth = width;
}

/*
1.绘制过度轨迹线
2.大间距绘制离线轨迹
*/
void	GeoMapSymbol_Vehicle::drawTrackPath(){
	
// 	if( !_showTrackPath){
// 		return;
// 	}
	//09.04.10 scott 每次都进行绘制轨迹点 
	wxCriticalSectionLocker l(_points_mutex);
	if( _geoPoints.size() == 0){
		return;
	}
	wxDC * dc = getLayer()->getDrawDC();
	wxPen pen;
	pen.SetColour(_trackPathColor);
	pen.SetWidth(_trackPathWidth);
	dc->SetPen(pen);
	
	//std::vector<wxPoint> ptlist(_viewPath.points.size());
	shared_ptr<wxPoint> start;
	//--从服务器读取视图路径点集合
	std::list<TrackGeoPointT>::iterator itr;
	TrackGeoPointT lastPt;
	for(itr=_geoPoints.begin();itr!=_geoPoints.end();itr++){
		if(itr->point.x==0 || itr->point.y ==0){
			continue;
		}
		if( !start.get()){
			start = shared_ptr<wxPoint>(new wxPoint);
			ViewPointT pt = getLayer()->getCanvas()->xy_Geo2View(GeoPointT( itr->point.x,itr->point.y));
			start->x = pt.x;
			start->y = pt.y;
			
		}else{
			//////////////////////////////////////////////////////////////////////////
			//判别与上点的时间间隔，过长则视为离线轨迹线
			if(itr->timestamp - lastPt.timestamp > 60*5){ // 5分钟间隔跳跃
				pen.SetStyle(wxDOT_DASH);
			}else{
				pen.SetStyle(wxSOLID);								
			}
			dc->SetPen(pen);
			//////////////////////////////////////////////////////////////////////////
			ViewPointT pt = getLayer()->getCanvas()->xy_Geo2View(GeoPointT( itr->point.x,itr->point.y));
			dc->DrawLine( start->x,start->y,pt.x,pt.y);
			start->x = pt.x;
			start->y = pt.y;			
		}
		lastPt = (*itr);
	}
	//////////////////////////////////////////////////////////////////////////
	//如果是回放状态便绘制过渡轨迹
	GeoMapLayer_Vehicle* layer = (GeoMapLayer_Vehicle*)_layer;
	if(layer->isReplayMode()){
		start.reset();
		//pen.SetWidth(1);
		pen.SetColour(*wxBLACK);
		dc->SetPen(pen);
		for(itr=_geoPoints.begin();itr!=_geoPoints.end();itr++){
			if(itr->point.x == 0 || itr->point.y == 0){
				continue;
			}
			if( !start.get()){
				start = shared_ptr<wxPoint>(new wxPoint);
				ViewPointT pt = getLayer()->getCanvas()->xy_Geo2View(GeoPointT( itr->point.x,itr->point.y));
				start->x = pt.x;
				start->y = pt.y;
			}else{
				pen.SetStyle(wxSOLID);								
				dc->SetPen(pen);
				ViewPointT pt = getLayer()->getCanvas()->xy_Geo2View(GeoPointT( itr->point.x,itr->point.y));
				dc->DrawLine( start->x,start->y,pt.x,pt.y);
				start->x = pt.x;
				start->y = pt.y;
				if(itr->timestamp >= getTimestamp()){ // reached current geoxy point
					break;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

//
void	GeoMapSymbol_Vehicle::resetTrackPoints(){
	wxCriticalSectionLocker l(_points_mutex);
	_geoPoints.clear();
}


void  GeoMapSymbol_Vehicle::addTrackPoint(const TrackGeoPointT& gpt){
	wxCriticalSectionLocker l(_points_mutex);
	_geoPoints.push_back(gpt);
}

void	GeoMapSymbol_Vehicle::addTrackPoints(const std::list<TrackGeoPointT>& gpts){
	wxCriticalSectionLocker l(_points_mutex);
	_geoPoints = gpts;
}
//////////////////////////////////////////////////////////////////////////
//-- GeoMapSymbol_UserPoi --------------------------------------------//
GeoMapSymbol_UserPoi::GeoMapSymbol_UserPoi(){
	_type = GEOMAP_SYMBOL_USER_POI;
}



int getScaleMask(){
		//_tagScaleRange  range;
		//int scale = _layer->getCanvas()->getScaleLevel();

		//return range;
		return 0;
}

void	GeoMapSymbol_UserPoi::draw(){
	//检测当前的地图缩放范围
 
	int scale = _layer->getCanvas()->getScaleLevel();
	int shift = gvmsSysConfig::instance().getPoiScaleLevel(scale);//(0~3)
	//默认不显示
	if( gvmsSysConfig::instance().getUserSettingValue(wxT("map.poi.visible"),wxT("0")) == wxT("1") ){
		if(_userpoi->maskLevel& (1<<shift)){  //地图级别允许显示
			GeoMapSymbolBase::draw();
		}
	}
}

void	GeoMapSymbol_UserPoi::drawCompound(){
}


geom::Envelope		GeoMapSymbol_UserPoi::getEnvelope(){
	return GeoMapSymbolBase::getEnvelope();
}

void	GeoMapSymbol_UserPoi::setGeoXY(double x,double y,bool & viewPtChanged){
	GeoMapSymbolBase::setGeoXY(x,y,viewPtChanged);
	_userpoi->geoX = x;
	_userpoi->geoY = y;
}

geom::Coordinate	GeoMapSymbol_UserPoi::getGeoXY(){
	return geom::Coordinate(_userpoi->geoX,_userpoi->geoY);
}

int		GeoMapSymbol_UserPoi::getObjectId(){
	return _userpoi->sid;
}

void GeoMapSymbol_UserPoi::init(){
	//shared_ptr< SysResIcon> icon = SysResMgr::instance().getIcon(_userpoi->iconId);
	setIcon(SysResMgr::instance().getIcon(_userpoi->iconId));	
}

wxString GeoMapSymbol_UserPoi::getText(){
	return wxString::FromUTF8(_userpoi->name.c_str());
}

void	GeoMapSymbol_UserPoi::drawIcon(){
	//setIcon(SysResMgr::instance().getIcon(_userpoi->iconId));	
	GeoMapSymbolBase::drawIcon();
}

void  GeoMapSymbol_UserPoi::setViewXY(int x, int y){
	GeoPointT gpt = _layer->getCanvas()->xy_View2Geo(ViewPointT(x,y));
	_userpoi->geoX = gpt.x;
	_userpoi->geoY = gpt.y;
}
//-- GeoMapSymbol_Barrier --------------------------------------------//

GeoMapSymbol_Barrier::GeoMapSymbol_Barrier(){
	_type = GEOMAP_SYMBOL_BARRIER;
}

void	GeoMapSymbol_Barrier::draw(){
	//shared_ptr<geom::Polygon> vpolygon = GeoMap_Geo2View(_geo_polygon.get(),_layer->getCanvas());
	if( gvmsSysConfig::instance().getUserSettingValue(wxT("map.barrier.visible"),wxT("0")) != wxT("1") ){
		return;
	}
	io::WKTReader reader;
	geom::Geometry* g = reader.read(_barrier->wktPolygon);
	if( g->getGeometryType()!="Polygon"){
		return;
	}
	shared_ptr<CoordinateSequence> cseq = shared_ptr<CoordinateSequence>( g->getCoordinates() );
	std::vector<GeoPointT > gpts;
	for(int n=0;n< cseq->getSize() ;n++){
		gpts.push_back( GeoPointT(cseq->getAt(n).x,cseq->getAt(n).y));
	}
	std::vector<ViewPointT> vpts;
	GeoMap_PointsGeo2View(gpts,vpts);
	
	//画多边形线框
	std::vector<wxPoint >  ptList;
	wxClientDC dc(_layer->getCanvas());
	shared_ptr<geom::Coordinate> last;
	for(int n=0;n< vpts.size() ;n++){
		ptList.push_back(wxPoint(vpts[n].x,vpts[n].y));
	}
		//与起始点的连线
		//dc.DrawLine( last->x,last->y,cseq->getAt(0).x,cseq->getAt(0).y);
	if( ptList.size()){
		wxBrush br(wxColour(128,255,128));
		wxPen pen(wxColour(128,255,128));
		//_layer->getDrawDC()->SetPen(pen);
		_layer->getDrawDC()->SetBrush(br);
		_layer->getDrawDC()->DrawPolygon(ptList.size(),&ptList[0]);
	}
}

void	GeoMapSymbol_Barrier::drawCompound(){
}

bool	GeoMapSymbol_Barrier::isIntersected(const ViewRectT & rc){
	//检测多边形中任何一点与视图区域相交则认为此多边形相交
	io::WKTReader reader;
	std::string text = _barrier->wktPolygon;
	geom::Geometry * g = reader.read(_barrier->wktPolygon);
	bool r = false;
	if( g && g->getGeometryType()=="Polygon"){
		;
	}else{
		return false;
	}
	shared_ptr<geom::CoordinateSequence> cs(g->getCoordinates());
	for(size_t n=0;n< cs->size();n++){
		const geom::Coordinate& cr = cs->getAt(n);
		ViewPointT vpt = _layer->getCanvas()->xy_Geo2View(GeoPointT(cr.x,cr.y));
		if( vpt.x >= rc.x && vpt.x< rc.x+rc.w && vpt.y>=rc.y && vpt.y<rc.y+rc.h ){
			return true;
		}
	}
	return false;
}

bool	GeoMapSymbol_Barrier::isIntersected(const ViewPointT& vpt){
	io::WKTReader reader;
	std::string text = _barrier->wktPolygon;
	std::auto_ptr<geom::Geometry> g (reader.read(_barrier->wktPolygon));
	bool r = false;
	if( g.get() && g->getGeometryType()=="Polygon"){
		;
	}else{
		return false;
	}
	shared_ptr<geom::Polygon> vpolygon = GeoMap_Geo2View((geom::Polygon*) g.get(),_layer->getCanvas());
	geom::Coordinate coor;
	coor.x = vpt.x;
	coor.y = vpt.y;
	geom::GeometryFactory factory;
	std::auto_ptr<geom::Point> pt (factory.createPoint(coor));
	return vpolygon->intersects(pt.get());
}

bool	GeoMapSymbol_Barrier::isGeoIntersected(const GeoRectT & rc){
	//shared_ptr<geom::CoordinateSequence> cseq = shared_ptr<geom::CoordinateSequence>( _geo_polygon->getCoordinates() );
	shared_ptr<geom::Polygon> polygon = GeoMap_CreatePolygon(rc);
	io::WKTReader reader;
	std::string text = _barrier->wktPolygon;
	geom::Geometry * g = reader.read(_barrier->wktPolygon);
	bool r = false;
	return true;
	if( g && g->getGeometryType()=="Polygon"){
		r = g->intersects( polygon.get() );
	}
	return r;
	
}
//视图举行区域
geom::Envelope		GeoMapSymbol_Barrier::getEnvelope(){
	shared_ptr<geom::Polygon> polygon = GeoMap_Geo2View(_geo_polygon.get(),_layer->getCanvas());
	return * polygon->getEnvelopeInternal();
}


geom::Coordinate	GeoMapSymbol_Barrier::getGeoXY(){
	//获取中心点坐标
	io::WKTReader reader;
	geom::Coordinate coor;
	std::string text = _barrier->wktPolygon;
	std::auto_ptr<geom::Geometry> g (reader.read(_barrier->wktPolygon));

	if( !g.get() || g->getGeometryType()!="Polygon"){
		;
	}else{
		g->getCentroid(coor);
	}
	return coor;
}

int		GeoMapSymbol_Barrier::getObjectId(){
	return _barrier->sid;
}

void GeoMapSymbol_Barrier::init(){
}

//计算当前坐标点与中心点的偏差，并依次修改每个顶点的坐标
void	GeoMapSymbol_Barrier::setGeoXY(double x,double y,bool & viewPtChanged){
	geom::Coordinate cxy;
	cxy = getGeoXY();
	if( cxy.x ==x && cxy.y == y){
		return;
	}
	double o_x,o_y;
	o_x = x - cxy.x;
	o_y = y - cxy.y;
	CoordinateSequence * cs;
	cs = _geo_polygon->getCoordinates();
	for( size_t n = 0;n<_geo_polygon->getNumPoints();n++){
		geom::Coordinate * pc =(geom::Coordinate *)&cs->getAt(n);
		pc->x+=o_x;
		pc->y+=o_y;
	}
}

void  GeoMapSymbol_Barrier::setViewXY(int x, int y){
	GeoPointT gpt;
	gpt = _layer->getCanvas()->xy_View2Geo(ViewPointT(x,y));
	bool vchg;
	setGeoXY(gpt.x,gpt.y,vchg);
}

shared_ptr<geom::Polygon>	GeoMapSymbol_Barrier::getViewPolygon(){
	return shared_ptr<geom::Polygon>();
}

ViewRectT GeoMapSymbol_Barrier::getIconRect(){
	return ViewRectT();
}

//-- GeoMapSymbol_Route --------------------------------------------//
GeoMapSymbol_Route::GeoMapSymbol_Route(){
	_type = GEOMAP_SYMBOL_ROUTE;
}
void	GeoMapSymbol_Route::draw(){
}
void	GeoMapSymbol_Route::drawCompound(){
}

bool	GeoMapSymbol_Route::isIntersected(const ViewRectT & rc){
	return false;
}
bool	GeoMapSymbol_Route::isGeoIntersected(const GeoRectT & rc){
	return false;
}

void GeoMapSymbol_Route::init(){
}