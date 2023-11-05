
#ifndef _GIS_NETWORK_PATH_H
#define _GIS_NETWORK_PATH_H

#include "swBase.h"

class swEXPORT CGIS_NetworkPath  
{
public:
	CGIS_NetworkPath();
	virtual ~CGIS_NetworkPath();

public:
	///ȡ��·�����б�
	CPtrList* GetPathpointList( );
	///��ʼ������
	BOOL InitNavigate( );
	///���������Ϣ�б�
	BOOL CalcuatePathTurnList( int* pErrorCode );

	// ׼��������Ϣ
	BOOL GetNavigateVoice( ST_GEO_PTXY pt,	// ����λ��
							BYTE byType = 1 );	// ���ص��������ͣ�Ŀǰδʹ�ã�
	// ��õ�����Ϣ
	BOOL GetNavigateVoiceInfo( BYTE &byMType,			//�������ͣ�0.���������1.����Ĳ����ǵڼ������ڣ�2.�����������Ϣ��
								swString &strCRName,		//��ǰ·��
								swString &strNRName,		//��һ·��
								double &dbDisNav,		//����������
								double &dbAngleNav,		//������㷽��
								double &dbDisDes,		//���յ����
								double &dbAngleDes,		//���յ㷽��
								swString &strVLong,		//Զ����������Ϣ
								swString &strVShort,		//������������Ϣ
								swString &strVInter,		// ·������
								swString &strRName,		//�¸�·��
								BYTE &byRMNum );		//����·�ڵĸ���

	CPtrList* GetNavigateVTRInfo( ST_GEO_RECT &rtBuff );

	///�õ��������������е�λ��
	void GetVoicePosition( POSITION &posCurr, POSITION &posVoice );
	CPtrList* GetPathTurnList( );
	BOOL IsInMobile( );
	void SetCTFlag( BOOL bCTFlag );
	void GetTLPosition( POSITION &posTurn );

	// ȡ�õ�ǰ�㵽�¸��ڵ�ľ��룬�������λ�ڽڵ��Ͼ���Ϊ0
	// ʹ��ǰȷ�ϵ��ù�����GetNavigateVoice
	double GetDisToNextNode();
	
private:
	//����TurnList����ڵ���������
	BOOL CheckTLSeriateMobile( CPtrList *pPPList, CPtrList *pTList );
	// ���ݻ�����Ϣ�õ�����������Ϣ
	swString RecombineTInfo(int nOrd, ST_TURN_INFO *pTInfo, int ntype);
	///������ʹ��·������
	BOOL CalculateCurrentPosition( ST_GEO_PTXY pt, CPtrList *pPathPointList, POSITION posVoice, POSITION &posCurrent, BOOL &bVoice );
	//�������·������ǰ����һ��
	BOOL CalculateRutNames( CPtrList *pPathPointList, POSITION posCurrent, swString &strCRName, swString &strNRName );
	///������һ����Ҫ����������λ��
	BOOL CalculateVoicePosition2( CPtrList *pPathPointList, POSITION posCurrent, ST_TURN_INFO **pTInfo );
	BOOL CalculateVoicePosition3( CPtrList *pPPList, CPtrList *pTList, POSITION posCurrent, POSITION &posTurn, POSITION &posVoice );

	BOOL CalculateVTRList( CPtrList *pPPList, ST_TURN_INFO *pTInfo );

	CPtrList *m_pPathpointList;		///·���ϵ���б�
	ST_VOICE_NODE *m_pVoiceNode;			//��ǰ�ڵ���Ϣ
	ST_VOICE_RUT  *m_pVoiceEnterRut;		//�ڵ�ǰ�ε�·��Ϣ
	ST_VOICE_RUT  *m_pVoiceExitRut;			//�ڵ��ε�·��Ϣ

	POSITION m_posCurrent;		///��ǰ�������е�λ��
	POSITION m_posVoice;		///��һ�������������е�λ��
	POSITION m_posVEnd;			//��һ�������������н�����λ��
	POSITION m_posTurn;			///��¼�����б�λ��
	POSITION m_posVEPre;			//��¼ǰ����������λ��
	BYTE	m_byMType;			//���������ͣ�0.�������ͣ�1.����Ĳ����ǵڼ������ڣ�2.�����������Ϣ��
	BYTE	m_byRMNum;			//����·�ڵĸ���
	BYTE	m_byAFlag;			// Ŀ�ĵ��Ƿ��ڻ����ڵı�־��1���ڣ�0�����ڣ�

	swString m_strLVoice;	// ��������ʾ������
	swString m_strSVoice;	// �̾�����ʾ������
	swString m_strIVoice;	// ·����ʾ������
	swString m_strInfo;		//��Ҫ��ʾ��·��
	BOOL m_bSRut;			//����Ƿ�ͬһ·��
	double m_dbDis;			//��¼����
	CPtrList *m_pTurnList;		//ת�������Ϣ�б�
	BOOL	m_bInMobile;	//��ʾ�Ƿ������������򻷵�����
	BYTE	m_byInMType;	//1����ʾ���������� 2����ʾ�ڻ������� 3����ʾ�ڻ���

	swString m_strCRName;	//��ǰ·��
	swString m_strNRName;	//��һ·��
	double m_dbDisNav;		//����������
	double m_dbAngleNav;	//������㷽��
	double m_dbDisDes;		//���յ����
	double m_dbAngleDes;	//���յ㷽��
	swString m_strVLong;		// Զ����������Ϣ
	swString m_strVShort;	// ������������Ϣ
	swString m_strVInter;	// ·��������Ϣ
	swString m_strRName;		//�¸�·��

	CPtrList* m_pTRList;		//����·�ε���Ϣ
	ST_GEO_RECT m_rtTRBuf;		//��ѯ��Χ
	BOOL m_bCTFlag;				//�Ƿ���㺽�߻�����Ϣ�ı�־
	double m_dbDis2Dest;		//�����㵽�յ�ľ��룻

	double m_dbDisToNode; // ��ǰ�㵽�¸����߽ڵ�ľ���
//////////////////////////////////////////////////////////////////////////
// lijun add this code for debug
#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif
//////////////////////////////////////////////////////////////////////////	


    //////////////////////////////////////////////////////////////////////////
    //lijun 070723
protected:
    CPtrList* m_pNodeList;//NODE����,�����г�������NODE,��node����������Ϣ.
public:
    CPtrList* GetNodeList()
    {
        if(!m_pNodeList)
        {
            m_pNodeList = new CPtrList;
        }

        return m_pNodeList;
    }
    //////////////////////////////////////////////////////////////////////////

public:
	bool GetbTipReCal(); // �Ƿ���Ϊʱ��Ƭ���ж����¼��㺽��

private:
	// ����ʱ��Ƭ����
	BOOL TimeSliceLimit(CPtrList *pList, double dbDisToNode, POSITION posCurNode);

	// ����NODE���б�ĵ�ǰλ�ã����ص���ǰ�ڵ�ľ��룬ʧ�ܷ���-1
	double CalCurPosInNodeList(POSITION posPathpoint);
	
	// ��ʽ������·����Ϣ
	void FormatVoiceRoadName(ST_TURN_INFO *pInfo, int nOrd, swString &strVoice);

	POSITION m_posLimit[20]; // ��¼��ʾ����λ��
	bool m_bTipCal; // ��ʾ���¼���
	double m_dbDisRoute; // ��ǰ���ߵĳ���
	POSITION m_posNodeCur; // ��¼NODE���б�ĵ�ǰλ��
};

#endif
