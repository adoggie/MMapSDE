// Gis_NetworkRoadSectionV3.h: interface for the CGIS_NetworkRoadSectionV3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_NETWORKROADSECTIONV3_H__8846572A_F110_4E70_B17F_D3A7D3971C1D__INCLUDED_)
#define AFX_GIS_NETWORKROADSECTIONV3_H__8846572A_F110_4E70_B17F_D3A7D3971C1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../V2/GIS_NetworkRoadSectionV2.h"

class CGIS_NetworkRoadSectionV3  : public CGIS_NetworkRoadSectionV2
{
public:
	CGIS_NetworkRoadSectionV3();
	~CGIS_NetworkRoadSectionV3();

	BOOL Calcu32Data();
    unsigned long Get32ID()             {return m_n32ID;}
    unsigned long Get32StartVertexID()  {return m_n32StartVertexID;}
    unsigned long Get32EndVertexID()    {return m_n32EndVertexID;}
    unsigned long* Get32LimitNormalList()    {return m_p32NormalList;}
    unsigned long* Get32LimitReverseList()   {return m_p32ReverseList;}

//////////////////////////////////////////////////////////////////////////
public:
	unsigned long 	m_n32ID;			        //路段ID
    unsigned long   m_n32StartVertexID;
    unsigned long   m_n32EndVertexID;

	unsigned long	*m_p32NormalList;		//禁止通行的路段id
	unsigned long	*m_p32ReverseList;	    //反向禁止通行的路段id


    
    unsigned char m_bValid;
    unsigned char m_nLayer;               //0,1,2三层
    

};

#endif // !defined(AFX_GIS_NETWORKROADSECTIONV3_H__8846572A_F110_4E70_B17F_D3A7D3971C1D__INCLUDED_)
