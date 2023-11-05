// DataFileAnalyse.cpp: implementation of the CDataFileAnalyse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sdetest.h"
#include "DataFileAnalyse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataFileAnalyse::CDataFileAnalyse()
{

}

CDataFileAnalyse::~CDataFileAnalyse()
{
	
}

void CDataFileAnalyse::DumpState()
{
	TRACE0("数据文件分析-------------------------------------\n");
	TRACE1("图幅:%d .\n",m_nBreadthCount);
	TRACE1("路段:%d .\n",m_nBreadthCount);
	TRACE1("节点:%d .\n",m_nBreadthCount);
	TRACE1("边缘节点:%d .\n",m_nBreadthCount);
	TRACE1("0条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[0]);
	TRACE1("1条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[1]);
	TRACE1("2条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[2]);
	TRACE1("3条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[3]);
	TRACE1("4条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[4]);
	TRACE1("5条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[5]);
	TRACE1("6条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[6]);
	TRACE1("7条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[7]);
	TRACE1("8条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[8]);
	TRACE1("9条edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[9]);
	TRACE1("10条及以上edge的节点:%d .\n",m_nVertexWithSpecialEdgeCount[10]);

}
