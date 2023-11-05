// GIS_NetworkNode.cpp: implementation of the CGIS_NetworkNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIS_NetworkNode.h"
#include "./MemoryMngLib/IMemoryMng.h"
#include "./MemoryMngLib/IMemoryMng2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkNode::CGIS_NetworkNode()
{
	m_pRut = NULL;
	m_pVtx = NULL;
	m_g = 0.0;
	m_f = 0.0;
	m_pParent = NULL;
	m_nLRNum = 0;
    m_bHaveJCT = FALSE;
}

CGIS_NetworkNode::~CGIS_NetworkNode()
{
	m_pRut = NULL;
	m_pVtx = NULL;
	m_g = 0.0;
	m_f = 0.0;
	m_pParent = NULL;
	m_nLRNum = 0;
    m_bHaveJCT = FALSE;
}

//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE
IMemoryMng* CGIS_NetworkNode::m_pMemMngOfSelf = NULL;
//--------------------------------------
void* CGIS_NetworkNode::operator new(size_t size)
{	
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkNode 的本体内存管理器尚未初始化.\n");
		THROW(new CUserException(TRUE,EN_ET_MEMOVER));
        return NULL;
    }

	void* pRtn = m_pMemMngOfSelf->newmalloc(size);
	if(!pRtn)
	{
        THROW(new CUserException(TRUE,EN_ET_MEMOVER));
	}
    return pRtn;
}

void CGIS_NetworkNode::operator delete (void* p,size_t size)
{
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkNode 的本体内存管理器尚未初始化.\n");
        return;
    }
    m_pMemMngOfSelf->newfree(p);
}
#endif
//end
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_NetworkNode::DumpState() const	
{  
    TRACE3("\tRut%.4d->Vtx%.4d[%d]",this->m_pRut->GetRutID(),this->m_pVtx->GetVtxID(),m_nLRNum);
    switch(m_pRut->GetRutDirection())
    {
    case 0:
        TRACE2("(%.4d<->%.4d)\n",m_pRut->GetRutStartVtxID() ,m_pRut->GetRutEndVtxID());
        break;
    case 1:
        TRACE2("(%.4d ->%.4d)\n",m_pRut->GetRutStartVtxID() ,m_pRut->GetRutEndVtxID());
        break;
    case 2:
        TRACE2("(%.4d ->%.4d)\n",m_pRut->GetRutEndVtxID() ,m_pRut->GetRutStartVtxID());
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    TRACE2("\t%f\n\t%f\n",m_pVtx->GetVtxPosition().dx,m_pVtx->GetVtxPosition().dy);
    TRACE2("\tg:%.6f,f:%.6f\n",m_g,m_f);

//    TRACE0("********************************************************************\n");
//    TRACE2("RutID[%d],BreadthID[%d]\n",this->m_pRut->GetRutID(),this->m_pRut->GetRBreadthID());
//    TRACE2("VtxID[%d],BreadthID[%d]\n",this->m_pVtx->GetVtxID(),this->m_pVtx->GetVBreadthID());
//    TRACE2("g[%f] f[%f]\n",this->m_g,this->m_f);
//    TRACE1("m_nLRNum[%d]\n",this->m_nLRNum);
//    TRACE0("********************************************************************\n");
    Sleep(100);
}

void CGIS_NetworkNode::AssertValid() const	
{   


}
#endif
//////////////////////////////////////////////////////////////////////////