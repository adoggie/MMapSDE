
#include "stdafx.h"
#include "SDEControler.h"
#include "GIS_MapDataManager.h"
#include "IMemoryMngSet.h"
#include "./MemoryMngLib/MemoryMng2V1.h"
#include "./MemoryMngLib/MemoryMngV3A.h"
#include "./MemoryMngLib/MemoryMng3V1.h"

//////////////////////////////////////////////////////////////////////////
#include "GIS_Feature.h"
#include "GIS_FeaturePoint.h"
#include "GIS_FeatureLine.h"
#include "GIS_FeaturePoly.h"
#include "GIS_FeatureRut.h"
//////////////////////////////////////////////////////////////////////
#define MEMORY_LEFT_MB_SIZE 3
//////////////////////////////////////////////////////////////////////////
enum enMemoryLimit
{
    LIMIT_MIN_BYTE_SIZE      = IMemoryMng::MB*3,
    LIMIT_TIGHTEN_BYTE_SIZE  = IMemoryMng::MB*6,
    LIMIT_MAX_BYTE_SIZE      = IMemoryMng::MB*8,
};
//////////////////////////////////////////////////////////////////////////
#include <memory>
using namespace std;
//////////////////////////////////////////////////////////////////////////
#include "MemoryDispatchForSpaceDataV2.h"



#define SAFE_DELETE(p) if(p){delete (p); (p) = NULL;}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryDispatchForSpaceDataV2::CMemoryDispatchForSpaceDataV2()
{
    m_pMemoryMng = NULL;
    m_hFileMap = NULL;
    m_pMemAddr = NULL;


    m_dwMemoryMngSize = 0;

}

CMemoryDispatchForSpaceDataV2::~CMemoryDispatchForSpaceDataV2()
{
    ClearOperation();

}

BOOL CMemoryDispatchForSpaceDataV2::InitialOperation()
{
#ifdef _WIN32_WCE  
	VERIFY(GetMemoryMng(9*IMemoryMng::MB));
#else
	VERIFY(GetMemoryMng(9*IMemoryMng::MB*3)); // scott 2009.1.27
#endif
    VERIFY(MemoryInitial());
    return TRUE;
}

BOOL CMemoryDispatchForSpaceDataV2::ClearOperation()
{
    VERIFY(MemoryClear());
    VERIFY(ReleaseMemoryMng());
    return TRUE;
}
/**
 * @brief 当
 *
 * @param 
 * @return
 * @note 触发此函数的时机有两处,一是每次启动导航时.二是??
*/
BOOL CMemoryDispatchForSpaceDataV2::Reset()
{   
    if(!IfValid())
        return FALSE;

    ASSERT(IfValid());

	MemoryClear();
    MemoryInitial();
	return TRUE;
}

BOOL CMemoryDispatchForSpaceDataV2::IfValid()
{
    if(m_pMemoryMng)
    {
        return TRUE;
    }
    else

    {
        return FALSE;
    }
}

BOOL CMemoryDispatchForSpaceDataV2::RequireRelease()
{
    //Lock();
	Update(UPDATE_RELEASE, 0);
    MemoryClear();
    VERIFY(ReleaseMemoryMng());
    //Unlock();
    return TRUE;
}

BOOL CMemoryDispatchForSpaceDataV2::RequireReGet(unsigned long nSize)
{
    if(nSize == m_dwMemoryMngSize)
    {
        return FALSE;
    }

    //Lock();
	Update(UPDATE_RELEASE, 0);
    MemoryClear();
    VERIFY(ReleaseMemoryMng());
    VERIFY(ReGetMemoryMng(nSize));
    MemoryInitial();
    //Unlock();
    return TRUE;
}
BOOL CMemoryDispatchForSpaceDataV2::RequireReset()
{
    //Lock();
	Update(UPDATE_RESET, 0);
    VERIFY(Reset());
    //Unlock();
    return TRUE; 
}

BOOL CMemoryDispatchForSpaceDataV2::CouldTighten   (unsigned long& nDeltaSize)
{
    if(m_dwMemoryMngSize < LIMIT_TIGHTEN_BYTE_SIZE )
        return FALSE;
    nDeltaSize = m_dwMemoryMngSize - LIMIT_MIN_BYTE_SIZE;
    return TRUE;

}
BOOL CMemoryDispatchForSpaceDataV2::RequireTighten (unsigned long& nDeltaSize,unsigned long& nLastSize)
{
    if(m_dwMemoryMngSize < LIMIT_TIGHTEN_BYTE_SIZE )
        return FALSE;
    DWORD dwOldMemoryMngSize = m_dwMemoryMngSize;

    //Lock();
	Update(UPDATE_RELEASE, 0);
    MemoryClear();
    VERIFY(ReleaseMemoryMng());
    VERIFY(ReGetMemoryMng(LIMIT_MIN_BYTE_SIZE));
    MemoryInitial();
    //Unlock();

    nDeltaSize = dwOldMemoryMngSize - m_dwMemoryMngSize;
    nLastSize = m_dwMemoryMngSize;
    return TRUE;
}
BOOL CMemoryDispatchForSpaceDataV2::RequireRelax   (unsigned long& nDeltaSize,unsigned long& nLastSize)
{
    if(m_dwMemoryMngSize >= LIMIT_MAX_BYTE_SIZE)
        return FALSE;
    DWORD dwOldMemoryMngSize = m_dwMemoryMngSize;

    //Lock();
	Update(UPDATE_RELEASE, 0);
    MemoryClear();
    VERIFY(ReleaseMemoryMng());
    if(!ReGetMemoryMng(LIMIT_MAX_BYTE_SIZE))
    {
        DWORD dwAllocateActualSize = 0;
        DWORD dwAllocateMaxSize = 0;
        VERIFY(GetMemoryMngMaximum(dwAllocateActualSize,dwAllocateMaxSize));
        TRACE2("Space relax to %d mb(%d kb).\n",dwAllocateActualSize>>20,dwAllocateActualSize>>10);
    }
    MemoryInitial();
    //Unlock();

    nDeltaSize = dwOldMemoryMngSize - m_dwMemoryMngSize;
    nLastSize = m_dwMemoryMngSize;
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CMemoryDispatchForSpaceDataV2::DumpState()
{
    TRACE0("*************CMemoryDispatchForSpaceDataV2 DumpState()********************\n");
    TRACE1("CGIS_Feature        size:%d bytes.\n",sizeof(CGIS_Feature));
    TRACE1("CGIS_FeaturePoint   size:%d bytes.\n",sizeof(CGIS_FeaturePoint));
    TRACE1("CGIS_FeatureLine    size:%d bytes.\n",sizeof(CGIS_FeatureLine));
    TRACE1("CGIS_FeaturePoly    size:%d bytes.\n",sizeof(CGIS_FeaturePoly));
    TRACE1("CGIS_FeatureRut     size:%d bytes.\n",sizeof(CGIS_FeatureRut));
}
#endif  

//////////////////////////////////////////////////////////////////////////
//内存初始化
BOOL CMemoryDispatchForSpaceDataV2::MemoryInitial()
{
    ASSERT(m_pMemoryMng);
#if 0
    return MemoryInitialSeparate();   
#else
    return MemoryInitialMixed();
#endif
}
BOOL CMemoryDispatchForSpaceDataV2::MemoryClear()
{
#if 0
     return  MemoryClearSeparate();  
#else
    return MemoryClearMixed();
#endif
}

BOOL CMemoryDispatchForSpaceDataV2::MemoryInitialMixed()
{
    ASSERT(m_pMemoryMng);
    IMemoryMng* pMemoryMng = m_pMemoryMng;
    pMemoryMng->ResetHeap();

	
	CGIS_Feature::m_pMemMngOfSelf           = pMemoryMng;
	CGIS_Feature::m_pMemMngOfFeaturePoint   = pMemoryMng;
	CGIS_Feature::m_pMemMngOfFeatureLine    = pMemoryMng;
	CGIS_Feature::m_pMemMngOfFeaturePoly    = pMemoryMng;
	CGIS_Feature::m_pMemMngOfFeatureRut     = pMemoryMng;
	CGIS_Feature::m_pMemMngOfBuddyData      = pMemoryMng;
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    pMemoryMng ->SetTextInfo(_T("SpaceData"));
#endif

	return TRUE;
}
BOOL CMemoryDispatchForSpaceDataV2::MemoryClearMixed()
{
    ASSERT(m_pMemoryMng);
    //MEMMNG_DUMPSTATE(m_pMemoryMng);
    return TRUE;
}


//创建缓冲数据堆 ,此函数未被使用
BOOL CMemoryDispatchForSpaceDataV2::MemoryInitialSeparate()
{   
    ASSERT(IfValid());
    //////////////////////////////////////////////////////////////////////////
		//这里是对将要创建的内存管理的堆大小的值 
	int nDestMngHeapSize[6] = {1,898,600,600,1400,4500}; //4500 = 4MB
    int nTotal = 8000;
    for(int i = 0; i < 6; i++)
    {
        nDestMngHeapSize[i] = nDestMngHeapSize[i]*(m_dwMemoryMngSize>>10)/8000;
    }
	//pMemoryMng用于第一步进行分配堆给其他的内存管理器对象
    IMemoryMng* pMemoryMng = m_pMemoryMng;
	pMemoryMng->ResetHeap();
	
	//先把对象做出来
	auto_ptr <IMemoryMng3> pSubMemoryMng0(new CMemoryMng3V1);
	if(!pSubMemoryMng0.get())return FALSE;
	auto_ptr <IMemoryMng3> pSubMemoryMng1(new CMemoryMng3V1);
	if(!pSubMemoryMng1.get())return FALSE;
	auto_ptr <IMemoryMng3> pSubMemoryMng2(new CMemoryMng3V1);
	if(!pSubMemoryMng2.get())return FALSE;
	auto_ptr <IMemoryMng3> pSubMemoryMng3(new CMemoryMng3V1);
	if(!pSubMemoryMng3.get())return FALSE;
	auto_ptr <IMemoryMng3> pSubMemoryMng4(new CMemoryMng3V1);
	if(!pSubMemoryMng4.get())return FALSE;
	auto_ptr <IMemoryMng> pSubMemoryMng5(new CMemoryMngV3A);
	if(!pSubMemoryMng5.get())return FALSE;
	
	
	
	//分配空间数据区
	DWORD dwMemSize = 0;
	DWORD dwCellSize = 0;
	void* pHeap = NULL;
	
	dwMemSize = 1*IMemoryMng::KB;
	dwCellSize = sizeof(CGIS_Feature);
	pHeap = pMemoryMng->newmalloc(dwMemSize);
	ASSERT(pHeap);
	VERIFY(pSubMemoryMng0->InitialHeap(pHeap,dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[1]*IMemoryMng::KB;
	dwCellSize = sizeof(CGIS_FeaturePoint);
	pHeap = pMemoryMng->newmalloc(dwMemSize);
	ASSERT(pHeap);
	VERIFY(pSubMemoryMng1->InitialHeap(pHeap,dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[2]*IMemoryMng::KB;
	dwCellSize = sizeof(CGIS_FeatureLine);
	pHeap = pMemoryMng->newmalloc(dwMemSize);
	ASSERT(pHeap);
	VERIFY(pSubMemoryMng2->InitialHeap(pHeap,dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[3]*IMemoryMng::KB;
	dwCellSize = sizeof(CGIS_FeaturePoly);
	pHeap = pMemoryMng->newmalloc(dwMemSize);
	ASSERT(pHeap);
	VERIFY(pSubMemoryMng3->InitialHeap(pHeap,dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[4]*IMemoryMng::KB;
	dwCellSize = sizeof(CGIS_FeatureRut);
	pHeap = pMemoryMng->newmalloc(dwMemSize);
	ASSERT(pHeap);
	VERIFY(pSubMemoryMng4->InitialHeap(pHeap,dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[5]*IMemoryMng::KB;
	dwCellSize = 0;
	pHeap = pMemoryMng->newmalloc(dwMemSize);
	ASSERT(pHeap);
	VERIFY(pSubMemoryMng5->InitialHeap(pHeap,dwMemSize));
	
	CGIS_Feature::m_pMemMngOfSelf           = pSubMemoryMng0.release();
	CGIS_Feature::m_pMemMngOfFeaturePoint   = pSubMemoryMng1.release();
	CGIS_Feature::m_pMemMngOfFeatureLine    = pSubMemoryMng2.release();
	CGIS_Feature::m_pMemMngOfFeaturePoly    = pSubMemoryMng3.release();
	CGIS_Feature::m_pMemMngOfFeatureRut     = pSubMemoryMng4.release();
	CGIS_Feature::m_pMemMngOfBuddyData      = pSubMemoryMng5.release();
	
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	CGIS_Feature::m_pMemMngOfSelf           ->SetTextInfo(_T("Space:CGIS_Feature"));
	CGIS_Feature::m_pMemMngOfFeaturePoint   ->SetTextInfo(_T("Space:CGIS_FeaturePoint"));
	CGIS_Feature::m_pMemMngOfFeatureLine    ->SetTextInfo(_T("Space:CGIS_FeatureLine"));
	CGIS_Feature::m_pMemMngOfFeaturePoly    ->SetTextInfo(_T("Space:CGIS_FeaturePoly"));
	CGIS_Feature::m_pMemMngOfFeatureRut     ->SetTextInfo(_T("Space:CGIS_FeatureRut"));
	CGIS_Feature::m_pMemMngOfBuddyData      ->SetTextInfo(_T("Space:CGIS_Feature::m_pMemMngOfBuddyData"));
#endif
	return TRUE;
}


BOOL CMemoryDispatchForSpaceDataV2::MemoryClearSeparate()
{
    ASSERT(IfValid());
    
    SAFE_DELETE(CGIS_Feature::m_pMemMngOfSelf);
    SAFE_DELETE(CGIS_Feature::m_pMemMngOfFeaturePoint);
    SAFE_DELETE(CGIS_Feature::m_pMemMngOfFeatureLine);
    SAFE_DELETE(CGIS_Feature::m_pMemMngOfFeaturePoly);
    SAFE_DELETE(CGIS_Feature::m_pMemMngOfFeatureRut);
    SAFE_DELETE(CGIS_Feature::m_pMemMngOfBuddyData);

    //////////////////////////////////////////////////////////////////////////
    m_pMemoryMng->ResetHeap();
    return TRUE;
}
/**
 * @brief 请求给定大小的内存
 *
 * @param 
 * @return
*/
BOOL CMemoryDispatchForSpaceDataV2::GetMemoryMng(DWORD dwMemorySize)
{

    ASSERT(!m_pMemoryMng);   
    
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    IMemoryMngSet* pMemoryMngSet = pSdeControl->GetMemoryMngSet();
    ASSERT(pMemoryMngSet); 
    //////////////////////////////////////////////////////////////////////////
    
    {
        //取内存状态作判断     
        MEMORYSTATUS mst;
        ::GlobalMemoryStatus( &mst );	
        //TRACE1("\tcurrent load[%.2d%].\n",mst.dwMemoryLoad);
        //TRACE1("\tavaliable physics[%dm].\n",mst.dwAvailPhys>>20);
        //TRACE1("\tavaliable virtual[%dm].\n",mst.dwAvailVirtual>>20);
        
#ifdef _WIN32_WCE        
        DWORD dwAvaliablePhys = mst.dwAvailPhys>>20;
        DWORD dwMemoryRequireSize = (dwMemorySize>>20) + 2;

        if( dwAvaliablePhys > dwMemoryRequireSize )
        {
            unsigned long nRootSize = dwMemorySize;
            if(!MallocExtendMemory(nRootSize,m_hFileMap,m_pMemAddr))
            {
                TRACE0("Reverse extend mem for spacedata failed!\n");
                return FALSE;	
            }
            IMemoryMng* pMemoryMng = new CMemoryMngV3A;
            ASSERT(pMemoryMng); 
            pMemoryMng->InitialHeap(m_pMemAddr,nRootSize);
            
            m_pMemoryMng = pMemoryMng;
            m_dwMemoryMngSize = nRootSize;
            TRACE1("Reverse extend mem %dm (series) for spacedata!\n",nRootSize>>20);
            return TRUE;
            
        }
        else
        {
            TRACE0("内存不足!\n");
            return FALSE;
        }
#else
        {
            unsigned long nRootSize = dwMemorySize;
            if(!MallocExtendMemory(nRootSize,m_hFileMap,m_pMemAddr))
            {
                TRACE0("Reverse extend mem for spacedata failed!\n");
                return FALSE;	
            }
            IMemoryMng* pMemoryMng = new CMemoryMngV3A;
            ASSERT(pMemoryMng); 
            pMemoryMng->InitialHeap(m_pMemAddr,nRootSize);
            
            m_pMemoryMng = pMemoryMng;
            m_dwMemoryMngSize = nRootSize;
            TRACE1("Reverse extend mem %dm (series) for spacedata!\n",nRootSize>>20);
            return TRUE; 
        }     
#endif    
    }
             
    return FALSE;
}

/**
 * @brief 尽最大能力分配
 *
 * @param 
 * @return
*/
BOOL CMemoryDispatchForSpaceDataV2::GetMemoryMngMaximum(DWORD& dwAllocateActualSize,DWORD& dwAllocateMaxSize)
{

    ASSERT(!m_pMemoryMng);   
    
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    IMemoryMngSet* pMemoryMngSet = pSdeControl->GetMemoryMngSet();
    ASSERT(pMemoryMngSet); 
    CMemoryDispatchForSpaceDataV2* pSpaceDispatch = pSdeControl->GetMemoryDispatchForSpace();
    ASSERT(pSpaceDispatch);
    //////////////////////////////////////////////////////////////////////////
    
    {
        //取内存状态作判断     
        MEMORYSTATUS mst;
        ::GlobalMemoryStatus( &mst );	
        //TRACE1("\tcurrent load      :%.2d%.\n",mst.dwMemoryLoad);
        //TRACE1("\tavaliable physics :%dm.\n",mst.dwAvailPhys>>20);
        //TRACE1("\tavaliable virtual :%dm.\n",mst.dwAvailVirtual>>20);
        
#ifdef _WIN32_WCE        
        DWORD dwAvaliablePhys = mst.dwAvailPhys>>20;
        ASSERT(dwAvaliablePhys > 6 );
        {
            unsigned long nRootSize = (dwAvaliablePhys-MEMORY_LEFT_MB_SIZE) *IMemoryMng::MB;
            if(!MallocExtendMemory(nRootSize,m_hFileMap,m_pMemAddr))
            {
                TRACE0("Reverse extend mem for network failed!\n");
                return FALSE;	
            }
            IMemoryMng* pMemoryMng = new CMemoryMngV3A;
            ASSERT(pMemoryMng); 
            pMemoryMng->InitialHeap(m_pMemAddr,nRootSize);
            
            m_pMemoryMng = pMemoryMng;
            m_dwMemoryMngSize = nRootSize;

            dwAllocateActualSize = nRootSize;
            dwAllocateMaxSize = nRootSize;
            return TRUE;          
        }
#else
        {
            unsigned long nRootSize = 8 * IMemoryMng::MB;
            if(!MallocExtendMemory(nRootSize,m_hFileMap,m_pMemAddr))
            {
                TRACE0("Reverse extend mem for network failed!\n");
                return FALSE;	
            }
            IMemoryMng* pMemoryMng = new CMemoryMngV3A;
            ASSERT(pMemoryMng); 
            pMemoryMng->InitialHeap(m_pMemAddr,nRootSize);
            
            m_pMemoryMng = pMemoryMng;
            m_dwMemoryMngSize = nRootSize;

            dwAllocateActualSize = nRootSize;
            dwAllocateMaxSize = nRootSize;
            return TRUE; 
        }     
#endif    
    }
             
    return FALSE;
}

BOOL CMemoryDispatchForSpaceDataV2::ReGetMemoryMng(unsigned long nSize)
{
    return GetMemoryMng(nSize);
}
BOOL CMemoryDispatchForSpaceDataV2::ReleaseMemoryMng()
{
    ASSERT(m_pMemoryMng);
    delete m_pMemoryMng;
    m_pMemoryMng = NULL;
    FreeExtendMemory(m_hFileMap,m_pMemAddr);   
    m_hFileMap = NULL;
    m_pMemAddr = NULL;

    return TRUE;
}


void CMemoryDispatchForSpaceDataV2::Observe(UPDATEPROC lpfnUpdateProc)
{
    m_lpfnUpdateProc = lpfnUpdateProc;
}
UINT CMemoryDispatchForSpaceDataV2::Update(enUpdateAspect eAspect, DWORD dwPara)
{
    UPDATEPROC UpdateProc =  m_lpfnUpdateProc;
    ASSERT(UpdateProc);
	return (*UpdateProc)(eAspect, dwPara);
}


BOOL CMemoryDispatchForSpaceDataV2::MallocExtendMemory(DWORD dwSize,HANDLE& hFile,PBYTE& pMem)
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

BOOL CMemoryDispatchForSpaceDataV2::FreeExtendMemory(HANDLE hFile,PBYTE pMem)
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
void CMemoryDispatchForSpaceDataV2::Lock()
{
	/*
    CSDEControler* pControl = CSDEControler::GetSDEControl();
	ASSERT( pControl );
	CGIS_MapDataManager* pMapManager = NULL;
	if( pControl )
		pMapManager = (CGIS_MapDataManager*)pControl->GetMapManager();
	ASSERT( pMapManager );
	CEvent* pSpaceMemEvent = pMapManager->GetSpaceMemEvent();
	ASSERT( pSpaceMemEvent );
	if( pSpaceMemEvent )
	{
		WaitForSingleObject( pSpaceMemEvent->m_hObject , INFINITE );
		pSpaceMemEvent->ResetEvent();
	}*/
	CSDEControler::GetSDEControl()->GetMapManager()->lock();
}
void CMemoryDispatchForSpaceDataV2::Unlock()
{
	/*
    CSDEControler* pControl = CSDEControler::GetSDEControl();
	ASSERT( pControl );
	CGIS_MapDataManager* pMapManager = NULL;
	if( pControl )
		pMapManager = (CGIS_MapDataManager*)pControl->GetMapManager();
	ASSERT( pMapManager );
	CEvent* pSpaceMemEvent = pMapManager->GetSpaceMemEvent();
	ASSERT( pSpaceMemEvent );
	if( pSpaceMemEvent )
	{
		pSpaceMemEvent->SetEvent();
	}*/
	CSDEControler::GetSDEControl()->GetMapManager()->unlock();
}

//////////////////////////////////////////////////////////////////////////

CSpaceMemoryLock::CSpaceMemoryLock()
{
	CSDEControler* pControl = CSDEControler::GetSDEControl();
	ASSERT( pControl );
    CMemoryDispatchForSpaceDataV2* pDispatch = pControl->GetMemoryDispatchForSpace();
    ASSERT(pDispatch);
    //////////////////////////////////////////////////////////////////////////
    
    pDispatch->Lock();
}

CSpaceMemoryLock::~CSpaceMemoryLock()
{
	CSDEControler* pControl = CSDEControler::GetSDEControl();
	ASSERT( pControl );
    CMemoryDispatchForSpaceDataV2* pDispatch = pControl->GetMemoryDispatchForSpace();
    ASSERT(pDispatch);
    //////////////////////////////////////////////////////////////////////////
    
    pDispatch->Unlock();    
}

//---------------------------
//070710添加此代码由于EVC只支持老式的异常处理,做不到在抛出异常时调用栈上对象的析构函数
#ifdef _WIN32_WCE
#ifdef _AFX_OLD_EXCEPTIONS
       
CSpaceMemoryLock::Unlock()
{
	CSDEControler* pControl = CSDEControler::GetSDEControl();
	ASSERT( pControl );
    CMemoryDispatchForSpaceDataV2* pDispatch = pControl->GetMemoryDispatchForSpace();
    ASSERT(pDispatch);
    //////////////////////////////////////////////////////////////////////////
    
    pDispatch->Unlock(); 
}

#endif
#endif
//////////////////////////////////////////////////////////////////////////
