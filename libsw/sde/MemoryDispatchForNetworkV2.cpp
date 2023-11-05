// MemoryDispatchForNetworkV2.cpp: implementation of the CMemoryDispatchForNetworkV2V2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SDEControler.h"
#include "IMemoryMngSet.h"
#include "./MemoryMngLib/MemoryMng2V1.h"
#include "./MemoryMngLib/MemoryMngV3A.h"
#include "./MemoryMngLib/MemoryMng3V1.h"

#include "MemoryDispatchForSpaceDataV2.h"

//////////////////////////////////////////////////////////////////////////
#include "IGIS_MapNetwork.h"
#include "GIS_MBBNetwork.h"
//////////////////////////////////////////////////////////////////////////
/**
 * ���������ʹ��·���滮ʱ���ڴ����
 *
 * 
 *
*/
#ifdef MEMORY_ANALYSE_IN_NETWORK
#undef MEMORY_ANALYSE_IN_NETWORK
#endif

//#define MEMORY_ANALYSE_IN_NETWORK
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
#include "../DebugFunctionLib/DebugFun.h"
/**
 * @brief   ���Ժ�����(ֻ�ڵ��԰汾�б�����)
 *
 * ��־�ļ�����
 * �����������Ļ����
 * �������ø��ٹ���
 *
 *    
 * @note    ��������ڵ��԰汾��ʹ����Щ���е�һ�����,Ӧ�ý���Щ����local��
 *          �������¶���Ϊ��,����������Ӧ��ע�ͼ���.       
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

//////////////////////////////////////////////////////////////////////////
#include <memory>
using namespace std;
//////////////////////////////////////////////////////////////////////////
#include "MemoryDispatchForNetworkV2.h"

#define MEMORY_NORMAL_LEFT_MB_SIZE      3
#define MEMORY_INSTANCY_LEFT_MB_SIZE    2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryDispatchForNetworkV2::CMemoryDispatchForNetworkV2()
{
    m_pMemoryMng = NULL;
    m_hFileMap = NULL;
    m_pMemAddr = NULL;


    m_dwMemoryMngSize = 0;
    m_bResetSpaceData = FALSE;


}

CMemoryDispatchForNetworkV2::~CMemoryDispatchForNetworkV2()
{
    ClearOperation();
}


BOOL CMemoryDispatchForNetworkV2::InitialOperation()
{
    return TRUE;
}

BOOL CMemoryDispatchForNetworkV2::ClearOperation()
{
    //////////////////////////////////////////////////////////////////////////
    //MemoryClear();    
    return TRUE;
}


BOOL CMemoryDispatchForNetworkV2::MallocExtendMemory(DWORD dwSize,HANDLE& hFile,PBYTE& pMem)
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

BOOL CMemoryDispatchForNetworkV2::FreeExtendMemory(HANDLE hFile,PBYTE pMem)
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

/**
 * @brief ��
 *
 * @param 
 * @return
 * @note �����˺�����ʱ��������,һ��ÿ����������ʱ.����??
*/

BOOL CMemoryDispatchForNetworkV2::Reset()
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

BOOL CMemoryDispatchForNetworkV2::IfValid()
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
void CMemoryDispatchForNetworkV2::DumpState()
{
    TRACE0("*************CMemoryDispatchForNetworkV2 DumpState()********************\n");
    //TRACE1("CGIS_Feature        size:%d bytes.\n",sizeof(CGIS_Feature));
    //TRACE1("CGIS_FeaturePoint   size:%d bytes.\n",sizeof(CGIS_FeaturePoint));
    //TRACE1("CGIS_FeatureLine    size:%d bytes.\n",sizeof(CGIS_FeatureLine));
    //TRACE1("CGIS_FeaturePoly    size:%d bytes.\n",sizeof(CGIS_FeaturePoly));
    //TRACE1("CGIS_FeatureRut     size:%d bytes.\n",sizeof(CGIS_FeatureRut));
}
#endif  

/**
 * @brief �������������
 *
 * @param 
 * @return
*/
BOOL CMemoryDispatchForNetworkV2::GetMemoryMngMaximumV2(DWORD& dwAllocateActualSize,DWORD& dwAllocateMaxSize)
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
        //ȡ�ڴ�״̬���ж�     
        MEMORYSTATUS mst;
        ::GlobalMemoryStatus( &mst );	
        //TRACE1("\tcurrent load      :%.2d%.\n",mst.dwMemoryLoad);
        //TRACE1("\tavaliable physics :%dm.\n",mst.dwAvailPhys>>20);
        //TRACE1("\tavaliable virtual :%dm.\n",mst.dwAvailVirtual>>20);
        
#ifdef _WIN32_WCE        
        DWORD dwAvaliablePhys = mst.dwAvailPhys>>20;
        {
            {
                CSpaceMemoryLock aLock;
                CSDEControler::GetSDEControl()->GetMemoryDispatchForSpace()->RequireReGet(3*IMemoryMng::MB);
                m_bResetSpaceData = TRUE;
            }
            //////////////////////////////////////////////////////////////////////////

            unsigned long nRootSize = (dwAvaliablePhys+5 - MEMORY_NORMAL_LEFT_MB_SIZE) *IMemoryMng::MB;
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
            unsigned long nRootSize = 12 * IMemoryMng::MB;
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
/**
 * @brief �������������
 *
 * @param 
 * @return
*/
BOOL CMemoryDispatchForNetworkV2::GetMemoryMngMaximumV3(DWORD& dwAllocateActualSize,DWORD& dwAllocateMaxSize)
{

    ASSERT(!m_pMemoryMng);   
    
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    IMemoryMngSet* pMemoryMngSet = pSdeControl->GetMemoryMngSet();
    ASSERT(pMemoryMngSet); 
    CMemoryDispatchForSpaceDataV2* pSpaceDispatch = pSdeControl->GetMemoryDispatchForSpace();
    ASSERT(pSpaceDispatch);
    //////////////////////////////////////////////////////////////////////////
    unsigned long nSpaceDeltaSize = 0;
    if(!pSpaceDispatch->CouldTighten(nSpaceDeltaSize))
    {
        TRACE0("MemoryForSpaceCan't be tighten.!\n");
    }
    //////////////////////////////////////////////////////////////////////////
    {
        //ȡ�ڴ�״̬���ж�     
        MEMORYSTATUS mst;
        ::GlobalMemoryStatus( &mst );	
        //TRACE1("\tcurrent load      :%.2d%.\n",mst.dwMemoryLoad);
        //TRACE1("\tavaliable physics :%dm.\n",mst.dwAvailPhys>>20);
        //TRACE1("\tavaliable virtual :%dm.\n",mst.dwAvailVirtual>>20);
        
#ifdef _WIN32_WCE        
        DWORD dwAvaliablePhys = mst.dwAvailPhys;
        if( (dwAvaliablePhys + nSpaceDeltaSize - (MEMORY_INSTANCY_LEFT_MB_SIZE<<20)) < (4<<20) )
            return FALSE;
        
        {
            unsigned long nDeltaSize;
            unsigned long nSpaceLastSize;
             
            {
                CSpaceMemoryLock aLock;
                if(CSDEControler::GetSDEControl()->GetMemoryDispatchForSpace()->RequireTighten(nDeltaSize,nSpaceLastSize))
                {
                    m_bResetSpaceData = TRUE;                    
                }
            }
            //////////////////////////////////////////////////////////////////////////
            unsigned long nRootSize = dwAvaliablePhys+nDeltaSize - (MEMORY_INSTANCY_LEFT_MB_SIZE<<20);
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
            unsigned long nRootSize = 12 * IMemoryMng::MB;
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
/**
 * @brief ������������䵫�ǲ���������������
 *
 * @param 
 * @return
*/
BOOL CMemoryDispatchForNetworkV2::GetMemoryMngMaximumV3ByLimit(
    DWORD dwMaxSize,            //��Ҫ������size
    DWORD& dwAllocateActualSize,//ʵ�ʷ������SIZE
    DWORD& dwAllocateMaxSize)   //���Է���������SIZE
{

    ASSERT(!m_pMemoryMng);   
    
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    IMemoryMngSet* pMemoryMngSet = pSdeControl->GetMemoryMngSet();
    ASSERT(pMemoryMngSet); 
    CMemoryDispatchForSpaceDataV2* pSpaceDispatch = pSdeControl->GetMemoryDispatchForSpace();
    ASSERT(pSpaceDispatch);
    //////////////////////////////////////////////////////////////////////////
    unsigned long nSpaceDeltaSize = 0;
    if(!pSpaceDispatch->CouldTighten(nSpaceDeltaSize))
    {
        TRACE0("MemoryForSpaceCan't be tighten.!\n");
    }
    //////////////////////////////////////////////////////////////////////////
    {
        //ȡ�ڴ�״̬���ж�     
        MEMORYSTATUS mst;
        ::GlobalMemoryStatus( &mst );	
        //TRACE1("\tcurrent load      :%.2d%.\n",mst.dwMemoryLoad);
        //TRACE1("\tavaliable physics :%dm.\n",mst.dwAvailPhys>>20);
        //TRACE1("\tavaliable virtual :%dm.\n",mst.dwAvailVirtual>>20);
        
#ifdef _WIN32_WCE        
        DWORD dwAvaliablePhys = mst.dwAvailPhys;
        if( (dwAvaliablePhys + nSpaceDeltaSize - (MEMORY_INSTANCY_LEFT_MB_SIZE<<20)) < (4<<20) )
            return FALSE;
        
        {
            unsigned long nDeltaSize;
            unsigned long nSpaceLastSize;
             
            {
                CSpaceMemoryLock aLock;
                if(CSDEControler::GetSDEControl()->GetMemoryDispatchForSpace()->RequireTighten(nDeltaSize,nSpaceLastSize))
                {
                    m_bResetSpaceData = TRUE;                    
                }
            }
            //////////////////////////////////////////////////////////////////////////
            unsigned long nRootSize = dwAvaliablePhys+nDeltaSize - (MEMORY_INSTANCY_LEFT_MB_SIZE<<20);
            if(nRootSize > dwMaxSize)
            {
                nRootSize = dwMaxSize;
            }
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
            unsigned long nRootSize = 12 * IMemoryMng::MB;
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
/**
 * @brief ������������䵫�ǲ���������������
 *
 * @param 
 * @return
*/
BOOL CMemoryDispatchForNetworkV2::GetMemoryMngMaximumV3ByMaxLimitAndLeftLimt(
    DWORD dwMaxSize,            //��Ҫ������size
    DWORD dwLeftLimitSize,      //��Ҫ������size
    DWORD& dwAllocateActualSize,//ʵ�ʷ������SIZE
    DWORD& dwAllocateMaxSize)   //���Է���������SIZE
{

    ASSERT(!m_pMemoryMng);   
    
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    IMemoryMngSet* pMemoryMngSet = pSdeControl->GetMemoryMngSet();
    ASSERT(pMemoryMngSet); 
    CMemoryDispatchForSpaceDataV2* pSpaceDispatch = pSdeControl->GetMemoryDispatchForSpace();
    ASSERT(pSpaceDispatch);
    //////////////////////////////////////////////////////////////////////////
    unsigned long nSpaceDeltaSize = 0;
    if(!pSpaceDispatch->CouldTighten(nSpaceDeltaSize))
    {
        TRACE0("MemoryForSpaceCan't be tighten.!\n");
    }
    //////////////////////////////////////////////////////////////////////////
    {
        //ȡ�ڴ�״̬���ж�     
        MEMORYSTATUS mst;
        ::GlobalMemoryStatus( &mst );	
        //TRACE1("\tcurrent load      :%.2d%.\n",mst.dwMemoryLoad);
        //TRACE1("\tavaliable physics :%dm.\n",mst.dwAvailPhys>>20);
        //TRACE1("\tavaliable virtual :%dm.\n",mst.dwAvailVirtual>>20);
        
#ifdef _WIN32_WCE        
        DWORD dwAvaliablePhys = mst.dwAvailPhys;
        if( (dwAvaliablePhys + nSpaceDeltaSize - dwLeftLimitSize) < (4<<20) )
            return FALSE;
        
        {
            unsigned long nDeltaSize;
            unsigned long nSpaceLastSize;
             
            {
                CSpaceMemoryLock aLock;
                if(CSDEControler::GetSDEControl()->GetMemoryDispatchForSpace()->RequireTighten(nDeltaSize,nSpaceLastSize))
                {
                    m_bResetSpaceData = TRUE;                    
                }
            }
            //////////////////////////////////////////////////////////////////////////
            unsigned long nRootSize = dwAvaliablePhys+nDeltaSize - dwLeftLimitSize;
            if(nRootSize > dwMaxSize)
            {
                nRootSize = dwMaxSize;
            }
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
            unsigned long nRootSize = 12 * IMemoryMng::MB;
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
/**
 * @brief ������Ҫ����ڴ��������
 *
 * �統ǰ�����ڴ�dwAvailPhys ������Ҫ������ֵ.��ô�������ֵ
 * �統ǰ�����ڴ� ����С������������֮��,���䵱ǰ����ֵ
 * �統ǰ�����ڴ� С����С����,�޷�����.
*/
BOOL CMemoryDispatchForNetworkV2::GetMemoryMngV2(
                                                 DWORD dwMinSize,
                                                 DWORD dwMaxSize,
                                                 DWORD& dwAllocateActualSize,
                                                 DWORD& dwAllocateMaxSize)
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
        //ȡ�ڴ�״̬���ж�     
        MEMORYSTATUS mst;
        ::GlobalMemoryStatus( &mst );	
        //TRACE1("\tcurrent load      :%.2d%.\n",mst.dwMemoryLoad);
        //TRACE1("\tavaliable physics :%dm.\n",mst.dwAvailPhys>>20);
        //TRACE1("\tavaliable virtual :%dm.\n",mst.dwAvailVirtual>>20);
        
#ifdef _WIN32_WCE        
        DWORD dwAvaliableAllocSize = mst.dwAvailPhys>>20;
        if(dwAvaliableAllocSize < MEMORY_NORMAL_LEFT_MB_SIZE)
        {
            dwAvaliableAllocSize = 0;
        }
        if( dwAvaliableAllocSize < (dwMinSize>>20) )
        {
            TRACE0("��ǰ�����ڴ�<����ڴ�����,���Է������ֵ���м���.\n");
            return GetMemoryMngMaximumV2(dwAllocateActualSize,dwAllocateMaxSize);
        }
        else if(dwAvaliableAllocSize < (dwMaxSize>>20) )
        {
            TRACE0("����ڴ�����>��ǰ�����ڴ�>����ڴ�����,���Է��䵱ǰ�����ڴ�ֵ���м���.\n");
            unsigned long nRootSize = dwAvaliableAllocSize<<20;
            if(!MallocExtendMemory(nRootSize,m_hFileMap,m_pMemAddr))
            {
                TRACE0("Reverse extend mem for network failed!\n");
                return FALSE;	
            }

            dwAllocateActualSize = nRootSize;
            dwAllocateMaxSize = (dwAvaliableAllocSize+5)*IMemoryMng::MB;
            
            IMemoryMng* pMemoryMng = new CMemoryMngV3A;
            ASSERT(pMemoryMng); 
            pMemoryMng->InitialHeap(m_pMemAddr,nRootSize);
            
            m_pMemoryMng = pMemoryMng;
            m_dwMemoryMngSize = nRootSize;

            return TRUE;
        }
        else
        {
            TRACE0("��ǰ�����ڴ� > ����ڴ�����,���Է�������ڴ�ֵ���м���.\n");
            unsigned long nRootSize = dwMaxSize;
            if(!MallocExtendMemory(nRootSize,m_hFileMap,m_pMemAddr))
            {
                TRACE0("Reverse extend mem for network failed!\n");
                return FALSE;	
            }

            dwAllocateActualSize = nRootSize;
            dwAllocateMaxSize = (dwAvaliableAllocSize+5)*IMemoryMng::MB;
            
            IMemoryMng* pMemoryMng = new CMemoryMngV3A;
            ASSERT(pMemoryMng); 
            pMemoryMng->InitialHeap(m_pMemAddr,nRootSize);
            
            m_pMemoryMng = pMemoryMng;
            m_dwMemoryMngSize = nRootSize;

            return TRUE;
        }
               
#else //#ifdef _WIN32_WCE   
        {
            unsigned long nRootSize = dwMinSize;
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
            dwAllocateMaxSize = dwMaxSize;
            return TRUE; 
        }     
#endif //#ifdef _WIN32_WCE
    }
             
    return FALSE;
}
/**
 * @brief ������Ҫ����ڴ��������
 *
 * �統ǰ�����ڴ�dwAvailPhys ������Ҫ������ֵ.��ô�������ֵ
 * �統ǰ�����ڴ� ����С������������֮��,���䵱ǰ����ֵ
 * �統ǰ�����ڴ� С����С����,�޷�����.
*/
BOOL CMemoryDispatchForNetworkV2::GetMemoryMngV3(
                                                 DWORD dwMinSize,
                                                 DWORD dwMaxSize,
                                                 DWORD& dwAllocateActualSize,
                                                 DWORD& dwAllocateMaxSize)
{

    ASSERT(!m_pMemoryMng);   
    
    CSDEControler* pSdeControl = CSDEControler::GetSDEControl() ;
    ASSERT(pSdeControl);
    IMemoryMngSet* pMemoryMngSet = pSdeControl->GetMemoryMngSet();
    ASSERT(pMemoryMngSet); 
    CMemoryDispatchForSpaceDataV2* pSpaceDispatch = pSdeControl->GetMemoryDispatchForSpace();
    ASSERT(pSpaceDispatch);
    //////////////////////////////////////////////////////////////////////////
    unsigned long nSpaceDeltaSize = 0;
    if(!pSpaceDispatch->CouldTighten(nSpaceDeltaSize))
    {
        TRACE0("MemoryForSpaceCan't be tighten.!\n");
    }
    
    {
        //ȡ�ڴ�״̬���ж�     
        MEMORYSTATUS mst;
        ::GlobalMemoryStatus( &mst );	
        //TRACE1("\tcurrent load      :%.2d%.\n",mst.dwMemoryLoad);
        //TRACE1("\tavaliable physics :%dm.\n",mst.dwAvailPhys>>20);
        //TRACE1("\tavaliable virtual :%dm.\n",mst.dwAvailVirtual>>20);
        
#ifdef _WIN32_WCE        
        DWORD dwAvaliableAllocSize = mst.dwAvailPhys;
        if(dwAvaliableAllocSize < (MEMORY_NORMAL_LEFT_MB_SIZE<<20))
        {
            dwAvaliableAllocSize = 0;
        }
        else
        {
            dwAvaliableAllocSize -= (MEMORY_NORMAL_LEFT_MB_SIZE<<20);
        }
        
        //////////////////////////////////////////////////////////////////////////
        //070508��Ӵ˴���,��������֮���ڴ�ͳ��ּ�Ϊ��������
        static int s_nCount = 0;
        s_nCount++;
        //////////////////////////////////////////////////////////////////////////


        if( dwAvaliableAllocSize < dwMinSize )
        {
            TRACE0("��ǰ�����ڴ�<����ڴ�����,���Է������ֵ���м���.\n");
            //return GetMemoryMngMaximumV3ByLimit(dwMaxSize, dwAllocateActualSize,dwAllocateMaxSize);
            //////////////////////////////////////////////////////////////////////////
            //070508��Ӵ˴���,��������֮���ڴ�ͳ��ּ�Ϊ��������
            {
                if(s_nCount < 4 )
                {
                    return GetMemoryMngMaximumV3ByMaxLimitAndLeftLimt(dwMaxSize,(MEMORY_NORMAL_LEFT_MB_SIZE<<20),dwAllocateActualSize,dwAllocateMaxSize);
                }
                else
                {
                    return GetMemoryMngMaximumV3ByLimit(dwMaxSize, dwAllocateActualSize,dwAllocateMaxSize);
                }
            }
            //////////////////////////////////////////////////////////////////////////
        }
        else if(dwAvaliableAllocSize < dwMaxSize )
        {
            TRACE0("����ڴ�����>��ǰ�����ڴ�>����ڴ�����,���Է��䵱ǰ�����ڴ�ֵ���м���.\n");
            unsigned long nRootSize = dwAvaliableAllocSize;
            if(!MallocExtendMemory(nRootSize,m_hFileMap,m_pMemAddr))
            {
                TRACE0("Reverse extend mem for network failed!\n");
                return FALSE;	
            }

            dwAllocateActualSize = nRootSize;
            dwAllocateMaxSize = dwAvaliableAllocSize + nSpaceDeltaSize;
            
            IMemoryMng* pMemoryMng = new CMemoryMngV3A;
            ASSERT(pMemoryMng); 
            pMemoryMng->InitialHeap(m_pMemAddr,nRootSize);
            
            m_pMemoryMng = pMemoryMng;
            m_dwMemoryMngSize = nRootSize;

            return TRUE;
        }
        else
        {
            TRACE0("��ǰ�����ڴ� > ����ڴ�����,���Է�������ڴ�ֵ���м���.\n");
            unsigned long nRootSize = dwMaxSize;
            if(!MallocExtendMemory(nRootSize,m_hFileMap,m_pMemAddr))
            {
                TRACE0("Reverse extend mem for network failed!\n");
                return FALSE;	
            }

            dwAllocateActualSize = nRootSize;
            dwAllocateMaxSize = dwAvaliableAllocSize + nSpaceDeltaSize;
            
            IMemoryMng* pMemoryMng = new CMemoryMngV3A;
            ASSERT(pMemoryMng); 
            pMemoryMng->InitialHeap(m_pMemAddr,nRootSize);
            
            m_pMemoryMng = pMemoryMng;
            m_dwMemoryMngSize = nRootSize;

            return TRUE;
        }
               
#else //#ifdef _WIN32_WCE   
        {
            unsigned long nRootSize = dwMinSize;
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
            dwAllocateMaxSize = dwMaxSize;
            return TRUE; 
        }     
#endif //#ifdef _WIN32_WCE
    }
             
    return FALSE;
}
BOOL CMemoryDispatchForNetworkV2::ReleaseMemoryMngV2()
{
    ASSERT(m_pMemoryMng);
    delete m_pMemoryMng;
    m_pMemoryMng = NULL;
    FreeExtendMemory(m_hFileMap,m_pMemAddr);
    m_hFileMap = NULL;
    m_pMemAddr = NULL;

    
    
    if(m_bResetSpaceData)
    {
        CSpaceMemoryLock aLock;
        CSDEControler::GetSDEControl()->GetMemoryDispatchForSpace()->RequireReGet(8*IMemoryMng::MB);
        m_bResetSpaceData = FALSE;  
    }
   
    return TRUE;
}
BOOL CMemoryDispatchForNetworkV2::ReleaseMemoryMngV3()
{
    ASSERT(m_pMemoryMng);
    delete m_pMemoryMng;
    m_pMemoryMng = NULL;
    FreeExtendMemory(m_hFileMap,m_pMemAddr);
    m_hFileMap = NULL;
    m_pMemAddr = NULL;

    
    
    if(m_bResetSpaceData)
    {
        unsigned long nDeltaSize;
        unsigned long nSpaceLastSize;
        CSpaceMemoryLock aLock;
        CSDEControler::GetSDEControl()->GetMemoryDispatchForSpace()->RequireRelax(nDeltaSize,nSpaceLastSize);
        m_bResetSpaceData = FALSE;  
    }
   
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//�ڴ��ʼ��
BOOL CMemoryDispatchForNetworkV2::MemoryInitial()
{
    ASSERT(m_pMemoryMng);
    
#ifdef MEMORY_ANALYSE_IN_NETWORK
    return MemoryInitialSeparate();
#else
    return MemoryInitialMixed();
#endif
}
BOOL CMemoryDispatchForNetworkV2::MemoryClear()
{
#ifdef MEMORY_ANALYSE_IN_NETWORK
    return  MemoryClearSeparate();
#else
    return MemoryClearMixed();
#endif
}

BOOL CMemoryDispatchForNetworkV2::MemoryInitialMixed()
{
    ASSERT(m_pMemoryMng);
    IMemoryMng* pMemoryMng = m_pMemoryMng;
    pMemoryMng->ResetHeap();

	
	CGIS_NetworkVtx::m_pMemMngOfSelf        = pMemoryMng;
    CGIS_NetworkRut::m_pMemMngOfSelf        = pMemoryMng;
    CGIS_NetworkNode::m_pMemMngOfSelf       = pMemoryMng;

    CGIS_NetworkVtx::m_pMemMngOfSortInfo    = pMemoryMng;
    CGIS_NetworkVtx::m_pMemMngOfEdgeInfo    = pMemoryMng;

    
    CGIS_NetworkVtx::m_pMemMngOfBuddyData   = pMemoryMng;  
    IGIS_NetWork::m_pMemMngOfBuddyData      = pMemoryMng;
    IGIS_MapNetwork::m_pMemMngOfBuddyData   = pMemoryMng;
    CGIS_NetworkRut::m_pMemMngOfBuddyData   = pMemoryMng;

    CGIS_MBBNetwork::m_pMemMngOfNetworkData = pMemoryMng;
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
    pMemoryMng ->SetTextInfo(_T("Network"));
#endif
    
	return TRUE;
}
BOOL CMemoryDispatchForNetworkV2::MemoryClearMixed()
{
    ASSERT(m_pMemoryMng);
    //MEMMNG_DUMPSTATE(m_pMemoryMng);
    return TRUE;
}

BOOL CMemoryDispatchForNetworkV2::MemoryInitialSeparate()
{   
    ASSERT(m_pMemoryMng);
    int nDestMngHeapSize[9] = 
    {
            8000,
            8000,
            8000,
            8000,
            8000,
            8000,
            4000,
    };
    IMemoryMng* pMemoryMng = m_pMemoryMng;
    pMemoryMng->ResetHeap();

   
	//�ȰѶ���������
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
	auto_ptr <IMemoryMng> pSubMemoryMng6(new CMemoryMngV3A);
	if(!pSubMemoryMng6.get())return FALSE;
	
	
	//����������
	DWORD dwMemSize = 0;
	DWORD dwCellSize = 0;
	void* pHeap = NULL;
	
	dwMemSize = nDestMngHeapSize[0]*IMemoryMng::KB;
	dwCellSize = sizeof(CGIS_NetworkVtx);
	VERIFY(pSubMemoryMng0->InitialHeap(dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[1]*IMemoryMng::KB;
	dwCellSize = sizeof(CGIS_NetworkRut);
	VERIFY(pSubMemoryMng1->InitialHeap(dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[2]*IMemoryMng::KB;
	dwCellSize = sizeof(CGIS_NetworkNode);
	VERIFY(pSubMemoryMng2->InitialHeap(dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[3]*IMemoryMng::KB;
	dwCellSize = sizeof(ST_GEO_VSORT);
	VERIFY(pSubMemoryMng3->InitialHeap(dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[4]*IMemoryMng::KB;
	dwCellSize = sizeof(ST_GEO_VEDGE);
	VERIFY(pSubMemoryMng4->InitialHeap(dwMemSize,dwCellSize));
	
	dwMemSize = nDestMngHeapSize[5]*IMemoryMng::KB;
	dwCellSize = 0;
	VERIFY(pSubMemoryMng5->InitialHeap(dwMemSize));

	dwMemSize = nDestMngHeapSize[6]*IMemoryMng::KB;
	dwCellSize = 0;
	VERIFY(pSubMemoryMng6->InitialHeap(dwMemSize));


	
	CGIS_NetworkVtx::m_pMemMngOfSelf        = pSubMemoryMng0.release();
    CGIS_NetworkRut::m_pMemMngOfSelf        = pSubMemoryMng1.release();
    CGIS_NetworkNode::m_pMemMngOfSelf       = pSubMemoryMng2.release();

    CGIS_NetworkVtx::m_pMemMngOfSortInfo    = pSubMemoryMng3.release();
    CGIS_NetworkVtx::m_pMemMngOfEdgeInfo    = pSubMemoryMng4.release();

    
    IGIS_NetWork::m_pMemMngOfBuddyData      = pSubMemoryMng5.release();
    
    IGIS_MapNetwork::m_pMemMngOfBuddyData   = IGIS_NetWork::m_pMemMngOfBuddyData;
    CGIS_NetworkRut::m_pMemMngOfBuddyData   = IGIS_NetWork::m_pMemMngOfBuddyData;	
    CGIS_NetworkVtx::m_pMemMngOfBuddyData   = IGIS_NetWork::m_pMemMngOfBuddyData;  
    
    CGIS_MBBNetwork::m_pMemMngOfNetworkData = pSubMemoryMng6.release();;
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	CGIS_NetworkVtx::m_pMemMngOfSelf        ->SetTextInfo(_T("Network:Vtx Self"));
    CGIS_NetworkRut::m_pMemMngOfSelf        ->SetTextInfo(_T("Network:Rut Self"));
    CGIS_NetworkNode::m_pMemMngOfSelf       ->SetTextInfo(_T("Network:Node Self"));

    CGIS_NetworkVtx::m_pMemMngOfSortInfo    ->SetTextInfo(_T("Network:Vtx SortInfo"));
    CGIS_NetworkVtx::m_pMemMngOfEdgeInfo    ->SetTextInfo(_T("Network:Vtx EdgeInfo"));

    
    IGIS_NetWork::m_pMemMngOfBuddyData      ->SetTextInfo(_T("Network:IGIS_NetWork::Buddy"));
    //IGIS_MapNetwork::m_pMemMngOfBuddyData   ->SetTextInfo(_T("Network:IGIS_MapNetwork Buddy"));
    //CGIS_NetworkRut::m_pMemMngOfBuddyData   ->SetTextInfo(_T("Network:Rut Buddy"));  
    //CGIS_NetworkVtx::m_pMemMngOfBuddyData   ->SetTextInfo(_T("Network:Vtx Buddy"));  
    CGIS_MBBNetwork::m_pMemMngOfNetworkData ->SetTextInfo(_T("TrunkNetwork"));
#endif
	return TRUE;
}
BOOL CMemoryDispatchForNetworkV2::MemoryClearSeparate()
{

	delete CGIS_NetworkVtx::m_pMemMngOfSelf;
	CGIS_NetworkVtx::m_pMemMngOfSelf = NULL;
	delete CGIS_NetworkRut::m_pMemMngOfSelf;
	CGIS_NetworkRut::m_pMemMngOfSelf = NULL;
	delete CGIS_NetworkNode::m_pMemMngOfSelf;
    CGIS_NetworkNode::m_pMemMngOfSelf = NULL; 
	delete CGIS_NetworkVtx::m_pMemMngOfSortInfo;
	CGIS_NetworkVtx::m_pMemMngOfSortInfo = NULL;
	delete CGIS_NetworkVtx::m_pMemMngOfEdgeInfo;
	CGIS_NetworkVtx::m_pMemMngOfEdgeInfo = NULL;


	delete IGIS_NetWork::m_pMemMngOfBuddyData;
    IGIS_NetWork::m_pMemMngOfBuddyData = NULL; 
//  delete IGIS_MapNetwork::m_pMemMngOfBuddyData;
	IGIS_MapNetwork::m_pMemMngOfBuddyData = NULL;
//	delete CGIS_NetworkRut::m_pMemMngOfBuddyData;
	CGIS_NetworkRut::m_pMemMngOfBuddyData = NULL;
//	delete CGIS_NetworkVtx::m_pMemMngOfBuddyData;
	CGIS_NetworkVtx::m_pMemMngOfBuddyData = NULL;

    MEMMNG_DUMPSTATE(CGIS_MBBNetwork::m_pMemMngOfNetworkData);
    delete CGIS_MBBNetwork::m_pMemMngOfNetworkData;
    CGIS_MBBNetwork::m_pMemMngOfNetworkData = NULL;


	return TRUE;
}

