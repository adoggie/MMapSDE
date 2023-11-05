
#ifndef _SW_GEO_POLYGON_H
#define _SW_GEO_POLYGON_H

#include "object.h"
#include "ftr_polygon.h"
/*
swGeoPolygon 
���������
*/
class swGeoPolygon:public swGeoObject{
friend class swHeapObjectFactory;
	swGeoPolygon( swUInt16 num);  //���캯������swGeoPolygon�ڶ��ϵĴ�С
public:
	/*
	���ݵ��������С�����ؼ������еĶ���ζ�����Ϊ��Щ����ο��ܳ����˱߽�
	*/
	bool operator<< (swByteStream&); 
	void destroy();
	void render(swMapRender* r,swUInt8 layerId);
private:
	swUInt8							_type;
	swUInt16						_polyNum;		//����θ���
	swUInt16						_ftrcnt;
	swGeometryPolygon**	_polygons;	//�����ָ��
};

#endif