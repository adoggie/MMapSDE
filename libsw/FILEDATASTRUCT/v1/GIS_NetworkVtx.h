// GIS_NetworkVtx.h: interface for the CGIS_NetworkVtx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIS_NETWORKVTX_H__C3263779_765B_4919_8B95_34D8883D4592__INCLUDED_)
#define AFX_GIS_NETWORKVTX_H__C3263779_765B_4919_8B95_34D8883D4592__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoGlobe.h"

class CGIS_NetworkVtx  
{
public:
	CGIS_NetworkVtx( );
	virtual ~CGIS_NetworkVtx();

public:
	void SetVtxInfo( unsigned short nVID, ST_GEO_PTXY ptPostion, int nVBID = 0 );
	void SetVRutInfo( BYTE byRNum = 0, unsigned short *pVRList = NULL, unsigned short *pRTList = NULL, BYTE *pRLList = NULL );
	void SetVEInfo( BYTE byEdge = 0, int nABID = 0, unsigned short nARID = 0 );

	unsigned short GetVtxID( );
	ST_GEO_PTXY GetVtxPosition( );
	BYTE GetVRutNum( );
	unsigned short* GetVRutList( );
	unsigned short* GetVRTurnList( );
	BYTE* GetVRLList( );

	BYTE GetVtxEdge( );
	int GetVAbutBID( );
	unsigned short GetVAbutRID( );

	int GetVBreadthID( );

	ST_GEO_VSORT* GetVtxSInfo( );
//	POSITION m_posOHead;
//	BYTE m_bySHead;				//Vertex status:0 1 2 ,
//											//   0 ���� δ�������,
//											//   1 OPEN,
//											//   2 CLOSE
//	POSITION m_posOEnd;
//	BYTE m_bySEnd;				//Vertex status:0 1 2 ,
//											//   0 ���� δ�������,
//											//   1 OPEN,
//											//   2 CLOSE
private:
	unsigned short	m_nVtxID;		//�ڵ�ID
	ST_GEO_PTXY		m_ptPosition;	//�ڵ�����
	ST_GEO_VEDGE	*m_pVEInfo;		//��Ե�ڵ���Ϣ
//	BYTE			m_byEdge;		//�Ƿ���ͼ����Ե:0-����,1-��
//	int				m_nABID;		//�ڽ�ͼ����
//	unsigned short	m_nARID;		//����Rut_id
	BYTE			m_byRNum;		//�����ӵ�·����
	unsigned short	*m_pVRList;		//�����ӵ�·��ID
	unsigned short	*m_pRTList;		//��ˮƽ����ļн�����
	BYTE			*m_pRLList;		//·������ͼ��ID
	int				m_nVBID;		//�ڵ�����ͼ��ID
	ST_GEO_VSORT	*m_pVSInfo;		//�ڵ����ʱ����Ϣ
};

#endif // !defined(AFX_GIS_NETWORKVTX_H__C3263779_765B_4919_8B95_34D8883D4592__INCLUDED_)
