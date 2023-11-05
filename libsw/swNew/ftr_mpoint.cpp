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

//�����ж�ȡsize�������
bool swGeometryMultiPoint::operator <<( swByteStream& stream){
	swUInt16 n;
	for(n=0;n<_size;n++){
		swAssertReturn(stream.readValue(_points[n])); //���ܳ�������Խ��Υ��������������Ϸ��ز�ɾ����ǰ����Ķ����ڴ��
	}
	return true;
}


void swGeometryMultiPoint::destroy(){
	// do nothing
}