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
	ASSERT(m_stData.m_byDirection < 4 );		//·�ε�˫�б�־(0��ʾ˫�У�1��ʾend��start,2��ʾstart��end,3��ʾ˫�����)
	ASSERT(m_stData.m_byCharge < 4);			//�շ�:0--���շ�1--˫���շ�2--�����շ�3--�����շ�
	

	ASSERT(m_stData.m_byRutLevel < 6);		//��·���ܼ���:
//									5--һ����·
//									4--������·
//									3--������·
//									2--�ļ���·
//									1--�弶��·
// 									0--������·

	
	//0x0c
	//ASSERT(m_dRLength >=0 && m_dRLength < 100000);			//·�γ���
	//0x10
	//ASSERT(m_nStartVertexID < 20000);//·����ʼ�ڵ�ID
	//ASSERT(m_nEndVertexID < 20000);	//·��β�ڵ�ID
	//0x14
	ASSERT(m_stData.m_byNormalNum < 50);	//�Ƿ���ڽ�ͨ����		:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
	ASSERT(	m_stData.m_byReverseNum < 50);	//�Ƿ�����ڽ�ͨ����	:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
	ASSERT(	m_stData.m_byRutType < 20);	//��·����:0--����1--δ�ɼ�����2--�����߷���3--��������·֮��ĵ�·
	//0x18
	ASSERT	(m_stData.m_nRBID <100000);			//·������ͼ��ID
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
        TRACE0("CGIS_NetworkRoadSectionV5 �ı����ڴ��������δ��ʼ��.\n");
        return NULL;
    }

    return m_pMemMngOfSelf->newmalloc();
}

void CGIS_NetworkRoadSectionV5::operator delete (void* p,size_t size)
{
	if(!m_pMemMngOfSelf)
    {
        TRACE0("CGIS_NetworkRoadSectionV5 �ı����ڴ��������δ��ʼ��.\n");
        return;
    }
    m_pMemMngOfSelf->newfree(p);
}
#endif
//end
//////////////////////////////////////////////////////////////////////////