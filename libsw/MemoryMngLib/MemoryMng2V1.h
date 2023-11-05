// MemoryMng2V1.h: interface for the CMemoryMng2V1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYMNG2V1_H__42A7E384_078F_490C_9494_440F847FAE75__INCLUDED_)
#define AFX_MEMORYMNG2V1_H__42A7E384_078F_490C_9494_440F847FAE75__INCLUDED_

#include "IMemoryMng2.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMemoryMng2V1  : public IMemoryMng2
{
public:
	CMemoryMng2V1();
	virtual ~CMemoryMng2V1();
	virtual BOOL InitialHeap(unsigned long nHeapSize,unsigned long nCellSize);
	virtual BOOL InitialHeap(void* pHeap,unsigned long nHeapSize,unsigned long nCellSize);
 	virtual BOOL ResetHeap() ;
	virtual BOOL ClearHeap();
    virtual void* newmalloc() ;
    virtual void newfree(void* addr);
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    virtual void DumpState();	
	virtual void AssertValid() const ;	
    virtual void SetTextInfo(LPCTSTR strInfo);
#endif
//////////////////////////////////////////////////////////////////////////
protected:
	struct header
	{
		struct header*	next_free;//前一实块地址
#ifdef _DEBUG
        DWORD m_dwFenceSign;
#endif
		struct header*	prev_free;
		
	};

    enum enConstValue
    {
        USED_HEADER_SIZE = sizeof(struct header) - 4,
    };
//////////////////////////////////////////////////////////////////////////
//用于统计的代码
#ifdef ANALYSE_DATA
public:
    virtual DWORD GetMaxHeapUsedSize(){return m_MaxHeapUsedSize;};
    virtual DWORD GetMaxCellCount(){return m_MaxCellCount;};
    //======================================================
protected:
    DWORD m_MaxHeapUsedSize;            //使用堆的最大大小
    DWORD m_MaxCellCount;

    int GetBlockCount();
    int GetFreeBlockCount();
    unsigned long GetFreeBlockTotalSize();
#endif
//////////////////////////////////////////////////////////////////////////
protected:
	
	struct header* __first;
	struct header* __last;	//表示堆上的最后一个块,就位置而言
	struct header* __rover;	//指向栈顶
	
	//堆的最后地址
	char* m_heap;
	char* m_heapEnd;
	char* last_valid_address;

	unsigned long m_nCellSize;
    int m_nInitHeapType;

	void* newmallocActual();
	void  newfreeActual(void* q);

	void* allocate_partial_block(struct header* q,unsigned int len	);
	void* extend_heap(unsigned int len);
	void* create_heap(unsigned int len);
	void  free_last_block(void);
	void* __sbrk(long incr);
    int brk(void* endds);
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
protected:
    CString m_strInfo;
#endif
};

#endif // !defined(AFX_MEMORYMNG2V1_H__42A7E384_078F_490C_9494_440F847FAE75__INCLUDED_)
