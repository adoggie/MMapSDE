#include "map_canvas.h"
#include "maptool_barrier.h"

#include "map_resource.h"
#include "map_help.h"
#include "mainapp.h"
#include "ui_barrier_detail.h"

GeoMapTool_NewBarrier::GeoMapTool_NewBarrier(){
	_cursorName = wxT("maptool_newbarrier");
	_type = GMTT_NEW_BARRIER;
	init();
}

void	GeoMapTool_NewBarrier::set(GeoMapCanvas* canvas){
	GeoMapToolBase::set( canvas );
}	

void  GeoMapTool_NewBarrier::onMouseEvent(wxMouseEvent & evt){
	//
	static std::vector<ViewPointT> trackPoints;
	if( evt.LeftDown()){

		trackPoints.push_back(ViewPointT(evt.GetX(),evt.GetY()));
		_points.clear();
	}
	std::vector<ViewPointT> pts;
	ViewRectT rc;
	wxMemoryDC			drawDC;
	wxBitmap				drawBitmap;	
	wxClientDC dc(_canvas);
	if( evt.Moving()){
		/*
		if( _points.size()==1){
			if( _lastpt.get()){ //擦出前一个移动点与起点的矩形区域
				pts = _points;
				pts.push_back(*_lastpt.get());
				rc = help_calcEnvelope(pts);
				dc.Blit(rc.x,rc.y,rc.w,rc.h,_canvas->getTopmostLayer()->getDrawDC(),rc.x,rc.y);
			}
			_lastpt = shared_ptr<ViewPointT>(new ViewPointT(evt.GetX(),evt.GetY()));
			//draw line
			//dc.DrawLine(_points[0].x,_points[0].y,_lastpt->x,_lastpt->y);
		}else if( _points.size()>=2){
			//draw polygon
			drawPolygon();
		}*/
		if(trackPoints.size()){
			ViewPointT exceptPt(0,0);
			if( _lastpt.get()){ //擦出前一个移动点与起点的矩形区域
				//pts = _points;
				//pts.push_back(*_lastpt.get());
				//rc = help_calcEnvelope(pts);
				//dc.Blit(rc.x,rc.y,rc.w,rc.h,_canvas->getTopmostLayer()->getDrawDC(),rc.x,rc.y);
				exceptPt = *_lastpt.get();
			}
			_lastpt = shared_ptr<ViewPointT>(new ViewPointT(evt.GetX(),evt.GetY()));
			_points = trackPoints;
			pts = trackPoints;
			pts.push_back(*_lastpt.get()); //绘制当前多边形
			ViewRectT bgRc;
			bgRc.x =0; bgRc.y = 0;bgRc.w = _canvas->GetSize().GetWidth(); bgRc.h = _canvas->GetSize().GetHeight();
			help_drawTransparentPolygon(pts,exceptPt,bgRc,&dc,_canvas->getTopmostLayer()->getDrawDC());
		}
	}
	if( evt.ButtonDClick(wxMOUSE_BTN_LEFT)){
		//commit ,提交新的barrier
		if( trackPoints.size()<3){
			trackPoints.clear();
			_points.clear();
			_canvas->update(false);
			return;
		}
		uiFrameBarrierDetail::instance()->show();
		_points = trackPoints;
		trackPoints.clear();
		uiFrameBarrierDetail::instance()->setSID();
		uiFrameBarrierDetail::instance()->setName(wxT(""));
		uiFrameBarrierDetail::instance()->setGroup(0);
		shared_ptr<geom::Polygon> polygon = GeoMap_CreatePolygonWithPoints(_points);
		geom::Coordinate coord;
		polygon->getEnvelopeInternal()->centre(coord);
		uiFrameBarrierDetail::instance()->setGeoXY(coord.x,coord.y);
		
		
	//	_points.clear();
	//	_canvas->update(false);
	}
	if( evt.RightUp()){
		_points.clear();
		trackPoints.clear();
		_canvas->update(false);
	}
}

void GeoMapTool_NewBarrier::drawLine(ViewPointT& p1,ViewPointT& p2){
	wxClientDC  dc(_canvas);
	drawBackEnvelope(p1,p2,&dc,_canvas->getTopmostLayer()->getDrawDC());
	
}

void GeoMapTool_NewBarrier::drawPolygon(){

}
void GeoMapTool_NewBarrier::onMapRedraw(const ViewRectT& rect){
	ViewRectT bgRc;
	wxClientDC dc(_canvas);
	bgRc.x =0; bgRc.y = 0;bgRc.w = _canvas->GetSize().GetWidth(); bgRc.h = _canvas->GetSize().GetHeight();
	help_drawTransparentPolygon(_points,ViewPointT(),bgRc,&dc,_canvas->getTopmostLayer()->getDrawDC());
}

/*
BOOL SetLayeredWindowAttributes(          HWND hwnd,
COLORREF crKey,
BYTE bAlpha,
DWORD dwFlags
);
*/