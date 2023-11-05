// GIS_BreadthMap.h: interface for the CGIS_BreadthMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_BREADTHMAP_H__1EB4A98F_D0EF_44E0_A08F_A3D1948E30DB__INCLUDED_)
#define AFX_GIS_BREADTHMAP_H__1EB4A98F_D0EF_44E0_A08F_A3D1948E30DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGIS_BreadthMap  
{
public:
	CGIS_BreadthMap( );
	~CGIS_BreadthMap();
	
public:
	int m_nBMID;
	int	m_nBreadthID;

	unsigned char	m_nLoadCounter;

};

#endif // !defined(AFX_GIS_BREADTHMAP_H__1EB4A98F_D0EF_44E0_A08F_A3D1948E30DB__INCLUDED_)
