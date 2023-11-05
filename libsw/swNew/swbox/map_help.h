
#ifndef _MAP_HELPER_H
#define _MAP_HELPER_H

#include "gvms_cmc.h"
#include "map_map.h"
#include <vector>
#include <map>
#include <list>
#include <wx/wx.h>

#include <geos.h>
using namespace geos;
//using namespace geos::io;

#include <boost/shared_ptr.hpp>
using namespace boost;

/* ��������geos ����*/
template <class T>
shared_ptr<geom::Geometry> GeoMap_getEnvelope( std::list< T* >& rects);

template <class T>
geom::Polygon*		GeoMap_createPolygon( T* rect);

template <class T>
bool GeoMap_geosGeometry2Rect( geom::Geometry * geometry, T& rect);

//geom::Coordinate operator = (geom::Coordinate &c,ViewRectT&);
//static operator geom::Coordinate(ViewRectT& v);
//ViewRectT operator =(const geom::Coordinate& c,const ViewRectT vrc);

template < class T>
inline geom::Coordinate GeoMap_Point2Coordinate(const tagPointT<T>& pt){
	geom::Coordinate c;
	c.x = (T)pt.x;
	c.y = (T)pt.y;
	return c;
}

template < class T>
inline tagPointT<T> GeoMap_Coordinate2Point(const geom::Coordinate& c){
	tagPointT<T> r;
	r.x = (T)c.x;
	r.y = (T)c.y;
	return r;
}

template <class T> 
inline void GeoMap_NormalizeRect( T & r){
	if( r.w <0){
		r.x+=r.w;
		r.w*=-1;
	}
	if( r.h < 0){
		r.y +=r.h;
		r.h*=-1;
	}
}

template <class T> 
inline geom::LinearRing  GeoMap_CreateLinearRing(const tagRectT<T>& rc){
	geom::GeometryFactory factory;
	geom::CoordinateArraySequence* coords =new geom::CoordinateArraySequence();
	coords->add( geom::Coordinate( rc.x,rc.y ));
	coords->add( geom::Coordinate( rc.x+rc.w-1,rc.y ));
	coords->add( geom::Coordinate( rc.x+rc.w-1,rc.y+rc.h-1 ));
	coords->add( geom::Coordinate( rc.x,rc.y+rc.h ));
	coords->add( geom::Coordinate( rc.x,rc.y ));
	geom::LinearRing ring(coords, &factory);
	return ring;
}

template <class T> 
inline shared_ptr<geom::Polygon>  GeoMap_CreatePolygon(const tagRectT<T>& rc){
	geom::GeometryFactory factory;
	geom::LinearRing ring = GeoMap_CreateLinearRing<T>(rc);
	shared_ptr<geom::Polygon> r;
	r = shared_ptr<geom::Polygon>( factory.createPolygon( ring,std::vector<geom::Geometry*>()) );
	return r;
}


//��������ת������ͼ�����
class GeoMapCanvas;
shared_ptr<geom::Polygon> GeoMap_Geo2View( geom::Polygon* geo,GeoMapCanvas* );

void drawBackEnvelope(ViewPointT& p1,ViewPointT& p2,wxDC* destDC,wxDC* srcDC);
void drawBackEnvelope(std::vector<ViewPointT>& polygon,wxDC* destDC,wxDC* srcDC);

ViewRectT help_calcEnvelope(std::vector<ViewPointT>& points);

void help_drawTransparentPolygon(std::vector<ViewPointT>& points,ViewPointT& exceptPt,ViewRectT & bgrc,wxDC* dest,wxDC* src);

//���ݶ���������ζ��� 
template<class T>
inline shared_ptr<geom::Polygon> GeoMap_CreatePolygonWithPoints(std::vector<T>& points){
	shared_ptr<geom::Polygon> polygon;
	GeometryFactory  factory;
	std::vector< geom::Coordinate >* ptset = new std::vector< geom::Coordinate >();
	for(int n= 0;n<points.size();n++){
		ptset->push_back( geom::Coordinate(points[n].x,points[n].y));
	}
	if( points.size()){
		ptset->push_back( geom::Coordinate(points[0].x,points[0].y));
	}
	geom::LinearRing* lr = factory.createLinearRing(factory.getCoordinateSequenceFactory()->create( ptset));;
	polygon = shared_ptr<geom::Polygon>(factory.createPolygon(lr,NULL));
	return polygon;
}

template <class T>
inline std::vector<tagPointT<T> > GeoMap_GetGeometryPoints(geom::Geometry* g){
	std::vector<tagPointT<T> > r;
	shared_ptr<geom::CoordinateSequence > cs = shared_ptr<geom::CoordinateSequence >(g->getCoordinates());
	size_t n=0;
	for(;n<cs->size();n++){
		r.push_back( tagPointT<T>((T)cs->getAt(n).x,(T)cs->getAt(n).y ));
	}
	return r;
}

//void GeoMap_PolygonGeo2View
//
void GeoMap_PointsView2Geo(std::vector<ViewPointT>& pts1,std::vector<GeoPointT>& pt2);
void GeoMap_PointsGeo2View(std::vector<GeoPointT>& pt1,std::vector<ViewPointT>& pts2);

template< class T>
inline std::string GeoMap_GetWktPolygon(std::vector<T>& points){
	shared_ptr<geom::Polygon> polygon;
	polygon = GeoMap_CreatePolygonWithPoints(points);
	std::string text;
	char buff[64];
	text="POLYGON((";
	for(int n=0;n<points.size();n++){
		sprintf(buff,"%0.4f %0.4f",points[n].x,points[n].y);
		text+=buff;
		//if( n!=points.size()-1){
			text+=",";
		//}
	}
	if(points.size()){
		sprintf(buff,"%0.4f %0.4f",points[0].x,points[0].y);
		text+=buff;
	}
	text+="))";
	return text;
}





#endif
