#include "map_canvas.h"
#include "map_tools.h"
#include "map_resource.h"
#include "map_help.h"
#include "mainapp.h"

///// GeoMapToolBase  /////////////////////////////////////////////////
void	GeoMapToolBase::set(GeoMapCanvas* c){
	static shared_ptr< wxCursor> keptCursor;
	if( c ){
		keptCursor = shared_ptr<wxCursor>(new wxCursor( c->GetCursor() ));
		if( _cursor.get() ){
			c->SetCursor( *_cursor.get() );
		}
		_canvas = c;
	}else{ // c== NULL
		if( _canvas ){
			if( keptCursor.get()){
				_canvas->SetCursor( *keptCursor.get() );
			}
		}		
		_canvas = NULL;
	}
}
void  GeoMapToolBase::onMouseEvent(wxMouseEvent & evt){
}

GeoMapToolBase::GeoMapToolBase(){
	_canvas = NULL;
	_type = GMTT_NULL;
}

void GeoMapToolBase::init(){
	shared_ptr<SysResIcon> icon = SysResMgr::instance().getIcon(_cursorName);
	if( icon.get() ){
		wxImage image = icon->bitmap.ConvertToImage();
		_cursor = shared_ptr<wxCursor>(new wxCursor(image) );
	}
}

void	GeoMapToolBase::drawRect(const ViewRectT& rect,wxDC * dc,bool restore /*=false 是否还原场景*/){
	if( !restore ){
		wxPen   pen(*wxRED,1,wxDOT_DASH);
		dc->SetPen(pen);
		//dc->SetLogicalFunction(wxXOR); 
		
		dc->DrawLine(rect.x,rect.y,rect.x+rect.w,rect.y);
		dc->DrawLine(rect.x,rect.y,rect.x,rect.y + rect.h);
		dc->DrawLine(rect.x,rect.y+rect.h,rect.x+rect.w,rect.y+rect.h);
		dc->DrawLine(rect.x+rect.w,rect.y,rect.x+rect.w,rect.y+rect.h);		
		
		//dc->Blit( rect.x,rect.y,rect.w,rect.h,_canvas->getDrawDC(),rect.x,rect.y,wxINVERT);
	}else{
		dc->Blit( rect.x,rect.y,rect.w+1,rect.h+1,_canvas->getTopmostLayer()->getDrawDC(),rect.x,rect.y);
	}
	
}

void	GeoMapToolBase::restoreRect(const ViewRectT& rect){
}


void GeoMapToolBase::addClient(GeoMapToolClient* c){
	wxCriticalSectionLocker l(_cs_clients);
	if( std::find(_clients.begin(),_clients.end(),c) == _clients.end() ){
		_clients.push_back(c);
	}
}

void GeoMapToolBase::delClient(GeoMapToolClient* c){
	wxCriticalSectionLocker l(_cs_clients);
	_clients.remove(c);
}

void GeoMapToolBase::onMapRedraw(const ViewRectT& rect){
	
}
//////////////////////////////////////////////////////////////////////////
/////GeoMapTool_Pan /////////////////////////////
GeoMapTool_Pan::GeoMapTool_Pan(){
	_cursorName = wxT("maptool_pan");
	_type = GMTT_PAN;
	init();

}

void	GeoMapTool_Pan::set(GeoMapCanvas* c){
	GeoMapToolBase::set(c);
}

void  GeoMapTool_Pan::onMouseEvent(wxMouseEvent & evt){
	if( evt.ButtonDown(wxMOUSE_BTN_LEFT ) ){
		_startPt = shared_ptr<ViewPointT>( new ViewPointT);
		_startPt->x = evt.GetX();
		_startPt->y = evt.GetY();		
	}else if( evt.ButtonUp(wxMOUSE_BTN_LEFT)){
		//开始移动
		if( _startPt.get()){
			int offx,offy;
			offx = evt.GetX() - _startPt->x;
			offy = evt.GetY() - _startPt->y;
			ViewPointT vxy( _canvas->getViewRect().w/2,_canvas->getViewRect().h/2);		//视图中心点
			vxy.x-=offx;
			vxy.y -=offy;
			GeoPointT cxy = _canvas->xy_View2Geo( vxy );
			_canvas->getView()->moveGeoCenterXY( cxy ); //新坐标点为中心点
			_startPt.reset();
			_canvas->update();
		}
	}
}

////////GeoMapTool_ZoomIn ///////////////////////////////
GeoMapTool_ZoomIn::GeoMapTool_ZoomIn(){
	_cursorName = wxT("maptool_zoomin");
	_type = GMTT_ZOOMIN;
	init();
}
//以鼠标点为中心放大
void	GeoMapTool_ZoomIn::set(GeoMapCanvas* c){
	GeoMapToolBase::set( c );

}

void  GeoMapTool_ZoomIn::onMouseEvent(wxMouseEvent & evt){
	static bool startDrag = false;
	static wxPen oldPen;
	static shared_ptr<wxClientDC> drawDC;
	static shared_ptr<ViewRectT> drawRC;

	if( evt.ButtonDown(wxMOUSE_BTN_LEFT ) ){
		_startPt = shared_ptr<ViewPointT>( new ViewPointT( evt.GetX(),evt.GetY()) );
	}else if( evt.ButtonUp(wxMOUSE_BTN_LEFT ) ){
		if( !drawRC.get()){ // have not dragged
			_canvas->getView()->zoomIn( evt.GetX(),evt.GetY());
			_canvas->update();
		}else{
			drawRect(*drawRC.get(),drawDC.get(),true);
			//放大当前选择区域
			if( drawRC->w > 10 && drawRC->h > 10 ){
					//  waitting for you completing 
				_canvas->scaleToViewRect(*drawRC.get(),true);
			}
		}
		drawDC.reset();
		drawRC.reset();
	}else if( evt.Dragging() && evt.LeftIsDown() ){
		if( !drawDC.get()){
			drawDC = shared_ptr<wxClientDC>(new wxClientDC(_canvas));			
		}
		if( drawRC.get() ){
			drawRect(*drawRC.get(),drawDC.get(),true); //还原上一次场景
		}else{
			drawRC = shared_ptr<ViewRectT>(new ViewRectT());
		}
		drawRC->x = _startPt->x;
		drawRC->y = _startPt->y;
		drawRC->w = evt.GetX()-_startPt->x;
		drawRC->h = evt.GetY()-_startPt->y;
		GeoMap_NormalizeRect(*drawRC.get());
		drawRect(*drawRC.get(),drawDC.get()); //绘制当前选择区域
	}else if( evt.Leaving() ){
		if( drawRC.get() ){
			drawRect(*drawRC.get(),drawDC.get(),true);
		}
	}
}

////////GeoMapTool_ZoomOut /////////////////////
GeoMapTool_ZoomOut::GeoMapTool_ZoomOut(){
	_cursorName = wxT("maptool_zoomout");
	_type = GMTT_ZOOMOUT;
	init();
}

void	GeoMapTool_ZoomOut::set(GeoMapCanvas* c){
	GeoMapToolBase::set( c );

}


void  GeoMapTool_ZoomOut::onMouseEvent(wxMouseEvent & evt){
	static bool startDrag = false;
	static wxPen oldPen;
	static shared_ptr<wxClientDC> drawDC;
	static shared_ptr<ViewRectT> drawRC;

	if( evt.ButtonDown(wxMOUSE_BTN_LEFT ) ){
		_startPt = shared_ptr<ViewPointT>( new ViewPointT( evt.GetX(),evt.GetY()) );
	}else if( evt.ButtonUp(wxMOUSE_BTN_LEFT ) ){
		if( !drawRC.get()){ // have not dragged
			_canvas->getView()->zoomOut( evt.GetX(),evt.GetY());
			_canvas->update();
		}else{
			drawRect(*drawRC.get(),drawDC.get(),true);
			//当前选择区域
			if( drawRC->w > 10 && drawRC->h > 10 ){
					//  waitting for you completing 
				_canvas->scaleToViewRect(*drawRC.get(),false);
			}
		}
		drawDC.reset();
		drawRC.reset();
	}else if( evt.Dragging() && evt.LeftIsDown() ){
		if( !drawDC.get()){
			drawDC = shared_ptr<wxClientDC>(new wxClientDC(_canvas));
		}
		if( drawRC.get() ){
			drawRect(*drawRC.get(),drawDC.get(),true); //还原上一次场景
		}else{
			drawRC = shared_ptr<ViewRectT>(new ViewRectT());
		}
		if( _startPt.get()){
			drawRC->x = _startPt->x;
			drawRC->y = _startPt->y;
			drawRC->w = evt.GetX()-_startPt->x;
			drawRC->h = evt.GetY()-_startPt->y;
			GeoMap_NormalizeRect(*drawRC.get());
			drawRect(*drawRC.get(),drawDC.get()); //绘制当前选择区域
		}
	}else if ( evt.Leaving() ){
		if( drawRC.get() ){
			drawRect(*drawRC.get(),drawDC.get(),true);
		}
	}
}

///// GeoMapTool_Select  /////////////////////////////////////////////////
GeoMapTool_Select::GeoMapTool_Select(){
	_cursorName = wxT("maptool_select");
	_type = GMTT_SELECT;
	init();
	selectTypes = GMTST_NULL;
}
void	GeoMapTool_Select::set(GeoMapCanvas*c){	
	GeoMapToolBase::set( c );
}

void	GeoMapTool_Select::setSelectTypes(unsigned int types){
	selectTypes = types;
}

unsigned int GeoMapTool_Select::getSelectTypes(){
	return selectTypes;
}

void  GeoMapTool_Select::onMouseEvent(wxMouseEvent & evt){
	static bool startDrag = false;
	static wxPen oldPen;
	static shared_ptr<wxClientDC> drawDC;
	static shared_ptr<ViewRectT> drawRC;
	

	if( evt.ButtonDown(wxMOUSE_BTN_LEFT ) ){
		_startPt = shared_ptr<ViewPointT>( new ViewPointT( evt.GetX(),evt.GetY()) );
		GeoMapSymbolList::iterator itr;
		for(itr = _cachedlist.begin();itr!=_cachedlist.end();itr++){
			(*itr)->setFocus(false);
		}
	}else if( evt.ButtonUp(wxMOUSE_BTN_LEFT ) ){
		if( !drawRC.get()){ // have not dragged
			//处理点选对象
			_cachedlist = _canvas->mapSelectViewPoint(ViewPointT(evt.GetX(),evt.GetY()),&evt,this);
		}else{
			drawRect(*drawRC.get(),drawDC.get(),true); //还原场景
			//处理框选对象
			_cachedlist=_canvas->mapSelectViewRect( *drawRC.get(),&evt,this);
		}
		//==设置聚焦状态
		GeoMapSymbolList::iterator itr;
		for(itr = _cachedlist.begin();itr!=_cachedlist.end();itr++){
			(*itr)->setFocus();
		}
		//==
		drawDC.reset();
		drawRC.reset();
	}else if( evt.Dragging() && evt.LeftIsDown() ){ //区域拖动中 
		if( !drawDC.get()){
			drawDC = shared_ptr<wxClientDC>(new wxClientDC(_canvas));
		}
		if( drawRC.get() ){
			drawRect(*drawRC.get(),drawDC.get(),true); //还原上一次场景
		}else{
			drawRC = shared_ptr<ViewRectT>(new ViewRectT());
		}
		if( _startPt.get()){
			drawRC->x = _startPt->x;
			drawRC->y = _startPt->y;
			drawRC->w = evt.GetX()-_startPt->x;
			drawRC->h = evt.GetY()-_startPt->y;
			GeoMap_NormalizeRect(*drawRC.get());
			drawRect(*drawRC.get(),drawDC.get()); //绘制当前选择区域
		}
	}else if ( evt.Leaving() ){
		if( drawRC.get() ){
			drawRect(*drawRC.get(),drawDC.get(),true);
		}
	}
}

////////	GeoMapTool_MeasureLine ////////////////////////////
GeoMapTool_MeasureLine::GeoMapTool_MeasureLine(){
	_cursorName = wxT("maptool_measure");
	_type = GMTT_MEASURE_LINE;
	init();
}

void	GeoMapTool_MeasureLine::set(GeoMapCanvas* c){
	GeoMapToolBase::set( c );
}

//可以进一步优化处理减少刷新图像抖动的现象
//记录上一点坐标只是刷新上一点与当前鼠标点的矩形区域
void	GeoMapTool_MeasureLine::clearScene(){
			//清除上一次绘制区域
			geom::GeometryFactory factory;
			std::vector<geom::Coordinate> coords;			
			std::list<shared_ptr<ViewPointT> >::iterator itr;
			
			
			for( itr = _ptlist.begin(); itr!=_ptlist.end();itr++){			
				//geom::Coordinate c = geom::Coordinate((*itr)->x,(*itr)->y);
				coords.push_back( geom::Coordinate((*itr)->x,(*itr)->y));
			}
			if(_lastpt.get()){
				coords.push_back( geom::Coordinate(_lastpt->x,_lastpt->y));
			}
			//shared_ptr<CoordinateSequence> cs( factory.getCoordinateSequenceFactory()->create(&coords));			
			CoordinateSequence* cs =  factory.getCoordinateSequenceFactory()->create(&coords);			
									
			shared_ptr<geom::MultiPoint> mpts(factory.createMultiPoint(*cs ));
		//	delete cs;
			geom::Envelope envp = *(geom::Envelope*)mpts->getEnvelopeInternal(); //所有点的最小边界框
			envp.expandBy(4,4);
			ViewRectT updateRc;
			updateRc.x = (int)envp.getMinX();
			updateRc.y = (int)envp.getMinY();
			updateRc.w = (int)(envp.getMaxX()-envp.getMinX());
			updateRc.h = (int)(envp.getMaxY()-envp.getMinY());
			wxClientDC dc(_canvas);
			dc.Blit( updateRc.x,updateRc.y,updateRc.w+1,updateRc.h+1,_canvas->getTopmostLayer()->getDrawDC(),updateRc.x,updateRc.y);
}

void  GeoMapTool_MeasureLine::onMouseEvent(wxMouseEvent & evt){
	if( evt.ButtonDown(wxMOUSE_BTN_LEFT ) ){
		_ptlist.push_back( shared_ptr<ViewPointT>( new ViewPointT(evt.GetX(),evt.GetY())));
		{
			if( _ptlist.size()==1){
				wxCriticalSectionLocker l(_cs_clients);
				std::list<GeoMapToolClient*>::iterator itr;
				for(itr= _clients.begin();itr!=_clients.end();itr++){
					GeoMapTool_Event evt;
					evt.src = GMTT_MEASURE_LINE;
					evt.msg[wxT("MSG")] = wxT("MEASURE_LINE_START");				
					(*itr)->OnMapToolEvent(evt);				
				}
			}
		}
	}
	std::list<shared_ptr<ViewPointT> >::iterator itr;
	// calc distances between points	
	//
	double dist_points,dist_off;
	dist_points=dist_off=0;
	if( evt.Moving()){
		if( _ptlist.size() ){
			clearScene();
			//-- new coordinate
			_lastpt = shared_ptr<ViewPointT>( new ViewPointT(evt.GetX(),evt.GetY()));
			dist_points = _canvas->getGeoDistance( _ptlist);
			itr = _ptlist.end();
			itr--;		// get last point
			dist_off = _canvas->getGeoDistance( *(*itr).get(), ViewPointT(evt.GetX(),evt.GetY()));
			// draw linestring
			shared_ptr<ViewPointT> last;
			wxClientDC dc(_canvas);
			wxPen   pen(*wxBLUE,3);
			dc.SetPen(pen);
			for( itr = _ptlist.begin(); itr!=_ptlist.end();itr++){				
				if( last.get()){
					dc.DrawLine( last->x,last->y, (*itr)->x,(*itr)->y);
				}
				last = (*itr);
			}
			dc.DrawLine( last->x,last->y, _lastpt->x,_lastpt->y);
			//////////////////
			//准备回送数据,send data back to user
			{
					wxCriticalSectionLocker l(_cs_clients);
					std::list<GeoMapToolClient*>::iterator itr;
					for(itr= _clients.begin();itr!=_clients.end();itr++){
						GeoMapTool_Event evt;
						evt.src = GMTT_MEASURE_LINE;
						evt.msg[wxT("MSG")] = wxT("MEASURE_LINE_DATA");
						evt.msg[wxT("DIST_POINTS")] = wxString::Format(wxT("%f"),dist_points); //已经结点之间的长度
						evt.msg[wxT("DIST_FREE")] = wxString::Format(wxT("%f"),dist_off);	//当前坐标与最后一个节点之间的距离
						(*itr)->OnMapToolEvent(evt);				
					}
			}
			////////////
		}
		//trace distance 
		//wxLogDebug(wxT("linestring's distance :%f"),dist_points+dist_off);
	}
	if( evt.RightUp()){
		if( _ptlist.size() ){
			clearScene();
		}
		_ptlist.clear();
		//--
		{
			wxCriticalSectionLocker l(_cs_clients);
			std::list<GeoMapToolClient*>::iterator itr;
			for(itr= _clients.begin();itr!=_clients.end();itr++){
				GeoMapTool_Event evt;
				evt.src = GMTT_MEASURE_LINE;
				evt.msg[wxT("MSG")] = wxT("MEASURE_LINE_END");				
				(*itr)->OnMapToolEvent(evt);				
			}
		}
		//--
	}
	
}
