
#ifndef _SW_GEO_POI_H
#define _SW_GEO_POI_H

#include "ftr_mpoint.h"
#include "object.h"

class swHeapObjectFactory;
/*
swGeoPOI
poi����һ������㣬���Ե���Ϣ��poi������Ϣ�洢���������ڴ��

*/
class swGeoPOI:public swGeoObject{
friend class swHeapObjectFactory;
	swGeoPOI( );
public:
	bool operator <<( swByteStream&); //���ֽ�����ȡ��������
	void destroy();
	void render(swMapRender* r,swUInt8 layerId);
private:
	swUInt16			_type;		// poi��� 
	swGeoPoint	_point;

};

#endif