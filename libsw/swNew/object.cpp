
#include "object.h"
#include "layer.h"
#include "geo_poi.h"
#include "geo_polygon.h"
#include "geo_road.h"
#include "memheap/memheap.h"


//////////////////////////////////////////////////////////////////////////
//
swHeapString* swHeapObjectFactory::createString(const char * ptr,swUInt16 size){
	swHeapString* r = NULL;
	swMemBlockID id;
	void *p = swMemHeapEngine::instance()->malloc( sizeof(swHeapString)+size+1,id) ;//ע���ڴ��ֽڶ�������
	if( p){
		r = new (p)swHeapString();
		memcpy( r+1,ptr,size);
		*((char*)(r+1)+size) = '\0';
		r->mbid = id;
		r->_ptr = ((char*)(r+1) );
		r->_size = size;
	}
	return r;
}

//�ͷŶ��ڴ��
void	swHeapObjectFactory::freeObject(swHeapObject* obj){
	if( !obj->mbid.isOk()){
		return;
	}
	obj->destroy();
	swMemHeapEngine::instance()->free(obj->mbid);
}


swGeoPOI*		swHeapObjectFactory::createGeoPOI(){
	swGeoPOI* r = NULL;
	swMemBlockID id;
	void *p = swMemHeapEngine::instance()->malloc( sizeof(swGeoPOI),id) ;//ע���ڴ��ֽڶ�������
	if( p){
		r = new (p)swGeoPOI();
		r->mbid = id;
	}
	return r;
}

swGeoRoad*	swHeapObjectFactory::createGeoRoad(swUInt16 num){
	swGeoRoad* r = NULL;
	swMemBlockID id;
	void *p = swMemHeapEngine::instance()->malloc( sizeof(swGeoRoad) + num*4,id) ;//ע���ڴ��ֽڶ�������
	if( p){
		r = new (p)swGeoRoad(num);
		r->mbid = id;
		r->_lines = (swGeometryLineString**)(r+1);  //�ڴ��Ŀ�ʼ��ַ
	}
	return r;
}

swGeoPolygon*	swHeapObjectFactory::createGeoPolygon(swUInt16 num){
	swGeoPolygon* r = NULL;
	swMemBlockID id;
	void *p = swMemHeapEngine::instance()->malloc( sizeof(swGeoPolygon)+ num*4,id) ;//ע���ڴ��ֽڶ�������
	if( p){
		r = new (p)swGeoPolygon(num);
		r->mbid = id;
		r->_polygons = (swGeometryPolygon**)(r+1);
		 
	}
	return r;
}

//////////////////////////////////////////////////////////////////////////
swGeometryMultiPoint* swHeapObjectFactory::createMultiPoint(swUInt16 num){
	swGeometryMultiPoint * r = NULL;
	swMemBlockID id;
	void *p = swMemHeapEngine::instance()->malloc( sizeof(swGeometryMultiPoint)+ sizeof(swGeoPoint)*(num),id) ;//ע���ڴ��ֽڶ�������
	if( p){
		r = new (p)swGeometryMultiPoint(num);
		r->mbid = id;
		r->_points =(swGeoPoint*)(r+1);  //ָ��β���ĵ�ַ����
	}	else{
		p = NULL;
	}
	return r;
}
//////////////////////////////////////////////////////////////////////////
swMapLayer* swHeapObjectFactory::createLayer(swUInt32 featureNum){
	swMapLayer* r = NULL;
	swMemBlockID id;
	void *p = swMemHeapEngine::instance()->malloc( sizeof(swMapLayer)+ sizeof(swHeapObject*) *(featureNum),id) ;//
	if( p){
		r = new (p)swMapLayer(featureNum);
		r->mbid = id;
		r->_features =(swGeoObject**)(r+1);  //ָ��β���ĵ�ַ����
	}
	return r;
}


swHeapObjectFactory swHeapObjectFactory::_handle;

swHeapObjectFactory& swHeapObjectFactory::instance(){
	return _handle;
}

