
/*
map_user.h
用户地图相关数据
*/
#ifndef _MAP_USER_H
#define _MAP_USER_H
#include "map_map.h"
#include <wx/wx.h>
#include <boost/shared_ptr.hpp>
using namespace boost;
#include <map>
struct mapGeoScenseT{
	int					sid;
	GeoPointT		cxy;					//中心点
	int					scale;				//地图缩放级别
	wxString		name;
};

typedef std::map<int,mapGeoScenseT>	mapGeoScenceListT;

#endif


