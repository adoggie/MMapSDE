
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


//���������ò���
struct swMapServerPropsT{
	int		viewEvictTime;	//��ͼ������ʱ��Ч��ʱ����
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
	bool	init( swStringProps& );			//��ʼ��������
	static swMapServer*		instance();
	swMapView* createView(const swMapIdentT& mapid);  //�򿪵�ͼ��ͼ

	bool	initMaps();	//��ʼ����ͼ
	bool	initComm();	//��ʼ��ͨ����ʩ
	void	dbgMsg(const swString& msg,int type = DBG_MESSAGE,int err=0);
	void	dbgMsg2(int type = DBG_MESSAGE,int err=0);
	void addLogStream(shared_ptr<swLogOutputStream>& los);	
	swMapServerPropsT&	getProps();
	
private:
	swMutex							_viewsMutex;
	std::list<swMapView*>		_viewList;
	
	static shared_ptr<swMapServer> 		_handle;
	std::map<swMapIdentT,swMap*>				_maps;		//��ͼ����
	swStringProps			_sprops;	//
	swLogOutputStreamList	_logstreams;
	swString		_servId;	//���������
	swString		_servIce;	//ice ������Ϣ
	swString		_mappath;	//��ͼ�洢·��
	swViewEvictor	_viewEvict;
	swMapServerPropsT	_props;
};


inline 
swMapServerPropsT&	swMapServer::getProps(){
	return _props;
}

#endif
