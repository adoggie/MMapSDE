// MemoryMng2V1.cpp: implementation of the CMemoryMng2V1 class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MemoryMng2V1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryMng2V1::CMemoryMng2V1()
{
	m_heap = NULL;
	m_heapEnd = NULL;
	last_valid_address = NULL;
	
	__first = NULL;
	__last	= NULL;
	__rover = NULL;

	m_nCellSize = 0;
    m_nInitHeapType = 0;
//////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_DATA
    m_MaxHeapUsedSize = 0;
    m_MaxCellCount = 0;
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    m_strInfo = _T("");
#endif    
//////////////////////////////////////////////////////////////////////////

}

CMemoryMng2V1::~CMemoryMng2V1()
{
    //MEMMNG_DUMPSTATE(this);
	ASSERT(m_heap);
    ClearHeap();
}

BOOL CMemoryMng2V1::InitialHeap(unsigned long nbytes,unsigned long nCellSize)
{
    ASSERT(m_nInitHeapType == 0);
	ASSERT(!(nCellSize&0x00000003));
//	ASSERT(!(nbytes%nCellSize));
	
	if(!nbytes)
		return FALSE;
	if(!nCellSize)
		return FALSE;
	
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

	m_nCellSize = nCellSize;
    m_nInitHeapType = 1;
	return TRUE;
}

BOOL CMemoryMng2V1::InitialHeap(void* pHeap,unsigned long nbytes,unsigned long nCellSize)
{
    ASSERT(m_nInitHeapType == 0);
	ASSERT(!(nCellSize&0x00000003));
//	ASSERT(!(nbytes%nCellSize));

	if(!nbytes)
		return FALSE;
	if(!nCellSize)
		return FALSE;
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

	m_nCellSize = nCellSize;
    m_nInitHeapType = 2;
	return TRUE;
}

BOOL CMemoryMng2V1::ResetHeap()
{
    ASSERT(m_nInitHeapType != 0);
    memset(m_heap,0xcd,m_heapEnd - m_heap);
    last_valid_address = (char*)m_heap;
    __first = NULL;
	__last	= NULL;
	__rover = NULL;

    return TRUE;
}

BOOL CMemoryMng2V1::ClearHeap()
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

	m_nCellSize = 0;
	return TRUE;
}

//内存池管理，请求内存
void* CMemoryMng2V1::newmalloc()
{
#ifdef _DEBUG
    AssertValid();
#endif
	void* p = newmallocActual();
#ifdef _DEBUG
    if(p)
    {
        struct header* pHeader = (struct header*)((char*)p - USED_HEADER_SIZE);
        pHeader->m_dwFenceSign = 0xfdfdfdfd;
        memset(p,0xcd,m_nCellSize);
    }
	else
	{
		TRACE1("********malloc %d bytes failed!********\n",m_nCellSize);
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

        m_MaxCellCount++;
    }
#endif
//////////////////////////////////////////////////////////////////////////

    return p;
}

void CMemoryMng2V1::newfree(void* p)
{
#ifdef _DEBUG
    if(p)
    {
        char* pFreeAddr = (char*)p;
        ASSERT(pFreeAddr > m_heap);
        ASSERT(pFreeAddr < m_heapEnd);
        ASSERT(!((DWORD)pFreeAddr&0x00000003));//必须为4字节对齐
       
        struct header* pHeader = (struct header*)((char*)p - USED_HEADER_SIZE);
        ASSERT(pHeader->m_dwFenceSign == 0xfdfdfdfd);
        memset(p,0xdd,m_nCellSize);
        pHeader->m_dwFenceSign = 0xdddddddd;
    }
    AssertValid();
    
#endif

	newfreeActual(p);
    
#ifdef _DEBUG
    AssertValid();
#ifdef ANALYSE_DATA
    m_MaxCellCount--;
#endif
#endif
}


#ifdef _DEBUG

void CMemoryMng2V1::DumpState()
{
    TRACE1("-------%s MemoryMng DumpState----------------\n",m_strInfo);
    TRACE3("heap[0x%x -- 0x%x](%d bytes).\n",m_heap,m_heapEnd,m_heapEnd - m_heap);
    TRACE1("__first             :0x%x.\n",__first);
    TRACE1("__last              :0x%x.\n",__last);
    TRACE1("__rover             :0x%x.\n",__rover);
    TRACE1("last_valid_address  :0x%x.\n",last_valid_address);
    TRACE1("Cell size			:%d.\n",m_nCellSize);
    TRACE1("Block size			:%d.\n",m_nCellSize+USED_HEADER_SIZE);
#ifdef ANALYSE_DATA
    TRACE1("MaxHeapUsedSize     :%d.\n",m_MaxHeapUsedSize);
    TRACE1("MaxCellCount        :%d.\n",m_MaxCellCount);
    TRACE1("BlockCount          :%d.\n",GetBlockCount());
    TRACE1("FreeBlockCount      :%d.\n",GetFreeBlockCount());
    TRACE1("FreeBlockTotalSize  :%d.\n",GetFreeBlockTotalSize());
#endif
    TRACE0("----------------------------------------\n");

}
void CMemoryMng2V1::AssertValid() const
{
    ASSERT(m_nInitHeapType != 0);
	ASSERT(!(m_nCellSize&0x00000003));
	ASSERT(m_heap);
    ASSERT(m_heapEnd);
    ASSERT(!((DWORD)m_heap&0x00000007));
    ASSERT(!((DWORD)m_heapEnd&0x00000003));
 
    unsigned long nBlockSize = m_nCellSize + USED_HEADER_SIZE;
    if(__first)
        ASSERT( !(((char*)__first-m_heap)%nBlockSize));
    if(__last)
        ASSERT( !(((char*)__last-m_heap)%nBlockSize));
    if(__rover)
        ASSERT( !(((char*)__rover-m_heap)%nBlockSize));
}

void CMemoryMng2V1::SetTextInfo(LPCTSTR strInfo)
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

void* CMemoryMng2V1:: newmallocActual()
{
	ASSERT(m_nCellSize);

    unsigned long nBlockSize = m_nCellSize + USED_HEADER_SIZE;
	//如果内存链不存在的话就创建它
	if(!__first)
	{
		return create_heap(nBlockSize);
	}

	struct header* q = NULL;
	//有自由内存链的话在链上找一个空块
	if(__rover)
	{
        q = __rover;
        __rover = __rover->next_free;
		return ( (void*)(&q->prev_free));

	}
	/* couldn't find a free block big enough,try to extend the heap*/
	return (extend_heap(nBlockSize));
}

/* dea;;pcates a block and returns it to the free-block queue */
void CMemoryMng2V1:: newfreeActual(void* pAddr)
{
	if( pAddr == NULL) /*ANSI requires this*/
		return;

    struct header* q = (struct header*)((char*)pAddr-USED_HEADER_SIZE);/* calc block's start address */
	
//	if( q == __last )
//	{
//		free_last_block();
//	}
//	else
//	{
//		q->next_free = __rover;
//		__rover= q; 
// 	}
    //这里最后一个块不回退了.
    q->next_free = __rover;
    __rover= q; 
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


/* attempts to extend the heap by pushing the brk level up returns a pointer to 
the last block if successful, NULL if not*/
void* CMemoryMng2V1:: extend_heap(unsigned int len)
{
	struct header* q;

	//继续扩展堆
	q = (struct header*)__sbrk( (long)len & 0xffffffff );
	if( (int)q == -1 )	return ((void*)NULL);

	__last = q;
	return ((void*)(&__last->prev_free));

}

/* creates a heap from scratch
 returns a pointer to the first block if successful
 NULL if not */
void* CMemoryMng2V1:: create_heap(unsigned int len)
{
	struct header* q;
	q = (struct header*)__sbrk( (long)len & 0xffffffff);
	if((int)q == -1)return ((void*)NULL);
	__first = q;
	__last = q;
	return ( (void*)(&q->prev_free));
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


/* frees the last block in the heap and adjusts the break level */
//从堆上把最后一个分配块去掉,再把最后块的指针向前挪一个块
void CMemoryMng2V1:: free_last_block(void)
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
		p = __last - m_nCellSize - USED_HEADER_SIZE;
		brk(__last);
		__last = p;
	}
}


/*
	在近堆栈中修改数据段空间申请.
    将终止值增加incr字节并相应修改已申请的空间.
    incr可以为负,这里已申请的空间减小
    返回值:
        成功:旧终止值
        失败:-1
*/
void* CMemoryMng2V1:: __sbrk(long incr)
{
    if( (last_valid_address+incr) > m_heapEnd)
        return (void*)-1;

    void* pRtn = (void*)last_valid_address;
	last_valid_address += incr;

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
int CMemoryMng2V1::brk(void* endds)
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

int CMemoryMng2V1::GetBlockCount()
{
    if(!__first)
        return 0;
    
    ASSERT(m_nCellSize);
    unsigned long nBlockSize = m_nCellSize + USED_HEADER_SIZE;

    int nCount = 1;
    struct header* pHeader = __first;

    while(pHeader!=__last)
    {
        nCount++;
        pHeader = (struct header*)((char*)pHeader + nBlockSize);
    }

    return nCount;
}
int CMemoryMng2V1::GetFreeBlockCount()
{
    if(!__rover)
        return 0;
    
    int nCount = 0;
    struct header* q = __rover;
    while(!q)
    {
        nCount++;
        q = q->next_free;
        
    }

    return nCount;

}
unsigned long CMemoryMng2V1::GetFreeBlockTotalSize()
{
    if(!__rover)
        return 0;
    
    unsigned long nSize = 0;
    struct header* q = __rover;
    while(!q)
    {
        nSize += m_nCellSize;
        q = q->next_free;        
    }
    
    return nSize;
}

#endif
//////////////////////////////////////////////////////////////////////////