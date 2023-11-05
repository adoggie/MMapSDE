
#include "stdafx.h"
#include "SDEControler.h"
#include "IMemoryMngSet.h"
#include "./MemoryMngLib/MemoryMng2V1.h"
#include "./MemoryMngLib/MemoryMngV3A.h"
#include "./MemoryMngLib/MemoryMng3V1.h"


#include "GIS_Feature.h"
#include "GIS_FeaturePoint.h"
#include "GIS_FeatureLine.h"
#include "GIS_FeaturePoly.h"
#include "GIS_FeatureRut.h"
#include <memory>
using namespace std;
//////////////////////////////////////////////////////////////////////////
#include "MemoryDispatchForQueryData.h"


//////////////////////////////////////////////////////////////////////////
#define SAFE_DELETE(p) if(p){delete (p); (p) = NULL;}


CMemoryDispatchForQueryData::CMemoryDispatchForQueryData()
{
    m_pMemoryMng = NULL;
}

CMemoryDispatchForQueryData::~CMemoryDispatchForQueryData()
{
    ClearOperation();
}

//////////////////////////////////////////////////////////////////////////
BOOL CMemoryDispatchForQueryData::InitialOperation()
{
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    IMemoryMngSet* pMemoryMngSet = pSdeControl->GetMemoryMngSet();
    ASSERT(pMemoryMngSet);
    //////////////////////////////////////////////////////////////////////////

   
    IMemoryMng* pMemoryMng = NULL;
    unsigned long nRootSize = 7*IMemoryMng::KB;
	if(!pMemoryMngSet->GetMemoryMng(nRootSize,pMemoryMng))
	{
        TRACE0("can't get the query data memorymng!\n");
        return FALSE;
    }

    TRACE1("Reverse %d m for query data!\n",nRootSize>>20);

    m_pMemoryMng = pMemoryMng;
    VERIFY(MemoryInitial());
    return TRUE;
}

BOOL CMemoryDispatchForQueryData::ClearOperation()
{
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    IMemoryMngSet* pMemoryMngSet = pSdeControl->GetMemoryMngSet();
    ASSERT(pMemoryMngSet);
    //////////////////////////////////////////////////////////////////////////
    MemoryClear();
    pMemoryMngSet->ReleaseMemoryMng(m_pMemoryMng);
    m_pMemoryMng = NULL;
    
    return TRUE;
}
/**
 * @brief 当
 *
 * @param 
 * @return
 * @note 触发此函数的时机有两处,一是每次启动导航时.二是??
*/

BOOL CMemoryDispatchForQueryData::Reset()
{   
    if(!IfValid())
    {
        InitialOperation();
    }

    ASSERT(IfValid());

	MemoryClear();
    MemoryInitial();
	return TRUE;
}

BOOL CMemoryDispatchForQueryData::IfValid()
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

#ifdef _DEBUG
void CMemoryDispatchForQueryData::DumpState()
{
    TRACE0("*************CMemoryDispatchForQueryData DumpState()********************\n");
}
#endif  

//////////////////////////////////////////////////////////////////////////
//内存初邕化
BOOL CMemoryDispatchForQueryData::MemoryInitial()
{   
    ASSERT(IfValid());
    //////////////////////////////////////////////////////////////////////////
	int nDestMngHeapSize[6] = {1,1,1,1,1,1};
	
	
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
	
	dwMemSize = nDestMngHeapSize[0]*IMemoryMng::KB;
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
	
	CGIS_Feature::m_pForQueryMemMngOfSelf           = pSubMemoryMng0.release();
	CGIS_Feature::m_pForQueryMemMngOfFeaturePoint   = pSubMemoryMng1.release();
	CGIS_Feature::m_pForQueryMemMngOfFeatureLine    = pSubMemoryMng2.release();
	CGIS_Feature::m_pForQueryMemMngOfFeaturePoly    = pSubMemoryMng3.release();
	CGIS_Feature::m_pForQueryMemMngOfFeatureRut     = pSubMemoryMng4.release();
	CGIS_Feature::m_pForQueryMemMngOfBuddyData      = pSubMemoryMng5.release();
	
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	CGIS_Feature::m_pForQueryMemMngOfSelf           ->SetTextInfo(_T("Query:CGIS_Feature"));
	CGIS_Feature::m_pForQueryMemMngOfFeaturePoint   ->SetTextInfo(_T("Query:CGIS_FeaturePoint"));
	CGIS_Feature::m_pForQueryMemMngOfFeatureLine    ->SetTextInfo(_T("Query:CGIS_FeatureLine"));
	CGIS_Feature::m_pForQueryMemMngOfFeaturePoly    ->SetTextInfo(_T("Query:CGIS_FeaturePoly"));
	CGIS_Feature::m_pForQueryMemMngOfFeatureRut     ->SetTextInfo(_T("Query:CGIS_FeatureRut"));
	CGIS_Feature::m_pForQueryMemMngOfBuddyData      ->SetTextInfo(_T("Query:CGIS_Feature::m_pMemMngOfBuddyData"));
#endif
	return TRUE;
}


BOOL CMemoryDispatchForQueryData::MemoryClear()
{
    ASSERT(IfValid());
    
    SAFE_DELETE(CGIS_Feature::m_pForQueryMemMngOfSelf);
    SAFE_DELETE(CGIS_Feature::m_pForQueryMemMngOfFeaturePoint);
    SAFE_DELETE(CGIS_Feature::m_pForQueryMemMngOfFeatureLine);
    SAFE_DELETE(CGIS_Feature::m_pForQueryMemMngOfFeaturePoly);
    SAFE_DELETE(CGIS_Feature::m_pForQueryMemMngOfFeatureRut);
    SAFE_DELETE(CGIS_Feature::m_pForQueryMemMngOfBuddyData);

    //////////////////////////////////////////////////////////////////////////
    m_pMemoryMng->ResetHeap();
    return TRUE;
}