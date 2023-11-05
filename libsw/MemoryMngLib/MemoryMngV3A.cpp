// MemoryMngV3A.cpp: implementation of the CMemoryMngV3A class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemoryMngV3A.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryMngV3A::CMemoryMngV3A()
{
	m_heap = NULL;
	m_heapEnd = NULL;
	last_valid_address = NULL;
	
	__first = NULL;
	__last	= NULL;
	__rover = NULL;

    m_nInitHeapType = 0;
//////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_DATA
    m_MaxHeapUsedSize = 0;
    m_MinRequireSize = 0xffffffff;
    m_MaxBlockSize = 0;
    m_MaxRequireSize = 0;
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    m_strInfo = _T("");
#endif    
//////////////////////////////////////////////////////////////////////////
    
}

CMemoryMngV3A::~CMemoryMngV3A()
{
    //MEMMNG_DUMPSTATE(this);
	//ASSERT(m_heap);
    ClearHeap();
}

BOOL CMemoryMngV3A::InitialHeap(unsigned long nbytes)
{
    ASSERT(m_nInitHeapType == 0);
	nbytes = (nbytes + 7) &0Xfffffff8;
	m_heap = (char*)malloc(nbytes);
    if(!m_heap)
        return FALSE;

    m_heapEnd = m_heap+nbytes;
	memset(m_heap,0xcd,m_heapEnd - m_heap);
    last_valid_address = (char*)m_heap;
    __first = NULL;
	__last	= NULL;
	__rover = NULL;

    m_nInitHeapType = 1;
	return TRUE;
}

BOOL CMemoryMngV3A::InitialHeap(void* pHeap,unsigned long nbytes)
{
    ASSERT(m_nInitHeapType == 0);
    if(!pHeap)
        return FALSE;
    
	ASSERT(!(nbytes&0x00000007));
    ASSERT( !( ((DWORD)pHeap)&0x00000007)); 
	m_heap = (char*)pHeap;
    if(!m_heap)
        return FALSE;

    m_heapEnd = m_heap+nbytes;
	memset(m_heap,0xcd,m_heapEnd - m_heap);
    last_valid_address = (char*)m_heap;
    __first = NULL;
	__last	= NULL;
	__rover = NULL;

    m_nInitHeapType = 2;
	return TRUE;
}

BOOL CMemoryMngV3A::ResetHeap()
{
    ASSERT(m_nInitHeapType != 0);
    memset(m_heap,0xcd,m_heapEnd - m_heap);
    last_valid_address = (char*)m_heap;
    __first = NULL;
	__last	= NULL;
	__rover = NULL;

    return TRUE;
}

BOOL CMemoryMngV3A::ClearHeap()
{
    if(m_nInitHeapType == 1)
    {
        if(m_heap)
        {
            free(m_heap);
            m_heap = NULL;
            
        }
    }

    m_heapEnd = NULL;
    last_valid_address = NULL;
    
    __first = NULL;
    __last	= NULL;
    __rover = NULL;

	return TRUE;
}

void* CMemoryMngV3A::newmalloc(unsigned long nbytes)
{
#ifdef _DEBUG
    AssertValid();
    if(nbytes)
    {
        nbytes+=4;
    }
#endif

	void* p = newmallocActual(nbytes);

#ifdef _DEBUG
    if(p)
    {
        struct header* pHeader = (struct header*)((char*)p - USED_HEADER_SIZE);
        pHeader->m_dwFenceSign = 0xfdfdfdfd;
        DWORD dwPayloadSize = (pHeader->size&0xfffffffe) - USED_HEADER_SIZE - 4;
        DWORD* pEndFenceSign = (DWORD*)((char*)p + dwPayloadSize);
        *pEndFenceSign = 0xfdfdfdfd;
        memset(p,0xcd,dwPayloadSize);
    }
	else
	{
		TRACE1("********malloc %d bytes failed!********\n",nbytes);
		DumpState();
	}

    AssertValid();
#endif

//////////////////////////////////////////////////////////////////////////
//用于统计的代码
#ifdef ANALYSE_DATA
    if(p)
    {
        DWORD dwNowHeapUsedSize = last_valid_address - m_heap;
        if(dwNowHeapUsedSize> m_MaxHeapUsedSize)
        {
            m_MaxHeapUsedSize = dwNowHeapUsedSize;
        }

        struct header* pHeader = (struct header*)((char*)p - USED_HEADER_SIZE);
        if(pHeader->size > m_MaxBlockSize)
        {
            m_MaxBlockSize = pHeader->size;
        }

        if( (nbytes - 4) > m_MaxRequireSize)
        {
            m_MaxRequireSize = nbytes - 4;
        }
        if( (nbytes - 4) < m_MinRequireSize)
        {
            m_MinRequireSize = nbytes - 4;
        }
    }
#endif
//////////////////////////////////////////////////////////////////////////

    return p;
}

void CMemoryMngV3A::newfree(void* p)
{
#ifdef _DEBUG
    if(p)
    {
        char* pFreeAddr = (char*)p;
        ASSERT(pFreeAddr > m_heap);
        ASSERT(pFreeAddr < m_heapEnd);
        ASSERT(!((DWORD)pFreeAddr&0x00000003));//必须为4字节对齐
       
        struct header* pHeader = (struct header*)((char*)p - USED_HEADER_SIZE);
        ASSERT(pHeader->size&0x00000001);
        ASSERT(pHeader->m_dwFenceSign == 0xfdfdfdfd);
        DWORD dwPayloadSize = (pHeader->size&0xfffffffe) - USED_HEADER_SIZE - 4;
        DWORD* pEndFenceSign = (DWORD*)((char*)p + dwPayloadSize);
        ASSERT((*pEndFenceSign) == 0xfdfdfdfd);
        memset(p,0xdd,dwPayloadSize);
        pHeader->m_dwFenceSign = 0xdddddddd;
        *pEndFenceSign = 0xdddddddd;


    }
    AssertValid();
#endif
	newfreeActual(p);
#ifdef _DEBUG
    AssertValid();
#endif
}


#ifdef _DEBUG
void CMemoryMngV3A::DumpState()
{
    TRACE1("-------%s MemoryMng DumpState----------------\n",m_strInfo);
    TRACE3("heap[0x%x -- 0x%x](%d bytes).\n",m_heap,m_heapEnd,m_heapEnd - m_heap);
    TRACE1("__first             :0x%x.\n",__first);
    TRACE1("__last              :0x%x.\n",__last);
    TRACE1("__rover             :0x%x.\n",__rover);
    TRACE1("last_valid_address  :0x%x.\n",last_valid_address);
#ifdef ANALYSE_DATA
    TRACE1("MaxHeapUsedSize     :%d.\n",m_MaxHeapUsedSize);
    TRACE1("MaxBlockSize        :%d.\n",m_MaxBlockSize+1);
    TRACE1("MinRequireSize      :%d.\n",m_MinRequireSize);
    TRACE1("MaxRequireSize      :%d.\n",m_MaxRequireSize);
    TRACE1("BlockCount          :%d.\n",GetBlockCount());
    TRACE1("FreeBlockCount      :%d.\n",GetFreeBlockCount());
    TRACE1("FreeBlockTotalSize  :%d.\n",GetFreeBlockTotalSize());
#endif
    TRACE0("-------------------------------------------\n");
    

}
void CMemoryMngV3A::AssertValid() const
{
	return;//*******************注意,这个函数太耗时,所以先关掉********************
	
    struct header* pHeader = __first;

    while(pHeader!=__last)
    {
        pHeader = (struct header*)((char*)pHeader + (pHeader->size&0xfffffffe));
    }
}

void CMemoryMngV3A::SetTextInfo(LPCTSTR strInfo)
{
    m_strInfo = strInfo;
}

#endif	

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/* attempts to allocate a given number of contiguous bytes on the  heap
returns a pointer to the first byte of user space ithin the allocated block
on NULL oif not enough sapce was available */

void* CMemoryMngV3A:: newmallocActual(unsigned long nbytes)
{
	if(nbytes == 0)	return NULL;

	/* add the header size and force an 8-byte boundary */
	nbytes = (nbytes + USED_HEADER_SIZE + 7) &0xfffffff8; //放大以便字节对齐

	//如果内存链不存在的话就创建它
	if(!__first)
	{
		return create_heap(nbytes);
	}

	struct header* q = NULL;
	//有自由内存链的话在链上找一个空块
	if( (q=__rover) != 0 ) // __rover  表示当前移动的数据指针
	{
		do 
		{
			/* big enough to break up ?*/
			if(q->size >= nbytes + FREE_HEADER_SIZE + DELTA_FACTOR) //缓冲包太大了，存储就浪费了，这里就要进行切割成两块
			{
				return ( allocate_partial_block(q,nbytes));
			}
			/* big enough to allocate?*/
			if( q->size >= nbytes)
			{
				__pull_free_block(q);
				++q->size; /*mark it as used */
				return ( (void*)(&q->prev_free));
			}
			q = q->next_free;	
		} 
		while(q != __rover);
	}
	/* couldn't find a free block big enough,try to extend the heap*/
	return (extend_heap(nbytes));
}

/* dea;;pcates a block and returns it to the free-block queue */
void CMemoryMngV3A:: newfreeActual(void* q)
{
	if( q == NULL) /*ANSI requires this*/
		return;

	q = (char*)q - USED_HEADER_SIZE;/* calc block's start address */
	
	if( q == __last )
	{
		free_last_block();
	}
	else
	{
		free_inner_block((struct header*)q);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/* removes a block from the free-block queue */
//这个块必须在之前判定是未被使用的,从空闲块队列中移出指定块
void CMemoryMngV3A:: __pull_free_block(struct header* q)
{
	struct header* p;

	if( (__rover = q->next_free) == q)
	{
		//如果要释放的块的下一个自由区刚好是它自已,漫指针就置空
		__rover = NULL;
	}
	else
	{
		//如果要释放的块的下个区不是它自己,把这个下面区的前区指针置为此区本体
		p = q->prev_free;
		__rover->prev_free = p;
		p->next_free = __rover;
	}
}

/* breaks up a large block into two pieces,
one for allocating and one which remains free
returns a pointer to the allocated block */
//从一个足够大的块的后边分配一个块
void* CMemoryMngV3A:: allocate_partial_block(
										  struct header* q,//上位结点指针
										  unsigned int len//本节点控制的长度
										  )
{
	struct header* n;//定义下相邻结点指针n

	//设定上位结点的控制块长度,并在上位结点后构造结点,并使此结点指向上位结点.
	q->size			-= len;
	n				= (struct header*)( (char*)q + q->size);
	n->size			= len + 1;
	n->prev_real	= q;

	if(__last == q)
	{
		//如果上位结点刚好是最后一个结点那么把最后一个结点移到n上,因为刚刚构建了n
		__last = n;
	}
	else
	{
		//上位结点不是最后一个结点.那么n结点后的结点的前一个实块指向n.(原来它是指向传入的指针q的.)
		q = (struct header*)( (char*)n + len);
		q->prev_real= n;
	}

	return((void*)(&n->prev_free));
}

/* attempts to extend the heap by pushing the brk level up returns a pointer to 
the last block if successful, NULL if not*/
void* CMemoryMngV3A:: extend_heap(unsigned int len)
{
	struct header* q;

	//继续扩展堆
	q = (struct header*)__sbrk( (long)len & 0xffffffff );
	if( (int)q == -1 )	return ((void*)NULL);

	q->prev_real = __last;
	q->size = len + 1; // ????
	__last = q;
	return ((void*)(&__last->prev_free));

}

/* creates a heap from scratch
 returns a pointer to the first block if successful
 NULL if not */
void* CMemoryMngV3A:: create_heap(unsigned int len)
{
	struct header* q;
	q = (struct header*)__sbrk( (long)len & 0xffffffff); //这里的mask似乎没有作用
	if((int)q == -1)return ((void*)NULL); //-1意味着没有可用空间了
	__first = q; //创建堆，设置起始和终止的内存链信息
	__last = q;
	q->size = len +1;// +1 ? 
	return ( (void*)(&q->prev_free));
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/* adds a block to the free-block queue*/
void CMemoryMngV3A:: insert_free_block(struct header* q)
{
	struct header* n;

	//在双向空闲链表中插入q
	if(__rover)
	{
        // (q | __rover => n) --------------__rover => q =>n
		n = __rover->next_free;
		__rover->next_free = q; 
		n->prev_free = q;
		q->next_free = n;
		q->prev_free = __rover;
	}
	else//否则以本尊生成空闲队列
	{
		//没有漫指针,(表示是第一次作free操作)

		__rover = q;//传入的块作为漫指针,自已链接自已.
		q->prev_free = q;
		q->next_free = q;
	}
}


/* joins two adjacent free blocks together */
void CMemoryMngV3A:: join_free_blocks(struct header* a,struct header* b)
{
    ASSERT(!(a->size&0x01));
    ASSERT(!(b->size&0x01));
   
	struct header* n;
	a->size += b->size;

	if(__last == b)
	{
		//如果b为最后一个块
		__last = a;
	}
	else
	{
		n = (struct header*)((char*)b + b->size);
		n->prev_real = a;
	}

	__pull_free_block(b);
}

/* frees the last block in the heap and adjusts the break level */
//从堆上把最后一个分配块去掉,再把最后块的指针向前挪一个块
void CMemoryMngV3A:: free_last_block(void)
{
	struct header* p;

	if( __first == __last ) /* freeing the ONLY block?*/
	{
		//头指针==尾指针表示只分配了一个块.
		brk(__first);
		__first = __last = NULL;
	}
	else
	{
		//找到最后一个块的前一个紧接块,把它置为最后一个块.
		p = __last->prev_real;

		if( (p->size & 0x0001) == 0) /* is privious block free?*/
		{
			__pull_free_block(p);//如果这个块没被使用,把这个块回收
			if( p == __first)
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
void CMemoryMngV3A:: free_inner_block(struct header* q)
{
	struct header *n,*p;

	//标记为未使用
	--q->size;							/* mark it as free */
	
	/* set up pointers to the previous and next blocks*/
	n = (struct header*)((char*)q + q->size);	//控制块就紧跟在数据区的后面
	p = q->prev_real;							//找到这个块的前一个紧连接的实块.


	//前一实块没有在使用中 且 当前块又不是首块,则把当前块归并到前一实块中 
	if( q != __first && (p->size & 0x0001) == 0) /* join to the previous block?*/
	{
		p->size += q->size;
		n->prev_real = p;
		q = p;
	 }
	 else
	 {
		 //否则把它归并到空闲队列中
		 insert_free_block(q); /* add it to the free queue */
	 }

	 if((n->size & 0x0001) == 0)
	 {
		 join_free_blocks(q,n);/* join with the next block?*/
	 }	
}


/*
从堆里面分配出一段内存
*/
void* CMemoryMngV3A:: __sbrk(long incr)
{
    if( (last_valid_address+incr) > m_heapEnd) ///堆中剩余的空间已经不能满足请求的大小
        return (void*)-1;

    void* pRtn = (void*)last_valid_address;
	last_valid_address += incr; //移动可用的堆剩余空间的地址指针

    return pRtn;
}
/*
    用于动态地改变分配给调用程序数据段的存储量.这种改变是通过
    重置程序的截断值进行的,截断值是数据段结尾处以上的第一个位置的地址.分配的存储
    空间量随着截断值的增加而增加.
*/
/*
将终止值修改为endds.
返回值 成功:0
失败 -1.
*/
int CMemoryMngV3A::brk(void* endds)
{
    if(endds <= m_heapEnd)
    {
	    last_valid_address = (char*)endds;
        return 0;
    }

    return -1;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//用于统计的代码
#ifdef ANALYSE_DATA

int CMemoryMngV3A::GetBlockCount()
{
    if(!__first)
        return 0;
    
    int nCount = 1;
    struct header* pHeader = __first;

    while(pHeader!=__last)
    {
        nCount++;
        pHeader = (struct header*)((char*)pHeader + (pHeader->size&0xfffffffe));
    }

    return nCount;
}
int CMemoryMngV3A::GetFreeBlockCount()
{
    if(!__rover)
        return 0;
    
    int nCount = 0;
    struct header* q= __rover;
	
    do 
    {
        nCount++;
        q = q->next_free;
    } 
    while(q != __rover);

    return nCount;

}
unsigned long CMemoryMngV3A::GetFreeBlockTotalSize()
{
    if(!__rover)
        return 0;
    
    unsigned long nSize = 0;
    struct header* q = __rover;
	
    do 
    {
        nSize += q->size;
        q = q->next_free;
    } 
    while(q != __rover);

    return nSize;
}

#endif
//////////////////////////////////////////////////////////////////////////