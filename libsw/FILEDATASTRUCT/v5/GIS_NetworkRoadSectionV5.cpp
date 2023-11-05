// Gis_NetworkRoadSectionV5.cpp: implementation of the CGIS_NetworkRoadSectionV5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Gis_NetworkRoadSectionV5.h"
#include "./MemoryMngLib/IMemoryMng.h"
#include "./MemoryMngLib/IMemoryMng2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkRoadSectionV5::CGIS_NetworkRoadSectionV5()
{
    m_pNormalList   =   NULL;	
	m_pReverseList  =   NULL;	

}

CGIS_NetworkRoadSectionV5::~CGIS_NetworkRoadSectionV5()
{
#ifdef MEMORY_SELF_MANAGE
    if(m_byDirection&0x80)
    {
        if(m_pNormalList)
        {
            m_pMemMngOfBuddyData->newfree(m_pNormalList);
            m_pNormalList = NULL;
        }
        if(m_pReverseList)
        {
            m_pMemMngOfBuddyData->newfree(m_pReverseList);
            m_pReverseList = NULL;
        }
    }
#endif
}

inline void CGIS_NetworkRoadSectionV5::MakeBuddyIndependent()
{
#ifdef MEMORY_SELF_MANAGE
    if(m_byNormalNum)
	{
        ASSERT(m_pNormalList);
        unsigned long* pDwordData = (unsigned long*)m_pMemMngOfBuddyData->newmalloc(m_byNormalNum<<2);
		memcpy(pDwordData,m_pNormalList,m_byNormalNum<<2);
		m_pNormalList = pDwordData;
	}

	if(m_byReverseNum)
	{
        ASSERT(m_pReverseList);
        unsigned long* pDwordData = (unsigned long*)m_pMemMngOfBuddyData->newmalloc(m_byReverseNum<<2);
		memcpy(pDwordData,m_pReverseList,m_byReverseNum<<2);
		m_pReverseList = pDwordData;
	}

    m_byDirection |= 0x80;
#endif
}
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_NetworkRoadSectionV5::AssertValid() const
{
	//0x04
	ASSERT(m_stData.m_byDirection < 4 );		//路段单双行标志(0表示双行，1表示end到start,2表示start到end,3表示双向禁行)
	ASSERT(m_stData.m_byCharge < 4);			//收费:0--不收费1--双向收费2--正向收费3--反向收费
	

	ASSERT(m_stData.m_byRutLevel < 6);		//道路功能级别:
//									5--一级道路
//									4--二级道路
//									3--三级道路
//									2--四级道路
//									1--五级道路
// 									0--六级道路

	
	//0x0c
	//ASSERT(m_dRLength >=0 && m_dRLength < 100000);			//路段长度
	//0x10
	//ASSERT(m_nStartVertexID < 20000);//路段起始节点ID
	//ASSERT(m_nEndVertexID < 20000);	//路段尾节点ID
	//0x14
	ASSERT(m_stData.m_byNormalNum < 50);	//是否存在交通限制		:0-没有交通限制;非0- 有交通限制(且表示路段个数)
	ASSERT(	m_stData.m_byReverseNum < 50);	//是否反向存在交通限制	:0-没有交通限制;非0- 有交通限制(且表示路段个数)
	ASSERT(	m_stData.m_byRutType < 20);	//道路类型:0--环岛1--未采集属性2--上下线分离3--两条高速路之间的道路
	//0x18
	ASSERT	(m_stData.m_nRBID <100000);			//路段所在图幅ID
}
void CGIS_NetworkRoadSectionV5::Dump(CDumpContext& dc) const
{

}
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE
IMemoryMng2* CGIS_NetworkRoadSectionV5::m_pMemMngOfSelf = NULL;
IMemoryMng* CGIS_NetworkRoadSectionV5::m_pMemMngOfBuddyData = NULL;
//--------------------------------------
void* CGIS_NetworkRoadSectionV5::operator new(size_t size)
{	
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkRoadSectionV5 的本体内存管理器尚未初始化.\n");
        return NULL;
    }

    return m_pMemMngOfSelf->newmalloc();
}

void CGIS_NetworkRoadSectionV5::operator delete (void* p,size_t size)
{
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkRoadSectionV5 的本体内存管理器尚未初始化.\n");
        return;
    }
    m_pMemMngOfSelf->newfree(p);
}
#endif
//end
//////////////////////////////////////////////////////////////////////////