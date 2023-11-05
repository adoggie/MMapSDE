
#ifndef _swlayer_h
#define _swlayer_h

#include "base.h"
#include "geo.h"
#include "feature.h"
#include "memheap/heap.h"


class swMapPersistContainer;

struct sw_persist_layer_data_desc_t{
	swUInt8		id;
	//swGeometryTypeT		geomType;	// points or lines or polygons 几何对象类型 (GeometryTypeT)
	//swUInt8		compress;	// 0-non compress,1-zlib
	swUInt32	offset;
	swUInt32	size;			//存储的数据大小
	swUInt32	orgsize;	//未压缩前数据大小
};

struct swMapLayerDrawPropT{
	swUInt32	penColor;
	swUInt8		penWidth;
	swUInt32	penStyle;
	swUInt32	brushColor;
	swUInt32	brushStyle;
	swString	fontName;
	swUInt8		fontSize;
	swUInt32	fontColor;
	swUInt32	fontStyle;
};

//swLayerStoreIdentT 目前考虑作为图层数据缓存时采用的图层标识
struct swMapLayerStoreIdentT{
	swMapPersistContainerIdentT persist;	//存储对象编号
	swMapBreadthIdentT		breadth;				//网格图
	swMapLayerIdentT			layer;					//层
};
/*
为了实现所有图层储存在同一份地图结构中而重新设计图层数据结构，使更清晰和简单
*/
struct swMapLayerProps{
	swUInt8			id; //	层编号		
	swUInt16		visibleMask	; // 0~15位
	swUInt16		annoMask;	//标注是否可见 ，道路和poi都是有文本标注的
	swUInt8			geomType;	//数据对象类型
	swUInt8			sizeIdx;	//网格类型索引编号 0 - 
	//swUInt8			drawprops[SWMAP_SCALELEVEL_NUM];	//定义每个缩放级别下显示属性
	std::vector<swMapLayerDrawPropT> drawprops;	//定义每个缩放级别下显示属性
};

/*
swMapLayer 图层对象数是有限的，图层内关联的图元对象是被安排存储在内存堆空间上，以便加速访问

*/
class swMapRender;
class swMapView;
struct swMapLayerProps;
class swGeoObject;
class swMapLayer:public swHeapObject{
	friend class swHeapObjectFactory;
public:	
	swMapLayer(swUInt32 featureNum);
	~swMapLayer();
	
	swMapLayerIdentT&	getId();
	swMapBreadthIdentT&	getBreadthId();
	swUInt8&	getGeomType();
	swUInt32&	getFeatureNum();

	bool	loadFeatures(swByteStream& stream,swMapView* view = NULL);		//加载图元数据
	void	clearFeatures();	//清楚图元数据
	
	void	destroy();
	void	render(swMapRender* r);

	
private:
	bool loadPoi(swByteStream& stream);
	bool loadRoad(swByteStream& stream);
	bool loadPolygon(swByteStream& stream);
private:
	swMapLayerIdentT			_id;					//层编号
	swMapBreadthIdentT		_bid;					//网格编号
	swUInt8								_geomType;		//对象数据类型

	swUInt32						_featureNum;			//图元对象数量
	swUInt32						_ftrcnt;
	swGeoObject** 			_features;				//features指向图元数据的指针地址集合的连续内存块空间
	//图元数据的指针并不是存在图层对象的同一块内存地址空间
	//在系统的运行过程中，图层是连续的被创建跟摧毁，所以图层的加载速度会影响系统的运行效率，所以考虑将图层对象设计成内层堆对象
	//不同的图元存储在分隔的内存堆块内，这些内存块的地址存储在layer对象的内存堆块的后部，紧挨着layer数据结构之后
};

inline 
swMapLayer::swMapLayer(swUInt32 featureNum){
	_featureNum = featureNum;
	_ftrcnt = 0;
}

typedef std::vector<swMapLayer*>	swMapLayerListT;





#endif