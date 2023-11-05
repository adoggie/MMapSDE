
#ifndef _SW_GEO_POLYGON_H
#define _SW_GEO_POLYGON_H

#include "object.h"
#include "ftr_polygon.h"
/*
swGeoPolygon 
允许多多边形
*/
class swGeoPolygon:public swGeoObject{
friend class swHeapObjectFactory;
	swGeoPolygon( swUInt16 num);  //构造函数决定swGeoPolygon在堆上的大小
public:
	/*
	根据地理区域大小，不必加载所有的多边形对象，因为这些多边形可能超出了边界
	*/
	bool operator<< (swByteStream&); 
	void destroy();
	void render(swMapRender* r,swUInt8 layerId);
private:
	swUInt8							_type;
	swUInt16						_polyNum;		//多边形个数
	swUInt16						_ftrcnt;
	swGeometryPolygon**	_polygons;	//多边形指针
};

#endif