//#include <stdio.h>
//#include <heap.h>


//struct header{
//
//	unsigned int size;
//	struct header* prev_real;
//	struct header* prev_free;
//	struct header* next_free;
//};
//struct header *__first,*__last,*__rover; 
//
///* removes a block from the free-block queue */
//void cdecl __pull_free_block(struct header* q)
//{
//	struct header * p;
//	if( ( __rover = q->next_free) == q )
//	{
//		__rover = NULL;
//	}
//	else
//	{
//		p = q->prev_free;
//		__rover->prev_free = p;
//		p->next_free = __rover;
//	}
//}
//
///* breaks up a large block into two pieces.
//one for allocating and one which remains free 
//returns a pointer to the allocated block */
//static void*cdecl allocate_partial_block(struct header*q,unsigned int len)
//{
//	struct header *n;
//	q->size = len;
//	n = (struct header*)((char*)q + q->size);
//	n->size = len+1;
//	n->prev_real = q;
//	if(__last == q)
//	{
//		__last = n;
//	}
//	else
//	{
//		q = (struct header*)((char*)n + len);
//		q->prev_real = n;
//	}
//
//	return ((void*)(&n->prev_free));
//}
///* attempts to extend the heap by pushing the brk level up
//returns a pointer to the last block if successful,
//NULL if not			*/
//static void * cdecl extend_heap(unsigned int len)
//{
//	struct header* q;
//	q = __sbrk((long)len & 0xffff);
//	if((int)q == -1)
//		return ((void*)NULL);
//
//	q->prev_real = __last;
//	q->size = len + -1;//?????
//	__last = q;
//
//	return ((void*)(&__last->prev_free));
//}
///* creates a heap from scratch
//returns a pointer to the first block if successful
//NULL if not */
//static void* cdecl create_heap( unsigned int len)
//{
//	struct header *q;
//	q = __sbrk((long)len&0xffff);
//	if((int)q == -1)
//		return ((void*)NULL);
//
//	__first = q;
//	__last = q;
//	q->size = len + 1;
//	return ((void*) (&q->prev_free));
//}
//
///* attempts to allocate a given number of contiguous bytes on the heap
//returns a pointer to the first byte of user sapce within the allocated block
//on NULL if not enough sapce was avilable */
//void* cdecl malloc(unsigned int nbytes)
//{
//	struct header *q;
//	if(nbytes == 0) return (NULL);
//	/* add the header size and force an 8-byte boundary*/
//	nbytes = (nbytes + USFD_HEADER_SIZE + 7) & 0XFFF8;
//	/* create a memory chain if it dosen't yet exist */
//	if(!__first)
//	{
//		return (create_heap(nbytes));
//	}
//	/* search for a free bolck through the memory chain */
//	if( ( q=__rover) != 0)
//	{
//		do 
//		{
//			/* big enough to break up?*/
//			if(q->size >= nbytes + free_header_size + delta_factor)
//			{
//				return ( allocate_partial_block(q,nbytes));
//			}
//			/* big enough to allocate?*/
//			if(q->size >= nbytes)
//			{
//				__pull_free_block(q);
//				++q->size;	/*mark it as used*/
//				return ((void*)(&q->prev_free));
//			}
//			q = q->next_free;	
//		} while(q != __rover);
//	}
//
//	/* couldn't find a free block big enough,try to extend the heap */
//	return (extend_heap(nbytes));
//
// }


#include "StdAfx.h"


struct header{

	unsigned int size;
	struct header* prev_real;
	struct header* prev_free;
	struct header* next_free;
};
struct header *__first,*__last,*__rover ; 

#define USED_HEADER_SIZE 16
#define FREE_HEADER_SIZE 16
#define DELTA_FACTOR 4
/* removes a block from the free-block queue */
//从空闲块队列中移除一个块
void cdecl __pull_free_block(struct header* q)
{
	struct header * p;
	
	//如果队列的下一空闲
	__rover = q->next_free;//__rover指到下一个空块
	if( __rover == q )
	{
		__rover = NULL;
	}
	else
	{
		//把q的前后两个空块链起来,__rover为最后一个空块
		p = q->prev_free;
		__rover->prev_free = p;
		p->next_free = __rover;
	}
}

/* breaks up a large block into two pieces.
one for allocating and one which remains free 
returns a pointer to the allocated block */
static void*cdecl allocate_partial_block(struct header*q,unsigned int len)
{
	struct header *n;
	q->size = len;
	n = (struct header*)((char*)q + q->size);//n指在q的后面
	n->size = len+1;//
	n->prev_real = q;

	if(__last == q)
	{
		//如果本来q为最后一块则把n作为最后一块.
		__last = n;
	}
	else
	{
		//q不是最后一块,则让q指向n后的自由空间,
		q = (struct header*)((char*)n + len);
		q->prev_real = n;
	}

	return ((void*)(&n->prev_free));
}
/* attempts to extend the heap by pushing the brk level up
returns a pointer to the last block if successful,
NULL if not			*/
static void * cdecl extend_heap(unsigned int len)
{
	struct header* q;

	q = __sbrk((long)len & 0xffff);
	if((int)q == -1)
		return ((void*)NULL);

	q->prev_real = __last;
	q->size = len + -1;//?????
	__last = q;

	return ((void*)(&__last->prev_free));
}
/* creates a heap from scratch
returns a pointer to the first block if successful
NULL if not */
static void* cdecl create_heap( unsigned int len)
{
	struct header *q;

	HANDLE  hHeap = HeapCreate( 0,0x100000,0);
	if(!hHeap)
		return ((void*)NULL);
 
	q = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,len);


	__first = q;
	__last = q;
	q->size = len + 1;
	
	return ((void*) (&q->prev_free));
}

/* attempts to allocate a given number of contiguous bytes on the heap
returns a pointer to the first byte of user sapce within the allocated block
on NULL if not enough sapce was avilable */
void* cdecl malloc(unsigned int nbytes)
{

	struct header *q;
	if(nbytes == 0)
		return (NULL);
	
	/* add the header size and force an 8-byte boundary*/
	nbytes = (nbytes + USED_HEADER_SIZE + 7) & 0XFFF8;
	/* create a memory chain if it dosen't yet exist */
	if(!__first)
	{
		return (create_heap(nbytes));
	}
	/* search for a free bolck through the memory chain */
	q = __rover; //q赋为自由块的反向头
	if( q != 0)
	{
		do 
		{
			/* big enough to break up?*/
			if(q->size >= nbytes + FREE_HEADER_SIZE + DELTA_FACTOR)
			{
				return ( allocate_partial_block(q,nbytes));
			}
			/* big enough to allocate?*/
			if(q->size >= nbytes)
			{
				__pull_free_block(q);
				++q->size;	/*mark it as used*/
				return ((void*)(&q->prev_free));
			}
			q = q->next_free;	//找下一个空块
		} while(q != __rover);
	}

	/* couldn't find a free block big enough,try to extend the heap */
	return (extend_heap(nbytes));

}

//////////////////////////////////////////////////////////////////////////
/* adds a block to the free-block queue */
static void cdecl insert_free_block(struct header * q)
{
	struct header * n;
	if(__rover)
	{
		n = __rover->next_free;
		__rover->next_free = q;
		n->prev_free = q;
		q->next_free = n;
		q->prev_free = __rover;
	}
	else
	{
		__rover = q;
		q->prev_free = q;
		q->next_free = q;
	}
}
/* join two adjacent free blocks together */
static void cdecl join_free_blocks(struct header* a,struct header *b)
{
	struct header *n;
	a->size += b->size;
	if(__last == b)
	{
		__last = a;
	}
	else
	{
		n = (struct header*)((char*)b + b->size);
		n->prev_real = a;
	}
	__pull_free_block(b);
}
/* free the last block in the heap and adjusts the break level */
static void cdecl free_last_block(void)
{
	static header* p;

	if(__first == __last)
	{
		brk(__first);
		__first = __last = NULL;
	}
	else
	{
		p = __last->prev_real;
		if((p->size& 0x0001) == 0)
		{
			__pull_free_block(p);
			if(p == __first)
				__first = __last = NULL;
			else 
				__last = p->prev_real;

			brk(p);
		}
		else
		{
			brk(__last);
			__last = p;
		}
	}
}
/* frees an interior block within the heap */
static void cdecl free_inner_block(struct header* q)							
{
	struct header *n,*p;
	--q->size; //mark it as free
	n = (struct header *)  ((char*) q + q->size); /* set up pointers to the */ 
	p = q->prev_real;								/* previous and next blocks */
	if( (p->size !0x0001) == 0 && q != __first)   /* join to the previous block?*/
	{
		p->size += q->size;//前一个实块的大小加上本次释放的块.
		n->prev_real = p;//后续块指向前一个实块
		q = p;
	}
	else			/* add it to the free queue */
	{
		//在释放一个块时,把这个块插入到自由块队列中去
		insert_free_block(q);
	}

	if((n->size & 0x0001) == 0)
	{
		join_free_blocks(q,n);
	}
}

/* deallocates a block and returns it to the free-block queue */
void cdecl free(void* q)
{
	if( q == NULL ) /*ANSI requires this */
		return;

	q = (char*) q - USED_HEADER_SIZE; /* calc block's start address */
	if( q == __last)
	{
		free_last_block();
	}
	else
	{
		free_inner_block(q)
	}
}