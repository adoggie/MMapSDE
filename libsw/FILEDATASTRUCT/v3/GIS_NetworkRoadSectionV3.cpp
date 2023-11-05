// Gis_NetworkRoadSectionV3.cpp: implementation of the CGIS_NetworkRoadSectionV3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Gis_NetworkRoadSectionV3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGIS_NetworkRoadSectionV3::CGIS_NetworkRoadSectionV3()
{
    m_bValid = TRUE;
    m_nLayer = 0xff;
}

CGIS_NetworkRoadSectionV3::~CGIS_NetworkRoadSectionV3()
{

}

BOOL CGIS_NetworkRoadSectionV3::Calcu32Data()
{
	m_n32ID             = (m_nRBID<<15) + m_nRutID;			//Â·¶ÎID
    m_n32StartVertexID  = (m_nRBID<<15) + m_nVtxStartID;
    m_n32EndVertexID    = (m_nRBID<<15) + m_nVtxEndID;

	
	{
		int nLimitNum = GetLimitNormalNum();
		unsigned short* p16List = GetLimitNormalList();
		
		unsigned long* p32List = new unsigned long[nLimitNum];
		for(int i = 0; i < nLimitNum; i++)
		{
			p32List[i] = (m_nRBID<<15)+p16List[i];
		}

		m_p32NormalList = p32List;
	}
	{
		int nLimitNum = GetLimitReverseNum();
		unsigned short* p16List = GetLimitReverseList();
		
		unsigned long* p32List = new unsigned long[nLimitNum];
		for(int i = 0; i < nLimitNum; i++)
		{
			p32List[i] = (m_nRBID<<15)+p16List[i];
		}

		m_p32ReverseList = p32List;

	}

	return TRUE;

}
