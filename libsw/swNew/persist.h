
#ifndef _SW_PERSIST_H
#define _SW_PERSIST_H

#include "base.h"
#include "layer.h"

enum swMemoryTypeT{
	MT_HEAP,			//内存堆缓冲类型
	MT_MAPPING		//系统Mapping类型 
};

//网格图块信息
struct sw_persist_breadth_desc_t{
	swUInt32		id;
	swUInt32		offset;		//文件偏移
	swUInt32		size;			//图块数据大小
	//std::vector<sw_persist_layer_desc_t> layers; //层数据描述信息
};

struct sw_persist_breadthsize_desc_t{
	swUInt8			sizeId;	// swMapBreadthSizeInfo::id
	std::map<swMapBreadthIdentT,shared_ptr<sw_persist_breadth_desc_t> >		breadthes;	//图块信息
};

//网格数据带信息
struct sw_persist_breadthzone_desc_t{
	swUInt8		index;	//网格大小规格编号
	swGeoSize	size;		//数据大小
	swUInt32	offset;	//网格带在地图数据文件中的开始位置
	swUInt32	descoffset;	//描述信息在文件中的偏移
};
typedef std::vector<sw_persist_breadthzone_desc_t> sw_persist_breadthzone_list_t;

struct swMapPersistContainerProps{
	swMapPersistContainerIdentT id;
	swString		name;
	swString		file;		//文件名称
	swGeoRect		mbr;
	swUInt32		jmpOffset;	//尾部网格块定义

	swUInt16		breadthNum;	//图块数量
	std::vector<swMapBreadthIdentT>		bids;	// 避免平凡的磁盘访问而缓冲的网格编号。小内存环境下不能缓冲所有的网格和图层信息
	std::map<swMapBreadthIdentT,sw_persist_breadth_desc_t>	breadthes;	//如果内存允许的话直接加载所有图块和内存信息到内存
	//std::map<swMapBreadthSizeId , shared_ptr<sw_persist_breadthsize_desc_t> > breadthsizes;	//图块规格不同
	sw_persist_breadthzone_list_t	breadzones;		//网格数据带描述
	
};



class swMap;

// feature存储介质对象
// 每个存储介质在使用时使用文件映射机制访问对象
class swMapPersistContainer{
public: 
	swMapPersistContainer( swMap* map);
	~swMapPersistContainer();
	swMapPersistContainerIdentT	getId();
	bool	create(const swString& file);  //
	//shared_ptr<swMapLayer>	getLayer(const swMapBreadthIdentT& breadthId,swUInt8 layerId,swMapBreadthSizeId bsId); //
	bool	getLayers(swMapView* view,const swGeoRect& grc,swUInt8 layerId,OUT std::list<swMapLayer*>& layers); //
	

	swMapLayer*	getLayer(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId); //
	sw_persist_layer_data_desc_t* getLayerPersistInfo(const swMapBreadthIdentT& breadthId,swUInt8 layerId);//,swMapBreadthSizeId bsId);
	bool		getByteStream(OUT swByteStream&,const swMapBreadthIdentT& breadthId,swUInt8 layerId);//,swMapBreadthSizeId bsId); //获取图层数据字节流,解压缩之后的数据流
	swMapBreadthIdentT	getBreadthId_lb(swUInt8 sizeIdx =0);	//左下图块编号
	swMapBreadthIdentT	getBreadthId_rt(swUInt8 sizeIdx =0);	//右上图块编号
private:
	bool	getLayerDesc(const swMapBreadthIdentT& breadthId,swUInt8 layerId,OUT sw_persist_layer_data_desc_t& layer); //取层信息
	swMapLayer*	getLayerTiny(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId); 
	swUInt32 getLayerData(const sw_persist_layer_data_desc_t& desc,OUT swMemBlockID& mbid,OUT swMemoryTypeT& mtype);

	void push_env();	//保存当前的文件操作状态
	void pop_env();		//还原状态  为了节省内存空间，不允许嵌套调用
	swByte* readData(const swUInt32 offset,swByte* data,INOUT swUInt32& size); //读取size字节
private:
	swMapPersistContainerProps		_props;
	void *	_ptrMapping; //文件映射指针
	swMap*	_map;
	wxFile	_file;	//文件指针
	swMemBlockID	 _mb_zones; //网格区描述信息,remember to free 
	swUInt32 _zonelen;	// 网格描述区域大小
};
//typedef std::map<swMapPersistContainerIdentT, shared_ptr<swMapPersistContainer> > swMapPersistContainerListT;
typedef std::vector<swMapPersistContainer* > swMapPersistContainerListT;

/*
swMapPersistContainer
图层对存储文件访问器，支持对图元数据的读写

系统最少必须有两个文件:
1.地图描述文件: 描述 图元存储文件信息、图层控制信息、缩放级别控制信息等等
2.地图数据文件: 整个地图的所有地图图元可以存放在一个文件或者分散在多个文件中。 图元的组织是按 网格大小进行分区，在每个分区中
				存在n个网格容器，每个网格里存在1～n个地图层数据，每个图层组织了n多个图元对象数据
*/





#endif
