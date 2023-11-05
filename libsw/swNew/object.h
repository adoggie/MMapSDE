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
	//virtual bool 	within(const swGeoRect& rc);	//�����Ƿ���ȫ���ڵ���������
	//virtual bool 	intersects(const swGeoRect& rc); //����
	//swMapLayer*		getLayer();
	//void setLayer(swMapLayer*);
protected:
	swHeapString*		_name;  // �������serializeʱ������
	//swMapLayer*		_layer;		//�����Ĳ����
}; 

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/*
swHeapString
�����ڴ�ѿ���ַ���
�����С���ַ������������������ڴ���ڣ���Ҫ�������ڴ�ʱ�ҵ�һƬ�ʺϴ�С���ڴ��
�ַ�������swHeapString�ṹ��С��β��
swHeapString�����ַ�����С��ζ��������洢�ڵ������ڴ����
::ptrָ��swHeapString֮������ݿ�ʼλ��
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
//�ڴ���ֽ���,���δ֪���ݵ�����
struct swHeapByteStream:public swHeapObject{
	swHeapByteStream(const swUInt32 size );
	swUInt16 getPtrOffset();  //����ptr�����������ʼ��ַ��ƫ��
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
�ڴ�Ѷ��󹤳�
*/
class swHeapObjectFactory{
public:
	swHeapString* createString(const char * ptr,swUInt16 size);	
	void					freeObject(swHeapObject* obj);	//�ͷ�
	swGeoPOI*			createGeoPOI(); //����poi
	swGeoRoad*		createGeoRoad(swUInt16 num);
	swGeoPolygon*	createGeoPolygon(swUInt16 num);
	swGeometryMultiPoint* createMultiPoint(swUInt16 num);
	swMapLayer* createLayer(swUInt32 featureNum);   //����ͼ�����
	swHeapByteStream* createByteStream(swUInt32 size) ;//�ڴ������
	static swHeapObjectFactory _handle;
	static swHeapObjectFactory& instance();
};


//////////////////////////////////////////////////////////////////////////

///

#endif

