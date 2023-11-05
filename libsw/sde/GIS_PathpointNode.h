
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

	int m_nNextRutID;	                    //�¸�·�ε�ID
	int m_nNodeVtxID;	                    //��ǰ�ڵ��ID
	int				m_nTurnAngle;			//·�εļн�

	char			*m_szRutAnno;			//�¸�·�ε�����
	CPtrList		*m_pNodeRutList;		//�ýڵ�������·����Ϣ����
	ST_VOICE_RUT	*m_pVRut;				//�����ڵ���һ·����Ϣ
	ST_VOICE_NODE	*m_pVNode;				//�����ڵ����Ϣ

	unsigned char   m_nAnnoNum;			    //�¸�·�����Ƶĳ���
	unsigned char	m_byRType;				//��һ·�εȼ�
                                            //0--����
                                            //1--���и���·��
                                            //2--����
                                            //3--��Ҫ�ط���&�ֶ�
											//4--һ���ص�
                                            //5--������
                                            //6--������·)

	unsigned char	m_byRutType;			//��·����:
                                            //0--����1--δ�ɼ�����2--�����߷���3--��������·֮��ĵ�·
											//4--����·���ڵĵ�·5--���Ӹ���·����ͨ��·�ĵ�·6--ͣ����
											//7--������15--���
	unsigned char	unUse;
	short			unUse2;
	short			m_nRMOrd;
	int				m_nBID;					//�ڵ�����ͼ��ID����ź�
	ST_GEO_RECT		m_rtCross;				//·�ڵķ�Χ

    enCrossStatus            m_eCrossStatus;          //·������ͼ״̬ 0-û�� 1-����ͼ 2-���ݼ��ػ���
	int		        m_nImgID;
	int		        m_nArrowID;
    
    CGIS_FeatureRut::SIGNINFO* m_pSignInfo;

    //��������ǰһ·�εĳ�����Ϣ��,�����Ǻ�һ·��.
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
