
#ifndef _GIS_PATHPOINT_NODE_H
#define _GIS_PATHPOINT_NODE_H

#include "swBase.h"

#include "GIS_Pathpoint.h"
#include "GIS_NetworkNodeRut.h"

class swEXPORT CGIS_PathpointNode : public CGIS_Pathpoint  
{
public:
    enum enCrossStatus
    {
        STATUS_NONE,
        STATUS_IMAGE,
        STATUS_GDI_DRAW,
    };
public:
	CGIS_PathpointNode();
	virtual ~CGIS_PathpointNode();

	CPtrList* GetNodeRutList( );
	void SetNextRutID( int nNextRutID );
	int GetNextRutID( );
	void SetRutAnno( char *szAnno );
	swString GetRutAnno( );
	BYTE GetRutAnnoNum( );
	void SetRutAnnoNum( BYTE nAnnoNum );
	void SetNodeVtxID( int nNodeVtxID );
	int GetNodeVtxID( );
	ST_VOICE_RUT* GetVRutInfo( );
	ST_VOICE_NODE* GetVNodeInfo( );
	void SetTurnAngle( int nTurnAngle );
	int  GetTurnAngle( );
	void SetRType( BYTE byRType );
	BYTE GetRType( );
	void SetRutType( BYTE byRutType );
	BYTE GetRutType( );
	void SetPBreadthID( int nBID );
	int GetPBreadthID( );
	void CreateCRect( ST_GEO_PTXY pPtList[], int nPtNum=1,float dExtendX = 0.00004,float dExtendY = 0.00004);
	void SetCRect( ST_GEO_RECT rtCross );
	ST_GEO_RECT GetCRect( );
	void SetRMOrder( short nRMOrd );
	short GetRMOrder( );


    void SetCrossStatus(enCrossStatus eStatus)
    {
        m_eCrossStatus = eStatus;
    }
    void SetCrossImgID(int nImgID)
    {
        m_nImgID = nImgID;
    }
    void SetCrossArrowID(int nArrowID)
    {
        m_nArrowID = nArrowID;
    }
    enCrossStatus GetCrossStatus()
    {
        return m_eCrossStatus;
    }
    int GetCrossImgID()
    {
        return m_nImgID;
    }
    int GetCrossArrowID()
    {
        return m_nArrowID;
    }
    //////////////////////////////////////////////////////////////////////////
    
    CGIS_FeatureRut::SIGNINFO* GetSignInfo()
    {
        return m_pSignInfo;
    }
     BOOL SetSignInfo(CGIS_FeatureRut::SIGNINFO* pInfo)
    {
        if(!pInfo)
            return FALSE;
        ASSERT(!m_pSignInfo);

        m_pSignInfo = new CGIS_FeatureRut::SIGNINFO;
        m_pSignInfo->wOutRutID = pInfo->wOutRutID;
        m_pSignInfo->byNameLen = pInfo->byNameLen;
        m_pSignInfo->pszName = new char[pInfo->byNameLen];
        memcpy(m_pSignInfo->pszName,pInfo->pszName,pInfo->byNameLen);
        
        return TRUE;
    }
     
     //////////////////////////////////////////////////////////////////////////
     
     BOOL SetLaneInfoDirect          (ST_LANDINFO* pLaneInfo);
     ST_LANDINFO* GetLaneInfo(){return m_pstLaneInfo;}

private:

	int m_nNextRutID;	                    //下个路段的ID
	int m_nNodeVtxID;	                    //当前节点的ID
	int				m_nTurnAngle;			//路段的夹角

	char			*m_szRutAnno;			//下个路段的名称
	CPtrList		*m_pNodeRutList;		//该节点上其余路段信息链表
	ST_VOICE_RUT	*m_pVRut;				//语音节点下一路段信息
	ST_VOICE_NODE	*m_pVNode;				//语音节点的信息

	unsigned char   m_nAnnoNum;			    //下个路段名称的长度
	unsigned char	m_byRType;				//下一路段等级
                                            //0--高速
                                            //1--都市高速路，
                                            //2--国道
                                            //3--主要地方道&轮渡
											//4--一般县道
                                            //5--市镇村道
                                            //6--其他道路)

	unsigned char	m_byRutType;			//道路类型:
                                            //0--环岛1--未采集属性2--上下线分离3--两条高速路之间的道路
											//4--交叉路口内的道路5--连接高速路和普通道路的道路6--停车区
											//7--服务区15--隧道
	unsigned char	unUse;
	short			unUse2;
	short			m_nRMOrd;
	int				m_nBID;					//节点所在图幅ID（编号后）
	ST_GEO_RECT		m_rtCross;				//路口的范围

    enCrossStatus            m_eCrossStatus;          //路口扩大图状态 0-没有 1-虚拟图 2-数据加载绘制
	int		        m_nImgID;
	int		        m_nArrowID;
    
    CGIS_FeatureRut::SIGNINFO* m_pSignInfo;

    //此数据是前一路段的车道信息数,并不是后一路段.
    ST_LANDINFO* m_pstLaneInfo;
    BYTE	m_byLaneNum;
	BYTE*  m_pbyLaneDir;


#ifdef _DEBUG
public:
	virtual void DumpState() const;		
	virtual void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////

};

#endif 
