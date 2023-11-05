#ifndef _SW_RENDER_H
#define _SW_RENDER_H
#include "base.h"
#include "object.h"
#include "tile.h"
#include "matrix.h"

enum swRenderOutputType{
	ROT_BUFFER_RGB,
	ROT_BUFFER_PNG,
	ROT_BUFFER_JPG,
	ROT_SVG_TEXT
};

class swRenderOutput{
	swGeoRect						geoRc;
	swViewRect					viewRc;
	swRenderOutputType	type;
	std::vector<swByte> stream;
};


class swMapView;

enum swRenderType{
	RT_BUFFER,
	RT_VECTOR,
	RT_NULL
};
//�������� 
struct swRenderProps{
	swRenderType	rtype;   //���Ʒ�ʽ
};

class swMapRender:public swObject{
public:
	swMapRender(swMapView* view);
	~swMapRender();
	swMapView*		getView();
	swRenderType	getType();
// 	virtual void		drawLayer(swMapLayer* layer);
// 	virtual void		beginLayer(swMapLayer* layer); //��ʼ����ͼ��(�л�brush,color,pen...)
// 	virtual void		endLayer(swMapLayer* layer);		//���ͼ�����

	virtual void		beginLayer(swUInt8 layerId); //��ʼ����ͼ��(�л�brush,color,pen...)
	virtual void		endLayer(swUInt8 layerId);		//���ͼ�����

	virtual void		sizeChanged(const swViewSize&);  //��ͼ����ʵ����ı�,��Ҫ�����ڲ�����

	/*
	virtual swRenderOutput*	getOutput();
	virtual void		drawFeature(swGeoObject* obj); //ֱ�ӻ���feature���� ,���ڴ��ȱ�����֮�� 
	virtual void		drawIcon();
	virtual void		drawRoad();
	virtual void		drawRailLine();
	virtual void drawLineString(swGeometryMultiPoint* points );
	virtual void drawPolygon(swGeometryMultiPoint* points );
	virtual void drawMultiLineString(const swUInt16 num,swGeometryMultiPoint** points );
	virtual void drawMultiPolygon(const swUInt16 num,swGeometryMultiPoint** points );
	virtual void drawPoint(const swGeoPoint* point);
	virtual void drawText(const swGeoPoint* point,const swHeapString* text);
	*/
	virtual swDC*		getDC();						//��ȡ�����豸����
	//void DrawLine(int x1, int y1, int x2, int y2);
	void transform_point(int& x,int& y);
	void rotate(float radians,const swViewPoint& cpt=swViewPoint(0,0)); // cpt-��ת���ĵ㣬Ĭ�ϴ�0��ʼ
	void scale(float sx,float sy);
	void translate(float tx,float ty);
protected:
	swMapView*		_view;
	swRenderType	_type;
	swRenderOutput*	_output;
	swMemoryDC		_memdc;
	swBitmap*			_memBitmap;
	swMatrixT			_matrix;
	swViewPoint		_rotateCpt;
	float					_rotate; //��ת�Ƕ�
};

/*
swMapRenderBuffer �ڴ�������
*/
class swMapRenderBuffer:public swMapRender{

};

/**
swMapRenderVector ʸ���������
**/
class  swMapRenderVector:public swMapRender{
	

};

/*
swMapRenderSVG svg���
*/
class swMapRenderSVG:public swMapRenderVector{

};

#endif