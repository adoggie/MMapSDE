
#ifndef _SW_FEATURE_POLYGON_H
#define _SW_FEATURE_POLYGON_H

#include "feature.h"
#include "object.h"
#include "ftr_mpoint.h"

/*
swGeometryPolygon
swGeometryMultiPoint�ǿ��Ա�������������洢���ڴ���ڣ�����swGeometryPolygon�����������
*/
typedef swGeometryMultiPoint swGeometryPolygon;
/*
class swGeometryPolygon:public swGeometryMultiPoint{
public:
	swGeometryPolygon(  swUInt16 size);
	void operator<<( swByteStream&);
private:
};*/


#endif