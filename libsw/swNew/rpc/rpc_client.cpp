#include "rpc_client.h"
#include "../view_end.h"

swMapRpcClient::swMapRpcClient(){

}

bool	swMapRpcClient::initRpc(swStringProps& props){
	wxInitAllImageHandlers();
	_communicator = Ice::initialize();
	try{
		_mapserver = swmap::IMapServerPrx::checkedCast(_communicator->stringToProxy(props["mapserver"]));
	}catch(...){
		return false;
	}		
	return true;
}

bool	swMapRpcClient::getBitmapTiles(swmap::MapIdentT& mapid,swUInt8 scale,
								swUInt16 tileWidth,swUInt16 tileHeight,const swViewTileRectListT rects,OUT swMapBitmapTileListT& tiles){
	tiles.clear();
	swmap::MapBitmapTileListT serverTiles;
	swmap::MapViewTileRectListT serverRects;
	
	swViewTileRectListT::const_iterator itr;

	for(itr=rects.begin();itr!=rects.end();itr++){ //依次请求不同的数据块
		swmap::MapViewTileRectT trc;
		serverRects.clear();
		trc.idx.x = itr->idx.x;
		trc.idx.y = itr->idx.y;
		trc.gcpt.x = itr->gcpt.x;
		trc.gcpt.y = itr->gcpt.y;
		trc.destpt.x = itr->destpt.x;
		trc.rotate = itr->rotate;
		trc.xscale = itr->xscale;
		trc.yscale = itr->yscale;
		trc.translate = itr->translate;
		serverRects.push_back(trc);
		try{
			serverTiles = _mapserver->getBitmapTiles(mapid,scale,tileWidth,tileHeight,serverRects);
			if( serverTiles.size()){
				 swmap::MapBitmapTileListT::iterator itrserv;
				 itrserv = serverTiles.begin();
				 swImage image;
				 image.Create(tileWidth,tileHeight);
				 swUInt32 size;
				 size = itrserv->bitmap.size();
				 
				 image.SetData(&itrserv->bitmap[0],true);
				 swBitmap bitmap(image);
				 swMemoryDC memdc;
				 memdc.SelectObject(bitmap);	
				 
				 itr->destdc->Blit(itr->destpt.x,itr->destpt.y,tileWidth,tileHeight,&memdc,0,0);
			}
		}catch(...){
		
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef SW_RPC_TEST_MAIN
int _tmain(int argc, _TCHAR* argv[]){
	swMapRpcClient client;
	swStringProps props;
	props["mapserver"]="swmapserver:tcp -h localhost -p 9090";
	
	if(!client.initRpc(props)){
		return -1;
	}

	swMapViewEnd vend;
	swmap::MapIdentT id;
	id.id = 0;
	vend.create(id,&client);
	
	vend.setScaleLevel(6);
		
	vend.sizeChanged(swViewSize(400,300));
	for(int n=0;n<1;n++){
		vend.setCenterPoint(swGeoPoint(121.41f+n/100.0,31.138f))	;	
	
		//vend.rotate(30);
		vend.render();
		vend.getMemBitmap()->SaveFile(swString::Format(swT("c:/rpc_out_%d.jpg"),n),wxBITMAP_TYPE_JPEG);
	}
	return 0;
}

#endif