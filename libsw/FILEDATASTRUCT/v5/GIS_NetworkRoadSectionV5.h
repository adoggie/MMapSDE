/**
 * @brief 主干路网的路段的对象定义
 *
 * @param 
 * @return
*/
#if !defined(AFX_GIS_NETWORKROADSECTIONV5_H__9519E6B9_B3D4_4D97_8B61_EC3033100850__INCLUDED_)
#define AFX_GIS_NETWORKROADSECTIONV5_H__9519E6B9_B3D4_4D97_8B61_EC3033100850__INCLUDED_

#include "NetworkFileStructV5.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IMemoryMng;
class IMemoryMng2;
//////////////////////////////////////////////////////////////////////////
#ifdef MEMORY_SELF_MANAGE
#undef MEMORY_SELF_MANAGE
#endif

//#define MEMORY_SELF_MANAGE
//////////////////////////////////////////////////////////////////////////

class CGIS_NetworkRoadSectionV5  
{
public:
	CGIS_NetworkRoadSectionV5();
	~CGIS_NetworkRoadSectionV5();

    StuNetworkFileStructV5::stuRoadSection* GetDataAddr(){return &m_stData;};
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
	void MakeBuddyIndependent();
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
    
    unsigned long	GetStartVtxID(void){return m_stData.m_nStartVertexID;};
    unsigned long	GetEndVtxID(void){return m_stData.m_nEndVertexID;};

    unsigned char	GetLimitNormalNum(void){return m_stData.m_byNormalNum;};
    unsigned char	GetLimitReverseNum(void){return m_stData.m_byReverseNum;};
    float			GetLength(void){return m_stData.m_dRLength;};
    
    unsigned long* GetLimitNormalList(void){return m_pNormalList;};
    unsigned long* GetLimitReverseList(void){return m_pReverseList;};


//////////////////////////////////////////////////////////////////////////
//数据成员
public:
    StuNetworkFileStructV5::stuRoadSection m_stData;
	unsigned long	*m_pNormalList;		//禁止通行的路段id
	unsigned long	*m_pReverseList;	//反向禁止通行的路段id

//////////////////////////////////////////////////////////////////////////
// this code for check self
#ifdef _DEBUG
	typedef void* CDumpContext;
	void AssertValid() const;
	void Dump(CDumpContext& dc) const;
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//lijun add for memmory manage
#ifdef MEMORY_SELF_MANAGE
public:
	void* operator new(size_t size);
	void operator delete (void*,size_t);

    static IMemoryMng2* m_pMemMngOfSelf;
    static IMemoryMng* m_pMemMngOfBuddyData;
#endif	
//////////////////////////////////////////////////////////////////////////
};


//设置变长数据基址
inline void CGIS_NetworkRoadSectionV5::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
{
	if(m_stData.m_byNormalNum)
	{
		m_pNormalList	= (unsigned long*)(pDwordAddr);
		pShortAddr += m_stData.m_byNormalNum*sizeof(unsigned long);
	}

	if(m_stData.m_byReverseNum)
	{
		m_pReverseList	= (unsigned long*)(pDwordAddr);
		pShortAddr += m_stData.m_byReverseNum*sizeof(unsigned long);
	}
}


#endif // !defined(AFX_GIS_NETWORKROADSECTIONV5_H__9519E6B9_B3D4_4D97_8B61_EC3033100850__INCLUDED_)
