#ifndef _SW_MAP_H
#define _SW_MAP_H

#include "base.h"
#include "layer.h"
#include "view.h"
#include "persist.h"



/*
地图网格块编码规则:

网格编号采用32位的无符整数，但其真正只使用24位
0xaabbbccc
bbb - 图块的x编号 4k个单位
ccc - 图块的y编号 4k个单位
aa  -
地图坐标的原点是在左下角，向上是Y增大方向，向右是X增大方向
一个网格会在多个地图数据文件中出现
*/

typedef swUInt16 swMapIdentT;	//地图标识 

struct sw_breadthsize_desc_t{
	swUInt8		index;
	swGeoSize	size;
	swUInt8		layernum;	//层数量
};
//地图配置信息
struct swMapProps{
	swMapProps(){}
	swMapIdentT		id;				//地图编号
	swString		ver;
	swString		name;	
	//std::map<swMapPersistContainerIdentT,swMapPersistContainerProps>	containers;	//数据容器
	std::vector<swMapLayerProps>	layerProps; //n层定义属性
	swGeoRect		mbr;				//地图的区域范围
	std::vector<sw_breadthsize_desc_t> breadthsizes; //网格规格数组
	//swMapBreadthIdentT	orgBreadth;		//基准参考网格编号 左下角那个
	swGeoPoint		orgPoint;												//参考网格中心点坐标
	swUInt8		compress;																//压缩方式
	std::vector<swMapLayerDrawPropT>	layerDrawProps;	//层绘制控制属性  
	swFloat		 scaleUnits[SWMAP_SCALELEVEL_NUM];		//缩放比例常数表
};

class swMapLayerDataSet;
class swMapView;

class swMap{
public:
	swMap();
	~swMap();
	bool	create(const swString& mapfile);			//创建地图对象，读取地图信息
	swMapBreadthIdentT	getBreadthMap(const swGeoPoint& pt,swUInt8 sizeIdx=0);						//根据经纬度查询属于哪个图块
	
	bool getBreadthes(const swGeoRect& rc,swUInt8 sizeIdx,OUT std::vector<swMapBreadthIdentT>& ids); //获取图块列表,ids在调用者端设置了接收容量，切勿超出
	void render(swMapView* view,const swGeoRect grc,const swMapLayerIdentT& layerId); //渲染地理矩形

	swMapProps&		getProps();
	bool	getBreadthRect(const swMapBreadthIdentT& bid,OUT swGeoRect&);		//网格块的地理区域
	swUInt8 getLayerNum();	//地图图层数量
	swMapLayerProps* getLayerProps(const swMapLayerIdentT& lid);  //读取图层属性
	swGeoSize	getBreadthSize(swUInt8 idx); //读取网格大小规格
	swUInt8		getBreadthLayerNum(swUInt8 sizeIdx); //读取此种规格网格的图层数量
	swString getMapDirectory();				//获取地图所在的文件目录

private:
	swMapProps	_props;
	//swMapPersistContainerListT	_conts;  //文件存储对象列表
	swMutex		_mtx_layers;
	std::map<swMapLayerStoreIdentT,swMapLayer*>		_layers_cached;	//缓冲的图层数据
	/*缓冲的图层数据仅仅指那些网格全部落在当前可视地理范围内的网格图层数据,与当前可视区域相交的网格
	不能列为缓冲对象,因为那些相交的网格图层加载的对象是不完整的
	缓冲层数据因为存在不完整的情况，所以应该放置在view对象中，对于完整加载的layer在做服务器应用的场合时可以考虑
	放置与map对象中以便多个view共享

	*/
	std::list< shared_ptr<swRenderOutput> > _tiles_cached;				//缓冲的输出数据对象(图像是可以进行短暂的缓冲)
	swMutex		_mutex;
	swMapPersistContainerListT			_persists;	//存储容器
	swFile		_file;
	swString	_mapDir;
	
};


#endif