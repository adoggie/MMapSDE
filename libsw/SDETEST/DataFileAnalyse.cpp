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
	TRACE0("�����ļ�����-------------------------------------\n");
	TRACE1("ͼ��:%d .\n",m_nBreadthCount);
	TRACE1("·��:%d .\n",m_nBreadthCount);
	TRACE1("�ڵ�:%d .\n",m_nBreadthCount);
	TRACE1("��Ե�ڵ�:%d .\n",m_nBreadthCount);
	TRACE1("0��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[0]);
	TRACE1("1��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[1]);
	TRACE1("2��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[2]);
	TRACE1("3��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[3]);
	TRACE1("4��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[4]);
	TRACE1("5��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[5]);
	TRACE1("6��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[6]);
	TRACE1("7��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[7]);
	TRACE1("8��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[8]);
	TRACE1("9��edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[9]);
	TRACE1("10��������edge�Ľڵ�:%d .\n",m_nVertexWithSpecialEdgeCount[10]);

}
