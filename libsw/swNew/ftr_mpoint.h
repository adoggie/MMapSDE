
#ifndef _SW_FEATURE_MULTIPOINT_H
#define _SW_FEATURE_MULTIPOINT_H

#include "base.h"
#include "ftr_collection.h"
#include "ftr_point.h"
#include "object.h"

/*
swGeometryMultiPoint
�㼯��,�洢�Ķ���㶼������ͬһ���ڴ�����뱾�������һ��
��С���ڴ����16�ֽڣ�����������������㣬���Զ����Դ���һ��

@@�������߶λ��߶���Σ�����ӵ������ʱ�����Ҫ��⵱ǰͼ���Ƿ����ڿ��ӵ���������
һ�������ͼԪ�����Ƕ����������ɣ�����ֻ���ؿɼ��Ķ����,�������������: ��Щ��������Ժ�layer֮�����
���л���Ļ��������ӵ�������䶯֮����Щ�������ݾͲ���ȷ��,����Ϊ�˽���������ͬʱ�ֲ��û��弼����
��ÿ�λ��Ƶ�������֮�󣬻������еİ����ڿ��ӵ��������ڲ�������ӵ��������ཻ������ͼ��(��Ϊ��Щ�ཻ������
ͼ�����ݼ��ص����ݿ����ǲ������ģ���Ϊ���������ͼ�鲻������)

*/
class swMapRender;
class swGeometryMultiPoint:public swHeapObject{
public:
	friend class swMapRender;
	friend class swHeapObjectFactory;
	swGeometryMultiPoint( swUInt16 size);
	bool operator <<( swByteStream&);
	swUInt16		getSize();
	swGeoPoint*	getPointN(swUInt16 n);
	bool		intersects(const swGeoRect& rc) ; //�Ƿ��ཻ
	bool		within(const swGeoRect& rc);   //�Ƿ�ȫ������������
	void destroy();
	//void	draw(swDC* dc,const swMapLayerProps* props);
private:
	swUInt16			_size;
	swGeoPoint*		_points;  //��һ��������������ռ�֮��,_pointsָ���׵㿪ʼλ��
	
};



#endif
