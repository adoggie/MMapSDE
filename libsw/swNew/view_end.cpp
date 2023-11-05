
#include "view_end.h"
#include "geo.h"
#include "map.h"
#include "layer_mgr.h"
#include "rpc/rpc_client.h"

swMapViewEnd::swMapViewEnd(){
	_cacheBitmap = NULL;
	_tmpBitmap = NULL;
	_rotate = 0;
	_newRotate = 0;
	 
}

swMapViewEnd::~swMapViewEnd(){
	if(_cacheBitmap){
		delete _cacheBitmap;
	}
	if(_tmpBitmap){
		delete _tmpBitmap;
	}
}

swMapLayerProps*	swMapViewEnd::getLayerProps(swUInt8 layerId){
	if( layerId>=_props.layerprops.size()){
		return NULL;
	}
	return &_props.layerprops[layerId];
}

/*
���ÿ��Ӵ�������֮�󣬸��ݵ�ǰ���ż����Զ������ɼ���������
*/
void	swMapViewEnd::setViewRect(const swViewRect& rc){
	//if(_props.viewRc == rc){
	//	return;
	//}
// 	_props.viewRc = rc;
// 	float geow,geoh;
// 	geow = (rc.width * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
// 	geoh = (rc.height * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
// 	_props.geoRc.x = _props.centerXY.x - geow/2.0;
// 	_props.geoRc.y = _props.centerXY.y - geoh/2.0;
// 	_props.geoRc.width = geow;
// 	_props.geoRc.height = geoh;
}

swViewRect	swMapViewEnd::getViewRect(){
	return _props.viewRc;
}

/*
���õ������ĵ�λ��֮����ݵ�ǰ��ʾ�������Զ��������ӵ�������,�����������Ӵ���Ĵ�С
*/
void				swMapViewEnd::setCenterPoint(const swGeoPoint& point){
	_newCenterPt = point;
/*
	_props.geoRc.x += point.x - _props.centerXY.x;
	_props.geoRc.y += point.y - _props.centerXY.y;
	_props.centerXY =point;
	*/
}

swGeoPoint	swMapViewEnd::getCenterPoint(){
	return _props.centerXY;
}

//������ͼ��ĵ�������Ϊ���ĵ�
void				swMapViewEnd::setCenterPoint(const swViewPoint& point){
	swGeoPoint  gpt;
	gpt = convertPointView2Geo(point);
	setCenterPoint(gpt);
}

swViewPoint	swMapViewEnd::getViewCenterPoint(){
	return swViewPoint( _props.viewRc.x+_props.viewRc.width/2,_props.viewRc.y + _props.viewRc.height/2);
}

void				swMapViewEnd::setScaleLevel(swUInt8 scale){
	if( scale >= SWMAP_SCALELEVEL_NUM ){
		scale = SWMAP_SCALELEVEL_NUM-1;
	}
	_newScale = scale;
	/*
	if( _props.scale == scale){
		return;
	}
	_props.scale = scale;
	//��ǰ�������ĵ㲻�䣬�������������С
	float geow,geoh;
	swViewRect &rc = _props.viewRc;
	
	geow = (rc.width * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
	geoh = (rc.height * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
	_props.geoRc.x = _props.centerXY.x - geow/2.0;
	_props.geoRc.y = _props.centerXY.y - geoh/2.0;
	_props.geoRc.width = geow;
	_props.geoRc.height = geoh;
	*/
}

swUInt8			swMapViewEnd::getScaleLevel(){
	return _props.scale;
}

void				swMapViewEnd::setScaleLevelTable(const swFloat* units){
#if SW_RPC_ICE_CLIENT ==0
	_sview->setScaleLevelTable(units);
	memcpy(_props.scaleUnits,units,SWMAP_SCALELEVEL_NUM);	
#endif
}

swFloat*	swMapViewEnd::getScaleLevelTable(){
#if SW_RPC_ICE_CLIENT 
	return NULL;
#else
	return _sview->getProps().scaleUnits;
#endif
}

swGeoRect		swMapViewEnd::getGeoRect(){
	return _props.geoRc;
}


swMapViewProps&	swMapViewEnd::getProps(){
	return _props;
}

 


swFloat		swMapViewEnd::getCurScaleUnit(){
	return _props.scaleUnits[_props.scale];
}

bool	swMapViewEnd::create(swMapView* sview){
	_sview = sview;
	setBuffer(1);
	#if SW_RPC_ICE_CLIENT ==0
	if(_sview){
		_props = _sview->getProps();
	}
	#endif
	setCenterPoint(swGeoPoint(121.345f,31.45f))	;
	setScaleLevel(10);
	return true;
}


 
swMapLayerDrawPropT* swMapViewEnd::getLayerDrawProp(swUInt8 layerId,swUInt8 scale/*=CURRENT_SCALE*/){
	if( scale ==CURRENT_SCALE ){
		scale = _props.scale;
	}
	return &getLayerProps(layerId)->drawprops[scale];
}

void	swMapViewEnd::setLayerDrawProp(swUInt8 layerId,swUInt8 scale,const swMapLayerDrawPropT& drawprop){
	getLayerProps(layerId)->drawprops[scale] = drawprop;
}

void	swMapViewEnd::setBuffer(swUInt8 b){
	_buffer = b;
}

/*
�Ӵ������ĵ㿪ʼ���ĸ������ƽ������ݶ������ͼ������С������ռ�õ����������
���Ұ����ĸ�����Ļ�����������(buffer)
��ͼ���ĵ�λ����ͼ���������ĵ�
*/
/*
swViewSize swMapViewEnd::getSizeOfCahceDC(){
	swDrawSize size;
	swViewPoint cxy;
// 	cxy.x = _props.viewRc.width/2;
// 	cxy.y = _props.viewRc.height/2;
	size = _cachedc.GetSize();
	cxy.x = size.GetWidth()/2;
	cxy.y = size.GetHeight()/2;
	swUInt16 x_pad,y_pad;
	swUInt16 space;
	space = cxy.x - SWMAP_BITMAP_TILE_WIDTH/2;
	x_pad = space/SWMAP_BITMAP_TILE_WIDTH + 1 + _buffer;
	space = cxy.y - SWMAP_BITMAP_TILE_HEIGHT/2;
	y_pad = space/SWMAP_BITMAP_TILE_HEIGHT + 1 + _buffer;
	size.width = (x_pad*2+1) * SWMAP_BITMAP_TILE_WIDTH;
	size.height = (y_pad*2+1) * SWMAP_BITMAP_TILE_HEIGHT;
	return size;
}
*/
/*
�������������ʱ�Ž��е���ͼ������
dc = NULL �����ڲ�memdc
*/
void swMapViewEnd::render(swDC* dc /*=NULL*/){
	swViewPoint pt1,pt2;
	swMapBitmapTileListT tiles;
	swViewTileRectListT tilerects;
	swDrawSize size1,size2;
	swDrawSize size ;
	size = _cachedc.GetSize();
	if( (size.GetWidth()&size.GetHeight()) == 0){
		return ;
	}

	if( getScaleLevel() == _newScale && _props.centerXY == _newCenterPt && _rotate ==_newRotate){
		if( dc ){
			size1 = dc->GetSize();
			size2 = _cachedc.GetSize();
			dc->Blit(0,0,size1.GetWidth(),size1.GetHeight(),&_cachedc,
								(size2.GetWidth()-size1.GetWidth())/2,
								(size2.GetHeight()-size1.GetHeight())/2);
		}
		return ;
	}
	//�л����ż��𣬵�����ȫ��Ч������������tile���С�����������建�������С
	if( _props.scale!= _newScale || (_rotate!=_newRotate) ){
_INVALID_SCREEN:
		_rotate = _newRotate;
		_props.scale = _newScale;
		_props.centerXY = _newCenterPt;
		swViewTileRectT rect;
		rect.gcpt = _props.centerXY;
		
		size = _cachedc.GetSize();
		rect.destdc = &_cachedc;
		rect.destpt = swViewPoint(0,0);
		rect.rotate = _rotate;
		tilerects.push_back(rect);
#if SW_RPC_ICE_CLIENT
		_rpclient->getBitmapTiles(_mapid,_props.scale,size.GetWidth(),size.GetHeight(),tilerects,tiles);
#else
		_sview->getBitmapTiles(_props.scale,size.GetWidth(),size.GetHeight(),tilerects,tiles);
#endif
		//swMapBitmapTileT& btile = *tiles.begin();
		//_cachedc.Blit(0,0,size.GetWidth(),size.GetHeight(),btile.memdc,0,0); //�����ͼ�鸴�Ƶ������ڴ��
		//_cachedc.DrawBitmap(btile.bitmap,0,0);
		if( dc ){
			size1 = dc->GetSize();
			size2 = _cachedc.GetSize();
			dc->Blit(0,0,size1.GetWidth(),size1.GetHeight(),&_cachedc,
				(size2.GetWidth()-size1.GetWidth())/2,
				(size2.GetHeight()-size1.GetHeight())/2); //������ͼ�����ľ�������
		}
		float geow,geoh;
		size = _cachedc.GetSize();
		geow = (size.GetWidth() * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
		geoh = (size.GetHeight() * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; //������Ϊ���Ӵ�����=���ٵ���ȵ�λ
		_props.geoRc.x = _props.centerXY.x - geow/2.0;
		_props.geoRc.y = _props.centerXY.y - geoh/2.0;
		_props.geoRc.width = geow;
		_props.geoRc.height = geoh;

		return ;
	}
	//////////////////////////////////////////////////////////////////////////
	if( _rotate!=_newRotate){
		_rotate = _newRotate;
		goto _INVALID_SCREEN ;
	}
	//ͬ���ż��𻺳崦��
	pt1 = convertPointGeo2View(_newCenterPt);
	pt2 = convertPointGeo2View(_props.centerXY); 
	//��������֮λ��
	if(pt1 == pt2){
		return ;
	}
	swUInt32 xoff,yoff;
	swUInt32 rows,cols; //��Ҫ���µ�����
	xoff = abs(pt1.x-pt2.x); //�������ĵ������ƫ��
	yoff = abs(pt1.y-pt2.y);

	swViewSize dcsize;
	size = _cachedc.GetSize();
	dcsize.width = size.GetWidth();
	dcsize.height = size.GetHeight();

	//if( xoff>= dcsize || )
	//����ƫ�Ƶ�x,y�����Խ����������
	cols = xoff/SWMAP_BITMAP_TILE_WIDTH;
	rows = yoff/SWMAP_BITMAP_TILE_HEIGHT;
	if(xoff%SWMAP_BITMAP_TILE_WIDTH){
		cols++;
	}
	if(yoff%SWMAP_BITMAP_TILE_HEIGHT){
		rows++;
	}
	
	//�ƶ���λ�Ƴ�������Ļ�Ĵ�С(x��y����)��Ҫ������ˢ����������
	if(cols >= size.GetWidth()/SWMAP_BITMAP_TILE_WIDTH || rows >=size.GetHeight()/SWMAP_BITMAP_TILE_HEIGHT){
		goto _INVALID_SCREEN;
	}
	//copy dc from maindc to tmpdc
	//�����л����ĵ�������㵼�»���dcҪ�����У���ʧ�Ĳ���Ҫ���server���»���
	//��������ͼ�����Ϊ��λ
	_tmpdc.Blit(-(pt1.x-pt2.x),-(pt1.y-pt2.y),dcsize.width,dcsize.height,&_cachedc,0,0);
	//////////////////////////////////////////////////////////////////////////
	_props.geoRc.x += _newCenterPt.x - _props.centerXY.x;
	_props.geoRc.y += _newCenterPt.y - _props.centerXY.y;
	_props.centerXY =_newCenterPt;
	//
	//����Ҫ����µ�ͼ��
	swUInt32 tileRows,tileCols,r,c;
	tileRows = dcsize.height/SWMAP_BITMAP_TILE_HEIGHT;
	tileCols = dcsize.width / SWMAP_BITMAP_TILE_WIDTH;
	if( pt1.x < pt2.x){
		for(r=0;r<tileRows;r++){
			for(c=0;c<cols;c++){
				swViewTileRectT trc;
				trc.idx = swViewTileRectT::INDEX(c,r);
				swViewPoint vpt; //�������ĵ�
				vpt.x = c*SWMAP_BITMAP_TILE_WIDTH+ SWMAP_BITMAP_TILE_WIDTH/2;
				vpt.y = r*SWMAP_BITMAP_TILE_HEIGHT+SWMAP_BITMAP_TILE_HEIGHT/2;
				trc.gcpt = convertPointView2Geo(vpt); //����ĵ������ĵ�
				trc.destdc = &_tmpdc;
				trc.destpt.x = c*SWMAP_BITMAP_TILE_WIDTH;
				trc.destpt.y = r*SWMAP_BITMAP_TILE_HEIGHT;
				trc.rotate = _rotate;
				tilerects.push_back(trc);
			}
		}
		if( pt1.y < pt2.y){ //�󲿺��ϲ���������
			for(r=0;r<rows;r++){
				for(c=cols;c<tileCols;c++){
					swViewTileRectT trc;
					trc.idx = swViewTileRectT::INDEX(c,r);
					swViewPoint vpt; //�������ĵ�
					vpt.x = c*SWMAP_BITMAP_TILE_WIDTH+ SWMAP_BITMAP_TILE_WIDTH/2;
					vpt.y = r*SWMAP_BITMAP_TILE_HEIGHT+SWMAP_BITMAP_TILE_HEIGHT/2;
					trc.gcpt = convertPointView2Geo(vpt); //����ĵ������ĵ�
					trc.destdc = &_tmpdc;
					trc.destpt.x = c*SWMAP_BITMAP_TILE_WIDTH;
					trc.destpt.y = r*SWMAP_BITMAP_TILE_HEIGHT;
					trc.rotate = _rotate;
					tilerects.push_back(trc);
				}
			}	
		}else if(pt1.y > pt2.y){ //�������� �󲿺��²���������
			for(r=tileRows-rows;r<tileRows;r++){
				for(c=cols;c<tileCols;c++){
					swViewTileRectT trc;
					trc.idx = swViewTileRectT::INDEX(c,r);
					swViewPoint vpt; //�������ĵ�
					vpt.x = c*SWMAP_BITMAP_TILE_WIDTH+ SWMAP_BITMAP_TILE_WIDTH/2;
					vpt.y = r*SWMAP_BITMAP_TILE_HEIGHT+SWMAP_BITMAP_TILE_HEIGHT/2;
					trc.gcpt = convertPointView2Geo(vpt); //����ĵ������ĵ�
					trc.destdc = &_tmpdc;
					trc.destpt.x = c*SWMAP_BITMAP_TILE_WIDTH;
					trc.destpt.y = r*SWMAP_BITMAP_TILE_HEIGHT;
					trc.rotate = _rotate;
					tilerects.push_back(trc);
				}
			}	
		}//////////////////////////////////////////////////////////////////////////
	}else if(pt1.x > pt2.x){
		for(r=0;r<tileRows;r++){ //�Ҳഹֱ����
			for(c=tileCols-cols;c<tileCols;c++){
				swViewTileRectT trc;
				trc.idx = swViewTileRectT::INDEX(c,r);
				swViewPoint vpt; //�������ĵ�
				vpt.x = c*SWMAP_BITMAP_TILE_WIDTH+ SWMAP_BITMAP_TILE_WIDTH/2;
				vpt.y = r*SWMAP_BITMAP_TILE_HEIGHT+SWMAP_BITMAP_TILE_HEIGHT/2;
				trc.gcpt = convertPointView2Geo(vpt); //����ĵ������ĵ�
				trc.destdc = &_tmpdc;
				trc.destpt.x = c*SWMAP_BITMAP_TILE_WIDTH;
				trc.destpt.y = r*SWMAP_BITMAP_TILE_HEIGHT;
				trc.rotate = _rotate;
				tilerects.push_back(trc);
			}
		}
		if(pt1.y < pt2.y){
			for(r=0;r<rows;r++){
				for(c=0;c<tileCols-cols;c++){
					swViewTileRectT trc;
					trc.idx = swViewTileRectT::INDEX(c,r);
					swViewPoint vpt; //�������ĵ�
					vpt.x = c*SWMAP_BITMAP_TILE_WIDTH+ SWMAP_BITMAP_TILE_WIDTH/2;
					vpt.y = r*SWMAP_BITMAP_TILE_HEIGHT+SWMAP_BITMAP_TILE_HEIGHT/2;
					trc.gcpt = convertPointView2Geo(vpt); //����ĵ������ĵ�
					trc.destdc = &_tmpdc;
					trc.destpt.x = c*SWMAP_BITMAP_TILE_WIDTH;
					trc.destpt.y = r*SWMAP_BITMAP_TILE_HEIGHT;
					trc.rotate = _rotate;
					tilerects.push_back(trc);
				}
			}	
		}else if(pt1.y > pt2.y){
			for(r=tileRows-rows;r<tileRows;r++){
				for(c=0;c<tileCols-cols;c++){
					swViewTileRectT trc;
					trc.idx = swViewTileRectT::INDEX(c,r);
					swViewPoint vpt; //�������ĵ�
					vpt.x = c*SWMAP_BITMAP_TILE_WIDTH+ SWMAP_BITMAP_TILE_WIDTH/2;
					vpt.y = r*SWMAP_BITMAP_TILE_HEIGHT+SWMAP_BITMAP_TILE_HEIGHT/2;
					trc.gcpt = convertPointView2Geo(vpt); //����ĵ������ĵ�
					trc.destdc = &_tmpdc;
					trc.destpt.x = c*SWMAP_BITMAP_TILE_WIDTH;
					trc.destpt.y = r*SWMAP_BITMAP_TILE_HEIGHT;
					trc.rotate = _rotate;
					tilerects.push_back(trc);
				}
			}	
		}// else if(pt1.y > pt2.y){
	}
	//////////////////////////////////////////////////////////////////////////
	//request tile from server
	#if SW_RPC_ICE_CLIENT
		_rpclient->getBitmapTiles(_mapid,_props.scale,SWMAP_BITMAP_TILE_WIDTH,SWMAP_BITMAP_TILE_HEIGHT,tilerects,tiles);
	#else
	_sview->getBitmapTiles(_props.scale,SWMAP_BITMAP_TILE_WIDTH,SWMAP_BITMAP_TILE_HEIGHT,tilerects,tiles);
	#endif
	// fix dc to tmpdc , and copy tmpdc to cachedc,���Ƶ���ʱ�ڴ滺������
// 	swMapBitmapTileListT::iterator itr;
// 	for(itr=tiles.begin();itr!=tiles.end();itr++){
// 		wxString buff;
// 		//wxGetTempFileName(wxT("c:/"),buff);
// 		//itr->memdc->GetAsBitmap().SaveFile(buff,wxBITMAP_TYPE_JPEG);
// 		//_tmpdc.Blit( itr->idx.x*SWMAP_BITMAP_TILE_WIDTH,itr->idx.y*SWMAP_BITMAP_TILE_HEIGHT,SWMAP_BITMAP_TILE_WIDTH,SWMAP_BITMAP_TILE_HEIGHT,itr->memdc,0,0);
// 		_tmpdc.DrawBitmap(itr->bitmap,itr->idx.x*SWMAP_BITMAP_TILE_WIDTH,itr->idx.y*SWMAP_BITMAP_TILE_HEIGHT);
// 	}
	
	_cachedc.Blit(0,0,size.GetWidth(),size.GetHeight(),&_tmpdc,0,0);
	//������Ŀ���豸ȥ
	if(dc){
		size1 = dc->GetSize();
		size2 = _cachedc.GetSize();
		dc->Blit(0,0,size1.GetWidth(),size1.GetHeight(),&_cachedc,
			(size2.GetWidth()-size1.GetWidth())/2,
			(size2.GetHeight()-size1.GetHeight())/2);
	}
}				

void swMapViewEnd::sizeChanged(const swViewSize& vsize){
	swDrawSize size;
	size = _cachedc.GetSize();
	swViewSize realsize;
	int xlen,ylen;
	xlen = vsize.width/SWMAP_BITMAP_TILE_WIDTH;
	if( vsize.width%SWMAP_BITMAP_TILE_WIDTH){
		xlen++;
	} 
	ylen = vsize.height/SWMAP_BITMAP_TILE_HEIGHT;
	if(vsize.height%SWMAP_BITMAP_TILE_HEIGHT){
		ylen++;
	}
	realsize.width = xlen * SWMAP_BITMAP_TILE_WIDTH;
	realsize.height = ylen * SWMAP_BITMAP_TILE_HEIGHT;
	//////////////////////////////////////////////////////////////////////////
	if( realsize.width == size.GetWidth() && realsize.height == size.GetHeight() ){
		return;
	}
	if(_cacheBitmap){
		delete _cacheBitmap;
	}
	if(_tmpBitmap){
		delete _tmpBitmap;
	}
	_cacheBitmap = new swBitmap();
	_cacheBitmap->Create(realsize.width,realsize.height);
	_tmpBitmap = new swBitmap();
	_tmpBitmap->Create(realsize.width,realsize.height);
	_cachedc.SelectObject(*_cacheBitmap);
	_tmpdc.SelectObject(*_tmpBitmap);

	_props.viewRc.x=0;_props.viewRc.y=0;
	_props.viewRc.width= realsize.width;
	_props.viewRc.height = realsize.height;

 	float geow,geoh;
 	geow = (realsize.width * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE; 
 	geoh = (realsize.height * getCurScaleUnit() * 1.0)/sw_METERS_PER_DEGREE;
 	_props.geoRc.x = _props.centerXY.x - geow/2.0;
 	_props.geoRc.y = _props.centerXY.y - geoh/2.0;
 	_props.geoRc.width = geow;
 	_props.geoRc.height = geoh;
 	
 	 _props.scale = 0xf0;	//ǿ����������ȫ��ˢ��
}

swBitmap*		swMapViewEnd::getMemBitmap(){
	return _cacheBitmap;
}

void	swMapViewEnd::rotate(float radians){
	
	_newRotate = radians*M_PI/180;
}

#if SW_RPC_ICE_CLIENT
bool			swMapViewEnd::create(swmap::MapIdentT& mapid,swMapRpcClient* client){
	create(NULL);
	_mapid = mapid;
	_rpclient = client;
	try{
		swmap::MapScalePrecisionT scales;
		scales = _rpclient->getMapServer()->getScalePrecision(mapid);
		memcpy(_props.scaleUnits,&scales,sizeof(scales));
	}catch(...){
		;
	}
	return true;
}
#endif

