// GIS_NetworkRoadSectionV6.h: interface for the CGIS_NetworkRoadSectionV6 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_NETWORKROADSECTIONV6_H__8510A428_24FA_44D5_BD2A_3878F010815C__INCLUDED_)
#define AFX_GIS_NETWORKROADSECTIONV6_H__8510A428_24FA_44D5_BD2A_3878F010815C__INCLUDED_

#include "../V5/GIS_NetworkRoadSectionV5.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGIS_NetworkRoadSectionV6  : public CGIS_NetworkRoadSectionV5
{
public:
	CGIS_NetworkRoadSectionV6();
	virtual ~CGIS_NetworkRoadSectionV6();
public:
    BOOL m_bValid;

};

#endif // !defined(AFX_GIS_NETWORKROADSECTIONV6_H__8510A428_24FA_44D5_BD2A_3878F010815C__INCLUDED_)
