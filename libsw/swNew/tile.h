
#ifndef _SWMAP_TILE_H
#define _SWMAP_TILE_H

#include "base.h"

//��ͼ��Ⱦ�������ͼ�����
struct swMapBitmapTileT{
	typedef swViewPoint INDEX;
	//swMemoryDC*	memdc;	// ��ͬһ������ֱ�Ӳ����ڴ���Ƴ����ĵ�ַ�����ݣ������ǵ��ֲ�ʽʱӦ�滻��RGB���ݻ���ʸ���ı���Ϣ
	//swBitmap		bitmap;	
	INDEX				idx;		//�ڴ����е�λ��(x,y)
};

typedef std::list<swMapBitmapTileT>  swMapBitmapTileListT; //
//typedef std::map<swUInt8,swMapBitmapTileListT> swMapScaleBitmapTileListT;


struct swViewTileRectT{
	typedef swViewPoint INDEX;
	swGeoPoint	gcpt;		//�������ĵ�
	INDEX				idx;		//�ڴ����е�λ��(x,y)
	swDC*				destdc;	//Ŀ����Ƴ���
	swViewPoint	destpt;	//Ŀ����ƿ�ʼ����
	float				rotate;	//��ת�Ƕ�
	float				xscale;	//���ű�
	float				yscale;	//���ű�
	float				translate;	//λ��ƫ��	
	swViewTileRectT(){
		rotate = 0;
		xscale = 1;
		yscale = 1;
		translate =0;
	}
};
typedef std::list<swViewTileRectT> swViewTileRectListT;


#endif