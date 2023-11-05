// MemoryMngV3A.h: interface for the CMemoryMngV3A class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYMNGV3A_H__95C7A0F6_583E_46D4_88DE_7BEF65FC3ED9__INCLUDED_)
#define AFX_MEMORYMNGV3A_H__95C7A0F6_583E_46D4_88DE_7BEF65FC3ED9__INCLUDED_

#include "IMemoryMng.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMemoryMngV3A  : public IMemoryMng
{
public:
	CMemoryMngV3A();
	virtual ~CMemoryMngV3A();
//////////////////////////////////////////////////////////////////////////
public:
	BOOL InitialHeap(unsigned long nHeapSize);
	BOOL InitialHeap(void* pHeap,unsigned long nHeapSize);
    BOOL ResetHeap();
    BOOL ClearHeap();
    void* newmalloc(unsigned long nbytes);
    void newfree(void* addr);
	//------------------------
#ifdef _DEBUG
	virtual void DumpState();	
	virtual void AssertValid() const;	
    virtual void SetTextInfo(LPCTSTR strInfo);
#endif	
//////////////////////////////////////////////////////////////////////////
protected:
	struct header
	{
		unsigned long	size; //大小包含header自身大小和数据大小

		struct header*	prev_real;//前一实块地址
		struct header*	next_free;

#ifdef _DEBUG
        DWORD m_dwFenceSign;
#endif
		struct header*	prev_free;
		
	};

    enum enConstValue
    {
        USED_HEADER_SIZE = sizeof(struct header) - 4,
        FREE_HEADER_SIZE = sizeof(struct header),
        DELTA_FACTOR = 0x100,
    };
//////////////////////////////////////////////////////////////////////////
//用于统计的代码
#ifdef ANALYSE_DATA
public:
    virtual DWORD GetMaxHeapUsedSize(){return m_MaxHeapUsedSize;};
    virtual DWORD GetMaxBlockSize(){return m_MaxBlockSize;};
    virtual DWORD GetMaxRequireSize(){return m_MaxRequireSize;};
    virtual DWORD GetMinRequireSize(){return m_MinRequireSize;};
    //==============================
protected:
    DWORD m_MaxHeapUsedSize;            //使用堆的最大大小
    DWORD m_MaxBlockSize;               //分配的最大块的大小
    DWORD m_MaxRequireSize;             //申请的最大块的大小
    DWORD m_MinRequireSize;             //申请的最大块的大小

    int GetBlockCount();
    int GetFreeBlockCount();
    unsigned long GetFreeBlockTotalSize();
#endif
//////////////////////////////////////////////////////////////////////////
protected:
	
	struct header* __first;
	struct header* __last;	//表示堆上的最后一个块,就位置而言
	struct header* __rover;	//自由链表头指针(漫指针)
	
	//堆的最后地址
	char* m_heap;
	char* m_heapEnd;
	char* last_valid_address;

    int m_nInitHeapType;

	void* newmallocActual(unsigned long nbytes);
	void  newfreeActual(void* q);

	void  __pull_free_block(struct header* q);
	void* allocate_partial_block(struct header* q,unsigned int len	);
	void* extend_heap(unsigned int len);
	void* create_heap(unsigned int len);
	void  insert_free_block(struct header* q);
	void  join_free_blocks(struct header* a,struct header* b);
	void  free_last_block(void);
	void  free_inner_block(struct header* q);
	void* __sbrk(long incr);
    int brk(void* endds);
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
protected:
    CString m_strInfo;
#endif
};

#endif // !defined(AFX_MEMORYMNGV3A_H__95C7A0F6_583E_46D4_88DE_7BEF65FC3ED9__INCLUDED_)
