// GIS_NetworkVertexV5.h: interface for the CGIS_NetworkVertexV5 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_NETWORKVERTEXV5_H__A504F1A1_3E6A_4A4E_8A5D_CC9AC0A7DE64__INCLUDED_)
#define AFX_GIS_NETWORKVERTEXV5_H__A504F1A1_3E6A_4A4E_8A5D_CC9AC0A7DE64__INCLUDED_

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

//����ΪҪд��Ľڵ����Ľṹ����
//////////////////////////////////////////////////////////////////////////
class CGIS_NetworkVertexV5  
{
public:
	CGIS_NetworkVertexV5();
	~CGIS_NetworkVertexV5();
    
    StuNetworkFileStructV5::stuVertex* GetDataAddr(){return &m_stData;};
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
	void MakeBuddyIndependent();
//////////////////////////////////////////////////////////////////////////
///�ڲ���������ݽṹ
public:
	//��Ե�ڵ���Ϣ�ṹ
	struct ST_GEO_VEDGE
	{
		unsigned long			nRoadSectionID;		//������Rut_id	
	};

	//Search��Ϣ�ṹ
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

    unsigned long   GetVtxID(){return m_stData.m_nID;};
    unsigned long   GetBreadthID(){return m_stData.m_nBreadthID;};

    float GetLat(){return m_stData.m_fLat;};
    float GetLog(){return m_stData.m_fLog;};

    unsigned char	GetRoadSectionNum(){return m_stData.m_nRoadSectionNum;};
    unsigned long*  GetRoadSectionList(){return m_pRoadSectionList;};

    unsigned short* GetRoadSectionTurnList(){return m_pRoadSectionTurnList;};
	unsigned short*	GetRoadSectionLayerIDList(){return m_pRoadSectionLayerIDLList;};

    unsigned char	GetEdge(){return m_stData.m_nEdge&0x01;};
    unsigned long	GetEdgeRoadSectionID()
    {
        if(m_pEdgeInfo) 
            return m_pEdgeInfo->nRoadSectionID;
        else 
            return 0xffffffff;
    };

    ST_GEO_VSORT*	GetVtxSInfo()
    {
        if(!m_pVSInfo)
        {
            m_pVSInfo = new ST_GEO_VSORT;
            memset( m_pVSInfo, 0, sizeof(ST_GEO_VSORT) );
        }
        return m_pVSInfo;
    }

//////////////////////////////////////////////////////////////////////////
#ifdef  _DEBUG
        BOOL SetEdgeRoadSectionID(unsigned long nID)
        {
            ASSERT(GetEdge());
            ASSERT(!m_pEdgeInfo);
            m_pEdgeInfo = new ST_GEO_VEDGE;
            ASSERT(m_pEdgeInfo);
            m_pEdgeInfo->nRoadSectionID = nID;

            return TRUE;
        }
#endif

//////////////////////////////////////////////////////////////////////////
//���ݳ�Ա
public:
    StuNetworkFileStructV5::stuVertex   m_stData;

	unsigned long	*m_pRoadSectionList;				//�����ӵ�·��ID
	unsigned short	*m_pRoadSectionTurnList;            //��ˮƽ����ļн�����
	unsigned short	*m_pRoadSectionLayerIDLList;		//·������ͼ��ID

	ST_GEO_VEDGE	*m_pEdgeInfo;		                //��Ե�ڵ���Ϣ
	ST_GEO_VSORT	*m_pVSInfo;		                    //�ڵ����ʱ����Ϣ

//////////////////////////////////////////////////////////////////////////
// this code for check self
#ifdef _DEBUG
public:
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
    static IMemoryMng2* m_pMemMngOfSortInfo;
    static IMemoryMng2* m_pMemMngOfEdgeInfo;
#endif	
//////////////////////////////////////////////////////////////////////////
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//���ñ䳤���ݻ�ַ
inline void CGIS_NetworkVertexV5::SetVarietyBaseAddr(
       char*& pCharAddr,
       char*& pShortAddr,
       char*& pDwordAddr)
{
	if(m_stData.m_nRoadSectionNum)
	{

		m_pRoadSectionTurnList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_stData.m_nRoadSectionNum*sizeof(unsigned short);

		m_pRoadSectionLayerIDLList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_stData.m_nRoadSectionNum*sizeof(unsigned short);

		m_pRoadSectionList	= (unsigned long*)(pDwordAddr);
		pDwordAddr += m_stData.m_nRoadSectionNum*sizeof(unsigned long);
	}

	if(GetEdge())
	{
		m_pEdgeInfo = (ST_GEO_VEDGE*)pDwordAddr;
		pDwordAddr += sizeof(ST_GEO_VEDGE);
	}
}

#endif // !defined(AFX_GIS_NETWORKVERTEXV5_H__A504F1A1_3E6A_4A4E_8A5D_CC9AC0A7DE64__INCLUDED_)
