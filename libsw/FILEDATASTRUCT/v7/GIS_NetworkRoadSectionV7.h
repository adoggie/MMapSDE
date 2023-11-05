/**
 * @brief 主干路网的路段的对象定义
 *
 * @param 
 * @return
*/
#if !defined(AFX_GIS_NETWORKROADSECTIONV7_H__9519E6B9_B3D4_4D97_8B61_EC3033100850__INCLUDED_)
#define AFX_GIS_NETWORKROADSECTIONV7_H__9519E6B9_B3D4_4D97_8B61_EC3033100850__INCLUDED_

#include "NetworkFileStructV7.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGIS_NetworkRoadSectionV7  
{
public:
	CGIS_NetworkRoadSectionV7();
	~CGIS_NetworkRoadSectionV7();

    friend class CGIS_NetworkVtx;    
    CGIS_NetworkRoadSectionV7* CreateCloneObject();
    void DestoryCloneObject();

    StuNetworkFileStructV7::stuRoadSection* GetDataAddr(){return &m_stData;};
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
public:	
	//////////////////////////////////////////////////////////////////////////
	//取得各个属性
    unsigned long	GetRutID(void){return m_stData.m_nRutID;};
    unsigned long   GetBreadthID(void){return m_stData.m_nRBID;};

    unsigned char	GetDirection(void){return m_stData.m_byDirection&0x0f;};
    unsigned char	GetCharge(void){return m_stData.m_byCharge;};
    unsigned char	GetLevel(void){return m_stData.m_byRutLevel;};
    unsigned char	GetRutType(void){return m_stData.m_byRutType;};
    unsigned char	GetType(void){return m_stData.m_byType;};
    unsigned char	GetRutCharge(void){return m_stData.m_byCharge;};
    
    unsigned long	GetStartVtxID(void){return m_stData.m_nStartVertexID;};
    unsigned long	GetEndVtxID(void){return m_stData.m_nEndVertexID;};

    unsigned char	GetLimitNormalNum(void){return m_stData.m_byNormalNum;};
    unsigned char	GetLimitReverseNum(void){return m_stData.m_byReverseNum;};
    float			GetLength(void){return m_stData.m_dRLength;};
    
    unsigned short* GetLimitNormalList(void){return m_pNormalList;};
    unsigned short* GetLimitReverseList(void){return m_pReverseList;};


//////////////////////////////////////////////////////////////////////////
//数据成员
public:
    StuNetworkFileStructV7::stuRoadSection m_stData;
	unsigned short	*m_pNormalList;		//禁止通行的路段id
	unsigned short	*m_pReverseList;	//反向禁止通行的路段id

//////////////////////////////////////////////////////////////////////////
// this code for check self
#ifdef _DEBUG
	typedef void* CDumpContext;
	void AssertValid() const;
	void Dump(CDumpContext& dc) const;
#endif
//////////////////////////////////////////////////////////////////////////

};


//设置变长数据基址
inline void CGIS_NetworkRoadSectionV7::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
{
	if(m_stData.m_byNormalNum)
	{
		m_pNormalList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_stData.m_byNormalNum*sizeof(unsigned short);
	}

	if(m_stData.m_byReverseNum)
	{
		m_pReverseList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_stData.m_byReverseNum*sizeof(unsigned short);
	}
}


#endif // !defined(AFX_GIS_NETWORKROADSECTIONV7_H__9519E6B9_B3D4_4D97_8B61_EC3033100850__INCLUDED_)
