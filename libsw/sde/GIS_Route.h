
#ifndef _GIS_ROUTE_H
#define _GIS_ROUTE_H

#include "swBase.h"

///*
//航线信息结构
typedef struct ST_GIS_ROUTEINFO
{
	//pathinfo
	int		enCalMethod;//计算方法
	int		enRoutePref;//选路方式
	int		enVehicleType;//车辆类型
	
	CArray<ST_GEO_PTXY,ST_GEO_PTXY&>		arPtWPPos;//途经点的位置数组
	CArray<int,int&>	arEnWPType;//途经点的类型数组
	CArray<char*,char*>						arSzAnno;//途经点标注数组

	CPtrList	*	 pPathPointList;

public:
	void	Serialize( CArchive & ar );

}tgROUTEINFO;

typedef struct ST_GIS_ROUTEHEAD 
{
	CString		strRouteName;//航线名称
	CString		strFileName;//保存航线的文件名
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
	BOOL	DeleteRoute(const CString & strRouteFile);//删除一条航线
	CString GetRouteName() const;
	void	SetRouteName( const CString & strRouteName  );//设置航线名称
	BOOL	GetRouteInfo( const CString & strRouteFile, tgROUTEINFO * pRouteInfo );//获得航线信息
	BOOL	SaveRoute();//保存航线。将航线写到文件中
	void	SetNetWorkRoute( CGIS_NetworkRoute * pNWRoute );

private:
	CGIS_NetworkRoute	*	m_pNWRoute;//航线

	CString		m_strRouteName;//航线名称
	UINT		m_nRouteID;//航线ID

	CString		m_strRouteFile;//航线文件

private:
	void	SerializeRoute( CArchive & ar );
};

#endif
