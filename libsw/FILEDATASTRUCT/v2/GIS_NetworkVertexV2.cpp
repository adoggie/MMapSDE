/**
 * @file   GIS_NetworkVertexV2.cpp
 * @brief 
 *
 * @purpose 
 * 
 *
 * @author  
 * @date    2006/10/25
 */

#include "stdafx.h"
#include "GIS_NetworkVertexV2.h"

//////////////////////////////////////////////////////////////////////////
//this code for the memmgr limit value
#ifdef _DEBUG
unsigned long CGIS_NetworkVertexV2::m_nCount = 0;
unsigned long CGIS_NetworkVertexV2::m_nMaxCount = 0;	
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkVertexV2::CGIS_NetworkVertexV2( )
{
	memset(this,0,sizeof(CGIS_NetworkVertexV2));

#ifdef _DEBUG
	m_nCount++;
	if(m_nCount >= m_nMaxCount)
	{
		m_nMaxCount = m_nCount;
	}
#endif

}
CGIS_NetworkVertexV2::~CGIS_NetworkVertexV2()
{	
#ifdef _DEBUG
	m_nCount--;
#endif
}




//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
//start
#ifdef MEMORY_SELF_MANAGE

CGIS_NetworkVertexV2* CGIS_NetworkVertexV2::m_pArrayOfObj = NULL;
char* CGIS_NetworkVertexV2::m_pOccupiedSign = NULL;
int CGIS_NetworkVertexV2::m_CurrentPosOfArray = 0;
int CGIS_NetworkVertexV2::m_CurrentObjCount = 0;

int* CGIS_NetworkVertexV2::m_pFreeObjectStack = NULL;
int CGIS_NetworkVertexV2::m_TopOfFreeObjectStack = 0;
//--------------------------------------
void* CGIS_NetworkVertexV2::operator new(size_t size)
{	
	if(!m_pArrayOfObj)
	{
		TRACE1("CGIS_NetworkVertexV2 保留空间(%d个对象).\n",MAX_MEMORY_OBJ_COUNT);
		m_pArrayOfObj = (CGIS_NetworkVertexV2*)malloc(sizeof(CGIS_NetworkVertexV2)*MAX_MEMORY_OBJ_COUNT);
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
			TRACE0("CGIS_NetworkVertexV2 保留空间过小.\n");
			return NULL;
		}

		m_CurrentObjCount++;

		m_pOccupiedSign[m_pFreeObjectStack[m_TopOfFreeObjectStack]] = 1;
		m_TopOfFreeObjectStack--;
		
		return &m_pArrayOfObj[m_pFreeObjectStack[m_TopOfFreeObjectStack+1]];

		//THROW(new CException(TRUE));
	}
	TRACE0("CGIS_NetworkVertexV2 保留空间过小.\n");
	return NULL;
}

void CGIS_NetworkVertexV2::operator delete (void* p,size_t size)
{
	if(!m_pArrayOfObj)
	{
		return;
	}

	char* pArrayStart = (char*)m_pArrayOfObj;
	char* pDest = (char*)p;
	int nPosOfOccupiedSign = (pDest - pArrayStart) / sizeof(CGIS_NetworkVertexV2);
	
	if(0 <= nPosOfOccupiedSign && nPosOfOccupiedSign < MAX_MEMORY_OBJ_COUNT)
	{
		m_CurrentObjCount--;
		m_pOccupiedSign[nPosOfOccupiedSign] = 0;
		
		m_TopOfFreeObjectStack++;
		m_pFreeObjectStack[m_TopOfFreeObjectStack] = nPosOfOccupiedSign;

		if(	m_CurrentObjCount == 0)
		{
			TRACE0("CGIS_NetworkVertexV2 删除保留空间.\n");

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
void CGIS_NetworkVertexV2::AssertValid() const
{
	ASSERT( m_nVtxID < 20000 );
	ASSERT( m_byRoadSectionNum < 11 );	

	ASSERT( m_fLat < 180.0f && m_fLat > -180.0f );
	ASSERT( m_fLog < 180.0f && m_fLog > -180.0f );	

	ASSERT( m_nVBID < 100000 );					//节点所在图幅ID

	ASSERT( m_pRoadSectionList);				//相连接的路段ID
	ASSERT( m_pRTList );						//与水平方向的夹角数组
	ASSERT( m_pRoadSectionLayerIDLList);		//路段所在图层ID
	
	for(int i = 0; i < m_byRoadSectionNum; i++)
	{
		ASSERT( m_pRoadSectionList[i] < 20000 );
		ASSERT( m_pRTList[i] <= 360 &&  m_pRTList[i] >= 0);
		ASSERT( m_pRoadSectionLayerIDLList[i] < 25 );
	}
	if(m_pEdgeInfo)
	{
		ASSERT(m_pEdgeInfo->byEdge == 1);
		ASSERT(m_pEdgeInfo->nBreadthID < 1000000);
		ASSERT(m_pEdgeInfo->nRoadSectionID < 20000);
	}
		
}
void CGIS_NetworkVertexV2::Dump() const
{
	TRACE0("Dump()-----------\n");

	TRACE1("m_nVtxID = %d \n",m_nVtxID);
	TRACE1("m_byRoadSectionNum = %d \n",m_byRoadSectionNum);
	TRACE1("m_fLat = %f \n",m_fLat);
	TRACE1("m_fLog = %f \n",m_fLog);
}
#endif
//////////////////////////////////////////////////////////////////////////
