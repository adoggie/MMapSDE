
#include "map_map.h"
#include "map_view.h"

double _$_arScale[15] = {
                0.200000f,
                0.600000f,
                1.000000f,
                2.000000f,
                4.000000f,
                8.000000f,
                15.000000f,
                30.000000f,
                60.000000f,
                //120.000000f,
								150.0f,
                300.000000f,
                600.000000f,
                1200.000000f,
                2000.000000f,
                5000.000000f,       
    };

GeoMapViewConfigT::GeoMapViewConfigT(){
	hWnd = NULL;		
	centerXY.x = 121.462959;
	centerXY.y = 31.241604;
	centerXY.x = 121.410538;
	centerXY.y = 31.137504;
	map_scale_list.resize(sizeof(_$_arScale)/sizeof(double));
	memcpy( &map_scale_list[0],_$_arScale,sizeof(_$_arScale));
	scale = 3;	
}

///////
//////
////
GeoMapView::GeoMapView(GeoMapLib*	maplib){
	_maplib = maplib;
}

bool GeoMapView::init(GeoMapViewConfigT& conf){
	_conf = conf;
	return true;
}

GeoMapViewConfigT&	GeoMapView::getConfig(){
	return _conf;
}

void	GeoMapView::getBitmap(GeoMapBitmapT& bitmap){
}

void	GeoMapView::zoomIn(int cx,int cy,int level ){
}

void	GeoMapView::zoomInRect(ViewRectT rect){
}

void	GeoMapView::zoomOut(int cx,int cy,int level ){

}

void	GeoMapView::zoomOutRect(ViewRectT rect){

}

void	GeoMapView::xyV2M(int x,int y,GeoPointT& point){
}

void	GeoMapView::xyM2V(const GeoPointT& point,int &x,int &y){

}
void	GeoMapView::xyM2V(double px,double py,ViewPointT& xy){
}


void	GeoMapView::changeSize(int cx,int cy){

}

GeoRectT GeoMapView::getGeoRect(){
	return GeoRectT();
}

void			GeoMapView::moveGeoCenterXY(GeoPointT xy){}
void	GeoMapView::draw(){}
void GeoMapView::setScale(double dx, double dy,int scalelevel){}
//////////
////////
//////
GeoMapLib::GeoMapLib(){

}

bool	GeoMapLib::init(GeoMapInitConfigT& conf){
	_conf = conf;
	CSDEControler::GetSDEControl();
	CSDEControler::enErrorCode eErrorCode;
	CString mapPath = _conf.mapPath.c_str() ;
	if( !CSDEControler::GetSDEControl()->InitSDE( mapPath, mapPath , &eErrorCode) ){
		return false;
	}
	return true;
}

GeoMapInitConfigT&	GeoMapLib::getConfig(){
	return _conf;
}

GeoMapLib&	GeoMapLib::instance(){
	return _handle;
}

GeoMapView*		GeoMapLib::newMapView(){
	GeoMapViewImpl * view = new GeoMapViewImpl(this);
	return view;
}



GeoMapLib GeoMapLib::_handle;




