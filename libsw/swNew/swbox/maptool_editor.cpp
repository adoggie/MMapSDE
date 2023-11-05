#include "map_canvas.h"
#include "maptool_editor.h"

#include "map_resource.h"
#include "map_help.h"
#include "mainapp.h"
#include "ui_userpoi_detail.h"
#include "errmsg.h"
#include "ui_barrier_detail.h"
#include "repository.h"
#include "ui_barrier_sel.h"
#include "sysconfig.h"
#include "ui_real_detail.h"
//////////////////////////////////////////////////////////////////////////
//GeoMapTool_ObjectEditor
GeoMapTool_ObjectEditor::GeoMapTool_ObjectEditor(){
	_cursorName = wxT("maptool_object_editor");
	_type = GMTT_EDITOR;
	init();
}

void	GeoMapTool_ObjectEditor::set(GeoMapCanvas*c){
	GeoMapToolBase::set( c );
}

void  GeoMapTool_ObjectEditor::onMouseEvent(wxMouseEvent & evt){
	static shared_ptr<GeoMapSymbolBase>  symbol;
	static bool dragging = false;
	if( evt.Dragging() && evt.LeftIsDown()){
		dragging = true;
	}
	if( evt.LeftDown( ) ){
		dragging = false;
		//车辆在最前被判断	
		/*
		if( selectTypes&GMTST_VEHICLE){
			symbol= _canvas->getLayer(GEOMAP_LAYER_VEHICLE)->getSymbol(ViewPointT(evt.GetX(),evt.GetY()));
		}
		if( selectTypes&GMTST_POI){
			symbol= _canvas->getLayer(GEOMAP_LAYER_USER_POI)->getSymbol(ViewPointT(evt.GetX(),evt.GetY()));
		}
		if( selectTypes & GMTST_BARRIER && symbol.get()==NULL){
			symbol= _canvas->getLayer(GEOMAP_LAYER_BARRIER)->getSymbol(ViewPointT(evt.GetX(),evt.GetY()));
		}
		*/
		symbol= _canvas->getLayer(GEOMAP_LAYER_USER_POI)->getSymbol(ViewPointT(evt.GetX(),evt.GetY()));
		if(!symbol.get()){
			symbol= _canvas->getLayer(GEOMAP_LAYER_BARRIER)->getSymbol(ViewPointT(evt.GetX(),evt.GetY()));
		}
		if( symbol.get()){
			_lastpt = shared_ptr<ViewPointT>( new ViewPointT(evt.GetX(),evt.GetY()));
		}
	}else if( evt.LeftUp()){
		if( !symbol.get()){
			return ;
		}
		if( !dragging ){
			if( symbol.get()){
				if( symbol->getType()==GEOMAP_SYMBOL_VEHICLE){	  //点选车辆，显示车辆信息及其运行状态 
					// scott ,to be done 
					GeoMapSymbol_Vehicle* vehicle = (GeoMapSymbol_Vehicle*)symbol.get();
					//uiFrameVehicleRealDetail::instance()->Show();
					//uiFrameVehicleRealDetail::instance()->set(vehicle->_vechile.sid);  //直接显示车辆运行参数
					
				}else if( gvmsSysConfig::instance().getUserSettingValue(wxT("map.poi.visible"),wxT("0"))==wxT("1")
				&& symbol->getType()==GEOMAP_SYMBOL_USER_POI){ //显示poi
					GeoMapSymbol_UserPoi * poi = (GeoMapSymbol_UserPoi*) symbol.get();
					uiFrameUserPoiDetail::instance()->Show();
					uiFrameUserPoiDetail::instance()->setSID( poi->_userpoi->sid);
					uiFrameUserPoiDetail::instance()->setName(wxString::FromUTF8(poi->_userpoi->name.c_str()));
					uiFrameUserPoiDetail::instance()->setGeoXY( poi->_userpoi->geoX, poi->_userpoi->geoY);
					uiFrameUserPoiDetail::instance()->setIconId(poi->_userpoi->iconId);
					uiFrameUserPoiDetail::instance()->setLevelMask(poi->_userpoi->maskLevel);
				}else if(	gvmsSysConfig::instance().getUserSettingValue(wxT("map.barrier.visible"),wxT("0"))==wxT("1")				
					&&symbol->getType()==GEOMAP_SYMBOL_BARRIER){
					geom::Coordinate cxy;					
					GeoMapSymbol_Barrier * barrier = (GeoMapSymbol_Barrier*) symbol.get();
					cxy = barrier->getGeoXY();
					uiFrameBarrierDetail::instance()->show(true);
					uiFrameBarrierDetail::instance()->setSID( barrier->_barrier->sid);
					uiFrameBarrierDetail::instance()->setName(wxString::FromUTF8(barrier->_barrier->name.c_str() ));
					uiFrameBarrierDetail::instance()->setGeoXY( cxy.x,cxy.y );
					uiFrameBarrierDetail::instance()->setGroup(barrier->_barrier->barrierGroupId);		
					//uiFrameBarrierSelectList::instance()->updateBarrier(barrier->_barrier->sid);
					uiFrameBarrierSelectList::instance()->selectBarrier(barrier->_barrier->sid);
				}else{
					// cann't reach here
					return;
				}
			}
		}
		dragging = false; //取消拖动标志
		if( _lastpt.get()){
			if( _lastpt->x == evt.GetX() && _lastpt->y == evt.GetY()){
				;
			}else{
				//////////////////////////////////////////////////////////////////////////
				gvms::CallReturnT r;
				r.succ = false;
				gvms::HashValueSet params;
				GeoPointT gpt;

				if( symbol.get()){
					gpt = _canvas->xy_View2Geo(ViewPointT(evt.GetX(),evt.GetY()));
					if( symbol->getType()==GEOMAP_SYMBOL_USER_POI){
						try{														
							params["lon"]= wxString::Format(wxT("%0.4f"),gpt.x).ToUTF8();
							params["lat"]= wxString::Format(wxT("%0.4f"),gpt.y).ToUTF8();
							if( symbol->getType() == GEOMAP_SYMBOL_USER_POI){
								GeoMapSymbol_UserPoi* poi = (GeoMapSymbol_UserPoi*)symbol.get();
								r = CMC_Application::instance().getRpcSTS()->updateUserPoi(poi->_userpoi->sid,params);
							}else if( symbol->getType() ==GEOMAP_SYMBOL_BARRIER ){
								GeoMapSymbol_Barrier* barrier = (GeoMapSymbol_Barrier*)symbol.get();
								r = CMC_Application::instance().getRpcSTS()->updateBarrier(barrier->_barrier->sid,params);
							}
							if(r.succ){
								symbol->setViewXY(evt.GetX(),evt.GetY());						
								symbol->getLayer()->redraw();
								_canvas->update(false);		
								//
								
							}else{
								wxMessageBox(ERRMSG_STS_ERROR);
								wxLogMessage(ERRMSG_STS_ERROR);
							}
						}catch(...){
							wxMessageBox(ERRMSG_NETWORK_ERROR);
							wxLogMessage(ERRMSG_NETWORK_ERROR);
						}
					}/// end poi commit GEOMAP_SYMBOL_USER_POI
					//////////////////////////////////////////////////////////////////////////
					else if(symbol->getType()==GEOMAP_SYMBOL_BARRIER){  //围栏提交
						//拖动围栏之后，每个多边形定点都要加上拖动的偏移量
						GeoPointT prev_pt = _canvas->xy_View2Geo(ViewPointT(_lastpt->x,_lastpt->y));
						double offx,offy;
						offx = gpt.x - prev_pt.x;
						offy = gpt.y - prev_pt.y;
						GeoMapSymbol_Barrier * barrier= (GeoMapSymbol_Barrier*)symbol.get();
						io::WKTReader reader;
						//wkt to geometry
						std::string text = barrier->_barrier->wktPolygon;
						std::auto_ptr<geom::Geometry> g (reader.read(text));
						 
						if( !g.get() || g->getGeometryType()!="Polygon"){
							;
						}else{
							//一次修改每个顶点
							std::vector<GeoPointT > pts = GeoMap_GetGeometryPoints<double>(g.get());
							size_t n;
							for(n=0;n<pts.size();n++){
								pts.at(n).x +=offx;
								pts.at(n).y +=offy;
							}
							std::string wkt;
							wkt = GeoMap_GetWktPolygon(pts);
							// commit to database
							try{
								params["polygon"] = wkt;
								r = CMC_Application::instance().getRpcSTS()->updateBarrier(barrier->_barrier->sid,params);
								if(r.succ){
									wxCriticalSectionLocker l(DataRepository::instance().barriers.mutex);
									barrier->_barrier->wktPolygon = wkt; // 修改wkt数据，绘制的时候symbol重新解析									
									//DataRepository::instance().barriers.dict[barrier->_barrier->sid]->wktPolygon = wkt;

									symbol->getLayer()->redraw();
									_canvas->update(false);	
									uiFrameBarrierDetail::instance()->setGeoXY(barrier->getGeoXY().x,barrier->getGeoXY().y);
									
									uiFrameBarrierSelectList::instance()->updateBarrier(barrier->_barrier->sid);
									uiFrameBarrierSelectList::instance()->selectBarrier(barrier->_barrier->sid);
								}else{
									wxMessageBox(ERRMSG_STS_ERROR);
									wxLogMessage(ERRMSG_STS_ERROR);
								}
							}catch(...){
								wxMessageBox(ERRMSG_NETWORK_ERROR);
								wxLogMessage(ERRMSG_NETWORK_ERROR);
							}
						}
					}
					//////////////////////////////////////////////////////////////////////////
				}//	if( symbol.get()){

				//////////////////////////////////////////////////////////////////////////
			}
		}
		symbol.reset();
		_lastpt.reset();
	}

	return;
	if( evt.LeftUp()){
		if(_lastpt.get()){
			//提交给服务器新的位置,maptool没办法只好与应用代码混合了
			gvms::CallReturnT r;
			r.succ = false;
			try{
				gvms::HashValueSet params;
				GeoPointT gpt;
				gpt = _canvas->xy_View2Geo(ViewPointT(evt.GetY(),evt.GetX()));
				params["LON"]= wxString::Format(wxT("%f"),gpt.x).ToUTF8();
				params["LAT"]= wxString::Format(wxT("%f"),gpt.y).ToUTF8();
				if( _cur_symbol->getType() == GEOMAP_SYMBOL_USER_POI){
					GeoMapSymbol_UserPoi* poi = (GeoMapSymbol_UserPoi*)_cur_symbol.get();
					r = CMC_Application::instance().getRpcSTS()->updateUserPoi(poi->_userpoi->sid,params);
				}else if( _cur_symbol->getType() ==GEOMAP_SYMBOL_BARRIER ){
					GeoMapSymbol_Barrier* barrier = (GeoMapSymbol_Barrier*)_cur_symbol.get();
					r = CMC_Application::instance().getRpcSTS()->updateBarrier(barrier->_barrier->sid,params);
				}

			}catch(...){}
			if(r.succ){
				_cur_symbol->setViewXY(evt.GetX(),evt.GetY());
				_cur_symbol->getLayer()->redraw();
				_canvas->update(false);
				//自动移动选择的图标移动的相对位移或则将其居中
			}
			_lastpt.reset();
			_cur_symbol.reset();
		}

	}
}


void GeoMapTool_ObjectEditor::onMapRedraw(const ViewRectT& rect){

}

void	GeoMapTool_ObjectEditor::setSelectTypes(unsigned int types){
	selectTypes = types;
}

unsigned int GeoMapTool_ObjectEditor::getSelectTypes(){
	return selectTypes;
}
