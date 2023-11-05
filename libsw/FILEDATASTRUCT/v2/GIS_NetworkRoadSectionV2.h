/** 
 * Copyright (c) 2006
 * @file    GIS_NetworkRoadSectionV2.h
 * @brief	����������ڵ���ʱ���м����,������¶��Ӧ�ò�
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
	//ȡ�ø�������
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

	//�������ӿ����ں���
	unsigned short GetRAngleStart( );
	unsigned short GetRAngleEnd( );

//////////////////////////////////////////////////////////////////////////
//���ݳ�Ա
public:
	unsigned short	m_nRutID;			//·��ID	
	unsigned char	m_byDirection;		//·�ε�˫�б�־(0��ʾ˫�У�1��ʾend��start,2��ʾstart��end,3��ʾ˫�����)
	unsigned char	m_byType;			//��·���:(0--���٣�1--���и���·��2--������3--��Ҫ�ط���&�ֶ�
										//4--һ���ص���5--��������6--������·��

	unsigned char	m_byCharge;			//�շ�:0--���շ�1--˫���շ�2--�����շ�3--�����շ�
	unsigned char	m_byLaneNum;			//·�γ�����:�����ݵĸ��ݵ�·�����ȱʡֵ
	unsigned char	m_bySpeed;			//�ٶȼ���:���ݵ�·�����ȱʡ�ٶ�
	unsigned char	m_byRutLevel;		//��·���ܼ���:

	float	m_dRLength;			//·�γ���

	unsigned short m_nVtxStartID;//·����ʼ�ڵ�ID
	unsigned short m_nVtxEndID;	//·��β�ڵ�ID

	unsigned char	m_byNormalNum;	//�Ƿ���ڽ�ͨ����		:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
	unsigned char	m_byReverseNum;	//�Ƿ�����ڽ�ͨ����	:0-û�н�ͨ����;��0- �н�ͨ����(�ұ�ʾ·�θ���)
	unsigned char	m_byVirtualPicNum;	//����ͼ����
	unsigned char	m_byRutType;	//��·����:0--����1--δ�ɼ�����2--�����߷���3--��������·֮��ĵ�·

	unsigned long	m_nRBID;			//·������ͼ��ID


	unsigned short	*m_pNormalList;		//��ֹͨ�е�·��id
	unsigned short	*m_pReverseList;	//�����ֹͨ�е�·��id
	unsigned long	*m_pVirtualPicList;	//����ͼ����		

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

//���ñ䳤���ݻ�ַ
inline void CGIS_NetworkRoadSectionV2::SetBaseAddr(char* pBaseAddr)
{
	if(m_pNormalList)
		m_pNormalList		= (unsigned short*)(	((DWORD)m_pNormalList)		+ pBaseAddr);
	if(m_pReverseList)
		m_pReverseList		= (unsigned short*)(	((DWORD)m_pReverseList)		+ pBaseAddr);
	if(m_pVirtualPicList)
		m_pVirtualPicList	= (unsigned long*)(		((DWORD)m_pVirtualPicList)	+ pBaseAddr);
}

//���ñ䳤���ݻ�ַ
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
