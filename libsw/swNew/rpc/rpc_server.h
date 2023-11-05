
#ifndef _RPC_SERVER_H
#define _RPC_SERVER_H

#include <Ice/Ice.h>
#include "../server.h"
#include "./ice/mapserver.h"


class MapRpcServer : public Ice::Application{
public:
	static MapRpcServer*		instance();
	int run(int, char*[]);
	Ice::ObjectAdapterPtr		 getAdapter();
private:
	static shared_ptr<MapRpcServer> 		_handle;
	Ice::ObjectAdapterPtr _adapter;
};


#endif

