#include "render.h"
#include "layer.h"
#include "view.h"

swMapRender::swMapRender(swMapView* view){
	_view = view;
	_memBitmap = NULL;
	cairo_matrix_init_identity(&_matrix);
	_rotateCpt.x = _rotateCpt.y = 0;
	_rotate = 0; //默认不旋转
}

swMapRender::~swMapRender(){
	if(_memBitmap){
		delete _memBitmap;
	}
}


swMapView*	swMapRender::getView(){
	return _view;
}

swRenderType	swMapRender::getType(){
	return _type;
}


void		swMapRender::beginLayer(swUInt8 layerId){
	swMapLayerDrawPropT* drawprop;
	drawprop = _view->getLayerDrawProp(layerId);
	swPen pen ;
	swBrush brush;
	swFont font;
	pen = getDC()->GetPen();
	brush = getDC()->GetBrush();
	font = getDC()->GetFont();
	swColor color;
	color.Set(drawprop->penColor);
	pen.SetColour(color);
	pen.SetWidth(drawprop->penWidth);
	color.Set(drawprop->brushColor);
	brush.SetColour(color);
	font.SetFaceName(drawprop->fontName);
	font.SetPointSize(drawprop->fontSize);
	getDC()->SetPen(pen);
	getDC()->SetBrush(brush);
	getDC()->SetFont(font);
}

void		swMapRender::endLayer(swUInt8 layerId){
}

void		swMapRender::sizeChanged(const swViewSize& size){
	swDrawSize dcsize;
	if( _memdc.IsOk() ){
		dcsize = _memdc.GetSize();
		if(dcsize.GetWidth() == size.width && dcsize.GetHeight()==size.height){
			return ;
		}
	}
	
	if(_memBitmap){
		delete _memBitmap;
	}
	_memBitmap  =new swBitmap();
	_memBitmap->Create(size.width,size.height);
	_memdc.SelectObject(*_memBitmap);
}

swDC*		swMapRender::getDC(){
	return &_memdc;
}

//目前支持原点旋转和位移，支持根据任何坐标点进行旋转					  
void swMapRender::transform_point(int& x,int& y){
 
	double dx,dy;	
	//#ifdef _SW_MATRIX_SUPPORT
	if( _rotate!=0 ){
		int x1,x2,y1,y2;
		
		x1 = x-_rotateCpt.x;
		y1 = y-_rotateCpt.y;
		
		dx = x1; dy = y1;
		cairo_matrix_transform_point(&_matrix,&dx,&dy);
		x2 = (int)dx;
		y2 =(int)dy;	
		
		x += x2-x1;
		y += y2-y1;
		return ;
	}
	//#endif
	dx = x; dy = y;
	cairo_matrix_transform_point(&_matrix,&dx,&dy);
	x = (int)dx;
	y =(int)dy;	
	
}

void swMapRender::rotate(float radians,const swViewPoint& cpt){
	//cairo_matrix_rotate(&_matrix,radians);
	cairo_matrix_init_rotate(&_matrix,radians);
	_rotateCpt = cpt;
	_rotate = radians;
}

void swMapRender::scale(float sx,float sy){
	cairo_matrix_scale(&_matrix,sx,sy);

}

void swMapRender::translate(float tx,float ty){
	cairo_matrix_translate(&_matrix,tx,ty);
}