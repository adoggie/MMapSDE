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
	unsigned short	m_nRutID;	//路段ID
	BYTE	m_byDirection;		//路段单双行标志
								//(0表示双行，1表示end到start,2表示start到end,3表示双向禁行)
	BYTE	m_byType;			//道路类别:(0--高速，1--都市高速路，2--国道，3--主要地方道&轮渡
									//4--一般县道，5--市镇村道，6--其他道路）
	BYTE	m_byCharge;			//收费:0--不收费1--双向收费2--正向收费3--反向收费
//	BYTE	m_byLaneNum;			//路段车道数:无数据的根据道路类别有缺省值
//	BYTE	m_bySpeed;			//速度级别:根据道路类别有缺省速度
	BYTE	m_byRutLevel;		//道路功能级别:5--一级道路4--二级道路3--三级道路
									//2--四级道路1--五级道路0--六级道路
	BYTE	m_byRutType;		//道路类型:0--环岛1--未采集属性2--上下线分离3--两条高速路之间的道路
									//4--交叉路口内的道路5--连接高速路和普通道路的道路6--停车区
									//7--服务区15--隧道
	float	m_dRLength;			//路段长度
	unsigned short m_nVtxStartID;//路段起始节点ID
	unsigned short m_nVtxEndID;	//路段尾节点ID
	int		m_nRBID;		//路段所在图幅ID
	
	ST_GEO_RLIMIT *m_pRLInfo;	//道路的禁行信息
//	BYTE	m_byLimit;			//是否存在交通限制:0-没有交通限制;非0- 有交通限制
//	unsigned short *m_pLRList;	//禁止通行的道路id
//	BYTE	m_byRLimit;			//是否反向存在交通限制:0-没有交通限制;非0- 有交通限制
//	unsigned short *m_pRLRList;	//反向禁止通行的道路id
//	ST_GEO_RVDRAFT *m_pRVInfo;	//道路虚拟图信息
//	BYTE	m_byVirtual;		//是否存在虚拟图:0- 没有虚拟图;非0- 有虚拟图
//	unsigned short *m_pVORList;	//出去的道路ID数组
//	int		*m_pVIDs;			//虚拟图编号数组
};

#endif // !defined(AFX_GIS_NETWORKRUT_H__59B77223_6C99_4281_8C53_D3654CD62287__INCLUDED_)
