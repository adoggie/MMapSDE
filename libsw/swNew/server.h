
#ifndef _SW_SERVER_H
#define _SW_SERVER_H

#include "base.h"
#include "util.h"
#include "object.h"
#include "view.h"
#include "map.h"
#include "dbglog.h"
#include <boost/shared_ptr.hpp>
using namespace  boost;


//服务器配置参数
struct swMapServerPropsT{
	int		viewEvictTime;	//视图心跳超时无效的时间间隔
};

#if SW_RPC_ICE_ENABLE

#include "rpc/ice/mapserver.h"
#include "rpc/rpc_server.h"


class swMapServer:public swObject,public swLogger,public swmap::IMapServer{
public:
	swmap::IMapViewPtr createView(const swmap::MapIdentT&, const Ice::Current&  ctx);
	Ice::Int getTimestamp( const Ice::Current&  ctx);
	swmap::MapBitmapTileListT	getBitmapTiles(const swmap::MapIdentT &mapid,Ice::Int scale,Ice::Int tileWidth,Ice::Int tileHeight,const swmap::MapViewTileRectListT& rects,const Ice::Current&  ctx);	
	swmap::MapScalePrecisionT getScalePrecision(const swmap::MapIdentT&, const ::Ice::Current& = ::Ice::Current());
public:
	void evictView();
private:

//////////////////////////////////////////////////////////////////////////

	
#else
class swMapServer:public swObject,public swLogger{
#endif



	class swViewEvictor:public swThread{
	public:
		swViewEvictor(swMapServer* server);
		void* Entry();	
		void terminate();
	private:
		swMapServer*	_server;
		bool		_launched;

	};
public:
 swMapServer();
	//shared_ptr<swMapView>	createView()
	bool	init( swStringProps& );			//初始化服务器
	static swMapServer*		instance();
	swMapView* createView(const swMapIdentT& mapid);  //打开地图视图

	bool	initMaps();	//初始化地图
	bool	initComm();	//初始化通信设施
	void	dbgMsg(const swString& msg,int type = DBG_MESSAGE,int err=0);
	void	dbgMsg2(int type = DBG_MESSAGE,int err=0);
	void addLogStream(shared_ptr<swLogOutputStream>& los);	
	swMapServerPropsT&	getProps();
	
private:
	swMutex							_viewsMutex;
	std::list<swMapView*>		_viewList;
	
	static shared_ptr<swMapServer> 		_handle;
	std::map<swMapIdentT,swMap*>				_maps;		//地图对象
	swStringProps			_sprops;	//
	swLogOutputStreamList	_logstreams;
	swString		_servId;	//服务器编号
	swString		_servIce;	//ice 配置信息
	swString		_mappath;	//地图存储路径
	swViewEvictor	_viewEvict;
	swMapServerPropsT	_props;
};


inline 
swMapServerPropsT&	swMapServer::getProps(){
	return _props;
}

#endif
