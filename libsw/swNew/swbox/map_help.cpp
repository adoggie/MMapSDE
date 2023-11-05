#include "main_window.h"
#include "map_canvas.h"
#include "map_help.h"




template <class T>
shared_ptr<geom::Geometry> GeoMap_getEnvelope( std::list< T >& rects){
	std::list< T>::iterator itr;
	shared_ptr<geom::Geometry> geom;

	for( itr= rects.begin(); itr!= rects.end(); itr++){
		geom::Polygon* p = GeoMap_createPolygon(*itr);
		if( geom.get() ){
			geom = shared_ptr<geom::Geometry>( p->Union( geom ) )
			delete p;
		}else{
			geom = shared_ptr<geom::Geometry>( p );
		}
	}
	return geom;
}

template <class T>
geom::Polygon*		GeoMap_createPolygon( T rect){
	geom::GeometryFactory factory;	
	geom::Polygon* p = factory.createPolygon();
	
	p->getCoordinates()->add( geom::Coordinate( (double*)rect.x,(double*)rect.y ) );	
	p->getCoordinates()->add( geom::Coordinate( (double*)(rect.x+rect.w),(double*)rect.y )  );	
	p->getCoordinates()->add( geom::Coordinate( (double*)(rect.x),(double*)(rect.y+rect.h))  );	
	p->getCoordinates()->add( geom::Coordinate( (double*)(rect.x+rect.w),(double*)(rect.y+rect.h)) );	
	return p;
}

template<class T>
bool GeoMap_geosGeometry2Rect( geom::Geometry * geometry, T& rect){
	if( geometry->getSize() != 4)	return false;
	shared_ptr<geom::CoordinateArraySequence> cseq( geometry->getCoordinates() );
	//rect.x = cseq[0]
	return true;
}

/////////////////////////
//转换 地理坐标多边形到视图多边形
shared_ptr<geom::Polygon> GeoMap_Geo2View( geom::Polygon* geo,GeoMapCanvas* canvas){
	shared_ptr<geom::Polygon> r;
	shared_ptr<geom::CoordinateSequence> cseq = shared_ptr<geom::CoordinateSequence>( geo->getCoordinates() );
	geom::GeometryFactory factory;
	geom::CoordinateArraySequence* coords =new geom::CoordinateArraySequence();
	if( cseq->size() ){
		//shared_ptr<geom::CoordinateArraySequence> coords (new geom::CoordinateArraySequence());
		ViewPointT vp ;
		for(int n=0; n<cseq->size();n++){
			vp = canvas->xy_Geo2View( GeoPointT( cseq->getAt(n).x,cseq->getAt(n).y));
			coords->add( GeoMap_Point2Coordinate<int>(vp) );
		}
		if( cseq->getSize()){
			vp = canvas->xy_Geo2View( GeoPointT( cseq->getAt(0).x,cseq->getAt(0).y));
			coords->add( GeoMap_Point2Coordinate<int>(vp) );
		}
		geom::LinearRing ring ( coords,&factory );
		r = shared_ptr<geom::Polygon>( factory.createPolygon( ring,std::vector<Geometry*>() ) );
	}
	return r;

}

void drawBackEnvelope(ViewPointT& p1,ViewPointT& p2,wxDC* destDC,wxDC* srcDC){
	geom::GeometryFactory factory;
	shared_ptr<geom::CoordinateSequence> cs = shared_ptr<geom::CoordinateSequence>(factory.getCoordinateSequenceFactory()->create(NULL));
	cs->add(geom::Coordinate(p1.x,p1.y));
	cs->add(geom::Coordinate(p2.x,p2.y));
	std::auto_ptr<geom::LineString> ls (factory.createLineString(*cs.get()));
	const Envelope * evp = ls->getEnvelopeInternal();
	destDC->Blit(evp->getMinX(),evp->getMinY(),evp->getWidth(),evp->getHeight(),srcDC,evp->getMinX(),evp->getMinY());
}

void drawBackEnvelope(std::vector<ViewPointT>& polygon,wxDC* destDC,wxDC* srcDC){
	geom::GeometryFactory factory;
	shared_ptr<geom::CoordinateSequence> cs = shared_ptr<geom::CoordinateSequence>(factory.getCoordinateSequenceFactory()->create(NULL));
	int n;
	for(n=0;n<polygon.size();n++){
		cs->add(geom::Coordinate(polygon[n].x,polygon[n].y));
	}
	std::auto_ptr<geom::LineString> ls (factory.createLineString(*cs.get()));
	const Envelope * evp = ls->getEnvelopeInternal();
	destDC->Blit(evp->getMinX(),evp->getMinY(),evp->getWidth(),evp->getHeight(),srcDC,evp->getMinX(),evp->getMinY());	
}

ViewRectT help_calcEnvelope(std::vector<ViewPointT>& points){
	geom::GeometryFactory factory;
	shared_ptr<geom::CoordinateSequence> cs = shared_ptr<geom::CoordinateSequence>(factory.getCoordinateSequenceFactory()->create(NULL));
	int n;
	for(n=0;n<points.size();n++){
		cs->add(geom::Coordinate(points[n].x,points[n].y));
	}
	std::auto_ptr<geom::LineString> ls (factory.createLineString(*cs.get()));
	const Envelope * evp = ls->getEnvelopeInternal();
	ViewRectT r;
	r.x = evp->getMinX();
	r.y = evp->getMinY();
	r.w = evp->getWidth();
	r.h = evp->getHeight();
	return r;
}

void help_drawTransparentPolygon(std::vector<ViewPointT>& points,ViewPointT& exceptPt,ViewRectT & bgrc,wxDC* dest,wxDC* src){
	wxMemoryDC			drawDC;
	wxBitmap				drawBitmap;	

	ViewRectT rc; 
	rc = help_calcEnvelope(points);
	//drawBitmap.Create( rc.w,rc.h);
	drawBitmap.Create(bgrc.w,bgrc.h);
	drawDC.SelectObject( drawBitmap );
	drawDC.SetPen(*wxBLACK_PEN);
	drawDC.SetBrush(*wxBLACK_BRUSH);
	//drawDC.SetBrush(*wxGREEN_BRUSH);
	drawDC.DrawRectangle(0,0,bgrc.w,bgrc.h);
	wxBrush br;
	br.SetColour(wxColour(0,255,0));
	drawDC.SetPen(*wxRED_PEN);
	drawDC.SetBrush(br);

	std::vector<wxPoint >  ptList;
	for(int n=0;n< points.size() ;n++){
		if( points[n].x == exceptPt.x && points[n].y == exceptPt.y){
			;
		}else{
			//ptList.push_back(wxPoint(points[n].x - rc.x,points[n].y -rc.y));
			ptList.push_back(wxPoint(points[n].x,points[n].y));
		}
	}
	//dest->SetBrush(*wxGREEN_BRUSH);
	if( ptList.size()>3){
		int s = ptList.size();
	}
//////////////////////////////////////////////////////////////////////////
/*
	drawDC.Blit(0,0,rc.w,rc.h,src,rc.x,rc.y);
	drawDC.DrawPolygon(ptList.size(),&ptList[0]);
	drawDC.Blit(0,0,rc.w,rc.h,src,rc.x,rc.y,wxAND);
	dest->Blit(rc.x,rc.y,rc.w,rc.h,&drawDC,0,0);//,wxOR);
	*/
//////////////////////////////////////////////////////////////////////////
	drawDC.Blit(0,0,bgrc.w,bgrc.h,src,0,0);
	if( ptList.size()){
		drawDC.DrawPolygon(ptList.size(),&ptList[0]);
	}
	drawDC.Blit(0,0,bgrc.w,bgrc.h,src,0,0,wxAND);
	dest->Blit(0,0,bgrc.w,bgrc.h,&drawDC,0,0);//,wxOR);
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void GeoMap_PointsView2Geo(std::vector<ViewPointT>& pts1,std::vector<GeoPointT>& pts2){
	pts2.clear();
	int n;
	GeoPointT gpt;
	for(n=0;n<pts1.size();n++){
		gpt = cmcMainFrame::instance()->getMapCanvas()->xy_View2Geo(pts1[n]);
		pts2.push_back(gpt);
	}
}
void GeoMap_PointsGeo2View(std::vector<GeoPointT>& pts1,std::vector<ViewPointT>& pts2){
	pts2.clear();
	int n;
	ViewPointT vpt;
	for(n=0;n<pts1.size();n++){
		vpt = cmcMainFrame::instance()->getMapCanvas()->xy_Geo2View(pts1[n]);
		pts2.push_back(vpt);
	}
}


