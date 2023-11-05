#ifndef _MEM_BUCKET_H
#define _MEM_BUCKET_H
#include "../base.h"
#include <boost/shared_ptr.hpp>
using namespace boost;
#include "memheap.h"



class swMemBucketSet;
//�ڴ�Ͱ����
//Ͱ����С����Ϊ1MB,Ͱ���ڴ����С��λΪ2^4=16�ֽڣ�������Ҫ8k�ֽ���Ϊ�����ʶʹ��״̬(0/1)
class swMemBucket{
friend swMemBucketSet;
public:
	swMemBucket(swMemBucketSet* set);
	~swMemBucket();
	bool		create();						//����Ͱ�ڴ�����
	void* ptr(swUInt16 idx);  //�ڴ����
	void * malloc(swMemBlockID&);		//����һ���ڴ�飬�����ʶΪ������
	void free(swMemBlockID&);	//��ʶΪ����
	swUInt32 getFreeNum();		//�����ڴ������
	swUInt32 getUsedNum();	//�Ѿ�ʹ�õĿ�����
public:
	swMemBucketID	 getId();
private:
	std::vector<swByte>			_bitmap;		//����ͼ
	swMemBucketSet* _set;
	swMemBucketID		_id;		//Ͱ�������
	char*			_memstart;
	swUInt32		_freenum;
};


//�ڴ�Ͱ��������
//Ͱ����಻�ܹ���255��Ͱ��ÿ��Ͱ���ֻ��64k����,�����ڹ���ʱ bucketsize/blocksize���ܴ���64k
class swMemBucketSet{
public:
	swMemBucketSet(swMemBlockSizeT blocksize ,swMemBucketSizeT bucketsize=BUCKET_1M);
	bool	create(swUInt16 bucketnum); 
	swMemBucketSizeT		getBucketSize();	//Ͱ����
	swMemBlockSizeT		getBucketBlockSize(); //�ڴ���С
	//void*				ptr(swMemBlockID& id);	//�õ��ڴ��ĵ�ַ
	swMemBucket*	createBucket(); //����һ����Ͱ,��Ԥ�ȴ�����Ͱ���ʱ����ϵͳ�����µĿռ�
	swUInt16	getBucketNum();	
	void* malloc(swMemBlockID&);	 
	swUInt32	getUsedBlockNum();   //����ʹ�õ��ڴ������
private:
	std::map<swMemBucketID,swMemBucket*>	_buckets;	//Ͱ�б� 
	swMemBucketSizeT		_bucketsize;		//Ͱ��С
	swMemBlockSizeT		_blocksize;	//�ڴ���С
};

#endif
