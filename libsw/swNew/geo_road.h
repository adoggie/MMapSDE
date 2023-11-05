
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
	swGeoRoad( swUInt16 num);				//����Ԥ��֪�����������Ա����ָ���С
public:
	bool operator << ( swByteStream&);
	void destroy();
	void render(swMapRender* r,swUInt8 layerId);
private:
// 	void drawRailLine(swMapRender* r) ; //������·��
// 	void drawRoadLine(swMapRender* r);
private:
	swUInt8								_type;
	swUInt16							_lineNum;		//����θ���
	swUInt16							_ftrcnt;
	swGeometryMultiPoint**	_lines;			//ָ������ָ�������ַ,Ҳ���Ƕ���֮����ڴ��ַ��ÿ����������ݴ洢�ڲ�ͬ���ڴ���� 
};

#endif