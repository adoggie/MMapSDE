
#ifndef _IMEMORYMNGSET_H
#define _IMEMORYMNGSET_H

#include "swBase.h"

#ifdef _DEBUG
#define MEMMNG_DUMPSTATE(p) ((p)->DumpState());
#define MEMMNG_ASSERTVALID(p) ((p)->AssertValid());
#else
#define MEMMNG_DUMPSTATE(p) 
#define MEMMNG_ASSERTVALID(p) 
#endif

class IMemoryMng;
//////////////////////////////////////////////////////////////////////////
class IMemoryMngSet  
{
public:
	IMemoryMngSet();
	virtual ~IMemoryMngSet();   
//////////////////////////////////////////////////////////////////////////
public:
    virtual BOOL InitialOperation() = 0;
	virtual BOOL IsMemoryOver() = 0;

    virtual BOOL GetMemoryMng(unsigned long nSize,IMemoryMng*& pMemoryMng) = 0;
    virtual BOOL ReleaseMemoryMng(IMemoryMng* pMemoryMng) = 0;

#ifdef _DEBUG
	virtual void DumpState() = 0;	
	virtual void AssertValid() const = 0;	
#endif
//////////////////////////////////////////////////////////////////////////

};

#endif 
