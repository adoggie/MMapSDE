#include "layer_mgr.h"
#include "view.h"
#include "map.h"

swMapLayerDataSet::swMapLayerDataSet(swMapView * view){
	_view = view;
}

// shared_ptr<swRenderOutput> 		swMapLayerDataSet::render(){
// }

/*
��ȡ������ͼ�����������ȫ�����뵱ǰ��ͼ�����������ͼ�����ཻ�Ļ��㻺��������ص�layer
*/
// bool	swMapLayerDataSet::getLayers(std::vector<swMapLayer*>& layers,const swMapBreadthIdentT& breadId,
// 																											 const swMapLayerIdentT& layerId){
// 	layers.clear();
// #ifndef _SW_MEMORY_TINY
// 	//1.��⻺��
// 	int n;
// 	for(n=0;n<_layers_cached.size();n++){
// 		swMapLayerStoreIdentT sid;
// 		sid = _layers_cached[n]->getSotredIdent();
// 		if( sid.breadth == breadId && sid.layer ==  layerId){
// 			layers.push_back(_layers_cached[n]);
// 		}
// 	}
// 	//һ������㻺��һ�������ͬһͼ������з�ɢ�洢��layers
// 	if(layers.size()){
// 		return true;
// 	}
//#endif
// 	//û�ҵ�������������ļ���ȥ��ȡ
// 	layers = _view->getMap()->getLayers(_view,breadId,layerId);
// #ifndef _SW_MEMORY_TINY
// 	//��⵱ǰ�����Ƿ���ȫ�����������������������л���
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
��װͼ������
1.�ӻ��������ȥ����
2.δ���ֻ������ȡ��ͼ�ļ�����
3.ͼ�������ȫ�����ڵ�ǰ�Ŀ��ӵ���������ʱ�����ǽ���ͼ����л���
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
		if( n<=12 && viewPrps.layerprops[n].visibleMask & (1<<scale) ){ //�˲��ڵ�ǰ���ű����¿ɼ�
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
