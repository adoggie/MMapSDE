
#ifndef _SW_GEO_POI_H
#define _SW_GEO_POI_H

#include "ftr_mpoint.h"
#include "object.h"

class swHeapObjectFactory;
/*
swGeoPOI
poi仅有一个坐标点，所以点信息与poi其他信息存储在连续的内存块

*/
class swGeoPOI:public swGeoObject{
friend class swHeapObjectFactory;
	swGeoPOI( );
public:
	bool operator <<( swByteStream&); //从字节流读取对象数据
	void destroy();
	void render(swMapRender* r,swUInt8 layerId);
private:
	swUInt16			_type;		// poi类别 
	swGeoPoint	_point;

};

#endif