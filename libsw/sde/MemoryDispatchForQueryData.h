#ifndef _MEMORY_DISPATH_FOR_QUERYDATA_H
#define _MEMORY_DISPATH_FOR_QUERYDATA_H

#include "swBase.h"

class CMemoryDispatchForQueryData  
{
public:
	CMemoryDispatchForQueryData();
	virtual ~CMemoryDispatchForQueryData();
    virtual BOOL InitialOperation();

    BOOL Reset();
    BOOL IfValid();
//////////////////////////////////////////////////////////////////////////
protected:
    IMemoryMng* m_pMemoryMng;
protected:
    BOOL MemoryInitial();
	BOOL MemoryClear();
    BOOL ClearOperation();

    
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    void DumpState();
#endif    
    
};

#endif 
