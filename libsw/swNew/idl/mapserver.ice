
#ifndef _MAP_SERVER_ICE
#define _MAP_SERVER_ICE


/*
mapserver.ice
2009.8.5 	scott created
1.�ṩ��ͼ��Ⱦ����
*/

#include <Ice/Identity.ice>

module swmap{

/*�������ݽṹ����*/
dictionary<string,string>	HashValueSet;
dictionary<string,string>	ReturnValueT;
sequence<byte> 				StreamDataT;
sequence<string>			StringListT;
sequence<HashValueSet>	HashValueListT;
sequence<int>					IntListT;
sequence<StringListT>	StringListListT;

//��ͼ��������
struct ViewTileIndexT{
	int 	x;
	int		y;
};

//��Ļ�����
struct ViewPointT{
	int 	x;
	int		y;
};


//���������
struct GeoPointT{
	float 	x;
	float		y;
};
sequence<GeoPointT> GeoPointListT;

//�������
struct GeoRectT{
	float 	x;
	float		y;
	float		width;
	float		height;
};
sequence<GeoRectT> GeoRectListT;


sequence<byte> BitmapDataT;



/*
MapBitmapTileT
��������Ⱦ���صĵ�ͼ��������
*/
struct MapBitmapTileT{
	ViewTileIndexT	idx;
	BitmapDataT			bitmap;
	int							width;
	int							height;
};

sequence<MapBitmapTileT> 	MapBitmapTileListT;

/*
MapViewTileRectT
��ͼ����������Ϣ
�ɿͻ��˷����͵ķ���������
*/
struct MapViewTileRectT{
	ViewTileIndexT	idx;
	GeoPointT				gcpt;
	ViewPointT			destpt; //not used
	float						rotate;	//��ת�Ƕ�
	float						xscale;	//���ű�
	float						yscale;	//���ű�
	float						translate;	//λ��ƫ��	
};
sequence<MapViewTileRectT> MapViewTileRectListT;


struct MapScalePrecisionT{
	float 	s0;
	float 	s1;
	float 	s2;
	float 	s3;
	float 	s4;
	float 	s5;
	float 	s6;
	float 	s7;
	float 	s8;
	float 	s9;
	float 	s10;
	float 	s11;
	float 	s12;
	float 	s13;
	float 	s14;
	float 	s15;
};

/*
MapIdentT 
�������˴洢�ĵ�ͼ��ʶ��Ϣ
*/
struct MapIdentT{
	int 	id;			//��ͼ��ʶ
	string name;	//��ͼ����
};


interface IMapView{
	/********************************8888888
	method: getBitmapTiles()
		�����������Ⱦ
	params:
		scale	��ͼ���ż���
		tileWidth	����������С���
		tileHeight	����������С���
		rects				������Ⱦ������������Ϣ 
	return:
			
	***********************************/
	MapBitmapTileListT	getBitmapTiles(int scale,int tileWidth,int tileHeight,MapViewTileRectListT rects);	
	MapScalePrecisionT	getScalePrecision();
	void 		setScalePrecision(MapScalePrecisionT scales);
	string  getId();		//��ͼ�����ʶ
	void	heartBeat();	//����
};
sequence<IMapView*> ServerViewListT;

interface IMapServer{
	IMapView	createView(MapIdentT mapid);		//������ͼ��ͼ
	int 	getTimestamp();											//��ȡϵͳʱ��  1970֮������ 	
	MapBitmapTileListT	getBitmapTiles(MapIdentT mapid,int scale,int tileWidth,int tileHeight,MapViewTileRectListT rects);	
	MapScalePrecisionT	getScalePrecision(MapIdentT mapid);
};



};



#endif
