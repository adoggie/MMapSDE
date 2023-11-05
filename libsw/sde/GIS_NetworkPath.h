
#ifndef _GIS_NETWORK_PATH_H
#define _GIS_NETWORK_PATH_H

#include "swBase.h"

class swEXPORT CGIS_NetworkPath  
{
public:
	CGIS_NetworkPath();
	virtual ~CGIS_NetworkPath();

public:
	///取得路径点列表
	CPtrList* GetPathpointList( );
	///初始化导航
	BOOL InitNavigate( );
	///计算机动信息列表
	BOOL CalcuatePathTurnList( int* pErrorCode );

	// 准备导航信息
	BOOL GetNavigateVoice( ST_GEO_PTXY pt,	// 车辆位置
							BYTE byType = 1 );	// 返回的数据类型（目前未使用）
	// 获得导航信息
	BOOL GetNavigateVoiceInfo( BYTE &byMType,			//机动类型（0.常规情况，1.输出的参数是第几个出口，2.距离与出口信息）
								swString &strCRName,		//当前路名
								swString &strNRName,		//下一路名
								double &dbDisNav,		//离机动点距离
								double &dbAngleNav,		//与机动点方向
								double &dbDisDes,		//离终点距离
								double &dbAngleDes,		//与终点方向
								swString &strVLong,		//远距离语音信息
								swString &strVShort,		//近距离语音信息
								swString &strVInter,		// 路口语音
								swString &strRName,		//下个路名
								BYTE &byRMNum );		//环岛路口的个数

	CPtrList* GetNavigateVTRInfo( ST_GEO_RECT &rtBuff );

	///得到语音点在链表中的位置
	void GetVoicePosition( POSITION &posCurr, POSITION &posVoice );
	CPtrList* GetPathTurnList( );
	BOOL IsInMobile( );
	void SetCTFlag( BOOL bCTFlag );
	void GetTLPosition( POSITION &posTurn );

	// 取得当前点到下个节点的距离，如果车辆位于节点上距离为0
	// 使用前确认调用过函数GetNavigateVoice
	double GetDisToNextNode();
	
private:
	//处理TurnList里存在的连续机动
	BOOL CheckTLSeriateMobile( CPtrList *pPPList, CPtrList *pTList );
	// 根据机动信息得到连续机动信息
	swString RecombineTInfo(int nOrd, ST_TURN_INFO *pTInfo, int ntype);
	///计算行使的路段区间
	BOOL CalculateCurrentPosition( ST_GEO_PTXY pt, CPtrList *pPathPointList, POSITION posVoice, POSITION &posCurrent, BOOL &bVoice );
	//计算相关路名（当前、下一）
	BOOL CalculateRutNames( CPtrList *pPathPointList, POSITION posCurrent, swString &strCRName, swString &strNRName );
	///计算下一个需要播放语音的位置
	BOOL CalculateVoicePosition2( CPtrList *pPathPointList, POSITION posCurrent, ST_TURN_INFO **pTInfo );
	BOOL CalculateVoicePosition3( CPtrList *pPPList, CPtrList *pTList, POSITION posCurrent, POSITION &posTurn, POSITION &posVoice );

	BOOL CalculateVTRList( CPtrList *pPPList, ST_TURN_INFO *pTInfo );

	CPtrList *m_pPathpointList;		///路线上点的列表
	ST_VOICE_NODE *m_pVoiceNode;			//当前节点信息
	ST_VOICE_RUT  *m_pVoiceEnterRut;		//节点前段道路信息
	ST_VOICE_RUT  *m_pVoiceExitRut;			//节点后段道路信息

	POSITION m_posCurrent;		///当前在链表中的位置
	POSITION m_posVoice;		///下一个语音在链表中的位置
	POSITION m_posVEnd;			//下一个语音在链表中结束的位置
	POSITION m_posTurn;			///记录机动列表位置
	POSITION m_posVEPre;			//记录前语音结束的位置
	BYTE	m_byMType;			//机动的类型（0.常规类型，1.输出的参数是第几个出口，2.距离与出口信息）
	BYTE	m_byRMNum;			//环岛路口的个数
	BYTE	m_byAFlag;			// 目的地是否在环岛内的标志（1、在，0、不在）

	swString m_strLVoice;	// 长距离提示的语音
	swString m_strSVoice;	// 短距离提示的语音
	swString m_strIVoice;	// 路口提示的语音
	swString m_strInfo;		//需要提示的路名
	BOOL m_bSRut;			//标记是否同一路段
	double m_dbDis;			//记录距离
	CPtrList *m_pTurnList;		//转弯机动信息列表
	BOOL	m_bInMobile;	//表示是否在连续机动或环岛里面
	BYTE	m_byInMType;	//1、表示在连续机动 2、表示在环岛里面 3、表示在环岛

	swString m_strCRName;	//当前路名
	swString m_strNRName;	//下一路名
	double m_dbDisNav;		//离机动点距离
	double m_dbAngleNav;	//与机动点方向
	double m_dbDisDes;		//离终点距离
	double m_dbAngleDes;	//与终点方向
	swString m_strVLong;		// 远距离语音信息
	swString m_strVShort;	// 近距离语音信息
	swString m_strVInter;	// 路口语音信息
	swString m_strRName;		//下个路名

	CPtrList* m_pTRList;		//机动路段的信息
	ST_GEO_RECT m_rtTRBuf;		//查询范围
	BOOL m_bCTFlag;				//是否计算航线机动信息的标志
	double m_dbDis2Dest;		//机动点到终点的距离；

	double m_dbDisToNode; // 当前点到下个航线节点的距离
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
    CPtrList* m_pNodeList;//NODE链表,这里列出了所有NODE,且node中有限行信息.
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
	bool GetbTipReCal(); // 是否因为时间片禁行而重新计算航线

private:
	// 处理时间片禁行
	BOOL TimeSliceLimit(CPtrList *pList, double dbDisToNode, POSITION posCurNode);

	// 计算NODE点列表的当前位置，返回到当前节点的距离，失败返回-1
	double CalCurPosInNodeList(POSITION posPathpoint);
	
	// 格式化语音路名信息
	void FormatVoiceRoadName(ST_TURN_INFO *pInfo, int nOrd, swString &strVoice);

	POSITION m_posLimit[20]; // 记录提示过的位置
	bool m_bTipCal; // 提示重新计算
	double m_dbDisRoute; // 当前航线的长度
	POSITION m_posNodeCur; // 记录NODE点列表的当前位置
};

#endif
