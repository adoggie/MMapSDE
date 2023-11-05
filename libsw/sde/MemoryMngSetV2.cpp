// MemoryMngSetV2.cpp: implementation of the CMemoryMngSetV2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "./MemoryMngLib/MemoryMngV3A.h"
#include "./MemoryMngLib/MemoryMng2v1.h"

#include <memory>
using namespace std;

#include "MemoryMngSetV2.h"

//////////////////////////////////////////////////////////////////////////
#include "DebugFunctionLib/DebugFun.h"
/**
 * @brief   调试函数库(只在调试版本中被定义)
 *
 * 日志文件功能
 * 调试输出到屏幕功能
 * 函数调用跟踪功能
 *
 *    
 * @note    如果不想在调试版本中使能这些宏中的一项或几项,应该将这些宏在local文
 *          件中重新定义为空,即打开下面相应的注释即可.       
*/

//#define DBG2SCR0(sz)              
//#define DBG2SCR1(sz, p1)          
//#define DBG2SCR2(sz, p1, p2)      
//#define DBG2SCR3(sz, p1, p2, p3) 
//#define TRACK_FUNCTION(sz) 
//#define DBG2LOG0(sz)                
//#define DBG2LOG1(sz, p1)            
//#define DBG2LOG2(sz, p1, p2)        
//#define DBG2LOG3(sz, p1, p2, p3)    
//////////////////////////////////////////////////////////////////////////
#define DBG2LOG0(sz)                CDebugFun::DebugToLog(_T("%s"), _T(sz))
#define DBG2LOG1(sz, p1)            CDebugFun::DebugToLog(_T(sz), p1)
#define DBG2LOG2(sz, p1, p2)        CDebugFun::DebugToLog(_T(sz), p1, p2)
#define DBG2LOG3(sz, p1, p2, p3)    CDebugFun::DebugToLog(_T(sz), p1, p2, p3)
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryMngSetV2::CMemoryMngSetV2()
{
    m_pRootMemoryMng = NULL;
}

CMemoryMngSetV2::~CMemoryMngSetV2()
{
	ClearOperation();
}

//////////////////////////////////////////////////////////////////////////
BOOL CMemoryMngSetV2::InitialOperation()
{
//////////////////////////////////////////////////////////////////////////
	MEMORYSTATUS mst;
	::GlobalMemoryStatus( &mst );

#ifdef _WIN32_WCE
	if(mst.dwAvailPhys < (26*IMemoryMng::MB))
	{

		TRACE0("系统内存不足以支持导航软件的运行.\n");
		DBG2LOG0("系统内存不足以支持导航软件的运行.\n");
		return FALSE;
	}
#else
    mst.dwAvailPhys = 26*IMemoryMng::MB;
#endif	

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//---------------------保留20M的内存用于航线规划和地图显示--------------------------

    unsigned long nSize = 10*IMemoryMng::KB;
	enHeapType eHeapType  = HEAP_PROCESS;
	auto_ptr <stuMemoryMngItem> pMemoryMngItem(new stuMemoryMngItem);
    ASSERT(pMemoryMngItem.get());
    auto_ptr <IMemoryMng> pMemoryMng(new CMemoryMngV3A);
    ASSERT(pMemoryMng.get());
    if( nSize <= 0 )
        return FALSE;

	if(HEAP_EXTEND == eHeapType)
	{
		
#ifdef _WIN32_WCE
		
		if(!MallocExtendMemory(nSize,pMemoryMngItem->hFileMap,pMemoryMngItem->pMemAddr))
		{
			TRACE0("保留扩展内存失败.\n");
			return FALSE;	
		}
		
		pMemoryMngItem->eHeapType       = HEAP_EXTEND;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#else
		pMemoryMngItem->pMemAddr        = (PBYTE)malloc(nSize);
		ASSERT(pMemoryMngItem->pMemAddr);
		pMemoryMngItem->eHeapType       = HEAP_PROCESS;
#endif
	}
	else if(HEAP_PROCESS == eHeapType)
	{
		pMemoryMngItem->pMemAddr        = (PBYTE)malloc(nSize);
		ASSERT(pMemoryMngItem->pMemAddr);
		pMemoryMngItem->eHeapType       = HEAP_PROCESS;
    }
	else
	{
		ASSERT(FALSE);
	}

    VERIFY(pMemoryMng->InitialHeap(pMemoryMngItem->pMemAddr,nSize));
    
    pMemoryMngItem->pMemoryMng      = pMemoryMng.release();	
    pMemoryMngItem->pMemoryMngSize  = nSize;
	m_pRootMemoryMng = pMemoryMngItem.release();
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    m_pRootMemoryMng->pMemoryMng->SetTextInfo(_T("RootMemoryMng"));
#endif
	//MEMMNG_DUMPSTATE(this);
    return TRUE;
}
BOOL CMemoryMngSetV2::ClearOperation()
{
	stuMemoryMngItem* pMemoryMngItem = m_pRootMemoryMng;
	if(!pMemoryMngItem)
		return FALSE;
		
	if(!pMemoryMngItem->pMemoryMng)
		return FALSE;
	
	delete pMemoryMngItem->pMemoryMng;
	pMemoryMngItem->pMemoryMng = NULL;
	
	if(HEAP_PROCESS == pMemoryMngItem->eHeapType )
	{
		delete pMemoryMngItem->pMemAddr;
		pMemoryMngItem->pMemAddr = NULL;
	}
	else if(HEAP_EXTEND == pMemoryMngItem->eHeapType)
	{
		FreeExtendMemory(pMemoryMngItem->hFileMap,pMemoryMngItem->pMemAddr);
		pMemoryMngItem->hFileMap = NULL;
		pMemoryMngItem->pMemAddr = NULL;
	}
	
	pMemoryMngItem->pMemoryMngSize = 0;
	
	///////////////////////
	delete pMemoryMngItem;
	pMemoryMngItem = NULL;

	m_pRootMemoryMng = NULL;
	return TRUE;	
}

BOOL CMemoryMngSetV2::GetMemoryMng(unsigned long nSize,IMemoryMng*& pMemoryMng)
{
	unsigned long nKey;
	stuMemoryMngItem* pMemoryMngItem = NULL;
    VERIFY(CreateMemoryMng(nSize,HEAP_INROOT,pMemoryMngItem));
	//add to list
	nKey = GetPseudoKey();
	pMemoryMngItem->nKey = nKey;
    m_MngList[nKey]		= pMemoryMngItem;

	pMemoryMng = m_MngList[nKey]->pMemoryMng;
	return TRUE;
}

BOOL CMemoryMngSetV2::ReleaseMemoryMng(IMemoryMng* pMemoryMng)
{
	POSITION pos = m_MngList.GetStartPosition();
	stuMemoryMngItem* pMemoryMngItem = NULL;
    unsigned long nKey = 0xffffffff;
	while( pos )
	{
		m_MngList.GetNextAssoc(pos,nKey,pMemoryMngItem);
		if(pMemoryMngItem)
		{
			if( pMemoryMngItem->pMemoryMng == pMemoryMng )
			{
				DestoryMemoryMng(pMemoryMngItem);				
				m_MngList[nKey] = NULL;
				return TRUE;
            }
		}
	}
    return FALSE;
    
}

BOOL CMemoryMngSetV2::IsMemoryOver()
{
	MEMORYSTATUS mst;
	::GlobalMemoryStatus( &mst );

	if( (mst.dwAvailPhys>>20) < 1 )
		return TRUE;

	if( (mst.dwAvailVirtual>>20) < 1 )
		return TRUE;

	return FALSE;
}

BOOL CMemoryMngSetV2::CreateMemoryMng(unsigned long nSize,enHeapType eHeapType,stuMemoryMngItem*& pItem)
{
    auto_ptr <stuMemoryMngItem> pMemoryMngItem(new stuMemoryMngItem);
    ASSERT(pMemoryMngItem.get());

    auto_ptr <IMemoryMng> pMemoryMng(new CMemoryMngV3A);
    ASSERT(pMemoryMng.get());
   
    if( nSize <= 0 )
        return FALSE;

	if(HEAP_INROOT == eHeapType)
	{
		pMemoryMngItem->pMemAddr        = (PBYTE)m_pRootMemoryMng->pMemoryMng->newmalloc(nSize);
		ASSERT(pMemoryMngItem->pMemAddr);
		pMemoryMngItem->eHeapType       = eHeapType;

	}
	else 
	{
		ASSERT(FALSE);
	}

    VERIFY(pMemoryMng->InitialHeap(pMemoryMngItem->pMemAddr,nSize));
    
    pMemoryMngItem->pMemoryMng      = pMemoryMng.release();	
    pMemoryMngItem->pMemoryMngSize  = nSize;
    
	pItem = pMemoryMngItem.release();
	return TRUE;
}

BOOL CMemoryMngSetV2::DestoryMemoryMng(stuMemoryMngItem* pItem)
{
	stuMemoryMngItem* pMemoryMngItem = pItem;
	if(!pMemoryMngItem)
		return FALSE;
		
	if(!pMemoryMngItem->pMemoryMng)
		return FALSE;
	
	delete pMemoryMngItem->pMemoryMng;
	pMemoryMngItem->pMemoryMng = NULL;
	
	if(HEAP_INROOT == pMemoryMngItem->eHeapType )
	{
		m_pRootMemoryMng->pMemoryMng->newfree(pMemoryMngItem->pMemAddr);
		pMemoryMngItem->pMemAddr = NULL;
	}
	else	
	{
		ASSERT(FALSE);
	}
	pMemoryMngItem->pMemoryMngSize = 0;
	
	///////////////////////
	delete pMemoryMngItem;
	pMemoryMngItem = NULL;
	return TRUE;	
}
//BOOL CMemoryMngSetV2::CreateMemoryMng(unsigned long nSize,enHeapType eHeapType,stuMemoryMngItem*& pItem)
//{
//    auto_ptr <stuMemoryMngItem> pMemoryMngItem(new stuMemoryMngItem);
//    ASSERT(pMemoryMngItem.get());
//
//    auto_ptr <IMemoryMng> pMemoryMng(new CMemoryMngV3A);
//    ASSERT(pMemoryMng.get());
//   
//    if( nSize <= 0 )
//        return FALSE;
//
//	if(HEAP_EXTEND == eHeapType)
//	{
//		
//#ifdef _WIN32_WCE
//		
//		if(!MallocExtendMemory(nSize,pMemoryMngItem->hFileMap,pMemoryMngItem->pMemAddr))
//		{
//			TRACE0("保留扩展内存失败.\n");
//			return FALSE;	
//		}
//		
//		pMemoryMngItem->eHeapType       = HEAP_EXTEND;
//		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#else
//		pMemoryMngItem->pMemAddr        = (PBYTE)malloc(nSize);
//		ASSERT(pMemoryMngItem->pMemAddr);
//		pMemoryMngItem->eHeapType       = HEAP_PROCESS;
//#endif
//	}
//	else if(HEAP_PROCESS == eHeapType)
//	{
//		pMemoryMngItem->pMemAddr        = (PBYTE)malloc(nSize);
//		ASSERT(pMemoryMngItem->pMemAddr);
//		pMemoryMngItem->eHeapType       = HEAP_PROCESS;
//    }
//	else
//	{
//		ASSERT(FALSE);
//	}
//
//    VERIFY(pMemoryMng->InitialHeap(pMemoryMngItem->pMemAddr,nSize));
//    
//    pMemoryMngItem->pMemoryMng      = pMemoryMng.release();	
//    pMemoryMngItem->pMemoryMngSize  = nSize;
//    
//	pItem = pMemoryMngItem.release();
//	return TRUE;
//}
//
//BOOL CMemoryMngSetV2::DestoryMemoryMng(stuMemoryMngItem* pItem)
//{
//	stuMemoryMngItem* pMemoryMngItem = pItem;
//	if(!pMemoryMngItem)
//		return FALSE;
//		
//	if(!pMemoryMngItem->pMemoryMng)
//		return FALSE;
//	
//	delete pMemoryMngItem->pMemoryMng;
//	pMemoryMngItem->pMemoryMng = NULL;
//	
//	if(HEAP_PROCESS == pMemoryMngItem->eHeapType )
//	{
//		delete pMemoryMngItem->pMemAddr;
//		pMemoryMngItem->pMemAddr = NULL;
//	}
//	else if(HEAP_EXTEND == pMemoryMngItem->eHeapType)
//	{
//		FreeExtendMemory(pMemoryMngItem->hFileMap,pMemoryMngItem->pMemAddr);
//		pMemoryMngItem->hFileMap = NULL;
//		pMemoryMngItem->pMemAddr = NULL;
//	}
//	
//	pMemoryMngItem->pMemoryMngSize = 0;
//	
//	///////////////////////
//	delete pMemoryMngItem;
//	pMemoryMngItem = NULL;
//	return TRUE;	
//}
//##ModelId=42144B8E00A5
unsigned long CMemoryMngSetV2::GetPseudoKey()
{
	unsigned long iTmpKey;
	stuMemoryMngItem* pTmpObj;

	iTmpKey = m_MngList.GetCount();
	for(;;)
	{
		if(!m_MngList.Lookup(iTmpKey,pTmpObj))
			break;
		iTmpKey++;
	}

	return iTmpKey;
}
BOOL CMemoryMngSetV2::MallocExtendMemory(DWORD dwSize,HANDLE& hFile,PBYTE& pMem)
{
	HANDLE hFileMap;
	PBYTE pFileMem;

	hFileMap = CreateFileMapping (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, dwSize, 0);
	if (hFileMap)
	{
		pFileMem = (PBYTE)MapViewOfFile (hFileMap, FILE_MAP_WRITE, 0, 0, 0);
		if (pFileMem)
		{
			memset(pFileMem,0xcd,dwSize);
			hFile = hFileMap;
			pMem = pFileMem;
			return TRUE;
		}
		else
		{
			CloseHandle(hFileMap);
		}
	}

	return FALSE;
}
BOOL CMemoryMngSetV2::FreeExtendMemory(HANDLE hFile,PBYTE pMem)
{
	if(!hFile)
		return FALSE;
	
	ASSERT(pMem);
	VERIFY(UnmapViewOfFile (pMem));
	pMem = NULL;

	CloseHandle( hFile);
	hFile = NULL;
	return TRUE;
}

#ifdef _DEBUG
void CMemoryMngSetV2::DumpState()
{
	MEMORYSTATUS mst;
	::GlobalMemoryStatus( &mst );

	TRACE0("GlobalMemoryStatus:\n");	
	TRACE1("\tCurrent Load      [%.2d%].\n",mst.dwMemoryLoad);
	TRACE1("\tTotal Physical    [%dm].\n",mst.dwTotalPhys>>20);
	TRACE1("\tAvailable	Physical[%dm].\n",mst.dwAvailPhys>>20);
	TRACE1("\tTotal Visual      [%dm].\n",mst.dwTotalVirtual>>20);
	TRACE1("\tAvailable Visual  [%dm].\n",mst.dwAvailVirtual>>20);

}
void CMemoryMngSetV2::AssertValid() const
{
}
#endif