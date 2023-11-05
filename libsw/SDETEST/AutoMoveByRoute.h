//////////////////////////////////////////////////////////////////////////
// AutoMoveByRoute.h

#if !defined(AFX_AUTOMOVEBYROUTE_H__CE317C81_FE0E_49B4_91A7_8331733E90EA__INCLUDED_)
#define AFX_AUTOMOVEBYROUTE_H__CE317C81_FE0E_49B4_91A7_8331733E90EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGIS_FeatureRoute;

class CAutoMoveByRoute
{
public:
	CAutoMoveByRoute();
	virtual ~CAutoMoveByRoute();

	// ��ʼ��ģ�⵼��
	BOOL InitSimulNav();

	// ���ģ�ⶨλ��Ϣ
	BOOL GetSimulInfo(double &dbLoogitude, double &dbLatitude, int &nAngle);

	// ����һ������
	void SetRoute(CGIS_FeatureRoute *pRoute);
	
protected:
	// �ƶ�����һ����
	void MoveNext();

private:

	CGIS_FeatureRoute *m_pFeaRoute;	// ������Ϣ

	int m_nRoadtype; // ��ǰ·�εȼ���0-���٣�1-��Ҫ��2-��Ҫ
	double	m_dbSpeed[3];	// ģ�⵼���ٶ� ��λm/s

	double	m_dbAngle;	// ��λ��
	double	m_dbLat;	// ��ǰγ��
	double	m_dbLoog;	// ��ǰ����

	POSITION m_posList;	// �б�λ��
	CPtrList *m_pList;	// ;�����б�

	BOOL m_bIsTail; // �����������
};

#endif // !defined(AFX_AUTOMOVEBYROUTE_H__CE317C81_FE0E_49B4_91A7_8331733E90EA__INCLUDED_)

///
