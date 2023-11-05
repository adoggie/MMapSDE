// Gis_NetworkVertexV3.cpp: implementation of the CGIS_NetworkVertexV3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Gis_NetworkVertexV3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkVertexV3::CGIS_NetworkVertexV3()
{
    m_nEdgeBreadthID = 0xffffffff;
	m_p32RoadSectionList = NULL;
	m_n32EdgeRoadSectionID = 0xffffffff;
    m_bValid = TRUE;
    m_nLayer = 0XFF;
}

CGIS_NetworkVertexV3::~CGIS_NetworkVertexV3()
{

}

//////////////////////////////////////////////////////////////////////////

BOOL CGIS_NetworkVertexV3::Calcu32Data()
{
	ASSERT(m_nVBID < 100000);
	ASSERT(m_nVtxID < 20000);
	m_n32ID			=	(m_nVBID<<15) + m_nVtxID;			//Â·¶ÎID
	
	if(GetEdge())
	{
        int nEdgeBreadthID          = GetEdgeBreadthID();
        int nEdgeRoadSectionID      = GetEdgeRoadSectionID();
        ASSERT(nEdgeBreadthID       < 100000);
        ASSERT(nEdgeRoadSectionID   < 20000);

		m_n32EdgeRoadSectionID = (nEdgeBreadthID<<15) + nEdgeRoadSectionID;			//Â·¶ÎID
	}
	{
		int nNum = GetRoadSectionNum();
		ASSERT(nNum > 0);
		unsigned short* p16List = GetRoadSectionList();
		
		unsigned long* p32List = new unsigned long[nNum];
		for(int i = 0; i < nNum; i++)
		{
			p32List[i] = (m_nVBID<<15)+p16List[i];
		}

		m_p32RoadSectionList = p32List;
	}

	return TRUE;
}