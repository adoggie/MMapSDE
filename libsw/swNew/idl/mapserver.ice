
#ifndef _MAP_SERVER_ICE
#define _MAP_SERVER_ICE


/*
mapserver.ice
2009.8.5 	scott created
1.提供地图渲染服务
*/

#include <Ice/Identity.ice>

module swmap{

/*基础数据结构定义*/
dictionary<string,string>	HashValueSet;
dictionary<string,string>	ReturnValueT;
sequence<byte> 				StreamDataT;
sequence<string>			StringListT;
sequence<HashValueSet>	HashValueListT;
sequence<int>					IntListT;
sequence<StringListT>	StringListListT;

//视图网格索引
struct ViewTileIndexT{
	int 	x;
	int		y;
};

//屏幕坐标点
struct ViewPointT{
	int 	x;
	int		y;
};


//地理坐标点
struct GeoPointT{
	float 	x;
	float		y;
};
sequence<GeoPointT> GeoPointListT;

//地理矩形
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
服务器渲染返回的地图区块数据
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
地图请求区域信息
由客户端发起传送的服务器处理
*/
struct MapViewTileRectT{
	ViewTileIndexT	idx;
	GeoPointT				gcpt;
	ViewPointT			destpt; //not used
	float						rotate;	//旋转角度
	float						xscale;	//缩放比
	float						yscale;	//缩放比
	float						translate;	//位移偏差	
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
服务器端存储的地图标识信息
*/
struct MapIdentT{
	int 	id;			//地图标识
	string name;	//地图名称
};


interface IMapView{
	/********************************8888888
	method: getBitmapTiles()
		请求网格块渲染
	params:
		scale	地图缩放级别
		tileWidth	请求网格块大小规格
		tileHeight	请求网格块大小规格
		rects				请求渲染的网格块矩形信息 
	return:
			
	***********************************/
	MapBitmapTileListT	getBitmapTiles(int scale,int tileWidth,int tileHeight,MapViewTileRectListT rects);	
	MapScalePrecisionT	getScalePrecision();
	void 		setScalePrecision(MapScalePrecisionT scales);
	string  getId();		//视图对象标识
	void	heartBeat();	//心跳
};
sequence<IMapView*> ServerViewListT;

interface IMapServer{
	IMapView	createView(MapIdentT mapid);		//创建地图视图
	int 	getTimestamp();											//获取系统时钟  1970之后秒数 	
	MapBitmapTileListT	getBitmapTiles(MapIdentT mapid,int scale,int tileWidth,int tileHeight,MapViewTileRectListT rects);	
	MapScalePrecisionT	getScalePrecision(MapIdentT mapid);
};



};



#endif
