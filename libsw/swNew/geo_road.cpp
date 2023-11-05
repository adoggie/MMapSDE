#include "geo_road.h"
#include "ftr_polygon.h"

#include "memheap/heap.h"
#include "render.h"
#include "view.h"


swGeoRoad::swGeoRoad( swUInt16 num){
	_lineNum = num;
	_ftrcnt = 0;
}

void swGeoRoad::destroy(){
	int n;
	for(n=0;n<_ftrcnt;n++){
		swGeometryPolygon* g = _lines[n];
		swHeapObjectFactory::instance().freeObject(g);	
	}
	if( _name ){
		swHeapObjectFactory::instance().freeObject(_name);
	}
}

/*
linestrings和polygons都是多对象的组合，根据地理显示区域不必加载全部的子对象
*/
bool swGeoRoad::operator << ( swByteStream& stream){
	//stream.forward(2); //跳跃折线数
	std::string name;
	stream.readString(name);
	
	stream.readValue(_type);

	swUInt16 n;
	swGeometryPolygon *g;
	swUInt16 num;
	for(n=0;n<_lineNum;n++){
		stream.readValue(num);
		g = swHeapObjectFactory::instance().createMultiPoint(num);
		if( g){
			if( *g << stream == false){ //发现stream数据错误
				swHeapObjectFactory::instance().freeObject(g);
				return false;
			}
			_lines[n] = g;
			_ftrcnt++;
		}
	}
	if( name.size()){
		_name = swHeapObjectFactory::instance().createString(name.c_str(),name.size());
	}
	return true;
}

void swGeoRoad::render(swMapRender* r,swUInt8 layerId){
	swMapView * view;
	view = r->getView();
	swDC* dc;
	dc = r->getDC();
	swMapLayerDrawPropT* drawProp;
	drawProp = view->getLayerDrawProp(layerId);
	swViewPoint vpt;
	swUInt16 n;
	for(n=0;n<_ftrcnt;n++){
		swUInt16 ptNum;
		ptNum = _lines[n]->getSize();
		swViewPoint lastpt;
		bool firstpt =true;
		for(swUInt16 p=0; p< ptNum;p++){
			swViewPoint pt = view->convertPointGeo2View( *_lines[n]->getPointN(p));		
			r->transform_point(pt.x,pt.y);	
			if( firstpt){
				lastpt = pt;	
				firstpt = false;
				continue;
			}
			if( lastpt != pt){
				dc->DrawLine(lastpt.x,lastpt.y,pt.x,pt.y);
				lastpt = pt;
			}
		}
	}
	//绘制道路标注	
	if(! view->isAnnoVisible(layerId)){
		return;
	}
}
 

/**


*/