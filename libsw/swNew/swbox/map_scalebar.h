
#ifndef _MAP_PLUGIN_SCALEBAR_H
#define _MAP_PLUGIN_SCALEBAR_H

#include "map_plugin.h"

class MapCanvasPlugin_ScaleBar:public GeoMapCanvasPlugin{
public:
	void OnDraw(GeoMapCanvas* canvas,wxDC* dc);
};


#endif
