
#include "view.h"
#include "geo.h"
#include "map.h"
#include "layer_mgr.h"
#include "server.h"

swMapView::swMapView(swMapServer* server){
	_render = NULL;
	_layerDataset = NULL;
	_server = server;
}

swMapView::~swMapView(){
	if(_render){
		delete _render;
	}	
	if(_layerDataset){
		delete _layerDataset;
	}
}

swMapLayerProps*	swMapView::getLayerProps(swUInt8 layerId){
	if( layerId>=_props.layerprops.size()){
		return NULL;
	}
	return &_props.layerprops[layerId];
}

/*
设置可视窗体区域之后，根据当前缩放级别自动调整可见地理区域
*/
void	swMapView::setViewRect(const swViewRect& rc){
	//if(_props.viewRc == rc){
	//	return;
	//}
	_props.viewRc = rc;
	float geow,geoh;
	geow = (rc.width * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //计算结果为可视窗体宽度=多少地理度单位
	geoh = (rc.height * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //计算结果为可视窗体宽度=多少地理度单位
	_props.geoRc.x = _props.centerXY.x - geow/2.0;
	_props.geoRc.y = _props.centerXY.y - geoh/2.0;
	_props.geoRc.width = geow;
	_props.geoRc.height = geoh;
	//////////////////////////////////////////////////////////////////////////
	//调整内部缓冲 
 	if( _render){
 		_render->sizeChanged(swViewSize(rc.width,rc.height));
 	}
}

swViewRect	swMapView::getViewRect(){
	return _props.viewRc;
}

/*
设置地理中心点位置之后根据当前显示比例尺自动调整可视地理区域,但不调整可视窗体的大小
*/
void				swMapView::setCenterPoint(const swGeoPoint& point){
	_props.geoRc.x += point.x - _props.centerXY.x;
	_props.geoRc.y += point.y - _props.centerXY.y;
	_props.centerXY =point;
}

swGeoPoint	swMapView::getCenterPoint(){
	return _props.centerXY;
}

//设置视图点的地理坐标为中心点
void				swMapView::setCenterPoint(const swViewPoint& point){
	swGeoPoint  gpt;
	gpt = convertPointView2Geo(point);
	setCenterPoint(gpt);
}

swViewPoint	swMapView::getViewCenterPoint(){
	return swViewPoint( _props.viewRc.x+_props.viewRc.width/2,_props.viewRc.y + _props.viewRc.height/2);
}

void				swMapView::setScaleLevel(swUInt8 scale){
	if( scale >= SWMAP_SCALELEVEL_NUM ){
		return;
	}
	if( _props.scale == scale){
		return;
	}
	_props.scale = scale;
	//当前地理中心点不变，调整地理区域大小
	float geow,geoh;
	swViewRect &rc = _props.viewRc;
	
	geow = (rc.width * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //计算结果为可视窗体宽度=多少地理度单位
	geoh = (rc.height * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //计算结果为可视窗体宽度=多少地理度单位
	_props.geoRc.x = _props.centerXY.x - geow/2.0;
	_props.geoRc.y = _props.centerXY.y - geoh/2.0;
	_props.geoRc.width = geow;
	_props.geoRc.height = geoh;
}

swUInt8			swMapView::getScaleLevel(){
	return _props.scale;
}

void				swMapView::setScaleLevelTable(const swFloat* units){
	memcpy(_props.scaleUnits,units,SWMAP_SCALELEVEL_NUM);
}

swFloat*	swMapView::getScaleLevelTable(){
	return _props.scaleUnits;
}

/*
*/
swGeoRect		swMapView::getGeoRect(){
	return _props.geoRc;
}

/*
swRenderOutput*			swMapView::getOutput(){
	return _render->getOutput();
}
*/
/*
update
读取地图数据并进行绘制
*/
void				swMapView::update(){
	//如果显示级别和显示地理区域没有变化或者变化极小的话就无需从新加载数据,直接返回
	_layerDataset->assemble();
	//_layerDataset->render();
}

swMapViewProps&	swMapView::getProps(){
	return _props;
}

// void				swMapView::setRender(shared_ptr<swMapRender>& r){
// 	_render = r;
// }

swMapRender*	swMapView::getRender(){
	return _render;
}
 

swMap*	swMapView::getMap(){
	return _map;
}

void	swMapView::setMap(swMap* m){
	_map = m;
}

swMapLayerDataSet* swMapView::getDataset(){
	return _layerDataset;
}

/*
计算与中心点的偏差
*/
swGeoPoint	swMapView::convertPointView2Geo(const swViewPoint& pt){
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
swViewPoint	swMapView::convertPointGeo2View(const swGeoPoint& pt){
	swGeoPoint cxy = _props.centerXY;
	swViewPoint r = getViewCenterPoint();
	r.x += (pt.x-cxy.x)*sw_METERS_PER_DEGREE/(getCurScaleUnit()*1.0);
	r.y += -(pt.y-cxy.y)*sw_METERS_PER_DEGREE/(getCurScaleUnit()*1.0); //两个坐标方向相反
	return r;
}

swFloat		swMapView::getCurScaleUnit(){
	return _props.scaleUnits[_props.scale];
}

bool	swMapView::create(swMap* map){
	// duplicate layer's properties from map
	_props.layerprops = map->getProps().layerProps;
	memcpy(_props.scaleUnits,map->getProps().scaleUnits,sizeof(map->getProps().scaleUnits));
	//_props.scaleUnits = map->getProps().scaleUnits;
	setMap(map);
	_render = new swMapRender(this);
	_layerDataset = new swMapLayerDataSet(this);
	
	#if SW_RPC_ICE_ENABLE
	_hbTime	= (int)time(0);
	#endif
	return true;
}

bool			swMapView::isAnnoVisible(swUInt8  layerId){
	return ( getLayerProps(layerId)->annoMask & (1<<_props.scale) ) == 1;
}

swMapLayerDrawPropT* swMapView::getLayerDrawProp(swUInt8 layerId,swUInt8 scale/*=CURRENT_SCALE*/){
	if( scale ==CURRENT_SCALE ){
		scale = _props.scale;
	}
	return &getLayerProps(layerId)->drawprops[scale];
}

void	swMapView::setLayerDrawProp(swUInt8 layerId,swUInt8 scale,const swMapLayerDrawPropT& drawprop){
	getLayerProps(layerId)->drawprops[scale] = drawprop;
}

 //请求网格块渲染
// 小网格切分时碰到大面积无效区域是将导致加载重复的网格数据块，所以这些网格数据块必须要进行缓冲
//否则当大面积无效视窗区域产生时，必须更改请求块大小
bool	swMapView::getBitmapTiles(swUInt8 scale,swUInt16 tileWidth,swUInt16 tileHeight,const swViewTileRectListT rects,OUT swMapBitmapTileListT& tiles){
	tiles.clear();
	_props.scale = scale;
	swViewTileRectListT::const_iterator itr;
	for(itr=rects.begin();itr!=rects.end();itr++){ //依次请求不同的数据块
		_props.centerXY = itr->gcpt;
		swViewRect rect;
		rect.x= 0; rect.y = 0;
		rect.width = tileWidth; rect.height= tileHeight;
		setViewRect(rect);

		swBrush br;
		br.SetColour(153,179,204);
		
		getRender()->getDC()->SetBrush(br);
		swPen  pen;
		pen.SetWidth(0);
		pen.SetColour(swColor(153,179,204));
		getRender()->getDC()->SetPen(pen);
		getRender()->getDC()->DrawRectangle(wxRect(0,0,tileWidth,tileHeight));
		swViewPoint vcpt;
		vcpt = convertPointGeo2View(_props.centerXY);		
		getRender()->rotate(itr->rotate,vcpt);
		
		update();
		swMapBitmapTileT bitmap;
		bitmap.idx = itr->idx;
		itr->destdc->Blit(itr->destpt.x,itr->destpt.y,tileWidth,tileHeight,getRender()->getDC(),0,0);
		tiles.push_back(bitmap);
	}
	return true;
}



//////////////////////////////////////////////////////////////////////////
#if SW_RPC_ICE_ENABLE
///- RPC- ///////////////////////////////////////////////////////////////////
swmap::MapBitmapTileListT	swMapView::getBitmapTiles(Ice::Int scale,Ice::Int tileWidth,Ice::Int tileHeight,const swmap::MapViewTileRectListT& rects,const Ice::Current&  ctx){
	swmap::MapBitmapTileListT tiles;
	swViewTileRectListT rects2;
	swMapBitmapTileListT tiles2;
	swmap::MapViewTileRectListT::const_iterator itr;
	for(itr=rects.begin();itr!=rects.end();itr++){
		swBitmap bitmap;
		swMemoryDC memdc;
		bitmap.Create(tileWidth,tileHeight);
		memdc.SelectObject(bitmap);
		 swViewTileRectT trc;
		 trc.gcpt.x = itr->gcpt.x;
		 trc.gcpt.y = itr->gcpt.y;
		 trc.idx.x = itr->idx.x;
		 trc.idx.y = itr->idx.y;
		 trc.destdc = &memdc;
		 trc.destpt.x = 0;
		 trc.destpt.y = 0;
		 trc.rotate = itr->rotate;
		 trc.xscale = itr->xscale;
		 trc.yscale = itr->yscale;
		 trc.translate = itr->translate;
		 rects2.clear();
		 rects2.push_back(trc);
		 tiles2.clear();
		getBitmapTiles(scale,tileWidth,tileHeight,rects2,tiles2);
		{			
			 swmap::MapBitmapTileT btile;			 
			 btile.width = tileWidth;
			 btile.height = tileHeight;
			 btile.idx.x = itr->idx.x;
			 btile.idx.y = itr->idx.y;
			 swImage	image;//(bitmap);
			 swUInt32 size;
			 image = bitmap.ConvertToImage();
			 size = image.GetWidth()*image.GetHeight()* bitmap.GetDepth()/8;
			 btile.bitmap.assign(image.GetData(),image.GetData()+size);		
			 tiles.push_back(btile);	 
		}	 		 
	}			
	return tiles;
}

swmap::MapScalePrecisionT	swMapView::getScalePrecision(const Ice::Current&  ctx){
	swmap::MapScalePrecisionT scales;
	swFloat*		p = getScaleLevelTable();
	scales.s0 = p[0];
	scales.s1 = p[1];
	scales.s2 = p[2];
	scales.s3 = p[3];
	scales.s4 = p[4];
	scales.s5 = p[5];
	scales.s6 = p[6];
	scales.s7 = p[7];
	scales.s8 = p[8];
	scales.s9 = p[9];
	scales.s10 = p[10];
	scales.s11 = p[11];
	scales.s12 = p[12];
	scales.s13 = p[13];
	scales.s14 = p[14];
	scales.s15 = p[15];	
	return scales;
}

void 		swMapView::setScalePrecision(const swmap::MapScalePrecisionT& scales,const Ice::Current&  ctx){
	setScaleLevelTable( (swFloat*)&scales);
}

std::string  swMapView::getId(const Ice::Current&  ctx){	 
	return _iceId.name;
}

void	swMapView::heartBeat(const Ice::Current&  ctx){
	_hbTime = (int)time(0);
}

Ice::Identity swMapView::getId(){
	return _iceId;
}

bool	swMapView::isInvalid(){
	if( (time(0) - _hbTime) > _server->getProps().viewEvictTime){
		return true;
	}
	return false;
}

void	swMapView::setId(const Ice::Identity& id){
	_iceId = id;
}
#endif


