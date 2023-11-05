
#ifndef _MEMORY_DISPATH_FOR_NETWORKV2_H
#define _MEMORY_DISPATH_FOR_NETWORKV2_H

#include "swBase.h"

class CMemoryDispatchForNetworkV2  
{
public:
	CMemoryDispatchForNetworkV2();
	virtual ~CMemoryDispatchForNetworkV2();
    virtual BOOL InitialOperation();

    /**
     * @brief 
     *
     * @param dwMinSize             -[in]��С����ֵ
     * @param dwMaxSize             -[in]������ֵ
     * @param dwAllocateActualSize  -[out]ʵ�ʷ���Ĵ�С
     * @param dwAllocateMaxSize     -[out]���Է���������ֵ.(�ɴ�ֵ���ж���ǰ�������Ƿ���������Ŭ��������)
     * 
     * @return ��ʾ�Ƿ����ɹ�
    */
    BOOL GetMemoryMngV2(DWORD dwMinSize,DWORD dwMaxSize,DWORD& dwAllocateActualSize,DWORD& dwAllocateMaxSize);
    BOOL GetMemoryMngV3(DWORD dwMinSize,DWORD dwMaxSize,DWORD& dwAllocateActualSize,DWORD& dwAllocateMaxSize);
    BOOL MemoryInitial();
	BOOL MemoryClear();


    BOOL ReleaseMemoryMngV2();
    BOOL ReleaseMemoryMngV3();
//////////////////////////////////////////////////////////////////////////
protected:
    BOOL GetMemoryMngMaximumV2(DWORD& ,DWORD&);
    BOOL GetMemoryMngMaximumV3(DWORD& ,DWORD&);
    BOOL GetMemoryMngMaximumV3ByLimit(DWORD,DWORD& ,DWORD&);
    BOOL GetMemoryMngMaximumV3ByMaxLimitAndLeftLimt(DWORD,DWORD,DWORD& ,DWORD&);
    IMemoryMng* m_pMemoryMng;
    HANDLE	            m_hFileMap;
    PBYTE	            m_pMemAddr;
    

    DWORD m_dwMemoryMngSize;


protected:
    BOOL Reset();
    BOOL IfValid();

    BOOL ClearOperation();
	//--------------------------------------
	BOOL MallocExtendMemory(DWORD dwSize,HANDLE& hFile,PBYTE& pMem);
	BOOL FreeExtendMemory(HANDLE hFile,PBYTE pMem);
	//--------------------------------------

    BOOL MemoryInitialMixed();
	BOOL MemoryClearMixed();

    BOOL MemoryInitialSeparate();
	BOOL MemoryClearSeparate();
   
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    void DumpState();
#endif    

protected:
    BOOL m_bResetSpaceData;
    DWORD m_dwRequireSize;
};

#endif 
