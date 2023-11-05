#ifndef _MEMORYMNGSET_H
#define _MEMORYMNGSET_H

#include "swBase.h"

#include <afxtempl.h>
#include "IMemoryMngSet.h"

class CMemoryMngSetV2 : public IMemoryMngSet
{
public:
	CMemoryMngSetV2();
	virtual ~CMemoryMngSetV2();
//////////////////////////////////////////////////////////////////////////
public:
    virtual BOOL InitialOperation();
    virtual BOOL GetMemoryMng(unsigned long nSize,IMemoryMng*& pMemoryMng);
    virtual BOOL ReleaseMemoryMng(IMemoryMng* pMemoryMng);
	virtual BOOL IsMemoryOver();

//////////////////////////////////////////////////////////////////////////
protected:
    enum enHeapType
    {
        HEAP_PROCESS,
        HEAP_EXTEND,
		HEAP_INROOT,
    };

    typedef struct 
    {
        IMemoryMng*         pMemoryMng;
		unsigned long		nKey;
        unsigned long       pMemoryMngSize;
        enHeapType          eHeapType;
        HANDLE	            hFileMap;
        PBYTE	            pMemAddr;
    }stuMemoryMngItem;

    stuMemoryMngItem* m_pRootMemoryMngInfo;
    CMap <unsigned long ,unsigned long ,stuMemoryMngItem*,stuMemoryMngItem*> m_MngList;
//////////////////////////////////////////////////////////////////////////
protected:
	unsigned long GetPseudoKey();
	BOOL CreateMemoryMng(unsigned long nSize,enHeapType eHeapType,stuMemoryMngItem*& pMemoryMng);
	BOOL DestoryMemoryMng(stuMemoryMngItem* pMemoryMng);
	//--------------------------------------
	BOOL MallocExtendMemory(DWORD dwSize,HANDLE& hFile,PBYTE& pMem);
	BOOL FreeExtendMemory(HANDLE hFile,PBYTE pMem);
	//--------------------------------------
    BOOL ClearOperation();

	stuMemoryMngItem* m_pRootMemoryMng;

#ifdef _DEBUG
	virtual void DumpState();	
	virtual void AssertValid() const;	
#endif
};

#endif 
