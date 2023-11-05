#include "server.h"
#include "util.h"

/*
props:
	confile	配置文件路径
			
*/
bool	swMapServer::init( swStringProps& props){
	swStringProps::const_iterator itr;
	itr = props.find("confile"); 
	if(  itr ==  props.end()){
		return false;		
	}
	
	wxInitAllImageHandlers();
	swMemHeapConfigT heapcfg;
	//swMemHeapProfileDescT memprof;
	swMemHeapEngine::instance()->init(heapcfg);
	swMemHeapEngine::instance()->setLogger(swMapServer::instance());
	//日志输出	
	swMapServer::instance()->addLogStream( shared_ptr<swLogOutputStream>(new swLogStdoutStream()));
	
	
	swXmlDocument xmldoc;	 
	if( !xmldoc.Load(swString::FromUTF8(props["confile"].c_str()))){
		return false;
	}
	swXmlNode * node;
	node = xmldoc.GetRoot();
	if(node->GetName()!= swT("mapserver")) {
		return false;
	}
	
	std::map<swString ,swString> attrs;
	attrs = swUtil_getXmlNodeProperties(node);
	swUtil_getDictStringValue(&attrs,swT("id"),_servId);
	swUtil_getDictStringValue(&attrs,swT("ice"),_servIce);
	//日志输出
	swString logfile;
	if( swUtil_getDictStringValue(&attrs,swT("logfile"),logfile) ){
		swMapServer::instance()->addLogStream( shared_ptr<swLogOutputStream>(new swLogFileStream(logfile)));
	}
	//////////////////////////////////////////////////////////////////////////
	swXmlNode * n1,* n2;
	n1 = node->GetChildren();
	while(n1){
		if( n1->GetName() == swT("maps")){
			attrs = swUtil_getXmlNodeProperties(n1);
			swUtil_getDictStringValue(&attrs,swT("path"),_mappath);
			n2 = n1->GetChildren();			
			while(n2){
				if(n2->GetName()==swT("map")){
					swString map_name,map_id,map_file;
					attrs = swUtil_getXmlNodeProperties(n2);
					swUtil_getDictStringValue(&attrs,swT("name"),map_name);
					swUtil_getDictStringValue(&attrs,swT("id"),map_id);
					swUtil_getDictStringValue(&attrs,swT("file"),map_file);
					swMap *map = new swMap();	
					if( map->create(_mappath+swT("/")+map_file) ){
						_maps[map->getProps().id] = map;
					}else{
						dbgMsg(swT("init map fail!"),DBG_ERROR);
						return false;		
					}
				}
				n2 = n2->GetNext();
			}
		}else if(n1->GetName() ==swT("memheap")){
			n2 = n1->GetChildren();
			while(n2){
				if( n2->GetName() == swT("bucketset")){
						attrs = swUtil_getXmlNodeProperties(n2);
						swString blocksize,bucketnum,bucketsize;
						swUtil_getDictStringValue(&attrs,swT("blocksize"),blocksize);
						swUtil_getDictStringValue(&attrs,swT("bucketnum"),bucketnum);
						swUtil_getDictStringValue(&attrs,swT("bucketsize"),bucketsize);
						swUInt32 iblocksize,ibucketnum,ibucketsize;
						ibucketnum =		atoi(bucketnum.c_str());
						iblocksize = 1 << atoi(blocksize.c_str());
						ibucketsize = (1<<20) * atoi(bucketsize.c_str());
						if(!swMemHeapEngine::instance()->createBucketSet(ibucketnum,(swMemBlockSizeT)iblocksize,(swMemBucketSizeT)ibucketsize)){
							dbgMsg(swT("init memheap bucket fail!"),DBG_ERROR);
							return false;
						}
				}
				n2 = n2->GetNext();			
			}			
		}
		
		n1 = n1->GetNext();
	}
	//////////////////////////////////////////////////////////////////////////
	#if SW_RPC_ICE_ENABLE
	_props.viewEvictTime = 60;	// 20秒接收不到心跳包视为view客户端撤销，服务端将删除此view servant对象
	//_viewEvict.Create();  // wxwidgets 的wxThread 千万不能使用
	//_viewEvict.Run();
	
	#endif
	
	return true;
}

swMapServer::swMapServer():_viewEvict(this){

}

swMapServer*		swMapServer::instance(){
	if( _handle.get() ==  NULL){
		_handle = shared_ptr<swMapServer>(new swMapServer());
	}
	return _handle.get();
}

shared_ptr<swMapServer> 		swMapServer::_handle;

swMapView* swMapServer::createView(const swMapIdentT& mapid){
	swMapView * view;
	swMap * map = NULL;
	std::map<swMapIdentT,swMap*>::iterator itr;
	itr=_maps.find(mapid);
	if(  itr== _maps.end()){
		return NULL;
	}
	map = itr->second;
	view = new swMapView(this);
	view->create(map);
	return view;
}

/*
初始化地图数据并登记注册
*/
bool	swMapServer::initMaps(){
	swMap * map;
	map = new swMap();
	//if( map->create(swT("E:/sw2us/maps/gis_map_make/final/china.gis")) ){
	if( map->create(swT("E:/map_build/final/china.gis")) ){
		_maps[map->getProps().id] = map;
	}
	return true;;	

}

bool	swMapServer::initComm(){

	return true;
}

void swMapServer::addLogStream(shared_ptr<swLogOutputStream>& los){
	_logstreams.push_back(los);	
}

void	swMapServer::dbgMsg(const swString& msg,int type,int err){
	swString text;
	swString msgtype;
	switch(type){
		case DBG_ERROR: msgtype="ERR";break;
		case DBG_WARNING: msgtype="WARN";break;
		case DBG_DEBUG: msgtype="DBG";break;
		case DBG_MESSAGE: msgtype="MSG";break;				
	}
	swDateTime now = swDateTime::Now();
	text = swString::Format(swT("%s %s %s %d %s"),now.FormatISODate(),now.FormatISOTime(),msgtype,err,msg);
	swLogOutputStreamList::iterator itr;
	for(itr=_logstreams.begin();itr!=_logstreams.end();itr++){
		(*itr)->writeln(text.ToUTF8().data());
	}		
}

void	swMapServer::dbgMsg2(int type,int err){
	dbgMsg(swT(""),type,err);
}

//////////////////////////////////////////////////////////////////////////
#if SW_RPC_ICE_ENABLE
swmap::IMapViewPtr swMapServer::createView(const swmap::MapIdentT& mapid ,const Ice::Current&  ctx){
	swmap::IMapViewPtr view;
	swMapIdentT mapid_dest;
	mapid_dest = mapid.id;
	view = createView(mapid_dest);
	if(view){
		swmap::IMapViewPrx proxy = swmap::IMapViewPrx::uncheckedCast(MapRpcServer::instance()->getAdapter()->addWithUUID(view));
		swMapView* mview = (swMapView*)view.get();
		mview->setId(proxy->ice_getIdentity());
		swScopeLock l(_viewsMutex);
		_viewList.push_back(mview); //加入缓存队列
	}
	return view;
}

Ice::Int swMapServer::getTimestamp( const Ice::Current&  ctx){
	return 0;
}

//检测无效的view会话，从通信适配器中删除
void swMapServer::evictView(){
	swScopeLock l(_viewsMutex);

	std::list<swMapView*>::iterator itr;
	itr = _viewList.begin();
	while( itr!=_viewList.end()){
		if( (*itr)->isInvalid() ){
			std::list<swMapView*>::iterator  tmp;
			tmp = itr;
			itr++;
			_viewList.erase(tmp);
			//////////////////////////////////////////////////////////////////////////
			//将view servant从adapter中删除
			Ice::Identity id;
			id = (*tmp)->getId();
			MapRpcServer::instance()->getAdapter()->remove(id);
			//////////////////////////////////////////////////////////////////////////
			continue;
		}
		itr++;
	}
}

swmap::MapBitmapTileListT	swMapServer::getBitmapTiles(const swmap::MapIdentT &mapid,Ice::Int scale,Ice::Int tileWidth,Ice::Int tileHeight,const swmap::MapViewTileRectListT& rects,const Ice::Current&  ctx){
	swmap::MapBitmapTileListT tiles;
	swViewTileRectListT rects2;
	swMapBitmapTileListT tiles2;
	swmap::MapViewTileRectListT::const_iterator itr;
	swMapView*	mapview;	
	swMapIdentT mapid_dest;
	mapid_dest = mapid.id;
	mapview = createView(mapid_dest);
	if(!mapview){
		return tiles;
	}	
	
	for(itr=rects.begin();itr!=rects.end();itr++){
		swBitmap bitmap;
		swMemoryDC memdc;
		bitmap.Create(tileWidth,tileHeight);
		memdc.SelectObject(bitmap);
		swViewTileRectT trc;
		trc.gcpt.x = itr->gcpt.x;
		trc.gcpt.y = itr->gcpt.y;
		trc.idx.x = itr->idx.x;
		trc.idx.y = itr->idx.y;
		trc.destdc = &memdc;
		trc.destpt.x = 0;
		trc.destpt.y = 0;
		trc.rotate = itr->rotate;
		trc.xscale = itr->xscale;
		trc.yscale = itr->yscale;
		trc.translate = itr->translate;
		rects2.clear();
		rects2.push_back(trc);
		tiles2.clear();
		mapview->getBitmapTiles(scale,tileWidth,tileHeight,rects2,tiles2);
		{			
			swmap::MapBitmapTileT btile;			 
			btile.width = tileWidth;
			btile.height = tileHeight;
			btile.idx.x = itr->idx.x;
			btile.idx.y = itr->idx.y;
			swImage	image;//(bitmap);
			swUInt32 size;
			image = bitmap.ConvertToImage();
			size = image.GetWidth()*image.GetHeight()* 3;//bitmap.GetDepth()/8;
			btile.bitmap.assign(image.GetData(),image.GetData()+size);
			
// 			swImage image2;//(tileWidth,tileHeight,&btile.bitmap[0],true);
// 			image2.Create(tileWidth,tileHeight);
// 			image2.SetData(&btile.bitmap[0],true);
// 			unsigned char* ptr = &btile.bitmap[0];
// 			if(image2.IsOk()){
// 				btile.idx.x =0;
// 				//bitmap.GetGDIImageData()->m_hBitmap.
// 				swBitmap bitmap2(image2);
// 				bitmap2.SaveFile("c:/dump10.jpg",wxBITMAP_TYPE_JPEG)	;
// 			}		
			tiles.push_back(btile);	 
		}	 		 
	}	
	delete mapview;		
	return tiles;
}


swmap::MapScalePrecisionT swMapServer::getScalePrecision(const swmap::MapIdentT& mapid, const ::Ice::Current& ){
	swMap * map = NULL;
	std::map<swMapIdentT,swMap*>::iterator itr;
	swmap::MapScalePrecisionT scales;
	itr=_maps.find(mapid.id);
	if(  itr== _maps.end()){
		return scales;
	}
	map = itr->second;
	memcpy(&scales,map->getProps().scaleUnits,sizeof(scales));
	return scales;
}

#endif


//////////////////////////////////////////////////////////////////////////
swMapServer::swViewEvictor::swViewEvictor(swMapServer* server){
	_server = server;
	_launched = false;
}

void* swMapServer::swViewEvictor::Entry(){
	_launched = true;
	while(_launched){
		#if SW_RPC_ICE_ENABLE
		_server->evictView();
		#endif
		swSleep(1000);
	}	
	_launched = false;
	return NULL;
}