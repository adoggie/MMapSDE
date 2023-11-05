
#ifndef _MEM_HEAP_H
#define _MEM_HEAP_H
#include "../base.h"
#include "memdump.h"
#include "../dbglog.h"

#include <boost/shared_ptr.hpp>
using namespace boost;

//�ڴ���С
enum swMemBlockSizeT{
	MBS_4 = 0x01 << 4,
	MBS_5 = 0x01 << 5,
	MBS_6 = 0x01 << 6,
	MBS_7 = 0x01 << 7,
	MBS_8 = 0x01 << 8,	//256
	MBS_9 = 0x01 << 9,	//512
	MBS_10 = 0x01 << 10, //1024 bytes
	MBS_11 = 0x01 << 11,	//2k
	MBS_12 = 0x01 << 12,	//4k
	MBS_13 = 0x01 << 13,	//8k
	MBS_14 = 0x01 << 14, // 16k
	MBS_15 = 0x01 << 15, // 32k
	MBS_16 = 0x01 << 16, // 64k
	MBS_17 = 0x01 << 17, // 128k
	MBS_18 = 0x01 << 18, // 256k
	MBS_19 = 0x01 << 19, // 512k
	MBS_20 = 0x01 << 20, // 1M
	MBS_MAX = 0x01 <<30,	//����ʵ�ֵĿ��С
};
//Ͱ��С����
enum swMemBucketSizeT{
	BUCKET_1M = MBS_20, //1M��Ͱ��С
	BUCKET_2M = (MBS_20)*2, //2M��Ͱ��С,2M��Ͱ�ڵ��ڴ�������MBS_5���ϣ���֤8k������Ѱַ��
};
#define swMemBucketId_Null 0
typedef swUInt8 swMemBucketID;	//Ͱ��� ����ϵͳ��ֻ�ܴ���1��255��Ͱ,0��ʾ�Ƿ�Ͱ ��
typedef struct _tagMemBlock{
	swUInt16	idx;						//�ڴ���±� ,���bucket��СΪ1M����ÿ������С�ߴ�Ϊ16�ֽ�, һ��poi�������Сռ��8���ֽ�
	swMemBucketID	bucketId;	//Ͱ���
	bool isOk(){
		if(bucketId == swMemBucketId_Null){
			return false;
		}
		return true;
	}
}swMemBlockID; 

struct swMemHeapBucketConfigT{
	swMemBlockSizeT blocksize;
	swMemBucketSizeT	bucketsize;
	swUInt32		bucketnum;	//Ͱ����
};

struct swMemHeapConfigT{
	swMemHeapBucketConfigT* buckets;	//Ͱ�����ã���0Ϊ��ֹ
};
/*
swMemHeapBucketConfigT buckets={
		{MBS_4,BUCKET_1M,4},  4��1M��С��Ͱ,Ͱ�ڶ���16�ֽڵ�λ���ڴ�飬
		{MBS_8,BUCKET_1M,10},  10��1M��С��Ͱ,Ͱ�ڶ���256�ֽڵ�λ���ڴ�飬4k*10���ڴ��
		{MBS_MAX,0,0}
		};
*/
class swMemBucketSet;
class swMemBucket;
class swMemHeapEngine{
public:
	bool	init(swMemHeapConfigT& config);
	static swMemHeapEngine* instance();
	void* malloc(swUInt32 objsize,swMemBlockID& mbid);	 //����ָ����С����,�����ڴ���ź͵�ַ,�����䲻��ʱ����Ҫ��ϵͳ�����µ��ڴ�����
	void	free(swMemBlockID&);
	swMemBucketID newBucketId() ;					//����Ͱ���
	void * ptr(swMemBlockID& bid);
	void	regBucket(swMemBucket* bucket); //�Ǽ�Ͱ��Ϣ
	~swMemHeapEngine();
	bool createBucketSet(swUInt16 bucknum,swMemBlockSizeT blocksize ,swMemBucketSizeT bucketsize = BUCKET_1M);
	void takeMemProfile(swMemHeapProfileDescT& desc);		//�����ڴ�ʹ�������Ϣ
	void dumpMemUsingProfile();
	void	setLogger(swLogger*	log);
private:
	static shared_ptr<swMemHeapEngine> _handle;
	//std::map<swMemBlockSizeT,swMemBucketSet*>		_buckettree; //Ͱ���νṹ
//	std::vector<swMemBlockSizeT> _blocksizelist;
	std::map<swMemBucketID,swMemBucket*>	_buckets;	//Ϊ����free���ṩ
	swMutex		_mutex_bucket;
	std::list<swMemBucketSet*> _buckset_list;
	swLogger*			_logger;
};


#endif