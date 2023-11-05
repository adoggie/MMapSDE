
#include "bucket.h"


swMemBucket::swMemBucket(swMemBucketSet* set){
	_memstart = NULL;
	_set = set;
}

swMemBucket::~swMemBucket(){
	if(_memstart){
		delete []_memstart;
	}
}

bool	swMemBucket::create(){
	int size ;
	size = _set->getBucketSize()/_set->getBucketBlockSize(); // /8;
	_freenum = size;
	if(size<8){
		size = 1;
	}else{
		size = size/8;
		if( size%8){
			size++;
		}
	}
	_bitmap.resize(size);
//	_bitmap = new char[size];
	memset(&_bitmap[0],0,size);
	_memstart = new char[_set->getBucketSize()];
	memset(_memstart,0,_set->getBucketSize());
	
	_id = swMemHeapEngine::instance()->newBucketId();
	swMemHeapEngine::instance()->regBucket(this);
	return _memstart?true:false;
}

void* swMemBucket::ptr(swUInt16 idx){
	if( idx * _set->getBucketBlockSize() >= _set->getBucketSize()){
		return NULL;
	}
	return _memstart+idx*_set->getBucketBlockSize();
}

void * swMemBucket::malloc(swMemBlockID& bid){
	bid.bucketId = swMemBucketId_Null;
	
	int n;
	if(_freenum ==0){
		return NULL;
	}
	
	for(n=0;n<_bitmap.size();n++){
		if( _bitmap[n] == 0xff){
			continue; //ignore
		}
		for(int b=0;b<8;b++){
			if( (_bitmap[n] & (1 <<b)) == 0){
				bid.idx =n*8+b;
				_bitmap[n] |= 1<<b; //标识此位非0，内存块被占用
				_freenum --;
				bid.bucketId = _id;
				return ptr(bid.idx);
			}
		}
	}
	return NULL;
}

void swMemBucket::free(swMemBlockID& bid){
	if(bid.bucketId == swMemBucketId_Null){
		return;
	}
	if( bid.idx * _set->getBucketBlockSize() >= _set->getBucketSize()){
		return ;
	}
	//标志位清0
	swByte& b = _bitmap[bid.idx/8];
	b&=~(1<<(bid.idx%8)); // that's ok, clear that bit
	_freenum++;
}

swUInt32 swMemBucket::getFreeNum(){
	return _freenum;
}

swMemBucketID	 swMemBucket::getId(){
	return _id;
}

swUInt32 swMemBucket::getUsedNum(){
	return _set->getBucketSize()/_set->getBucketBlockSize() - _freenum;
}

//////////////////////////////////////////////////////////////////////////


swMemBucketSet::swMemBucketSet(swMemBlockSizeT blocksize ,swMemBucketSizeT bucketsize/*=BUCKET_1M*/){
	_blocksize = blocksize;
	_bucketsize = bucketsize;
}

bool	swMemBucketSet::create(swUInt16 bucketnum){
	swMemBucket * b;
	for(swUInt16 n=0;n<bucketnum;n++){
		b = new swMemBucket(this);
		b->create();
		_buckets[b->getId()] = b;
	}
	return true;
}

swMemBucketSizeT		swMemBucketSet::getBucketSize(){
	return _bucketsize;
}

swMemBlockSizeT		swMemBucketSet::getBucketBlockSize(){
	return _blocksize;
}

swMemBucket*	swMemBucketSet::createBucket(){
	swMemBucket* b = new swMemBucket(this);
	b->create();
	_buckets[b->getId()] = b;
	return b;
}

swUInt16	swMemBucketSet::getBucketNum(){
	return (swUInt16)_buckets.size();
}

/*
malloc()
到空闲桶里面分配内存块
*/
void* swMemBucketSet::malloc(swMemBlockID& bid){
	void * r = NULL;
	std::map<swMemBucketID,swMemBucket*>::iterator itr;
	for(itr=_buckets.begin();itr!=_buckets.end();itr++){
		r = itr->second->malloc(bid);
		if(r){
			break;
		}
	}
	return r;
}

swUInt32	swMemBucketSet::getUsedBlockNum(){
	swUInt32 r=0;
	std::map<swMemBucketID,swMemBucket*>::iterator itr;
	for(itr = _buckets.begin();itr!=_buckets.end();itr++){
		r+=itr->second->getUsedNum();
	}
	return r;
} 