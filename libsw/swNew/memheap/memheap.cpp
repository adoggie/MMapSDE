#include "memheap.h"
#include "bucket.h"


bool	swMemHeapEngine::init(swMemHeapConfigT& config){
	//_buckset_list
	_logger = NULL;
	return true;
}


swMemHeapEngine::~swMemHeapEngine(){
	std::list<swMemBucketSet*>::iterator itr;
	for(itr = _buckset_list.begin();itr!=_buckset_list.end();itr++){
		swMemBucketSet* bset;
		bset = *itr;
		delete bset;
	}
}

swMemHeapEngine* swMemHeapEngine::instance(){
	if(_handle.get() ==  NULL){
		_handle = shared_ptr<swMemHeapEngine>(new swMemHeapEngine() );
	}
	return _handle.get();
}

void* swMemHeapEngine::malloc(swUInt32 objsize,swMemBlockID&mbid){
	void* r = NULL;
	//std::map<swMemBlockSizeT,swMemBucketSet*>::iterator itr;
	swMemBlockSizeT bsize = MBS_MAX;
	std::list<swMemBucketSet*>::iterator itr;
	for(itr=_buckset_list.begin();itr!=_buckset_list.end();itr++){ // �����ҵ�һ�����������С��Ͱ
		if(objsize <= (swMemBlockSizeT) (*itr)->getBucketBlockSize()){
			r = (*itr)->malloc(mbid);
			if( r){
				break;
			}
			//ƥ���С�����ϲ����ҵ�,���Ե��ڴ��һ����������������ڴ��
			//Ŀǰ��ʵ��
			//bsize = (*itr)->getBucketBlockSize();
			//break;
		}
	}
// 	if(bsize == MBS_MAX){
// 		return NULL;
// 	}
	//r=_buckettree[bsize]->malloc(mbid);
	if(!r &&_logger){
		swString msg;
		msg = swString::Format(swT("malloc %d bytes"),objsize);
		_logger->dbgMsg(msg,DBG_WARNING,SWMAP_ERR_HEAPBLOCK_MALLOC_FAILED);
	}
	return r;
}

void	swMemHeapEngine::free(swMemBlockID& bid){
	if( _buckets.find(bid.bucketId)!= _buckets.end()){
		_buckets[bid.bucketId]->free(bid);
	}
}

swMemBucketID swMemHeapEngine::newBucketId(){
	static swMemBucketID  bucketid= 1; //0 ����Ч��Ͱ���
	return bucketid++;
}

void	swMemHeapEngine::regBucket(swMemBucket* bucket){
	_buckets[bucket->getId()] = bucket;
}

void * swMemHeapEngine::ptr(swMemBlockID& bid){
	if( _buckets.find(bid.bucketId)!= _buckets.end()){
		return _buckets[bid.bucketId]->ptr(bid.idx);
	}
	return NULL;
}

shared_ptr<swMemHeapEngine> swMemHeapEngine::_handle;

/*
������Ͱʱ���밴��Ͱ��С��������
*/
bool swMemHeapEngine::createBucketSet(swUInt16 bucknum,swMemBlockSizeT blocksize ,swMemBucketSizeT bucketsize){
	swMemBucketSet* bset;
	bset = new swMemBucketSet(blocksize,bucketsize);
	if( bset->create(bucknum) == false){
		return false;
	}
// 	std::list<swMemBucketSet*>::iterator itr;
// 	for(itr=_buckset_list.begin();itr!=_buckset_list.end();itr++){
// 		if( )
// 	}
	_buckset_list.push_back(bset);
	return true;
}


void swMemHeapEngine::takeMemProfile(swMemHeapProfileDescT& desc){
	desc.bucketsets.clear();
	std::list<swMemBucketSet*>::iterator itr;
	for( itr=_buckset_list.begin();itr!= _buckset_list.end();itr++){
		swMemHeapBucketSetDescT bset;
		bset.blocksize = (*itr)->getBucketBlockSize();
		bset.bucketnum = (*itr)->getBucketNum();
		bset.bucketsize = (*itr)->getBucketSize();
		bset.blocknum = bset.bucketnum * (bset.bucketsize/bset.blocksize);
		bset.blockused = (*itr)->getUsedBlockNum();
		desc.bucketsets.push_back(bset);
	}
}

void swMemHeapEngine::dumpMemUsingProfile(){
	std::list<swMemBucketSet*>::iterator itr;
	if(!_logger){
		return;
	}
	_logger->dbgMsg(swT("---------------- swMemHeap Profile -----------------"));		
	swUInt16 n=0;
	for( itr=_buckset_list.begin();itr!= _buckset_list.end();itr++){
		swMemHeapBucketSetDescT bset;
		bset.blocksize = (*itr)->getBucketBlockSize();
		bset.bucketnum = (*itr)->getBucketNum();
		bset.bucketsize = (*itr)->getBucketSize();
		bset.blocknum = bset.bucketnum * (bset.bucketsize/bset.blocksize);
		bset.blockused = (*itr)->getUsedBlockNum();
		swString fmtstr;		
		fmtstr = swString::Format(swT("bucketset(0x%x) blocks: total=0x%x,used=%d"),bset.blocksize,bset.blocknum,bset.blockused);
		_logger->dbgMsg(fmtstr);		
		n++;
	}	
}

void	swMemHeapEngine::setLogger(swLogger*	log){
	_logger = log;
}