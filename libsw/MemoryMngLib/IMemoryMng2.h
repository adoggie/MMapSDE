// IMemoryMng2.h: interface for the IMemoryMng2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMEMORYMNG2_H__AE409A8C_E536_490D_B6A4_990A79BDC276__INCLUDED_)
#define AFX_IMEMORYMNG2_H__AE409A8C_E536_490D_B6A4_990A79BDC276__INCLUDED_

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

class IMemoryMng2  
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
	IMemoryMng2(){};
	virtual ~IMemoryMng2(){};
	virtual BOOL InitialHeap(unsigned long nHeapSize,unsigned long nCellSize) = 0;
	virtual BOOL InitialHeap(void* pHeap,unsigned long nHeapSize,unsigned long nCellSize) = 0;
 	virtual BOOL ResetHeap() = 0;
    virtual void* newmalloc() = 0;
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
    virtual DWORD GetMaxCellCount() = 0;	
#endif


};

#endif // !defined(AFX_IMEMORYMNG2_H__AE409A8C_E536_490D_B6A4_990A79BDC276__INCLUDED_)
