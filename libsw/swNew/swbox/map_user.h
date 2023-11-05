
/*
map_user.h
�û���ͼ�������
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
	GeoPointT		cxy;					//���ĵ�
	int					scale;				//��ͼ���ż���
	wxString		name;
};

typedef std::map<int,mapGeoScenseT>	mapGeoScenceListT;

#endif


