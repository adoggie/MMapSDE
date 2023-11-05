// GIS_NetworkVertexV6.h: interface for the CGIS_NetworkVertexV6 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_NETWORKVERTEXV6_H__65C9814B_812B_4E04_991B_1583D94BC33D__INCLUDED_)
#define AFX_GIS_NETWORKVERTEXV6_H__65C9814B_812B_4E04_991B_1583D94BC33D__INCLUDED_

#include "../V5/GIS_NetworkVertexV5.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGIS_NetworkVertexV6  : public CGIS_NetworkVertexV5
{
public:
	CGIS_NetworkVertexV6();
	virtual ~CGIS_NetworkVertexV6();
public:
    BOOL m_bValid;

};

#endif // !defined(AFX_GIS_NETWORKVERTEXV6_H__65C9814B_812B_4E04_991B_1583D94BC33D__INCLUDED_)
