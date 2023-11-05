#ifndef _MEMORY_DISPATCH_FOR_SPACEDATAV2_H
#define _MEMORY_DISPATCH_FOR_SPACEDATAV2_H

#include "swBase.h"

class CMemoryDispatchForSpaceDataV2  
{
public:
	CMemoryDispatchForSpaceDataV2();
	virtual ~CMemoryDispatchForSpaceDataV2();
    virtual BOOL InitialOperation();

    BOOL IfValid();

    BOOL RequireRelease();
    BOOL RequireReGet(unsigned long nSize);
    BOOL RequireReset();


    //////////////////////////////////////////////////////////////////////////
    //07-04-23日添加此接口
    /**
     * @brief   收缩/松弛 空间数据区
     * @note    参数均为出口参数.前者为收缩/松驰的大小.后者为操作后空间区的大小
    */
    BOOL RequireTighten (unsigned long& nDeltaSize,unsigned long& nLastSize);
    BOOL RequireRelax   (unsigned long& nDeltaSize,unsigned long& nLastSize);
    BOOL CouldTighten   (unsigned long& nDeltaSize);
    //////////////////////////////////////////////////////////////////////////
    /**
     * @brief   锁定空间数据区
     * @note    在使用位于此空间的数据前,一定要先做此操作
    */
    void Lock();
    /**
     * @brief   解锁空间数据区
     * @note    使用完空间数据,一定要空间数据区进行解锁.
    */
    void Unlock();

//////////////////////////////////////////////////////////////////////////
public:
	enum enUpdateAspect
	{
		UPDATE_NONE = 0,
		UPDATE_FIRST = 1,

		UPDATE_RELEASE	 = UPDATE_FIRST,//所有空间即将被释放
		UPDATE_RESET,                 //所有空间即将被重置,但空间并没有释放

		UPDATE_LAST
	};
	typedef UINT (CALLBACK* UPDATEPROC) (enUpdateAspect eAspect, DWORD dwPara);
    void Observe(UPDATEPROC lpfnUpdateProc); 
//////////////////////////////////////////////////////////////////////////
protected:
    UPDATEPROC m_lpfnUpdateProc; 

    IMemoryMng* m_pMemoryMng; //根内存管理器,拥有最大的堆空间，再切分内存给其他内存管理器
    HANDLE	            m_hFileMap;
    PBYTE	            m_pMemAddr;
    
    DWORD m_dwMemoryMngSize;
    
protected:
    BOOL Reset();
    
    BOOL MemoryInitial();
	BOOL MemoryClear();

    BOOL MemoryInitialMixed();
	BOOL MemoryClearMixed();

    BOOL MemoryInitialSeparate();
	BOOL MemoryClearSeparate();

    BOOL GetMemoryMng();
    BOOL GetMemoryMng(DWORD dwMemorySize);
    BOOL GetMemoryMngMaximum(DWORD& dwAllocateActualSize,DWORD& dwAllocateMaxSize);
    
    BOOL ReGetMemoryMng(unsigned long nSize);
    BOOL ReleaseMemoryMng();
    
    BOOL ClearOperation();
    UINT Update(enUpdateAspect eAspect, DWORD dwPara);
	//--------------------------------------
	BOOL MallocExtendMemory(DWORD dwSize,HANDLE& hFile,PBYTE& pMem);
	BOOL FreeExtendMemory(HANDLE hFile,PBYTE pMem);
	//--------------------------------------


    
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    void DumpState();
#endif    
};

class CSpaceMemoryLock
{ 
public:
    CSpaceMemoryLock();
    ~CSpaceMemoryLock();

//////////////////////////////////////////////////////////////////////////
//070710添加此接口由于EVC只支持老式的异常处理,做不到在抛出异常时调用栈上对象的析构函数,注意,此接口目前
//    只用于CGIS_MapBreadthNetwork的两个用于填充的函数,其它代码不可调用.
#ifdef _WIN32_WCE
#ifdef _AFX_OLD_EXCEPTIONS
public:
    Unlock();
#endif
#endif
//////////////////////////////////////////////////////////////////////////

};

#endif 
