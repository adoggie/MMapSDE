
#ifndef _swlayer_h
#define _swlayer_h

#include "base.h"
#include "geo.h"
#include "feature.h"
#include "memheap/heap.h"


class swMapPersistContainer;

struct sw_persist_layer_data_desc_t{
	swUInt8		id;
	//swGeometryTypeT		geomType;	// points or lines or polygons ���ζ������� (GeometryTypeT)
	//swUInt8		compress;	// 0-non compress,1-zlib
	swUInt32	offset;
	swUInt32	size;			//�洢�����ݴ�С
	swUInt32	orgsize;	//δѹ��ǰ���ݴ�С
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

//swLayerStoreIdentT Ŀǰ������Ϊͼ�����ݻ���ʱ���õ�ͼ���ʶ
struct swMapLayerStoreIdentT{
	swMapPersistContainerIdentT persist;	//�洢������
	swMapBreadthIdentT		breadth;				//����ͼ
	swMapLayerIdentT			layer;					//��
};
/*
Ϊ��ʵ������ͼ�㴢����ͬһ�ݵ�ͼ�ṹ�ж��������ͼ�����ݽṹ��ʹ�������ͼ�
*/
struct swMapLayerProps{
	swUInt8			id; //	����		
	swUInt16		visibleMask	; // 0~15λ
	swUInt16		annoMask;	//��ע�Ƿ�ɼ� ����·��poi�������ı���ע��
	swUInt8			geomType;	//���ݶ�������
	swUInt8			sizeIdx;	//��������������� 0 - 
	//swUInt8			drawprops[SWMAP_SCALELEVEL_NUM];	//����ÿ�����ż�������ʾ����
	std::vector<swMapLayerDrawPropT> drawprops;	//����ÿ�����ż�������ʾ����
};

/*
swMapLayer ͼ������������޵ģ�ͼ���ڹ�����ͼԪ�����Ǳ����Ŵ洢���ڴ�ѿռ��ϣ��Ա���ٷ���

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

	bool	loadFeatures(swByteStream& stream,swMapView* view = NULL);		//����ͼԪ����
	void	clearFeatures();	//���ͼԪ����
	
	void	destroy();
	void	render(swMapRender* r);

	
private:
	bool loadPoi(swByteStream& stream);
	bool loadRoad(swByteStream& stream);
	bool loadPolygon(swByteStream& stream);
private:
	swMapLayerIdentT			_id;					//����
	swMapBreadthIdentT		_bid;					//������
	swUInt8								_geomType;		//������������

	swUInt32						_featureNum;			//ͼԪ��������
	swUInt32						_ftrcnt;
	swGeoObject** 			_features;				//featuresָ��ͼԪ���ݵ�ָ���ַ���ϵ������ڴ��ռ�
	//ͼԪ���ݵ�ָ�벢���Ǵ���ͼ������ͬһ���ڴ��ַ�ռ�
	//��ϵͳ�����й����У�ͼ���������ı��������ݻ٣�����ͼ��ļ����ٶȻ�Ӱ��ϵͳ������Ч�ʣ����Կ��ǽ�ͼ�������Ƴ��ڲ�Ѷ���
	//��ͬ��ͼԪ�洢�ڷָ����ڴ�ѿ��ڣ���Щ�ڴ��ĵ�ַ�洢��layer������ڴ�ѿ�ĺ󲿣�������layer���ݽṹ֮��
};

inline 
swMapLayer::swMapLayer(swUInt32 featureNum){
	_featureNum = featureNum;
	_ftrcnt = 0;
}

typedef std::vector<swMapLayer*>	swMapLayerListT;





#endif