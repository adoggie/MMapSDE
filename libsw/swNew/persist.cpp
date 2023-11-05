#include "persist.h"
#include "object.h"
#include "zlib/zlib.h"
#include "layer.h"
#include "memheap/memheap.h"
#include "map.h"

swMapPersistContainer::swMapPersistContainer( swMap* map){
	_map = map;
	_mb_zones.bucketId = swMemBucketId_Null;
}

swMapPersistContainer::~swMapPersistContainer(){
	if(_mb_zones.isOk()){
		swMemHeapEngine::instance()->free(_mb_zones);
	}
}

/*地图数据存储文件
*/
bool	swMapPersistContainer::create(const swString& file){
	swString datafile;
	datafile = _map->getMapDirectory() +swT("/") +file;
	if( !_file.Open(datafile) ){
		return false;
	}
	swMemBlockID mbid;
	swUInt32 size;
	size = 1024;
	swByte* bytes = (swByte*)swMemHeapEngine::instance()->malloc(size,mbid);
	if(!bytes){
		return false;
	}
	if( ! readData(0,bytes,size)){
		swMemHeapEngine::instance()->free(mbid);
		return false;
	}
	swByteStream stream(bytes,size);
	swByte magic[17];
	magic[16] ='\0';
	memcpy(magic,stream.current(),16);
	stream.forward(16);
	if(memcmp((char*)magic,"swMapData",strlen("swMapData"))!=0){
		swMemHeapEngine::instance()->free(mbid);
		return false;
	}
	stream.readValue(_props.id);
	stream.readString(_props.name);
	stream.readValue(_props.mbr);
	stream.readValue(_props.jmpOffset);
	
	swMemHeapEngine::instance()->free(mbid);
	//////////////////////////////////////////////////////////////////////////
	//读网格带数据
	swUInt8 zoneNum;
	_file.Seek(_props.jmpOffset);
	_file.Read(&zoneNum,1);
	_zonelen = _file.Length() - _props.jmpOffset;
	if ( !swMemHeapEngine::instance()->malloc(_zonelen,_mb_zones) ){
		return false;  //请求堆内存失败
	}
	_file.Read(swMemHeapEngine::instance()->ptr(_mb_zones),_zonelen);
	stream = swByteStream((swByte*)swMemHeapEngine::instance()->ptr(_mb_zones),_zonelen);
	for( swUInt8 n=0 ;n < zoneNum;n++){
		sw_persist_breadthzone_desc_t zone;
		//zone.descoffset = _file.Tell();
// 		swReadValue(&_file,zone.index);
// 		swReadValue(&_file,zone.size);
// 		swReadValue(&_file,zone.offset);
		zone.descoffset = stream.current()-stream.begin();
		stream.readValue(zone.index); stream.readValue(zone.size); stream.readValue(zone.offset);
		swUInt16 bnum;
		//swReadValue(&_file,bnum);
		stream.readValue(bnum);
		//_file.Seek(bnum*(13+ SWMAP_LAYER_DATA_DESC_LEN* _map->getBreadthLayerNum(zone.index)),swFromCurrent);
		stream.forward(bnum*(SWMAP_BREADTH_DATA_DESC_LEN+ SWMAP_LAYER_DATA_DESC_LEN* _map->getBreadthLayerNum(zone.index)) );
		_props.breadzones.push_back(zone);
	}
	return true;
}

bool	swMapPersistContainer::getLayers(swMapView* view,const swGeoRect& grc,swUInt8 layerId,
																				OUT std::list<swMapLayer*>& layers){
	//swMapLayer* layer = NULL;
	if( !_props.mbr.isIntersect(grc) ){ //请求地理区域未能与本数据文件MBR相交
		return false;
	}
	//检测图层的网格大小索引	
	swMapLayerProps* props;
	props = view->getMap()->getLayerProps(layerId);
	if(!props){
		return false;
	}
	if(  props->sizeIdx >= _props.breadzones.size()){
		return false;  //层网格规格未能在本地数据文件中找到
	}
	//扫描rect区域内的网格编号
	std::vector<swMapBreadthIdentT> bids;
	_map->getBreadthes(grc,props->sizeIdx,bids);
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
//绘制网格中心点编号，用于调试	
// 	{ // for test render breadth rect
// 		if( layerId == 3){
// 			swGeoSize bsize;
// 			bsize = view->getMap()->getBreadthSize(props->sizeIdx)	;
// 			for(swUInt16 n=0;n< bids.size();n++){
// 				swViewPoint vpt;
// 				swGeoPoint gpt;
// 				swUInt16 x,y;
// 				x = swBreadth_Axis_x(bids[n]);
// 				y = swBreadth_Axis_y(bids[n]);
// 				gpt.x = x*view->getMap()->getBreadthSize(props->sizeIdx).width + view->getMap()->getProps().orgPoint.x;
// 				gpt.y = y*view->getMap()->getBreadthSize(props->sizeIdx).height + view->getMap()->getProps().orgPoint.y;
// 				vpt = view->convertPointGeo2View(gpt);
// 				swString bidname;
// 				bidname = swString::Format(swT("0x%x"),bids[n]);
// 				view->getRender()->getDC()->DrawText(bidname,swDrawPoint(vpt.x,vpt.y));
// 			}
// 		}	
// 	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	int n;
//	swMapBreadthIdentT bid;
	swMapLayer* layer;
	for(n=0;n<bids.size();n++){
		layer = getLayer(view,bids[n],layerId);
		if(layer){
			layer->render(view->getRender());
			//layers.push_back(layer);
			swHeapObjectFactory::instance().freeObject(layer);
		}
	}
	return true;
}


/*
getLayer
在MEMORY_TINY的情况下，无法将feature全部加载之后再进行绘制，而必须读取一个feature绘制一个feature
在一个地图数据存储文件内包含若干个网格存储区段 
(zones)
*/
swMapLayer*	swMapPersistContainer::getLayer(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId){
	swMapLayer * layer = NULL;
	swMemoryTypeT memtype;

#ifdef SW_MEMORY_TINY
	return getLayerTiny(view,breadthId,layerId);
#endif
	
	sw_persist_layer_data_desc_t layer_desc;
	if(!getLayerDesc(breadthId,layerId,layer_desc)){
		return NULL;
	}
	if(layer_desc.size == 0){
		return NULL;  //数据为空
	}
	push_env();
	swMemBlockID mbid;
	swUInt32 size;
	size = getLayerData(layer_desc,mbid,memtype);
	if(size == 0){
		return NULL;
	}
//////////////////////////////////////////////////////////////////////////

		swUInt32 fnum; // 图元数目从内存中读取
///		swMapLayerIdentT id;
		swByteStream	stream((swByte*) swMemHeapEngine::instance()->ptr(mbid),size);
		stream.forward(1);
		stream.readValue(fnum);
		layer = swHeapObjectFactory::instance().createLayer(fnum);
		if( !layer){
			swMemHeapEngine::instance()->free(mbid);
			return NULL;
		}
		stream.rewind();
		stream.readValue(layer->getId());
		stream.readValue(layer->getFeatureNum());
#ifndef _SW_MEMORY_TINY
		//view = NULL; //要求直接绘制feature而不需
#endif
		if(!layer->loadFeatures(stream,view)){
			swHeapObjectFactory::instance().freeObject( (swHeapObject*) layer);
			layer = NULL;
		}
//////////////////////////////////////////////////////////////////////////
	swMemHeapEngine::instance()->free(mbid);

	pop_env();// restore to previous mapping status 
	return layer;
}

/*
	readData()
	从offset位置读取文件数据size字节到data缓冲区
*/
swByte* swMapPersistContainer::readData(const swUInt32 offset,OUT swByte* data,INOUT swUInt32& size){
	swByte* r = NULL;
	_file.Seek(offset);//,swFromCurrent);
	swUInt32 read ;
	read = _file.Read(data,size);
	if( read == size ){
		r = data;
	}
	size = read;
	return r;
}

/*
getLayerData()
定位层数据，如果压缩数据便解压缩然后返回
win32: 采用文件映射 定位到层数据
如果是其他系统没有文件映射机制，则必须 heap堆中分配出一个大的内存区块来存放
$如果是压缩数据，解压出来的数据也要到heap中去请求一块大的内存块来存放

@returns: 返回读取的实际字节数
*/
swUInt32 swMapPersistContainer::getLayerData(const sw_persist_layer_data_desc_t& desc,OUT swMemBlockID& mbid,OUT swMemoryTypeT& mtype){
 	swByte * data = NULL;
 	//swHeapByteStream *stream,* stream1,*stream2 ;
	swUInt32 size;
	swMemBlockID mb1,mb2;
 	//stream1 = swHeapObjectFactory::instance().createByteStream(desc.size); //层数据大小 
	if( !swMemHeapEngine::instance()->malloc(desc.size,mb1) ){
		return 0;
	}
	size = desc.size;
	if( !readData(desc.offset,(swByte*)swMemHeapEngine::instance()->ptr(mb1),size)){//从文件中读层数据到缓冲
		swMemHeapEngine::instance()->free(mb1);
 		return 0;
 	}
	
 	mbid = mb1;
 	if( _map->getProps().compress == 1 ){ /* zlib */
		if( !swMemHeapEngine::instance()->malloc(desc.orgsize,mb2) ){
			swMemHeapEngine::instance()->free(mb1);
			return false;
		}  	 
 		size = desc.orgsize;
		int cret = uncompress( (unsigned char*)swMemHeapEngine::instance()->ptr(mb2), (uLongf*)&size, (unsigned char*)swMemHeapEngine::instance()->ptr(mb1), desc.size);
 		if( cret!=Z_OK){
 			swMemHeapEngine::instance()->free(mb1);
 			swMemHeapEngine::instance()->free(mb2);
 			return 0;
 		}
		swMemHeapEngine::instance()->free(mb1);
 		mbid = mb2;
 	}	
 	//////////////////////////////////////////////////////////////////////////
 	return size;
}

/*
通常情况下从缓冲信息中读取
*/
bool	swMapPersistContainer::getLayerDesc(const swMapBreadthIdentT& breadthId,swUInt8 layerId,OUT sw_persist_layer_data_desc_t& layer){
#ifdef _SW_MEMORY_TINY
	/*每次都从磁盘读取或者从小缓存读取，避免以来过多的内存*/
	return true;
#endif
	/*
	std::map<swMapBreadthIdentT,sw_persist_breadth_desc_t>::iterator itr;
	itr=_props.breadthes.find(breadthId);
	if( itr==_props.breadthes.end()){
		return false; //网格编号不存在
	}
	if( layerId >= itr->second.layers.size()  ){
		return false; // 层编号越界
	}
	layer = itr->second.layers[layerId];
	*/

	//在breadthzone中检索layer的数据位置
	swUInt8 sizeIdx;
	if( breadthId==  swBreadth_NULL){
		return false;
	}
	sizeIdx = swBreadth_SizeIdx(breadthId);
	sw_persist_breadthzone_desc_t* zone;
	zone = &_props.breadzones[sizeIdx];
	//直接跳跃到指定网格内层描述信息位置
	swUInt16 x,y,x1,x2,y1,y2;
	x = swBreadth_Axis_x(breadthId);
	y = swBreadth_Axis_y(breadthId);
	//
	swMapBreadthIdentT lb,rt;
	lb = getBreadthId_lb(sizeIdx);
	if(lb == swBreadth_NULL){
		return false;
	}
	rt = getBreadthId_rt(sizeIdx);
	x1 = swBreadth_Axis_x(lb);
	y1 = swBreadth_Axis_y(lb);
	x2 = swBreadth_Axis_x(rt);
	y2 = swBreadth_Axis_y(rt);
	if(x < x1 || x > x2 || y < y1 || y> y2){
		return false; //请求的网格并不在在当前地图数据文件内
	}
	swUInt16 cols,rows;
	swUInt32 distance;
	cols = x2 - x1 + 1;
	rows = y2 - y1 + 1;
	distance = ( (y-y1)*cols + x - x1) * (_map->getBreadthLayerNum(sizeIdx)*SWMAP_LAYER_DATA_DESC_LEN+SWMAP_BREADTH_DATA_DESC_LEN); // (_map->getLayerNum()*13+13) 表示一个网格描述图层信息的字节总数
	swUInt32 offset;
	swByteStream stream( (swByte*)swMemHeapEngine::instance()->ptr(_mb_zones),_zonelen);
	offset = zone->descoffset + SWMAP_BREADTHZONE_DESC_LEN + distance ; // 网格块描述信息在文件中的偏移
	//find breadth desc start position, n layer defs be arranged together , id as 0,1,2....
	//网格内定义的层编号可以是无序的，所以要根据当前请求的层id进行遍历(这里不做任何容错校验)
	//这个操作将是异常的频繁，对磁盘io的依赖巨大，所以要考虑缓冲
	//offset = offset + SWMAP_BREADTH_DATA_DESC_LEN + SWMAP_LAYER_DATA_DESC_LEN * SWMAP_GET_LAYER_INDEX(layerId);
	swUInt8 id;
	stream.forward(offset+SWMAP_BREADTH_DATA_DESC_LEN);	
	for(swUInt8 n=0;n< _map->getBreadthLayerNum(sizeIdx);n++){ //控制范围，防止溢出
		stream.readValue(id);
		stream.readValue(layer.offset);
		stream.readValue(layer.size);
		stream.readValue(layer.orgsize);
		if( id == layerId){			
			return true;	
		}			
	}
	
// 	_file.Seek(offset); //定位到指定网格块内的层描述信息的头部了
// 	swMemBlockID	 mbid;
// 	if ( !swMemHeapEngine::instance()->malloc(SWMAP_LAYER_DATA_DESC_LEN,mbid) ){
// 		return false;  //请求堆内存失败
// 	}
// 	if( SWMAP_LAYER_DATA_DESC_LEN != _file.Read( swMemHeapEngine::instance()->ptr(mbid),SWMAP_LAYER_DATA_DESC_LEN ) ){
// 		swMemHeapEngine::instance()->free(mbid);
// 		return false;
// 	}
// 	swByteStream stream( (swByte*)swMemHeapEngine::instance()->ptr(mbid),SWMAP_LAYER_DATA_DESC_LEN);
// 	stream.readValue(layer.id);
// 	if(layer.id !=SWMAP_GET_LAYER_INDEX(layerId) ){
// 		//请求和返回的图层编号不匹配
// 		swMemHeapEngine::instance()->free(mbid);
// 		return false;
// 	}
//	swMemHeapEngine::instance()->free(mbid);
	return false;
}

swMapBreadthIdentT	swMapPersistContainer::getBreadthId_lb(swUInt8 sizeIdx){
	return _map->getBreadthMap(swGeoPoint(_props.mbr.x,_props.mbr.y),sizeIdx);
}

swMapBreadthIdentT	swMapPersistContainer::getBreadthId_rt(swUInt8 sizeIdx){
	return _map->getBreadthMap(swGeoPoint(_props.mbr.x + _props.mbr.width,_props.mbr.y+_props.mbr.height),sizeIdx);
}												 

swMapLayer*	swMapPersistContainer::getLayerTiny(swMapView* view,const swMapBreadthIdentT& breadthId,swUInt8 layerId){
	//view->getRender()->drawFeature()
	return NULL;
}

sw_persist_layer_data_desc_t* swMapPersistContainer::getLayerPersistInfo(const swMapBreadthIdentT& breadthId,swUInt8 layerId){
	return NULL;
}

bool		swMapPersistContainer::getByteStream(OUT swByteStream&,const swMapBreadthIdentT& breadthId,swUInt8 layerId){
	return true;
}

void swMapPersistContainer::push_env(){

}

void swMapPersistContainer::pop_env(){

}
