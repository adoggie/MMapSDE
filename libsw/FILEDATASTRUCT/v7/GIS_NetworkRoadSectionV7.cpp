// Gis_NetworkRoadSectionV7.cpp: implementation of the CGIS_NetworkRoadSectionV7 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Gis_NetworkRoadSectionV7.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkRoadSectionV7::CGIS_NetworkRoadSectionV7()
{
    m_stData.m_byNormalNum = 0;
    m_stData.m_byReverseNum = 0;
    m_pNormalList   =   NULL;	
	m_pReverseList  =   NULL;	

}

CGIS_NetworkRoadSectionV7::~CGIS_NetworkRoadSectionV7()
{

}

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_NetworkRoadSectionV7::AssertValid() const
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

    if(m_stData.m_byNormalNum)
    {
        ASSERT(m_pNormalList);
    }
    if(m_stData.m_byReverseNum)
    {
        ASSERT(m_pReverseList);
    }


}
void CGIS_NetworkRoadSectionV7::Dump(CDumpContext& dc) const
{

}
#endif
//////////////////////////////////////////////////////////////////////////
CGIS_NetworkRoadSectionV7* CGIS_NetworkRoadSectionV7::CreateCloneObject()
{
    CGIS_NetworkRoadSectionV7* pFtr = (CGIS_NetworkRoadSectionV7*)malloc(sizeof(CGIS_NetworkRoadSectionV7));
    memset(pFtr,0,sizeof(CGIS_NetworkRoadSectionV7));	
    pFtr->m_stData = m_stData;
	return pFtr;    
}
void CGIS_NetworkRoadSectionV7::DestoryCloneObject()
{
    memset(this,0,sizeof(CGIS_NetworkRoadSectionV7));
    free(this);

}

//////////////////////////////////////////////////////////////////////////