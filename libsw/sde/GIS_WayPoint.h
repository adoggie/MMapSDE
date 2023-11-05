
#ifndef _GIS_WAYPOINT_H
#define _GIS_WAYPOINT_H

#include "swBase.h"
#include "GIS_NetworkVtx.h"

/**
;����
*/
class swEXPORT  CGIS_WayPoint  
{
public:
	CGIS_WayPoint(UINT nID);
	virtual ~CGIS_WayPoint();
	UINT GetWPID( );
	ST_GEO_PTXY GetWPPosition( );
	void SetWPPosition( ST_GEO_PTXY ptWaypointPos );
	
	//����/ȡ��ͶӰλ��
	ST_GEO_PTXY GetWPProjPosition( EnMapType enMapType);
	void SetWPProjPosition(ST_GEO_PTXY ptProjectPos, EnMapType enMapType);
	
	//����/ȡ�� ��������;�����������Ч�ڵ�ID
	int GetWPVtxID( EnMapType enMapType );
	void SetWPVtxID( int nWPVtxID, EnMapType enMapType );

	//����/ȡ�� ��������;��������ĵ�·ID
  int GetWPRutID( EnMapType enMapType);
	void SetWPRutID( int nWPRutID, EnMapType enMapType );
	
	//����/ȡ�� ��;���������
	EnWaypointType GetWPType();
	void SetWPType(EnWaypointType enWPType);

	//����/ȡ�� �����������·�ĵ㼯
	void SetWPRutPtList( ST_GEO_PTXY *pPtList, unsigned short nPtNum, unsigned short nPPos, EnMapType enMapType );
	ST_GEO_PTXY* GetWPRutPtList( EnMapType enMapType);
	unsigned short GetWPRutPtNum( EnMapType enMapType );
	unsigned short GetWPProjPos( EnMapType enMapType);
	//���Ŀ����·��
	void ClearWPRutInfo( );

	//��ע�ͱ�ע����
	BYTE GetWPAnnoNum( );
	void SetWPAnnoNum( BYTE nAnnoNum );
	char* GetWPAnnoation( );
	void SetWPAnnoation( char* szAnno );

	//����/ȡ�õ�ͼID
	unsigned short GetWPMapID( );
	void SetWPMapID( unsigned short nMapID, EnMapType enMapType );

	//����/ȡ�� ��������;��������ĵ�·����·��ID
	int GetWPRutNID( EnMapType enMapType );
	void SetWPRutNID( int nWPRutNID, EnMapType enMapType );
	
	///����/ȡ�� ;��������·����
	void SetWPRutName( swString strWPRName );
	swString GetWPRutName();

	//����/ȡ�� ��������;��������ĵ�·�ĵȼ�
	BYTE GetWPRutLevel( EnMapType enMapType );
	void SetWPRutLevel( BYTE nLevel, EnMapType enMapType );

	
private:
	UINT	m_nID;
	ST_GEO_PTXY m_ptWPPos;				//;�����λ��
	int m_nWPCityVtxID;					//��������;�����������Ч�ڵ�
	int m_nWPNationVtxID;				//ȫ������;�����������Ч�ڵ�
	int m_nWPCityRutID;					//��������;��������ĵ�·ID
	int m_nWPCityRutNID;				//��������;��������ĵ�·����·��ID
	int m_nWPNationRutID;				//ȫ������;��������ĵ�·
	int m_nWPNationRutNID;				//��������;��������ĵ�·����·��ID
	
	unsigned short	m_nCPPos;			//������ͶӰ��λ�����ĸ��յ��Ա�
	unsigned short	m_nNPPos;			//ȫ����ͶӰ��λ�����ĸ��յ��Ա�
	unsigned short	m_nCRPtNum;			//�����������·�ĵ����
	unsigned short	m_nNRPtNum;			//ȫ���������·�ĵ����
	ST_GEO_PTXY		*m_pCRPtList;		//�����������·�ĵ㼯
	ST_GEO_PTXY		*m_pNRPtList;		//ȫ���������·�ĵ㼯
	ST_GEO_PTXY m_ptCityWPProjPos;		//������;������·���ϵ�ͶӰ��λ��
	ST_GEO_PTXY m_ptNationWPProjPos;	//ȫ����;������·���ϵ�ͶӰ��λ��

    BYTE m_nWPCityRutLevel;              //��������;��������ĵ�·�ĵȼ�
	BYTE m_nWPNationRutLevel;;           //ȫ������;��������ĵ�·�ĵȼ�
	unsigned short m_nMapID;
	swString m_strWPRName;				//;��������·����
	EnWaypointType m_enWPType;			//;���������
	char *m_szAnno;						//;�����ע
	BYTE m_nAnnoNum;					//��ע����

#ifdef _DEBUG
public:
	void DumpState() const;		
	void AssertValid() const;	
#endif

};

#endif
