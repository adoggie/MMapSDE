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
		TRACE1("CGIS_NetworkRoadSectionV2 保留空间(%d个对象).\n",MAX_MEMORY_OBJ_COUNT);
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
			TRACE0("CGIS_NetworkRoadSectionV2 保留空间过小.\n");
			return NULL;
		}

		m_CurrentObjCount++;

		ASSERT(m_pOccupiedSign[m_pFreeObjectStack[m_TopOfFreeObjectStack]] == 0);
		m_pOccupiedSign[m_pFreeObjectStack[m_TopOfFreeObjectStack]] = 1;
		m_TopOfFreeObjectStack--;
		
		return &m_pArrayOfObj[m_pFreeObjectStack[m_TopOfFreeObjectStack+1]];

		//THROW(new CException(TRUE));
	}
	TRACE0("CGIS_NetworkRoadSectionV2 保留空间过小.\n");
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
			TRACE0("CGIS_NetworkRoadSectionV2 删除保留空间.\n");
			
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
	ASSERT(m_nRutID < 20000);			//路段ID
	
	//0x04
	ASSERT(m_byDirection < 4 );		//路段单双行标志(0表示双行，1表示end到start,2表示start到end,3表示双向禁行)
	ASSERT(m_byType < 7 );			//道路类别:(0--高速，1--都市高速路，2--国道，3--主要地方道&轮渡
										//4--一般县道，5--市镇村道，6--其他道路）
	ASSERT(m_byCharge < 4);			//收费:0--不收费1--双向收费2--正向收费3--反向收费
	ASSERT(m_byLaneNum < 256);			//路段车道数:无数据的根据道路类别有缺省值
	
	//0x08
	ASSERT(m_bySpeed < 256);			//速度级别:根据道路类别有缺省速度
	ASSERT(m_byRutLevel < 6);		//道路功能级别:
//									5--一级道路
//									4--二级道路
//									3--三级道路
//									2--四级道路
//									1--五级道路
// 									0--六级道路

	
	//0x0c
	ASSERT(m_dRLength >=0 && m_dRLength < 100000);			//路段长度
	//0x10
	ASSERT(m_nVtxStartID < 20000);//路段起始节点ID
	ASSERT(m_nVtxEndID < 20000);	//路段尾节点ID
	//0x14
	ASSERT(m_byNormalNum < 22);	//是否存在交通限制		:0-没有交通限制;非0- 有交通限制(且表示路段个数)
	ASSERT(	m_byReverseNum < 22);	//是否反向存在交通限制	:0-没有交通限制;非0- 有交通限制(且表示路段个数)
	ASSERT(	m_byVirtualPicNum == 0);	//虚拟图数量
	ASSERT(	m_byRutType < 20);	//道路类型:0--环岛1--未采集属性2--上下线分离3--两条高速路之间的道路
	//0x18
	ASSERT	(m_nRBID < 100000);			//路段所在图幅ID
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
