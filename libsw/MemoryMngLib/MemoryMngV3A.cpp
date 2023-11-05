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
//����ͳ�ƵĴ���
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
        ASSERT(!((DWORD)pFreeAddr&0x00000003));//����Ϊ4�ֽڶ���
       
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
	return;//*******************ע��,�������̫��ʱ,�����ȹص�********************
	
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
	nbytes = (nbytes + USED_HEADER_SIZE + 7) &0xfffffff8; //�Ŵ��Ա��ֽڶ���

	//����ڴ��������ڵĻ��ʹ�����
	if(!__first)
	{
		return create_heap(nbytes);
	}

	struct header* q = NULL;
	//�������ڴ����Ļ���������һ���տ�
	if( (q=__rover) != 0 ) // __rover  ��ʾ��ǰ�ƶ�������ָ��
	{
		do 
		{
			/* big enough to break up ?*/
			if(q->size >= nbytes + FREE_HEADER_SIZE + DELTA_FACTOR) //�����̫���ˣ��洢���˷��ˣ������Ҫ�����и������
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
//����������֮ǰ�ж���δ��ʹ�õ�,�ӿ��п�������Ƴ�ָ����
void CMemoryMngV3A:: __pull_free_block(struct header* q)
{
	struct header* p;

	if( (__rover = q->next_free) == q)
	{
		//���Ҫ�ͷŵĿ����һ���������պ���������,��ָ����ÿ�
		__rover = NULL;
	}
	else
	{
		//���Ҫ�ͷŵĿ���¸����������Լ�,�������������ǰ��ָ����Ϊ��������
		p = q->prev_free;
		__rover->prev_free = p;
		p->next_free = __rover;
	}
}

/* breaks up a large block into two pieces,
one for allocating and one which remains free
returns a pointer to the allocated block */
//��һ���㹻��Ŀ�ĺ�߷���һ����
void* CMemoryMngV3A:: allocate_partial_block(
										  struct header* q,//��λ���ָ��
										  unsigned int len//���ڵ���Ƶĳ���
										  )
{
	struct header* n;//���������ڽ��ָ��n

	//�趨��λ���Ŀ��ƿ鳤��,������λ��������,��ʹ�˽��ָ����λ���.
	q->size			-= len;
	n				= (struct header*)( (char*)q + q->size);
	n->size			= len + 1;
	n->prev_real	= q;

	if(__last == q)
	{
		//�����λ���պ������һ�������ô�����һ������Ƶ�n��,��Ϊ�ոչ�����n
		__last = n;
	}
	else
	{
		//��λ��㲻�����һ�����.��ôn����Ľ���ǰһ��ʵ��ָ��n.(ԭ������ָ�����ָ��q��.)
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

	//������չ��
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
	q = (struct header*)__sbrk( (long)len & 0xffffffff); //�����mask�ƺ�û������
	if((int)q == -1)return ((void*)NULL); //-1��ζ��û�п��ÿռ���
	__first = q; //�����ѣ�������ʼ����ֹ���ڴ�����Ϣ
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

	//��˫����������в���q
	if(__rover)
	{
        // (q | __rover => n) --------------__rover => q =>n
		n = __rover->next_free;
		__rover->next_free = q; 
		n->prev_free = q;
		q->next_free = n;
		q->prev_free = __rover;
	}
	else//�����Ա������ɿ��ж���
	{
		//û����ָ��,(��ʾ�ǵ�һ����free����)

		__rover = q;//����Ŀ���Ϊ��ָ��,������������.
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
		//���bΪ���һ����
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
//�Ӷ��ϰ����һ�������ȥ��,�ٰ������ָ����ǰŲһ����
void CMemoryMngV3A:: free_last_block(void)
{
	struct header* p;

	if( __first == __last ) /* freeing the ONLY block?*/
	{
		//ͷָ��==βָ���ʾֻ������һ����.
		brk(__first);
		__first = __last = NULL;
	}
	else
	{
		//�ҵ����һ�����ǰһ�����ӿ�,������Ϊ���һ����.
		p = __last->prev_real;

		if( (p->size & 0x0001) == 0) /* is privious block free?*/
		{
			__pull_free_block(p);//��������û��ʹ��,����������
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

	//���Ϊδʹ��
	--q->size;							/* mark it as free */
	
	/* set up pointers to the previous and next blocks*/
	n = (struct header*)((char*)q + q->size);	//���ƿ�ͽ������������ĺ���
	p = q->prev_real;							//�ҵ�������ǰһ�������ӵ�ʵ��.


	//ǰһʵ��û����ʹ���� �� ��ǰ���ֲ����׿�,��ѵ�ǰ��鲢��ǰһʵ���� 
	if( q != __first && (p->size & 0x0001) == 0) /* join to the previous block?*/
	{
		p->size += q->size;
		n->prev_real = p;
		q = p;
	 }
	 else
	 {
		 //��������鲢�����ж�����
		 insert_free_block(q); /* add it to the free queue */
	 }

	 if((n->size & 0x0001) == 0)
	 {
		 join_free_blocks(q,n);/* join with the next block?*/
	 }	
}


/*
�Ӷ���������һ���ڴ�
*/
void* CMemoryMngV3A:: __sbrk(long incr)
{
    if( (last_valid_address+incr) > m_heapEnd) ///����ʣ��Ŀռ��Ѿ�������������Ĵ�С
        return (void*)-1;

    void* pRtn = (void*)last_valid_address;
	last_valid_address += incr; //�ƶ����õĶ�ʣ��ռ�ĵ�ַָ��

    return pRtn;
}
/*
    ���ڶ�̬�ظı��������ó������ݶεĴ洢��.���ָı���ͨ��
    ���ó���Ľض�ֵ���е�,�ض�ֵ�����ݶν�β�����ϵĵ�һ��λ�õĵ�ַ.����Ĵ洢
    �ռ������Žض�ֵ�����Ӷ�����.
*/
/*
����ֵֹ�޸�Ϊendds.
����ֵ �ɹ�:0
ʧ�� -1.
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
//����ͳ�ƵĴ���
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