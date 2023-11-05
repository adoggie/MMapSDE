#include "map.h"
#include "memheap/memheap.h"
#include "memheap/memdump.h"
/*
@func: create()
打开地图文件
@params:
	mapfile - 地图数据文件全路径
*/
bool	swMap::create(const swString& mapfile){
	if( !_file.Open(mapfile) ){
		return false;
	}
	_mapDir = swFileName(mapfile).GetPath();
	swUInt32 size;
	swMemBlockID mbid;
	size = _file.Length();
	swByte* buff = (swByte*)swMemHeapEngine::instance()->malloc(size,mbid);
	if( !buff){
		return false;
	}
	_file.Read(buff,size);
	swByteStream stream(buff,size);
	_file.Close();
	if( memcmp(buff,"swMapFile",9) !=0 ){
		swMemHeapEngine::instance()->free(mbid);
		return false;
	}
	stream.forward(24);
	stream.readValue(_props.id);
	stream.readString(_props.name);
	swUInt8 sizeu8;

	stream.readValue(sizeu8);
	//map_layer_draw_property_ t
	for(swUInt8 n=0;n<sizeu8;n++){
		swMapLayerDrawPropT dp;
		stream.readValue(dp.penColor);
		stream.readValue(dp.penWidth);
		stream.readValue(dp.penStyle);
		stream.readValue(dp.brushColor);
		stream.readValue(dp.brushStyle);
		stream.readString(dp.fontName);
		stream.readValue(dp.fontSize);
		stream.readValue(dp.fontColor);
		stream.readValue(dp.fontStyle);
		_props.layerDrawProps.push_back(dp);
	}
	//地图层描述信息列表 ,map_layer_desc_t
	stream.readValue(sizeu8);
	for(swUInt8 n=0;n<sizeu8;n++){
		swMapLayerProps prop;
		stream.readValue(prop.id);
		stream.readValue(prop.visibleMask);
		stream.readValue(prop.annoMask);
		stream.readValue(prop.geomType);
		for(swUInt8 s=0;s<SWMAP_SCALELEVEL_NUM;s++){ //读取层在每一个缩放级别定义的绘制属性编号
			swUInt8 idx;
			stream.readValue(idx);
			prop.drawprops.push_back(_props.layerDrawProps[idx] );			
		}
		stream.readValue(prop.sizeIdx);
		_props.layerProps.push_back(prop);
	}
	//地图网格规格数量
	stream.readValue(sizeu8);
	for(swUInt8 n=0;n<sizeu8;n++){
		sw_breadthsize_desc_t	bsize;
		
		stream.readValue(bsize.index);
		stream.readValue(bsize.size);
		stream.readValue(bsize.layernum);
		_props.breadthsizes.push_back(bsize);
	}
	//压缩信息
	stream.readValue(_props.compress);	
	stream.readValue(_props.orgPoint);
	//////////////////////////////////////////////////////////////////////////
	//读缩放级别
	stream.readValue(sizeu8);
	if( sizeu8 !=SWMAP_SCALELEVEL_NUM){
		return false;
	}
	for(swUInt8 n=0;n<sizeu8;n++){
		stream.readValue(_props.scaleUnits[n]);
	}

	//////////////////////////////////////////////////////////////////////////
	stream.readValue(sizeu8);
	swMapPersistContainer *pc;
	swMapPersistContainerListT datafiles;
	for(swUInt8 n=0;n<sizeu8;n++){ //读地图数据文件信息
		pc =new swMapPersistContainer(this);
		swUInt32 id;
		swString name,file;
		stream.readValue(id);
		stream.readString(name);
		stream.readString(file);		
		if( pc->create(file)){
			_persists.push_back(pc);
		}else{
			delete pc;
		}
	}
	//地图层渲染属性列表
	swMemHeapEngine::instance()->free(mbid);

	
	return true;
}
// 
// shared_ptr<swMapLayerDataSet>		swMap::createLayerDataset(swMapView* ) {
// 	return shared_ptr<swMapLayerDataSet>(new swMapLayerDataSet);
// }

/*
	@func: getBreadthMap
		据坐标和网格块大小来计算当前点处于哪一个网格区域，返回pt落入的网格编号
	@param:
		pt - 经纬度坐标点
		sizeIdx - 图块网格规格编号
*/
swMapBreadthIdentT	swMap::getBreadthMap(const swGeoPoint& pt,swUInt8 sizeIdx /* sizeidx=0*/){
	swGeoSize breadthsize;
	breadthsize = getBreadthSize(sizeIdx);
	int xnum,ynum ;
	float offx,offy;
	offx = pt.x - _props.orgPoint.x;
	offy = pt.y - _props.orgPoint.y;
	if( offx <= breadthsize.width/2.0  || offy <= breadthsize.height/2.0){
		return swBreadth_NULL;		
	}
	xnum =(int) ((pt.x - _props.orgPoint.x)/ breadthsize.width);
	ynum =(int)((pt.y - _props.orgPoint.y)/breadthsize.height);
	float xlen,ylen;
	xlen = fabs(pt.x - _props.orgPoint.x) - fabs(xnum * breadthsize.width);
	ylen = fabs(pt.y - _props.orgPoint.y) - fabs(ynum * breadthsize.height);
	if( xlen > breadthsize.width/2.0){
		xnum ++; //网格编号增1
	}
	if( ylen > breadthsize.height/2.0){
		ynum ++; 
	}
	if( xnum >= 0xfff || ynum >=0xfff){
		return	 swBreadth_NULL;
	}
	return swMakeBreadthId2(xnum,ynum,sizeIdx);
}

swGeoSize	swMap::getBreadthSize(swUInt8 idx){
	if( idx >= _props.breadthsizes.size()){
		idx = 0;
	}
	return _props.breadthsizes[idx].size;
}

swUInt8		swMap::getBreadthLayerNum(swUInt8 sizeIdx){
	if( sizeIdx >= _props.breadthsizes.size()){
		sizeIdx = 0;
	}
	return _props.breadthsizes[sizeIdx].layernum;
}

/*
getBreadthes
计算地理显示区域内的网格图块
*/
bool swMap::getBreadthes(const swGeoRect& rc,swUInt8 sizeIdx,OUT std::vector<swMapBreadthIdentT>& ids){
	swGeoPoint lt,rb;
	ids.clear();
	lt.x = rc.x;
	lt.y = rc.y;
	rb.x = rc.x+rc.width;
	rb.y = rc.y + rc.height;
	swMapBreadthIdentT b1,b2;
	b1 =getBreadthMap(lt);
	if(b1 == swBreadth_NULL){
		b1 = swMakeBreadthId2(0,0,sizeIdx);
	}
	b2 = getBreadthMap(rb);
	int x1,y1,x2,y2;
	x1 = swBreadth_Axis_x( b1); y1 = swBreadth_Axis_y(b1);
	x2 = swBreadth_Axis_x( b2); y2 = swBreadth_Axis_y(b2);
	int c,r;
	for(c=x1;c<=x2;c++){
		for(r=y1;r<=y2;r++){
			swMapBreadthIdentT bid =swMakeBreadthId2(c,r,sizeIdx);
			ids.push_back(bid);
		}
	}
	return true;
}


/*
汇聚了一个省市的图层集合之后进行绘制
如果要进一步的节省空间的话，将绘制操作推进到每一个网格，即加载每一个网格图层之后立刻进行绘制并马上释放堆上内存
*/
void swMap::render(swMapView* view,const swGeoRect grc,const swMapLayerIdentT& layerId){
	swUInt16 n;
	swMapPersistContainer* cont;
	std::list<swMapLayer*> layers;
	for(n=0;n<_persists.size();n++){
		cont = _persists[n];
		layers.clear();
		if( cont->getLayers(view,grc,layerId,layers) ){
// 			std::list<swMapLayer*>::iterator itr;
// 			for(itr = layers.begin();itr!= layers.end();itr++){
// 				(*itr)->render(view->getRender());
// 				swHeapObjectFactory::instance().freeObject(*itr); //马上释放内存
// 			}
		}
	}
}

swMapProps&		swMap::getProps(){
	return _props;
}

//计算网格的中心地理点坐标
bool	swMap::getBreadthRect(const swMapBreadthIdentT& bid,OUT swGeoRect& rc){
	swGeoPoint gpt;
	swUInt16 x,y;
//	swUInt16 ox,oy;
	swUInt8 sizeIdx;
	x = swBreadth_Axis_x(bid);
	y = swBreadth_Axis_y(bid);
	sizeIdx = swBreadth_SizeIdx(bid);
	swGeoSize breadthsize;
	breadthsize = getBreadthSize(sizeIdx);
	gpt.x = _props.orgPoint.x + (x)*breadthsize.width;
	gpt.y = _props.orgPoint.y + (y)*breadthsize.height;
	rc.width = breadthsize.width;
	rc.height = breadthsize.height;
	rc.x = gpt.x -  rc.width/2.0;
	rc.y = gpt.y - rc.height/2.0;
	return true;
}

swMap::swMap(){

}

swMap::~swMap(){
	swMapPersistContainerListT::iterator itr;
	for(itr=_persists.begin();itr!=_persists.end();itr++){
		swMapPersistContainer * pc;
		pc = *itr;
		delete pc;		
	}
}

swMapLayerProps* swMap::getLayerProps(const swMapLayerIdentT& id){
	swMapLayerProps* r = NULL;
	if( id<_props.layerProps.size()){
		r = &_props.layerProps[id];
	}
	return r;
}

swUInt8 swMap::getLayerNum(){
	return (swUInt8) _props.layerProps.size();
}

swString swMap::getMapDirectory(){
	return _mapDir;
}