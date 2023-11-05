/**
 * @file   GIS_NetworkRoadSectionV2.cpp
 * @brief 
 *
 * @purpose 
 * 
 *
 * @author  
 * @date    2006/10/25
 */

#include "stdafx.h"
#include "GIS_NetworkRoadSectionV2.h"


#ifdef _DEBUG
unsigned long CGIS_NetworkRoadSectionV2::m_nCount = 0;
unsigned long CGIS_NetworkRoadSectionV2::m_nMaxCount = 0;	
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkRoadSectionV2::CGIS_NetworkRoadSectionV2()
{
	memset(this,0,sizeof(CGIS_NetworkRoadSectionV2));
	
#ifdef _DEBUG
	m_nCount++;
	if(m_nCount >= m_nMaxCount)
	{
		m_nMaxCount = m_nCount;
	}
#endif
}

CGIS_NetworkRoadSectionV2::~CGIS_NetworkRoadSectionV2()
{
#ifdef _DEBUG
	m_nCount--;
#endif
}


//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE

CGIS_NetworkRoadSectionV2* CGIS_NetworkRoadSectionV2::m_pArrayOfObj = NULL;
char* CGIS_NetworkRoadSectionV2::m_pOccupiedSign = NULL;
int CGIS_NetworkRoadSectionV2::m_CurrentPosOfArray = 0;
int CGIS_NetworkRoadSectionV2::m_CurrentObjCount = 0;

int* CGIS_NetworkRoadSectionV2::m_pFreeObjectStack = NULL;
int CGIS_NetworkRoadSectionV2::m_TopOfFreeObjectStack = 0;
//--------------------------------------
void* CGIS_NetworkRoadSectionV2::operator new(size_t size)
{	
	if(!m_pArrayOfObj)
	{
		TRACE1("CGIS_NetworkRoadSectionV2 �����ռ�(%d������).\n",MAX_MEMORY_OBJ_COUNT);
		m_pArrayOfObj = (CGIS_NetworkRoadSectionV2*)malloc(sizeof(CGIS_NetworkRoadSectionV2)*MAX_MEMORY_OBJ_COUNT);
		m_pOccupiedSign = new char[MAX_MEMORY_OBJ_COUNT];
		m_pFreeObjectStack = new int[MAX_MEMORY_OBJ_COUNT];
		m_TopOfFreeObjectStack = -1;
		
		m_CurrentObjCount++;

		m_CurrentPosOfArray = 0;
		m_pOccupiedSign[m_CurrentPosOfArray] = 1;
		return &m_pArrayOfObj[m_CurrentPosOfArray++];
	}

	if(m_CurrentPosOfArray < MAX_MEMORY_OBJ_COUNT)
	{
		m_CurrentObjCount++;

		m_pOccupiedSign[m_CurrentPosOfArray] = 1;
		return &m_pArrayOfObj[m_CurrentPosOfArray++];
	}
	else
	{
		if(m_TopOfFreeObjectStack < 0)
		{
			TRACE0("CGIS_NetworkRoadSectionV2 �����ռ��С.\n");
			return NULL;
		}

		m_CurrentObjCount++;

		ASSERT(m_pOccupiedSign[m_pFreeObjectStack[m_TopOfFreeObjectStack]] == 0);
		m_pOccupiedSign[m_pFreeObjectStack[m_TopOfFreeObjectStack]] = 1;
		m_TopOfFreeObjectStack--;
		
		return &m_pArrayOfObj[m_pFreeObjectStack[m_TopOfFreeObjectStack+1]];

		//THROW(new CException(TRUE));
	}
	TRACE0("CGIS_NetworkRoadSectionV2 �����ռ��С.\n");
	return NULL;
}

void CGIS_NetworkRoadSectionV2::operator delete (void* p,size_t size)
{
	if(!m_pArrayOfObj)
	{
		return;
	}

	char* pArrayStart = (char*)m_pArrayOfObj;
	char* pDest = (char*)p;
	int nPosOfOccupiedSign = (pDest - pArrayStart) / sizeof(CGIS_NetworkRoadSectionV2);
	
	if(0 <= nPosOfOccupiedSign && nPosOfOccupiedSign < MAX_MEMORY_OBJ_COUNT)
	{
		m_CurrentObjCount--;
		m_pOccupiedSign[nPosOfOccupiedSign] = 0;

		m_TopOfFreeObjectStack++;
		m_pFreeObjectStack[m_TopOfFreeObjectStack] = nPosOfOccupiedSign;
	
		if(	m_CurrentObjCount == 0)
		{
			TRACE0("CGIS_NetworkRoadSectionV2 ɾ�������ռ�.\n");
			
			free(m_pArrayOfObj);
			m_pArrayOfObj = NULL;

			delete []m_pOccupiedSign;
			m_pOccupiedSign = NULL;
			m_CurrentPosOfArray = 0;

			delete [] m_pFreeObjectStack;
			m_pFreeObjectStack = NULL;
			m_TopOfFreeObjectStack = -1;

		}
	}
}
#endif
//end
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CGIS_NetworkRoadSectionV2::AssertValid() const
{
	ASSERT(m_nRutID < 20000);			//·��ID
	
	//0x04
	ASSERT(m_byDirection < 4 );		//·�ε�˫�б�־(0��ʾ˫�У�1��ʾend��start,2��ʾstart��end,3��ʾ˫�����)
	ASSERT(m_byType < 7 );			//��·���:(0--���٣�1--���и���·��2--������3--��Ҫ�ط���&�ֶ�
										//4--һ���ص���5--��������6--������·��
	ASSERT(m_byCharge < 4);			//�շ�:0--���շ�1--˫���շ�2--�����շ�3--�����շ�
	ASSERT(m_byLaneNum < 256);			//·�γ�����:�����ݵĸ��ݵ�·�����ȱʡֵ
	
	//0x08
	ASSERT(m_bySpeed < 256);			//�ٶȼ���:���ݵ�·�����ȱʡ�ٶ�
	ASSERT(m_byRutLevel < 6);		//��·���ܼ���:
//									5--һ����·
//									4--������·
//									3--������·
//									2--�ļ���·
//									1--�弶��·
// 									0--������·

	
	//0x0c
	ASSERT(m_dRLength >=0 && m_dRLength < 100000);			//·�γ���
	//0x10
	ASSERT(m_nVtxStartID < 20000);//·����ʼ�ڵ�ID
	ASSERT(m_nVtxEndID < 20000);	//·��β�ڵ�ID
	//0x14
	ASSERT(m_byNormalNum < 22);	//�Ƿ���ڽ�ͨ����		:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
	ASSERT(	m_byReverseNum < 22);	//�Ƿ�����ڽ�ͨ����	:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
	ASSERT(	m_byVirtualPicNum == 0);	//����ͼ����
	ASSERT(	m_byRutType < 20);	//��·����:0--����1--δ�ɼ�����2--�����߷���3--��������·֮��ĵ�·
	//0x18
	ASSERT	(m_nRBID < 100000);			//·������ͼ��ID
}
void CGIS_NetworkRoadSectionV2::Dump(CDumpContext& dc) const
{

}
#endif
//////////////////////////////////////////////////////////////////////////
BOOL CGIS_NetworkRoadSectionV2::IsInSameRoute(
    CGIS_NetworkRoadSectionV2* pRoadSection0,
    CGIS_NetworkRoadSectionV2* pRoadSection1)
{
    BOOL bRtn = TRUE;
	if( pRoadSection0->GetRutDirection()    != pRoadSection1->GetRutDirection() )
    {
        //TRACE2("Direction Different:%d<->%d\n",pRoadSection0->GetRutDirection(),pRoadSection1->GetRutDirection());
        bRtn = FALSE;
    }
	if( pRoadSection0->GetType()         != pRoadSection1->GetType()      )
    {
        //TRACE2("Type Different      :%d<->%d\n",pRoadSection0->GetType(),pRoadSection1->GetType());
        bRtn = FALSE;
    }
	if( pRoadSection0->GetRutCharge()        != pRoadSection1->GetRutCharge()     )
    {
        //TRACE2("Charge Different    :%d<->%d\n",pRoadSection0->GetRutCharge(),pRoadSection1->GetRutCharge());
        bRtn = FALSE;
    }


    return bRtn;
}

unsigned long CGIS_NetworkRoadSectionV2::GetPattern()
{
    return this->GetRutCharge()+this->GetRutDirection()*10+this->GetType()*100;
}
