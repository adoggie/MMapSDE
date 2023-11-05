#include "layer_mgr.h"
#include "view.h"
#include "map.h"

swMapLayerDataSet::swMapLayerDataSet(swMapView * view){
	_view = view;
}

// shared_ptr<swRenderOutput> 		swMapLayerDataSet::render(){
// }

/*
读取出来的图层数据如果完全存在与当前视图区域而不与视图区域相交的话便缓存这个加载的layer
*/
// bool	swMapLayerDataSet::getLayers(std::vector<swMapLayer*>& layers,const swMapBreadthIdentT& breadId,
// 																											 const swMapLayerIdentT& layerId){
// 	layers.clear();
// #ifndef _SW_MEMORY_TINY
// 	//1.检测缓冲
// 	int n;
// 	for(n=0;n<_layers_cached.size();n++){
// 		swMapLayerStoreIdentT sid;
// 		sid = _layers_cached[n]->getSotredIdent();
// 		if( sid.breadth == breadId && sid.layer ==  layerId){
// 			layers.push_back(_layers_cached[n]);
// 		}
// 	}
// 	//一旦缓冲便缓冲一个网格的同一图层的所有分散存储的layers
// 	if(layers.size()){
// 		return true;
// 	}
//#endif
// 	//没找到缓冲的则到数据文件中去读取
// 	layers = _view->getMap()->getLayers(_view,breadId,layerId);
// #ifndef _SW_MEMORY_TINY
// 	//检测当前网格是否完全落入地理可视区域，如果是则进行缓存
// 	swGeoRect grc;
// 	_view->getMap()->getBreadthRect(breadId,grc);
// 	if( grc.isWithin(_view->getGeoRect())){
// 		for(n=0;n<layers.size();n++){
// 			_layers_cached.push_back(layers[n]);
// 		}		
// 	}
// #endif
// 	return true;
// }
/*
组装图层数据
1.从缓冲队列中去检索
2.未发现缓冲则读取地图文件数据
3.图层如果完全包含在当前的可视地理区域内时，考虑将此图层进行缓存
*/
bool		swMapLayerDataSet::assemble(){
	swGeoRect grc = _view->getGeoRect();
	swMapViewProps&	 viewPrps = _view->getProps();
	swUInt8 n;
	swUInt8 scale;
	scale = _view->getScaleLevel();
	//std::vector<swMapBreadthIdentT> bids;

	//std::vector<swMapLayer*> layers;
	for(n=0;n< _view->getMap()->getLayerNum();n++){
		if( n<=12 && viewPrps.layerprops[n].visibleMask & (1<<scale) ){ //此层在当前缩放比例下可见
			_view->getRender()->beginLayer(n);
			_view->getMap()->render(_view,_view->getGeoRect(),n);
			_view->getRender()->endLayer(n);
		} 
	}
	return true;
}

// bool		swMapLayerDataSet::setDataCachedType(eDataCachedType dct){
// 	_dct = dct;
// 	return true;
// }


swMapLayerDataSet::~swMapLayerDataSet(){
// 	int n;
// 	for(n=0;n<_layers_cached.size();n++){
// 		delete _layers_cached[n];
// 	}
}
