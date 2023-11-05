#ifndef _SW_VIEW_H
#define _SW_VIEW_H

#include "base.h"
#include "object.h"
#include "render.h"
#include "layer.h"
#include "layer_mgr.h"
#include "tile.h"

//view的唯一标识
struct swViewIdentT{
	swUInt32	id;
	swString	toString();
};


struct swMapViewProps{
	swViewRect						viewRc;			//当前视图矩形
	swGeoRect							geoRc;			//当前地理矩形
	swGeoPoint						centerXY;		//地图中心控制点
	swUInt8								scale;	//缩放级别 SWMAP_SCALELEVEL_NUM 从0下标开始
	swFloat		 scaleUnits[SWMAP_SCALELEVEL_NUM];	//每个像素表示n米 
	std::vector<swMapLayerProps> layerprops;	//层控制属性,可以任意修改多个层属性
	swRenderType					render;	//绘制器方式
};

class swMapLayerDataSet;
/*
如果用于超级服务器渲染，则可以将每一次渲染出来的图片缓冲到服务器，记录图形地理区域，缩放级别和图像数据即可
带再次请求数据时可以将已经渲染的图像进行拼接然后再次输出
每次服务器产生图像时也可进行切片传输，客户端将这些片进行缓存和组装来加速渲染
*/
class swMap;
class swMapServer;

#if SW_RPC_ICE_ENABLE
#include "rpc/ice/mapserver.h"
class swMapView:public swmap::IMapView{
public:
	swmap::MapBitmapTileListT	getBitmapTiles(Ice::Int scale,Ice::Int tileWidth,Ice::Int tileHeight,const swmap::MapViewTileRectListT& rects,const Ice::Current&  ctx);	
	swmap::MapScalePrecisionT	getScalePrecision(const Ice::Current&  ctx);
	void 		setScalePrecision(const swmap::MapScalePrecisionT& scales,const Ice::Current&  ctx);
	std::string  getId(const Ice::Current&  ctx);		//视图对象标识
	void	heartBeat(const Ice::Current&  ctx);	//心跳
	//////////////////////////////////////////////////////////////////////////
	void	setId(const Ice::Identity& id);
	//////////////////////////////////////////////////////////////////////////
	Ice::Identity getId();
	bool	isInvalid()	; //检测是否无效了
private:
	Ice::Identity		_iceId;
	int			_hbTime;		//上次心跳时间
#else
class swMapView{
#endif

public:
	swMapView(swMapServer* server);
	~swMapView();
	bool			create(swMap* map);
	void				setViewRect(const swViewRect& rc);		///<设置显示区域大小
	swViewRect	getViewRect();
	void				setCenterPoint(const swGeoPoint& );
	swGeoPoint	getCenterPoint();
	void				setCenterPoint(const swViewPoint& );
	swViewPoint	getViewCenterPoint();

	void				setScaleLevel(swUInt8 );							///<设置缩放级别 0 ~ (SWMAP_SCALELEVEL_NUM-1)
	swUInt8			getScaleLevel();
	//void				setScaleLevelTable(const swMapScaleLevelTableT&);
	void				setScaleLevelTable(const swFloat* units); //设置缩放级别
	swFloat*		getScaleLevelTable();

	swGeoRect		getGeoRect();															///<读取当前地理可视区域
	swRenderOutput*		getOutput();		///渲染输出数据
	void				update();		//请求绘制,并产生renderOutput
	swMapViewProps&	getProps();
	//void				setRender(shared_ptr<swMapRender>& r);		//s
	swMapRender*	getRender();
	 
	swMap*	getMap();
	void				setMap(swMap*);						//设置地图对象
	swMapLayerDataSet* getDataset();		
	swGeoPoint	convertPointView2Geo(const swViewPoint& pt); //转换视图坐标到地理坐标
	swViewPoint	convertPointGeo2View(const swGeoPoint& pt); //转换视图坐标到地理坐标
	swFloat		getCurScaleUnit();											// 获得当前缩放比下屏幕一像素表示多少地理米长度
	void		fitGeoRect(const swGeoRect& rc) ;								// 将指定大小的地理区域放入当前的视图窗体内可见，要求自动调节缩放比
	//std::vector<swMapLayer*>	getLayers(swMapView* view, const swMapBreadthIdentT& breadId,const swMapLayerIdentT& layerId); //获取图层数据
	bool			isAnnoVisible(swUInt8  layerId);			//当前层对象的标注是否显示
#define CURRENT_SCALE 0xff
	swMapLayerDrawPropT* getLayerDrawProp(swUInt8 layerId,swUInt8 scale=CURRENT_SCALE); //
	void	setLayerDrawProp(swUInt8 layerId,swUInt8 scale,const swMapLayerDrawPropT& drawprop);
	swMapLayerProps* getLayerProps(swUInt8 id); 

	bool	getBitmapTiles(swUInt8 scale,swUInt16 tileWidth,swUInt16 tileHeight,const swViewTileRectListT rects,OUT swMapBitmapTileListT& tiles); //请求网格块渲染
	void	setId(const std::string id);
private:
	//swViewIdentT									_id;
	std::string								_id;
	swMapViewProps								_props;
	swMap*							_map;											//当前使用的地图
	swMapRender*					_render;							//输出
	swMapLayerDataSet*	 _layerDataset;	//层数据集
	swMapServer* _server;

};

#endif