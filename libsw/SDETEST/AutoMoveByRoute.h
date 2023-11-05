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

	// 初始化模拟导航
	BOOL InitSimulNav();

	// 获得模拟定位信息
	BOOL GetSimulInfo(double &dbLoogitude, double &dbLatitude, int &nAngle);

	// 设置一条航线
	void SetRoute(CGIS_FeatureRoute *pRoute);
	
protected:
	// 移动到下一个点
	void MoveNext();

private:

	CGIS_FeatureRoute *m_pFeaRoute;	// 航线信息

	int m_nRoadtype; // 当前路段等级，0-高速，1-主要，2-次要
	double	m_dbSpeed[3];	// 模拟导航速度 单位m/s

	double	m_dbAngle;	// 方位角
	double	m_dbLat;	// 当前纬度
	double	m_dbLoog;	// 当前经度

	POSITION m_posList;	// 列表位置
	CPtrList *m_pList;	// 途经点列表

	BOOL m_bIsTail; // 导航到达最后
};

#endif // !defined(AFX_AUTOMOVEBYROUTE_H__CE317C81_FE0E_49B4_91A7_8331733E90EA__INCLUDED_)

///
