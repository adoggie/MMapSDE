#include "layer.h"
#include "object.h"
#include "view.h"
#include "geo_poi.h"
#include "geo_road.h"
#include "geo_polygon.h"
#include "map.h"

swMapLayer::~swMapLayer(){

}


void swMapLayer::destroy(){
	swUInt32 n;
	for(n=0;n<_ftrcnt;n++){
		swHeapObjectFactory::instance().freeObject(_features[n]);
	}
}
// swMapLayerProps*	swMapLayer::getProps(){
// 
// }

// bool	swMapLayer::isAnnoVisible(){
// 
// 	return false;
// }
 

swMapLayerIdentT&	swMapLayer::getId(){
	return _id;
}

 
swMapBreadthIdentT&	swMapLayer::getBreadthId(){
	return _bid;
}

swUInt8&	swMapLayer::getGeomType(){
	return _geomType;
}
 
/*
loadFeatures
将数据读入_features指针地址上

map_layer_data_block_t{
	layerId			uint8				层编号
	featureNum	uint32			对象个数
	features	map_feature_object[1..n]
}


@params:
	stream - 已经完成了数据的定位跳跃 stream.current就是层数据的开始
*/
bool	swMapLayer::loadFeatures(swByteStream& stream,swMapView* view ){
	swGeoObject* f;
	swUInt32 n;
	swUInt16 num;
	swUInt8 geomType;
	geomType = view->getMap()->getLayerProps(_id)->geomType;
	for(n=0;n<_featureNum;n++){
		f = NULL;
		if( geomType ==  SWMAP_GEOMTYPE_POINT){
			f = swHeapObjectFactory::instance().createGeoPOI();
		}else if( geomType==SWMAP_GEOMTYPE_LINE){
			stream.readValue(num);
			f = swHeapObjectFactory::instance().createGeoRoad(num);
		}else if(geomType==SWMAP_GEOMTYPE_POLYGON){
			stream.readValue(num);
			f = swHeapObjectFactory::instance().createGeoPolygon(num);
		}else{
			return false;
		}
		if(!f){
			return true; //加载有限的图元对象
		}
		//f->setLayer(this);
		if( *f << stream == false){ //读入此地图对象的数据
			swHeapObjectFactory::instance().freeObject(f);
			break;
		}		
		//////////////////////////////////////////////////////////////////////////
		_features[_ftrcnt++] = f;
		
		if(false){ //在内存紧张的时候要求马上绘制出对象
			//view->getRender()->drawFeature(f);
		}
	}
	return true;
}

/*
map_feature_object{}
目前不考虑对象的检索，所以poi只需存放 坐标/名称/类型
map_feature_poi_t{
name			string_field_t			名称
type			uint8								类别
//	pointNum	uint8								点数目
point		geo_point_t
}
*/
// bool swMapLayer::loadPoi(swByteStream& stream){
// 	swGeoPOI* f;
// 	int n;
// 	for(n=0;n<_featureNum;n++){
// 		f = swHeapObjectFactory::instance().createGeoPOI();
// 		if(!f){
// 			break; //加载有限的图元对象
// 		}
// 		if( *f << stream == false){
// 			swHeapObjectFactory::instance().freeObject(f);
// 			break;
// 		}
// 		_features[_ftrcnt++] = f;
// 	}
// 
// 	return true;
// }

bool swMapLayer::loadRoad(swByteStream& stream){
	
	return true;
}

bool swMapLayer::loadPolygon(swByteStream& stream){
	return true;
}


void	swMapLayer::clearFeatures(){
}



void	swMapLayer::render(swMapRender* r){
	swUInt32 n;
	for(n=0;n<_ftrcnt;n++){
		swGeoObject * geobj = _features[n];
		if( geobj){
			geobj->render(r,_id);
		}
	}
}


swUInt32&	swMapLayer::getFeatureNum(){
	return _featureNum;
}