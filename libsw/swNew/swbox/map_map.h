
#ifndef _MAP_MAP_H
#define _MAP_MAP_H

#include <string>
#include <vector>
#include <map>
#include <list>



template < class T>
struct tagPointT{
	T x,y;
	tagPointT(T x_,T y_):x(x_),y(y_){}
	tagPointT(){ x=y=0;}
	bool operator==(const tagPointT<T>& pt){
		if( x == pt.x && y == pt.y){
			return true;
		}
		return false;
	}
};
typedef tagPointT< double > GeoPointT;
typedef tagPointT< int > ViewPointT;

struct TrackGeoPointT{
	int timestamp;
	GeoPointT point;	
};


template <class T>
struct tagSizeT{
	T w,h;
	tagSizeT(){ w=h=0;}
};
typedef tagSizeT<int> ViewSizeT;
typedef tagSizeT<double> GeoSizeT;


template <class T=int>
struct tagRectT{
	T x,y,w,h;
	tagRectT(){ x=y=w=h=0;};
	bool operator ==(const tagRectT<T>& rc){
		if( x==rc.x && y==rc.y && w==rc.w && h==rc.h){
			return true;
		}
		return false;
	}
	tagPointT<T>	_1(){
		return tagPointT<T>(x,y);
	}
	tagPointT<T>	_2(){
		return tagPointT<T>(x+w,y);
	}
	tagPointT<T>	_3(){
		return tagPointT<T>(x,y+h);
	}
	tagPointT<T>	_4(){
		return tagPointT<T>(x+w,y+h);
	}
};

typedef tagRectT<int>			ViewRectT;
typedef tagRectT<double>		GeoRectT;

struct GeoMapViewConfigT{
	GeoMapViewConfigT();
	void* hWnd;
	std::vector<double>	map_scale_list;
	int		scale;			//缩放级别
	GeoPointT	centerXY;	//地图中心点
	GeoRectT	map_extent;	//地图可是范围
};

struct GeoMapInitConfigT{
	std::string		mapPath;	//地图路径
};

struct GeoMapBitmapT{
	int		width;
	int		height;
	void*	hdc;	
	std::vector<unsigned char>	data;
};


class GeoMapLib;
class GepMapCanvas;
class GeoMapView{
public:
	GeoMapView(GeoMapLib*	maplib);
	virtual bool init(GeoMapViewConfigT& conf);
	GeoMapViewConfigT&	getConfig();
	virtual void	getBitmap(GeoMapBitmapT& bitmap);
	// cx,cy 新的中心点坐标
	virtual void	zoomIn(int cx,int cy,int level = 1);
	virtual void	zoomInRect(ViewRectT rect);	//放大矩形区域
	virtual void	zoomOut(int cx,int cy,int level = 1);
	virtual void	zoomOutRect(ViewRectT rect);
	virtual void	xyV2M(int x,int y,GeoPointT& point);		//地图视图转换
	virtual void	xyM2V(const GeoPointT& point,int &x,int &y);
	virtual void	xyM2V(double px,double py,ViewPointT& xy);
	virtual void	changeSize(int cx,int cy);
	virtual GeoRectT	getGeoRect();
	virtual void			moveGeoCenterXY(GeoPointT xy);
	virtual void	draw();
	virtual void setScale(double dx, double dy,int scalelevel);
	virtual void getOffset(float &x, float &y){}
	friend class GepMapCanvas;
protected:
	GeoMapLib*				_maplib;
	GeoMapViewConfigT	_conf;
};

class GeoMapLib{
public:
	GeoMapLib();
	bool	init(GeoMapInitConfigT& conf);
	GeoMapInitConfigT&	getConfig();
	static GeoMapLib&	instance();
	GeoMapView*		newMapView();
private:
	static GeoMapLib	_handle;
	friend GeoMapView;
	GeoMapInitConfigT	_conf;
};

#endif

