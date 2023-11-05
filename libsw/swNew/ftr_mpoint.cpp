#include "ftr_mpoint.h"


swUInt16		swGeometryMultiPoint::getSize(){
	return _size;
}

swGeoPoint*	swGeometryMultiPoint::getPointN(swUInt16 n){
	if(n>=_size){
		return NULL;
	}
	return _points + n;
}

swGeometryMultiPoint::swGeometryMultiPoint( swUInt16 size){
	_size = size;
}

//从留中读取size个坐标点
bool swGeometryMultiPoint::operator <<( swByteStream& stream){
	swUInt16 n;
	for(n=0;n<_size;n++){
		swAssertReturn(stream.readValue(_points[n])); //可能出现数据越界违例的情况，便马上返回并删除当前请求的对象内存块
	}
	return true;
}


void swGeometryMultiPoint::destroy(){
	// do nothing
}