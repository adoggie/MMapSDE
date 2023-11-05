// Gis_NetworkVertexV3.h: interface for the CGIS_NetworkVertexV3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_NETWORKVERTEXV3_H__A848D9D9_5780_4F1C_B6D1_610937D9F361__INCLUDED_)
#define AFX_GIS_NETWORKVERTEXV3_H__A848D9D9_5780_4F1C_B6D1_610937D9F361__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../V2/GIS_NetworkVertexV2.h"

class CGIS_NetworkVertexV3  : public CGIS_NetworkVertexV2
{
public:
	CGIS_NetworkVertexV3();
	~CGIS_NetworkVertexV3();

	BOOL Calcu32Data();
    ////////////////////////////////////////////////////////////////////////// 
    unsigned long   Get32ID(){return m_n32ID;};
    unsigned long*  Get32RoadSectionList(){return m_p32RoadSectionList;};				   
    unsigned long	Get32EdgeRoadSectionID(){return m_n32EdgeRoadSectionID;};
    ;					

//////////////////////////////////////////////////////////////////////////
public:
	unsigned long   m_n32ID;
	unsigned long   m_nEdgeBreadthID;
	unsigned long*  m_p32RoadSectionList;				//相连接的路段ID
	unsigned long	m_n32EdgeRoadSectionID;					//边界连接Rut_id

    unsigned char   m_bValid;
    unsigned char   m_nLayer;               //0,1,2三层
};

#endif // !defined(AFX_GIS_NETWORKVERTEXV3_H__A848D9D9_5780_4F1C_B6D1_610937D9F361__INCLUDED_)
