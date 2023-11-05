
#ifndef _GIS_NETWORK_STATUS_H
#define _GIS_NETWORK_STATUS_H

#include "swBase.h"

class CGIS_NetworkStatus  
{
public:
	CGIS_NetworkStatus();
	virtual ~CGIS_NetworkStatus();
public:

    static int m_nCalculateStatus;//0-未开始计算.1-正在计算中
    static long m_bCalculateStopSign;
    static BYTE   m_nStartCalculateTime;
};

#endif 
