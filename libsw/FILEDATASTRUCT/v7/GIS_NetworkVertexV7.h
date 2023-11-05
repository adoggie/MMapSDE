// GIS_NetworkVertexV7.h: interface for the CGIS_NetworkVertexV7 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_NETWORKVERTEXV7_H__A504F1A1_3E6A_4A4E_8A5D_CC9AC0A7DE64__INCLUDED_)
#define AFX_GIS_NETWORKVERTEXV7_H__A504F1A1_3E6A_4A4E_8A5D_CC9AC0A7DE64__INCLUDED_

#include "NetworkFileStructV7.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




//����ΪҪд��Ľڵ����Ľṹ����
//////////////////////////////////////////////////////////////////////////
class CGIS_NetworkVertexV7  
{
public:
	CGIS_NetworkVertexV7();
	~CGIS_NetworkVertexV7();
    
    friend class CGIS_NetworkVtx;    
    CGIS_NetworkVertexV7* CreateCloneObject();
    void DestoryCloneObject();

    StuNetworkFileStructV7::stuVertex* GetDataAddr(){return &m_stData;};
	void SetVarietyBaseAddr(char*& pCharAddr,char*& pShortAddr,char*& pDwordAddr);
	void MakeBuddyIndependent();
//////////////////////////////////////////////////////////////////////////
///�ڲ���������ݽṹ
public:
	//��Ե�ڵ���Ϣ�ṹ
	struct ST_GEO_VEDGE
	{
        unsigned short nBreadthID;
		unsigned short nRoadSectionID;		//������Rut_id	
	};

	//Search��Ϣ�ṹ
	struct ST_GEO_VSORT
	{
		BYTE bySHead;	//Vertex status:0 1 2 
		BYTE bySEnd;	//Vertex status:0 1 2 
        /*
        0-����δ�������
        1-��Ȩֵ�������������뵽OPEN����ȥ.
        2-��Ȩֵ�������������뵽Close����ȥ.
        */
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
    unsigned short*  GetRoadSectionList(){return m_pRoadSectionList;};
    unsigned short* GetRoadSectionTurnList(){return m_pRoadSectionTurnList;};
    
    unsigned char	GetLowLayerRoadSectionNum(){return m_stData.m_nLowLayerRoadSectionNum;};
    unsigned short* GetLowLayerRoadSectionTurnList(){return m_pLowLayerRoadSectionTurnList;};

    unsigned char	GetEdge(){return m_stData.m_nEdge&0x01;};
    unsigned short	GetEdgeRoadSectionID()
    {
        if(m_pEdgeInfo) 
            return m_pEdgeInfo->nRoadSectionID;
        else 
            return 0xffff;
    };

    unsigned short	GetEdgeBreadthID()
    {
        if(m_pEdgeInfo) 
            return m_pEdgeInfo->nBreadthID;
        else 
            return 0xffff;
    }
    ST_GEO_VSORT* GetVtxSInfo();
//////////////////////////////////////////////////////////////////////////
#ifdef  _DEBUG
        BOOL SetEdgeInfo(unsigned short nBreadthID, unsigned short nRoadSectionID)
        {
            ASSERT(GetEdge());
            ASSERT(!m_pEdgeInfo);
            m_pEdgeInfo = new ST_GEO_VEDGE;
            ASSERT(m_pEdgeInfo);
            m_pEdgeInfo->nBreadthID     = nBreadthID;
            m_pEdgeInfo->nRoadSectionID = nRoadSectionID;

            return TRUE;
        }
#endif

//////////////////////////////////////////////////////////////////////////
//���ݳ�Ա
public:
    StuNetworkFileStructV7::stuVertex   m_stData;

	unsigned short	*m_pRoadSectionList;				        //�����ӵ�·��ID
	unsigned short	*m_pRoadSectionTurnList;                    //��ˮƽ����ļн�����
	unsigned short	*m_pLowLayerRoadSectionTurnList;            //��ˮƽ����ĵͼ�·�μн�����

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

};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//���ñ䳤���ݻ�ַ
inline void CGIS_NetworkVertexV7::SetVarietyBaseAddr(
       char*& pCharAddr,
       char*& pShortAddr,
       char*& pDwordAddr)
{
	if(m_stData.m_nRoadSectionNum)
	{

		m_pLowLayerRoadSectionTurnList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_stData.m_nLowLayerRoadSectionNum*sizeof(unsigned short);

		m_pRoadSectionTurnList	= (unsigned short*)(pShortAddr);
		pShortAddr += m_stData.m_nRoadSectionNum*sizeof(unsigned short);

		m_pRoadSectionList	    = (unsigned short*)(pShortAddr);
		pShortAddr += m_stData.m_nRoadSectionNum*sizeof(unsigned short);
	}

	if(GetEdge())
	{
		m_pEdgeInfo = (ST_GEO_VEDGE*)pDwordAddr;
		pDwordAddr += sizeof(ST_GEO_VEDGE);
	}
}

#endif // !defined(AFX_GIS_NETWORKVERTEXV7_H__A504F1A1_3E6A_4A4E_8A5D_CC9AC0A7DE64__INCLUDED_)
