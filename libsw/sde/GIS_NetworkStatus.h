
#ifndef _GIS_NETWORK_STATUS_H
#define _GIS_NETWORK_STATUS_H

#include "swBase.h"

class CGIS_NetworkStatus  
{
public:
	CGIS_NetworkStatus();
	virtual ~CGIS_NetworkStatus();
public:

    static int m_nCalculateStatus;//0-δ��ʼ����.1-���ڼ�����
    static long m_bCalculateStopSign;
    static BYTE   m_nStartCalculateTime;
};

#endif 
