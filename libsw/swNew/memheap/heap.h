#ifndef _HEAP_BASE_H
#define _HEAP_BASE_H

#include "memheap.h"

//���ڴ����,ÿ��swHeapObject���󶼱��洢��һ���ڴ����
struct swHeapObject{
	swMemBlockID mbid;
	virtual void destroy() = NULL;  //�����ڱ��ͷŵ�ʱ�����Ҫʵ���������
};

/*
�ⲿ���õ��ڴ�ѿ������destroy()ʱ����ͷ�
*/
#endif

