
#include "stdafx.h"

#include "SDEControler.h"
#include "IMemoryMngSet.h"
#include "./MemoryMngLib/MemoryMng3V1.h"
#include "./MemoryMngLib/MemoryMngV3A.h"

#include "GIS_PathpointMark.h"
#include "GIS_PathpointNode.h"

#include <memory>
using namespace std;

#include "GIS_PathPoint.h"

CGIS_Pathpoint::CGIS_Pathpoint()
{
	m_enPathpointType = EN_PATHPOINTTYPE_INFLEXION;
	m_dDisNext = 0.0;
#ifdef _DEBUG
	m_nObjectCount++;
#endif
}

CGIS_Pathpoint::~CGIS_Pathpoint()
{
#ifdef _DEBUG
	m_nObjectCount--;
#endif
    
#ifdef MEMORY_SELF_MANAGE	
	if(!m_nObjectCount)
	{
        TRACE0("PathPoint对象全部释放.\n");
        //MEMMNG_DUMPSTATE(m_pMemMngOfSelf);
        //MEMMNG_DUMPSTATE(m_pMemMngOfPathpointNode);
        //MEMMNG_DUMPSTATE(m_pMemMngOfBuddyData);
		m_pMemMngOfBuddyData->ResetHeap();
	}
#endif
}
void CGIS_Pathpoint::SetPathpointType( EnPathpointType enPathpointType )
{
	m_enPathpointType = enPathpointType;
}
EnPathpointType CGIS_Pathpoint::GetPathpointType( )
{
	return m_enPathpointType;
}
void CGIS_Pathpoint::SetPathpointPosition( ST_GEO_PTXY ptPos )
{
	m_ptPos.dx = ptPos.dx;
	m_ptPos.dy = ptPos.dy;
}
ST_GEO_PTXY CGIS_Pathpoint::GetPathpointPosition( )
{
	return m_ptPos;
}
void CGIS_Pathpoint::CalcDis2Next( ST_GEO_PTXY ptPos )
{
    //ASSERT(fabs(m_ptPos.dx - ptPos.dx) > FLOAT_DETA_XY);
    //ASSERT(fabs(m_ptPos.dy - ptPos.dy) > FLOAT_DETA_XY);
	m_dDisNext = CalcDist( m_ptPos.dx, m_ptPos.dy, ptPos.dx, ptPos.dy );
}
float CGIS_Pathpoint::GetDis2Next( )
{
	return m_dDisNext;
}

/////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
unsigned long CGIS_Pathpoint::m_nObjectCount = 0;
int CGIS_Pathpoint::GetNowObjectCount()
{
	return m_nObjectCount;
}
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE
IMemoryMng* CGIS_Pathpoint::m_pMemMngOfSelf = NULL;
IMemoryMng* CGIS_Pathpoint::m_pMemMngOfPathpointNode = NULL;
IMemoryMng*  CGIS_Pathpoint::m_pMemMngOfBuddyData = NULL;
//--------------------------------------
void* CGIS_Pathpoint::operator new(size_t size)
{	
	if(!m_pMemMngOfSelf)
    {
        //TRACE0("CGIS_Pathpoint 的本体内存管理器尚未初始化.\n");
        
		int nDestMngHeapSize[3] = {1700,200,100};;
	
		auto_ptr <IMemoryMng3> pMemMng(new CMemoryMng3V1);
		ASSERT(pMemMng.get());
		if(!pMemMng->InitialHeap(nDestMngHeapSize[0]*IMemoryMng::KB,sizeof(CGIS_Pathpoint)))
			return NULL;

		auto_ptr <IMemoryMng3> pMemMngOfPathpointNode(new CMemoryMng3V1);
		ASSERT(pMemMngOfPathpointNode.get());
		if(!pMemMngOfPathpointNode->InitialHeap(nDestMngHeapSize[1]*IMemoryMng::KB,sizeof(CGIS_PathpointNode)))
			return NULL;

//		auto_ptr <IMemoryMng> pMemMngOfBuddyData(new CMemoryMngV3A);
//		ASSERT(pMemMngOfBuddyData.get());
//		if(!pMemMngOfBuddyData->InitialHeap(nDestMngHeapSize[2]*IMemoryMng::KB))
//			return NULL;
		auto_ptr <IMemoryMng> pMemMngOfBuddyData(new CMemoryMngV3A);
		ASSERT(pMemMngOfBuddyData.get());
		if(!pMemMngOfBuddyData->InitialHeap(nDestMngHeapSize[2]*IMemoryMng::KB))
			return NULL;


        TRACE1("为CGIS_Pathpoint 保留了%dk内存.\n",					nDestMngHeapSize[0]);
        TRACE1("为CGIS_PathpointNode 保留了%dk内存.\n",				nDestMngHeapSize[1]);
        TRACE1("为CGIS_PathpointNode的BuddyData保留了%dk内存.\n",	nDestMngHeapSize[2]);

		//在这里分配的这片资源永远不回收.直到程序退出.可以支持110000个pathpoint.
		m_pMemMngOfSelf = pMemMng.release();
		m_pMemMngOfPathpointNode = pMemMngOfPathpointNode.release();
		m_pMemMngOfBuddyData = pMemMngOfBuddyData.release();
    }

	void* pRtn = NULL;
	switch(size)
	{
	case sizeof(CGIS_Pathpoint):
		{
			
			pRtn = m_pMemMngOfSelf->newmalloc(size);
		}
		break;
	case sizeof(CGIS_PathpointMark):
		{
			pRtn = ::operator new(size);
		}
		break;
	case sizeof(CGIS_PathpointNode):
		{
			pRtn =  m_pMemMngOfPathpointNode->newmalloc(size);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return pRtn;
}

void CGIS_Pathpoint::operator delete (void* p,size_t size)
{
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_Pathpoint 的本体内存管理器尚未初始化.\n");
        return;
    }

	switch(size)
	{
	case sizeof(CGIS_Pathpoint):
		{
			m_pMemMngOfSelf->newfree(p);
		}
		break;
	case sizeof(CGIS_PathpointMark):
		{
			::operator delete(p);
		}
		break;
	case sizeof(CGIS_PathpointNode):
		{
			m_pMemMngOfPathpointNode->newfree(p);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	
}
#endif
//end
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_Pathpoint::DumpState() const	
{  
    TRACE2("\tPosition(%.5f,%.5f)",m_ptPos.dx,m_ptPos.dy);
    TRACE2("\tDisToNext(%.1fkm,%.1fm)\n",m_dDisNext/1000.0,m_dDisNext);
}

void CGIS_Pathpoint::AssertValid() const	
{   


}
#endif
//////////////////////////////////////////////////////////////////////////