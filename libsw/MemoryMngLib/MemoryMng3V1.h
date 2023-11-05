// MemoryMng3V1.h: interface for the CMemoryMng3V1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYMNG3V1_H__D68AB970_929A_407B_83EC_E4E4D91BFB63__INCLUDED_)
#define AFX_MEMORYMNG3V1_H__D68AB970_929A_407B_83EC_E4E4D91BFB63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMemoryMng3.h"
class IMemoryMng2;
//////////////////////////////////////////////////////////////////////////

class CMemoryMng3V1  : public IMemoryMng3
{
public:
	CMemoryMng3V1();
	virtual ~CMemoryMng3V1();
//////////////////////////////////////////////////////////////////////////
	virtual BOOL InitialHeap(unsigned long nHeapSize);
	virtual BOOL InitialHeap(void* pHeap,unsigned long nHeapSize);
 	virtual BOOL ResetHeap();
    virtual void* newmalloc(unsigned long nbytes);
    virtual void newfree(void* addr);

	virtual BOOL InitialHeap(unsigned long nHeapSize,unsigned long nCellSize);
	virtual BOOL InitialHeap(void* pHeap,unsigned long nHeapSize,unsigned long nCellSize);
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	virtual void DumpState();	
	virtual void AssertValid() const ;
    virtual void SetTextInfo(LPCTSTR strInfo);
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_DATA
public:
    virtual DWORD GetMaxHeapUsedSize();
    virtual DWORD GetMaxBlockSize();
    virtual DWORD GetMaxRequireSize();
    virtual DWORD GetMinRequireSize();
    virtual DWORD GetMaxCellCount();
#endif
protected:
    IMemoryMng2* m_pMemoryMng;
#ifdef _DEBUG
    unsigned long m_nCellSize;
#endif
};

#endif // !defined(AFX_MEMORYMNG3V1_H__D68AB970_929A_407B_83EC_E4E4D91BFB63__INCLUDED_)
