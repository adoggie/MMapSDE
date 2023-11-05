
#ifndef _SW_GEO_ROAD_H
#define _SW_GEO_ROAD_H

#include "object.h"
#include "ftr_linestring.h"

/*
+-----------+------------+
| swGeoRoad | pinters... |
+-----------+^-----------+
*/

class swGeoRoad:public swGeoObject{
friend class swHeapObjectFactory;
	swGeoRoad( swUInt16 num);				//必须预先知道线条个数以便分配指针大小
public:
	bool operator << ( swByteStream&);
	void destroy();
	void render(swMapRender* r,swUInt8 layerId);
private:
// 	void drawRailLine(swMapRender* r) ; //绘制铁路线
// 	void drawRoadLine(swMapRender* r);
private:
	swUInt8								_type;
	swUInt16							_lineNum;		//多边形个数
	swUInt16							_ftrcnt;
	swGeometryMultiPoint**	_lines;			//指向多边形指针数组地址,也就是对象之后的内存地址，每个多边形数据存储在不同的内存块内 
};

#endif