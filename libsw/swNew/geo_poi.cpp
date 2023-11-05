#include "geo_poi.h"
#include "view.h"
#include "render.h"

swGeoPOI::swGeoPOI(){

}

bool swGeoPOI::operator <<( swByteStream& stream){
	//swGeoObject::operator <<(stream);
	std::string name;
	stream.readString(name);
	if( name.size()){
		_name = swHeapObjectFactory::instance().createString(name.c_str(),(swUInt16)name.size());
	}
	swAssertReturn(stream.readValue(_type));
	swAssertReturn(stream.readValue(_point));
	return true;
}

void swGeoPOI::destroy(){
	swGeoObject::destroy();
	if(_name){
		swHeapObjectFactory::instance().freeObject(_name);
	}
}

void swGeoPOI::render(swMapRender* r,swUInt8 layerId){
	swMapView * view;
	view = r->getView();
	swDC* dc;
	dc = r->getDC();
	swMapLayerDrawPropT* drawProp;
	drawProp = view->getLayerDrawProp(layerId);
	swViewPoint vpt;
	vpt  = view->convertPointGeo2View(_point);
	swDrawPoint pt;
	pt.x = vpt.x;
	pt.y = vpt.y;
	dc->DrawCircle(pt,3);
	if(! view->isAnnoVisible(layerId)){
		return;
	}
}	 