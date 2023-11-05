// GIS_NetworkRut.h: interface for the CGIS_NetworkRut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_NETWORKRUT_H__59B77223_6C99_4281_8C53_D3654CD62287__INCLUDED_)
#define AFX_GIS_NETWORKRUT_H__59B77223_6C99_4281_8C53_D3654CD62287__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GeoGlobe.h"
#include "GIS_NetworkVtx.h"

class CGIS_NetworkRut  
{
public:
	CGIS_NetworkRut();
	virtual ~CGIS_NetworkRut();

	void SetRutInfo( unsigned short nRID = 0, BYTE byDirtect = 0, BYTE byType = 6, BYTE byCharge = 0,
			BYTE byLNum = 0,	BYTE bySpeed = 6, BYTE byRLevel = 0, BYTE byRType = 1,
			float dRLength = 0.0, unsigned short nVStart = 0, unsigned short nVEnd = 0, int nRBID = 0);
	void SetRutLimitInfo( BYTE byLimit = 0, unsigned short *pLRList = NULL );
	void SetRutRLimitInfo( BYTE byLimit = 0, unsigned short *pLRList = NULL );
	void SetRutVirtualInfo( BYTE byVirtual = 0, unsigned short *pVORList = NULL, int *pVIDs = NULL );
	int GetRutID( );
	BYTE GetRutDirection( );
	BYTE GetType( );
	BYTE GetRutCharge( );
	BYTE GetRutLaneNum();
	BYTE GetRutSpeed( );
	BYTE GetRutType( );
	BYTE GetRutLevel( );
	float GetRutLength( );
	unsigned short GetRutStartVtxID( );
	unsigned short GetRutEndVtxID( );
	int GetRBreadthID( );

	BYTE GetLimitRNum();
	unsigned short* GetLimitRList( );
	BYTE GetRLimitRNum();
	unsigned short* GetRLimitRList( );
private:
	unsigned short	m_nRutID;	//·��ID
	BYTE	m_byDirection;		//·�ε�˫�б�־
								//(0��ʾ˫�У�1��ʾend��start,2��ʾstart��end,3��ʾ˫�����)
	BYTE	m_byType;			//��·���:(0--���٣�1--���и���·��2--������3--��Ҫ�ط���&�ֶ�
									//4--һ���ص���5--��������6--������·��
	BYTE	m_byCharge;			//�շ�:0--���շ�1--˫���շ�2--�����շ�3--�����շ�
//	BYTE	m_byLaneNum;			//·�γ�����:�����ݵĸ��ݵ�·�����ȱʡֵ
//	BYTE	m_bySpeed;			//�ٶȼ���:���ݵ�·�����ȱʡ�ٶ�
	BYTE	m_byRutLevel;		//��·���ܼ���:5--һ����·4--������·3--������·
									//2--�ļ���·1--�弶��·0--������·
	BYTE	m_byRutType;		//��·����:0--����1--δ�ɼ�����2--�����߷���3--��������·֮��ĵ�·
									//4--����·���ڵĵ�·5--���Ӹ���·����ͨ��·�ĵ�·6--ͣ����
									//7--������15--���
	float	m_dRLength;			//·�γ���
	unsigned short m_nVtxStartID;//·����ʼ�ڵ�ID
	unsigned short m_nVtxEndID;	//·��β�ڵ�ID
	int		m_nRBID;		//·������ͼ��ID
	
	ST_GEO_RLIMIT *m_pRLInfo;	//��·�Ľ�����Ϣ
//	BYTE	m_byLimit;			//�Ƿ���ڽ�ͨ����:0-û�н�ͨ����;��0- �н�ͨ����
//	unsigned short *m_pLRList;	//��ֹͨ�еĵ�·id
//	BYTE	m_byRLimit;			//�Ƿ�����ڽ�ͨ����:0-û�н�ͨ����;��0- �н�ͨ����
//	unsigned short *m_pRLRList;	//�����ֹͨ�еĵ�·id
//	ST_GEO_RVDRAFT *m_pRVInfo;	//��·����ͼ��Ϣ
//	BYTE	m_byVirtual;		//�Ƿ��������ͼ:0- û������ͼ;��0- ������ͼ
//	unsigned short *m_pVORList;	//��ȥ�ĵ�·ID����
//	int		*m_pVIDs;			//����ͼ�������
};

#endif // !defined(AFX_GIS_NETWORKRUT_H__59B77223_6C99_4281_8C53_D3654CD62287__INCLUDED_)
