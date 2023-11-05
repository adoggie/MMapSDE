#include "geo_polygon.h"
#include "render.h"
#include "view.h"

swGeoPolygon::swGeoPolygon( swUInt16 num){
	_polyNum = num;
	_ftrcnt =0;
}

void swGeoPolygon::destroy(){
	int n;
	for(n=0;n<_ftrcnt;n++){
		swGeometryPolygon* g = _polygons[n];
		swHeapObjectFactory::instance().freeObject(g);	
	}
	if( _name ){
		swHeapObjectFactory::instance().freeObject(_name);
	}
}

/*
����Ҫ���part�Ƿ����ڿ��ӵ��������������������ཻ,������������
*/
bool swGeoPolygon::operator << ( swByteStream& stream){
//	stream.forward(2);
	std::string name;
	stream.readString(name);
	
	swAssertReturn(stream.readValue(_type));
	swUInt16 n;
	swGeometryPolygon *g;
	swUInt16 num;
	for(n=0;n<_polyNum;n++){
		swAssertReturn(stream.readValue(num));
		g = swHeapObjectFactory::instance().createMultiPoint(num);
		if( g){
			if( *g << stream == false){ //����stream���ݴ���
				swHeapObjectFactory::instance().freeObject(g);
				return false;
			}
			_polygons[n] = g;
			_ftrcnt++;
		}else{
			n=0;
		}
	}
	if( name.size()){
		_name = swHeapObjectFactory::instance().createString(name.c_str(),name.size());
	}
	return true;
}

void swGeoPolygon::render(swMapRender* r,swUInt8 layerId){
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
		ptNum = _polygons[n]->getSize();
		swMemBlockID mbid;
		swDrawPoint* pts = (swDrawPoint*)swMemHeapEngine::instance()->malloc(sizeof(swDrawPoint)*ptNum,mbid);
		for(swUInt16 p=0; p< ptNum;p++){
			swViewPoint pt = view->convertPointGeo2View( *_polygons[n]->getPointN(p));
			r->transform_point(pt.x,pt.y);	
			pts[p].x = pt.x;
			pts[p].y = pt.y;
		}
		 
		dc->DrawPolygon(ptNum,pts);
		swMemHeapEngine::instance()->free(mbid);
	}
	//���Ʊ�ע	
	if(! view->isAnnoVisible(layerId)){
		return;
	}
}

