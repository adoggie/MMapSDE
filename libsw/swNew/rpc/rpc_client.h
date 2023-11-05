#ifndef _RPC_CLIENT_H
#define _RPC_CLIENT_H


#include <Ice/Ice.h>
#include "./ice/mapserver.h"
#include "../base.h"
#include "../tile.h"

class swMapRpcClient{
public:
	swMapRpcClient();
	bool	initRpc(swStringProps& props);
	swmap::IMapServerPrx getMapServer();
	bool	getBitmapTiles(swmap::MapIdentT& mapid, swUInt8 scale,swUInt16 tileWidth,swUInt16 tileHeight,const swViewTileRectListT rects,OUT swMapBitmapTileListT& tiles); //«Î«ÛÕ¯∏ÒøÈ‰÷»æ

private:	
	Ice::CommunicatorPtr _communicator;
	swmap::IMapServerPrx	_mapserver;
};

inline 
swmap::IMapServerPrx swMapRpcClient::getMapServer(){
	return _mapserver;
}

#endif
