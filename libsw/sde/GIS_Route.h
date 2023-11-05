
#ifndef _GIS_ROUTE_H
#define _GIS_ROUTE_H

#include "swBase.h"

///*
//������Ϣ�ṹ
typedef struct ST_GIS_ROUTEINFO
{
	//pathinfo
	int		enCalMethod;//���㷽��
	int		enRoutePref;//ѡ·��ʽ
	int		enVehicleType;//��������
	
	CArray<ST_GEO_PTXY,ST_GEO_PTXY&>		arPtWPPos;//;�����λ������
	CArray<int,int&>	arEnWPType;//;�������������
	CArray<char*,char*>						arSzAnno;//;�����ע����

	CPtrList	*	 pPathPointList;

public:
	void	Serialize( CArchive & ar );

}tgROUTEINFO;

typedef struct ST_GIS_ROUTEHEAD 
{
	CString		strRouteName;//��������
	CString		strFileName;//���溽�ߵ��ļ���
}tgROUTEHEAD;

typedef CArray<tgROUTEHEAD,tgROUTEHEAD&> ARROUTEHEAD;
//*/

#include "GIS_NetworkRoute.h"

class swEXPORT CGIS_Route 
{
public:
	CGIS_Route( const CString & strRouteName = "", const UINT & nID = 0 );
	virtual ~CGIS_Route();

public:
	void	SerializeRInfo( CArchive & ar );
	BOOL	DeleteRoute(const CString & strRouteFile);//ɾ��һ������
	CString GetRouteName() const;
	void	SetRouteName( const CString & strRouteName  );//���ú�������
	BOOL	GetRouteInfo( const CString & strRouteFile, tgROUTEINFO * pRouteInfo );//��ú�����Ϣ
	BOOL	SaveRoute();//���溽�ߡ�������д���ļ���
	void	SetNetWorkRoute( CGIS_NetworkRoute * pNWRoute );

private:
	CGIS_NetworkRoute	*	m_pNWRoute;//����

	CString		m_strRouteName;//��������
	UINT		m_nRouteID;//����ID

	CString		m_strRouteFile;//�����ļ�

private:
	void	SerializeRoute( CArchive & ar );
};

#endif
