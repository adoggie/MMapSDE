/*
view_end.h
客户端视图绘制实现
缓冲输出图形块，支持快速更新地图和支持分布式架构
*/


#ifndef _SW_VIEW_END_H
#define _SW_VIEW_END_H

#include "base.h"
#include "object.h"
#include "render.h"
#include "layer.h"
#include "layer_mgr.h"
#include "view.h"
#include "tile.h"

#if SW_RPC_ICE_CLIENT
class swMapRpcClient;
#endif
 
class swMapViewEnd{
public:
	swMapViewEnd();
	~swMapViewEnd();
	bool			create(swMapView* sview=NULL);
	#if SW_RPC_ICE_CLIENT
	bool			create(swmap::MapIdentT& mapid,swMapRpcClient*);	
	#endif
	
	void				setViewRect(const swViewRect& rc);		///<设置显示区域大小
	swViewRect	getViewRect();
	void				setCenterPoint(const swGeoPoint& );
	swGeoPoint	getCenterPoint();
	void				setCenterPoint(const swViewPoint& );
	swViewPoint	getViewCenterPoint();

	void				setScaleLevel(swUInt8 );							///<设置缩放级别 0 ~ (SWMAP_SCALELEVEL_NUM-1)
	swUInt8			getScaleLevel();
	void				setScaleLevelTable(const swFloat* units); //设置缩放级别
	swFloat*		getScaleLevelTable();

	swGeoRect		getGeoRect();															///<读取当前地理可视区域
	swMapViewProps&	getProps();
	swViewIdentT&				getId();
	swGeoPoint	convertPointView2Geo(const swViewPoint& pt); //转换视图坐标到地理坐标
	swViewPoint	convertPointGeo2View(const swGeoPoint& pt); //转换视图坐标到地理坐标
	swFloat		getCurScaleUnit();											// 获得当前缩放比下屏幕一像素表示多少地理米长度
	void		fitGeoRect(const swGeoRect& rc) ;								// 将指定大小的地理区域放入当前的视图窗体内可见，要求自动调节缩放比
	
#define CURRENT_SCALE 0xff
	swMapLayerDrawPropT* getLayerDrawProp(swUInt8 layerId,swUInt8 scale=CURRENT_SCALE); //
	void	setLayerDrawProp(swUInt8 layerId,swUInt8 scale,const swMapLayerDrawPropT& drawprop);
	swMapLayerProps* getLayerProps( swUInt8 id); 
	void	setBuffer(swUInt8 b);  //设置边界厚度
	//swViewSize getSizeOfCahceDC();	//计算缓冲DC的大小
	void render(swDC* dc=NULL);					//渲染到目标设备场景

	void sizeChanged(const swViewSize& size); //客户窗体改变大小
	swBitmap*		getMemBitmap();
	void				rotate(float radians) ;  //地图旋转 
private:
	#if SW_RPC_ICE_CLIENT
	swmap::MapIdentT _mapid;
	swMapRpcClient*	_rpclient;	
	#endif
private:
	swMapView*		_sview;	 // server view
	swMapViewProps	_props;
	swMemoryDC	_cachedc;	//缓冲DC
	swMemoryDC	_tmpdc;
	swBitmap*		_cacheBitmap;
	swBitmap*		_tmpBitmap;
	swUInt8			_buffer;	//边界缓冲网格数，默认为1
	swGeoPoint	_newCenterPt;
	swUInt8			_newScale;			//最新设置的缩放级别 
	float				_rotate;
	float				_newRotate;
};


/*
计算与中心点的偏差
*/
inline 
swGeoPoint	swMapViewEnd::convertPointView2Geo(const swViewPoint& pt){
	swViewPoint cxy = getViewCenterPoint();
	swGeoPoint r;
	r = _props.centerXY;
	r.x += ((pt.x - cxy.x) * getCurScaleUnit()*1.0)/sw_METERS_PER_DEGREE;
	r.y += ( -(pt.y - cxy.y) * getCurScaleUnit()*1.0)/sw_METERS_PER_DEGREE;//地理坐标的y就是纬度，与屏幕坐标的y方向相反
	return r;
}


/*
计算与中西地理坐标点的差值(度)，转换为米，计算当前比例尺下的像素个数
*/
inline 
swViewPoint	swMapViewEnd::convertPointGeo2View(const swGeoPoint& pt){
	swGeoPoint cxy = _props.centerXY;
	swViewPoint r = getViewCenterPoint();
	r.x += (pt.x-cxy.x)*sw_METERS_PER_DEGREE/(getCurScaleUnit()*1.0);
	r.y += -(pt.y-cxy.y)*sw_METERS_PER_DEGREE/(getCurScaleUnit()*1.0); //两个坐标方向相反
	return r;
}

#endif

/*
当前视图中心点的地理坐标为参照基准点，地图视图移动是计算新的中心点与当前中心点的位移来计算视图缓冲DC的偏差缓冲

*/