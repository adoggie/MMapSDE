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