// MemoryMng3V1.cpp: implementation of the CMemoryMng3V1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemoryMng2V1.h"
#include "MemoryMng3V1.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryMng3V1::CMemoryMng3V1()
{
    m_pMemoryMng = new CMemoryMng2V1;
    ASSERT(m_pMemoryMng);
#ifdef _DEBUG
    m_nCellSize = 0;
#endif
}


CMemoryMng3V1::~CMemoryMng3V1()
{
    ASSERT(m_pMemoryMng);
    delete m_pMemoryMng;
    m_pMemoryMng = NULL;
}

BOOL CMemoryMng3V1::InitialHeap(unsigned long nHeapSize)
{
    ASSERT(FALSE);
    return FALSE;
}
BOOL CMemoryMng3V1::InitialHeap(void* pHeap,unsigned long nHeapSize)
{
    ASSERT(FALSE);
    return FALSE;
}
BOOL CMemoryMng3V1::ResetHeap()
{
    return m_pMemoryMng->ResetHeap();
}
void* CMemoryMng3V1::newmalloc(unsigned long nbytes)
{
#ifdef _DEBUG
    ASSERT(nbytes == m_nCellSize);
#endif
    return m_pMemoryMng->newmalloc();
}
void CMemoryMng3V1::newfree(void* addr)
{
    m_pMemoryMng->newfree(addr);
}

BOOL CMemoryMng3V1::InitialHeap(unsigned long nHeapSize,unsigned long nCellSize)
{
#ifdef _DEBUG
    m_nCellSize = nCellSize;
#endif
    return m_pMemoryMng->InitialHeap(nHeapSize,nCellSize);
}
BOOL CMemoryMng3V1::InitialHeap(void* pHeap,unsigned long nHeapSize,unsigned long nCellSize)
{
#ifdef _DEBUG
    m_nCellSize = nCellSize;
#endif
    return m_pMemoryMng->InitialHeap(pHeap,nHeapSize, nCellSize);
}

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CMemoryMng3V1::DumpState()
{
    m_pMemoryMng->DumpState();

}
void CMemoryMng3V1::AssertValid() const
{
    m_pMemoryMng->AssertValid();
}

void CMemoryMng3V1::SetTextInfo(LPCTSTR strInfo)
{
    m_pMemoryMng->SetTextInfo(strInfo);
}
#endif	
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#ifdef ANALYSE_DATA
DWORD CMemoryMng3V1::GetMaxHeapUsedSize()
{
    return m_pMemoryMng->GetMaxHeapUsedSize();
}
DWORD CMemoryMng3V1::GetMaxBlockSize()
{
    ASSERT(FALSE);
    return 0;
}
DWORD CMemoryMng3V1::GetMaxRequireSize()
{
    ASSERT(FALSE);
    return 0;
}
DWORD CMemoryMng3V1::GetMinRequireSize()
{
    ASSERT(FALSE);
    return 0;
}
DWORD CMemoryMng3V1::GetMaxCellCount()
{
    return m_pMemoryMng->GetMaxCellCount();
}
#endif
//////////////////////////////////////////////////////////////////////////


