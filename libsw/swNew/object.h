#ifndef _SW_OBJECT_H
#define _SW_OBJECT_H

#include "base.h"
#include "memheap/heap.h"


class swObject{
public:
	void		setData(void*);
	void*		getData();
protected:
	void *				_data;
	swStringProps	_strhash;	
};

class swHeapString;
class swMapRender;
class swMapView;
class swMapLayer;

//////////////////////////////////////////////////////////////////////////
class swGeoObject:public swHeapObject{
public:
	swGeoObject(){ _name = NULL;}
	virtual bool operator<<(swByteStream&){ return false;}
	virtual void destroy(){}
	virtual void render(swMapRender* r,swUInt8 layerId){}
	//virtual bool 	within(const swGeoRect& rc);	//对象是否完全落在地理区域内
	//virtual bool 	intersects(const swGeoRect& rc); //对象
	//swMapLayer*		getLayer();
	//void setLayer(swMapLayer*);
protected:
	swHeapString*		_name;  // 子类对象serialize时被构建
	//swMapLayer*		_layer;		//隶属的层对象
}; 

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/*
swHeapString
基于内存堆块的字符串
对象大小与字符串被放置在连续的内存块内，需要在申请内存时找到一片适合大小的内存块
字符串排在swHeapString结构大小的尾部
swHeapString不管字符串大小如何都被独立存储在单独的内存块内
::ptr指向swHeapString之后的数据开始位置
*/
class swHeapString:public swHeapObject{
	swHeapString();
	friend class swHeapObjectFactory;
public:
	swUInt16		size();
	char*				data();
// 	void	setSize(swUInt16 size){
// 		_size;
// 	}
// 	void	setData(char* ptr){
// 		_ptr = ptr;
// 	}
	void destroy(){};  
private:
	swUInt16	_size;
	char*			_ptr;
};

inline swHeapString::swHeapString(){
	_size = 0;
	_ptr = NULL;
}

inline swUInt16		swHeapString::size(){
	return _size;
}
inline char*				swHeapString::data(){
	return _ptr;
}
//////////////////////////////////////////////////////////////////////////
//内存堆字节流,存放未知内容的数据
struct swHeapByteStream:public swHeapObject{
	swHeapByteStream(const swUInt32 size );
	swUInt16 getPtrOffset();  //计算ptr数据离对象起始地址的偏移
	swUInt32	size;
	swByte*		ptr;
};

//////////////////////////////////////////////////////////////////////////
class swGeoPOI;
class swGeoRoad;
class swGeoPolygon;
class swGeometryMultiPoint;
class swMapLayer;
/*
swHeapObjectFactory
内存堆对象工厂
*/
class swHeapObjectFactory{
public:
	swHeapString* createString(const char * ptr,swUInt16 size);	
	void					freeObject(swHeapObject* obj);	//释放
	swGeoPOI*			createGeoPOI(); //创建poi
	swGeoRoad*		createGeoRoad(swUInt16 num);
	swGeoPolygon*	createGeoPolygon(swUInt16 num);
	swGeometryMultiPoint* createMultiPoint(swUInt16 num);
	swMapLayer* createLayer(swUInt32 featureNum);   //创建图层对象
	swHeapByteStream* createByteStream(swUInt32 size) ;//内存块申请
	static swHeapObjectFactory _handle;
	static swHeapObjectFactory& instance();
};


//////////////////////////////////////////////////////////////////////////

///

#endif

