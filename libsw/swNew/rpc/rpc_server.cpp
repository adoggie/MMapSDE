#include "rpc_server.h"
#if SW_RPC_ICE_ENABLE

int MapRpcServer::run(int, char*[]){
	swStringProps serverprops;
	serverprops["confile"] = "mapserv.xml";
	
	if( !swMapServer::instance()->init(serverprops)){
		return -1;
	}
	_adapter = communicator()->createObjectAdapter("MAPSERVER");
	//_adapter = communicator()->createObjectAdapterWithEndpoints("mapserver","tcp -h localhost -p 9090");
	swmap::IMapServerPtr mapserver = swMapServer::instance();
	_adapter->add(mapserver,communicator()->stringToIdentity("swmapserver"));
	_adapter->activate();
	swMapServer::instance()->dbgMsg(swT("mapserver Rpc started!"));
	communicator()->waitForShutdown();
	return 0;
}

MapRpcServer*		MapRpcServer::instance(){
	if( _handle.get() ==  NULL){
		_handle = shared_ptr<MapRpcServer>(new MapRpcServer());
	}
	return _handle.get();
}

shared_ptr<MapRpcServer> 		MapRpcServer::_handle;

Ice::ObjectAdapterPtr		 MapRpcServer::getAdapter(){
	return _adapter;
}

//////////////////////////////////////////////////////////////////////////



int _tmain(int argc, _TCHAR* argv[]){
	
	return MapRpcServer::instance()->main(argc, argv, "mapserv.conf");
}
#endif




