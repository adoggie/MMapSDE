
#ifndef _SW_HEAPMEM_DUMP_H
#define _SW_HEAPMEM_DUMP_H

#include "../base.h"
/*
 memdump.h
 ���ڴ�Ӱ�����
*/

struct swMemHeapBucketSetDescT{
	swUInt32	blocksize;	//Ͱ�ڴ���С
	swUInt32	blockused;	//ʹ�õĿ����� 
	swUInt32	bucketnum;	//Ͱ����
	swUInt32	blocknum;		//�ڴ������
	
	swUInt32	bucketsize;	//Ͱ��ʵ���ڴ�ռ��С ����1M ��2M...
};

struct swMemHeapProfileDescT{
	std::list<swMemHeapBucketSetDescT> bucketsets;  //Ͱ����Ϣ
};




#endif

