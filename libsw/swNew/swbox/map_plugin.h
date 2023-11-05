
#ifndef _MAP_PLUGIN_H
#define _MAP_PLUGIN_H

#include <wx/wx.h>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

#include <map>

#include <boost/shared_ptr.hpp>
using namespace boost;

class GeoMapCanvas;

class GeoMapCanvasPlugin{
public:
	virtual void OnDraw(GeoMapCanvas* canvas,wxDC* dc) = NULL;
};

#endif
