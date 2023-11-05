
#ifndef _MAPTOOL_BARRIER_H
#define _MAPTOOL_BARRIER_H

#include "map_tools.h"
class GeoMapTool_NewBarrier:public GeoMapToolBase{
public:
	GeoMapTool_NewBarrier();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
	
private:
	void onMapRedraw(const ViewRectT& rect=ViewRectT());
	void drawLine(ViewPointT& p1,ViewPointT& p2);
	void drawPolygon();
public:	
	shared_ptr<ViewPointT>	_lastpt;
	std::vector<ViewPointT>		_points;
};

#endif

