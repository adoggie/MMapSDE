#include "map_canvas.h"
#include "main_window.h"
#include "maptool_poi.h"

#include "map_resource.h"
#include "map_help.h"
#include "mainapp.h"
#include "ui_userpoi_detail.h"

GeoMapTool_NewPoi::GeoMapTool_NewPoi(){
	_cursorName = wxT("maptool_newpoi");
	_type = GMTT_NEW_POI;
	init();
	wxBitmap _bitmap_default = SysResMgr::instance().getIcon(wxT("poi_sign_0"))->bitmap;
	
}
 
void	GeoMapTool_NewPoi::set(GeoMapCanvas* canvas){
	//return;
///	wxCriticalSectionLocker l(_drawMutex);
//	if( !_framePoiDetail.get()){
//		_framePoiDetail =  shared_ptr<uiFrameUserPoiDetail>(new uiFrameUserPoiDetail(cmcMainFrame::instance().get()));
//	}
	
	if( canvas == NULL){
		//_framePoiDetail->Show(false);
		uiFrameUserPoiDetail::instance()->Show(false);
		_lastpt.reset();
		_canvas = NULL;
		//_canvas->update(false);
	}
	GeoMapToolBase::set( canvas );
}	 

//��ͼˢ�µ�ʱ��֪ͨ 
void GeoMapTool_NewPoi::onMapRedraw(const ViewRectT& rect){
	wxCriticalSectionLocker l(_drawMutex);
	drawTempPoi();
}

void GeoMapTool_NewPoi::drawTempPoi(){
	//wxLogMessage(wxT("drao poi sense..."));
	if( _canvas == NULL){
		return ;
	}
	 wxClientDC dc(_canvas);
	 if(!dc.IsOk()){
		 return;
	 }
	 if( !_bitmap_default.IsOk()){
		 return;
	 }
	int x,y;
	x = _lastpt->x - _bitmap_default.GetWidth()/2		;
	y = _lastpt->y - _bitmap_default.GetHeight()/2;
	dc.DrawBitmap(_bitmap_default,x,y,true);	
	
}

/*
ÿ�ε������Ѿ�����poi���ƶ�����������
*/
void  GeoMapTool_NewPoi::onMouseEvent(wxMouseEvent & evt){
	if( !_canvas)
		return;
	if( evt.ButtonDown(wxMOUSE_BTN_LEFT ) ){		
		_lastpt = shared_ptr<ViewPointT>( new ViewPointT);		
		_lastpt->x = evt.GetX();
		_lastpt->y = evt.GetY();	
		
		_canvas->update(false) ; //ɾ����ǰ
	}else if( evt.ButtonUp(wxMOUSE_BTN_LEFT)){
		if( !_lastpt.get()){
			return;
		}		
		uiFrameUserPoiDetail::instance()->Show();
		uiFrameUserPoiDetail::instance()->set();
		GeoPointT gpt;
		gpt = _canvas->xy_View2Geo(ViewPointT(evt.GetX(),evt.GetY()));
		uiFrameUserPoiDetail::instance()->setGeoXY(gpt.x,gpt.y);
	}	
}

void GeoMapTool_NewPoi::reset(){
	uiFrameUserPoiDetail::instance()->Show(false);
	_lastpt.reset();
}