/** 
 * Copyright (c) 2006
 * @file    GIS_NetworkRoadSectionV2.h
 * @brief	这个类是用于导航时进行计算的,并不暴露给应用层
 * @version
 * @author
 * @date    2006/08/25
 * 
 * This file is under the source control of VSS
 * $History: $
 * $Revision: $
 * $Date: $
 * $NoKeywords: $ 
 */

#if !defined(_GIS_NETWORKROADSECTIONV2_H_)
#define _GIS_NETWORKROADSECTIONV2_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//////////////////////////////////////////////////////////////////////////
//#define MEMORY_SELF_MANAGE
//////////////////////////////////////////////////////////////////////////

class CGIS_NetworkRoadSectionV2  
{
public:
	CGIS_NetworkRoadSectionV2();
	~CGIS_NetworkRoadSectionV2();

	void SetBaseAddr(char* pBaseAddr);
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
	static BOOL IsInSameRoute(CGIS_NetworkRoadSectionV2* pRoadSection0,CGIS_NetworkRoadSectionV2* pRoadSection1);
	unsigned long GetPattern();

public:	
	//////////////////////////////////////////////////////////////////////////
	//取得各个属性
	int				GetRutID(void);
	unsigned char	GetRutDirection(void);
	unsigned char	GetType(void);
	unsigned char	GetRutCharge(void);
	unsigned char	GetRutLaneNum();
	unsigned char	GetRutSpeed(void);
	unsigned char	GetRutType(void);
	unsigned char	GetRutLevel(void);
	float			GetRutLength(void);
	unsigned short	GetRutStartVtxID(void);
	unsigned short	GetRutEndVtxID(void);
	int				GetRBreadthID(void);

	unsigned char	GetLimitNormalNum(void);
	unsigned short* GetLimitNormalList(void);
	unsigned char	GetLimitReverseNum(void);
	unsigned short* GetLimitReverseList(void);

	//这两个接口用于海外
	unsigned short GetRAngleStart( );
	unsigned short GetRAngleEnd( );

//////////////////////////////////////////////////////////////////////////
//数据成员
public:
	unsigned short	m_nRutID;			//路段ID	
	unsigned char	m_byDirection;		//路段单双行标志(0表示双行，1表示end到start,2表示start到end,3表示双向禁行)
	unsigned char	m_byType;			//道路类别:(0--高速，1--都市高速路，2--国道，3--主要地方道&轮渡
										//4--一般县道，5--市镇村道，6--其他道路）

	unsigned char	m_byCharge;			//收费:0--不收费1--双向收费2--正向收费3--反向收费
	unsigned char	m_byLaneNum;			//路段车道数:无数据的根据道路类别有缺省值
	unsigned char	m_bySpeed;			//速度级别:根据道路类别有缺省速度
	unsigned char	m_byRutLevel;		//道路功能级别:

	float	m_dRLength;			//路段长度

	unsigned short m_nVtxStartID;//路段起始节点ID
	unsigned short m_nVtxEndID;	//路段尾节点ID

	unsigned char	m_byNormalNum;	//是否存在交通限制		:0-没有交通限制;非0- 有交通限制(且表示路段个数)
	unsigned char	m_byReverseNum;	//是否反向存在交通限制	:0-没有交通限制;非0- 有交通限制(且表示路段个数)
	unsigned char	m_byVirtualPicNum;	//虚拟图数量
	unsigned char	m_byRutType;	//道路类型:0--环岛1--未采集属性2--上下线分离3--两条高速路之间的道路

	unsigned long	m_nRBID;			//路段所在图幅ID


	unsigned short	*m_pNormalList;		//禁止通行的路段id
	unsigned short	*m_pReverseList;	//反向禁止通行的路段id
	unsigned long	*m_pVirtualPicList;	//虚拟图链表		

//////////////////////////////////////////////////////////////////////////
//this code for the memmgr limit value
	//start
	
#ifdef _DEBUG
public:
	static unsigned long m_nCount;
	static unsigned long m_nMaxCount;
#endif
	
	//end
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
////lijun add for memmory manage
#ifdef MEMORY_SELF_MANAGE

public:
	void* operator new(size_t size);
	void operator delete(void*,size_t);

protected:
	enum enConstValue
	{
		MAX_MEMORY_OBJ_COUNT = 50000,
	};
	static CGIS_NetworkRoadSectionV2* m_pArrayOfObj;
	static char* m_pOccupiedSign;
	static int m_CurrentPosOfArray;
	static int m_CurrentObjCount;
	
	static int* m_pFreeObjectStack;
	static int m_TopOfFreeObjectStack;
#endif
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// this code for check self
#ifdef _DEBUG
	typedef void* CDumpContext;
public:
	void AssertValid() const;
	void Dump(CDumpContext& dc) const;
#endif
//////////////////////////////////////////////////////////////////////////

};

//设置变长数据基址
inline void CGIS_NetworkRoadSectionV2::SetBaseAddr(char* pBaseAddr)
{
	if(m_pNormalList)
		m_pNormalList		= (unsigned short*)(	((DWORD)m_pNormalList)		+ pBaseAddr);
	if(m_pReverseList)
		m_pReverseList		= (unsigned short*)(	((DWORD)m_pReverseList)		+ pBaseAddr);
	if(m_pVirtualPicList)
		m_pVirtualPicList	= (unsigned long*)(		((DWORD)m_pVirtualPicList)	+ pBaseAddr);
}

//设置变长数据基址
inline void CGIS_NetworkRoadSectionV2::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
{
	if(m_byNormalNum)
	{
		m_pNormalList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_byNormalNum*sizeof(unsigned short);
	}

	if(m_byReverseNum)
	{
		m_pReverseList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_byReverseNum*sizeof(unsigned short);
	}
}

inline int CGIS_NetworkRoadSectionV2::GetRutID( )
{
	return m_nRutID;
}
inline unsigned char CGIS_NetworkRoadSectionV2::GetRutDirection( )
{
	return m_byDirection;
}
inline unsigned char CGIS_NetworkRoadSectionV2::GetRutType( )
{
	return m_byRutType;
}
inline unsigned char CGIS_NetworkRoadSectionV2::GetType( )
{
	return m_byType;
}
inline unsigned char CGIS_NetworkRoadSectionV2::GetRutLaneNum()
{
	return m_byLaneNum;
}
inline unsigned char CGIS_NetworkRoadSectionV2::GetRutCharge( )
{
	return m_byCharge;
}
inline unsigned char CGIS_NetworkRoadSectionV2::GetRutSpeed( )
{
	return m_bySpeed;
}
inline unsigned char CGIS_NetworkRoadSectionV2::GetRutLevel( )
{
	return m_byRutLevel;
}
inline float CGIS_NetworkRoadSectionV2::GetRutLength( )
{
	return m_dRLength;
}
inline unsigned short CGIS_NetworkRoadSectionV2::GetRutStartVtxID( )
{
	return m_nVtxStartID;
}
inline unsigned short CGIS_NetworkRoadSectionV2::GetRutEndVtxID( )
{
	return m_nVtxEndID;
}
inline int CGIS_NetworkRoadSectionV2::GetRBreadthID( )
{
	return m_nRBID;
}
inline unsigned char CGIS_NetworkRoadSectionV2::GetLimitNormalNum()
{
	return m_byNormalNum;
}
inline unsigned short* CGIS_NetworkRoadSectionV2::GetLimitNormalList( )
{
	return m_pNormalList;
}
inline unsigned char CGIS_NetworkRoadSectionV2::GetLimitReverseNum()
{
	return m_byReverseNum;
}
inline unsigned short* CGIS_NetworkRoadSectionV2::GetLimitReverseList( )
{
	return m_pReverseList;
}

#endif // !defined(_GIS_NETWORKROADSECTIONV2_H_)
