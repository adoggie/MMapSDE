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

/*��ͼ���ݴ洢�ļ�
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
	//�����������
	swUInt8 zoneNum;
	_file.Seek(_props.jmpOffset);
	_file.Read(&zoneNum,1);
	_zonelen = _file.Length() - _props.jmpOffset;
	if ( !swMemHeapEngine::instance()->malloc(_zonelen,_mb_zones) ){
		return false;  //������ڴ�ʧ��
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
	if( !_props.mbr.isIntersect(grc) ){ //�����������δ���뱾�����ļ�MBR�ཻ
		return false;
	}
	//���ͼ��������С����	
	swMapLayerProps* props;
	props = view->getMap()->getLayerProps(layerId);
	if(!props){
		return false;
	}
	if(  props->sizeIdx >= _props.breadzones.size()){
		return false;  //��������δ���ڱ��������ļ����ҵ�
	}
	//ɨ��rect�����ڵ�������
	std::vector<swMapBreadthIdentT> bids;
	_map->getBreadthes(grc,props->sizeIdx,bids);
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
//�����������ĵ��ţ����ڵ���	
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
��MEMORY_TINY������£��޷���featureȫ������֮���ٽ��л��ƣ��������ȡһ��feature����һ��feature
��һ����ͼ���ݴ洢�ļ��ڰ������ɸ�����洢���� 
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
		return NULL;  //����Ϊ��
	}
	push_env();
	swMemBlockID mbid;
	swUInt32 size;
	size = getLayerData(layer_desc,mbid,memtype);
	if(size == 0){
		return NULL;
	}
//////////////////////////////////////////////////////////////////////////

		swUInt32 fnum; // ͼԪ��Ŀ���ڴ��ж�ȡ
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
		//view = NULL; //Ҫ��ֱ�ӻ���feature������
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
	��offsetλ�ö�ȡ�ļ�����size�ֽڵ�data������
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
��λ�����ݣ����ѹ�����ݱ��ѹ��Ȼ�󷵻�
win32: �����ļ�ӳ�� ��λ��������
���������ϵͳû���ļ�ӳ����ƣ������ heap���з����һ������ڴ����������
$�����ѹ�����ݣ���ѹ����������ҲҪ��heap��ȥ����һ�����ڴ�������

@returns: ���ض�ȡ��ʵ���ֽ���
*/
swUInt32 swMapPersistContainer::getLayerData(const sw_persist_layer_data_desc_t& desc,OUT swMemBlockID& mbid,OUT swMemoryTypeT& mtype){
 	swByte * data = NULL;
 	//swHeapByteStream *stream,* stream1,*stream2 ;
	swUInt32 size;
	swMemBlockID mb1,mb2;
 	//stream1 = swHeapObjectFactory::instance().createByteStream(desc.size); //�����ݴ�С 
	if( !swMemHeapEngine::instance()->malloc(desc.size,mb1) ){
		return 0;
	}
	size = desc.size;
	if( !readData(desc.offset,(swByte*)swMemHeapEngine::instance()->ptr(mb1),size)){//���ļ��ж������ݵ�����
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
ͨ������´ӻ�����Ϣ�ж�ȡ
*/
bool	swMapPersistContainer::getLayerDesc(const swMapBreadthIdentT& breadthId,swUInt8 layerId,OUT sw_persist_layer_data_desc_t& layer){
#ifdef _SW_MEMORY_TINY
	/*ÿ�ζ��Ӵ��̶�ȡ���ߴ�С�����ȡ����������������ڴ�*/
	return true;
#endif
	/*
	std::map<swMapBreadthIdentT,sw_persist_breadth_desc_t>::iterator itr;
	itr=_props.breadthes.find(breadthId);
	if( itr==_props.breadthes.end()){
		return false; //�����Ų�����
	}
	if( layerId >= itr->second.layers.size()  ){
		return false; // ����Խ��
	}
	layer = itr->second.layers[layerId];
	*/

	//��breadthzone�м���layer������λ��
	swUInt8 sizeIdx;
	if( breadthId==  swBreadth_NULL){
		return false;
	}
	sizeIdx = swBreadth_SizeIdx(breadthId);
	sw_persist_breadthzone_desc_t* zone;
	zone = &_props.breadzones[sizeIdx];
	//ֱ����Ծ��ָ�������ڲ�������Ϣλ��
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
		return false; //��������񲢲����ڵ�ǰ��ͼ�����ļ���
	}
	swUInt16 cols,rows;
	swUInt32 distance;
	cols = x2 - x1 + 1;
	rows = y2 - y1 + 1;
	distance = ( (y-y1)*cols + x - x1) * (_map->getBreadthLayerNum(sizeIdx)*SWMAP_LAYER_DATA_DESC_LEN+SWMAP_BREADTH_DATA_DESC_LEN); // (_map->getLayerNum()*13+13) ��ʾһ����������ͼ����Ϣ���ֽ�����
	swUInt32 offset;
	swByteStream stream( (swByte*)swMemHeapEngine::instance()->ptr(_mb_zones),_zonelen);
	offset = zone->descoffset + SWMAP_BREADTHZONE_DESC_LEN + distance ; // �����������Ϣ���ļ��е�ƫ��
	//find breadth desc start position, n layer defs be arranged together , id as 0,1,2....
	//�����ڶ���Ĳ��ſ���������ģ�����Ҫ���ݵ�ǰ����Ĳ�id���б���(���ﲻ���κ��ݴ�У��)
	//������������쳣��Ƶ�����Դ���io�������޴�����Ҫ���ǻ���
	//offset = offset + SWMAP_BREADTH_DATA_DESC_LEN + SWMAP_LAYER_DATA_DESC_LEN * SWMAP_GET_LAYER_INDEX(layerId);
	swUInt8 id;
	stream.forward(offset+SWMAP_BREADTH_DATA_DESC_LEN);	
	for(swUInt8 n=0;n< _map->getBreadthLayerNum(sizeIdx);n++){ //���Ʒ�Χ����ֹ���
		stream.readValue(id);
		stream.readValue(layer.offset);
		stream.readValue(layer.size);
		stream.readValue(layer.orgsize);
		if( id == layerId){			
			return true;	
		}			
	}
	
// 	_file.Seek(offset); //��λ��ָ��������ڵĲ�������Ϣ��ͷ����
// 	swMemBlockID	 mbid;
// 	if ( !swMemHeapEngine::instance()->malloc(SWMAP_LAYER_DATA_DESC_LEN,mbid) ){
// 		return false;  //������ڴ�ʧ��
// 	}
// 	if( SWMAP_LAYER_DATA_DESC_LEN != _file.Read( swMemHeapEngine::instance()->ptr(mbid),SWMAP_LAYER_DATA_DESC_LEN ) ){
// 		swMemHeapEngine::instance()->free(mbid);
// 		return false;
// 	}
// 	swByteStream stream( (swByte*)swMemHeapEngine::instance()->ptr(mbid),SWMAP_LAYER_DATA_DESC_LEN);
// 	stream.readValue(layer.id);
// 	if(layer.id !=SWMAP_GET_LAYER_INDEX(layerId) ){
// 		//����ͷ��ص�ͼ���Ų�ƥ��
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
