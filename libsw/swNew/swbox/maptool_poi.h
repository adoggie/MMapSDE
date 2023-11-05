
#ifndef _MAPTOOL_POI_H
#define _MAPTOOL_POI_H

#include "map_tools.h"
//
class uiFrameUserPoiDetail;
//////////////////////////////////////////////////////////////////////////
class GeoMapTool_NewPoi:public GeoMapToolBase{
public:
	GeoMapTool_NewPoi();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
	void onMapRedraw(const ViewRectT& rect=ViewRectT());
	void drawTempPoi();
	void reset();
public:
	shared_ptr<ViewPointT>	_lastpt;
	shared_ptr<GeoMapSymbolBase> _cur_symbol;
	wxCriticalSection			_drawMutex;
	wxBitmap					_bitmap_default;
};
#endif

