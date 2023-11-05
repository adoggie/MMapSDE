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
    //07-04-23����Ӵ˽ӿ�
    /**
     * @brief   ����/�ɳ� �ռ�������
     * @note    ������Ϊ���ڲ���.ǰ��Ϊ����/�ɳ۵Ĵ�С.����Ϊ������ռ����Ĵ�С
    */
    BOOL RequireTighten (unsigned long& nDeltaSize,unsigned long& nLastSize);
    BOOL RequireRelax   (unsigned long& nDeltaSize,unsigned long& nLastSize);
    BOOL CouldTighten   (unsigned long& nDeltaSize);
    //////////////////////////////////////////////////////////////////////////
    /**
     * @brief   �����ռ�������
     * @note    ��ʹ��λ�ڴ˿ռ������ǰ,һ��Ҫ�����˲���
    */
    void Lock();
    /**
     * @brief   �����ռ�������
     * @note    ʹ����ռ�����,һ��Ҫ�ռ����������н���.
    */
    void Unlock();

//////////////////////////////////////////////////////////////////////////
public:
	enum enUpdateAspect
	{
		UPDATE_NONE = 0,
		UPDATE_FIRST = 1,

		UPDATE_RELEASE	 = UPDATE_FIRST,//���пռ伴�����ͷ�
		UPDATE_RESET,                 //���пռ伴��������,���ռ䲢û���ͷ�

		UPDATE_LAST
	};
	typedef UINT (CALLBACK* UPDATEPROC) (enUpdateAspect eAspect, DWORD dwPara);
    void Observe(UPDATEPROC lpfnUpdateProc); 
//////////////////////////////////////////////////////////////////////////
protected:
    UPDATEPROC m_lpfnUpdateProc; 

    IMemoryMng* m_pMemoryMng; //���ڴ������,ӵ�����Ķѿռ䣬���з��ڴ�������ڴ������
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
//070710��Ӵ˽ӿ�����EVCֻ֧����ʽ���쳣����,���������׳��쳣ʱ����ջ�϶������������,ע��,�˽ӿ�Ŀǰ
//    ֻ����CGIS_MapBreadthNetwork�������������ĺ���,�������벻�ɵ���.
#ifdef _WIN32_WCE
#ifdef _AFX_OLD_EXCEPTIONS
public:
    Unlock();
#endif
#endif
//////////////////////////////////////////////////////////////////////////

};

#endif 
