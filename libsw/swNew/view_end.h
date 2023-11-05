/*
view_end.h
�ͻ�����ͼ����ʵ��
�������ͼ�ο飬֧�ֿ��ٸ��µ�ͼ��֧�ֲַ�ʽ�ܹ�
*/


#ifndef _SW_VIEW_END_H
#define _SW_VIEW_END_H

#include "base.h"
#include "object.h"
#include "render.h"
#include "layer.h"
#include "layer_mgr.h"
#include "view.h"
#include "tile.h"

#if SW_RPC_ICE_CLIENT
class swMapRpcClient;
#endif
 
class swMapViewEnd{
public:
	swMapViewEnd();
	~swMapViewEnd();
	bool			create(swMapView* sview=NULL);
	#if SW_RPC_ICE_CLIENT
	bool			create(swmap::MapIdentT& mapid,swMapRpcClient*);	
	#endif
	
	void				setViewRect(const swViewRect& rc);		///<������ʾ�����С
	swViewRect	getViewRect();
	void				setCenterPoint(const swGeoPoint& );
	swGeoPoint	getCenterPoint();
	void				setCenterPoint(const swViewPoint& );
	swViewPoint	getViewCenterPoint();

	void				setScaleLevel(swUInt8 );							///<�������ż��� 0 ~ (SWMAP_SCALELEVEL_NUM-1)
	swUInt8			getScaleLevel();
	void				setScaleLevelTable(const swFloat* units); //�������ż���
	swFloat*		getScaleLevelTable();

	swGeoRect		getGeoRect();															///<��ȡ��ǰ�����������
	swMapViewProps&	getProps();
	swViewIdentT&				getId();
	swGeoPoint	convertPointView2Geo(const swViewPoint& pt); //ת����ͼ���굽��������
	swViewPoint	convertPointGeo2View(const swGeoPoint& pt); //ת����ͼ���굽��������
	swFloat		getCurScaleUnit();											// ��õ�ǰ���ű�����Ļһ���ر�ʾ���ٵ����׳���
	void		fitGeoRect(const swGeoRect& rc) ;								// ��ָ����С�ĵ���������뵱ǰ����ͼ�����ڿɼ���Ҫ���Զ��������ű�
	
#define CURRENT_SCALE 0xff
	swMapLayerDrawPropT* getLayerDrawProp(swUInt8 layerId,swUInt8 scale=CURRENT_SCALE); //
	void	setLayerDrawProp(swUInt8 layerId,swUInt8 scale,const swMapLayerDrawPropT& drawprop);
	swMapLayerProps* getLayerProps( swUInt8 id); 
	void	setBuffer(swUInt8 b);  //���ñ߽���
	//swViewSize getSizeOfCahceDC();	//���㻺��DC�Ĵ�С
	void render(swDC* dc=NULL);					//��Ⱦ��Ŀ���豸����

	void sizeChanged(const swViewSize& size); //�ͻ�����ı��С
	swBitmap*		getMemBitmap();
	void				rotate(float radians) ;  //��ͼ��ת 
private:
	#if SW_RPC_ICE_CLIENT
	swmap::MapIdentT _mapid;
	swMapRpcClient*	_rpclient;	
	#endif
private:
	swMapView*		_sview;	 // server view
	swMapViewProps	_props;
	swMemoryDC	_cachedc;	//����DC
	swMemoryDC	_tmpdc;
	swBitmap*		_cacheBitmap;
	swBitmap*		_tmpBitmap;
	swUInt8			_buffer;	//�߽绺����������Ĭ��Ϊ1
	swGeoPoint	_newCenterPt;
	swUInt8			_newScale;			//�������õ����ż��� 
	float				_rotate;
	float				_newRotate;
};


/*
���������ĵ��ƫ��
*/
inline 
swGeoPoint	swMapViewEnd::convertPointView2Geo(const swViewPoint& pt){
	swViewPoint cxy = getViewCenterPoint();
	swGeoPoint r;
	r = _props.centerXY;
	r.x += ((pt.x - cxy.x) * getCurScaleUnit()*1.0)/sw_METERS_PER_DEGREE;
	r.y += ( -(pt.y - cxy.y) * getCurScaleUnit()*1.0)/sw_METERS_PER_DEGREE;//���������y����γ�ȣ�����Ļ�����y�����෴
	return r;
}


/*
�������������������Ĳ�ֵ(��)��ת��Ϊ�ף����㵱ǰ�������µ����ظ���
*/
inline 
swViewPoint	swMapViewEnd::convertPointGeo2View(const swGeoPoint& pt){
	swGeoPoint cxy = _props.centerXY;
	swViewPoint r = getViewCenterPoint();
	r.x += (pt.x-cxy.x)*sw_METERS_PER_DEGREE/(getCurScaleUnit()*1.0);
	r.y += -(pt.y-cxy.y)*sw_METERS_PER_DEGREE/(getCurScaleUnit()*1.0); //�������귽���෴
	return r;
}

#endif

/*
��ǰ��ͼ���ĵ�ĵ�������Ϊ���ջ�׼�㣬��ͼ��ͼ�ƶ��Ǽ����µ����ĵ��뵱ǰ���ĵ��λ����������ͼ����DC��ƫ���

*/