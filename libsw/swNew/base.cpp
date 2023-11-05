#include "base.h"
#include "memheap/memheap.h"
#ifdef _WIN32
	
#include <Winsock.h>
#pragma comment(lib,"ws2_32.lib")

swUInt16 swN2H16(swUInt16 x){
	return (swUInt16)ntohs(u_short(x));
}

swUInt32 swN2H32(swUInt32 x){
	return (swUInt32)ntohl(u_long(x));
};


#endif

swByteStream::swByteStream(bool bigedian /*= true*/){
	_be = bigedian;
	_ptr =NULL;
	_curr = NULL;
	_size = 0;
}

swByteStream::swByteStream(swByte* start,swUInt32 size,bool bigedian /*= true*/){
	_size = size;
	_be = bigedian;
	_ptr = _curr = start;
}

swByte*		swByteStream::begin(){
		return _ptr;
}

/*
forward()
越界返回NULL
*/
swByte*		swByteStream::forward(swUInt32 n){
// 	if( _curr-_ptr+n > _size ){
// 		return NULL;
// 	}
	_curr+=n;
	return _curr;
}

/*
back()
越界返回首部地址
*/
swByte*		swByteStream::back(swUInt32 n){
	if( _curr-n <_ptr){
		return _ptr;
	}
	_curr-=n;
	return _curr;
}

/*
rewind()
重新绕回开始位置
*/
swByte*		swByteStream::rewind(){ //

	return _curr = _ptr;
}
	
swUInt32	swByteStream::size(){
		return _size;
}

void			swByteStream::reset(){
	_ptr = _curr = NULL;
	_size = 0;
}
	
void			swByteStream::setCurrent(swByte* cr){
	_curr = cr;
}
	
swByte*		swByteStream::current(){
	return _curr;
}

bool			swByteStream::isEOF(){
	if( _curr-_ptr>= _size){
		return true;
	}
	return false;
}

swUInt32	swByteStream::leftBytes(){
	return _size - (_curr - _ptr);
}

bool			swByteStream::readValue(swUInt32& val,bool f){
	val = (swUInt32)swN2H32(*(swUInt32*)_curr);
	if(f){
		forward(4);
	}
	return true;
}

bool			swByteStream::readValue(swUInt16& val,bool f){
	val = (swUInt16)swN2H16(*(swUInt16*)_curr);
	if(f){
		forward(2);
	}
	return true;
}

bool			swByteStream::readValue(swUInt8& val,bool f){
	val = *(swUInt8*)_curr;
	if(f){
		forward(1);
	}
	return true;
}
bool			swByteStream::readValue(swFloat& val,bool f){
	swUInt32 u32v;
	readValue(u32v,f);
	//val =(swFloat)u32v;
	memcpy(&val,&u32v,4);
	return true;
}
bool			swByteStream::readValue(swDouble& val,bool f){
	return false;
}
bool			swByteStream::readValue(swGeoRect&val,bool f){
	readValue(val.x,f);
	readValue(val.y,f);
	readValue(val.width,f);
	readValue(val.height,f);
	return true;
}
bool			swByteStream::readValue(swGeoPoint&val,bool f){
	readValue(val.x,f);
	readValue(val.y,f);
	return true;
}
bool			swByteStream::readValue(swGeoSize&val,bool f){
	readValue(val.width,f);
	readValue(val.height,f);
	return true;
}
bool			swByteStream::readString(swString& val,bool f){
	swUInt8 len;
	readValue(len,f);
	if(len){
		val = swString::FromUTF8( (char*)_curr,len);
		forward(len);
	}
	return true;
	
}

bool			swByteStream::readString(std::string& val,bool f){
	swUInt8 len;
	readValue(len,f);
	if(len){
		val.append((char*)_curr,len);
		forward(len);
	}
	return true;
}

void swByteStream::operator =(const swByteStream& stream){
	reset();
	_ptr = stream._ptr;
	_size = stream._size;
	_curr = stream._curr;
	_be = stream._be;
}

//////////////////////////////////////////////////////////////////////////

bool			swReadValue(swFile* file,swUInt32& val){
	if( file->Read(&val,4) !=4){
		return false;
	}
	val = swN2H32(val);
	return true;
}
bool			swReadValue(swFile* file,swUInt16& val){
	if( file->Read(&val,2) !=2){
		return false;
	}
	val = swN2H16(val);
	return true;
}
bool			swReadValue(swFile* file,swUInt8& val){
	if( file->Read(&val,1) !=1){
		return false;
	}
	return true;
}
bool			swReadValue(swFile* file,swFloat& val){
	swUInt32 u32v;
	if(!swReadValue(file,u32v)){
		return false;
	}
	memcpy(&val,&u32v,4);
	return true;
}

bool			swReadValue(swFile* file,swDouble& val){
	return false;
}

bool			swReadValue(swFile* file,swGeoRect&val){
	if( !swReadValue(file,val.x) || !swReadValue(file,val.y) || !swReadValue(file,val.width) || !swReadValue(file,val.height)){
		return false;
	}
	return true;

}
bool			swReadValue(swFile* file,swGeoPoint&val){
	if( !swReadValue(file,val.x) || !swReadValue(file,val.y) ){
		return false;
	}
	return true;

}
bool			swReadValue(swFile* file,swGeoSize&val){
	if( !swReadValue(file,val.width) || !swReadValue(file,val.height)){
		return false;
	}
	return true;

}
bool			swReadString(swFile* file,swString& val){
	swUInt8 len;
	swReadValue(file,len);
	if(len){
		swMemBlockID mbid;
		char* str = (char*)swMemHeapEngine::instance()->malloc(len,mbid);
		if(str){
			val = swString::FromUTF8(str,len);
			swMemHeapEngine::instance()->free(mbid);
		}
	}
	return true;
}

bool			swReadString(swFile* file,std::string& val){
	swUInt8 len;
	swReadValue(file,len);
	if(len){
		swMemBlockID mbid;
		char* str = (char*)swMemHeapEngine::instance()->malloc(len,mbid);
		if(str){
			val.append(str,len);
			swMemHeapEngine::instance()->free(mbid);
		}
	}
	return true;
}

