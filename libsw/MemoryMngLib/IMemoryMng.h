// IMemoryMng.h: interface for the IMemoryMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMEMORYMNG_H__A410EB52_98F9_4951_986D_A1D8C6E1D6FB__INCLUDED_)
#define AFX_IMEMORYMNG_H__A410EB52_98F9_4951_986D_A1D8C6E1D6FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
#define MEMMNG_DUMPSTATE(p) ((p)->DumpState());
#define MEMMNG_ASSERTVALID(p) ((p)->AssertValid());
#else
#define MEMMNG_DUMPSTATE(p) 
#define MEMMNG_ASSERTVALID(p) 
#endif

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#ifdef ANALYSE_DATA
#undef ANALYSE_DATA
#endif
#define ANALYSE_DATA
#endif
//////////////////////////////////////////////////////////////////////////

class IMemoryMng  
{
//////////////////////////////////////////////////////////////////////////
public:
    enum
    {
        KB = 1024,
        MB = 1048576,
    };
//////////////////////////////////////////////////////////////////////////
public:
	IMemoryMng(){};
	virtual ~IMemoryMng(){};
	virtual BOOL InitialHeap(unsigned long nHeapSize) = 0;
	virtual BOOL InitialHeap(void* pHeap,unsigned long nHeapSize) = 0;
 	virtual BOOL ResetHeap() = 0;
    virtual void* newmalloc(unsigned long nbytes) = 0;
    virtual void newfree(void* addr) = 0;
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	virtual void DumpState() = 0;	
	virtual void AssertValid() const = 0;
    virtual void SetTextInfo(LPCTSTR strInfo) = 0;
#endif  
//////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_DATA
    virtual DWORD GetMaxHeapUsedSize() = 0;	
    virtual DWORD GetMaxBlockSize() = 0;	
    virtual DWORD GetMaxRequireSize() = 0;	
    virtual DWORD GetMinRequireSize() = 0;	
#endif


};

#endif // !defined(AFX_IMEMORYMNG_H__A410EB52_98F9_4951_986D_A1D8C6E1D6FB__INCLUDED_)
