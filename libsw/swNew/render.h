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
//绘制属性 
struct swRenderProps{
	swRenderType	rtype;   //绘制方式
};

class swMapRender:public swObject{
public:
	swMapRender(swMapView* view);
	~swMapRender();
	swMapView*		getView();
	swRenderType	getType();
// 	virtual void		drawLayer(swMapLayer* layer);
// 	virtual void		beginLayer(swMapLayer* layer); //开始绘制图层(切换brush,color,pen...)
// 	virtual void		endLayer(swMapLayer* layer);		//完成图层绘制

	virtual void		beginLayer(swUInt8 layerId); //开始绘制图层(切换brush,color,pen...)
	virtual void		endLayer(swUInt8 layerId);		//完成图层绘制

	virtual void		sizeChanged(const swViewSize&);  //视图的现实区域改变,需要调整内部缓冲

	/*
	virtual swRenderOutput*	getOutput();
	virtual void		drawFeature(swGeoObject* obj); //直接绘制feature对象 ,在内存紧缺的情况之下 
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
	virtual swDC*		getDC();						//获取绘制设备场景
	//void DrawLine(int x1, int y1, int x2, int y2);
	void transform_point(int& x,int& y);
	void rotate(float radians,const swViewPoint& cpt=swViewPoint(0,0)); // cpt-旋转中心点，默认从0开始
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
	float					_rotate; //旋转角度
};

/*
swMapRenderBuffer 内存绘制输出
*/
class swMapRenderBuffer:public swMapRender{

};

/**
swMapRenderVector 矢量数据输出
**/
class  swMapRenderVector:public swMapRender{
	

};

/*
swMapRenderSVG svg输出
*/
class swMapRenderSVG:public swMapRenderVector{

};

#endif