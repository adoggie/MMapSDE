/**
 * @file   GIS_NetworkVertexV2.h
 * @brief 
 *
 * @purpose 
 * 
 *
 * @author  
 * @date    2006/10/25
 */
#if !defined(_GIS_NETWORKVERTEXV2_H_)
#define _GIS_NETWORKVERTEXV2_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoGlobe.h"


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//#define MEMORY_SELF_MANAGE
//////////////////////////////////////////////////////////////////////////

class CGIS_NetworkVertexV2  
{
public:
	CGIS_NetworkVertexV2( );
	~CGIS_NetworkVertexV2();

	void SetBaseAddr(char* pBaseAddr);
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
//////////////////////////////////////////////////////////////////////////
///�ڲ���������ݽṹ
public:
	//��Ե�ڵ���Ϣ�ṹ
	struct ST_GEO_VEDGE
	{
		unsigned char			byEdge;				//�Ƿ���ͼ����Ե:0-����,1-��
		unsigned char			unUsed;		
		unsigned short			nRoadSectionID;		//����Rut_id
		int						nBreadthID;		    //�����ID	
	};

	//Search��Ϣ�ṹ
	//����ڵ��������Ϣ
	struct ST_GEO_VSORT
	{
		BYTE bySHead;	//Vertex status:0 1 2 ,	0 ����δ�������,1 OPEN, 2 CLOSE
		BYTE bySEnd;	//Vertex status:0 1 2 ,0 ���� δ�������,1 OPEN,2 CLOSE
		unsigned short unUse;

		POSITION posOHead;
		POSITION posOEnd;
	};
//////////////////////////////////////////////////////////////////////////
public:

	unsigned short	GetVtxID( );
	float GetLat();
	float GetLog();

	ST_GEO_PTXY		GetVtxPosition( );
	unsigned char	GetRoadSectionNum();
	unsigned short* GetRoadSectionList();
	unsigned short* GetRoadSectionTurnList( );
	unsigned short*	GetRoadSectionLayerIDList( );
    unsigned char	GetEdge();
	int				GetEdgeBreadthID( );
	unsigned short	GetEdgeRoadSectionID( );
	int				GetVBreadthID( );

	ST_GEO_VSORT*	GetVtxSInfo( );


//////////////////////////////////////////////////////////////////////////
//���ݳ�Ա
public:
	//0x00
	unsigned short	m_nVtxID;		//�ڵ�ID
	unsigned char	m_byRoadSectionNum;		//�����ӵ�·����
	unsigned char	m_byEdge;				//�Ƿ��Ǳ߽���

	//0x04
	float m_fLat;//γ������
	//0x08
	float m_fLog;//��������
	//0x0c
	unsigned long	m_nVBID;							//�ڵ�����ͼ��ID
	//0x10
	unsigned short	*m_pRoadSectionList;				//�����ӵ�·��ID
	//0x14
	unsigned short	*m_pRTList;							//��ˮƽ����ļн�����
	//0x18
	unsigned short	*m_pRoadSectionLayerIDLList;		//·������ͼ��ID
	//0x1c
	ST_GEO_VEDGE	*m_pEdgeInfo;		//��Ե�ڵ���Ϣ
	//0x20
	ST_GEO_VSORT	*m_pVSInfo;		//�ڵ����ʱ����Ϣ

	
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
//lijun add for memmory manage
#ifdef MEMORY_SELF_MANAGE
	
public:
	void* operator new(size_t size);
	void operator delete (void*,size_t);

protected:
	enum enConstValue
	{
		MAX_MEMORY_OBJ_COUNT = 50000,
	};
	static CGIS_NetworkVertexV2* m_pArrayOfObj;
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
public:
	void AssertValid() const;
	void Dump() const;
#endif
//////////////////////////////////////////////////////////////////////////
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//���ñ䳤������ַ
inline void CGIS_NetworkVertexV2::SetBaseAddr(char* pBaseAddr)
{
	if(m_pRoadSectionList)
		m_pRoadSectionList			= (unsigned short*)(	((DWORD)m_pRoadSectionList)			+ pBaseAddr);
	if(m_pRTList)
		m_pRTList					= (unsigned short*)(	((DWORD)m_pRTList)					+ pBaseAddr);
	if(m_pRoadSectionLayerIDLList)
		m_pRoadSectionLayerIDLList	= (unsigned short*)(	((DWORD)m_pRoadSectionLayerIDLList)	+ pBaseAddr);
	if(m_pEdgeInfo)
		m_pEdgeInfo					= (ST_GEO_VEDGE*)(		((DWORD)m_pEdgeInfo)				+ pBaseAddr);
//	if(m_pVSInfo)
//		m_pVSInfo					= (ST_GEO_VSORT*)(		((DWORD)m_pVSInfo)					+ pBaseAddr);

}

//���ñ䳤���ݻ�ַ
inline void CGIS_NetworkVertexV2::SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr)
{
	if(m_byRoadSectionNum)
	{
		m_pRoadSectionList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_byRoadSectionNum*sizeof(unsigned short);

		m_pRTList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_byRoadSectionNum*sizeof(unsigned short);

		m_pRoadSectionLayerIDLList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_byRoadSectionNum*sizeof(unsigned short);

	}

	if(m_byEdge)
	{
		m_pEdgeInfo = (ST_GEO_VEDGE*)pDwordAddr;
		pDwordAddr += sizeof(ST_GEO_VEDGE);
	}
}

inline unsigned short CGIS_NetworkVertexV2::GetVtxID( )
{
	return m_nVtxID;
}
inline float CGIS_NetworkVertexV2::GetLat()
{
	return m_fLat;
}
inline float CGIS_NetworkVertexV2::GetLog()
{
	return m_fLog;
}
inline ST_GEO_PTXY CGIS_NetworkVertexV2::GetVtxPosition( )
{
	ST_GEO_PTXY aPt;
	aPt.dx = m_fLog;
	aPt.dy = m_fLat;
	return aPt;
}
inline BYTE CGIS_NetworkVertexV2::GetRoadSectionNum( )
{
	return m_byRoadSectionNum;
}
inline unsigned short* CGIS_NetworkVertexV2::GetRoadSectionList( )
{
	return m_pRoadSectionList;
}
inline unsigned short* CGIS_NetworkVertexV2::GetRoadSectionTurnList( )
{
	return m_pRTList;
}
inline unsigned short* CGIS_NetworkVertexV2::GetRoadSectionLayerIDList( )
{
	return m_pRoadSectionLayerIDLList;
}
inline BYTE CGIS_NetworkVertexV2::GetEdge( )
{
	if( m_pEdgeInfo )
		return m_pEdgeInfo->byEdge;
	return 0;
}
inline int CGIS_NetworkVertexV2::GetEdgeBreadthID( )
{
	if( m_pEdgeInfo )
		return m_pEdgeInfo->nBreadthID;
	return -1;
}
inline unsigned short CGIS_NetworkVertexV2::GetEdgeRoadSectionID()
{
	if( m_pEdgeInfo )
		return m_pEdgeInfo->nRoadSectionID;
	return -1;
}

inline int CGIS_NetworkVertexV2::GetVBreadthID()
{
	return m_nVBID;
}
inline CGIS_NetworkVertexV2::ST_GEO_VSORT* CGIS_NetworkVertexV2::GetVtxSInfo( )
{
	if( m_pVSInfo == NULL )
	{
		m_pVSInfo = new ST_GEO_VSORT;
		memset( m_pVSInfo, 0, sizeof(ST_GEO_VSORT) );
	}
	return m_pVSInfo;
}
#endif // !defined(_GIS_NETWORKVERTEXV2_H_)
